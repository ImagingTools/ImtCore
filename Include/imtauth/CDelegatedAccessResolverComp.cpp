// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtauth/CDelegatedAccessResolverComp.h>


namespace imtauth
{


// reimplemented (imtauth::IDelegatedAccess)

DelegatedTenantList CDelegatedAccessResolverComp::GetAccessibleTenants(
		const QByteArray& userId,
		const QByteArray& homeTenantId) const
{
	DelegatedTenantList result;

	if (!m_membershipManagerCompPtr.IsValid() || !m_crossOrgGrantCompPtr.IsValid()){
		return result;
	}

	// 1. Direct memberships
	const ITenantMembershipManager::MembershipIds membershipIds = m_membershipManagerCompPtr->GetMembershipsByUser(userId);
	QByteArrayList directTenantIds;
	for (const QByteArray& membershipId : membershipIds){
		ITenantMembershipUniquePtr membershipPtr = m_membershipManagerCompPtr->GetMembership(membershipId);
		if (membershipPtr.IsValid()){
			QByteArray tenantId = membershipPtr->GetTenantId();
			if (!directTenantIds.contains(tenantId)){
				directTenantIds.append(tenantId);

				DelegatedTenantInfo info;
				info.tenantId = tenantId;
				info.isDelegated = false;
				result.append(info);
			}
		}
	}

	// 2. Delegated access via cross-org grants targeting the user's home tenant
	const CrossOrgGrants grants = m_crossOrgGrantCompPtr->GetGrantsByTargetTenant(homeTenantId);
	for (const CrossOrgGrantInfo& grant : grants){
		if (!grant.isActive){
			continue;
		}
		// Check expiry via HasAccess with empty roleId (always returns true for active grants)
		// but we need to verify effective status, so check at least one role
		bool isEffective = false;
		if (grant.roleIds.isEmpty()){
			// Grant without roles — still provides visibility
			isEffective = true;
		}
		else{
			// Verify at least one role is accessible (checks expiry internally)
			for (const QByteArray& roleId : grant.roleIds){
				if (m_crossOrgGrantCompPtr->HasAccess(grant.sourceTenantId, homeTenantId, roleId)){
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
			// Already have direct membership — skip delegated entry
			continue;
		}

		// Avoid duplicates from multiple grants to the same source tenant
		bool alreadyAdded = false;
		for (int i = 0; i < result.size(); ++i){
			if (result[i].tenantId == sourceTenantId && result[i].isDelegated){
				// Merge roles from additional grants
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


bool CDelegatedAccessResolverComp::HasDelegatedRole(
		const QByteArray& userId,
		const QByteArray& homeTenantId,
		const QByteArray& targetTenantId,
		const QByteArray& roleId) const
{
	if (!m_membershipManagerCompPtr.IsValid() || !m_crossOrgGrantCompPtr.IsValid()){
		return false;
	}

	// 1. Check direct membership role first
	if (m_membershipManagerCompPtr->IsMember(userId, targetTenantId)){
		if (roleId.isEmpty()){
			return true;
		}
		return m_membershipManagerCompPtr->HasMinimumRole(userId, targetTenantId, roleId);
	}

	// 2. Check delegated access via cross-org grant
	if (roleId.isEmpty()){
		// Any effective grant from target to home provides access
		const CrossOrgGrants grants = m_crossOrgGrantCompPtr->GetGrantsByTargetTenant(homeTenantId);
		for (const CrossOrgGrantInfo& grant : grants){
			if (grant.sourceTenantId == targetTenantId && grant.isActive){
				return true;
			}
		}
		return false;
	}

	return m_crossOrgGrantCompPtr->HasAccess(targetTenantId, homeTenantId, roleId);
}


QByteArrayList CDelegatedAccessResolverComp::GetDelegatedRoles(
		const QByteArray& homeTenantId,
		const QByteArray& targetTenantId) const
{
	QByteArrayList result;

	if (!m_crossOrgGrantCompPtr.IsValid()){
		return result;
	}

	const CrossOrgGrants grants = m_crossOrgGrantCompPtr->GetGrantsByTargetTenant(homeTenantId);
	for (const CrossOrgGrantInfo& grant : grants){
		if (grant.sourceTenantId != targetTenantId){
			continue;
		}
		if (!grant.isActive){
			continue;
		}
		for (const QByteArray& roleId : grant.roleIds){
			if (!roleId.isEmpty() && !result.contains(roleId)){
				// Verify grant is effective (not expired)
				if (m_crossOrgGrantCompPtr->HasAccess(targetTenantId, homeTenantId, roleId)){
					result.append(roleId);
				}
			}
		}
	}

	return result;
}


bool CDelegatedAccessResolverComp::IsDelegatedAccess(
		const QByteArray& userId,
		const QByteArray& homeTenantId,
		const QByteArray& targetTenantId) const
{
	if (!m_membershipManagerCompPtr.IsValid() || !m_crossOrgGrantCompPtr.IsValid()){
		return false;
	}

	// If user is a direct member, it's not delegated access
	if (m_membershipManagerCompPtr->IsMember(userId, targetTenantId)){
		return false;
	}

	// Check if there's an effective grant from target to home
	return m_crossOrgGrantCompPtr->HasAccess(targetTenantId, homeTenantId, QByteArray());
}


} // namespace imtauth
