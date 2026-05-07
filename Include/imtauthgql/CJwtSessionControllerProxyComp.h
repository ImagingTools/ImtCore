// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ImtCore includes
#include <imtclientgql/TClientRequestManagerCompWrap.h>
#include <GeneratedFiles/imtauthsdl/SDL/1.0/CPP/Sessions.h>


namespace imtauthgql
{


class CJwtSessionControllerProxyComp:
			public imtclientgql::TClientRequestManagerCompWrap<
							sdl::imtauth::Sessions::CGraphQlHandlerCompBase>
{
public:
	typedef imtclientgql::TClientRequestManagerCompWrap<sdl::imtauth::Sessions::CGraphQlHandlerCompBase> BaseClass;

	I_BEGIN_COMPONENT(CJwtSessionControllerProxyComp);
	I_END_COMPONENT;

protected:
	// reimplemented (sdl::imtauth::Sessions::CGraphQlHandlerCompBase)
	virtual sdl::imtauth::Sessions::CValidateSessionPayload OnValidateSession(
				const sdl::imtauth::Sessions::CValidateSessionGqlRequest& validateSessionRequest,
				const ::imtgql::CGqlRequest& gqlRequest,
				QString& errorMessage) const override;
	virtual sdl::imtauth::Sessions::CValidateJwtPayload OnValidateJwt(
				const sdl::imtauth::Sessions::CValidateJwtGqlRequest& validateJwtRequest,
				const ::imtgql::CGqlRequest& gqlRequest,
				QString& errorMessage) const override;
	virtual sdl::imtauth::Sessions::CGetSessionPayload OnGetSession(
				const sdl::imtauth::Sessions::CGetSessionGqlRequest& getSessionRequest,
				const ::imtgql::CGqlRequest& gqlRequest,
				QString& errorMessage) const override;
	virtual sdl::imtauth::Sessions::CRefreshTokenPayload OnRefreshToken(
				const sdl::imtauth::Sessions::CRefreshTokenGqlRequest& refreshTokenRequest,
				const ::imtgql::CGqlRequest& gqlRequest,
				QString& errorMessage) const override;
	virtual sdl::imtauth::Sessions::CCreateNewSessionPayload OnCreateNewSession(
				const sdl::imtauth::Sessions::CCreateNewSessionGqlRequest& createNewSessionRequest,
				const ::imtgql::CGqlRequest& gqlRequest,
				QString& errorMessage) const override;
	virtual sdl::imtauth::Sessions::CSelectTenantPayload OnSelectTenant(
				const sdl::imtauth::Sessions::CSelectTenantGqlRequest& selectTenantRequest,
				const ::imtgql::CGqlRequest& gqlRequest,
				QString& errorMessage) const override;
	virtual sdl::imtauth::Sessions::CRemoveSessionPayload OnRemoveSession(
				const sdl::imtauth::Sessions::CRemoveSessionGqlRequest& removeSessionRequest,
				const ::imtgql::CGqlRequest& gqlRequest,
				QString& errorMessage) const override;
	virtual sdl::imtauth::Sessions::CGetUserFromJwtPayload OnGetUserFromJwt(
				const sdl::imtauth::Sessions::CGetUserFromJwtGqlRequest& getUserFromJwtRequest,
				const ::imtgql::CGqlRequest& gqlRequest,
				QString& errorMessage) const override;
	virtual sdl::imtauth::Sessions::CGetTenantFromJwtPayload OnGetTenantFromJwt(
				const sdl::imtauth::Sessions::CGetTenantFromJwtGqlRequest& getTenantFromJwtRequest,
				const ::imtgql::CGqlRequest& gqlRequest,
				QString& errorMessage) const override;
};


} // namespace imtauthgql
