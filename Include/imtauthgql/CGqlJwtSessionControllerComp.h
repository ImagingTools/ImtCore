// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ACF includes
#include <ilog/TLoggerCompWrap.h>

// ImtCore includes
#include <imtauth/IJwtSessionController.h>
#include <imtauth/ITenantManager.h>
#include <imtauth/ITenantMembershipManager.h>
#include <GeneratedFiles/imtauthsdl/SDL/1.0/CPP/Sessions>


namespace imtauthgql
{


class CGqlJwtSessionControllerComp: public sdl::imtauth::Sessions::CGraphQlHandlerCompBase
{
public:
	typedef sdl::imtauth::Sessions::CGraphQlHandlerCompBase BaseClass;

	I_BEGIN_COMPONENT(CGqlJwtSessionControllerComp);
		I_ASSIGN(m_jwtSessionControllerCompPtr, "JwtSessionController", "Json Web Token session controller", false, "JwtSessionController");
		I_ASSIGN(m_tenantManagerCompPtr, "TenantManager", "Tenant manager", false, "TenantManager");
		I_ASSIGN(m_membershipManagerCompPtr, "MembershipManager", "Tenant membership manager", false, "TenantMembershipManager");
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

private:
	QByteArray GetAuthenticatedUserId(const ::imtgql::CGqlRequest& gqlRequest) const;
	bool CanUseTenant(const QByteArray& userId, const QByteArray& tenantId, QString& errorMessage) const;
	sdl::imtauth::Sessions::CUserSession::V1_0 CreateUserSessionData(
			const imtauth::IJwtSessionController::UserSession& userSession) const;

protected:
	I_REF(imtauth::IJwtSessionController, m_jwtSessionControllerCompPtr);
	I_REF(imtauth::ITenantManager, m_tenantManagerCompPtr);
	I_REF(imtauth::ITenantMembershipManager, m_membershipManagerCompPtr);
	I_REF(iser::IVersionInfo, m_versionInfoCompPtr);
};


} // namespace imtauthgql
