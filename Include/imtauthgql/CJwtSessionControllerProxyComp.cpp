// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <GeneratedFiles/imtauthsdl/SDL/1.0/CPP/Sessions.h>
#include <imtauthgql/CJwtSessionControllerProxyComp.h>


namespace imtauthgql
{


// reimplemented (sdl::V1_0::imtauth::CSessionsGqlHandlerCompBase)

sdl::V1_0::imtauth::CValidateSessionPayload CJwtSessionControllerProxyComp::OnValidateSession(
			const sdl::V1_0::imtauth::CValidateSessionGqlRequest& /*validateSessionRequest*/,
			const ::imtgql::CGqlRequest& gqlRequest,
			QString& errorMessage) const
{
	return SendModelRequest<sdl::V1_0::imtauth::CValidateSessionPayload>(gqlRequest, errorMessage);
}


sdl::V1_0::imtauth::CValidateJwtPayload CJwtSessionControllerProxyComp::OnValidateJwt(
			const sdl::V1_0::imtauth::CValidateJwtGqlRequest& /*validateJwtRequest*/,
			const ::imtgql::CGqlRequest& gqlRequest,
			QString& errorMessage) const
{
	return SendModelRequest<sdl::V1_0::imtauth::CValidateJwtPayload>(gqlRequest, errorMessage);
}


sdl::V1_0::imtauth::CGetSessionPayload CJwtSessionControllerProxyComp::OnGetSession(
			const sdl::V1_0::imtauth::CGetSessionGqlRequest& /*getSessionRequest*/,
			const ::imtgql::CGqlRequest& gqlRequest,
			QString& errorMessage) const
{
	return SendModelRequest<sdl::V1_0::imtauth::CGetSessionPayload>(gqlRequest, errorMessage);
}


sdl::V1_0::imtauth::CRefreshTokenPayload CJwtSessionControllerProxyComp::OnRefreshToken(
			const sdl::V1_0::imtauth::CRefreshTokenGqlRequest& /*refreshTokenRequest*/,
			const ::imtgql::CGqlRequest& gqlRequest,
			QString& errorMessage) const
{
	return SendModelRequest<sdl::V1_0::imtauth::CRefreshTokenPayload>(gqlRequest, errorMessage);
}


sdl::V1_0::imtauth::CCreateNewSessionPayload CJwtSessionControllerProxyComp::OnCreateNewSession(
			const sdl::V1_0::imtauth::CCreateNewSessionGqlRequest& /*createNewSessionRequest*/,
			const ::imtgql::CGqlRequest& gqlRequest,
			QString& errorMessage) const
{
	return SendModelRequest<sdl::V1_0::imtauth::CCreateNewSessionPayload>(gqlRequest, errorMessage);
}


sdl::V1_0::imtauth::CSelectTenantPayload CJwtSessionControllerProxyComp::OnSelectTenant(
			const sdl::V1_0::imtauth::CSelectTenantGqlRequest& /*selectTenantRequest*/,
			const ::imtgql::CGqlRequest& gqlRequest,
			QString& errorMessage) const
{
	return SendModelRequest<sdl::V1_0::imtauth::CSelectTenantPayload>(gqlRequest, errorMessage);
}


sdl::V1_0::imtauth::CRemoveSessionPayload CJwtSessionControllerProxyComp::OnRemoveSession(
			const sdl::V1_0::imtauth::CRemoveSessionGqlRequest& /*removeSessionRequest*/,
			const ::imtgql::CGqlRequest& gqlRequest,
			QString& errorMessage) const
{
	return SendModelRequest<sdl::V1_0::imtauth::CRemoveSessionPayload>(gqlRequest, errorMessage);
}


sdl::V1_0::imtauth::CGetUserFromJwtPayload CJwtSessionControllerProxyComp::OnGetUserFromJwt(
			const sdl::V1_0::imtauth::CGetUserFromJwtGqlRequest& /*getUserFromJwtRequest*/,
			const ::imtgql::CGqlRequest& gqlRequest,
			QString& errorMessage) const
{
	return SendModelRequest<sdl::V1_0::imtauth::CGetUserFromJwtPayload>(gqlRequest, errorMessage);
}


sdl::V1_0::imtauth::CGetTenantFromJwtPayload CJwtSessionControllerProxyComp::OnGetTenantFromJwt(
			const sdl::V1_0::imtauth::CGetTenantFromJwtGqlRequest& /*getTenantFromJwtRequest*/,
			const ::imtgql::CGqlRequest& gqlRequest,
			QString& errorMessage) const
{
	return SendModelRequest<sdl::V1_0::imtauth::CGetTenantFromJwtPayload>(gqlRequest, errorMessage);
}


} // namespace imtauthgql
