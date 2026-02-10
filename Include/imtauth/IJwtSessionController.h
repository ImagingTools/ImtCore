// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ACF includes
#include <istd/IPolymorphic.h>

// ImtCore includes
#include <imtauth/ISession.h>


namespace imtauth
{


class IJwtSessionController: virtual public istd::IPolymorphic
{
public:
	struct UserSession
	{
		QByteArray userId;
		QByteArray accessToken;
		QByteArray refreshToken;
	};

	enum JwtState
	{
		JS_NONE,
		JS_OK,
		JS_EXPIRED,
		JS_INVALID
	};

	virtual bool ValidateSession(const QByteArray& sessionId) const = 0;
	virtual JwtState ValidateJwt(const QByteArray& jwt) const = 0;
	virtual bool RefreshToken(const QByteArray& refreshToken, UserSession& outputData) const = 0;
	virtual bool CreateNewSession(const QByteArray& userId, UserSession& outputData) const = 0;
	virtual ISessionSharedPtr GetSession(const QByteArray& sessionId) const = 0;
	virtual bool RemoveSession(const QByteArray& sessionId) const = 0;
	virtual QByteArray GetUserFromJwt(const QByteArray& jwt) const = 0;
	virtual QByteArray GetSessionFromJwt(const QByteArray& jwt) const = 0;
};


} // namespace imtauth


