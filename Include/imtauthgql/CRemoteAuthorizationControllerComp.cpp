// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtauthgql/CRemoteAuthorizationControllerComp.h>


namespace imtauthgql
{


// protected methods

// reimplemented (sdl::imtauth::Authorization::V1_0::CGraphQlHandlerCompBase)

sdl::imtauth::Authorization::CAuthorizationPayload CRemoteAuthorizationControllerComp::OnAuthorization(
			const sdl::imtauth::Authorization::CAuthorizationGqlRequest& /*authorizationRequest*/,
			const imtgql::CGqlRequest& gqlRequest,
			QString& errorMessage) const
{
	return SendModelRequest<sdl::imtauth::Authorization::CAuthorizationPayload>(gqlRequest, errorMessage);
}


sdl::imtauth::Authorization::CAuthorizationPayload CRemoteAuthorizationControllerComp::OnUserToken(
			const sdl::imtauth::Authorization::CUserTokenGqlRequest& /*userTokenRequest*/,
			const ::imtgql::CGqlRequest& gqlRequest,
			QString& errorMessage) const
{
	return SendModelRequest<sdl::imtauth::Authorization::CAuthorizationPayload>(gqlRequest, errorMessage);
}


sdl::imtauth::Authorization::CLogoutPayload CRemoteAuthorizationControllerComp::OnLogout(
			const sdl::imtauth::Authorization::CLogoutGqlRequest& /*logoutRequest*/,
			const ::imtgql::CGqlRequest& gqlRequest,
			QString& errorMessage) const
{
	return SendModelRequest<sdl::imtauth::Authorization::CLogoutPayload>(gqlRequest, errorMessage);
}


sdl::imtauth::Authorization::CPermissionList CRemoteAuthorizationControllerComp::OnGetPermissions(
			const sdl::imtauth::Authorization::CGetPermissionsGqlRequest& /*getPermissionsRequest*/,
			const ::imtgql::CGqlRequest& gqlRequest,
			QString& errorMessage) const
{
	return SendModelRequest<sdl::imtauth::Authorization::CPermissionList>(gqlRequest, errorMessage);
}


} // namespace imtauthgql


