// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ImtCore includes
#include <imtclientgql/TClientRequestManagerCompWrap.h>

// Generated includes
#include <GeneratedFiles/imtauthsdl/SDL/1.0/CPP/GroupCollectionDocumentService.h>


namespace imtauthgql
{


/**
 * Client-side proxy that forwards the group-representation GraphQL endpoints
 * to a remote server.
 */
class CRemoteGroupCollectionDocumentServiceControllerComp:
			public imtclientgql::TClientRequestManagerCompWrap<
							sdl::imtauth::GroupCollectionDocumentService::CGraphQlHandlerCompBase>
{
public:
	typedef imtclientgql::TClientRequestManagerCompWrap<
					sdl::imtauth::GroupCollectionDocumentService::CGraphQlHandlerCompBase> BaseClass;

	I_BEGIN_COMPONENT(CRemoteGroupCollectionDocumentServiceControllerComp);
	I_END_COMPONENT;

protected:
	// reimplemented (sdl::imtauth::GroupCollectionDocumentService::CGraphQlHandlerCompBase)
	virtual sdl::imtauth::Groups::CGroupData OnGetGroupRepresentation(
				const sdl::imtauth::GroupCollectionDocumentService::CGetGroupRepresentationGqlRequest& getGroupRepresentationRequest,
				const ::imtgql::CGqlRequest& gqlRequest,
				QString& errorMessage) const override;
	virtual sdl::imtbase::CollectionDocumentService::CDocumentOperationStatus OnUpdateGroupFromRepresentation(
				const sdl::imtauth::GroupCollectionDocumentService::CUpdateGroupFromRepresentationGqlRequest& updateGroupFromRepresentationRequest,
				const ::imtgql::CGqlRequest& gqlRequest,
				QString& errorMessage) const override;
};


} // namespace imtauthgql
