#include <imtauth/CFeaturePackageRepresentationControllerComp.h>


namespace imtauth
{


// public methods

// reimplemented (IRepresentationController)

bool CFeaturePackageRepresentationControllerComp::IsModelSupported(const istd::IChangeable& dataModel) const
{
	const imtlic::IFeaturePackage* featurePackagePtr = dynamic_cast<const imtlic::IFeaturePackage*>(&dataModel);
	const imtlic::IFeatureDependenciesManager* dependenciesManagerPtr = dynamic_cast<const imtlic::IFeatureDependenciesManager*>(&dataModel);

	return (featurePackagePtr != nullptr) && (dependenciesManagerPtr != nullptr);
}


bool CFeaturePackageRepresentationControllerComp::GetRepresentationFromDataModel(const istd::IChangeable& dataModel, imtbase::CTreeItemModel& representation, const iprm::IParamsSet* /*paramsPtr*/) const
{
	if (!IsModelSupported(dataModel)){
		return false;
	}

	const imtlic::IFeaturePackage* packagePtr = dynamic_cast<const imtlic::IFeaturePackage*>(&dataModel);
	const imtlic::IFeatureDependenciesManager* dependenciesManagerPtr = dynamic_cast<const imtlic::IFeatureDependenciesManager*>(&dataModel);
	if (packagePtr != nullptr && dependenciesManagerPtr != nullptr){
		QByteArray packageId = packagePtr->GetPackageId();
		representation.SetData("Id", packageId);

		imtbase::CTreeItemModel* permissionsModelPtr = representation.AddTreeModel("Features");
		imtbase::CTreeItemModel* dependenciesModelPtr = representation.AddTreeModel("Dependencies");

		QByteArrayList featureCollectionIds = packagePtr->GetFeatureList().GetElementIds().toList();
		for (const QByteArray& featureCollectionId : featureCollectionIds){
			const imtlic::IFeatureInfo* featureInfoPtr = packagePtr->GetFeatureInfo(featureCollectionId);
			if (featureInfoPtr != nullptr){
				int index = permissionsModelPtr->InsertNewItem();

				QByteArray featureId = featureInfoPtr->GetFeatureId();
				QString featureName = featureInfoPtr->GetFeatureName();

				permissionsModelPtr->SetData("Id", featureId, index);
				permissionsModelPtr->SetData("Name", featureName, index);

				QByteArrayList featureDependenciesList = dependenciesManagerPtr->GetFeatureDependencies(featureId);
				if (!featureDependenciesList.isEmpty()){
					QByteArray dependencies = featureDependenciesList.join(';');
					dependenciesModelPtr->SetData(featureId, dependencies);
				}

				const imtlic::FeatureInfoList& subFeatures = featureInfoPtr->GetSubFeatures();
				if (subFeatures.GetCount() > 0){
					imtbase::CTreeItemModel* childModelPtr = permissionsModelPtr->AddTreeModel("ChildModel", index);
					InsertSubFeaturesToModelFromData(*dependenciesManagerPtr, subFeatures, *childModelPtr, *dependenciesModelPtr);
				}
			}
		}

		return true;
	}

	return false;
}


bool CFeaturePackageRepresentationControllerComp::GetDataModelFromRepresentation(const imtbase::CTreeItemModel& representation, istd::IChangeable& dataModel) const
{
	return false;
}


// private methods

void CFeaturePackageRepresentationControllerComp::InsertSubFeaturesToModelFromData(
		const imtlic::IFeatureDependenciesManager& dependenciesManager,
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

			QByteArrayList featureDependenciesList = dependenciesManager.GetFeatureDependencies(featureId);
			if (!featureDependenciesList.isEmpty()){
				QByteArray dependencies = featureDependenciesList.join(';');
				dependenciesModel.SetData(featureId, dependencies);
			}

			const imtlic::FeatureInfoList& subFeatures = featureInfoPtr->GetSubFeatures();
			imtbase::CTreeItemModel* childModelPtr = subFeaturesModel.AddTreeModel("ChildModel", index);

			InsertSubFeaturesToModelFromData(dependenciesManager, subFeatures, *childModelPtr, dependenciesModel);
		}
	}
}


} // namespace imtauth


