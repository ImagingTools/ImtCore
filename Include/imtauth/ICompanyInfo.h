#pragma once


// ACF includes
#include <istd/TPointerVector.h>

// ImtCore includes
#include <imtauth/ICompanyBaseInfo.h>


namespace imtauth
{


class ICompanyInfo: virtual public ICompanyBaseInfo
{
public:
	virtual const IAddressProvider* GetAddresses() const = 0;
};


} // namespace imtauth
