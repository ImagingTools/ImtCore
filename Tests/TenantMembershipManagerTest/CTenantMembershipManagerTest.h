// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once

// Qt includes
#include <QtCore/QObject>
#include <QtCore/QUuid>
#include <QtCore/QMap>
#include <QtCore/QPair>
#include <QtCore/QDateTime>
#include <QtTest/QtTest>

#include <memory>


namespace imtauth
{


struct MockMembership
{
	QByteArray id;
	QByteArray userId;
	QByteArray tenantId;
	QByteArray roleId;
	bool isActive = true;
	QString joinedAt;
	QString updatedAt;
};


/**
	Mock implementation of ITenantMembershipManager for unit testing.
	Simulates UNIQUE(UserId, TenantId) constraint and role hierarchy.
*/
class CMockMembershipManager
{
public:
	// Role hierarchy: owner > admin > member > viewer
	static int RoleWeight(const QByteArray& roleId)
	{
		if (roleId == "owner") return 100;
		if (roleId == "admin") return 75;
		if (roleId == "member") return 50;
		if (roleId == "viewer") return 25;
		return 0;
	}

	QByteArrayList GetMembershipsByTenant(const QByteArray& tenantId) const
	{
		QByteArrayList result;
		for (auto it = m_memberships.constBegin(); it != m_memberships.constEnd(); ++it){
			if (it.value().tenantId == tenantId){
				result.append(it.key());
			}
		}
		return result;
	}

	QByteArrayList GetMembershipsByUser(const QByteArray& userId) const
	{
		QByteArrayList result;
		for (auto it = m_memberships.constBegin(); it != m_memberships.constEnd(); ++it){
			if (it.value().userId == userId){
				result.append(it.key());
			}
		}
		return result;
	}

	std::shared_ptr<MockMembership> GetMembership(const QByteArray& membershipId) const
	{
		if (m_memberships.contains(membershipId)){
			return std::make_shared<MockMembership>(m_memberships[membershipId]);
		}
		return nullptr;
	}

	std::shared_ptr<MockMembership> FindMembership(const QByteArray& userId, const QByteArray& tenantId) const
	{
		for (auto it = m_memberships.constBegin(); it != m_memberships.constEnd(); ++it){
			if (it.value().userId == userId && it.value().tenantId == tenantId){
				return std::make_shared<MockMembership>(it.value());
			}
		}
		return nullptr;
	}

	QByteArray AddMembership(const QByteArray& userId, const QByteArray& tenantId, const QByteArray& roleId)
	{
		// Validate tenant exists
		if (!m_validTenantIds.contains(tenantId)){
			return QByteArray();
		}

		// Enforce UNIQUE(UserId, TenantId) constraint
		for (auto it = m_memberships.constBegin(); it != m_memberships.constEnd(); ++it){
			if (it.value().userId == userId && it.value().tenantId == tenantId){
				return QByteArray(); // Duplicate
			}
		}

		MockMembership membership;
		membership.id = QUuid::createUuid().toByteArray(QUuid::WithoutBraces);
		membership.userId = userId;
		membership.tenantId = tenantId;
		membership.roleId = roleId;
		membership.isActive = true;
		membership.joinedAt = QDateTime::currentDateTimeUtc().toString(Qt::ISODate);
		membership.updatedAt = membership.joinedAt;

		m_memberships[membership.id] = membership;
		return membership.id;
	}

	bool RemoveMembership(const QByteArray& membershipId)
	{
		if (!m_memberships.contains(membershipId)){
			return false;
		}
		m_memberships.remove(membershipId);
		return true;
	}

	bool UpdateMembershipRole(const QByteArray& membershipId, const QByteArray& newRoleId)
	{
		if (!m_memberships.contains(membershipId)){
			return false;
		}
		m_memberships[membershipId].roleId = newRoleId;
		m_memberships[membershipId].updatedAt = QDateTime::currentDateTimeUtc().toString(Qt::ISODate);
		return true;
	}

	bool IsMember(const QByteArray& userId, const QByteArray& tenantId) const
	{
		for (auto it = m_memberships.constBegin(); it != m_memberships.constEnd(); ++it){
			if (it.value().userId == userId && it.value().tenantId == tenantId && it.value().isActive){
				return true;
			}
		}
		return false;
	}

	bool HasMinimumRole(const QByteArray& userId, const QByteArray& tenantId, const QByteArray& minimumRoleId) const
	{
		// Check if user is owner via tenant data
		if (m_tenantOwners.contains(tenantId) && m_tenantOwners[tenantId] == userId){
			return true; // Owner has all roles
		}

		for (auto it = m_memberships.constBegin(); it != m_memberships.constEnd(); ++it){
			if (it.value().userId == userId && it.value().tenantId == tenantId && it.value().isActive){
				return RoleWeight(it.value().roleId) >= RoleWeight(minimumRoleId);
			}
		}
		return false;
	}

	// Test helpers
	void addValidTenant(const QByteArray& tenantId, const QByteArray& ownerId = QByteArray())
	{
		m_validTenantIds.insert(tenantId);
		if (!ownerId.isEmpty()){
			m_tenantOwners[tenantId] = ownerId;
		}
	}

	QMap<QByteArray, MockMembership> m_memberships;
	QSet<QByteArray> m_validTenantIds;
	QMap<QByteArray, QByteArray> m_tenantOwners; // tenantId -> ownerId
};


} // namespace imtauth


class CTenantMembershipManagerTest: public QObject
{
	Q_OBJECT

private Q_SLOTS:
	void init();
	void cleanup();

	// Add membership tests
	void testAddMembership_Success();
	void testAddMembership_DuplicateUserTenant_Fails();
	void testAddMembership_InvalidTenant_Fails();

	// Get membership tests
	void testGetMembership_Existing();
	void testGetMembership_NonExisting_ReturnsNull();
	void testFindMembership_Existing();
	void testFindMembership_NonExisting_ReturnsNull();
	void testGetMembershipsByTenant_Empty();
	void testGetMembershipsByTenant_Multiple();
	void testGetMembershipsByUser_MultipleTenants();

	// Remove/update tests
	void testRemoveMembership_Existing();
	void testRemoveMembership_NonExisting_ReturnsFalse();
	void testUpdateMembershipRole_Success();
	void testUpdateMembershipRole_NonExisting_ReturnsFalse();

	// Permission checks
	void testIsMember_True();
	void testIsMember_False();
	void testHasMinimumRole_Owner();
	void testHasMinimumRole_Admin();
	void testHasMinimumRole_Member_BelowRequired();
	void testHasMinimumRole_NonMember_ReturnsFalse();

private:
	imtauth::CMockMembershipManager* m_managerPtr = nullptr;
	QByteArray m_tenantId;
	QByteArray m_userId;
};
