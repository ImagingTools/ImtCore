// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ImtCore includes
#include <imtlic/IProductInfo.h>
#include <imtclientgql/TClientRequestManagerCompWrap.h>
#include <GeneratedFiles/imtauthsdl/SDL/1.0/CPP/Profile.h>


namespace imtauthgql
{


class CRemoteProfileControllerComp:
			public imtclientgql::TClientRequestManagerCompWrap<
							sdl::V1_0::imtauth::CProfileGqlHandlerCompBase>
{
public:
	typedef imtclientgql::TClientRequestManagerCompWrap<sdl::V1_0::imtauth::CProfileGqlHandlerCompBase> BaseClass;

	I_BEGIN_COMPONENT(CRemoteProfileControllerComp);
		I_ASSIGN(m_productInfoCompPtr, "ProductInfo", "Product feature container", false, "ProductInfo");
	I_END_COMPONENT;

protected:
	// reimplemented (sdl::V1_0::imtauth::CProfileGqlHandlerCompBase)
	virtual sdl::V1_0::imtauth::CProfileData OnGetProfile(
				const sdl::V1_0::imtauth::CGetProfileGqlRequest& getProfileRequest,
				const ::imtgql::CGqlRequest& gqlRequest,
				QString& errorMessage) const override;
	virtual sdl::V1_0::imtauth::CSetProfileResponse OnSetProfile(
				const sdl::V1_0::imtauth::CSetProfileGqlRequest& setProfileRequest,
				const ::imtgql::CGqlRequest& gqlRequest,
				QString& errorMessage) const override;
	virtual sdl::V1_0::imtauth::CGetUserOrganizationsPayload OnGetUserOrganizations(
				const sdl::V1_0::imtauth::CGetUserOrganizationsGqlRequest& getUserOrganizationsRequest,
				const ::imtgql::CGqlRequest& gqlRequest, QString& errorMessage) const override;

private:
	I_REF(imtlic::IProductInfo, m_productInfoCompPtr);
};


} // namespace imtauthgql
