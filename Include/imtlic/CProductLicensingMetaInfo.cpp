#include <imtlic/CProductLicensingMetaInfo.h>


// ImtCore includes
#include <imtlic/IProductLicensingInfo.h>


namespace imtlic
{


QString CProductLicensingMetaInfo::GetMetaInfoName(int metaInfoType) const
{
	switch (metaInfoType){
	case imtlic::IProductLicensingInfo::MIT_PRODUCT_NAME:
		return QObject::tr("Product");
	case imtlic::IProductLicensingInfo::MIT_PRODUCT_CATEGORY_ID:
		return QObject::tr("Category-ID");
	}

	return BaseClass::GetMetaInfoName(metaInfoType);
}


} // namespace imtlic


