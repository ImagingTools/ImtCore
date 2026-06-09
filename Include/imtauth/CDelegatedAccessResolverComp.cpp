// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtauth/CDelegatedAccessResolverComp.h>


namespace imtauth
{


TenantAccessList CDelegatedAccessResolverComp::GetAccessibleTenants(
		const QByteArray& userId,
		const QByteArray& currentTenantId) const
{
	TenantAccessList result;
	if (!m_membershipManagerCompPtr.IsValid() || !m_crossOrgGrantCompPtr.IsValid()){
		return result;
	}

	QByteArrayList seenTenants;

	const ITenantMembershipManager::MembershipIds membershipIds =
				m_membershipManagerCompPtr->GetMembershipsByUser(userId);
	for (const QByteArray& membershipId : membershipIds){
		ITenantMembershipUniquePtr membershipPtr = m_membershipManagerCompPtr->GetMembership(membershipId);
		if (!membershipPtr.IsValid()){
			continue;
		}
		const QByteArray tenantId = membershipPtr->GetTenantId();
		if (tenantId.isEmpty() || seenTenants.contains(tenantId)){
			continue;
		}

		TenantAccessInfo info;
		info.tenantId = tenantId;
		info.accessType = TAT_MEMBERSHIP;
		result.append(info);
		seenTenants.append(tenantId);
	}

	const QByteArrayList delegatedSources =
				m_crossOrgGrantCompPtr->GetDelegatedSourceTenants(currentTenantId);
	for (const QByteArray& sourceTenantId : delegatedSources){
		if (seenTenants.contains(sourceTenantId)){
			continue;
		}

		TenantAccessInfo info;
		info.tenantId = sourceTenantId;
		info.accessType = TAT_DELEGATED;
		info.delegatedRoleIds = m_crossOrgGrantCompPtr->GetGrantedRoles(sourceTenantId, currentTenantId);
		result.append(info);
		seenTenants.append(sourceTenantId);
	}

	return result;
}


TenantAccessInfo CDelegatedAccessResolverComp::ResolveTenantAccess(
		const QByteArray& userId,
		const QByteArray& currentTenantId,
		const QByteArray& targetTenantId) const
{
	TenantAccessInfo info;
	info.tenantId = targetTenantId;

	if (!m_membershipManagerCompPtr.IsValid() || !m_crossOrgGrantCompPtr.IsValid()){
		return info;
	}

	if (m_membershipManagerCompPtr->IsMember(userId, targetTenantId)){
		info.accessType = TAT_MEMBERSHIP;
		return info;
	}

	if (m_crossOrgGrantCompPtr->IsDelegatedAccess(targetTenantId, currentTenantId)){
		info.accessType = TAT_DELEGATED;
		info.delegatedRoleIds = m_crossOrgGrantCompPtr->GetGrantedRoles(targetTenantId, currentTenantId);
		return info;
	}

	return info;
}


bool CDelegatedAccessResolverComp::IsAccessAllowed(
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


} // namespace imtauth
