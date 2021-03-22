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

	// reimplemented (IAddressProvider)
	virtual const imtbase::ICollectionInfo& GetAddressList() const override;
	virtual const IAddress* GetAddress(const QByteArray& featureId) const override;
};


} // namespace imtauth


