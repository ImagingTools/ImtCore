// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once

// Qt includes
#include <QtCore/QObject>
#include <QtCore/QDateTime>
#include <QtCore/QList>
#include <QtCore/QPair>
#include <QtTest/QtTest>

// ImtCore includes
#include <imtauth/ICrossOrgGrant.h>
#include <imtauth/IDelegatedAccess.h>


namespace imtauth
{


/**
	Self-contained mock replicating the access-resolution semantics of
	CCrossOrgGrantManagerComp, used to verify the expected behaviour of
	cross-org grants without bringing up the component framework.
*/
class CMockCrossOrgGrantManager
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

	static bool IsGrantEffective(const CrossOrgGrantInfo& info)
	{
		if (!info.isActive){
			return false;
		}
		if (!info.expiresAt.isEmpty()){
			QDateTime expiresAt = QDateTime::fromString(info.expiresAt, Qt::ISODateWithMs);
			if (expiresAt.isValid() && expiresAt < QDateTime::currentDateTimeUtc()){
				return false;
			}
		}
		return true;
	}

	bool HasAccess(
				const QByteArray& sourceTenantId,
				const QByteArray& targetTenantId,
				const QByteArray& roleId) const
	{
		if (roleId.isEmpty()){
			return true;
		}

		for (const CrossOrgGrantInfo& info : GetGrantsByTargetTenant(targetTenantId)){
			if (info.sourceTenantId != sourceTenantId){
				continue;
			}
			if (!IsGrantEffective(info)){
				continue;
			}
			if (info.roleIds.contains(roleId)){
				return true;
			}
		}

		return false;
	}

	QByteArrayList GetDelegatedSourceTenants(const QByteArray& targetTenantId) const
	{
		QByteArrayList result;
		for (const CrossOrgGrantInfo& info : GetGrantsByTargetTenant(targetTenantId)){
			if (!IsGrantEffective(info)){
				continue;
			}
			if (!result.contains(info.sourceTenantId)){
				result.append(info.sourceTenantId);
			}
		}
		return result;
	}

	QByteArrayList GetGrantedRoles(
				const QByteArray& sourceTenantId,
				const QByteArray& targetTenantId) const
	{
		QByteArrayList result;
		for (const CrossOrgGrantInfo& info : GetGrantsByTargetTenant(targetTenantId)){
			if (info.sourceTenantId != sourceTenantId){
				continue;
			}
			if (!IsGrantEffective(info)){
				continue;
			}
			for (const QByteArray& roleId : info.roleIds){
				if (!result.contains(roleId)){
					result.append(roleId);
				}
			}
		}
		return result;
	}

	bool IsDelegatedAccess(
				const QByteArray& sourceTenantId,
				const QByteArray& targetTenantId) const
	{
		for (const CrossOrgGrantInfo& info : GetGrantsByTargetTenant(targetTenantId)){
			if (info.sourceTenantId != sourceTenantId){
				continue;
			}
			if (IsGrantEffective(info)){
				return true;
			}
		}
		return false;
	}

	QList<CrossOrgGrantInfo> m_grants;
	int m_counter = 0;
};


/**
	Self-contained mock replicating CDelegatedAccessResolverComp semantics:
	composes direct memberships (a simple user->tenant list) with the cross-org
	grant mock to resolve organization visibility and delegated permission
	checks without bringing up the component framework.
*/
class CMockDelegatedAccessResolver
{
public:
	void AddMembership(const QByteArray& userId, const QByteArray& tenantId)
	{
		m_memberships.append(qMakePair(userId, tenantId));
	}

	bool IsMember(const QByteArray& userId, const QByteArray& tenantId) const
	{
		for (const auto& membership : m_memberships){
			if (membership.first == userId && membership.second == tenantId){
				return true;
			}
		}
		return false;
	}

