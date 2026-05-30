// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ImtCore includes
#include <imtclientgql/TClientRequestManagerCompWrap.h>
#include <GeneratedFiles/imtauthsdl/SDL/1.0/CPP/Sessions>


namespace imtauthgql
{


class CJwtSessionControllerProxyComp:
			public imtclientgql::TClientRequestManagerCompWrap<
							sdl::V1_0::imtauth::CSessionsGqlHandlerCompBase>
{
public:
	typedef imtclientgql::TClientRequestManagerCompWrap<sdl::V1_0::imtauth::CSessionsGqlHandlerCompBase> BaseClass;

	I_BEGIN_COMPONENT(CJwtSessionControllerProxyComp);
	I_END_COMPONENT;

protected:
	// reimplemented (sdl::V1_0::imtauth::CSessionsGqlHandlerCompBase)
	virtual sdl::V1_0::imtauth::CValidateSessionPayload OnValidateSession(
				const sdl::V1_0::imtauth::CValidateSessionGqlRequest& validateSessionRequest,
				const ::imtgql::CGqlRequest& gqlRequest,
				QString& errorMessage) const override;
	virtual sdl::V1_0::imtauth::CValidateJwtPayload OnValidateJwt(
				const sdl::V1_0::imtauth::CValidateJwtGqlRequest& validateJwtRequest,
				const ::imtgql::CGqlRequest& gqlRequest,
				QString& errorMessage) const override;
	virtual sdl::V1_0::imtauth::CGetSessionPayload OnGetSession(
				const sdl::V1_0::imtauth::CGetSessionGqlRequest& getSessionRequest,
				const ::imtgql::CGqlRequest& gqlRequest,
				QString& errorMessage) const override;
	virtual sdl::V1_0::imtauth::CRefreshTokenPayload OnRefreshToken(
				const sdl::V1_0::imtauth::CRefreshTokenGqlRequest& refreshTokenRequest,
				const ::imtgql::CGqlRequest& gqlRequest,
				QString& errorMessage) const override;
	virtual sdl::V1_0::imtauth::CCreateNewSessionPayload OnCreateNewSession(
				const sdl::V1_0::imtauth::CCreateNewSessionGqlRequest& createNewSessionRequest,
				const ::imtgql::CGqlRequest& gqlRequest,
				QString& errorMessage) const override;
	virtual sdl::V1_0::imtauth::CSelectTenantPayload OnSelectTenant(
				const sdl::V1_0::imtauth::CSelectTenantGqlRequest& selectTenantRequest,
				const ::imtgql::CGqlRequest& gqlRequest,
				QString& errorMessage) const override;
	virtual sdl::V1_0::imtauth::CRemoveSessionPayload OnRemoveSession(
				const sdl::V1_0::imtauth::CRemoveSessionGqlRequest& removeSessionRequest,
				const ::imtgql::CGqlRequest& gqlRequest,
				QString& errorMessage) const override;
	virtual sdl::V1_0::imtauth::CGetUserFromJwtPayload OnGetUserFromJwt(
				const sdl::V1_0::imtauth::CGetUserFromJwtGqlRequest& getUserFromJwtRequest,
				const ::imtgql::CGqlRequest& gqlRequest,
				QString& errorMessage) const override;
	virtual sdl::V1_0::imtauth::CGetTenantFromJwtPayload OnGetTenantFromJwt(
				const sdl::V1_0::imtauth::CGetTenantFromJwtGqlRequest& getTenantFromJwtRequest,
				const ::imtgql::CGqlRequest& gqlRequest,
				QString& errorMessage) const override;
};


} // namespace imtauthgql
