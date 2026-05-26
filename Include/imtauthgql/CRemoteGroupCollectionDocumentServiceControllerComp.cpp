// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtauthgql/CRemoteGroupCollectionDocumentServiceControllerComp.h>


namespace imtauthgql
{


// protected methods

// reimplemented (sdl::imtauth::GroupCollectionDocumentService::CGraphQlHandlerCompBase)

sdl::imtauth::Groups::CGroupData CRemoteGroupCollectionDocumentServiceControllerComp::OnGetGroupRepresentation(
		const sdl::imtauth::GroupCollectionDocumentService::CGetGroupRepresentationGqlRequest& /*getGroupRepresentationRequest*/,
		const ::imtgql::CGqlRequest& gqlRequest,
		QString& errorMessage) const
{
	return SendModelRequest<sdl::imtauth::Groups::CGroupData>(gqlRequest, errorMessage);
}


sdl::imtbase::CollectionDocumentService::CDocumentOperationStatus CRemoteGroupCollectionDocumentServiceControllerComp::OnUpdateGroupFromRepresentation(
		const sdl::imtauth::GroupCollectionDocumentService::CUpdateGroupFromRepresentationGqlRequest& /*updateGroupFromRepresentationRequest*/,
		const ::imtgql::CGqlRequest& gqlRequest,
		QString& errorMessage) const
{
	return SendModelRequest<sdl::imtbase::CollectionDocumentService::CDocumentOperationStatus>(gqlRequest, errorMessage);
}


} // namespace imtauthgql
