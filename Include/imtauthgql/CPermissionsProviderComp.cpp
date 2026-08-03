// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtauthgql/CPermissionsProviderComp.h>


// ImtCore includes
#include <imtauthgql/imtauthgql.h>
#include <GeneratedFiles/imtauthsdl/SDL/1.0/CPP/Permissions.h>


namespace imtauthgql
{


// reimplemented (sdl::V1_0::imtauth::CPermissionsGqlHandlerCompBase)

sdl::V1_0::imtauth::CGetProductPermissionsPayload CPermissionsProviderComp::OnGetProductPermissions(
			const sdl::V1_0::imtauth::CGetProductPermissionsGqlRequest& getProductPermissionsRequest,
			const ::imtgql::CGqlRequest& gqlRequest,
			QString& errorMessage) const
{
	sdl::V1_0::imtauth::CGetProductPermissionsPayload response;

	if (!m_productInfoCompPtr.IsValid()){
		Q_ASSERT_X(false, "Attribute 'FeatureContainer' was not set", "CPermissionsProviderComp");
		return response;
	}

	auto arguments = getProductPermissionsRequest.GetRequestedArguments();
	if (!arguments.input.has_value()){
		Q_ASSERT(false);
		return response;
	}

	if (!arguments.input->productId || arguments.input->productId->isEmpty()){
		errorMessage = QString("Unable to get permissions. Product-ID is empty.");
		SendErrorMessage(0, errorMessage, "CPermissionsProviderComp");
		response.errorMessage = errorMessage;
		return response;
	}

	QSet<QByteArray> allowedPermissions;
	const QSet<QByteArray>* allowedPermissionsPtr = nullptr;

	QByteArray tenantId;
	if (arguments.input->tenantId){
		tenantId = *arguments.input->tenantId;
	}

	if (!tenantId.isEmpty()){
		if (!m_tenantManagerCompPtr.IsValid()){
			errorMessage = QString("Unable to get tenant permissions. Tenant manager is not configured.");
			SendErrorMessage(0, errorMessage, "CPermissionsProviderComp");
			response.errorMessage = errorMessage;
			return response;
		}

		QByteArrayList tenantPermissions = m_tenantManagerCompPtr->GetTenantPermissions(tenantId);
		for (const QByteArray& permissionId : tenantPermissions){
			QByteArray normalizedPermissionId = permissionId.trimmed();
			if (!normalizedPermissionId.isEmpty()){
				allowedPermissions.insert(normalizedPermissionId);
			}
		}

		allowedPermissionsPtr = &allowedPermissions;
	}

	QByteArray languageId;
	const imtgql::IGqlContext* gqlContextPtr = gqlRequest.GetRequestContext();
	if (gqlContextPtr != nullptr){
		languageId = gqlContextPtr->GetLanguageId();
	}

	response.groups.Emplace();
	BuildPermissionGroups<sdl::V1_0::imtauth::CPermissionGroup, sdl::V1_0::imtauth::CPermissionEntry>(
			m_productInfoCompPtr.GetPtr(),
			m_translationManagerCompPtr.IsValid() ? m_translationManagerCompPtr.GetPtr() : nullptr,
			*response.groups,
			languageId,
			allowedPermissionsPtr);

	return response;
}


sdl::V1_0::imtauth::CGetProductPermissionsPayload CPermissionsProviderComp::OnGetUserPermissions(
			const sdl::V1_0::imtauth::CGetUserPermissionsGqlRequest& getUserPermissionsRequest,
			const ::imtgql::CGqlRequest& gqlRequest,
			QString& errorMessage) const
{
	sdl::V1_0::imtauth::CGetProductPermissionsPayload response;

	if (!m_productInfoCompPtr.IsValid()){
		Q_ASSERT_X(false, "Attribute 'FeatureContainer' was not set", "CPermissionsProviderComp");
		return response;
	}

	auto arguments = getUserPermissionsRequest.GetRequestedArguments();
	if (!arguments.input.has_value()){
		Q_ASSERT(false);
		return response;
	}

	if (!arguments.input->productId || arguments.input->productId->isEmpty()){
		errorMessage = QString("Unable to get permissions. Product-ID is empty.");
		SendErrorMessage(0, errorMessage, "CPermissionsProviderComp");
		response.errorMessage = errorMessage;
		return response;
	}

	QByteArray productId = *arguments.input->productId;

	QByteArray languageId;
	const imtauth::IUserInfo* userInfoPtr = nullptr;
	const imtgql::IGqlContext* gqlContextPtr = gqlRequest.GetRequestContext();
	if (gqlContextPtr != nullptr){
		languageId = gqlContextPtr->GetLanguageId();
		userInfoPtr = gqlContextPtr->GetUserInfo();
	}

	if (userInfoPtr == nullptr){
		errorMessage = QString("Unable to get user permissions. No authenticated user.");
		SendErrorMessage(0, errorMessage, "CPermissionsProviderComp");
		response.errorMessage = errorMessage;
		return response;
	}

	// Superuser/administrator sees the full permission tree.
	QSet<QByteArray> allowedPermissions;
	const QSet<QByteArray>* allowedPermissionsPtr = nullptr;
	if (!userInfoPtr->IsAdmin()){
		// Use the tenant-adapted effective permission set (same adaptation as
		// GetProfile): only permissions the user actually possesses in the
		// current tenant context - or true globals for "No Organization" -
		// may be offered, e.g. as personal access token scopes.
		const imtauth::IUserInfo::FeatureIds userPermissions = GetEffectiveUserPermissions(
					gqlContextPtr->GetUserId(),
					*userInfoPtr,
					gqlContextPtr->GetTenantId(),
					productId,
					m_bindingManagerCompPtr.IsValid() ? m_bindingManagerCompPtr.GetPtr() : nullptr,
					m_delegatedAccessCompPtr.IsValid() ? m_delegatedAccessCompPtr.GetPtr() : nullptr,
					m_membershipManagerCompPtr.IsValid() ? m_membershipManagerCompPtr.GetPtr() : nullptr,
					m_roleInfoProviderCompPtr.IsValid() ? m_roleInfoProviderCompPtr.GetPtr() : nullptr);
		for (const QByteArray& permissionId : userPermissions){
			QByteArray normalizedPermissionId = permissionId.trimmed();
			if (!normalizedPermissionId.isEmpty()){
				allowedPermissions.insert(normalizedPermissionId);
			}
		}
		allowedPermissionsPtr = &allowedPermissions;
	}

	response.groups.Emplace();
	BuildPermissionGroups<sdl::V1_0::imtauth::CPermissionGroup, sdl::V1_0::imtauth::CPermissionEntry>(
			m_productInfoCompPtr.GetPtr(),
			m_translationManagerCompPtr.IsValid() ? m_translationManagerCompPtr.GetPtr() : nullptr,
			*response.groups,
			languageId,
			allowedPermissionsPtr);

	return response;
}


} // namespace imtauthgql
