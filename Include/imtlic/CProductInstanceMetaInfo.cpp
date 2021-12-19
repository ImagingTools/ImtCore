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
	}

	return BaseClass::GetMetaInfoName(metaInfoType);
}


} // namespace imtlic


