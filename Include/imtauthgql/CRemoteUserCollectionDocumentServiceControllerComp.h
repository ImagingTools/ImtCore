// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ImtCore includes
#include <imtclientgql/TClientRequestManagerCompWrap.h>

// Generated includes
#include <GeneratedFiles/imtauthsdl/SDL/1.0/CPP/UserCollectionDocumentService.h>


namespace imtauthgql
{


/**
 * Client-side proxy that forwards the user-representation GraphQL endpoints
 * to a remote server.
 */
class CRemoteUserCollectionDocumentServiceControllerComp:
			public imtclientgql::TClientRequestManagerCompWrap<
							sdl::imtauth::UserCollectionDocumentService::CGraphQlHandlerCompBase>
{
public:
	typedef imtclientgql::TClientRequestManagerCompWrap<
					sdl::imtauth::UserCollectionDocumentService::CGraphQlHandlerCompBase> BaseClass;

	I_BEGIN_COMPONENT(CRemoteUserCollectionDocumentServiceControllerComp);
	I_END_COMPONENT;

protected:
	// reimplemented (sdl::imtauth::UserCollectionDocumentService::CGraphQlHandlerCompBase)
	virtual sdl::imtauth::Users::CUserData OnGetUserRepresentation(
				const sdl::imtauth::UserCollectionDocumentService::CGetUserRepresentationGqlRequest& getUserRepresentationRequest,
				const ::imtgql::CGqlRequest& gqlRequest,
				QString& errorMessage) const override;
	virtual sdl::imtbase::CollectionDocumentService::CDocumentOperationStatus OnUpdateUserFromRepresentation(
				const sdl::imtauth::UserCollectionDocumentService::CUpdateUserFromRepresentationGqlRequest& updateUserFromRepresentationRequest,
				const ::imtgql::CGqlRequest& gqlRequest,
				QString& errorMessage) const override;
};


} // namespace imtauthgql
