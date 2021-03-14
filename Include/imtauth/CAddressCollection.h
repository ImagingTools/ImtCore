#pragma once


// ImtCore includes
#include <imtbase/TAggergatedObjectCollectionWrap.h>
#include <imtbase/CCollectionInfo.h>
#include <imtauth/IAddressProvider.h>
#include <imtauth/CAddress.h>


namespace imtauth
{


class CAddressCollection: public imtbase::TAggergatedObjectCollectionWrap<IAddressProvider, CAddress>
{
public:
	typedef imtbase::TAggergatedObjectCollectionWrap<IAddressProvider, CAddress> BaseClass;

	CAddressCollection();

	// reimplemented (IAddressProviderr)
	virtual const imtbase::ICollectionInfo& GetAddressList() const override;
	virtual const IAddress* GetAddress(const QByteArray& featureId) const override;
};


} // namespace imtauth


