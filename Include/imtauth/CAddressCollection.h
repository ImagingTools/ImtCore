// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ImtCore includes
#include <imtbase/TAggergatedObjectCollectionWrap.h>
#include <imtbase/CCollectionInfo.h>
#include <imtauth/IAddressManager.h>
#include <imtauth/CAddress.h>


namespace imtauth
{


class CAddressCollection: public imtbase::TAggergatedObjectCollectionWrap<IAddressManager, CAddress>
{
public:
	typedef imtbase::TAggergatedObjectCollectionWrap<IAddressManager, CAddress> BaseClass;

	CAddressCollection();

	// reimplemented (IAddressManager)
	virtual QByteArray AddAddress(IAddress* addressPtr) override;
	virtual bool RemoveAddress(const QByteArray& addressId) override;
	virtual bool RemoveAllAddresses() override;
	virtual IAddress* GetEditableAddress(const QByteArray& addressId) override;

	// reimplemented (IAddressProvider)
	virtual const imtbase::ICollectionInfo& GetAddressList() const override;
	virtual const IAddress* GetAddress(const QByteArray& addressId) const override;
	virtual AddressCategory GetAddressCategory(const QByteArray& addressId) const override;
};


} // namespace imtauth


