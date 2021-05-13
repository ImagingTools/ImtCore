#pragma once


// ImtCore includes
#include <imtauth/IAddressProvider.h>


namespace imtbase
{
	class ICollectionInfo;
}


namespace imtauth
{


class IAddress;


/**
	Manager of addresses.
	\ingroup Authentification
*/
class IAddressManager: virtual public IAddressProvider
{
public:
	virtual QByteArray AddAddress(IAddress* addressPtr) = 0;
	virtual bool RemoveAddress(const QByteArray& addressId) = 0;
	virtual bool RemoveAllAddresses() = 0;
	virtual IAddress* GetEditableAddress(const QByteArray& addressId) = 0;
};


} // namespace imtauth


