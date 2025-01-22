#pragma once


// ACF includes
#include <iser/IObject.h>

// ImtCore includes
#include <imtauth/IUserInfo.h>
#include <imtauth/IJsonWebToken.h>


namespace imtauth
{


class IJwtSessionInfo: virtual public iser::ISerializable
{
public:
	virtual const imtauth::IJsonWebToken* GetAccessToken() const = 0;
	virtual void SetAccessToken(const imtauth::IJsonWebToken* accessTokenPtr) = 0;

	virtual QByteArray GetRefreshToken() const = 0;
	virtual void SetRefreshToken(const QByteArray& refreshToken) = 0;

	virtual QDateTime GetCreationDate() const = 0;
	virtual void SetCreationDate(const QDateTime& creationDate) = 0;

	virtual int GetAccessTokenLifeTime() const = 0;
	virtual void SetAccessTokenLifeTime(int lifeTime) = 0;

	virtual int GetRefreshTokenLifeTime() const = 0;
	virtual void SetRefreshTokenLifeTime(int lifeTime) = 0;

	virtual bool IsAccessTokenAlive() const = 0;
	virtual bool IsRefreshTokenAlive() const = 0;
};


} // namespace imtauth

