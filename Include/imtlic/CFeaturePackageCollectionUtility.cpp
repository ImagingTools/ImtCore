#include <imtlic/CFeaturePackageCollectionUtility.h>


// ImtCore includes
#include <imtbase/IObjectCollection.h>
#include <imtlic/IFeatureInfo.h>
#include <imtlic/IFeatureDependenciesProvider.h>
#include <imtlic/IFeaturePackage.h>


namespace imtlic
{


// public methods

QByteArrayList CFeaturePackageCollectionUtility::GetPackageIds(const imtbase::IObjectCollection& packageCollection)
{
	return packageCollection.GetElementIds().toList();
}


const IFeaturePackage* CFeaturePackageCollectionUtility::GetPackagePtr(const imtbase::IObjectCollection& packageCollection, const QByteArray& packageId)
{
	IFeaturePackage* packagePtr = dynamic_cast<IFeaturePackage*>(
				const_cast<istd::IChangeable*>(packageCollection.GetObjectPtr(packageId)));

	return packagePtr;
}


QString CFeaturePackageCollectionUtility::GetPackageName(const imtbase::IObjectCollection& packageCollection, const QByteArray& packageId)
{
	QByteArrayList packageIds = GetPackageIds(packageCollection);
	if (packageIds.contains(packageId)){
		return packageCollection.GetElementInfo(packageId, imtbase::ICollectionInfo::EIT_NAME).toString();
	}

	return QString();
}


CFeaturePackageCollectionUtility::FeatureDependencyMap CFeaturePackageCollectionUtility::GetDependencies(const imtbase::IObjectCollection& packageCollection)
{
	FeatureDependencyMap featureDependencyMap;

	imtbase::ICollectionInfo::Ids packageCollectionIds = packageCollection.GetElementIds();
	for (const QByteArray& packageCollectionId : packageCollectionIds){
		const imtlic::IFeaturePackage* packagePtr = GetPackagePtr(packageCollection, packageCollectionId);
		if (packagePtr != nullptr){
			const imtlic::IFeatureDependenciesProvider* dependenciesProvider = packagePtr->GetDependenciesInfoProvider();
			if (dependenciesProvider != nullptr){
				imtbase::ICollectionInfo::Ids featureCollectionIds = packagePtr->GetFeatureList().GetElementIds();
				for (const QByteArray& featureCollectionId : featureCollectionIds){
					const imtlic::IFeatureInfo* featurePtr = packagePtr->GetFeatureInfo(featureCollectionId);
					if (featurePtr != nullptr){
						QByteArray featureId = featurePtr->GetFeatureId();

						featureDependencyMap[featureId] = dependenciesProvider->GetFeatureDependencies(featureId);
					}
				}
			}
		}
	}

	return featureDependencyMap;
}


bool CFeaturePackageCollectionUtility::HasDependency(const CFeaturePackageCollectionUtility::FeatureDependencyMap& dependencyMap, const QByteArray& fromFeatureId, const QByteArray& toFeatureId)
{
	if (dependencyMap.contains(fromFeatureId)){
		QByteArrayList featureIds = dependencyMap.value(fromFeatureId);

		if (featureIds.contains(toFeatureId)){
			return true;
		}

		for (const QByteArray& featureId : featureIds){
			if (HasDependency(dependencyMap, featureId, toFeatureId)){
				return true;
			}
		}
	}

	return false;
}


QByteArrayList CFeaturePackageCollectionUtility::GetFeatureIds(const imtbase::IObjectCollection& packageCollection, const QByteArray& packageId)
{
	QByteArrayList retVal;
	const IFeaturePackage* packagePtr =  GetPackagePtr(packageCollection, packageId);
	if (packagePtr != nullptr){
		QByteArrayList featureCollectionIds = packagePtr->GetFeatureList().GetElementIds().toList();
		for (const QByteArray& featureCollectionId : featureCollectionIds){
			retVal.append(packagePtr->GetFeatureInfo(featureCollectionId)->GetFeatureId());
		}
	}

	return retVal;
}


QByteArrayList CFeaturePackageCollectionUtility::GetAllFeatureIds(const imtbase::IObjectCollection& packageCollection)
{
	QByteArrayList retVal;

	QByteArrayList packageIds = GetPackageIds(packageCollection);
	for (const QByteArray& packageId : packageIds){
		const IFeaturePackage* packagePtr = GetPackagePtr(packageCollection, packageId);
		if (packagePtr != nullptr){
			QByteArrayList featureIds = packagePtr->GetFeatureList().GetElementIds().toList();
			for (const QByteArray& featureId : featureIds){
				const IFeatureInfo* featurePtr = packagePtr->GetFeatureInfo(featureId);
				retVal.append(featurePtr->GetFeatureId());
			}
		}
	}

	return retVal;
}


const IFeatureInfo* CFeaturePackageCollectionUtility::GetFeaturePtr(const imtbase::IObjectCollection& packageCollection, const QByteArray& featureId)
{
	QByteArrayList packageIds = GetPackageIds(packageCollection);
	for (const QByteArray& packageId : packageIds){
		const IFeaturePackage* packagePtr = GetPackagePtr(packageCollection, packageId);
		if (packagePtr != nullptr){
			QByteArrayList featureCollectionIds = packagePtr->GetFeatureList().GetElementIds().toList();
			for (const QByteArray& featureCollectionId : featureCollectionIds){
				const IFeatureInfo* featurePtr = packagePtr->GetFeatureInfo(featureCollectionId);
				if (featurePtr != nullptr){
					if (featurePtr->GetFeatureId() == featureId){
						return featurePtr;
					}
				}
			}
		}
	}

	return nullptr;
}


const IFeaturePackage* CFeaturePackageCollectionUtility::GetFeaturePackagePtr(const imtbase::IObjectCollection& packageCollection, const QByteArray& featureId)
{
	QByteArrayList packageIds = GetPackageIds(packageCollection);
	for (const QByteArray& packageId : packageIds){
		const IFeaturePackage* packagePtr = GetPackagePtr(packageCollection, packageId);
		if (packagePtr != nullptr){
			QByteArrayList featureCollectionIds = packagePtr->GetFeatureList().GetElementIds().toList();
			for (const QByteArray& featureCollectionId : featureCollectionIds){
				const IFeatureInfo* featurePtr = packagePtr->GetFeatureInfo(featureCollectionId);
				if (featurePtr != nullptr){
					if (featurePtr->GetFeatureId() == featureId){
						return packagePtr;
					}
				}
			}
		}
	}

	return nullptr;
}


} // namespace imtlic


