#pragma once


// ACF includes
#include <istd/IPolymorphic.h>

// ImtCore includes
#include <imtauth/IJwtSessionInfo.h>


namespace imtauth
{


class IJwtSessionController: virtual public istd::IPolymorphic
{
public:
	virtual bool ValidateAccessToken(const QByteArray& accessToken) const = 0;
	virtual QByteArray GenerateAccessToken(const QByteArray& refreshToken) const = 0;
	virtual QByteArray CreateNewSession(const QByteArray& userId) const = 0;
	virtual const IJwtSessionInfo* GetSession(const QByteArray& userId) const = 0;
};


} // namespace imtauth


