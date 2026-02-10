// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ACF includes
#include <ilog/TLoggerCompWrap.h>

// ImtCore includes
#include <imtauth/IJwtSessionController.h>
#include <GeneratedFiles/imtauthsdl/SDL/1.0/CPP/Sessions.h>


namespace imtauthgql
{


class CGqlJwtSessionControllerComp: public sdl::imtauth::Sessions::CGraphQlHandlerCompBase
{
public:
	typedef sdl::imtauth::Sessions::CGraphQlHandlerCompBase BaseClass;

	I_BEGIN_COMPONENT(CGqlJwtSessionControllerComp);
		I_REGISTER_INTERFACE(imtauth::IJwtSessionController);
		I_ASSIGN(m_jwtSessionControllerCompPtr, "JwtSessionController", "Json Web Token session controller", false, "JwtSessionController");
		I_ASSIGN(m_versionInfoCompPtr, "VersionInfo", "Version info", false, "VersionInfo");
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
	virtual sdl::imtauth::Sessions::CRemoveSessionPayload OnRemoveSession(
		const sdl::imtauth::Sessions::CRemoveSessionGqlRequest& removeSessionRequest,
		const ::imtgql::CGqlRequest& gqlRequest,
		QString& errorMessage) const override;
	virtual sdl::imtauth::Sessions::CGetUserFromJwtPayload OnGetUserFromJwt(
		const sdl::imtauth::Sessions::CGetUserFromJwtGqlRequest& getUserFromJwtRequest,
		const ::imtgql::CGqlRequest& gqlRequest,
		QString& errorMessage) const override;

protected:
	I_REF(imtauth::IJwtSessionController, m_jwtSessionControllerCompPtr);
	I_REF(iser::IVersionInfo, m_versionInfoCompPtr);
};


} // namespace imtauthgql
