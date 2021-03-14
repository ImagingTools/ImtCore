#include <imtauth/CAddressCollection.h>


namespace imtauth
{


// public methods

CAddressCollection::CAddressCollection()
	:BaseClass(CAddress::GetTypeId(), "Address", "Addresses")
{
}


// reimplemented (IAddressProvider)

const imtbase::ICollectionInfo& CAddressCollection::GetAddressList() const
{
	return m_collection;
}


const IAddress* CAddressCollection::GetAddress(const QByteArray& addressId) const
{
	return dynamic_cast<const IAddress*>(m_collection.GetObjectPtr(addressId));
}


} // namespace imtauth


