// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ACF includes
#include <ilog/TLoggerCompWrap.h>

// ImtCore includes
#include <imtauth/IJwtSessionController.h>
#include <imtauth/ISessionAuthorizationProvider.h>
#include <imtauth/ITenantManager.h>
#include <imtauth/ITenantMembershipManager.h>
#include <imtauth/IDelegatedAccess.h>
#include <imtauth/IPasswordPolicy.h>
#include <imtbase/IObjectCollection.h>
#include <imtauth/ITenantEntityBindingManager.h>
#include <imtservergql/IServerTokenReceiver.h>
#include <GeneratedFiles/imtauthsdl/SDL/1.0/CPP/Sessions_fwd.h>


namespace imtauthgql
{


class CGqlJwtSessionControllerComp:
			public sdl::V1_0::imtauth::CSessionsGqlHandlerCompBase,
			virtual public imtauth::ISessionAuthorizationProvider
{
public:
	typedef sdl::V1_0::imtauth::CSessionsGqlHandlerCompBase BaseClass;

	I_BEGIN_COMPONENT(CGqlJwtSessionControllerComp);
		I_REGISTER_INTERFACE(imtauth::ISessionAuthorizationProvider);
		I_ASSIGN(m_jwtSessionControllerCompPtr, "JwtSessionController", "Json Web Token session controller", false, "JwtSessionController");
		I_ASSIGN(m_tenantManagerCompPtr, "TenantManager", "Tenant manager", false, "TenantManager");
		I_ASSIGN(m_membershipManagerCompPtr, "MembershipManager", "Tenant membership manager", false, "TenantMembershipManager");
		I_ASSIGN(m_delegatedAccessCompPtr, "DelegatedAccess", "Delegated access resolver for cross-org grants", false, "DelegatedAccessResolver");
		I_ASSIGN(m_versionInfoCompPtr, "VersionInfo", "Version info", false, "VersionInfo");
		I_ASSIGN(m_userCollectionCompPtr, "UserCollection", "User collection, read for the password lifetime check on a token refresh", false, "UserCollection");
		I_ASSIGN(m_passwordPolicyCompPtr, "PasswordPolicy", "Password policy: a token refresh is refused once the password has expired", false, "PasswordPolicy");
		I_ASSIGN(m_authorizationSessionControllerCompPtr, "AuthorizationSessionController", "Uncached JWT session controller for fresh authorization; ordinary session operations still use JwtSessionController", false, "");
		I_ASSIGN(m_roleCollectionCompPtr, "RoleCollection", "Roles, read at their source for session authorization", false, "RoleCollection");
		I_ASSIGN(m_userGroupCollectionCompPtr, "UserGroupCollection", "User groups, read at their source for session authorization", false, "UserGroupCollection");
		I_ASSIGN(m_bindingManagerCompPtr, "BindingManager", "Organizations of the roles and groups", false, "TenantEntityBindingManager");
		I_ASSIGN(m_serverTokenReceiverCompPtr, "ServerTokenReceiver", "Server tokens issued by this server", false, "ServerTokenReceiver");
	I_END_COMPONENT;

	// reimplemented (imtauth::ISessionAuthorizationProvider)
	virtual SessionAuthorization GetSessionAuthorization(
				const QByteArray& accessToken,
				const QByteArray& productId,
				bool isEstablishedSubscription) const override;

protected:
	// reimplemented (sdl::V1_0::imtauth::CSessionsGqlHandlerCompBase)
	virtual sdl::V1_0::imtauth::CGetSessionAuthorizationPayload OnGetSessionAuthorization(
				const sdl::V1_0::imtauth::CGetSessionAuthorizationGqlRequest& getSessionAuthorizationRequest,
				const ::imtgql::CGqlRequest& gqlRequest,
				QString& errorMessage) const override;
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
	AuthorizationState CheckAuthorizationServer(const ::imtgql::CGqlRequest& gqlRequest, QByteArray& productId, QString& message) const;
	QByteArray GetAuthenticatedUserId(const ::imtgql::CGqlRequest& gqlRequest) const;
	bool CanUseTenant(const QByteArray& userId, const QByteArray& tenantId, QString& errorMessage) const;
	bool IsPasswordChangeRequired(const QByteArray& userId) const;
	sdl::V1_0::imtauth::CUserSession CreateUserSessionData(
			const imtauth::IJwtSessionController::UserSession& userSession) const;

protected:
	I_REF(imtauth::IJwtSessionController, m_jwtSessionControllerCompPtr);
	I_REF(imtauth::ITenantManager, m_tenantManagerCompPtr);
	I_REF(imtauth::ITenantMembershipManager, m_membershipManagerCompPtr);
	I_REF(imtauth::IDelegatedAccess, m_delegatedAccessCompPtr);
	I_REF(iser::IVersionInfo, m_versionInfoCompPtr);
	I_REF(imtbase::IObjectCollection, m_userCollectionCompPtr);
	I_REF(imtauth::IPasswordPolicy, m_passwordPolicyCompPtr);
	I_REF(imtauth::IJwtSessionController, m_authorizationSessionControllerCompPtr);
	I_REF(imtbase::IObjectCollection, m_roleCollectionCompPtr);
	I_REF(imtbase::IObjectCollection, m_userGroupCollectionCompPtr);
	I_REF(imtauth::ITenantEntityBindingManager, m_bindingManagerCompPtr);
	I_REF(imtservergql::IServerTokenReceiver, m_serverTokenReceiverCompPtr);
};


} // namespace imtauthgql
