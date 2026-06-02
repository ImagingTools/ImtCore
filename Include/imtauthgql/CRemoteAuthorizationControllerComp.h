// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ImtCore includes
#include <imtclientgql/TClientRequestManagerCompWrap.h>
#include <GeneratedFiles/imtauthsdl/SDL/1.0/CPP/Authorization.h>


namespace imtauthgql
{


class CRemoteAuthorizationControllerComp:
		public imtclientgql::TClientRequestManagerCompWrap<
					sdl::V1_0::imtauth::CAuthorizationGqlHandlerCompBase>
{
public:
	typedef imtclientgql::TClientRequestManagerCompWrap<sdl::V1_0::imtauth::CAuthorizationGqlHandlerCompBase> BaseClass;

	I_BEGIN_COMPONENT(CRemoteAuthorizationControllerComp);
	I_END_COMPONENT;

protected:
	// reimplemented (sdl::V1_0::imtauth::CAuthorizationGqlHandlerCompBase)
	virtual sdl::V1_0::imtauth::CAuthorizationPayload OnAuthorization(
				const sdl::V1_0::imtauth::CAuthorizationGqlRequest& authorizationRequest,
				const imtgql::CGqlRequest& gqlRequest,
				QString& errorMessage) const override;
	virtual sdl::V1_0::imtauth::CAuthorizationPayload OnUserToken(
				const sdl::V1_0::imtauth::CUserTokenGqlRequest& userTokenRequest,
				const ::imtgql::CGqlRequest& gqlRequest,
				QString& errorMessage) const override;
	virtual sdl::V1_0::imtauth::CLogoutPayload OnLogout(
				const sdl::V1_0::imtauth::CLogoutGqlRequest& logoutRequest,
				const ::imtgql::CGqlRequest& gqlRequest,
				QString& errorMessage) const override;
	virtual sdl::V1_0::imtauth::CPermissionList OnGetPermissions(
				const sdl::V1_0::imtauth::CGetPermissionsGqlRequest& getPermissionsRequest,
				const ::imtgql::CGqlRequest& gqlRequest,
				QString& errorMessage) const override;
};


} // namespace imtauthgql


