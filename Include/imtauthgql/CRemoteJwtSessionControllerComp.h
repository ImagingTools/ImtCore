// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ACF includes
#include <ilog/TLoggerCompWrap.h>

// ImtCore includes
#include <imtauth/IJwtSessionController.h>
#include <imtclientgql/TClientRequestManagerCompWrap.h>


namespace imtauthgql
{


class CRemoteJwtSessionControllerComp:
			public imtclientgql::TClientRequestManagerCompWrap<ilog::CLoggerComponentBase>,
			virtual public imtauth::IJwtSessionController
{
public:
	typedef imtclientgql::TClientRequestManagerCompWrap<ilog::CLoggerComponentBase> BaseClass;

	I_BEGIN_COMPONENT(CRemoteJwtSessionControllerComp);
		I_REGISTER_INTERFACE(imtauth::IJwtSessionController);
		I_ASSIGN(m_sessionFactCompPtr, "SessionFactory", "Factory used for creation of the new session", false, "SessionFactory");
	I_END_COMPONENT;

protected:
	// reimplemented (imtauth::IJwtSessionController)
	virtual bool ValidateSession(const QByteArray& sessionId) const override;
	virtual JwtState ValidateJwt(const QByteArray& token) const override;
	virtual bool RefreshToken(const QByteArray& refreshToken, UserSession& userSession) const override;
	virtual bool CreateNewSession(const QByteArray& userId, UserSession& userSession) const override;
	virtual imtauth::ISessionSharedPtr GetSession(const QByteArray& sessionId) const override;
	virtual bool RemoveSession(const QByteArray& sessionId) const override;
	virtual QByteArray GetUserFromJwt(const QByteArray& jwt) const override;
	virtual QByteArray GetSessionFromJwt(const QByteArray& jwt) const override;

protected:
	I_FACT(imtauth::ISession, m_sessionFactCompPtr);
};


} // namespace imtauthgql
