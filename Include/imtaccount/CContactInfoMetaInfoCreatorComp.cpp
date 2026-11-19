// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtaccount/CContactInfoMetaInfoCreatorComp.h>


// ACF includes
#include <imod/TModelWrap.h>
#include <idoc/CStandardDocumentMetaInfo.h>

// ImtCore includes
#include <imtaccount/IContactInfo.h>


namespace imtaccount
{


// protected methods

// reimplemented (imtbase::IMetaInfoCreator)

bool CContactInfoMetaInfoCreatorComp::CreateMetaInfo(
			const istd::IChangeable* dataPtr,
			const QByteArray& typeId,
			idoc::MetaInfoPtr& metaInfoPtr) const
{
	if (m_objectTypeIdsAttrPtr.FindValue(typeId) < 0){
		return false;
	}

	metaInfoPtr.SetPtr(new imod::TModelWrap<MetaInfo>);

	if (dataPtr == nullptr){
		return true;
	}

	const IContactInfo* contactPtr = dynamic_cast<const IContactInfo*>(dataPtr);
	if (contactPtr == nullptr){
		return false;
	}

	metaInfoPtr->SetMetaInfo(IContactInfo::MIT_EMAIL, contactPtr->GetEmail());
	metaInfoPtr->SetMetaInfo(IContactInfo::MIT_PHONE, contactPtr->GetPhone());

	return true;
}


// public methods of embedded class MetaInfo

QString CContactInfoMetaInfoCreatorComp::MetaInfo::GetMetaInfoName(int metaInfoType) const
{
	switch (metaInfoType){
	case IContactInfo::MIT_EMAIL:
		return QObject::tr("EMail");
	case IContactInfo::MIT_PHONE:
		return QObject::tr("Phone");
	}

	return BaseClass::GetMetaInfoName(metaInfoType);
}


} // namespace imtaccount


