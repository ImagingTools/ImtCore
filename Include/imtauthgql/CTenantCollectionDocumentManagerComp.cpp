// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtauthgql/CTenantCollectionDocumentManagerComp.h>


// ImtCore includes
#include <imtauth/ITenantInfo.h>
#include <imtbase/IObjectCollection.h>
#include <imtbase/IObjectCollectionIterator.h>
#include <imtdoc/CDocumentSavedEvent.h>
#include <imtgql/IGqlContext.h>


namespace imtauthgql
{


// protected methods

// reimplemented (CGraphQlHandlerCompBase)

sdl::imtauth::Tenants::CTenantData CTenantCollectionDocumentManagerComp::OnGetTenantRepresentation(
		const sdl::imtauth::TenantCollectionDocumentManager::CGetTenantRepresentationGqlRequest& getTenantRepresentationRequest,
		const ::imtgql::CGqlRequest& gqlRequest,
		QString& errorMessage) const
{
	sdl::imtauth::TenantCollectionDocumentManager::GetTenantRepresentationRequestArguments arguments = getTenantRepresentationRequest.GetRequestedArguments();
	if (!arguments.input.Version_1_0){
		Q_ASSERT(false);
		return sdl::imtauth::Tenants::CTenantData();
	}

	QByteArray userId = GetUserId(gqlRequest);

	QByteArray objectId;
	if (arguments.input.Version_1_0->id){
		objectId = *arguments.input.Version_1_0->id;
	}

	if (objectId.isEmpty()){
		errorMessage = QStringLiteral("Missing document ID");
		return sdl::imtauth::Tenants::CTenantData();
	}

	istd::IChangeableSharedPtr documentPtr;
	m_documentManagerCompPtr->GetDocumentData(userId, objectId, documentPtr);
	if (!documentPtr.IsValid()){
		errorMessage = QStringLiteral("Document not found");
		return sdl::imtauth::Tenants::CTenantData();
	}

	const imtauth::ITenantInfo* tenantPtr = dynamic_cast<const imtauth::ITenantInfo*>(documentPtr.GetPtr());
	if (tenantPtr == nullptr){
		errorMessage = QStringLiteral("Invalid document type");
		return sdl::imtauth::Tenants::CTenantData();
	}

	sdl::imtauth::Tenants::CTenantData response;
	response.Version_1_0.Emplace();

	response.Version_1_0->id = tenantPtr->GetTenantId();
	response.Version_1_0->name = tenantPtr->GetTenantName();
	response.Version_1_0->description = tenantPtr->GetTenantDescription();
	response.Version_1_0->isActive = tenantPtr->IsActive();
	response.Version_1_0->createdAt = tenantPtr->GetCreatedAt();
	response.Version_1_0->updatedAt = tenantPtr->GetUpdatedAt();

	return response;
}


sdl::imtbase::CollectionDocumentManager::CDocumentOperationStatus CTenantCollectionDocumentManagerComp::OnUpdateTenantFromRepresentation(
		const sdl::imtauth::TenantCollectionDocumentManager::CUpdateTenantFromRepresentationGqlRequest& updateTenantFromRepresentationRequest,
		const ::imtgql::CGqlRequest& gqlRequest,
		QString& errorMessage) const
{
	sdl::imtauth::TenantCollectionDocumentManager::UpdateTenantFromRepresentationRequestArguments arguments = updateTenantFromRepresentationRequest.GetRequestedArguments();
	if (!arguments.input.Version_1_0){
		Q_ASSERT(false);
		return sdl::imtbase::CollectionDocumentManager::CDocumentOperationStatus();
	}

	sdl::imtbase::CollectionDocumentManager::CDocumentOperationStatus response;
	response.Version_1_0.Emplace();
	response.Version_1_0->status = sdl::imtbase::CollectionDocumentManager::EDocumentOperationStatus::Failed;

	QByteArray documentId;
	if (arguments.input.Version_1_0->documentId){
		documentId = *arguments.input.Version_1_0->documentId;
	}

	if (documentId.isEmpty()){
		errorMessage = QStringLiteral("Missing document ID");
		return response;
	}

	QByteArray userId = GetUserId(gqlRequest);

	istd::IChangeableSharedPtr documentPtr;
	m_documentManagerCompPtr->GetDocumentData(userId, documentId, documentPtr);
	if (!documentPtr.IsValid()){
		response.Version_1_0->status = sdl::imtbase::CollectionDocumentManager::EDocumentOperationStatus::InvalidDocumentId;
		return response;
	}

	imtauth::ITenantInfo* tenantPtr = dynamic_cast<imtauth::ITenantInfo*>(documentPtr.GetPtr());
	if (tenantPtr == nullptr){
		response.Version_1_0->status = sdl::imtbase::CollectionDocumentManager::EDocumentOperationStatus::InvalidDocumentId;
		return response;
	}

	sdl::imtauth::Tenants::CTenantData::V1_0 tenantData;
	if (arguments.input.Version_1_0->tenant){
		tenantData = *arguments.input.Version_1_0->tenant;
	}

	if (tenantData.name){
		tenantPtr->SetTenantName(*tenantData.name);
	}

	if (tenantData.description){
		tenantPtr->SetTenantDescription(*tenantData.description);
	}

	if (tenantData.isActive){
		tenantPtr->SetActive(*tenantData.isActive);
	}

	m_documentManagerCompPtr->SetDocumentData(userId, documentId, *documentPtr);
	m_documentManagerCompPtr->SaveDocument(userId, documentId);

	response.Version_1_0->status = sdl::imtbase::CollectionDocumentManager::EDocumentOperationStatus::Success;

	return response;
}


// reimplemented (imtdoc::IDocumentManagerEventHandler)

bool CTenantCollectionDocumentManagerComp::ProcessEvent(imtdoc::CEventBase* eventPtr)
{
	imtdoc::CDocumentSavedEvent* savedEventPtr = dynamic_cast<imtdoc::CDocumentSavedEvent*>(eventPtr);
	if (savedEventPtr != nullptr){
		QByteArray documentId = savedEventPtr->GetDocumentId();
		QByteArray userId = savedEventPtr->GetUserId();

		// Refresh DB-computed fields (createdAt/updatedAt) from the collection after save
		imtbase::IObjectCollection::DataPtr collectionDataPtr;
		const imtauth::ITenantInfo* dbTenantPtr = nullptr;
		if (m_tenantCollectionCompPtr.IsValid() && m_tenantCollectionCompPtr->GetObjectData(documentId, collectionDataPtr)){
			dbTenantPtr = dynamic_cast<const imtauth::ITenantInfo*>(collectionDataPtr.GetPtr());
		}

		istd::IChangeableSharedPtr documentPtr;
		imtauth::ITenantInfo* documentTenantPtr = nullptr;
		if (m_documentManagerCompPtr->GetDocumentData(userId, documentId, documentPtr) == imtdoc::IDocumentManager::OS_OK){
			documentTenantPtr = dynamic_cast<imtauth::ITenantInfo*>(documentPtr.GetPtr());
		}

		if (dbTenantPtr != nullptr && documentTenantPtr != nullptr){
			documentTenantPtr->SetCreatedAt(dbTenantPtr->GetCreatedAt());
			documentTenantPtr->SetUpdatedAt(dbTenantPtr->GetUpdatedAt());

			m_documentManagerCompPtr->SetDocumentData(userId, documentId, *documentPtr);
			m_documentManagerCompPtr->SaveDocument(userId, documentId);
		}
	}

	return true;
}


} // namespace imtauthgql


