#pragma once


// ImtCore includes
#include <imtauth/IPersonBaseInfo.h>


namespace imtauth
{


class IPersonInfo: virtual public IPersonBaseInfo
{
public:
	virtual const IAddressProvider* GetAddresses() const = 0;
};


} // namespace imtauth


