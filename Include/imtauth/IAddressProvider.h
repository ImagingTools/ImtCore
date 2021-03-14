#pragma once


// ACF includes
#include <istd/IChangeable.h>


namespace imtbase
{
	class ICollectionInfo;
}


namespace imtauth
{


class IAddress;


/**
	Provider of addresses.
	\ingroup Authentification
*/
class IAddressProvider: virtual public istd::IChangeable
{
public:
	virtual const imtbase::ICollectionInfo& GetAddressList() const = 0;
	virtual const IAddress* GetAddress(const QByteArray& addressId) const = 0;
};


} // namespace imtauth


