#include <imtauth/CAddressCollection.h>


namespace imtauth
{


// public methods

CAddressCollection::CAddressCollection()
	:BaseClass(CAddress::GetTypeId(), "Address", "Addresses")
{
}


// reimplemented (IAddressManager)
QByteArray CAddressCollection::AddAddress(IAddress* addressPtr)
{
	return m_collection.InsertNewObject("Address", "", "", addressPtr);
}


bool CAddressCollection::RemoveAddress(const QByteArray& addressId)
{
	return m_collection.RemoveObject(addressId);
}


bool CAddressCollection::RemoveAllAddresses()
{
	return m_collection.ResetData();
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


