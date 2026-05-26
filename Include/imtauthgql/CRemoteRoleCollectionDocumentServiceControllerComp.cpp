// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtauthgql/CRemoteRoleCollectionDocumentServiceControllerComp.h>


namespace imtauthgql
{


// protected methods

// reimplemented (sdl::imtauth::RoleCollectionDocumentService::CGraphQlHandlerCompBase)

sdl::imtauth::Roles::CRoleData CRemoteRoleCollectionDocumentServiceControllerComp::OnGetRoleRepresentation(
		const sdl::imtauth::RoleCollectionDocumentService::CGetRoleRepresentationGqlRequest& /*getRoleRepresentationRequest*/,
		const ::imtgql::CGqlRequest& gqlRequest,
		QString& errorMessage) const
{
	return SendModelRequest<sdl::imtauth::Roles::CRoleData>(gqlRequest, errorMessage);
}


sdl::imtbase::CollectionDocumentService::CDocumentOperationStatus CRemoteRoleCollectionDocumentServiceControllerComp::OnUpdateRoleFromRepresentation(
		const sdl::imtauth::RoleCollectionDocumentService::CUpdateRoleFromRepresentationGqlRequest& /*updateRoleFromRepresentationRequest*/,
		const ::imtgql::CGqlRequest& gqlRequest,
		QString& errorMessage) const
{
	return SendModelRequest<sdl::imtbase::CollectionDocumentService::CDocumentOperationStatus>(gqlRequest, errorMessage);
}


} // namespace imtauthgql
