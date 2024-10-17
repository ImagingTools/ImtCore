#pragma once


// ACF includes
#include <istd/IPolymorphic.h>


namespace imtcom
{


class IConnectionController: virtual public istd::IPolymorphic
{
public:
	virtual bool Connect() = 0;
	virtual bool Disconnect() = 0;
};


} // namespace imtcom


