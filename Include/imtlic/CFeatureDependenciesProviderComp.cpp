#include <imtlic/CFeatureDependenciesProviderComp.h>


// ImtCore includes
#include <imtbase/ICollectionInfo.h>
#include <imtlic/CFeaturePackage.h>


namespace imtlic
{


QByteArrayList CFeatureDependenciesProviderComp::GetFeatureDependencies(const QByteArray& featureId) const
{
	if (!m_packageCollectionCompPtr.IsValid()){
		return QByteArrayList();
	}

	imtbase::ICollectionInfo::Ids packageIds = m_packageCollectionCompPtr->GetElementIds();
	for (const imtbase::ICollectionInfo::Id& packageId : packageIds){
		imtbase::IObjectCollection::DataPtr dataPtr;
		if (m_packageCollectionCompPtr->GetObjectData(packageId, dataPtr)){
			const imtlic::CFeaturePackage* packagePtr  = dynamic_cast<const imtlic::CFeaturePackage*>(dataPtr.GetPtr());
			if (packagePtr != nullptr){
				imtbase::ICollectionInfo::Ids featureIds = packagePtr->GetFeatureList().GetElementIds();

				if (featureIds.contains(featureId)){
					return packagePtr->GetFeatureDependencies(featureId);
				}
			}
		}
	}

	return QByteArrayList();
}


const IFeatureInfoProvider* CFeatureDependenciesProviderComp::GetDependencyContainer(const QByteArray& dependencyId) const
{
	return nullptr;
}


} // namespace imtlic


