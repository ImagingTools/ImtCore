// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtlicgql/CFeatureCollectionControllerComp.h>


// ACF includes
#include <iprm/CIdParam.h>
#include <iprm/CParamsSet.h>

// ImtCore includes
#include <imtbase/CComplexCollectionFilter.h>
#include <imtlic/CFeatureInfo.h>
#include <imtlic/IProductInfo.h>
#include <imtlic/CLicenseDefinition.h>


namespace imtlicgql
{


// protected methods

bool CFeatureCollectionControllerComp::CreateFeatureFromRepresentationModel(
			const sdl::imtlic::Features::CFeatureData::V1_0& featureRepresentationData,
			const QByteArray& rootFeatureId,
			imtlic::CFeatureInfo& featureInfo,
			QString& errorMessage) const
{

	if (!featureRepresentationData.featureId || featureRepresentationData.featureId->isEmpty()){
		errorMessage = QString("Unable to create feature with an empty 'Feature-ID'");

		return false;
	}
	
	QByteArray featureId = *featureRepresentationData.featureId;
	
	imtbase::IComplexCollectionFilter::FieldFilter fieldFilter;
	fieldFilter.fieldId = "FeatureId";
	fieldFilter.filterValue = featureId;
	
	imtbase::IComplexCollectionFilter::FilterExpression groupFilter;
	groupFilter.fieldFilters << fieldFilter;
	
	imtbase::CComplexCollectionFilter complexFilter;
	complexFilter.SetFilterExpression(groupFilter);
	
	iprm::CParamsSet filterParam;
	filterParam.SetEditableParameter("ComplexFilter", &complexFilter);

	imtbase::ICollectionInfo::Ids collectionIds = m_objectCollectionCompPtr->GetElementIds(0, -1, &filterParam);
	if (!collectionIds.isEmpty()){
		QByteArray id = collectionIds[0];
		if (rootFeatureId != id){
			errorMessage = QT_TR_NOOP(QString("Feature-ID: '%1' already exists")).arg(qPrintable(featureId));
			return false;
		}
	}

	if (!featureRepresentationData.featureName || featureRepresentationData.featureName->isEmpty()){
		errorMessage = QString("Feature Name cannot be empty");
		return false;
	}
	
	featureInfo.SetFeatureId(featureId);

	QString featureName = *featureRepresentationData.featureName;
	featureInfo.SetFeatureName(featureName);

	QString description;
	if (featureRepresentationData.description){
		description = *featureRepresentationData.description;
	}
	featureInfo.SetFeatureDescription(description);

	QByteArray dependencies;
	if (featureRepresentationData.dependencies){
		dependencies = *featureRepresentationData.dependencies;
	}
	featureInfo.SetDependencies(dependencies.split(';'));

	bool isOptional = bool(featureRepresentationData.optional && *featureRepresentationData.optional);
	featureInfo.SetOptional(isOptional);

	bool isPermission = bool(featureRepresentationData.isPermission && *featureRepresentationData.isPermission);
	featureInfo.SetIsPermission(isPermission);

	imtsdl::TElementList<sdl::imtlic::Features::CFeatureData::V1_0> subFeatureDataList;
	if (featureRepresentationData.subFeatures){
		subFeatureDataList = *featureRepresentationData.subFeatures;
	}
	for (const istd::TSharedNullable<sdl::imtlic::Features::CFeatureData::V1_0>& subFeatureData : subFeatureDataList){
		if (!subFeatureData.HasValue()){
			continue;
		}

		imtlic::CFeatureInfo* subFeatureInfoPtr = new imtlic::CFeatureInfo();

		bool ok = CreateFeatureFromRepresentationModel(*subFeatureData, rootFeatureId, *subFeatureInfoPtr, errorMessage);
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
	featureRepresentationData.featureId = std::move(featureId);

	QString featureName = featureInfo.GetFeatureName();
	featureRepresentationData.featureName = std::move(featureName);
	featureRepresentationData.name = std::move(featureName);

	QByteArrayList dependencyList = featureInfo.GetDependencies();
	QByteArray dependencies = dependencyList.join(';');
	featureRepresentationData.dependencies = std::move(dependencies);

	QString description = featureInfo.GetFeatureDescription();
	featureRepresentationData.description = std::move(description);

	bool isOptional = featureInfo.IsOptional();
	featureRepresentationData.optional = isOptional;

	bool isPermission = featureInfo.IsPermission();
	featureRepresentationData.isPermission = isPermission;

	imtsdl::TElementList<sdl::imtlic::Features::CFeatureData::V1_0> subFeatureDataList;
	const imtlic::IFeatureInfo::FeatureInfoList& subFeatures = featureInfo.GetSubFeatures();
	if (!subFeatures.isEmpty()){
		for (int i = 0; i < subFeatures.count(); i++){
			imtlic::IFeatureInfoSharedPtr featureInfoPtr = subFeatures.at(i);
			if (!featureInfoPtr.IsValid()){
				return false;
			}

			const imtlic::CFeatureInfo* subFeatureInfoPtr = dynamic_cast<const imtlic::CFeatureInfo*>(featureInfoPtr.GetPtr());
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

	featureRepresentationData.subFeatures = std::move(subFeatureDataList);

	return true;
}


// reimplemented (sdl::imtlic::Features::CFeatureCollectionControllerCompBase)

void CFeatureCollectionControllerComp::OnAfterRemoveElements(const QByteArrayList& elementIds, const imtgql::CGqlRequest& /*gqlRequest*/) const
{
	for (const QByteArray& elementId : elementIds){
		SynchronizeProducts(elementId);
		SynchronizeLicenses(elementId);
	}
}


bool CFeatureCollectionControllerComp::CreateRepresentationFromObject(
			const imtbase::IObjectCollectionIterator& objectCollectionIterator,
			const sdl::imtlic::Features::CFeaturesListGqlRequest& featuresListRequest,
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

	sdl::imtlic::Features::FeaturesListRequestInfo requestInfo = featuresListRequest.GetRequestInfo();

	if (requestInfo.items.isIdRequested){
		representationObject.id = objectId;
	}

	if (requestInfo.items.isTypeIdRequested){
		QByteArray collectionObjectId = m_objectCollectionCompPtr->GetObjectTypeId(objectId);
		representationObject.typeId = collectionObjectId;
	}

	if (requestInfo.items.isFeatureIdRequested){
		QByteArray featureId = featureInfoPtr->GetFeatureId();
		representationObject.featureId = featureId;
	}

	if (requestInfo.items.isNameRequested){
		QString featureName = featureInfoPtr->GetFeatureName();
		representationObject.name = featureName;
	}

	if (requestInfo.items.isFeatureNameRequested){
		QString featureName = featureInfoPtr->GetFeatureName();
		representationObject.featureName = featureName;
	}

	if (requestInfo.items.isDescriptionRequested){
		QString description = featureInfoPtr->GetFeatureDescription();
		representationObject.description = description;
	}

	if (requestInfo.items.isOptionalRequested){
		bool isOptional = featureInfoPtr->IsOptional();
		representationObject.optional = isOptional;
	}

	if (requestInfo.items.isDependenciesRequested){
		QByteArray dependencies = featureInfoPtr->GetDependencies().join(';');
		representationObject.dependencies = dependencies;
	}

	if (requestInfo.items.isSubFeaturesRequested){
		sdl::imtlic::Features::CFeatureData::V1_0 featureData;
		bool ok = CreateRepresentationModelFromFeatureInfo(*featureInfoPtr, featureData, errorMessage);
		if (ok){
			representationObject.subFeatures = *featureData.subFeatures;
		}
	}

	if (requestInfo.items.isAddedRequested){
		QDateTime addedTime = objectCollectionIterator.GetElementInfo("Added").toDateTime().toUTC();
		QString added = addedTime.toLocalTime().toString("dd.MM.yyyy hh:mm:ss");
		representationObject.added = std::move(added);
	}

	if (requestInfo.items.isTimeStampRequested){
		QDateTime lastModifiedTime = objectCollectionIterator.GetElementInfo("TimeStamp").toDateTime().toUTC();

		QString lastModified = lastModifiedTime.toLocalTime().toString("dd.MM.yyyy hh:mm:ss");
		representationObject.timeStamp = std::move(lastModified);
	}

	return true;
}


istd::IChangeableUniquePtr CFeatureCollectionControllerComp::CreateObjectFromRepresentation(
			const sdl::imtlic::Features::CFeatureData::V1_0& featureDataRepresentation,
			QByteArray& newObjectId,
			QString& errorMessage) const
{
	if (!m_featureInfoFactCompPtr.IsValid()){
		errorMessage = QString("Unable to create object from representation. Error: Attribute 'm_featureInfoFactCompPtr' was not set");
		SendErrorMessage(0, errorMessage, "CFeatureCollectionControllerComp");

		return nullptr;
	}

	imtlic::IFeatureInfoUniquePtr featureInstancePtr = m_featureInfoFactCompPtr.CreateInstance();
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

	if (featureDataRepresentation.id){
		newObjectId = *featureDataRepresentation.id;
	}
	if (newObjectId.isEmpty()){
		newObjectId = QUuid::createUuid().toString(QUuid::WithoutBraces).toUtf8();
	}
	featureInfoPtr->SetObjectUuid(newObjectId);

	QString name;
	QString description;

	if (featureDataRepresentation.featureName){
		name = *featureDataRepresentation.featureName;
	}
	featureInfoPtr->SetFeatureName(name);

	if (featureDataRepresentation.description){
		description = *featureDataRepresentation.description;
	}
	featureInfoPtr->SetFeatureDescription(description);

	bool ok = CreateFeatureFromRepresentationModel(featureDataRepresentation, newObjectId, *featureInfoPtr, errorMessage);
	if (!ok){
		SendErrorMessage(0, errorMessage, "CFeatureCollectionControllerComp");

		return nullptr;
	}

	istd::IChangeableUniquePtr retVal;
	retVal.MoveCastedPtr<imtlic::IFeatureInfo>(featureInstancePtr);

	return retVal;
}


bool CFeatureCollectionControllerComp::CreateRepresentationFromObject(
			const istd::IChangeable& data,
			const sdl::imtlic::Features::CGetFeatureItemGqlRequest& featureItemRequest,
			sdl::imtlic::Features::CFeatureData::V1_0& representationPayload,
			QString& errorMessage) const
{
	const imtlic::CIdentifiableFeatureInfo* featureInfoPtr = dynamic_cast<const imtlic::CIdentifiableFeatureInfo*>(&data);
	if (featureInfoPtr == nullptr){
		errorMessage = QString("Unable to create representation from object. Error: Object is invalid");
		SendErrorMessage(0, errorMessage, "CFeatureCollectionControllerComp");

		return false;
	}

	sdl::imtlic::Features::GetFeatureItemRequestArguments arguments = featureItemRequest.GetRequestedArguments();
	if (!arguments.input.Version_1_0.has_value()){
		I_CRITICAL();
		return false;
	}
	
	QByteArray id;
	if (arguments.input.Version_1_0->id){
		id = *arguments.input.Version_1_0->id;
	}
	representationPayload.id = std::move(id);

	bool ok = CreateRepresentationModelFromFeatureInfo(*featureInfoPtr, representationPayload, errorMessage);
	if (!ok){
		errorMessage = QString("Unable to create representaion from object. Error: '%1'").arg(errorMessage);
		SendErrorMessage(0, errorMessage, "CFeatureCollectionControllerComp");

		return false;
	}

	return true;
}


bool CFeatureCollectionControllerComp::UpdateObjectFromRepresentationRequest(
			const imtgql::CGqlRequest& /*rawGqlRequest*/,
			const sdl::imtlic::Features::CUpdateFeatureGqlRequest& updateFeatureRequest,
			istd::IChangeable& object,
			QString& errorMessage) const
{
	imtlic::CIdentifiableFeatureInfo* featureInfoPtr = dynamic_cast<imtlic::CIdentifiableFeatureInfo*>(&object);
	if (featureInfoPtr == nullptr){
		errorMessage = QString("Unable to update feature from representation. Error: Object is invalid");
		SendErrorMessage(0, errorMessage, "CFeatureCollectionControllerComp");

		return false;
	}
	
	sdl::imtlic::Features::UpdateFeatureRequestArguments arguments = updateFeatureRequest.GetRequestedArguments();
	if (!arguments.input.Version_1_0.has_value()){
		I_CRITICAL();
		return false;
	}
	
	if (!arguments.input.Version_1_0->id.has_value()){
		I_CRITICAL();
		return false;
	}
	
	if (!arguments.input.Version_1_0->item.has_value()){
		I_CRITICAL();
		return false;
	}

	QByteArray featureId = *arguments.input.Version_1_0->id;

	featureInfoPtr->ResetData();

	featureInfoPtr->SetObjectUuid(featureId);
	sdl::imtlic::Features::CFeatureData::V1_0 featureData = *arguments.input.Version_1_0->item;

	bool ok = CreateFeatureFromRepresentationModel(featureData, featureId, *featureInfoPtr, errorMessage);
	if (!ok){
		errorMessage = QString("Unable to update feature from representation. Error: '%1'").arg(errorMessage);
		SendErrorMessage(0, errorMessage, "CFeatureCollectionControllerComp");

		return false;
	}

	SynchronizeProducts(featureId, featureInfoPtr);
	SynchronizeLicenses(featureId, featureInfoPtr);

	return true;
}


// private methods

void CFeatureCollectionControllerComp::SynchronizeProducts(const QByteArray& featureId, const imtlic::IFeatureInfo* featureInfoPtr) const
{
	if (m_productCollectionCompPtr.IsValid()){
		imtbase::ICollectionInfo::Ids elementIds = m_productCollectionCompPtr->GetElementIds();
		for (const imtbase::ICollectionInfo::Id& elementId : elementIds){
			imtbase::IObjectCollection::DataPtr dataPtr;
			if (m_productCollectionCompPtr->GetObjectData(elementId, dataPtr)){
				imtlic::IProductInfo* productInfoPtr = dynamic_cast<imtlic::IProductInfo*>(dataPtr.GetPtr());
				if (productInfoPtr != nullptr){
					imtbase::IObjectCollection*  featureCollectionPtr = productInfoPtr->GetFeatures();
					if (featureCollectionPtr != nullptr){
						imtbase::ICollectionInfo::Ids featureElementIds =featureCollectionPtr->GetElementIds();
						if (featureElementIds.contains(featureId)){
							productInfoPtr->RemoveFeature(featureId);

							if (featureInfoPtr != nullptr){
								productInfoPtr->AddFeature(featureId, *featureInfoPtr);
							}

							m_productCollectionCompPtr->SetObjectData(elementId, *productInfoPtr);
							return;
						}
					}
				}
			}
		}
	}
}


void CFeatureCollectionControllerComp::SynchronizeLicenses(const QByteArray& featureId, const imtlic::IFeatureInfo* featureInfoPtr) const
{
	if (m_licenseCollectionCompPtr.IsValid()){
		imtbase::ICollectionInfo::Ids elementIds = m_licenseCollectionCompPtr->GetElementIds();
		for (const imtbase::ICollectionInfo::Id& elementId : elementIds){
			imtbase::IObjectCollection::DataPtr dataPtr;
			if (m_licenseCollectionCompPtr->GetObjectData(elementId, dataPtr)){
				imtlic::CLicenseDefinition* licenseDefinitionPtr = dynamic_cast<imtlic::CLicenseDefinition*>(dataPtr.GetPtr());
				if (licenseDefinitionPtr != nullptr){
					if (licenseDefinitionPtr->ContainsFeature(featureId)){
						licenseDefinitionPtr->RemoveFeature(featureId);

						if (featureInfoPtr != nullptr){
							imtlic::ILicenseDefinition::FeatureInfo featureInfo;
							featureInfo.id = featureId;
							featureInfo.name = featureInfoPtr->GetFeatureName();
							licenseDefinitionPtr->AddFeature(featureInfo);
						}

						m_licenseCollectionCompPtr->SetObjectData(elementId, *licenseDefinitionPtr);
						return;
					}
				}
			}
		}
	}
}


} // namespace imtlicgql


