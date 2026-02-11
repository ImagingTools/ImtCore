// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtlicgql/CLicenseCollectionControllerComp.h>


// ACF includes
#include <iprm/CParamsSet.h>

// ImtCore includes
#include <imtlic/CLicenseDefinition.h>
#include <imtlic/IProductInfo.h>
#include <imtbase/CComplexCollectionFilter.h>


namespace imtlicgql
{


// protected methods

// reimplemented (sdl::imtlic::Licenses::CLicenseCollectionControllerCompBase)

bool CLicenseCollectionControllerComp::CreateRepresentationFromObject(
			const imtbase::IObjectCollectionIterator& objectCollectionIterator,
			const sdl::imtlic::Licenses::CLicensesListGqlRequest& licensesListRequest,
			sdl::imtlic::Licenses::CLicenseItem::V1_0& representationObject,
			QString& errorMessage) const
{
	if (!m_objectCollectionCompPtr.IsValid()){
		errorMessage = QString("Unable to create representation from object. Error: Attribute 'm_objectCollectionCompPtr' was not set");
		SendErrorMessage(0, errorMessage, "CLicenseCollectionControllerComp");

		return false;
	}

	QByteArray objectId = objectCollectionIterator.GetObjectId();

	imtlic::CIdentifiableLicenseDefinition* licenseInfoPtr = nullptr;
	imtbase::IObjectCollection::DataPtr dataPtr;
	if (objectCollectionIterator.GetObjectData(dataPtr)){
		licenseInfoPtr = dynamic_cast<imtlic::CIdentifiableLicenseDefinition*>(dataPtr.GetPtr());
	}

	if (licenseInfoPtr == nullptr){
		errorMessage = QString("Unable to create representation from object '%1'").arg(qPrintable(objectId));
		SendErrorMessage(0, errorMessage, "CLicenseCollectionControllerComp");

		return false;
	}
	
	QByteArray productUuid = licenseInfoPtr->GetProductId();

	idoc::MetaInfoPtr metaInfo = objectCollectionIterator.GetDataMetaInfo();

	sdl::imtlic::Licenses::LicensesListRequestInfo requestInfo = licensesListRequest.GetRequestInfo();

	if (requestInfo.items.isIdRequested){
		representationObject.id = QByteArray(objectId);
	}

	if (requestInfo.items.isTypeIdRequested){
		representationObject.typeId = QByteArray(m_objectCollectionCompPtr->GetObjectTypeId(objectId));
	}

	if (requestInfo.items.isLicenseIdRequested){
		representationObject.licenseId = QByteArray(licenseInfoPtr->GetLicenseId());
	}

	if (requestInfo.items.isLicenseNameRequested){
		representationObject.licenseName = QString(licenseInfoPtr->GetLicenseName());
	}

	if (requestInfo.items.isDescriptionRequested){
		representationObject.description = QString(licenseInfoPtr->GetLicenseDescription());
	}

	if (requestInfo.items.isProductIdRequested){
		if (m_productCollectionCompPtr.IsValid()){
			imtbase::IObjectCollection::DataPtr productDataPtr;
			if (m_productCollectionCompPtr->GetObjectData(productUuid, productDataPtr)){
				const imtlic::IProductInfo* productInfoPtr = dynamic_cast<const imtlic::IProductInfo*>(productDataPtr.GetPtr());
				if (productInfoPtr != nullptr){
					representationObject.productId = productInfoPtr->GetProductId();
				}
			}
		}
	}

	if (requestInfo.items.isProductUuidRequested){
		representationObject.productUuid = QByteArray(licenseInfoPtr->GetProductId());
	}

	if (requestInfo.items.isParentLicensesRequested){
		representationObject.parentLicenses = QByteArray(licenseInfoPtr->GetDependencies().join(';'));
	}

	if (requestInfo.items.isFeaturesRequested){
		QByteArrayList featureIds;

		for (const imtlic::ILicenseDefinition::FeatureInfo& featureInfo : licenseInfoPtr->GetFeatureInfos()){
			featureIds << featureInfo.id;
		}

		representationObject.features = QByteArray(featureIds.join(';'));
	}

	if (requestInfo.items.isAddedRequested){
		QDateTime addedTime = objectCollectionIterator.GetElementInfo("Added").toDateTime();
		addedTime.setTimeSpec(Qt::UTC);

		QString added = addedTime.toLocalTime().toString("dd.MM.yyyy hh:mm:ss");
		representationObject.added = std::move(added);
	}

	if (requestInfo.items.isTimeStampRequested){
		QDateTime lastModifiedTime = objectCollectionIterator.GetElementInfo("TimeStamp").toDateTime();
		lastModifiedTime.setTimeSpec(Qt::UTC);

		QString lastModified = lastModifiedTime.toLocalTime().toString("dd.MM.yyyy hh:mm:ss");
		representationObject.timeStamp = std::move(lastModified);
	}

	return true;
}


istd::IChangeableUniquePtr CLicenseCollectionControllerComp::CreateObjectFromRepresentation(
			const sdl::imtlic::Licenses::CLicenseDefinitionData::V1_0& licenseDataRepresentation,
			QByteArray& newObjectId,
			QString& errorMessage) const
{
	if (!m_licenseInfoFactCompPtr.IsValid()){
		errorMessage = QString("Unable to create object from representation. Error: Attribute 'm_licenseInfoFactCompPtr' was not set");
		SendErrorMessage(0, errorMessage, "CLicenseCollectionControllerComp");

		return nullptr;
	}

	imtlic::ILicenseDefinitionUniquePtr licenseInstancePtr = m_licenseInfoFactCompPtr.CreateInstance();
	if (!licenseInstancePtr.IsValid()){
		errorMessage = QString("Unable to create license instance. Error: Invalid object");
		SendErrorMessage(0, errorMessage, "CLicenseCollectionControllerComp");

		return nullptr;
	}

	if (!FillObjectFromRepresentation(licenseDataRepresentation, *licenseInstancePtr, newObjectId, errorMessage)){
		errorMessage = QString("Unable to create object from representation. Error: '%1'").arg(errorMessage);
		SendErrorMessage(0, errorMessage, "CLicenseCollectionControllerComp");

		return nullptr;
	}

	istd::IChangeableUniquePtr retVal;
	retVal.MoveCastedPtr<imtlic::ILicenseDefinition>(licenseInstancePtr);

	return retVal;
}


bool CLicenseCollectionControllerComp::CreateRepresentationFromObject(
	const istd::IChangeable& data,
	const sdl::imtlic::Licenses::CLicenseItemGqlRequest& licenseItemRequest,
	sdl::imtlic::Licenses::CLicenseDefinitionData::V1_0& representationPayload,
	QString& errorMessage) const
{
	const imtlic::CIdentifiableLicenseDefinition* licenseInfoPtr = dynamic_cast<const imtlic::CIdentifiableLicenseDefinition*>(&data);
	if (licenseInfoPtr == nullptr){
		errorMessage = QString("Unable to create representation from object. Error: Object is invalid");
		SendErrorMessage(0, errorMessage, "CProductCollectionControllerComp");

		return false;
	}

	sdl::imtlic::Licenses::LicenseItemRequestArguments arguments = licenseItemRequest.GetRequestedArguments();
	if (!arguments.input.Version_1_0.has_value()){
		I_CRITICAL();
		return false;
	}

	if (arguments.input.Version_1_0->id){
		representationPayload.id = QByteArray(*arguments.input.Version_1_0->id);
	}

	QString name = licenseInfoPtr->GetLicenseName();
	representationPayload.licenseName = QString(name);

	QByteArray licenseId = licenseInfoPtr->GetLicenseId();
	representationPayload.licenseId = std::move(licenseId);

	QString description = licenseInfoPtr->GetLicenseDescription();
	representationPayload.description = std::move(description);

	QByteArray productId = licenseInfoPtr->GetProductId();
	representationPayload.productId = std::move(productId);

	QByteArrayList featureUuids;
	for (const imtlic::ILicenseDefinition::FeatureInfo& featureInfo : licenseInfoPtr->GetFeatureInfos()){
		featureUuids << featureInfo.id;
	}
	representationPayload.features = QByteArray(featureUuids.join(';'));

	QByteArray dependencies = licenseInfoPtr->GetDependencies().join(';');
	representationPayload.parentLicenses = std::move(dependencies);

	return true;
}

bool CLicenseCollectionControllerComp::UpdateObjectFromRepresentationRequest(
			const imtgql::CGqlRequest& /*rawGqlRequest*/,
			const sdl::imtlic::Licenses::CLicenseUpdateGqlRequest& licenseUpdateRequest,
			istd::IChangeable& object,
			QString& errorMessage) const
{
	imtlic::CIdentifiableLicenseDefinition* licenseInfoPtr = dynamic_cast<imtlic::CIdentifiableLicenseDefinition*>(&object);
	if (licenseInfoPtr == nullptr){
		errorMessage = QString("Unable to update representation from object. Error: Object is invalid");
		SendErrorMessage(0, errorMessage, "CLicenseCollectionControllerComp");

		return false;
	}
	
	sdl::imtlic::Licenses::LicenseUpdateRequestArguments arguments = licenseUpdateRequest.GetRequestedArguments();
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

	QByteArray objectId = *arguments.input.Version_1_0->id;
	licenseInfoPtr->ResetData();

	licenseInfoPtr->SetObjectUuid(objectId);

	sdl::imtlic::Licenses::CLicenseDefinitionData::V1_0 licenseData = *arguments.input.Version_1_0->item;

	if (!FillObjectFromRepresentation(licenseData, *licenseInfoPtr, objectId, errorMessage)){
		errorMessage = QString("Unable to update object from representation. Error: '%1'").arg(errorMessage);
		SendErrorMessage(0, errorMessage, "CLicenseCollectionControllerComp");

		return false;
	}

	return true;
}


// private methods

bool CLicenseCollectionControllerComp::FillObjectFromRepresentation(
			const sdl::imtlic::Licenses::CLicenseDefinitionData::V1_0& licenseDataRepresentation,
			istd::IChangeable& object,
			QByteArray& newObjectId,
			QString& errorMessage) const
{
	imtlic::CIdentifiableLicenseDefinition* licenseInfoPtr = dynamic_cast<imtlic::CIdentifiableLicenseDefinition*>(&object);
	if (licenseInfoPtr == nullptr){
		errorMessage = QString("Object is invalid");
		SendErrorMessage(0, errorMessage, "CLicenseCollectionControllerComp");

		return false;
	}

	if (licenseDataRepresentation.id){
		newObjectId = *licenseDataRepresentation.id;
	}

	if (newObjectId.isEmpty()){
		newObjectId = QUuid::createUuid().toString(QUuid::WithoutBraces).toUtf8();
	}
	licenseInfoPtr->SetObjectUuid(newObjectId);

	QByteArray licenseId;
	if (licenseDataRepresentation.licenseId){
		licenseId = *licenseDataRepresentation.licenseId;
	}

	if (licenseId.isEmpty()){
		errorMessage = QString("Empty License-ID");
		SendErrorMessage(0, errorMessage, "CLicenseCollectionControllerComp");

		return false;
	}
	
	imtbase::IComplexCollectionFilter::FieldFilter fieldFilter;
	fieldFilter.fieldId = "LicenseId";
	fieldFilter.filterValue = licenseId;
	
	imtbase::IComplexCollectionFilter::FilterExpression groupFilter;
	groupFilter.fieldFilters << fieldFilter;
	
	imtbase::CComplexCollectionFilter complexFilter;
	complexFilter.SetFilterExpression(groupFilter);
	
	iprm::CParamsSet filterParam;
	filterParam.SetEditableParameter("ComplexFilter", &complexFilter);

	imtbase::ICollectionInfo::Ids collectionIds = m_objectCollectionCompPtr->GetElementIds(0, -1, &filterParam);
	if (!collectionIds.isEmpty()){
		QByteArray id = collectionIds[0];
		if (newObjectId != id){
			errorMessage = QT_TR_NOOP(QString("License-ID: '%1' already exists.")).arg(qPrintable(licenseId));

			return false;
		}
	}

	licenseInfoPtr->SetLicenseId(licenseId);

	QString name;
	QString description;

	if (licenseDataRepresentation.licenseName){
		name = *licenseDataRepresentation.licenseName;
	}
	licenseInfoPtr->SetLicenseName(name);

	if (licenseDataRepresentation.description){
		description = *licenseDataRepresentation.description;
	}
	licenseInfoPtr->SetLicenseDescription(description);

	QByteArray productId;
	if (licenseDataRepresentation.productId){
		productId = *licenseDataRepresentation.productId;
	}
	licenseInfoPtr->SetProductId(productId);

	QByteArrayList dependencyIds;
	if (licenseDataRepresentation.parentLicenses){
		dependencyIds = licenseDataRepresentation.parentLicenses->split(';');
	}
	dependencyIds.removeAll("");
	licenseInfoPtr->SetDependencies(dependencyIds);

	QByteArrayList featureIds;
	if (licenseDataRepresentation.features){
		featureIds = licenseDataRepresentation.features->split(';');
	}

	imtlic::ILicenseDefinition::FeatureInfos featureInfos;
	for (const QByteArray& featureUuid : featureIds){
		imtlic::ILicenseDefinition::FeatureInfo featureInfo;
		featureInfo.id = featureUuid;
		featureInfos << featureInfo;
	}
	licenseInfoPtr->SetFeatureInfos(featureInfos);

	return true;
}


} // namespace imtlicgql


