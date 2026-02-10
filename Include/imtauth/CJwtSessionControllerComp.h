// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ACF includes
#include <ilog/TLoggerCompWrap.h>
#include <ifile/IFileNameParam.h>
#include <iprm/INameParam.h>

// ImtCore includes
#include <imtauth/IJwtSessionController.h>
#include <imtbase/IObjectCollection.h>


namespace imtauth
{


class CJwtSessionControllerComp:
			public ilog::CLoggerComponentBase,
			virtual public imtauth::IJwtSessionController
{
public:
	typedef icomp::CComponentBase BaseClass;

	I_BEGIN_COMPONENT(CJwtSessionControllerComp);
		I_REGISTER_INTERFACE(imtauth::IJwtSessionController);
		I_ASSIGN(m_sessionCollectionCompPtr, "SessionCollection", "Session collection", true, "SessionCollection");
		I_ASSIGN(m_sessionFactoryCompPtr, "SessionFactory", "Session factory", true, "SessionFactory");
		I_ASSIGN(m_secretKeyParamCompPtr, "SecretKeyParam", "Secret key param", true, "SecretKeyParam");
		I_ASSIGN(m_refreshTokenLifetimeAttrPtr, "RefreshTokenLifetime", "Refresh token Lifetime (in secs)", false, 86400);
		I_ASSIGN(m_jwtLifetimeAttrPtr, "JwtLifetime", "JWT Lifetime (in secs)", false, 3600);
	I_END_COMPONENT;

protected:
	// reimplemented (imtauth::IJwtSessionController)
	virtual bool ValidateSession(const QByteArray& sessionId) const override;
	virtual JwtState ValidateJwt(const QByteArray& token) const override;
	virtual bool RefreshToken(const QByteArray& refreshToken, UserSession& userSession) const override;
	virtual bool CreateNewSession(const QByteArray& userId, UserSession& userSession) const override;
	virtual ISessionSharedPtr GetSession(const QByteArray& sessionId) const override;
	virtual bool RemoveSession(const QByteArray& sessionId) const override;
	virtual QByteArray GetUserFromJwt(const QByteArray& jwt) const override;
	virtual QByteArray GetSessionFromJwt(const QByteArray& jwt) const override;

private:
	QByteArray JsonObjectToBase64(const QJsonObject& object) const;
	QJsonObject JsonObjectFromBase64(const QByteArray& data) const;
	QByteArray InsertNewSession(const istd::IChangeable& object) const;
	QByteArray CreateSignature(const QByteArray& headerBase64, const QByteArray& payloadBase64) const;
	QByteArray GenerateJwt(const QByteArray& sessionId, const QByteArray& userId) const;
	QByteArray GetSessionIdByRefreshToken(const QByteArray& refreshToken) const;

private:
	I_REF(imtbase::IObjectCollection, m_sessionCollectionCompPtr);
	I_REF(iprm::INameParam, m_secretKeyParamCompPtr);
	I_FACT(imtauth::ISession, m_sessionFactoryCompPtr);
	I_ATTR(int, m_refreshTokenLifetimeAttrPtr);
	I_ATTR(int, m_jwtLifetimeAttrPtr);
};


} // namespace imtauth
