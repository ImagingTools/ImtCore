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
							sdl::imtauth::Profile::CGraphQlHandlerCompBase>
{
public:
	typedef imtclientgql::TClientRequestManagerCompWrap<sdl::imtauth::Profile::CGraphQlHandlerCompBase> BaseClass;

	I_BEGIN_COMPONENT(CRemoteProfileControllerComp);
		I_ASSIGN(m_productInfoCompPtr, "ProductInfo", "Product feature container", false, "ProductInfo");
	I_END_COMPONENT;

protected:
	// reimplemented (sdl::imtauth::Profile::CGraphQlHandlerCompBase)
	virtual sdl::imtauth::Profile::CProfileData OnGetProfile(
				const sdl::imtauth::Profile::CGetProfileGqlRequest& getProfileRequest,
				const ::imtgql::CGqlRequest& gqlRequest,
				QString& errorMessage) const override;
	virtual sdl::imtauth::Profile::CSetProfileResponse OnSetProfile(
				const sdl::imtauth::Profile::CSetProfileGqlRequest& setProfileRequest,
				const ::imtgql::CGqlRequest& gqlRequest,
				QString& errorMessage) const override;
	virtual sdl::imtauth::Profile::CGetUserOrganizationsPayload OnGetUserOrganizations(
				const sdl::imtauth::Profile::CGetUserOrganizationsGqlRequest& getUserOrganizationsRequest,
				const ::imtgql::CGqlRequest& gqlRequest, QString& errorMessage) const override;

private:
	I_REF(imtlic::IProductInfo, m_productInfoCompPtr);
};


} // namespace imtauthgql
