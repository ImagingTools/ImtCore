// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once

// Qt includes
#include <QtCore/QObject>
#include <QtTest/QtTest>

// ImtCore includes
#include <imtauth/IDelegatedAccess.h>
#include <imtauth/ICrossOrgGrant.h>
#include <imtauth/ITenantMembershipManager.h>
#include <imtauth/ITenantMembership.h>


namespace imtauth
{


/**
	Minimal mock for ITenantMembership used in delegated access tests.
*/
class CMockTenantMembership
{
public:
	QByteArray membershipId;
	QByteArray userId;
	QByteArray tenantId;
	QByteArray roleId;
};


/**
	Mock membership manager for testing delegated access resolution.
*/
class CMockMembershipManager
{
public:
	struct MembershipEntry
	{
		QByteArray membershipId;
		QByteArray userId;
		QByteArray tenantId;
		QByteArray roleId;
	};

	QByteArray AddMembership(const QByteArray& userId, const QByteArray& tenantId, const QByteArray& roleId)
	{
		MembershipEntry entry;
		entry.membershipId = QByteArray::number(++m_counter);
		entry.userId = userId;
		entry.tenantId = tenantId;
		entry.roleId = roleId;
		m_memberships.append(entry);
		return entry.membershipId;
	}

	QByteArrayList GetMembershipsByUser(const QByteArray& userId) const
	{
		QByteArrayList result;
		for (const MembershipEntry& entry : m_memberships){
			if (entry.userId == userId){
				result.append(entry.membershipId);
			}
		}
		return result;
	}

	bool IsMember(const QByteArray& userId, const QByteArray& tenantId) const
	{
		for (const MembershipEntry& entry : m_memberships){
			if (entry.userId == userId && entry.tenantId == tenantId){
				return true;
			}
		}
		return false;
	}

	bool HasMinimumRole(const QByteArray& userId, const QByteArray& tenantId, const QByteArray& minimumRoleId) const
	{
		for (const MembershipEntry& entry : m_memberships){
			if (entry.userId == userId && entry.tenantId == tenantId){
				if (minimumRoleId.isEmpty()){
					return true;
				}
				return entry.roleId == minimumRoleId;
			}
		}
		return false;
	}

	QByteArray GetTenantId(const QByteArray& membershipId) const
	{
		for (const MembershipEntry& entry : m_memberships){
			if (entry.membershipId == membershipId){
				return entry.tenantId;
			}
		}
		return QByteArray();
	}

	QList<MembershipEntry> m_memberships;
	int m_counter = 0;
};


/**
	Mock cross-org grant manager (same as CCrossOrgGrantTest mock).
*/
class CMockCrossOrgGrantForDelegated
{
public:
	QByteArray CreateGrant(
				const QByteArray& sourceTenantId,
				const QByteArray& targetTenantId,
				const QByteArrayList& roleIds,
				const QString& description = QString(),
				const QString& expiresAt = QString())
	{
		if (sourceTenantId.isEmpty() || targetTenantId.isEmpty()){
			return QByteArray();
		}
		if (sourceTenantId == targetTenantId){
			return QByteArray();
		}

		CrossOrgGrantInfo info;
		info.grantId = QByteArray::number(++m_counter);
		info.sourceTenantId = sourceTenantId;
		info.targetTenantId = targetTenantId;
		info.roleIds = roleIds;
		info.description = description;
		info.createdAt = QDateTime::currentDateTimeUtc().toString(Qt::ISODateWithMs);
		info.expiresAt = expiresAt;
		info.isActive = true;

		m_grants.append(info);
		return info.grantId;
	}

	bool RevokeGrant(const QByteArray& grantId)
	{
		for (CrossOrgGrantInfo& info : m_grants){
			if (info.grantId == grantId && info.isActive){
				info.isActive = false;
				return true;
			}
		}
		return false;
	}

	CrossOrgGrants GetGrantsByTargetTenant(const QByteArray& targetTenantId) const
	{
		CrossOrgGrants result;
		for (const CrossOrgGrantInfo& info : m_grants){
			if (info.targetTenantId == targetTenantId){
				result.append(info);
			}
		}
		return result;
	}

	bool HasAccess(
				const QByteArray& sourceTenantId,
				const QByteArray& targetTenantId,
				const QByteArray& roleId) const
	{
		if (roleId.isEmpty()){
			return true;
		}

		const CrossOrgGrants grants = GetGrantsByTargetTenant(targetTenantId);
		for (const CrossOrgGrantInfo& info : grants){
			if (info.sourceTenantId != sourceTenantId){
				continue;
			}
			if (!info.isActive){
				continue;
			}
			if (!info.expiresAt.isEmpty()){
				QDateTime expiresAt = QDateTime::fromString(info.expiresAt, Qt::ISODateWithMs);
				if (expiresAt.isValid() && expiresAt < QDateTime::currentDateTimeUtc()){
					continue;
				}
			}
			if (info.roleIds.contains(roleId)){
				return true;
			}
		}
		return false;
	}

	QList<CrossOrgGrantInfo> m_grants;
	int m_counter = 0;
};


/**
	Standalone delegated access resolver for unit tests.
	Mirrors CDelegatedAccessResolverComp logic using the mock managers.
*/
class CMockDelegatedAccessResolver
{
public:
	CMockDelegatedAccessResolver(CMockMembershipManager* membershipPtr, CMockCrossOrgGrantForDelegated* grantPtr)
		: m_membershipPtr(membershipPtr)
		, m_grantPtr(grantPtr)
	{
	}

