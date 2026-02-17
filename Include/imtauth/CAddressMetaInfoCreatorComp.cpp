// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtauth/CAddressMetaInfoCreatorComp.h>


// ACF includes
#include <imod/TModelWrap.h>

// ImtCore includes
#include <imtauth/IAddress.h>


namespace imtauth
{


// protected methods

// reimplemented (imtbase::IMetaInfoCreator)

bool CAddressMetaInfoCreatorComp::CreateMetaInfo(
			const istd::IChangeable* dataPtr,
			const QByteArray& typeId,
			idoc::MetaInfoPtr& metaInfoPtr) const
{
		if (!m_objectTypeIdsAttrPtr.IsValid()){
		return false;
	}

	bool isSupported = false;
	int count = m_objectTypeIdsAttrPtr.GetCount();
	for (int i = 0; i < count; i++){
		if (m_objectTypeIdsAttrPtr[i] == typeId){
			isSupported = true;
			break;
		}
	}

	if (!isSupported){
		return false;
	}

	metaInfoPtr.SetPtr(new imod::TModelWrap<MetaInfo>);

	if (dataPtr == nullptr){
		return true;
	}

	const IAddress* addressPtr = dynamic_cast<const IAddress*>(dataPtr);
	if (addressPtr == nullptr){
		return false;
	}

	metaInfoPtr->SetMetaInfo(IAddress::MIT_CITY, addressPtr->GetCity());
	metaInfoPtr->SetMetaInfo(IAddress::MIT_COUNTRY, addressPtr->GetCountry());
	metaInfoPtr->SetMetaInfo(IAddress::MIT_POSTAL_CODE, addressPtr->GetPostalCode());
	metaInfoPtr->SetMetaInfo(IAddress::MIT_STREET, addressPtr->GetStreet());

	return true;
}


// public methods of embedded class MetaInfo

QString CAddressMetaInfoCreatorComp::MetaInfo::GetMetaInfoName(int metaInfoType) const
{
	switch (metaInfoType){
	case IAddress::MIT_CITY:
		return QObject::tr("City");
	case IAddress::MIT_COUNTRY:
		return QObject::tr("Country");
	case IAddress::MIT_POSTAL_CODE:
		return QObject::tr("Postal Code");
	case IAddress::MIT_STREET:
		return QObject::tr("Street");
	}

	return BaseClass::GetMetaInfoName(metaInfoType);
}


} // namespace imtauth