	TenantAccessList GetAccessibleTenants(const QByteArray& userId, const QByteArray& currentTenantId) const
	{
		TenantAccessList result;
		QByteArrayList seen;

		for (const auto& membership : m_memberships){
			if (membership.first != userId){
				continue;
			}
			if (membership.second.isEmpty() || seen.contains(membership.second)){
				continue;
			}
			TenantAccessInfo info;
			info.tenantId = membership.second;
			info.accessType = TAT_MEMBERSHIP;
			result.append(info);
			seen.append(membership.second);
		}

		for (const QByteArray& source : m_grantManager.GetDelegatedSourceTenants(currentTenantId)){
			if (seen.contains(source)){
				continue;
			}
			TenantAccessInfo info;
			info.tenantId = source;
			info.accessType = TAT_DELEGATED;
			info.delegatedRoleIds = m_grantManager.GetGrantedRoles(source, currentTenantId);
			result.append(info);
			seen.append(source);
		}

		return result;
	}

	TenantAccessInfo ResolveTenantAccess(
				const QByteArray& userId,
				const QByteArray& currentTenantId,
				const QByteArray& targetTenantId) const
	{
		TenantAccessInfo info;
		info.tenantId = targetTenantId;

		if (IsMember(userId, targetTenantId)){
			info.accessType = TAT_MEMBERSHIP;
			return info;
		}
		if (m_grantManager.IsDelegatedAccess(targetTenantId, currentTenantId)){
			info.accessType = TAT_DELEGATED;
			info.delegatedRoleIds = m_grantManager.GetGrantedRoles(targetTenantId, currentTenantId);
			return info;
		}
		return info;
	}

	bool IsAccessAllowed(
				const QByteArray& userId,
				const QByteArray& currentTenantId,
				const QByteArray& targetTenantId,
				const QByteArray& requiredRoleId) const
	{
		const TenantAccessInfo info = ResolveTenantAccess(userId, currentTenantId, targetTenantId);
		switch (info.accessType){
			case TAT_MEMBERSHIP:
				return true;
			case TAT_DELEGATED:
				return requiredRoleId.isEmpty() || info.delegatedRoleIds.contains(requiredRoleId);
			case TAT_NONE:
			default:
				return false;
		}
	}

	CMockCrossOrgGrantManager m_grantManager;
	QList<QPair<QByteArray, QByteArray>> m_memberships;
};


} // namespace imtauth


class CCrossOrgGrantTest: public QObject
{
	Q_OBJECT

private Q_SLOTS:
	void init();
	void cleanup();

	// Data object tests
	void testGrantInfo_RoundTrip();
	void testGrantInfo_CopyFromAndClone();
	void testGrantInfo_ResetDataDefaults();

	// Manager semantics
	void testCreateGrant_Success();
	void testCreateGrant_MissingFields_Fails();
	void testCreateGrant_SameTenant_Fails();
	void testRevokeGrant_DeactivatesGrant();
	void testRevokeGrant_Unknown_ReturnsFalse();

	// Access resolution
	void testHasAccess_NoneAlwaysAllowed();
	void testHasAccess_GrantMatrix();
	void testHasAccess_RevokedDenied();
	void testHasAccess_ExpiredDenied();
	void testHasAccess_ScopedGrant();
	void testHasAccess_WrongSourceDenied();

	// Delegated-access resolution
	void testDelegatedSourceTenants_ListsEffectiveGrantsOnce();
	void testDelegatedSourceTenants_ExcludesRevokedAndExpired();
	void testGrantedRoles_UnionAcrossGrants();
	void testGrantedRoles_WrongSourceEmpty();
	void testIsDelegatedAccess_ReflectsEffectiveGrant();

	// Delegated-access resolver (membership + grant composition)
	void testResolver_AccessibleTenantsCombineMembershipAndDelegated();
	void testResolver_MembershipTakesPrecedenceOverDelegated();
	void testResolver_ResolveTenantAccessTypes();
	void testResolver_IsAccessAllowedDelegatedScope();
	void testResolver_IsAccessAllowedMembershipAndNone();

private:
	imtauth::CMockCrossOrgGrantManager* m_managerPtr = nullptr;
	imtauth::CMockDelegatedAccessResolver* m_resolverPtr = nullptr;
};
