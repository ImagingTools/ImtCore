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
			const sdl::imtlic::Features::CFeatureData::V1_0& featureRepresentationData,
			const QByteArray& rootFeatureId,
			imtlic::CFeatureInfo& featureInfo,
			QString& errorMessage) const
{

	if (!featureRepresentationData.FeatureId || featureRepresentationData.FeatureId->isEmpty()){
		errorMessage = QString("Unable to create feature with an empty 'Feature-ID'");

		return false;
	}

	QByteArray featureId = *featureRepresentationData.FeatureId;
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
		if (rootFeatureId != id){
			errorMessage = QT_TR_NOOP(QString("Feature-ID: '%1' already exists. Please rename")).arg(qPrintable(featureId));
			return false;
		}
	}

	if (!featureRepresentationData.FeatureName || featureRepresentationData.FeatureName->isEmpty()){
		errorMessage = QString("Unable to create feature with an empty 'Feature Name'");
		return false;
	}

	QString featureName = *featureRepresentationData.FeatureName;
	featureInfo.SetFeatureName(featureName);

	QString description;
	if (featureRepresentationData.Description){
		description = *featureRepresentationData.Description;
	}
	featureInfo.SetFeatureDescription(description);

	QByteArray dependencies;
	if (featureRepresentationData.Dependencies){
		dependencies = *featureRepresentationData.Dependencies;
	}
	featureInfo.SetDependencies(dependencies.split(';'));

	bool isOptional = bool(featureRepresentationData.Optional && *featureRepresentationData.Optional);
	featureInfo.SetOptional(isOptional);

	bool isPermission = bool(featureRepresentationData.IsPermission && *featureRepresentationData.IsPermission);
	featureInfo.SetIsPermission(isPermission);

	QList<sdl::imtlic::Features::CFeatureData::V1_0> subFeatureDataList;
	if (featureRepresentationData.SubFeatures){
		subFeatureDataList = *featureRepresentationData.SubFeatures;
	}
	for (const sdl::imtlic::Features::CFeatureData::V1_0& subFeatureData : subFeatureDataList){
		imtlic::CFeatureInfo* subFeatureInfoPtr = new imtlic::CFeatureInfo();

		bool ok = CreateFeatureFromRepresentationModel(subFeatureData, rootFeatureId, *subFeatureInfoPtr, errorMessage);
		if (!ok){
			return false;
		}

		featureInfo.InsertSubFeature(subFeatureInfoPtr);
	}

	return true;
}


bool CFeatureCollectionControllerComp::CreateRepresentationModelFromFeatureInfo(
			const imtlic::CFeatureInfo& featureInfo,
			sdl::imtlic::Features::CFeatureData::V1_0& featureRepresentationData,
			QString& errorMessage) const
{
	QByteArray featureId = featureInfo.GetFeatureId();
	featureRepresentationData.FeatureId = std::make_optional<QByteArray>(featureId);

	QString featureName = featureInfo.GetFeatureName();
	featureRepresentationData.FeatureName = std::make_optional<QString>(featureName);
	featureRepresentationData.Name = std::make_optional<QString>(featureName);

	QByteArrayList dependencyList = featureInfo.GetDependencies();
	QByteArray dependencies = dependencyList.join(';');
	featureRepresentationData.Dependencies = std::make_optional<QByteArray>(dependencies);

	QString description = featureInfo.GetFeatureDescription();
	featureRepresentationData.Description = std::make_optional<QString>(description);

	bool isOptional = featureInfo.IsOptional();
	featureRepresentationData.Optional = std::make_optional<bool>(isOptional);

	bool isPermission = featureInfo.IsPermission();
	featureRepresentationData.IsPermission = std::make_optional<bool>(isPermission);

	QList<sdl::imtlic::Features::CFeatureData::V1_0> subFeatureDataList;
	const imtlic::FeatureInfoList& subFeatures = featureInfo.GetSubFeatures();
	if (!subFeatures.IsEmpty()){
		for (int i = 0; i < subFeatures.GetCount(); i++){
			const imtlic::IFeatureInfo* featureInfoPtr = subFeatures.GetAt(i);
			if (featureInfoPtr == nullptr){
				return false;
			}

			const imtlic::CFeatureInfo* subFeatureInfoPtr = dynamic_cast<const imtlic::CFeatureInfo*>(featureInfoPtr);
			Q_ASSERT(subFeatureInfoPtr != nullptr);

			sdl::imtlic::Features::CFeatureData::V1_0 subfeatureData;
			imtbase::CTreeItemModel subFeatureRepresentationModel;
			bool ok = CreateRepresentationModelFromFeatureInfo(*subFeatureInfoPtr, subfeatureData, errorMessage);
			if (!ok){
				return false;
			}

			subFeatureDataList << subfeatureData;
		}
	}

	featureRepresentationData.SubFeatures = std::make_optional<QList<sdl::imtlic::Features::CFeatureData::V1_0>>(subFeatureDataList);

	return true;
}


// reimplemented (sdl::imtlic::Features::V1_0::CFeatureCollectionControllerCompBase)

