// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtauthgql/CRemoteUserCollectionDocumentServiceControllerComp.h>


namespace imtauthgql
{


// protected methods

// reimplemented (sdl::imtauth::UserCollectionDocumentService::CGraphQlHandlerCompBase)

sdl::imtauth::Users::CUserData CRemoteUserCollectionDocumentServiceControllerComp::OnGetUserRepresentation(
		const sdl::imtauth::UserCollectionDocumentService::CGetUserRepresentationGqlRequest& /*getUserRepresentationRequest*/,
		const ::imtgql::CGqlRequest& gqlRequest,
		QString& errorMessage) const
{
	return SendModelRequest<sdl::imtauth::Users::CUserData>(gqlRequest, errorMessage);
}


sdl::imtbase::CollectionDocumentService::CDocumentOperationStatus CRemoteUserCollectionDocumentServiceControllerComp::OnUpdateUserFromRepresentation(
		const sdl::imtauth::UserCollectionDocumentService::CUpdateUserFromRepresentationGqlRequest& /*updateUserFromRepresentationRequest*/,
		const ::imtgql::CGqlRequest& gqlRequest,
		QString& errorMessage) const
{
	return SendModelRequest<sdl::imtbase::CollectionDocumentService::CDocumentOperationStatus>(gqlRequest, errorMessage);
}


} // namespace imtauthgql
