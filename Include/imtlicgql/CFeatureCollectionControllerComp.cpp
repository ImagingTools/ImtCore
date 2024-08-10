#include <imtlicgql/CFeatureCollectionControllerComp.h>


// ImtCore includes
#include <imtlic/CFeatureInfo.h>


namespace imtlicgql
{


// protected methods

// reimplemented (imtgql::CObjectCollectionControllerCompBase)

bool CFeatureCollectionControllerComp::SetupGqlItem(
			const imtgql::CGqlRequest& gqlRequest,
			imtbase::CTreeItemModel& model,
			int itemIndex,
			const imtbase::IObjectCollectionIterator* objectCollectionIterator,
			QString& /*errorMessage*/) const
{
	if (objectCollectionIterator == nullptr){
		return false;
	}

	bool retVal = true;

	QByteArrayList informationIds = GetInformationIds(gqlRequest, "items");

	QByteArray objectId = objectCollectionIterator->GetObjectId();

	if (!informationIds.isEmpty() && m_objectCollectionCompPtr.IsValid()){
		const imtlic::CFeatureInfo* featureInfoPtr = nullptr;

		imtbase::IObjectCollection::DataPtr dataPtr;
		if (objectCollectionIterator->GetObjectData(dataPtr)){
			featureInfoPtr = dynamic_cast<const imtlic::CFeatureInfo*>(dataPtr.GetPtr());
		}

		if (featureInfoPtr != nullptr){
			for (const QByteArray& informationId : informationIds){
				QVariant elementInformation;

				if(informationId == "Id"){
					elementInformation = objectId;
				}
				else if(informationId == "TypeId"){
					elementInformation = m_objectCollectionCompPtr->GetObjectTypeId(objectId);
				}
				else if(informationId == "FeatureId"){
					elementInformation = featureInfoPtr->GetFeatureId();
				}
				else if(informationId == "FeatureName" || informationId == "Name"){
					elementInformation = featureInfoPtr->GetFeatureName();
				}
				else if(informationId == "FeatureDescription" || informationId == "Description"){
					elementInformation = featureInfoPtr->GetFeatureDescription();
				}
				else if(informationId == "Optional"){
					elementInformation = featureInfoPtr->IsOptional();
				}
				else if(informationId == "Dependencies"){
					elementInformation = featureInfoPtr->GetDependencies().join(';');
				}
				else if(informationId == "ChildModel"){
					imtbase::CTreeItemModel* representationModelPtr = model.AddTreeModel("ChildModel", itemIndex);

					CreateChildModelRepresentation(*featureInfoPtr, *representationModelPtr);

					continue;
				}
				else if(informationId == "Added"){
					QDateTime addedTime =  objectCollectionIterator->GetElementInfo("Added").toDateTime();
					addedTime.setTimeSpec(Qt::UTC);

					elementInformation = addedTime.toLocalTime().toString("dd.MM.yyyy hh:mm:ss");
				}
				else if(informationId == "LastModified"){
					QDateTime lastTime =  objectCollectionIterator->GetElementInfo("LastModified").toDateTime();
					lastTime.setTimeSpec(Qt::UTC);

					elementInformation = lastTime.toLocalTime().toString("dd.MM.yyyy hh:mm:ss");
				}

				if (elementInformation.isNull()){
					elementInformation = "";
				}

				retVal = retVal && model.SetData(informationId, elementInformation, itemIndex);
			}

			return true;
		}
	}

	return false;
}


bool CFeatureCollectionControllerComp::CreateChildModelRepresentation(
			const imtlic::CFeatureInfo& featureInfo,
			imtbase::CTreeItemModel& representationModel) const
{
	const imtlic::FeatureInfoList& subFeatures = featureInfo.GetSubFeatures();

	for (int i = 0; i < subFeatures.GetCount(); i++){
		const imtlic::IFeatureInfo* featureInfoPtr = subFeatures.GetAt(i);
		if (featureInfoPtr == nullptr){
			return false;
		}

		int index = representationModel.InsertNewItem();

		representationModel.SetData("FeatureId", featureInfoPtr->GetFeatureId(), index);
		representationModel.SetData("FeatureName", featureInfoPtr->GetFeatureName(), index);
		representationModel.SetData("Optional", featureInfoPtr->IsOptional(), index);
		representationModel.SetData("FeatureDescription", featureInfoPtr->GetFeatureDescription(), index);
		representationModel.SetData("Dependencies", featureInfoPtr->GetDependencies().join(';'), index);
		representationModel.SetData("ChildModel", 0, index);

		const imtlic::FeatureInfoList& features = featureInfoPtr->GetSubFeatures();
		if (!features.IsEmpty()){
			const imtlic::CFeatureInfo* subFeatureInfoPtr = dynamic_cast<const imtlic::CFeatureInfo*>(featureInfoPtr);
			Q_ASSERT(subFeatureInfoPtr != nullptr);

			imtbase::CTreeItemModel* childModelPtr = representationModel.AddTreeModel("ChildModel", index);
			bool ok = CreateChildModelRepresentation(*subFeatureInfoPtr, *childModelPtr);
			if (!ok){
				return false;
			}
		}
	}

	return true;
}


} // namespace imtlicgql


