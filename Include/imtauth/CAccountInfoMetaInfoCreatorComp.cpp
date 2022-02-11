#include <imtauth/CAccountInfoMetaInfoCreatorComp.h>


// ACF includes
#include <imod/TModelWrap.h>

// ImtCore includes
#include <imtauth/IAccountInfo.h>
#include <imtauth/CAccountInfoMetaInfo.h>


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

	metaInfoPtr.SetPtr(new imod::TModelWrap<CAccountInfoMetaInfo>);

	if (dataPtr == nullptr){
		return true;
	}

	const IAccountInfo* accountPtr = dynamic_cast<const IAccountInfo*>(dataPtr);
	if (accountPtr == nullptr){
		return false;
	}

	switch (accountPtr->GetAccountType()){
	case IAccountInfo::AT_PERSON:
		metaInfoPtr->SetMetaInfo(IAccountInfo::MIT_ACCOUNT_TYPE, QObject::tr("Private"));
		break;
	case IAccountInfo::AT_COMPANY:
		metaInfoPtr->SetMetaInfo(IAccountInfo::MIT_ACCOUNT_TYPE, QObject::tr("Company"));
		break;
	default:
		metaInfoPtr->SetMetaInfo(IAccountInfo::MIT_ACCOUNT_TYPE, QObject::tr("Unknown"));
		break;
	}

	metaInfoPtr->SetMetaInfo(IAccountInfo::MIT_ACCOUNT_NAME, accountPtr->GetAccountName());
	metaInfoPtr->SetMetaInfo(IAccountInfo::MIT_ACCOUNT_DESCRIPTION, accountPtr->GetAccountDescription());

	IContactInfo* contactPtr = const_cast<IContactInfo*>(accountPtr->GetAccountOwner());
	if (contactPtr != nullptr){
		metaInfoPtr->SetMetaInfo(IAccountInfo::MIT_CONTACT_EMAIL, contactPtr->GetMail());
		metaInfoPtr->SetMetaInfo(IAccountInfo::MIT_CONTACT_FIRST_NAME, contactPtr->GetNameField(IContactInfo::NFT_FIRST_NAME));
		metaInfoPtr->SetMetaInfo(IAccountInfo::MIT_CONTACT_LAST_NAME, contactPtr->GetNameField(IContactInfo::NFT_LAST_NAME));

		if (accountPtr->GetAccountType() == IAccountInfo::AT_PERSON){
			metaInfoPtr->SetMetaInfo(IAccountInfo::MIT_CONTACT_BIRTHDAY, contactPtr->GetBirthday());
			metaInfoPtr->SetMetaInfo(IAccountInfo::MIT_CONTACT_NICKNAME, contactPtr->GetNameField(IContactInfo::NFT_NICKNAME));
		}
	}

	return true;
}


} // namespace imtauth


