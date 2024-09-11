#include <imtlicgql/CFeatureCollectionControllerComp.h>


// ACF includes
#include <iprm/CIdParam.h>
#include <iprm/CParamsSet.h>

// ImtCore includes
#include <imtlic/CFeatureInfo.h>


namespace imtlicgql
{


// protected methods

bool CFeatureCollectionControllerComp::CreateFeatureFromRepresentationModel(
			const sdl::imtlic::Features::CFeatureData& featureRepresentationData,
			imtlic::CFeatureInfo& featureInfo,
			QString& errorMessage) const
{
	QByteArray objectId = featureRepresentationData.GetId();

	QByteArray featureId = featureRepresentationData.GetFeatureId();
	if (featureId.isEmpty()){
		errorMessage = QString("Unable to create feature with an empty 'Feature-ID'");
		return false;
	}

	featureInfo.SetFeatureId(featureId);

	iprm::CIdParam idParam;
	idParam.SetId(featureId);

	iprm::CParamsSet paramsSet1;
	paramsSet1.SetEditableParameter("FeatureId", &idParam);

	iprm::CParamsSet filterParam;
	filterParam.SetEditableParameter("ObjectFilter", &paramsSet1);

	imtbase::ICollectionInfo::Ids collectionIds = m_objectCollectionCompPtr->GetElementIds(0, -1, &filterParam);
	if (!collectionIds.isEmpty()){
		QByteArray id = collectionIds[0];
		if (objectId != id){
			errorMessage = QT_TR_NOOP(QString("Feature-ID: '%1' already exists. Please rename")).arg(qPrintable(featureId));
			return false;
		}
	}

	QString featureName = featureRepresentationData.GetFeatureName();
	if (featureName.isEmpty()){
		errorMessage = QString("Unable to create feature with an empty 'Feature Name'");
		return false;
	}

	featureInfo.SetFeatureName(featureName);

	QString description = featureRepresentationData.GetDescription();
	featureInfo.SetFeatureDescription(description);

	QByteArray dependencies = featureRepresentationData.GetDependencies();
	featureInfo.SetDependencies(dependencies.split(';'));

	bool isOptional = featureRepresentationData.GetOptional();
	featureInfo.SetOptional(isOptional);

	QList<sdl::imtlic::Features::CFeatureData> subFeatureDataList = featureRepresentationData.GetSubFeatures();
	for (const sdl::imtlic::Features::CFeatureData& subFeatureData : subFeatureDataList){
		imtlic::CFeatureInfo* subFeatureInfoPtr = new imtlic::CFeatureInfo();

		bool ok = CreateFeatureFromRepresentationModel(subFeatureData, *subFeatureInfoPtr, errorMessage);
		if (!ok){
			return false;
		}

		featureInfo.InsertSubFeature(subFeatureInfoPtr);
	}

	return true;
}


bool CFeatureCollectionControllerComp::CreateRepresentationModelFromFeatureInfo(
			const imtlic::CFeatureInfo& featureInfo,
			sdl::imtlic::Features::CFeatureData& featureRepresentationData,
			QString& errorMessage) const
{
	QByteArray featureId = featureInfo.GetFeatureId();
	featureRepresentationData.SetFeatureId(featureId);

	QString featureName = featureInfo.GetFeatureName();
	featureRepresentationData.SetFeatureName(featureName);
	featureRepresentationData.SetName(featureName);

	QByteArrayList dependencies = featureInfo.GetDependencies();
	featureRepresentationData.SetDependencies(dependencies.join(';'));

	QString description = featureInfo.GetFeatureDescription();
	featureRepresentationData.SetDescription(description);

	bool isOptional = featureInfo.IsOptional();
	featureRepresentationData.SetOptional(isOptional);

	QList<sdl::imtlic::Features::CFeatureData> subFeatureDataList;
	const imtlic::FeatureInfoList& subFeatures = featureInfo.GetSubFeatures();
	if (!subFeatures.IsEmpty()){
		for (int i = 0; i < subFeatures.GetCount(); i++){
			const imtlic::IFeatureInfo* featureInfoPtr = subFeatures.GetAt(i);
			if (featureInfoPtr == nullptr){
				return false;
			}

			const imtlic::CFeatureInfo* subFeatureInfoPtr = dynamic_cast<const imtlic::CFeatureInfo*>(featureInfoPtr);
			Q_ASSERT(subFeatureInfoPtr != nullptr);

			sdl::imtlic::Features::CFeatureData subfeatureData;
			imtbase::CTreeItemModel subFeatureRepresentationModel;
			bool ok = CreateRepresentationModelFromFeatureInfo(*subFeatureInfoPtr, subfeatureData, errorMessage);
			if (!ok){
				return false;
			}

			subFeatureDataList << subfeatureData;
		}
	}

	featureRepresentationData.SetSubFeatures(subFeatureDataList);

	return true;
}


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

