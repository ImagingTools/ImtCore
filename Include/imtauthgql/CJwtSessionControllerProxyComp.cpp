// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtauthgql/CJwtSessionControllerProxyComp.h>


namespace imtauthgql
{


// reimplemented (sdl::imtauth::Sessions::CGraphQlHandlerCompBase)

sdl::imtauth::Sessions::CValidateSessionPayload CJwtSessionControllerProxyComp::OnValidateSession(
			const sdl::imtauth::Sessions::CValidateSessionGqlRequest& /*validateSessionRequest*/,
			const ::imtgql::CGqlRequest& gqlRequest,
			QString& errorMessage) const
{
	return SendModelRequest<sdl::imtauth::Sessions::CValidateSessionPayload>(gqlRequest, errorMessage);
}


sdl::imtauth::Sessions::CValidateJwtPayload CJwtSessionControllerProxyComp::OnValidateJwt(
			const sdl::imtauth::Sessions::CValidateJwtGqlRequest& /*validateJwtRequest*/,
			const ::imtgql::CGqlRequest& gqlRequest,
			QString& errorMessage) const
{
	return SendModelRequest<sdl::imtauth::Sessions::CValidateJwtPayload>(gqlRequest, errorMessage);
}


sdl::imtauth::Sessions::CGetSessionPayload CJwtSessionControllerProxyComp::OnGetSession(
			const sdl::imtauth::Sessions::CGetSessionGqlRequest& /*getSessionRequest*/,
			const ::imtgql::CGqlRequest& gqlRequest,
			QString& errorMessage) const
{
	return SendModelRequest<sdl::imtauth::Sessions::CGetSessionPayload>(gqlRequest, errorMessage);
}


sdl::imtauth::Sessions::CRefreshTokenPayload CJwtSessionControllerProxyComp::OnRefreshToken(
			const sdl::imtauth::Sessions::CRefreshTokenGqlRequest& /*refreshTokenRequest*/,
			const ::imtgql::CGqlRequest& gqlRequest,
			QString& errorMessage) const
{
	return SendModelRequest<sdl::imtauth::Sessions::CRefreshTokenPayload>(gqlRequest, errorMessage);
}


sdl::imtauth::Sessions::CCreateNewSessionPayload CJwtSessionControllerProxyComp::OnCreateNewSession(
			const sdl::imtauth::Sessions::CCreateNewSessionGqlRequest& /*createNewSessionRequest*/,
			const ::imtgql::CGqlRequest& gqlRequest,
			QString& errorMessage) const
{
	return SendModelRequest<sdl::imtauth::Sessions::CCreateNewSessionPayload>(gqlRequest, errorMessage);
}


sdl::imtauth::Sessions::CSelectTenantPayload CJwtSessionControllerProxyComp::OnSelectTenant(
			const sdl::imtauth::Sessions::CSelectTenantGqlRequest& /*selectTenantRequest*/,
			const ::imtgql::CGqlRequest& gqlRequest,
			QString& errorMessage) const
{
	return SendModelRequest<sdl::imtauth::Sessions::CSelectTenantPayload>(gqlRequest, errorMessage);
}


sdl::imtauth::Sessions::CRemoveSessionPayload CJwtSessionControllerProxyComp::OnRemoveSession(
			const sdl::imtauth::Sessions::CRemoveSessionGqlRequest& /*removeSessionRequest*/,
			const ::imtgql::CGqlRequest& gqlRequest,
			QString& errorMessage) const
{
	return SendModelRequest<sdl::imtauth::Sessions::CRemoveSessionPayload>(gqlRequest, errorMessage);
}


sdl::imtauth::Sessions::CGetUserFromJwtPayload CJwtSessionControllerProxyComp::OnGetUserFromJwt(
			const sdl::imtauth::Sessions::CGetUserFromJwtGqlRequest& /*getUserFromJwtRequest*/,
			const ::imtgql::CGqlRequest& gqlRequest,
			QString& errorMessage) const
{
	return SendModelRequest<sdl::imtauth::Sessions::CGetUserFromJwtPayload>(gqlRequest, errorMessage);
}


} // namespace imtauthgql
