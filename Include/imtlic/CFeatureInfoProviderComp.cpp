#include <imtlic/CFeatureInfoProviderComp.h>

// ImtCore includes
#include <imtlic/CFeaturePackage.h>


namespace imtlic
{


const IFeatureInfo* CFeatureInfoProviderComp::GetFeatureInfo(const QByteArray &featureId) const
{
	if (!m_packageCollectionCompPtr.IsValid()){
		return nullptr;
	}

	imtbase::ICollectionInfo::Ids packageIds = m_packageCollectionCompPtr->GetElementIds();
	for (const imtbase::ICollectionInfo::Id& packageId : packageIds){
		imtbase::IObjectCollection::DataPtr dataPtr;
		if (m_packageCollectionCompPtr->GetObjectData(packageId, dataPtr)){
			const imtlic::CFeaturePackage* packagePtr  = dynamic_cast<const imtlic::CFeaturePackage*>(dataPtr.GetPtr());
			if (packagePtr != nullptr){
				const imtlic::IFeatureInfo* featureInfoPtr = packagePtr->FindFeatureById(featureId);
				if (featureInfoPtr != nullptr){
					return dynamic_cast<const imtlic::IFeatureInfo*>(featureInfoPtr->CloneMe());
				}
			}
		}
	}

	return nullptr;
}


const imtbase::IObjectCollection *CFeatureInfoProviderComp::GetFeaturePackages() const
{
	return nullptr;
}


const imtbase::ICollectionInfo &CFeatureInfoProviderComp::GetFeatureList() const
{
	return imtbase::CCollectionInfo();
}


const IFeatureDependenciesProvider *CFeatureInfoProviderComp::GetDependenciesInfoProvider() const
{
	return nullptr;
}


const imtbase::ICollectionInfo *CFeatureInfoProviderComp::GetParentFeatureInfoProviderList() const
{
	return nullptr;
}


const imtlic::IFeatureInfoProvider *CFeatureInfoProviderComp::GetParentFeatureInfoProvider(const QByteArray &parentId) const
{
	return nullptr;
}


} // namespace imtlic


