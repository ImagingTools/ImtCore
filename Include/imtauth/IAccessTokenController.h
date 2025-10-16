#pragma once


// ImtCore includes
#include <imtauth/IAccessTokenProvider.h>


namespace imtauth
{


class IAccessTokenController: virtual public imtauth::IAccessTokenProvider
{
public:
	virtual bool SetToken(const QByteArray& userId, const QByteArray& accessToken) = 0;
};


} // namespace imtauth