bool CFeatureCollectionControllerComp::CreateRepresentationFromObject(
			const imtbase::IObjectCollectionIterator& objectCollectionIterator,
			const sdl::imtlic::Features::V1_0::CFeaturesListGqlRequest& featuresListRequest,
			sdl::imtlic::Features::CFeatureItem::V1_0& representationObject,
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

	sdl::imtlic::Features::V1_0::FeaturesListRequestInfo requestInfo = featuresListRequest.GetRequestInfo();

	if (requestInfo.items.isIdRequested){
		representationObject.Id = std::make_optional<QByteArray>(objectId);
	}

	if (requestInfo.items.isTypeIdRequested){
		QByteArray collectionObjectId = m_objectCollectionCompPtr->GetObjectTypeId(objectId);
		representationObject.TypeId = std::make_optional<QByteArray>(collectionObjectId);
	}

	if (requestInfo.items.isFeatureIdRequested){
		QByteArray featureId = featureInfoPtr->GetFeatureId();
		representationObject.FeatureId = std::make_optional<QByteArray>(featureId);
	}

	if (requestInfo.items.isNameRequested){
		QString featureName = featureInfoPtr->GetFeatureName();
		representationObject.Name = std::make_optional<QString>(featureName);
	}

	if (requestInfo.items.isFeatureNameRequested){
		QString featureName = featureInfoPtr->GetFeatureName();
		representationObject.FeatureName = std::make_optional<QString>(featureName);
	}

	if (requestInfo.items.isDescriptionRequested){
		QString descriptions = featureInfoPtr->GetFeatureDescription();
		representationObject.Description = std::make_optional<QString>(descriptions);
	}

	if (requestInfo.items.isOptionalRequested){
		bool isOptional = featureInfoPtr->IsOptional();
		representationObject.Optional = std::make_optional<bool>(isOptional);
	}

	if (requestInfo.items.isDependenciesRequested){
		QByteArray dependencies = featureInfoPtr->GetDependencies().join(';');
		representationObject.Dependencies = std::make_optional<QByteArray>(dependencies);
	}

	if (requestInfo.items.isSubFeaturesRequested){
		sdl::imtlic::Features::CFeatureData::V1_0 featureData;
		bool ok = CreateRepresentationModelFromFeatureInfo(*featureInfoPtr, featureData, errorMessage);
		if (ok){
			representationObject.SubFeatures = QList<sdl::imtlic::Features::CFeatureData::V1_0>(*featureData.SubFeatures);
		}
	}

	if (requestInfo.items.isAddedRequested){
		QDateTime addedTime = objectCollectionIterator.GetElementInfo("Added").toDateTime();
		addedTime.setTimeSpec(Qt::UTC);

		QString added = addedTime.toLocalTime().toString("dd.MM.yyyy hh:mm:ss");
		representationObject.Added = std::make_optional<QString>(added);
	}

	if (requestInfo.items.isLastModifiedRequested){
		QDateTime lastModifiedTime = objectCollectionIterator.GetElementInfo("LastModified").toDateTime();
		lastModifiedTime.setTimeSpec(Qt::UTC);

		QString lastModified = lastModifiedTime.toLocalTime().toString("dd.MM.yyyy hh:mm:ss");
		representationObject.LastModified = std::make_optional<QString>(lastModified);
	}

	return true;
}


istd::IChangeable* CFeatureCollectionControllerComp::CreateObjectFromRepresentation(
			const sdl::imtlic::Features::CFeatureData::V1_0& featureDataRepresentation,
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

	if (featureDataRepresentation.Id){
		newObjectId = *featureDataRepresentation.Id;
	}
	if (newObjectId.isEmpty()){
		newObjectId = QUuid::createUuid().toString(QUuid::WithoutBraces).toUtf8();
	}
	featureInfoPtr->SetObjectUuid(newObjectId);

	if (featureDataRepresentation.FeatureName){
		name = *featureDataRepresentation.FeatureName;
	}
	featureInfoPtr->SetFeatureName(name);

	if (featureDataRepresentation.Description){
		description = *featureDataRepresentation.Description;
	}
	featureInfoPtr->SetFeatureDescription(description);

	bool ok = CreateFeatureFromRepresentationModel(featureDataRepresentation, newObjectId, *featureInfoPtr, errorMessage);
	if (!ok){
		SendErrorMessage(0, errorMessage, "CFeatureCollectionControllerComp");

		return nullptr;
	}

	return featureInstancePtr.PopPtr();
}


bool CFeatureCollectionControllerComp::CreateRepresentationFromObject(
			const istd::IChangeable& data,
			const sdl::imtlic::Features::V1_0::CGetFeatureItemGqlRequest& featureItemRequest,
			sdl::imtlic::Features::CFeatureDataPayload::V1_0& representationPayload,
			QString& errorMessage) const
{
	const imtlic::CIdentifiableFeatureInfo* featureInfoPtr = dynamic_cast<const imtlic::CIdentifiableFeatureInfo*>(&data);
	if (featureInfoPtr == nullptr){
		errorMessage = QString("Unable to create representation from object. Error: Object is invalid");
		SendErrorMessage(0, errorMessage, "CFeatureCollectionControllerComp");

		return false;
	}

	sdl::imtlic::Features::V1_0::GetFeatureItemRequestArguments arguments = featureItemRequest.GetRequestedArguments();

	sdl::imtlic::Features::CFeatureData::V1_0 featureData;

	QByteArray id;
	if (arguments.input.Id){
		id = *arguments.input.Id;
	}
	featureData.Id = std::make_optional<QByteArray>(id);

	bool ok = CreateRepresentationModelFromFeatureInfo(*featureInfoPtr, featureData, errorMessage);
	if (!ok){
		errorMessage = QString("Unable to create representaion from object");
		return false;
	}

	representationPayload.FeatureData = std::make_optional<sdl::imtlic::Features::CFeatureData::V1_0>(featureData);

	return true;
}


bool CFeatureCollectionControllerComp::UpdateObjectFromRepresentationRequest(
			const imtgql::CGqlRequest& /*rawGqlRequest*/,
			const sdl::imtlic::Features::V1_0::CUpdateFeatureGqlRequest& /*updateFeatureRequest*/,
			istd::IChangeable& /*object*/,
			QString& /*errorMessage*/) const
{
	return false;
}


} // namespace imtlicgql


