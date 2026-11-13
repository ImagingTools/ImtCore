// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ACF includes
#include <ilog/TLoggerCompWrap.h>

// ImtCore includes
#include <imtauth/IJwtSessionController.h>
#include <imtauth/ISessionAuthorizationProvider.h>
#include <imtauth/IUserInfo.h>
#include <imtclientgql/IServerCredentialProvider.h>
#include <imtclientgql/TClientRequestManagerCompWrap.h>


namespace imtauthgql
{


class CRemoteJwtSessionControllerComp:
			public imtclientgql::TClientRequestManagerCompWrap<ilog::CLoggerComponentBase>,
			virtual public imtauth::IJwtSessionController,
			virtual public imtauth::ISessionAuthorizationProvider
{
public:
	typedef imtclientgql::TClientRequestManagerCompWrap<ilog::CLoggerComponentBase> BaseClass;

	I_BEGIN_COMPONENT(CRemoteJwtSessionControllerComp);
		I_REGISTER_INTERFACE(imtauth::IJwtSessionController);
		I_REGISTER_INTERFACE(imtauth::ISessionAuthorizationProvider);
		I_ASSIGN(m_sessionFactCompPtr, "SessionFactory", "Factory used for creation of the new session", false, "SessionFactory");
		I_ASSIGN(m_userFactCompPtr, "UserFactory", "Creates the computed user returned by session authorization", false, "UserFactory");
		I_ASSIGN(m_serverCredentialCompPtr, "ServerCredentialProvider", "Token of this server at the authorization server: without one the sessions are not authorized there", false, "ServerCredentialProvider");
	I_END_COMPONENT;

	// reimplemented (imtauth::ISessionAuthorizationProvider)
	virtual bool IsAvailable() const override;
	virtual SessionAuthorization GetSessionAuthorization(
				const QByteArray& accessToken,
				const QByteArray& productId,
				bool isEstablishedSubscription) const override;

protected:
	// reimplemented (imtauth::IJwtSessionController)
	virtual bool ValidateSession(const QByteArray& sessionId, bool* isCheckedPtr = nullptr) const override;
	virtual JwtState ValidateJwt(const QByteArray& token) const override;
	virtual bool RefreshToken(const QByteArray& refreshToken, UserSession& userSession) const override;
	virtual bool CreateNewSession(const QByteArray& userId, const QByteArray& tenantId, UserSession& userSession) const override;
	virtual imtauth::ISessionSharedPtr GetSession(const QByteArray& sessionId) const override;
	virtual bool RemoveSession(const QByteArray& sessionId) const override;
	virtual QByteArray GetUserFromJwt(const QByteArray& jwt) const override;
	virtual QByteArray GetTenantFromJwt(const QByteArray& jwt) const override;
	virtual QByteArray GetSessionFromJwt(const QByteArray& jwt) const override;

protected:
	I_FACT(imtauth::ISession, m_sessionFactCompPtr);
	I_FACT(imtauth::IUserInfo, m_userFactCompPtr);
	I_REF(imtclientgql::IServerCredentialProvider, m_serverCredentialCompPtr);
};


} // namespace imtauthgql
