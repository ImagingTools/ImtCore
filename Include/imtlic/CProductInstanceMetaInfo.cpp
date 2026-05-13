// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtlic/CProductInstanceMetaInfo.h>


// ImtCore includes
#include <imtlic/IProductInstanceInfo.h>
#include <imtlic/IProductInstanceInfoProvider.h>


namespace imtlic
{


QString CProductInstanceMetaInfo::GetMetaInfoName(int metaInfoType) const
{
	switch (metaInfoType){
	case imtlic::IProductInstanceInfoProvider::MIT_LICENSE_INSTANCE_INFO_LIST:
		return QObject::tr("Licenses");
	case imtlic::IProductInstanceInfo::MIT_PRODUCT_INSTANCE_ID:
		return QObject::tr("Instance-ID");
	case imtlic::IProductInstanceInfo::MIT_PRODUCT_CATEGORY_ID:
		return QObject::tr("Category-ID");
	}

	return BaseClass::GetMetaInfoName(metaInfoType);
}


} // namespace imtlic


