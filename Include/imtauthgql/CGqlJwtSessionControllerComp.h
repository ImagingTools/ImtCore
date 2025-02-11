#pragma once


// ACF includes
#include <ilog/TLoggerCompWrap.h>

// ImtCore includes
#include <imtauth/IJwtSessionController.h>
#include <GeneratedFiles/imtauthsdl/SDL/1.0/CPP/Session.h>


namespace imtauthgql
{


class CGqlJwtSessionControllerComp: public sdl::imtauth::Session::CGraphQlHandlerCompBase
{
public:
	typedef sdl::imtauth::Session::CGraphQlHandlerCompBase BaseClass;

	I_BEGIN_COMPONENT(CGqlJwtSessionControllerComp);
		I_REGISTER_INTERFACE(imtauth::IJwtSessionController);
		I_ASSIGN(m_jwtSessionControllerCompPtr, "JwtSessionController", "Json Web Token session controller", false, "JwtSessionController");
		I_ASSIGN(m_versionInfoCompPtr, "VersionInfo", "Version info", false, "VersionInfo");
	I_END_COMPONENT;

protected:
	// reimplemented (sdl::imtauth::Session::CGraphQlHandlerCompBase)
	virtual sdl::imtauth::Session::CValidateSessionPayload OnValidateSession(
		const sdl::imtauth::Session::CValidateSessionGqlRequest& validateSessionRequest,
		const ::imtgql::CGqlRequest& gqlRequest,
		QString& errorMessage) const override;
	virtual sdl::imtauth::Session::CValidateJwtPayload OnValidateJwt(
		const sdl::imtauth::Session::CValidateJwtGqlRequest& validateJwtRequest,
		const ::imtgql::CGqlRequest& gqlRequest,
		QString& errorMessage) const override;
	virtual sdl::imtauth::Session::CGetSessionPayload OnGetSession(
		const sdl::imtauth::Session::CGetSessionGqlRequest& getSessionRequest,
		const ::imtgql::CGqlRequest& gqlRequest,
		QString& errorMessage) const override;
	virtual sdl::imtauth::Session::CRefreshTokenPayload OnRefreshToken(
		const sdl::imtauth::Session::CRefreshTokenGqlRequest& refreshTokenRequest,
		const ::imtgql::CGqlRequest& gqlRequest,
		QString& errorMessage) const override;
	virtual sdl::imtauth::Session::CCreateNewSessionPayload OnCreateNewSession(
		const sdl::imtauth::Session::CCreateNewSessionGqlRequest& createNewSessionRequest,
		const ::imtgql::CGqlRequest& gqlRequest,
		QString& errorMessage) const override;
	virtual sdl::imtauth::Session::CRemoveSessionPayload OnRemoveSession(
		const sdl::imtauth::Session::CRemoveSessionGqlRequest& removeSessionRequest,
		const ::imtgql::CGqlRequest& gqlRequest,
		QString& errorMessage) const override;
	virtual sdl::imtauth::Session::CGetUserFromJwtPayload OnGetUserFromJwt(
		const sdl::imtauth::Session::CGetUserFromJwtGqlRequest& getUserFromJwtRequest,
		const ::imtgql::CGqlRequest& gqlRequest,
		QString& errorMessage) const override;

protected:
	I_REF(imtauth::IJwtSessionController, m_jwtSessionControllerCompPtr);
	I_REF(iser::IVersionInfo, m_versionInfoCompPtr);
};


} // namespace imtauthgql
