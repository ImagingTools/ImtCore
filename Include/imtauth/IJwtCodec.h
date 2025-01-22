#pragma once


// ACF includes
#include <iser/IObject.h>

// ImtCore includes
#include <imtauth/IUserInfo.h>
#include <imtauth/IJsonWebToken.h>


namespace imtauth
{


class IJwtCodec: virtual public istd::IPolymorphic
{
public:
	virtual QByteArray EncodeToken(const IJsonWebToken& jsonWebToken) const = 0;
	virtual const IJsonWebToken* DecodeToken(const QByteArray& token) const = 0;

};


} // namespace imtauth

