#include <imtauth/CUserInfoMetaInfo.h>


// ImtCore includes
#include <imtauth/IUserInfo.h>


namespace imtauth
{


QString CUserInfoMetaInfo::GetMetaInfoName(int metaInfoType) const
{
	switch (metaInfoType){
	case IUserInfo::MetaInfoTypes::MIT_EMAIL:
		return QObject::tr("Email");
	case IUserInfo::MetaInfoTypes::MIT_USERNAME:
		return QObject::tr("Username");
	case IUserInfo::MetaInfoTypes::MIT_NAME:
		return QObject::tr("Name");
	case IUserInfo::MetaInfoTypes::MIT_DESCRIPTION:
		return QObject::tr("Description");
	}

	return BaseClass::GetMetaInfoName(metaInfoType);
}


} // namespace imtauth


