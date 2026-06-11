// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ImtCore includes
#include <imtauth/ITenantManager.h>
#include <imtauth/ITenantMembershipManager.h>
#include <imtauth/IDelegatedAccess.h>


namespace imtauth
{


/**
	Check if the user has access to the target tenant, considering both
	direct membership and delegated access via cross-org grants.

	Use this for read/query operations where delegated users should be
	able to access data within the target tenant.

	For mutation/management operations, use direct membership checks only
	(delegated users should not gain management capabilities).

	\param tenantManager Tenant manager for owner checks.
	\param membershipManager Membership manager for direct membership checks.
	\param delegatedAccessPtr Optional delegated access resolver (may be nullptr).
	\param userId The user requesting access.
	\param homeTenantId The user's home tenant (from GQL context).
	\param targetTenantId The tenant the user wants to access.
	\return true if the user has access via ownership, direct membership, or delegation.
*/
inline bool HasDelegatedTenantAccess(
		const ITenantManager& tenantManager,
		const ITenantMembershipManager& membershipManager,
		const IDelegatedAccess* delegatedAccessPtr,
		const QByteArray& userId,
		const QByteArray& homeTenantId,
		const QByteArray& targetTenantId)
{
	// 1. Owner bypasses all permission checks
	ITenantInfoUniquePtr tenantPtr = tenantManager.GetTenant(targetTenantId);
	if (tenantPtr.IsValid() && tenantPtr->GetOwnerId() == userId){
		return true;
	}

	// 2. Direct membership
	if (membershipManager.HasMinimumRole(userId, targetTenantId, QByteArray())){
		return true;
	}

	// 3. Delegated access via cross-org grant
	if (delegatedAccessPtr != nullptr){
		return delegatedAccessPtr->HasDelegatedRole(userId, homeTenantId, targetTenantId, QByteArray());
	}

	return false;
}


/**
	Check if the user has a specific delegated role in the target tenant.

	Checks both direct membership roles and delegated roles from cross-org grants.
	Use this when a specific role (e.g., admin) is required for an operation.

	\param tenantManager Tenant manager for owner checks.
	\param membershipManager Membership manager for direct membership checks.
	\param delegatedAccessPtr Optional delegated access resolver (may be nullptr).
	\param userId The user requesting access.
	\param homeTenantId The user's home tenant (from GQL context).
	\param targetTenantId The tenant the user wants to access.
	\param roleId The role required for the operation (empty = any role).
	\return true if the user has the specified role via membership or delegation.
*/
inline bool HasDelegatedTenantRole(
		const ITenantManager& tenantManager,
		const ITenantMembershipManager& membershipManager,
		const IDelegatedAccess* delegatedAccessPtr,
		const QByteArray& userId,
		const QByteArray& homeTenantId,
		const QByteArray& targetTenantId,
		const QByteArray& roleId)
{
	// 1. Owner bypasses all permission checks
	ITenantInfoUniquePtr tenantPtr = tenantManager.GetTenant(targetTenantId);
	if (tenantPtr.IsValid() && tenantPtr->GetOwnerId() == userId){
		return true;
	}

	// 2. Direct membership with required role
	if (membershipManager.HasMinimumRole(userId, targetTenantId, roleId)){
		return true;
	}

	// 3. Delegated role via cross-org grant
	if (delegatedAccessPtr != nullptr){
		return delegatedAccessPtr->HasDelegatedRole(userId, homeTenantId, targetTenantId, roleId);
	}

	return false;
}


} // namespace imtauth
