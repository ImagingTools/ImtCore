#include <imtauth/CCollectionPermissionsProviderComp.h>

//ImtCore includes
#include <imtlic/IProductLicensingInfo.h>


namespace imtauth
{


const QByteArrayList CCollectionPermissionsProviderComp::GetPermissionInfos(const QByteArray& productId) const
{
//	imtlic::ILicenseDefinition::FeatureInfos featureInfos;

//	if (m_productCollectionCompPtr.IsValid()){
//		imtbase::IObjectCollection::DataPtr productDataPtr;
//		if (m_productCollectionCompPtr->GetObjectData(productId, productDataPtr)){
//			const imtlic::IProductLicensingInfo* productPtr = dynamic_cast<const imtlic::IProductLicensingInfo*>(productDataPtr.GetPtr());

//			const imtbase::ICollectionInfo& licenseList = productPtr->GetLicenseList();
//			const imtbase::IObjectCollectionInfo::Ids licenseCollectionIds = licenseList.GetElementIds();

//			for (const QByteArray& licenseId : licenseCollectionIds){
//				const imtlic::ILicenseDefinition* licenseInfoPtr = productPtr->GetLicenseInfo(licenseId);
//				if (licenseInfoPtr != nullptr){
//					featureInfos += licenseInfoPtr->GetFeatureInfos();
//				}
//			}
//		}
//	}

	return QByteArrayList();
}


} // namespace imtauth
