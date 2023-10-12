#include <imtlic/CLicenseInfoProviderComp.h>


// ImtCore includes
#include <imtbase/CCollectionInfo.h>
#include <imtlic/IProductLicensingInfo.h>
#include <imtlic/ILicenseDefinition.h>


namespace imtlic
{


// public methods

// reimplemented (ILicenseInfoProvider)

const imtbase::ICollectionInfo& CLicenseInfoProviderComp::GetLicenseList() const
{
	return imtbase::CCollectionInfo();
}


const imtlic::ILicenseDefinition* CLicenseInfoProviderComp::GetLicenseInfo(const QByteArray& licenseId) const
{
	imtbase::ICollectionInfo::Ids productIds = m_productCollectionCompPtr->GetElementIds();
	for (const imtbase::ICollectionInfo::Id& productId : productIds){
		imtbase::IObjectCollection::DataPtr dataPtr;
		if (m_productCollectionCompPtr->GetObjectData(productId, dataPtr)){
			const imtlic::IProductLicensingInfo* productPtr = dynamic_cast<const imtlic::IProductLicensingInfo*>(dataPtr.GetPtr());
			if (productPtr != nullptr){
				const imtbase::ICollectionInfo& licenseList = productPtr->GetLicenseList();
				const imtbase::IObjectCollectionInfo::Ids licenseCollectionIds = licenseList.GetElementIds();

				const imtlic::ILicenseDefinition* licenseInfoPtr = productPtr->GetLicenseInfo(licenseId);
				if (licenseInfoPtr != nullptr){
					return dynamic_cast<const imtlic::ILicenseDefinition*>(licenseInfoPtr->CloneMe());
				}
			}
		}
	}

	return nullptr;
}


} // namespace imtlic


