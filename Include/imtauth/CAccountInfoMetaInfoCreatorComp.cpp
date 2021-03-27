#include <imtauth/CAccountInfoMetaInfoCreatorComp.h>


// ACF includes
#include <imod/TModelWrap.h>

// ImtCore includes
#include <imtauth/IAccountInfo.h>


namespace imtauth
{


// protected methods

// reimplemented (imtbase::IMetaInfoCreator)

bool CAccountInfoMetaInfoCreatorComp::CreateMetaInfo(
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

	const IAccountInfo* accountPtr = dynamic_cast<const IAccountInfo*>(dataPtr);
	if (accountPtr == nullptr){
		return false;
	}

	metaInfoPtr->SetMetaInfo(IAccountInfo::MIT_ACCOUNT_TYPE, (int)accountPtr->GetAccountType());
	metaInfoPtr->SetMetaInfo(IAccountInfo::MIT_ACCOUNT_NAME, accountPtr->GetAccountName());
	metaInfoPtr->SetMetaInfo(IAccountInfo::MIT_ACCOUNT_DESCRIPTION, accountPtr->GetAccountDescription());

	IContactInfo* contactPtr = const_cast<IContactInfo*>(accountPtr->GetAccountOwner());
	if (contactPtr != nullptr){
		metaInfoPtr->SetMetaInfo(IAccountInfo::MIT_CONTACT_EMAIL, contactPtr->GetMail());
		metaInfoPtr->SetMetaInfo(IAccountInfo::MIT_CONTACT_BIRTHDAY, contactPtr->GetBirthday());
		metaInfoPtr->SetMetaInfo(IAccountInfo::MIT_CONTACT_FIRST_NAME, contactPtr->GetNameField(IContactInfo::NFT_FIRST_NAME));
		metaInfoPtr->SetMetaInfo(IAccountInfo::MIT_CONTACT_LAST_NAME, contactPtr->GetNameField(IContactInfo::NFT_LAST_NAME));
		metaInfoPtr->SetMetaInfo(IAccountInfo::MIT_CONTACT_NICKNAME, contactPtr->GetNameField(IContactInfo::NFT_NICKNAME));
	}

	return true;
}


// public methods of embedded class MetaInfo

QString CAccountInfoMetaInfoCreatorComp::MetaInfo::GetMetaInfoName(int metaInfoType) const
{
	switch (metaInfoType){
	case IAccountInfo::MIT_ACCOUNT_TYPE:
		return QObject::tr("Account Type");
	case IAccountInfo::MIT_ACCOUNT_NAME:
		return QObject::tr("Account Name");
	case IAccountInfo::MIT_ACCOUNT_DESCRIPTION:
		return QObject::tr("Description");
	case IAccountInfo::MIT_CONTACT_EMAIL:
		return QObject::tr("E-Mail");
	case IAccountInfo::MIT_CONTACT_BIRTHDAY:
		return QObject::tr("Birthday");
	case IAccountInfo::MIT_CONTACT_FIRST_NAME:
		return QObject::tr("First Name");
	case IAccountInfo::MIT_CONTACT_LAST_NAME:
		return QObject::tr("Last Name");
	case IAccountInfo::MIT_CONTACT_NICKNAME:
		return QObject::tr("Nickname");
	}

	return BaseClass::GetMetaInfoName(metaInfoType);
}


} // namespace imtauth


