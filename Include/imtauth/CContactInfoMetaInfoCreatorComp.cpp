#include <imtauth/CContactInfoMetaInfoCreatorComp.h>


// ACF includes
#include <imod/TModelWrap.h>
#include <idoc/CStandardDocumentMetaInfo.h>

// ImtCore includes
#include <imtauth/IContactInfo.h>


namespace imtauth
{


// protected methods

// reimplemented (imtbase::IMetaInfoCreator)

bool CContactInfoMetaInfoCreatorComp::CreateMetaInfo(
			const istd::IChangeable* dataPtr,
			const QByteArray& typeId,
			MetaInfoPtr& metaInfoPtr) const
{
	if (typeId != *m_objectTypeIdAttrPtr){
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

	metaInfoPtr->SetMetaInfo(IContactInfo::MIT_EMAIL, contactPtr->GetEMail());
	metaInfoPtr->SetMetaInfo(IContactInfo::MIT_GENDER_TYPE, contactPtr->GetGenderType());
	metaInfoPtr->SetMetaInfo(IContactInfo::MIT_BIRTHDAY, contactPtr->GetBirthday());
	metaInfoPtr->SetMetaInfo(IContactInfo::MIT_FIRST_NAME, contactPtr->GetNameField(IContactInfo::NFT_FIRST_NAME));
	metaInfoPtr->SetMetaInfo(IContactInfo::MIT_LAST_NAME, contactPtr->GetNameField(IContactInfo::NFT_LAST_NAME));
	metaInfoPtr->SetMetaInfo(IContactInfo::MIT_NICKNAME, contactPtr->GetNameField(IContactInfo::NFT_NICKNAME));

	return true;
}


// public methods of embedded class MetaInfo

QString CContactInfoMetaInfoCreatorComp::MetaInfo::GetMetaInfoName(int metaInfoType) const
{
	switch (metaInfoType){
	case IContactInfo::MIT_EMAIL:
		return QObject::tr("EMail");
	case IContactInfo::MIT_GENDER_TYPE:
		return QObject::tr("Gender");
	case IContactInfo::MIT_BIRTHDAY:
		return QObject::tr("Birthday");
	case IContactInfo::MIT_FIRST_NAME:
		return QObject::tr("First Name");
	case IContactInfo::MIT_LAST_NAME:
		return QObject::tr("Last Name");
	case IContactInfo::MIT_NICKNAME:
		return QObject::tr("Nickname");
	}

	return BaseClass::GetMetaInfoName(metaInfoType);
}


} // namespace imtauth


