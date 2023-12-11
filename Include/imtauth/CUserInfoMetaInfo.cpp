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
	}

	return BaseClass::GetMetaInfoName(metaInfoType);
}


} // namespace imtauth


