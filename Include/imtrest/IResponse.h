#pragma once


// ACF includes
#include <istd/IChangeable.h>


namespace imtrest
{


class IResponse: virtual public istd::IChangeable
{
public:
	virtual QByteArray GetRequestId() const = 0;
};


} // namespace imtrest



