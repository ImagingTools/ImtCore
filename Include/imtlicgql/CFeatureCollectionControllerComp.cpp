#include <imtlicgql/CFeatureCollectionControllerComp.h>


// ImtCore includes
#include <imtlic/CFeatureInfo.h>


namespace imtlicgql
{


// protected methods

// reimplemented (sdl::imtlic::Features::CFeatureCollectionControllerCompBase)

bool CFeatureCollectionControllerComp::CreateRepresentationFromObject(
			const imtbase::IObjectCollectionIterator& objectCollectionIterator,
			const sdl::imtlic::Features::CFeaturesListGqlRequest& featuresListRequest,
			sdl::imtlic::Features::CFeatureItem& representationObject,
			QString& errorMessage) const
{
	if (!m_objectCollectionCompPtr.IsValid()){
		errorMessage = QString("Unable to create representation from object. Error: Attribute 'm_objectCollectionCompPtr' was not set");
		SendErrorMessage(0, errorMessage, "CFeatureCollectionControllerComp");

		return false;
	}

	QByteArray objectId = objectCollectionIterator.GetObjectId();

	imtlic::CIdentifiableFeatureInfo* featureInfoPtr = nullptr;
	imtbase::IObjectCollection::DataPtr dataPtr;
	if (objectCollectionIterator.GetObjectData(dataPtr)){
		featureInfoPtr = dynamic_cast<imtlic::CIdentifiableFeatureInfo*>(dataPtr.GetPtr());
	}

	if (featureInfoPtr == nullptr){
		errorMessage = QString("Unable to create representation from object '%1'").arg(qPrintable(objectId));
		SendErrorMessage(0, errorMessage, "CFeatureCollectionControllerComp");

		return false;
	}

	idoc::MetaInfoPtr metaInfo = objectCollectionIterator.GetDataMetaInfo();

	sdl::imtlic::Features::FeaturesListRequestInfo requestInfo = featuresListRequest.GetRequestInfo();

	if (requestInfo.items.isIdRequested){
		representationObject.SetId(objectId);
	}

	if (requestInfo.items.isFeatureIdRequested){
		representationObject.SetFeatureId(featureInfoPtr->GetFeatureId());
	}

	if (requestInfo.items.isNameRequested){
		representationObject.SetName(featureInfoPtr->GetFeatureName());
	}

	if (requestInfo.items.isDescriptionRequested){
		representationObject.SetDescription(featureInfoPtr->GetFeatureDescription());
	}

	if (requestInfo.items.isOptionalRequested){
		representationObject.SetOptional(featureInfoPtr->IsOptional());
	}

	if (requestInfo.items.isDependenciesRequested){
		representationObject.SetDependencies(featureInfoPtr->GetDependencies().join(';'));
	}

	if (requestInfo.items.isAddedRequested){
		QDateTime addedTime = objectCollectionIterator.GetElementInfo("Added").toDateTime();
		addedTime.setTimeSpec(Qt::UTC);

		QString added = addedTime.toLocalTime().toString("dd.MM.yyyy hh:mm:ss");
		representationObject.SetAdded(added);
	}

	if (requestInfo.items.isLastModifiedRequested){
		QDateTime lastModifiedTime = objectCollectionIterator.GetElementInfo("LastModified").toDateTime();
		lastModifiedTime.setTimeSpec(Qt::UTC);

		QString lastModified = lastModifiedTime.toLocalTime().toString("dd.MM.yyyy hh:mm:ss");
		representationObject.SetLastModified(lastModified);
	}

	return true;
}


istd::IChangeable* CFeatureCollectionControllerComp::CreateObjectFromRepresentation(
			const sdl::imtlic::Features::CFeatureData& featureDataRepresentation,
			QByteArray& newObjectId,
			QString& name,
			QString& description,
			QString& errorMessage) const
{
	if (!m_featureInfoFactCompPtr.IsValid()){
		errorMessage = QString("Unable to create object from representation. Error: Attribute 'm_featureInfoFactCompPtr' was not set");
		SendErrorMessage(0, errorMessage, "CFeatureCollectionControllerComp");

		return nullptr;
	}

	istd::TDelPtr<imtlic::IFeatureInfo> featureInstancePtr = m_featureInfoFactCompPtr.CreateInstance();
	if (!featureInstancePtr.IsValid()){
		errorMessage = QString("Unable to create feature instance. Error: Invalid object");
		SendErrorMessage(0, errorMessage, "CFeatureCollectionControllerComp");

		return nullptr;
	}

	imtlic::CIdentifiableFeatureInfo* featureInfoPtr = dynamic_cast<imtlic::CIdentifiableFeatureInfo*>(featureInstancePtr.GetPtr());
	if (featureInfoPtr == nullptr){
		errorMessage = QString("Unable to cast feature instance to identifable object. Error: Invalid object");
		SendErrorMessage(0, errorMessage, "CFeatureCollectionControllerComp");

		return nullptr;
	}

	newObjectId = featureDataRepresentation.GetId();
	featureInfoPtr->SetObjectUuid(newObjectId);

	name = featureDataRepresentation.GetName();
	featureInfoPtr->SetFeatureName(name);

	description = featureDataRepresentation.GetDescription();
	featureInfoPtr->SetFeatureDescription(description);

	return featureInstancePtr.PopPtr();
}


bool CFeatureCollectionControllerComp::CreateRepresentationFromObject(
			const istd::IChangeable& data,
			const sdl::imtlic::Features::CFeatureItemGqlRequest& featureItemRequest,
			sdl::imtlic::Features::CFeatureDataPayload& representationPayload,
			QString& errorMessage) const
{
	const imtlic::CIdentifiableFeatureInfo* featureInfoPtr = dynamic_cast<const imtlic::CIdentifiableFeatureInfo*>(&data);
	if (featureInfoPtr == nullptr){
		errorMessage = QString("Unable to create representation from object. Error: Object is invalid");
		SendErrorMessage(0, errorMessage, "CFeatureCollectionControllerComp");

		return false;
	}

	sdl::imtlic::Features::FeatureItemRequestArguments arguments = featureItemRequest.GetRequestedArguments();

	sdl::imtlic::Features::CFeatureData featureData;

	QByteArray id = arguments.input.GetId();
	featureData.SetId(id);

	representationPayload.SetFeatureData(featureData);

	return true;
}


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


