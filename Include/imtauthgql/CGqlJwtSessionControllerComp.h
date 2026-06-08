// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ACF includes
#include <ilog/TLoggerCompWrap.h>

// ImtCore includes
#include <imtauth/IJwtSessionController.h>
#include <imtauth/ITenantManager.h>
#include <imtauth/ITenantMembershipManager.h>
#include <imtauthsdl/SDL/1.0/CPP/Sessions_fwd.h>


namespace imtauthgql
{


class CGqlJwtSessionControllerComp: public sdl::V1_0::imtauth::CSessionsGqlHandlerCompBase
{
public:
	typedef sdl::V1_0::imtauth::CSessionsGqlHandlerCompBase BaseClass;

	I_BEGIN_COMPONENT(CGqlJwtSessionControllerComp);
		I_ASSIGN(m_jwtSessionControllerCompPtr, "JwtSessionController", "Json Web Token session controller", false, "JwtSessionController");
		I_ASSIGN(m_tenantManagerCompPtr, "TenantManager", "Tenant manager", false, "TenantManager");
		I_ASSIGN(m_membershipManagerCompPtr, "MembershipManager", "Tenant membership manager", false, "TenantMembershipManager");
		I_ASSIGN(m_versionInfoCompPtr, "VersionInfo", "Version info", false, "VersionInfo");
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

private:
	QByteArray GetAuthenticatedUserId(const ::imtgql::CGqlRequest& gqlRequest) const;
	bool CanUseTenant(const QByteArray& userId, const QByteArray& tenantId, QString& errorMessage) const;
	sdl::V1_0::imtauth::CUserSession CreateUserSessionData(
			const imtauth::IJwtSessionController::UserSession& userSession) const;

protected:
	I_REF(imtauth::IJwtSessionController, m_jwtSessionControllerCompPtr);
	I_REF(imtauth::ITenantManager, m_tenantManagerCompPtr);
	I_REF(imtauth::ITenantMembershipManager, m_membershipManagerCompPtr);
	I_REF(iser::IVersionInfo, m_versionInfoCompPtr);
};


} // namespace imtauthgql
