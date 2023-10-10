#include <imtauth/CFeaturePackageRepresentationControllerComp.h>


// ImtCore includes
#include <imtlic/CFeatureContainer.h>


namespace imtauth
{


// public methods

// reimplemented (imtbase::CObjectRepresentationControllerCompBase)

bool CFeaturePackageRepresentationControllerComp::GetRepresentationFromValue(
			const istd::IChangeable& dataModel,
			imtbase::CTreeItemModel& representation,
			const iprm::IParamsSet* /*paramsPtr*/) const
{
	const imtlic::CFeatureContainer* packagePtr = dynamic_cast<const imtlic::CFeatureContainer*>(&dataModel);
	Q_ASSERT(packagePtr != nullptr);

	QByteArrayList featureCollectionIds = packagePtr->GetFeatureList().GetElementIds().toList();
	for (const QByteArray& featureCollectionId : featureCollectionIds){
		const imtlic::CFeatureInfo* featureInfoPtr = dynamic_cast<const imtlic::CFeatureInfo*>(packagePtr->GetFeatureInfo(featureCollectionId));
		if (featureInfoPtr != nullptr){
			istd::TDelPtr<imtbase::CTreeItemModel> featureModelPtr(new imtbase::CTreeItemModel);

			QString errorMessage;
			bool ok = CreateRepresentationModelFromFeatureInfo(*featureInfoPtr, *featureModelPtr, errorMessage);
			if (!ok){
				SendErrorMessage(0, errorMessage, "CObjectRepresentationControllerCompBase");

				return false;
			}

			int index = representation.InsertNewItem();
			representation.CopyItemDataFromModel(index, featureModelPtr.PopPtr(), 0);
			representation.SetData("Id", featureCollectionId, index);
		}
	}

	return true;
}


// reimplemented (IRepresentationController)

bool CFeaturePackageRepresentationControllerComp::IsModelSupported(const istd::IChangeable& dataModel) const
{
	const imtlic::CFeatureContainer* featureContainerPtr = dynamic_cast<const imtlic::CFeatureContainer*>(&dataModel);
	return featureContainerPtr != nullptr;
}


bool CFeaturePackageRepresentationControllerComp::GetDataModelFromRepresentation(
			const imtbase::CTreeItemModel& /*representation*/,
			istd::IChangeable& /*dataModel*/) const
{
	return false;
}


bool CFeaturePackageRepresentationControllerComp::GetRepresentationFromDataModel(const istd::IChangeable& dataModel, imtbase::CTreeItemModel& representation, const iprm::IParamsSet* paramsPtr) const
{
	return GetRepresentationFromValue(dataModel, representation, paramsPtr);
}


// protected methods

bool CFeaturePackageRepresentationControllerComp::CreateRepresentationModelFromFeatureInfo(
			const imtlic::CFeatureInfo& featureInfo,
			imtbase::CTreeItemModel& representationModel,
			QString& errorMessage) const
{
	QByteArray featureId = featureInfo.GetFeatureId();

	representationModel.SetData("FeatureId", featureId);
	representationModel.SetData("FeatureName", featureInfo.GetFeatureName());
	representationModel.SetData("Optional", featureInfo.IsOptional());
	representationModel.SetData("FeatureDescription", featureInfo.GetFeatureDescription());
	representationModel.SetData("Dependencies", featureInfo.GetDependencies().join(';'));
	representationModel.SetData("ChildModel", 0);

	const imtlic::FeatureInfoList& subFeatures = featureInfo.GetSubFeatures();
	if (!subFeatures.IsEmpty()){
		imtbase::CTreeItemModel* childModelPtr = representationModel.AddTreeModel("ChildModel");
		Q_ASSERT(childModelPtr != nullptr);

		for (int i = 0; i < subFeatures.GetCount(); i++){
			const imtlic::IFeatureInfo* featureInfoPtr = subFeatures.GetAt(i);
			if (featureInfoPtr == nullptr){
				errorMessage = QString("Unable to create representation model for invalid subfeature. Parent feature id: %1.").arg(qPrintable(featureId));
				return false;
			}

			const imtlic::CFeatureInfo* subFeatureInfoPtr = dynamic_cast<const imtlic::CFeatureInfo*>(featureInfoPtr);
			Q_ASSERT(subFeatureInfoPtr != nullptr);

			imtbase::CTreeItemModel subFeatureRepresentationModel;
			bool ok = CreateRepresentationModelFromFeatureInfo(*subFeatureInfoPtr, subFeatureRepresentationModel, errorMessage);
			if (!ok){
				return false;
			}

			childModelPtr->InsertNewItem();
			childModelPtr->CopyItemDataFromModel(i, &subFeatureRepresentationModel, 0);
		}
	}

	return true;
}


} // namespace imtauth


