#include <imtauth/CAccountInfoMetaInfo.h>


// ImtCore includes
#include <imtauth/IAccountInfo.h>


namespace imtauth
{


QString CAccountInfoMetaInfo::GetMetaInfoName(int metaInfoType) const
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