	DelegatedTenantList GetAccessibleTenants(const QByteArray& userId, const QByteArray& homeTenantId) const
	{
		DelegatedTenantList result;

		// Direct memberships
		QByteArrayList directTenantIds;
		const QByteArrayList membershipIds = m_membershipPtr->GetMembershipsByUser(userId);
		for (const QByteArray& membershipId : membershipIds){
			QByteArray tenantId = m_membershipPtr->GetTenantId(membershipId);
			if (!tenantId.isEmpty() && !directTenantIds.contains(tenantId)){
				directTenantIds.append(tenantId);
				DelegatedTenantInfo info;
				info.tenantId = tenantId;
				info.isDelegated = false;
				result.append(info);
			}
		}

		// Delegated via grants
		const CrossOrgGrants grants = m_grantPtr->GetGrantsByTargetTenant(homeTenantId);
		for (const CrossOrgGrantInfo& grant : grants){
			if (!grant.isActive){
				continue;
			}
			bool isEffective = false;
			if (grant.roleIds.isEmpty()){
				isEffective = true;
			}
			else{
				for (const QByteArray& roleId : grant.roleIds){
					if (m_grantPtr->HasAccess(grant.sourceTenantId, homeTenantId, roleId)){
						isEffective = true;
						break;
					}
				}
			}
			if (!isEffective){
				continue;
			}

			QByteArray sourceTenantId = grant.sourceTenantId;
			if (directTenantIds.contains(sourceTenantId)){
				continue;
			}

			bool alreadyAdded = false;
			for (int i = 0; i < result.size(); ++i){
				if (result[i].tenantId == sourceTenantId && result[i].isDelegated){
					for (const QByteArray& roleId : grant.roleIds){
						if (!result[i].roleIds.contains(roleId)){
							result[i].roleIds.append(roleId);
						}
					}
					alreadyAdded = true;
					break;
				}
			}

			if (!alreadyAdded){
				DelegatedTenantInfo info;
				info.tenantId = sourceTenantId;
				info.grantId = grant.grantId;
				info.roleIds = grant.roleIds;
				info.isDelegated = true;
				result.append(info);
			}
		}

		return result;
	}

	bool HasDelegatedRole(const QByteArray& userId, const QByteArray& homeTenantId,
						  const QByteArray& targetTenantId, const QByteArray& roleId) const
	{
		if (m_membershipPtr->IsMember(userId, targetTenantId)){
			if (roleId.isEmpty()){
				return true;
			}
			return m_membershipPtr->HasMinimumRole(userId, targetTenantId, roleId);
		}

		if (roleId.isEmpty()){
			const CrossOrgGrants grants = m_grantPtr->GetGrantsByTargetTenant(homeTenantId);
			for (const CrossOrgGrantInfo& grant : grants){
				if (grant.sourceTenantId == targetTenantId && grant.isActive){
					return true;
				}
			}
			return false;
		}

		return m_grantPtr->HasAccess(targetTenantId, homeTenantId, roleId);
	}

	QByteArrayList GetDelegatedRoles(const QByteArray& homeTenantId, const QByteArray& targetTenantId) const
	{
		QByteArrayList result;
		const CrossOrgGrants grants = m_grantPtr->GetGrantsByTargetTenant(homeTenantId);
		for (const CrossOrgGrantInfo& grant : grants){
			if (grant.sourceTenantId != targetTenantId || !grant.isActive){
				continue;
			}
			for (const QByteArray& roleId : grant.roleIds){
				if (!roleId.isEmpty() && !result.contains(roleId)){
					if (m_grantPtr->HasAccess(targetTenantId, homeTenantId, roleId)){
						result.append(roleId);
					}
				}
			}
		}
		return result;
	}

	bool IsDelegatedAccess(const QByteArray& userId, const QByteArray& homeTenantId,
						   const QByteArray& targetTenantId) const
	{
		if (m_membershipPtr->IsMember(userId, targetTenantId)){
			return false;
		}
		return m_grantPtr->HasAccess(targetTenantId, homeTenantId, QByteArray());
	}

private:
	CMockMembershipManager* m_membershipPtr;
	CMockCrossOrgGrantForDelegated* m_grantPtr;
};


} // namespace imtauth


class CDelegatedAccessTest: public QObject
{
	Q_OBJECT

private Q_SLOTS:
	void init();
	void cleanup();

	// Org visibility
	void testGetAccessibleTenants_DirectMembershipOnly();
	void testGetAccessibleTenants_DelegatedOnly();
	void testGetAccessibleTenants_MixedMembershipAndDelegated();
	void testGetAccessibleTenants_DirectTakesPrecedence();
	void testGetAccessibleTenants_InactiveGrantExcluded();
	void testGetAccessibleTenants_ExpiredGrantExcluded();
	void testGetAccessibleTenants_MultipleGrantsMergeRoles();

	// Permission check
	void testHasDelegatedRole_DirectMembership();
	void testHasDelegatedRole_DelegatedGrant();
	void testHasDelegatedRole_NoAccess();
	void testHasDelegatedRole_EmptyRoleMeansAny();
	void testHasDelegatedRole_WrongRole();
	void testHasDelegatedRole_RevokedGrant();

	// Delegated roles
	void testGetDelegatedRoles_ReturnsGrantedRoles();
	void testGetDelegatedRoles_ExcludesExpired();
	void testGetDelegatedRoles_NoneForDirectMember();

	// Context detection
	void testIsDelegatedAccess_TrueForGrantOnly();
	void testIsDelegatedAccess_FalseForDirectMember();
	void testIsDelegatedAccess_FalseForNoAccess();

private:
	imtauth::CMockMembershipManager* m_membershipPtr = nullptr;
	imtauth::CMockCrossOrgGrantForDelegated* m_grantPtr = nullptr;
	imtauth::CMockDelegatedAccessResolver* m_resolverPtr = nullptr;
};
