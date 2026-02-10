// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
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


