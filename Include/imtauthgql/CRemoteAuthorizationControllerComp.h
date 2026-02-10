// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ImtCore includes
#include <imtclientgql/TClientRequestManagerCompWrap.h>
#include <GeneratedFiles/imtauthsdl/SDL/1.0/CPP/Authorization.h>


namespace imtauthgql
{


class CRemoteAuthorizationControllerComp:
		public imtclientgql::TClientRequestManagerCompWrap<
					sdl::imtauth::Authorization::CGraphQlHandlerCompBase>
{
public:
	typedef imtclientgql::TClientRequestManagerCompWrap<sdl::imtauth::Authorization::CGraphQlHandlerCompBase> BaseClass;

	I_BEGIN_COMPONENT(CRemoteAuthorizationControllerComp);
	I_END_COMPONENT;

protected:
	// reimplemented (sdl::imtauth::Authorization::CGraphQlHandlerCompBase)
	virtual sdl::imtauth::Authorization::CAuthorizationPayload OnAuthorization(
				const sdl::imtauth::Authorization::CAuthorizationGqlRequest& authorizationRequest,
				const imtgql::CGqlRequest& gqlRequest,
				QString& errorMessage) const override;
	virtual sdl::imtauth::Authorization::CAuthorizationPayload OnUserToken(
				const sdl::imtauth::Authorization::CUserTokenGqlRequest& userTokenRequest,
				const ::imtgql::CGqlRequest& gqlRequest,
				QString& errorMessage) const override;
	virtual sdl::imtauth::Authorization::CLogoutPayload OnLogout(
				const sdl::imtauth::Authorization::CLogoutGqlRequest& logoutRequest,
				const ::imtgql::CGqlRequest& gqlRequest,
				QString& errorMessage) const override;
	virtual sdl::imtauth::Authorization::CPermissionList OnGetPermissions(
				const sdl::imtauth::Authorization::CGetPermissionsGqlRequest& getPermissionsRequest,
				const ::imtgql::CGqlRequest& gqlRequest,
				QString& errorMessage) const override;
};


} // namespace imtauthgql


