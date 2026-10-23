// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtauthgql/CTenantCollectionDocumentServiceControllerComp.h>

// ImtCore includes
#include <imtbasesdl/SDL/1.0/CPP/CollectionDocumentService.h>
#include <imtgql/IGqlContext.h>
#include <imtbasesdl/SDL/1.0/CPP/UndoManager.h>
#include <imtauthgql/TenantAccess.h>


namespace imtauthgql
{


namespace CDM = sdl::V1_0::imtbase;


CDM::CDocumentList CTenantCollectionDocumentServiceControllerComp::OnGetOpenedDocumentList(
		const CDM::CGetOpenedDocumentListGqlRequest& getOpenedDocumentListRequest,
		const ::imtgql::CGqlRequest& gqlRequest,
		QString& errorMessage) const
{
	// Get the full list from the base class
	CDM::CDocumentList fullList = BaseClass::OnGetOpenedDocumentList(getOpenedDocumentListRequest, gqlRequest, errorMessage);

	// Determine the current session tenant
	const imtgql::IGqlContext* contextPtr = gqlRequest.GetRequestContext();
	if (contextPtr == nullptr){
		return fullList;
	}

	QByteArray sessionTenantId = contextPtr->GetTenantId();

	// If no tenant is selected, return an empty list — user cannot edit any tenant
	// If a tenant is selected, only return documents matching that tenant
	CDM::CDocumentList filteredList;
	filteredList.documentList.emplace();

	if (!sessionTenantId.isEmpty() && fullList.documentList){
		for (const auto& docInfo : *fullList.documentList){
			if (docInfo && docInfo->objectId && *docInfo->objectId == sessionTenantId){
				filteredList.documentList->append(docInfo);
			}
		}
	}

	return filteredList;
}


CDM::CDocumentInfo CTenantCollectionDocumentServiceControllerComp::OnCreateNewDocument(
		const CDM::CCreateNewDocumentGqlRequest& createNewDocumentRequest,
		const ::imtgql::CGqlRequest& gqlRequest,
		QString& errorMessage) const
{
	const imtgql::IGqlContext* contextPtr = gqlRequest.GetRequestContext();
	if (contextPtr == nullptr || !CanCreateOrganization(*contextPtr)){
		errorMessage = QStringLiteral("Permission denied: /OrganizationManagement/Add");
		return CDM::CDocumentInfo();
	}

	return BaseClass::OnCreateNewDocument(createNewDocumentRequest, gqlRequest, errorMessage);
}


CDM::CDocumentInfo CTenantCollectionDocumentServiceControllerComp::OnOpenDocument(
		const CDM::COpenDocumentGqlRequest& openDocumentRequest,
		const ::imtgql::CGqlRequest& gqlRequest,
		QString& errorMessage) const
{
	// Opening a tenant document is only allowed when the user has selected
	// that same tenant (session tenantId must match the tenant being opened).
	const imtgql::IGqlContext* contextPtr = gqlRequest.GetRequestContext();
	if (contextPtr != nullptr){
		QByteArray sessionTenantId = contextPtr->GetTenantId();
		if (sessionTenantId.isEmpty()){
			errorMessage = QStringLiteral("An organization can be edited only while you work in it. Switch to the organization first.");
			return CDM::CDocumentInfo();
		}

		// Extract the objectId (tenantId) being opened from the request arguments
		const auto& arguments = openDocumentRequest.GetRequestedArguments();
		if (!arguments.input.has_value()){
			Q_ASSERT(false);
			return CDM::CDocumentInfo();
		}

		if (arguments.input->id){
			QByteArray requestedTenantId = *arguments.input->id;
			if (!requestedTenantId.isEmpty() && requestedTenantId != sessionTenantId){
				errorMessage = QStringLiteral("An organization can be edited only while you work in it. Switch to the organization first.");
				return CDM::CDocumentInfo();
			}
		}
	}

	return BaseClass::OnOpenDocument(openDocumentRequest, gqlRequest, errorMessage);
}


CDM::CDocumentOperationStatus CTenantCollectionDocumentServiceControllerComp::OnSaveDocument(
		const CDM::CSaveDocumentGqlRequest& saveDocumentRequest,
		const ::imtgql::CGqlRequest& gqlRequest,
		QString& errorMessage) const
{
	const auto& arguments = saveDocumentRequest.GetRequestedArguments();
	if (arguments.input.has_value() && arguments.input->documentId){
		const imtgql::IGqlContext* contextPtr = gqlRequest.GetRequestContext();
		const QByteArray sessionTenantId = (contextPtr != nullptr) ? contextPtr->GetTenantId() : QByteArray();
		const QByteArray tenantId = GetCollectionObjectId(GetUserId(gqlRequest), *arguments.input->documentId);
		if (tenantId.isEmpty() && (contextPtr == nullptr || !CanCreateOrganization(*contextPtr))){
			errorMessage = QStringLiteral("Permission denied: /OrganizationManagement/Add");
			CDM::CDocumentOperationStatus retVal;
			retVal.status = CDM::EDocumentOperationStatus::Failed;
			retVal.message = errorMessage;
			return retVal;
		}

		if (!tenantId.isEmpty() && tenantId != sessionTenantId){
			errorMessage = QStringLiteral("An organization can be edited only while you work in it. Switch to the organization first.");
			CDM::CDocumentOperationStatus retVal;
			retVal.status = CDM::EDocumentOperationStatus::Failed;
			retVal.message = errorMessage;
			return retVal;
		}
	}

	return BaseClass::OnSaveDocument(saveDocumentRequest, gqlRequest, errorMessage);
}


} // namespace imtauthgql
