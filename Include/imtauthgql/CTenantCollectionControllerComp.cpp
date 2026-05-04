// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtauthgql/CTenantCollectionControllerComp.h>


// ACF includes
#include <iqt/iqt.h>

// ImtCore includes
#include <imtauth/CTenantInfo.h>
#include <imtauth/ITenantInfoProvider.h>


namespace imtauthgql
{


// protected methods

bool CTenantCollectionControllerComp::FillObjectFromRepresentation(
			const sdl::imtauth::Tenants::CTenantData::V1_0& tenantDataRepresentation,
			istd::IChangeable& object,
			QByteArray& newObjectId,
			QString& errorMessage) const
{
	auto tenantInfoPtr = dynamic_cast<imtauth::CIdentifiableTenantInfo*>(&object);
	if (tenantInfoPtr == nullptr){
		errorMessage = QString("Unable to cast tenant instance to identifiable object. Error: Invalid object");
		SendErrorMessage(0, errorMessage, "CTenantCollectionControllerComp");
		return false;
	}

	tenantInfoPtr->SetTenantId(newObjectId);

	QString name;
	if (tenantDataRepresentation.name){
		name = *tenantDataRepresentation.name;
	}
	if (name.isEmpty()){
		errorMessage = QString("Tenant name cannot be empty");
		return false;
	}

	imtbase::ICollectionInfo::Ids collectionIds = m_objectCollectionCompPtr->GetElementIds();
	for (imtbase::ICollectionInfo::Id& collectionId : collectionIds){
		imtbase::IObjectCollection::DataPtr tenantDataPtr;
		if (m_objectCollectionCompPtr->GetObjectData(collectionId, tenantDataPtr)){
			auto currentTenantInfoPtr = dynamic_cast<imtauth::ITenantInfo*>(tenantDataPtr.GetPtr());
			if (currentTenantInfoPtr != nullptr){
				if (collectionId != newObjectId){
					QString currentTenantName = currentTenantInfoPtr->GetTenantName();
					if (currentTenantName == name){
						errorMessage = QT_TR_NOOP(QString("Tenant Name '%1' already exists").arg(currentTenantName));
						return false;
					}
				}
			}
		}
	}

	tenantInfoPtr->SetTenantName(name);

	if (tenantDataRepresentation.description){
		tenantInfoPtr->SetTenantDescription(*tenantDataRepresentation.description);
	}

	if (tenantDataRepresentation.isActive){
		tenantInfoPtr->SetActive(*tenantDataRepresentation.isActive);
	}

	return true;
}


// reimplemented (sdl::imtbase::ImtCollection::CGraphQlHandlerCompBase)

sdl::imtbase::ImtCollection::CVisualStatus CTenantCollectionControllerComp::OnGetObjectVisualStatus(
			const sdl::imtbase::ImtCollection::CGetObjectVisualStatusGqlRequest& getObjectVisualStatusRequest,
			const ::imtgql::CGqlRequest& gqlRequest,
			QString& errorMessage) const
{
	sdl::imtbase::ImtCollection::CVisualStatus response = BaseClass::OnGetObjectVisualStatus(getObjectVisualStatusRequest, gqlRequest, errorMessage);
	if (!errorMessage.isEmpty()){
		return sdl::imtbase::ImtCollection::CVisualStatus();
	}
	
	if (!response.Version_1_0.has_value()){
		Q_ASSERT(false);
		return response;
	}
	
	if (!response.Version_1_0->text.has_value()){
		Q_ASSERT(false);
		return response;
	}
	
	if (response.Version_1_0->text->isEmpty()){
		response.Version_1_0->text = "<no name>";
	}
	
	QByteArray languageId;
	const imtgql::IGqlContext* gqlContextPtr = getObjectVisualStatusRequest.GetRequestContext();
	if (gqlContextPtr != nullptr){
		languageId = gqlContextPtr->GetLanguageId();
	}

	QString translation = iqt::GetTranslation(m_translationManagerCompPtr.GetPtr(), QString(QT_TR_NOOP("Tenants")).toUtf8(), languageId, "CTenantCollectionControllerComp");
	response.Version_1_0->text = translation + QByteArrayLiteral(" / ") + *response.Version_1_0->text;

	return response;
}


// reimplemented (sdl::imtauth::Tenants::CTenantCollectionControllerCompBase)

bool CTenantCollectionControllerComp::CreateRepresentationFromObject(
			const imtbase::IObjectCollectionIterator& objectCollectionIterator,
			const sdl::imtauth::Tenants::CGetTenantListGqlRequest& getTenantListRequest,
			sdl::imtauth::Tenants::CTenantItemData::V1_0& representationObject,
			QString& errorMessage) const
{
	QByteArray objectId = objectCollectionIterator.GetObjectId();

	const imtauth::ITenantInfo* tenantInfoPtr = nullptr;
	imtbase::IObjectCollection::DataPtr tenantDataPtr;
	if (objectCollectionIterator.GetObjectData(tenantDataPtr)){
		tenantInfoPtr = dynamic_cast<const imtauth::ITenantInfo*>(tenantDataPtr.GetPtr());
	}

	if (tenantInfoPtr == nullptr){
		errorMessage = QString("Unable to create representation from object '%1'").arg(qPrintable(objectId));
		SendErrorMessage(0, errorMessage, "CTenantCollectionControllerComp");

		return false;
	}

	sdl::imtauth::Tenants::GetTenantListRequestInfo requestInfo = getTenantListRequest.GetRequestInfo();

	if (requestInfo.items.isIdRequested){
		representationObject.id = QByteArray(objectId);
	}

	if (requestInfo.items.isTypeIdRequested){
		representationObject.typeId = objectCollectionIterator.GetObjectTypeId();
	}

	if (requestInfo.items.isNameRequested){
		representationObject.name = QString(tenantInfoPtr->GetTenantName());
	}

	if (requestInfo.items.isDescriptionRequested){
		representationObject.description = QString(tenantInfoPtr->GetTenantDescription());
	}

	if (requestInfo.items.isIsActiveRequested){
		representationObject.isActive = tenantInfoPtr->IsActive();
	}

	if (requestInfo.items.isCreatedAtRequested){
		representationObject.createdAt = tenantInfoPtr->GetCreatedAt();
	}

	return true;
}


istd::IChangeableUniquePtr CTenantCollectionControllerComp::CreateObjectFromRepresentation(
			const sdl::imtauth::Tenants::CTenantData::V1_0& tenantDataRepresentation,
			QByteArray& newObjectId,
			QString& errorMessage) const
{
	if (!m_tenantInfoFactCompPtr.IsValid()){
		errorMessage = QString("Unable to create object from representation. Error: Attribute 'm_tenantInfoFactCompPtr' was not set");
		SendErrorMessage(0, errorMessage, "CTenantCollectionControllerComp");

		return nullptr;
	}

	imtauth::ITenantInfoUniquePtr tenantInstancePtr = m_tenantInfoFactCompPtr.CreateInstance();
	if (!tenantInstancePtr.IsValid()){
		errorMessage = QString("Unable to create tenant instance. Error: Invalid object");
		SendErrorMessage(0, errorMessage, "CTenantCollectionControllerComp");

		return nullptr;
	}

	auto tenantInfoPtr = dynamic_cast<imtauth::CIdentifiableTenantInfo*>(tenantInstancePtr.GetPtr());
	if (tenantInfoPtr == nullptr){
		errorMessage = QString("Unable to cast tenant instance to identifiable object. Error: Invalid object");
		SendErrorMessage(0, errorMessage, "CTenantCollectionControllerComp");

		return nullptr;
	}

	if (tenantDataRepresentation.id){
		newObjectId = *tenantDataRepresentation.id;
	}
	if (newObjectId.isEmpty()){
		newObjectId = QUuid::createUuid().toString(QUuid::WithoutBraces).toUtf8();
	}
	tenantInfoPtr->SetObjectUuid(newObjectId);

	if (!FillObjectFromRepresentation(tenantDataRepresentation, *tenantInfoPtr, newObjectId, errorMessage)){
		return nullptr;
	}

	return tenantInstancePtr;
}


bool CTenantCollectionControllerComp::UpdateObjectFromRepresentationRequest(
			const imtgql::CGqlRequest& /*rawGqlRequest*/,
			const sdl::imtauth::Tenants::CTenantUpdateGqlRequest& tenantUpdateRequest,
			istd::IChangeable& object,
			QString& errorMessage) const
{
	sdl::imtauth::Tenants::CTenantData representation;
	
	if (!tenantUpdateRequest.GetRequestedArguments().input.Version_1_0.has_value()){
		I_CRITICAL();
		return false;
	}

	if (tenantUpdateRequest.GetRequestedArguments().input.Version_1_0->item){
		representation.Version_1_0 = tenantUpdateRequest.GetRequestedArguments().input.Version_1_0->item;
	}

	imtauth::CIdentifiableTenantInfo* tenantInfoPtr = dynamic_cast<imtauth::CIdentifiableTenantInfo*>(&object);
	if (tenantInfoPtr == nullptr){
		errorMessage = QString("Unable to cast tenant instance to identifiable object. Error: Invalid object");
		SendErrorMessage(0, errorMessage, "CTenantCollectionControllerComp");

		return false;
	}

	QByteArray objectId = tenantInfoPtr->GetObjectUuid();

	return FillObjectFromRepresentation(*representation.Version_1_0, object, objectId, errorMessage);
}


// reimplemented (imtservergql::CPermissibleGqlRequestHandlerComp)

bool CTenantCollectionControllerComp::CheckPermissions(const imtgql::CGqlRequest& gqlRequest, QString& errorMessage) const
{
	return BaseClass::CheckPermissions(gqlRequest, errorMessage);
}


} // namespace imtauthgql


