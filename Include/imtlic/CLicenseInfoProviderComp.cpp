// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
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
	static imtbase::CCollectionInfo retVal;

	return retVal;
}


istd::TUniqueInterfacePtr<imtlic::ILicenseDefinition> CLicenseInfoProviderComp::GetLicenseInfo(const QByteArray& licenseId) const
{
	imtbase::ICollectionInfo::Ids productIds = m_productCollectionCompPtr->GetElementIds();
	for (const imtbase::ICollectionInfo::Id& productId : productIds){
		imtbase::IObjectCollection::DataPtr dataPtr;
		if (m_productCollectionCompPtr->GetObjectData(productId, dataPtr)){
			const imtlic::IProductLicensingInfo* productPtr = dynamic_cast<const imtlic::IProductLicensingInfo*>(dataPtr.GetPtr());
			if (productPtr != nullptr){
				istd::TUniqueInterfacePtr<imtlic::ILicenseDefinition> licenseInfoPtr = productPtr->GetLicenseInfo(licenseId);
				if (licenseInfoPtr.IsValid()){
					istd::TUniqueInterfacePtr<imtlic::ILicenseDefinition> retVal;

					retVal.MoveCastedPtr(licenseInfoPtr->CloneMe());

					return retVal;
				}
			}
		}
	}

	return nullptr;
}


} // namespace imtlic


