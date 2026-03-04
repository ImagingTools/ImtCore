// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
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

	default:
		break;
	}

	return BaseClass::GetMetaInfoName(metaInfoType);
}


} // namespace imtauth


