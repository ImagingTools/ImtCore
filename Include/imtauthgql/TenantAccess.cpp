// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtauthgql/TenantAccess.h>


// ImtCore includes
#include <imtauth/IUserInfo.h>
#include <imtauth/ITenantMembership.h>
#include <imtgql/IGqlContext.h>


namespace imtauthgql
{


bool HasTenantIsolationAccess(
			const imtauth::ITenantManager& tenantManager,
			const imtauth::ITenantMembershipManager* membershipManagerPtr,
			const imtgql::CGqlRequest& gqlRequest,
			const QByteArray& tenantId,
			QString& errorMessage)
{
	const imtgql::IGqlContext* contextPtr = gqlRequest.GetRequestContext();
	if (contextPtr == nullptr){
		errorMessage = QStringLiteral("Access denied: request context is missing");
		return false;
	}

	const imtauth::IUserInfo* userInfoPtr = contextPtr->GetUserInfo();
	if (userInfoPtr != nullptr){
		if (userInfoPtr->IsAdmin()){
			return true;
		}
	}

	const QByteArray contextTenantId = contextPtr->GetTenantId();
	if (!contextTenantId.isEmpty()){
		if (contextTenantId == tenantId){
			return true;
		}

		errorMessage = QStringLiteral("Access denied: requested tenant does not match current tenant context");
		return false;
	}

	const QByteArray userId = contextPtr->GetUserId();
	if (userId.isEmpty()){
		errorMessage = QStringLiteral("Access denied: unauthenticated request context");
		return false;
	}

	imtauth::ITenantInfoUniquePtr tenantPtr = tenantManager.GetTenant(tenantId);
	if (tenantPtr.IsValid() && tenantPtr->GetOwnerId() == userId){
		return true;
	}

	if (membershipManagerPtr != nullptr && membershipManagerPtr->IsMember(userId, tenantId)){
		return true;
	}

	errorMessage = QStringLiteral("Access denied: tenant is outside current user scope");
	return false;
}


bool HasTenantManageAccess(
			const imtauth::ITenantManager& tenantManager,
			const imtauth::ITenantMembershipManager* membershipManagerPtr,
			const imtgql::CGqlRequest& gqlRequest,
			const QByteArray& tenantId,
			const QByteArray& requiredOrgPermission,
			QString& errorMessage)
{
	const imtgql::IGqlContext* contextPtr = gqlRequest.GetRequestContext();
	if (contextPtr == nullptr){
		errorMessage = QStringLiteral("Access denied: request context is missing");
		return false;
	}

	const imtauth::IUserInfo* userInfoPtr = contextPtr->GetUserInfo();
	if (userInfoPtr != nullptr && userInfoPtr->IsAdmin()){
		return true;
	}

	const QByteArray contextTenantId = contextPtr->GetTenantId();
	if (!contextTenantId.isEmpty() && contextTenantId != tenantId){
		errorMessage = QStringLiteral("Access denied: requested tenant does not match current tenant context");
		return false;
	}

	const QByteArray userId = contextPtr->GetUserId();
	if (userId.isEmpty()){
		errorMessage = QStringLiteral("Access denied: unauthenticated request context");
		return false;
	}

	imtauth::ITenantInfoUniquePtr tenantPtr = tenantManager.GetTenant(tenantId);
	if (!tenantPtr.IsValid()){
		errorMessage = QStringLiteral("Tenant not found");
		return false;
	}

	if (tenantPtr->GetOwnerId() == userId || tenantPtr->GetCreatorId() == userId){
		return true;
	}

	if (!requiredOrgPermission.isEmpty() && membershipManagerPtr != nullptr){
		imtauth::ITenantMembershipUniquePtr msPtr = membershipManagerPtr->FindMembership(userId, tenantId);
		if (msPtr.IsValid() && msPtr->IsActive() && msPtr->GetOrganizationPermissions().contains(requiredOrgPermission)){
			return true;
		}
	}

	errorMessage = QStringLiteral("Access denied: insufficient permissions to manage tenant");
	return false;
}


} // namespace imtauthgql


