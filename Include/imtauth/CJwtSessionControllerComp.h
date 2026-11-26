// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// Qt includes
#include <QtCore/QHash>
#include <QtCore/QMutex>

// ACF includes
#include <ilog/TLoggerCompWrap.h>
#include <ifile/IFileNameParam.h>
#include <iprm/INameParam.h>
#include <iprm/ITextParam.h>

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
		I_ASSIGN(m_refreshTokenLifetimeParamCompPtr, "RefreshTokenLifetimeParam", "Lifetime of a session since its last refresh (in secs), a setting of the server; RefreshTokenLifetime when unset", false, "RefreshTokenLifetimeParam");
		I_ASSIGN(m_jwtLifetimeParamCompPtr, "JwtLifetimeParam", "Lifetime of an access token (in secs), a setting of the server; JwtLifetime when unset", false, "JwtLifetimeParam");
		I_ASSIGN(m_reuseLeewayAttrPtr, "RefreshTokenReuseLeeway", "Seconds a rotated refresh token is only refused, e.g. two tabs refreshing at once; used later, it is taken as stolen and its session is closed", false, 30);
	I_END_COMPONENT;

protected:
	// reimplemented (imtauth::IJwtSessionController)
	virtual bool ValidateSession(const QByteArray& sessionId, bool* isCheckedPtr = nullptr) const override;
	virtual JwtState ValidateJwt(const QByteArray& token) const override;
	virtual bool RefreshToken(const QByteArray& refreshToken, UserSession& userSession) const override;
	virtual bool CreateNewSession(const QByteArray& userId, const QByteArray& tenantId, UserSession& userSession) const override;
	virtual ISessionSharedPtr GetSession(const QByteArray& sessionId) const override;
	virtual bool RemoveSession(const QByteArray& sessionId) const override;
	virtual QByteArray GetUserFromJwt(const QByteArray& jwt) const override;
	virtual QByteArray GetTenantFromJwt(const QByteArray& jwt) const override;
	virtual QByteArray GetSessionFromJwt(const QByteArray& jwt) const override;

private:
	QByteArray JsonObjectToBase64(const QJsonObject& object) const;
	QJsonObject JsonObjectFromBase64(const QByteArray& data) const;
	QByteArray InsertNewSession(const istd::IChangeable& object) const;
	QByteArray CreateSignature(const QByteArray& headerBase64, const QByteArray& payloadBase64) const;
	QByteArray GenerateJwt(const QByteArray& sessionId, const QByteArray& userId, const QByteArray& tenantId, const QByteArray& productId) const;
	QByteArray GetSessionIdByRefreshToken(const QByteArray& refreshToken) const;
	int GetJwtLifetime() const;
	int GetRefreshTokenLifetime() const;
	/// A positive whole number of seconds of the setting, or the attribute.
	static int GetLifetime(const iprm::ITextParam* paramPtr, int attributeValue);
	/**
		A refresh token rotated before. While its successor is unused it gives the current tokens of its session again: the answer
		to it may have been lost, or another client of the session refreshed at the same time. A token whose successor was rotated
		too is refused within the leeway; later its session is closed, somebody holds a copy of it. Called with m_refreshMutex held.
	*/
	bool ReplayRotatedToken(const QByteArray& refreshToken, UserSession& userSession) const;
	void RememberRotatedToken(const QByteArray& refreshToken, const QByteArray& successorToken, const QByteArray& sessionId, qint64 sessionEndMs) const;
	static QByteArray GetTokenHash(const QByteArray& refreshToken);

private:
	I_REF(imtbase::IObjectCollection, m_sessionCollectionCompPtr);
	I_REF(iprm::INameParam, m_secretKeyParamCompPtr);
	I_FACT(imtauth::ISession, m_sessionFactoryCompPtr);
	I_ATTR(int, m_refreshTokenLifetimeAttrPtr);
	I_ATTR(int, m_jwtLifetimeAttrPtr);
	I_REF(iprm::ITextParam, m_refreshTokenLifetimeParamCompPtr);
	I_REF(iprm::ITextParam, m_jwtLifetimeParamCompPtr);
	I_ATTR(int, m_reuseLeewayAttrPtr);

	// Serializes refresh-token rotation so that two concurrent refresh calls
	// for the same session cannot race (e.g. read-modify-write the session
	// record concurrently or both succeed with the same old refresh token).
	mutable QMutex m_refreshMutex;

	struct RotatedToken
	{
		QByteArray sessionId;
		// The hash of the token it was replaced with
		QByteArray successorHash;
		qint64 rotatedAtMs = 0;
	};

	// The refresh tokens rotated since the start of the server, by their hash: a reuse is told from an unknown token.
	// Kept while their session could live. Guarded by m_refreshMutex
	mutable QHash<QByteArray, RotatedToken> m_rotatedTokens;
	// The end of each session with rotated tokens, moved on with every refresh
	mutable QHash<QByteArray, qint64> m_rotatedSessionEnds;
	// The ended ones are dropped from time to time, not with every refresh
	mutable qint64 m_nextPruneMs = 0;
};


} // namespace imtauth
