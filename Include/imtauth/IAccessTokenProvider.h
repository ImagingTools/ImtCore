#pragma once


// ACF includes
#include <istd/IChangeable.h>


namespace imtauth
{


class IAccessTokenProvider: virtual public istd::IChangeable
{
public:
	virtual QByteArray GetToken(const QByteArray& userId) const = 0;
};


} // namespace imtauth


