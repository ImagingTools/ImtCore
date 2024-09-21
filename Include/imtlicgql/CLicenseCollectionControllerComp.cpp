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
	sdl::imtlic::Licenses::V1_0::CLicenseItem& representationObject,
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
		representationObject.SetId(objectId);
	}

	if (requestInfo.items.isTypeIdRequested){
		representationObject.SetTypeId(m_objectCollectionCompPtr->GetObjectTypeId(objectId));
	}

	if (requestInfo.items.isLicenseIdRequested){
		representationObject.SetLicenseId(licenseInfoPtr->GetLicenseId());
	}

	if (requestInfo.items.isNameRequested){
		representationObject.SetName(licenseInfoPtr->GetLicenseName());
	}

	if (requestInfo.items.isLicenseNameRequested){
		representationObject.SetLicenseName(licenseInfoPtr->GetLicenseName());
	}

	if (requestInfo.items.isDescriptionRequested){
		representationObject.SetDescription(licenseInfoPtr->GetLicenseDescription());
	}

	if (requestInfo.items.isProductIdRequested){
		representationObject.SetProductId(objectCollectionIterator.GetElementInfo("ProductId").toByteArray());
	}

	if (requestInfo.items.isProductUuidRequested){
		representationObject.SetProductUuid(licenseInfoPtr->GetProductId());
	}

	if (requestInfo.items.isParentLicensesRequested){
		QByteArrayList featureIds;

		for (const imtlic::ILicenseDefinition::FeatureInfo& featureInfo : licenseInfoPtr->GetFeatureInfos()){
			featureIds << featureInfo.id;
		}

		representationObject.SetParentLicenses(featureIds.join(';'));
	}

	if (requestInfo.items.isFeaturesRequested){
		representationObject.SetFeatures(licenseInfoPtr->GetDependencies().join(';'));
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


istd::IChangeable* CLicenseCollectionControllerComp::CreateObjectFromRepresentation(
	const sdl::imtlic::Licenses::V1_0::CLicenseDefinitionData& licenseDataRepresentation,
	QByteArray& newObjectId,
	QString& name,
	QString& description,
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

	newObjectId = licenseDataRepresentation.GetId();
	if (newObjectId.isEmpty()){
		newObjectId = QUuid::createUuid().toString(QUuid::WithoutBraces).toUtf8();
	}
	licenseInfoPtr->SetObjectUuid(newObjectId);

	QByteArray licenseId = licenseDataRepresentation.GetLicenseId();

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

	name = licenseDataRepresentation.GetName();
	licenseInfoPtr->SetLicenseName(name);

	description = licenseDataRepresentation.GetDescription();
	licenseInfoPtr->SetLicenseDescription(description);

	QByteArray productId = licenseDataRepresentation.GetProductId();
	licenseInfoPtr->SetProductId(productId);

	QByteArrayList dependencyIds = licenseDataRepresentation.GetParentLicenses().split(';');
	dependencyIds.removeAll("");
	licenseInfoPtr->SetDependencies(dependencyIds);

	QByteArrayList featureIds = licenseDataRepresentation.GetFeatures().split(';');
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
	sdl::imtlic::Licenses::V1_0::CLicenseDataPayload& representationPayload,
	QString& errorMessage) const
{
	const imtlic::CIdentifiableLicenseDefinition* licenseInfoPtr = dynamic_cast<const imtlic::CIdentifiableLicenseDefinition*>(&data);
	if (licenseInfoPtr == nullptr){
		errorMessage = QString("Unable to create representation from object. Error: Object is invalid");
		SendErrorMessage(0, errorMessage, "CProductCollectionControllerComp");

		return false;
	}

	sdl::imtlic::Licenses::V1_0::LicenseItemRequestArguments arguments = licenseItemRequest.GetRequestedArguments();
	sdl::imtlic::Licenses::V1_0::CLicenseDefinitionData licenseData;

	licenseData.SetId(arguments.input.GetId());

	QString name = licenseInfoPtr->GetLicenseName();
	licenseData.SetName(name);
	licenseData.SetLicenseName(name);

	QByteArray licenseId = licenseInfoPtr->GetLicenseId();
	licenseData.SetLicenseId(licenseId);

	QString description = licenseInfoPtr->GetLicenseDescription();
	licenseData.SetDescription(description);

	QByteArray productId = licenseInfoPtr->GetProductId();
	licenseData.SetProductId(productId);

	QByteArrayList featureUuids;
	for (const imtlic::ILicenseDefinition::FeatureInfo& featureInfo : licenseInfoPtr->GetFeatureInfos()){
		featureUuids << featureInfo.id;
	}
	licenseData.SetFeatures(featureUuids.join(';'));

	QByteArray dependencies = licenseInfoPtr->GetDependencies().join(';');
	licenseData.SetParentLicenses(dependencies);

	representationPayload.SetLicenseDefinitionData(licenseData);

	return true;
}


} // namespace imtlicgql


