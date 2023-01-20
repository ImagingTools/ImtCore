#include <imtlic/CProductMetaInfoCreatorComp.h>


// ACF includes
#include <imod/TModelWrap.h>

// ImtCore includes
#include <imtbase/ICollectionInfo.h>
#include <imtlic/ILicenseInstance.h>
#include <imtlic/IProductLicensingInfo.h>
#include <imtlic/IProductInstanceInfo.h>
//#include <imtlic/IProductInstanceInfoProvider.h>
#include <imtlic/CProductLicensingMetaInfo.h>


namespace imtlic
{


// protected methods

// reimplemented (imtbase::IMetaInfoCreator)

bool CProductMetaInfoCreatorComp::CreateMetaInfo(
			const istd::IChangeable* dataPtr,
			const QByteArray& typeId,
			idoc::MetaInfoPtr& metaInfoPtr) const
{
	if (typeId != *m_objectTypeIdAttrPtr){
		return false;
	}

	metaInfoPtr.SetPtr(new imod::TModelWrap<CProductLicensingMetaInfo>);

	if (dataPtr == nullptr){
		return true;
	}

	const imtlic::IProductLicensingInfo* productLicensingPtr = dynamic_cast<const imtlic::IProductLicensingInfo*>(dataPtr);
	if (productLicensingPtr == nullptr){
		return false;
	}

	metaInfoPtr->SetMetaInfo(IProductLicensingInfo::MIT_PRODUCT_NAME, productLicensingPtr->GetName());
//	metaInfoPtr->SetMetaInfo(IProductLicensingInfo::MIT_PRODUCT_PRODUCT_ID, productLicensingPtr->GetProductId());
	metaInfoPtr->SetMetaInfo(IProductLicensingInfo::MIT_PRODUCT_CATEGORY_ID, productLicensingPtr->GetCategoryId());

	return true;
}


} // namespace imtlic


