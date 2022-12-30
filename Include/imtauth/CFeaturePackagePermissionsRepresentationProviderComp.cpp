#include <imtauth/CFeaturePackagePermissionsRepresentationProviderComp.h>


//ImtCore includes
#include <imtbase/ICollectionInfo.h>
#include <imtlic/IFeatureInfo.h>
#include <imtlic/CFeaturePackage.h>


namespace imtauth
{


QByteArray CFeaturePackagePermissionsRepresentationProviderComp::GetModelId() const
{
	return *m_modelIdAttrPtr;
}


bool CFeaturePackagePermissionsRepresentationProviderComp::IsModelSupported(const istd::IChangeable& dataModel) const
{
	const imtlic::IFeaturePackage* featurePackagePtr = dynamic_cast<const imtlic::IFeaturePackage*>(&dataModel);

	return (featurePackagePtr != nullptr);
}


bool CFeaturePackagePermissionsRepresentationProviderComp::GetRepresentationFromDataModel(const istd::IChangeable& dataModel, imtbase::CTreeItemModel& representation) const
{
	if (m_featurePackageCompPtr.IsValid()){
		imtbase::CTreeItemModel* permissionsModelPtr = representation.AddTreeModel("Permissions");
		imtbase::CTreeItemModel* dependenciesModelPtr = representation.AddTreeModel("Dependencies");

		const imtlic::CFeaturePackage* packagePtr = dynamic_cast<const imtlic::CFeaturePackage*>(m_featurePackageCompPtr.GetPtr());
		if (packagePtr != nullptr){
			QByteArrayList featureCollectionIds = packagePtr->GetFeatureList().GetElementIds().toList();

			for (const QByteArray& featureCollectionId : featureCollectionIds){
				const imtlic::IFeatureInfo* featureInfoPtr = packagePtr->GetFeatureInfo(featureCollectionId);
				if (featureInfoPtr != nullptr){
					int index = permissionsModelPtr->InsertNewItem();

					QByteArray featureId = featureInfoPtr->GetFeatureId();
					QString featureName = featureInfoPtr->GetFeatureName();

					permissionsModelPtr->SetData("Id", featureId, index);
					permissionsModelPtr->SetData("Name", featureName, index);

					QByteArrayList featureDependenciesList = packagePtr->GetFeatureDependencies(featureId);
					if (!featureDependenciesList.isEmpty()){
						QByteArray dependencies = featureDependenciesList.join(';');
						dependenciesModelPtr->SetData(featureId, dependencies);
					}

					const imtlic::FeatureInfoList& subFeatures = featureInfoPtr->GetSubFeatures();
					if (subFeatures.GetCount() > 0){
						imtbase::CTreeItemModel* childModelPtr = permissionsModelPtr->AddTreeModel("ChildModel", index);
						InsertSubFeaturesToModelFromData(packagePtr, subFeatures, *childModelPtr, *dependenciesModelPtr);
					}
				}
			}

			return true;
		}
	}

	return false;
}


bool CFeaturePackagePermissionsRepresentationProviderComp::GetDataModelFromRepresentation(const imtbase::CTreeItemModel& representation, istd::IChangeable& dataModel) const
{
	return false;
}


// private methods

void CFeaturePackagePermissionsRepresentationProviderComp::InsertSubFeaturesToModelFromData(
			const imtlic::CFeaturePackage* packagePtr,
			const imtlic::FeatureInfoList& subFeaturesList,
			imtbase::CTreeItemModel& subFeaturesModel,
			imtbase::CTreeItemModel& dependenciesModel) const
{
	for (int i = 0; i < subFeaturesList.GetCount(); ++i){
		const imtlic::IFeatureInfo* featureInfoPtr = subFeaturesList.GetAt(i);
		if (featureInfoPtr != nullptr){
			QByteArray featureId = featureInfoPtr->GetFeatureId();
			QString featureName = featureInfoPtr->GetFeatureName();

			int index = subFeaturesModel.InsertNewItem();

			subFeaturesModel.SetData("Id", featureId, index);
			subFeaturesModel.SetData("Name", featureName, index);

			QByteArrayList featureDependenciesList = packagePtr->GetFeatureDependencies(featureId);
			if (!featureDependenciesList.isEmpty()){
				QByteArray dependencies = featureDependenciesList.join(';');
				dependenciesModel.SetData(featureId, dependencies);
			}

			const imtlic::FeatureInfoList& subFeatures = featureInfoPtr->GetSubFeatures();
			if (subFeatures.GetCount() > 0){
				imtbase::CTreeItemModel* childModelPtr = subFeaturesModel.AddTreeModel("ChildModel", index);

				InsertSubFeaturesToModelFromData(packagePtr, subFeatures, *childModelPtr, dependenciesModel);
			}
		}
	}
}


} // namespace imtauth


