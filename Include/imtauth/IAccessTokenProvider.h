#pragma once


// ACF includes
#include <istd/IPolymorphic.h>


namespace imtauth
{


class IAccessTokenProvider: virtual public istd::IPolymorphic
{
public:
	virtual QByteArray GetToken(const QByteArray& userId) const = 0;
};


} // namespace imtauth


