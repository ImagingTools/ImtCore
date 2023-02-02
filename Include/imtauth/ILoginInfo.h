#pragma once


// ACF includes
#include <istd/IChangeable.h>


namespace imtauth
{


class ILoginInfo: virtual public istd::IChangeable
{
public:
	virtual bool IsRememberUserEnabled() const = 0;
	virtual void EnableRememberUser(bool enable = true) = 0;
	virtual QString GetLastUser() const = 0;
	virtual void SetLastUser(const QString& userName) = 0;
	virtual QByteArray GetRefreshToken() const = 0;
	virtual void SetRefreshToken(const QByteArray& refreshToken) = 0;
};


} // namespace imtauth


