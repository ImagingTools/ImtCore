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

	const IAccountInfo* accountInfoPtr = dynamic_cast<const IAccountInfo*>(dataPtr);
	if (accountInfoPtr == nullptr){
		return false;
	}

	metaInfoPtr->SetMetaInfo(IAccountInfo::MIT_CONTACT_EMAIL, accountInfoPtr->GetAccountOwnerEMail());
	metaInfoPtr->SetMetaInfo(IAccountInfo::MIT_ACCOUNT_TYPE, (int)accountInfoPtr->GetAccountType());
	metaInfoPtr->SetMetaInfo(IAccountInfo::MIT_ACCOUNT_NAME, accountInfoPtr->GetAccountName());
	metaInfoPtr->SetMetaInfo(IAccountInfo::MIT_ACCOUNT_DESCRIPTION, accountInfoPtr->GetAccountDescription());

	return true;
}


// public methods of embedded class MetaInfo

QString CAccountInfoMetaInfoCreatorComp::MetaInfo::GetMetaInfoName(int metaInfoType) const
{
	switch (metaInfoType){
	case IAccountInfo::MIT_CONTACT_EMAIL:
		return QObject::tr("Contact EMail");
	case IAccountInfo::MIT_ACCOUNT_TYPE:
		return QObject::tr("Account Type");
	case IAccountInfo::MIT_ACCOUNT_NAME:
		return QObject::tr("Account Name");
	case IAccountInfo::MIT_ACCOUNT_DESCRIPTION:
		return QObject::tr("Account Description");
	}

	return BaseClass::GetMetaInfoName(metaInfoType);
}


} // namespace imtauth


