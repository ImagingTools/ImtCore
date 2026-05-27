// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtauthgql/CTenantCollectionDocumentServiceControllerComp.h>

// ImtCore includes
#include <imtgql/IGqlContext.h>


namespace imtauthgql
{


namespace CDM = sdl::imtbase::CollectionDocumentService;


CDM::CDocumentInfo CTenantCollectionDocumentServiceControllerComp::OnCreateNewDocument(
		const CDM::CCreateNewDocumentGqlRequest& createNewDocumentRequest,
		const ::imtgql::CGqlRequest& gqlRequest,
		QString& errorMessage) const
{
	// Creating a new tenant is only allowed when the user has NOT selected
	// any tenant (currentTenantId must be empty).
	const imtgql::IGqlContext* contextPtr = gqlRequest.GetRequestContext();
	if (contextPtr != nullptr){
		QByteArray sessionTenantId = contextPtr->GetTenantId();
		if (!sessionTenantId.isEmpty()){
			errorMessage = QStringLiteral("Cannot create a new tenant while switched to an organization. Please deselect the current tenant first.");
			return CDM::CDocumentInfo();
		}
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
			errorMessage = QStringLiteral("Cannot open a tenant for editing without switching to it first. Please select the tenant.");
			return CDM::CDocumentInfo();
		}

		// Extract the objectId (tenantId) being opened from the request arguments
		const auto& arguments = openDocumentRequest.GetRequestedArguments();
		if (arguments.input.Version_1_0 && arguments.input.Version_1_0->id){
			QByteArray requestedTenantId = *arguments.input.Version_1_0->id;
			if (!requestedTenantId.isEmpty() && requestedTenantId != sessionTenantId){
				errorMessage = QStringLiteral("Cannot edit a tenant you are not currently switched to. Please switch to this organization first.");
				return CDM::CDocumentInfo();
			}
		}
	}

	return BaseClass::OnOpenDocument(openDocumentRequest, gqlRequest, errorMessage);
}


} // namespace imtauthgql