	if (requestInfo.items.isTypeIdRequested){
		representationObject.SetTypeId(m_objectCollectionCompPtr->GetObjectTypeId(objectId));
	}

	if (requestInfo.items.isFeatureIdRequested){
		representationObject.SetFeatureId(featureInfoPtr->GetFeatureId());
	}

	if (requestInfo.items.isNameRequested){
		representationObject.SetName(featureInfoPtr->GetFeatureName());
	}

	if (requestInfo.items.isFeatureNameRequested){
		representationObject.SetFeatureName(featureInfoPtr->GetFeatureName());
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

	if (requestInfo.items.isSubFeaturesRequested){
		sdl::imtlic::Features::CFeatureData featureData;
		bool ok = CreateRepresentationModelFromFeatureInfo(*featureInfoPtr, featureData, errorMessage);
		if (ok){
			representationObject.SetSubFeatures(featureData.GetSubFeatures());
		}
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
	if (newObjectId.isEmpty()){
		newObjectId = QUuid::createUuid().toString(QUuid::WithoutBraces).toUtf8();
	}
	featureInfoPtr->SetObjectUuid(newObjectId);

	name = featureDataRepresentation.GetFeatureName();
	featureInfoPtr->SetFeatureName(name);

	description = featureDataRepresentation.GetDescription();
	featureInfoPtr->SetFeatureDescription(description);

	bool ok = CreateFeatureFromRepresentationModel(featureDataRepresentation, *featureInfoPtr, errorMessage);
	if (!ok){
		SendErrorMessage(0, errorMessage, "CFeatureCollectionControllerComp");

		return nullptr;
	}

	return featureInstancePtr.PopPtr();
}


bool CFeatureCollectionControllerComp::CreateRepresentationFromObject(
			const istd::IChangeable& data,
			const sdl::imtlic::Features::CGetFeatureItemGqlRequest& featureItemRequest,
			sdl::imtlic::Features::CFeatureDataPayload& representationPayload,
			QString& errorMessage) const
{
	const imtlic::CIdentifiableFeatureInfo* featureInfoPtr = dynamic_cast<const imtlic::CIdentifiableFeatureInfo*>(&data);
	if (featureInfoPtr == nullptr){
		errorMessage = QString("Unable to create representation from object. Error: Object is invalid");
		SendErrorMessage(0, errorMessage, "CFeatureCollectionControllerComp");

		return false;
	}

	sdl::imtlic::Features::GetFeatureItemRequestArguments arguments = featureItemRequest.GetRequestedArguments();

	sdl::imtlic::Features::CFeatureData featureData;

	QByteArray id = arguments.input.GetId();
	featureData.SetId(id);

	bool ok = CreateRepresentationModelFromFeatureInfo(*featureInfoPtr, featureData, errorMessage);
	if (!ok){
		errorMessage = QString("Unable to create representaion from object");
		return false;
	}

	representationPayload.SetFeatureData(featureData);

	return true;
}


} // namespace imtlicgql


