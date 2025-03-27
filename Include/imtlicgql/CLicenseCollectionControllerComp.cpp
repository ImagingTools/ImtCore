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
		representationObject.Id = QByteArray(objectId);
	}

	if (requestInfo.items.isTypeIdRequested){
		representationObject.TypeId = QByteArray(m_objectCollectionCompPtr->GetObjectTypeId(objectId));
	}

	if (requestInfo.items.isLicenseIdRequested){
		representationObject.LicenseId = QByteArray(licenseInfoPtr->GetLicenseId());
	}

	if (requestInfo.items.isLicenseNameRequested){
		representationObject.LicenseName = QString(licenseInfoPtr->GetLicenseName());
	}

	if (requestInfo.items.isDescriptionRequested){
		representationObject.Description = QString(licenseInfoPtr->GetLicenseDescription());
	}

	if (requestInfo.items.isProductIdRequested){
		if (m_productCollectionCompPtr.IsValid()){
			imtbase::IObjectCollection::DataPtr dataPtr;
			if (m_productCollectionCompPtr->GetObjectData(productUuid, dataPtr)){
				const imtlic::IProductInfo* productInfoPtr = dynamic_cast<const imtlic::IProductInfo*>(dataPtr.GetPtr());
				if (productInfoPtr != nullptr){
					representationObject.ProductId = productInfoPtr->GetProductId();
				}
			}
		}
	}

	if (requestInfo.items.isProductUuidRequested){
		representationObject.ProductUuid = QByteArray(licenseInfoPtr->GetProductId());
	}

	if (requestInfo.items.isParentLicensesRequested){
		representationObject.ParentLicenses = QByteArray(licenseInfoPtr->GetDependencies().join(';'));
	}

	if (requestInfo.items.isFeaturesRequested){
		QByteArrayList featureIds;

		for (const imtlic::ILicenseDefinition::FeatureInfo& featureInfo : licenseInfoPtr->GetFeatureInfos()){
			featureIds << featureInfo.id;
		}

		representationObject.Features = QByteArray(featureIds.join(';'));
	}

	if (requestInfo.items.isAddedRequested){
		QDateTime addedTime = objectCollectionIterator.GetElementInfo("Added").toDateTime();
		addedTime.setTimeSpec(Qt::UTC);

		QString added = addedTime.toLocalTime().toString("dd.MM.yyyy hh:mm:ss");
		representationObject.Added = std::make_optional<QString>(added);
	}

	if (requestInfo.items.isTimeStampRequested){
		QDateTime lastModifiedTime = objectCollectionIterator.GetElementInfo("TimeStamp").toDateTime();
		lastModifiedTime.setTimeSpec(Qt::UTC);

		QString lastModified = lastModifiedTime.toLocalTime().toString("dd.MM.yyyy hh:mm:ss");
		representationObject.TimeStamp = std::make_optional<QString>(lastModified);
	}

	return true;
}


istd::IChangeable* CLicenseCollectionControllerComp::CreateObjectFromRepresentation(
	const sdl::imtlic::Licenses::CLicenseDefinitionData::V1_0& licenseDataRepresentation,
	QByteArray& newObjectId,
	QString& errorMessage) const
{
	if (!m_licenseInfoFactCompPtr.IsValid()){
		errorMessage = QString("Unable to create object from representation. Error: Attribute 'm_licenseInfoFactCompPtr' was not set");
		SendErrorMessage(0, errorMessage, "CLicenseCollectionControllerComp");

		return nullptr;
	}

	istd::TDelPtr<imtlic::ILicenseDefinition> licenseInstancePtr = m_licenseInfoFactCompPtr.CreateInstance();
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

	return licenseInstancePtr.PopPtr();
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

	if (arguments.input.Version_1_0->Id){
		representationPayload.Id = QByteArray(*arguments.input.Version_1_0->Id);
	}

	QString name = licenseInfoPtr->GetLicenseName();
	representationPayload.LicenseName = QString(name);

	QByteArray licenseId = licenseInfoPtr->GetLicenseId();
	representationPayload.LicenseId = std::make_optional<QByteArray>(licenseId);

	QString description = licenseInfoPtr->GetLicenseDescription();
	representationPayload.Description = std::make_optional<QString>(description);

	QByteArray productId = licenseInfoPtr->GetProductId();
	representationPayload.ProductId = std::make_optional<QByteArray>(productId);

	QByteArrayList featureUuids;
	for (const imtlic::ILicenseDefinition::FeatureInfo& featureInfo : licenseInfoPtr->GetFeatureInfos()){
		featureUuids << featureInfo.id;
	}
	representationPayload.Features = QByteArray(featureUuids.join(';'));

	QByteArray dependencies = licenseInfoPtr->GetDependencies().join(';');
	representationPayload.ParentLicenses = std::make_optional<QByteArray>(dependencies);

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

	licenseInfoPtr->ResetData();

	QByteArray objectId = *licenseUpdateRequest.GetRequestedArguments().input.Version_1_0->Id;
	sdl::imtlic::Licenses::CLicenseDefinitionData::V1_0 licenseData = *licenseUpdateRequest.GetRequestedArguments().input.Version_1_0->Item;

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

	if (licenseDataRepresentation.Id){
		newObjectId = *licenseDataRepresentation.Id;
	}

	if (newObjectId.isEmpty()){
		newObjectId = QUuid::createUuid().toString(QUuid::WithoutBraces).toUtf8();
	}
	licenseInfoPtr->SetObjectUuid(newObjectId);

	QByteArray licenseId;
	if (licenseDataRepresentation.LicenseId){
		licenseId = *licenseDataRepresentation.LicenseId;
	}

	if (licenseId.isEmpty()){
		errorMessage = QString("Empty License-ID");
		SendErrorMessage(0, errorMessage, "CLicenseCollectionControllerComp");

		return false;
	}
	
	imtbase::IComplexCollectionFilter::FieldFilter fieldFilter;
	fieldFilter.fieldId = "LicenseId";
	fieldFilter.filterValue = licenseId;
	
	imtbase::IComplexCollectionFilter::GroupFilter groupFilter;
	groupFilter.fieldFilters << fieldFilter;
	
	imtbase::CComplexCollectionFilter complexFilter;
	complexFilter.SetFieldsFilter(groupFilter);
	
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

	if (licenseDataRepresentation.LicenseName){
		name = *licenseDataRepresentation.LicenseName;
	}
	licenseInfoPtr->SetLicenseName(name);

	if (licenseDataRepresentation.Description){
		description = *licenseDataRepresentation.Description;
	}
	licenseInfoPtr->SetLicenseDescription(description);

	QByteArray productId;
	if (licenseDataRepresentation.ProductId){
		productId = *licenseDataRepresentation.ProductId;
	}
	licenseInfoPtr->SetProductId(productId);

	QByteArrayList dependencyIds;
	if (licenseDataRepresentation.ParentLicenses){
		dependencyIds = licenseDataRepresentation.ParentLicenses->split(';');
	}
	dependencyIds.removeAll("");
	licenseInfoPtr->SetDependencies(dependencyIds);

	QByteArrayList featureIds;
	if (licenseDataRepresentation.Features){
		featureIds = licenseDataRepresentation.Features->split(';');
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


