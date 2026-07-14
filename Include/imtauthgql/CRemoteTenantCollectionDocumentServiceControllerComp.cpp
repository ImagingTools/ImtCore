// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtauthgql/CRemoteTenantCollectionDocumentServiceControllerComp.h>
#include <GeneratedFiles/imtauthsdl/SDL/1.0/CPP/TenantCollectionDocumentService.h>


namespace imtauthgql
{


// protected methods

// reimplemented (sdl::V1_0::imtauth::CTenantCollectionDocumentServiceGqlHandlerCompBase)

sdl::V1_0::imtauth::CTenantData CRemoteTenantCollectionDocumentServiceControllerComp::OnGetTenantRepresentation(
		const sdl::V1_0::imtauth::CGetTenantRepresentationGqlRequest& /*getTenantRepresentationRequest*/,
		const ::imtgql::CGqlRequest& gqlRequest,
		QString& errorMessage) const
{
	sdl::V1_0::imtauth::CTenantData response = SendModelRequest<sdl::V1_0::imtauth::CTenantData>(gqlRequest, errorMessage);
	return response;
}


sdl::V1_0::imtbase::CDocumentOperationStatus CRemoteTenantCollectionDocumentServiceControllerComp::OnUpdateTenantFromRepresentation(
		const sdl::V1_0::imtauth::CUpdateTenantFromRepresentationGqlRequest& /*updateTenantFromRepresentationRequest*/,
		const ::imtgql::CGqlRequest& gqlRequest,
		QString& errorMessage) const
{
	return SendModelRequest<sdl::V1_0::imtbase::CDocumentOperationStatus>(gqlRequest, errorMessage);
}


// private methods


} // namespace imtauthgql
