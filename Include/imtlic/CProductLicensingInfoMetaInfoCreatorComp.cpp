#include <imtlic/CProductLicensingInfoMetaInfoCreatorComp.h>


// ACF includes
#include <imod/TModelWrap.h>

// ImtCore includes
#include <imtbase/ICollectionInfo.h>
#include <imtlic/ILicenseInfo.h>
#include <imtlic/IProductLicensingInfo.h>
#include <imtlic/IProductLicensingInfoProvider.h>


namespace imtlic
{


// protected methods

// reimplemented (imtbase::IMetaInfoCreator)


bool CProductLicensingInfoMetaInfoCreatorComp::CreateMetaInfo(
			const istd::IChangeable* dataPtr,
			const QByteArray& typeId,
			idoc::MetaInfoPtr& metaInfoPtr) const
{
	if (typeId != *m_objectTypeIdAttrPtr){
		return false;
	}

	metaInfoPtr.SetPtr(new imod::TModelWrap<MetaInfo>);

	if (dataPtr == nullptr){
		return true;
	}

	const imtlic::IProductLicensingInfo* productLicensingInfoPtr = dynamic_cast<const imtlic::IProductLicensingInfo*>(dataPtr);
	if (productLicensingInfoPtr == nullptr){
		return false;
	}

	QString retVal;

	imtbase::ICollectionInfo::Ids ids = productLicensingInfoPtr->GetLicenseList().GetElementIds();
	for (imtbase::ICollectionInfo::Id id : ids){
		const imtlic::ILicenseInfo* licenseInfoPtr = productLicensingInfoPtr->GetLicenseInfo(id);
		retVal += licenseInfoPtr->GetLicenseName() + " (" + licenseInfoPtr->GetLicenseId() + ")" + "\n";
	}

	retVal.chop(1);

	metaInfoPtr->SetMetaInfo(IProductLicensingInfoProvider::MIT_LICENSES_INFO_LIST, retVal);
	metaInfoPtr->SetMetaInfo(IProductLicensingInfo::MIT_PRODUCT_NAME, productLicensingInfoPtr->GetName());
	metaInfoPtr->SetMetaInfo(IProductLicensingInfo::MIT_PRODUCT_CATEGORY_ID, productLicensingInfoPtr->GetCategoryId());

	return true;
}


// public methods of embedded class MetaInfo

QString CProductLicensingInfoMetaInfoCreatorComp::MetaInfo::GetMetaInfoName(int metaInfoType) const
{
	switch (metaInfoType){
	case imtlic::IProductLicensingInfoProvider::MIT_LICENSES_INFO_LIST:
		return QObject::tr("Licenses");
	}

	return BaseClass::GetMetaInfoName(metaInfoType);
}


} // namespace imtlic


