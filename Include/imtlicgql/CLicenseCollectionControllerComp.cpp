#include <imtlicgql/CLicenseCollectionControllerComp.h>


// ACF includes
#include <iprm/CIdParam.h>
#include <iprm/CParamsSet.h>

// ImtCore includes
#include <imtlic/CLicenseDefinition.h>


namespace imtlicgql
{


// protected methods

// reimplemented (sdl::imtlic::Licenses::V1_0::CLicenseCollectionControllerCompBase)

bool CLicenseCollectionControllerComp::CreateRepresentationFromObject(
	const imtbase::IObjectCollectionIterator& objectCollectionIterator,
	const sdl::imtlic::Licenses::V1_0::CLicensesListGqlRequest& licensesListRequest,
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

	idoc::MetaInfoPtr metaInfo = objectCollectionIterator.GetDataMetaInfo();

	sdl::imtlic::Licenses::V1_0::LicensesListRequestInfo requestInfo = licensesListRequest.GetRequestInfo();

	if (requestInfo.items.isIdRequested){
		representationObject.Id = QByteArray(objectId);
	}

	if (requestInfo.items.isTypeIdRequested){
		representationObject.TypeId = QByteArray(m_objectCollectionCompPtr->GetObjectTypeId(objectId));
	}

	if (requestInfo.items.isLicenseIdRequested){
		representationObject.LicenseId = QByteArray(licenseInfoPtr->GetLicenseId());
	}

	if (requestInfo.items.isNameRequested){
		representationObject.Name = QString(licenseInfoPtr->GetLicenseName());
	}

	if (requestInfo.items.isLicenseNameRequested){
		representationObject.LicenseName = QString(licenseInfoPtr->GetLicenseName());
	}

	if (requestInfo.items.isDescriptionRequested){
		representationObject.Description = QString(licenseInfoPtr->GetLicenseDescription());
	}

	if (requestInfo.items.isProductIdRequested){
		representationObject.ProductId = QByteArray(objectCollectionIterator.GetElementInfo("ProductId").toByteArray());
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

	if (requestInfo.items.isLastModifiedRequested){
		QDateTime lastModifiedTime = objectCollectionIterator.GetElementInfo("LastModified").toDateTime();
		lastModifiedTime.setTimeSpec(Qt::UTC);

		QString lastModified = lastModifiedTime.toLocalTime().toString("dd.MM.yyyy hh:mm:ss");
		representationObject.LastModified = std::make_optional<QString>(lastModified);
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

	imtlic::CIdentifiableLicenseDefinition* licenseInfoPtr = dynamic_cast<imtlic::CIdentifiableLicenseDefinition*>(licenseInstancePtr.GetPtr());
	if (licenseInfoPtr == nullptr){
		errorMessage = QString("Unable to cast license instance to identifable object. Error: Invalid object");
		SendErrorMessage(0, errorMessage, "CLicenseCollectionControllerComp");

		return nullptr;
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
		errorMessage = QString("Unable to create license with an empty License-ID");
		SendErrorMessage(0, errorMessage, "CLicenseCollectionControllerComp");

		return nullptr;
	}

	iprm::CIdParam idParam;
	idParam.SetId(licenseId);

	iprm::CParamsSet paramsSet1;
	paramsSet1.SetEditableParameter("LicenseId", &idParam);

	iprm::CParamsSet filterParam;
	filterParam.SetEditableParameter("ObjectFilter", &paramsSet1);

	imtbase::ICollectionInfo::Ids collectionIds = m_objectCollectionCompPtr->GetElementIds(0, -1, &filterParam);
	if (!collectionIds.isEmpty()){
		QByteArray id = collectionIds[0];
		if (newObjectId != id){
			errorMessage = QT_TR_NOOP(QString("License-ID: '%1' already exists.")).arg(qPrintable(licenseId));

			return nullptr;
		}
	}

	licenseInfoPtr->SetLicenseId(licenseId);

	QString name;
	QString description;

	if (licenseDataRepresentation.Name){
		name = *licenseDataRepresentation.Name;
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

	return licenseInstancePtr.PopPtr();
}


bool CLicenseCollectionControllerComp::CreateRepresentationFromObject(
	const istd::IChangeable& data,
	const sdl::imtlic::Licenses::V1_0::CLicenseItemGqlRequest& licenseItemRequest,
	sdl::imtlic::Licenses::CLicenseDataPayload::V1_0& representationPayload,
	QString& errorMessage) const
{
	const imtlic::CIdentifiableLicenseDefinition* licenseInfoPtr = dynamic_cast<const imtlic::CIdentifiableLicenseDefinition*>(&data);
	if (licenseInfoPtr == nullptr){
		errorMessage = QString("Unable to create representation from object. Error: Object is invalid");
		SendErrorMessage(0, errorMessage, "CProductCollectionControllerComp");

		return false;
	}

	sdl::imtlic::Licenses::V1_0::LicenseItemRequestArguments arguments = licenseItemRequest.GetRequestedArguments();
	sdl::imtlic::Licenses::CLicenseDefinitionData::V1_0 licenseData;

	if (arguments.input.Id){
		licenseData.Id = QByteArray(*arguments.input.Id);
	}

	QString name = licenseInfoPtr->GetLicenseName();
	licenseData.Name = QString(name);
	licenseData.LicenseName = QString(name);

	QByteArray licenseId = licenseInfoPtr->GetLicenseId();
	licenseData.LicenseId = std::make_optional<QByteArray>(licenseId);

	QString description = licenseInfoPtr->GetLicenseDescription();
	licenseData.Description = std::make_optional<QString>(description);

	QByteArray productId = licenseInfoPtr->GetProductId();
	licenseData.ProductId = std::make_optional<QByteArray>(productId);

	QByteArrayList featureUuids;
	for (const imtlic::ILicenseDefinition::FeatureInfo& featureInfo : licenseInfoPtr->GetFeatureInfos()){
		featureUuids << featureInfo.id;
	}
	licenseData.Features = QByteArray(featureUuids.join(';'));

	QByteArray dependencies = licenseInfoPtr->GetDependencies().join(';');
	licenseData.ParentLicenses = std::make_optional<QByteArray>(dependencies);

	representationPayload.LicenseDefinitionData = std::make_optional<sdl::imtlic::Licenses::CLicenseDefinitionData::V1_0>(licenseData);

	return true;
}

bool CLicenseCollectionControllerComp::UpdateObjectFromRepresentationRequest(
			const imtgql::CGqlRequest& /*rawGqlRequest*/,
			const sdl::imtlic::Licenses::V1_0::CLicenseUpdateGqlRequest& /*licenseUpdateRequest*/,
			istd::IChangeable& /*object*/,
			QString& /*errorMessage*/) const
{
	return false;
}


} // namespace imtlicgql


