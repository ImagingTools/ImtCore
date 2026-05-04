// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtauthgql/CTenantCollectionDocumentManagerComp.h>


// ImtCore includes
#include <imtauth/ITenantInfo.h>
#include <imtdoc/CDocumentSaveEvent.h>
#include <imtdoc/CDocumentDeleteEvent.h>
#include <imtdoc/CDocumentRefreshEvent.h>
#include <imtbase/IObjectCollection.h>
#include <imtbase/IObjectCollectionIterator.h>
#include <imtgql/CGqlRequestContextManager.h>


namespace imtauthgql
{


sdl::imtauth::Tenants::CTenantData CTenantCollectionDocumentManagerComp::OnGetTenantRepresentation(
		const sdl::imtauth::TenantCollectionDocumentManager::CGetTenantRepresentationGqlRequest& getTenantRepresentationRequest,
		const ::imtgql::CGqlRequest& /*gqlRequest*/,
		QString& errorMessage) const
{
	sdl::imtauth::Tenants::CTenantData result;
	result.Version_1_0.Emplace();

	if (!getTenantRepresentationRequest.input.Version_1_0.HasValue()){
		errorMessage = QStringLiteral("Missing input");
		return result;
	}

	const QByteArray documentId = getTenantRepresentationRequest.input.Version_1_0->m_id.HasValue()
			? *getTenantRepresentationRequest.input.Version_1_0->m_id
			: QByteArray();

	if (documentId.isEmpty()){
		errorMessage = QStringLiteral("Missing document ID");
		return result;
	}

	if (!m_documentManagerCompPtr.IsValid()){
		errorMessage = QStringLiteral("Document manager not available");
		return result;
	}

	const istd::IChangeable* objectPtr = m_documentManagerCompPtr->GetDocument(documentId);
	if (objectPtr == nullptr){
		errorMessage = QStringLiteral("Document not found");
		return result;
	}

	const imtauth::ITenantInfo* tenantPtr = dynamic_cast<const imtauth::ITenantInfo*>(objectPtr);
	if (tenantPtr == nullptr){
		errorMessage = QStringLiteral("Invalid document type");
		return result;
	}

	result.Version_1_0->id = tenantPtr->GetTenantId();
	result.Version_1_0->name = tenantPtr->GetTenantName();
	result.Version_1_0->description = tenantPtr->GetTenantDescription();
	result.Version_1_0->isActive = tenantPtr->IsActive();
	result.Version_1_0->createdAt = tenantPtr->GetCreatedAt();
	result.Version_1_0->updatedAt = tenantPtr->GetUpdatedAt();

	return result;
}


sdl::imtbase::CollectionDocumentManager::CDocumentOperationStatus CTenantCollectionDocumentManagerComp::OnUpdateTenantFromRepresentation(
		const sdl::imtauth::TenantCollectionDocumentManager::CUpdateTenantFromRepresentationGqlRequest& updateTenantFromRepresentationRequest,
		const ::imtgql::CGqlRequest& /*gqlRequest*/,
		QString& errorMessage) const
{
	sdl::imtbase::CollectionDocumentManager::CDocumentOperationStatus result;
	result.Version_1_0.Emplace();

	if (!updateTenantFromRepresentationRequest.input.Version_1_0.HasValue()){
		errorMessage = QStringLiteral("Missing input");
		result.Version_1_0->status = QStringLiteral("Failed");
		return result;
	}

	const QByteArray documentId = updateTenantFromRepresentationRequest.input.Version_1_0->documentId.HasValue()
			? *updateTenantFromRepresentationRequest.input.Version_1_0->documentId
			: QByteArray();

	if (documentId.isEmpty()){
		errorMessage = QStringLiteral("Missing document ID");
		result.Version_1_0->status = QStringLiteral("Failed");
		return result;
	}

	if (!m_documentManagerCompPtr.IsValid()){
		errorMessage = QStringLiteral("Document manager not available");
		result.Version_1_0->status = QStringLiteral("Failed");
		return result;
	}

	istd::IChangeable* objectPtr = m_documentManagerCompPtr->GetDocument(documentId);
	if (objectPtr == nullptr){
		errorMessage = QStringLiteral("Document not found");
		result.Version_1_0->status = QStringLiteral("Failed");
		return result;
	}

	imtauth::ITenantInfo* tenantPtr = dynamic_cast<imtauth::ITenantInfo*>(objectPtr);
	if (tenantPtr == nullptr){
		errorMessage = QStringLiteral("Invalid document type");
		result.Version_1_0->status = QStringLiteral("Failed");
		return result;
	}

	const auto& tenantData = updateTenantFromRepresentationRequest.input.Version_1_0->tenant;
	if (tenantData.Version_1_0.HasValue()){
		if (tenantData.Version_1_0->name.HasValue()){
			tenantPtr->SetTenantName(*tenantData.Version_1_0->name);
		}
		if (tenantData.Version_1_0->description.HasValue()){
			tenantPtr->SetTenantDescription(*tenantData.Version_1_0->description);
		}
		if (tenantData.Version_1_0->isActive.HasValue()){
			tenantPtr->SetActive(*tenantData.Version_1_0->isActive);
		}
	}

	bool saved = m_documentManagerCompPtr->SaveDocument(documentId);
	if (!saved){
		errorMessage = QStringLiteral("Failed to save document");
		result.Version_1_0->status = QStringLiteral("Failed");
		return result;
	}

	result.Version_1_0->status = QStringLiteral("Success");
	return result;
}


bool CTenantCollectionDocumentManagerComp::ProcessEvent(imtdoc::CEventBase* /*eventPtr*/)
{
	return true;
}


} // namespace imtauthgql
