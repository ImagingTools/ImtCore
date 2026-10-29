// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtauthgql/CRemoteProfileControllerComp.h>
#include <GeneratedFiles/imtauthsdl/SDL/1.0/CPP/Profile.h>


// ImtCore includes
#include <imtauthgql/imtauthgql.h>


namespace imtauthgql
{


// reimplemented (sdl::V1_0::imtauth::CProfileGqlHandlerCompBase)

sdl::V1_0::imtauth::CProfileData CRemoteProfileControllerComp::OnGetProfile(
			const sdl::V1_0::imtauth::CGetProfileGqlRequest& /*getProfileRequest*/,
			const ::imtgql::CGqlRequest& gqlRequest,
			QString& errorMessage) const
{
	sdl::V1_0::imtauth::CProfileData profileData =
				SendModelRequest<sdl::V1_0::imtauth::CProfileData>(gqlRequest, errorMessage);

	// The auth server does not know the features of this product, so their requirements are resolved here.
	const imtgql::IGqlContext* gqlContextPtr = gqlRequest.GetRequestContext();
	imtlic::IProductInfo* productInfoPtr = m_productInfoCompPtr.IsValid() ? m_productInfoCompPtr.GetPtr() : nullptr;
	if (productInfoPtr == nullptr && gqlContextPtr != nullptr){
		productInfoPtr = gqlContextPtr->GetProductInfo();
	}

	if (productInfoPtr != nullptr && profileData.permissions.HasValue()){
		AddRequiredPermissions(
					*profileData.permissions,
					*productInfoPtr,
					m_translationManagerCompPtr.IsValid() ? m_translationManagerCompPtr.GetPtr() : nullptr,
					(gqlContextPtr != nullptr) ? gqlContextPtr->GetLanguageId() : QByteArray(),
					(gqlContextPtr != nullptr) ? gqlContextPtr->GetTenantPermissions() : QByteArrayList());
	}

	return profileData;
}


sdl::V1_0::imtauth::CSetProfileResponse CRemoteProfileControllerComp::OnSetProfile(
			const sdl::V1_0::imtauth::CSetProfileGqlRequest& /*setProfileRequest*/,
			const ::imtgql::CGqlRequest& gqlRequest,
			QString& errorMessage) const
{
	return SendModelRequest<sdl::V1_0::imtauth::CSetProfileResponse>(gqlRequest, errorMessage);
}


sdl::V1_0::imtauth::CGetUserOrganizationsPayload CRemoteProfileControllerComp::OnGetUserOrganizations(
			const sdl::V1_0::imtauth::CGetUserOrganizationsGqlRequest& /*getUserOrganizationsRequest*/,
			const ::imtgql::CGqlRequest& gqlRequest, QString& errorMessage) const
{
	return SendModelRequest<sdl::V1_0::imtauth::CGetUserOrganizationsPayload>(gqlRequest, errorMessage);
}


} // namespace imtauthgql
