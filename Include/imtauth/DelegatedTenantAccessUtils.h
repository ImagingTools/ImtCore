// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ImtCore includes
#include <imtauth/ITenantManager.h>
#include <imtauth/ITenantMembershipManager.h>
#include <imtauth/IDelegatedAccess.h>


namespace imtauth
{


/**
	Check if the user has access to the target tenant: its owner, its creator, a member, or a user its contracts give permissions.

	Use this for read/query operations where delegated users should be
	able to access data within the target tenant.

	For mutation/management operations, use direct membership checks only
	(delegated users should not gain management capabilities).

	\param delegatedAccessPtr Optional delegated access resolver (may be nullptr).
*/
inline bool HasDelegatedTenantAccess(
		const ITenantManager& tenantManager,
		const ITenantMembershipManager& membershipManager,
		const IDelegatedAccess* delegatedAccessPtr,
		const QByteArray& userId,
		const QByteArray& targetTenantId)
{
	ITenantInfoUniquePtr tenantPtr = tenantManager.GetTenant(targetTenantId);
	if (tenantPtr.IsValid() && (tenantPtr->GetOwnerId() == userId || tenantPtr->GetCreatorId() == userId)){
		return true;
	}

	if (membershipManager.IsMember(userId, targetTenantId)){
		return true;
	}

	return delegatedAccessPtr != nullptr && delegatedAccessPtr->HasDelegatedAccess(userId, targetTenantId);
}


} // namespace imtauth


