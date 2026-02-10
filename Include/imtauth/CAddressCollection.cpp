// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
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
	imtbase::ICollectionInfo::Ids addressIds;
	addressIds << addressId;
	return m_collection.RemoveElements(addressIds);
}


bool CAddressCollection::RemoveAllAddresses()
{
	return m_collection.ResetData();
}

IAddress* CAddressCollection::GetEditableAddress(const QByteArray &addressId)
{
	return dynamic_cast<IAddress*>(const_cast<istd::IChangeable*>(m_collection.GetObjectPtr(addressId)));
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


IAddressProvider::AddressCategory CAddressCollection::GetAddressCategory(const QByteArray& /*addressId*/) const
{
	return AC_CONTACT;
}


} // namespace imtauth


