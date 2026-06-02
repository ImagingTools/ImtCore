// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtauthgql/CRemoteAuthorizationControllerComp.h>
#include <GeneratedFiles/imtauthsdl/SDL/1.0/CPP/Authorization.h>


namespace imtauthgql
{


// protected methods

// reimplemented (sdl::V1_0::imtauth::CAuthorizationGqlHandlerCompBase)

sdl::V1_0::imtauth::CAuthorizationPayload CRemoteAuthorizationControllerComp::OnAuthorization(
			const sdl::V1_0::imtauth::CAuthorizationGqlRequest& /*authorizationRequest*/,
			const imtgql::CGqlRequest& gqlRequest,
			QString& errorMessage) const
{
	return SendModelRequest<sdl::V1_0::imtauth::CAuthorizationPayload>(gqlRequest, errorMessage);
}


sdl::V1_0::imtauth::CAuthorizationPayload CRemoteAuthorizationControllerComp::OnUserToken(
			const sdl::V1_0::imtauth::CUserTokenGqlRequest& /*userTokenRequest*/,
			const ::imtgql::CGqlRequest& gqlRequest,
			QString& errorMessage) const
{
	return SendModelRequest<sdl::V1_0::imtauth::CAuthorizationPayload>(gqlRequest, errorMessage);
}


sdl::V1_0::imtauth::CLogoutPayload CRemoteAuthorizationControllerComp::OnLogout(
			const sdl::V1_0::imtauth::CLogoutGqlRequest& /*logoutRequest*/,
			const ::imtgql::CGqlRequest& gqlRequest,
			QString& errorMessage) const
{
	return SendModelRequest<sdl::V1_0::imtauth::CLogoutPayload>(gqlRequest, errorMessage);
}


sdl::V1_0::imtauth::CPermissionList CRemoteAuthorizationControllerComp::OnGetPermissions(
			const sdl::V1_0::imtauth::CGetPermissionsGqlRequest& /*getPermissionsRequest*/,
			const ::imtgql::CGqlRequest& gqlRequest,
			QString& errorMessage) const
{
	return SendModelRequest<sdl::V1_0::imtauth::CPermissionList>(gqlRequest, errorMessage);
}


} // namespace imtauthgql


