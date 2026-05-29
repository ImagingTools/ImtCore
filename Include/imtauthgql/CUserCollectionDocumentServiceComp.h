// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ImtCore includes
#include <imtdoc/IDocumentService.h>

// Generated includes
#include <GeneratedFiles/imtauthsdl/SDL/1.0/CPP/UserCollectionDocumentService.h>


namespace imtauthgql
{


/**
 * Server-side handler for the User collection's per-document representation
 * GraphQL endpoints (analogous to CTenantCollectionDocumentServiceComp).
 *
 * Reads/writes the IUserInfo working copy that the server-side
 * CollectionDocumentManager holds for a given documentId.
 */
class CUserCollectionDocumentServiceComp:
			public sdl::imtauth::UserCollectionDocumentService::CGraphQlHandlerCompBase
{
public:
	typedef sdl::imtauth::UserCollectionDocumentService::CGraphQlHandlerCompBase BaseClass;

	I_BEGIN_COMPONENT(CUserCollectionDocumentServiceComp)
		I_ASSIGN(m_documentManagerCompPtr, "CollectionDocumentService", "Collection document manager", false, "CollectionDocumentService");
	I_END_COMPONENT

protected:
	// reimplemented (CGraphQlHandlerCompBase)
	virtual sdl::imtauth::Users::CUserData OnGetUserRepresentation(
				const sdl::imtauth::UserCollectionDocumentService::CGetUserRepresentationGqlRequest& getUserRepresentationRequest,
				const ::imtgql::CGqlRequest& gqlRequest,
				QString& errorMessage) const override;
	virtual sdl::imtbase::CollectionDocumentService::CDocumentOperationStatus OnUpdateUserFromRepresentation(
				const sdl::imtauth::UserCollectionDocumentService::CUpdateUserFromRepresentationGqlRequest& updateUserFromRepresentationRequest,
				const ::imtgql::CGqlRequest& gqlRequest,
				QString& errorMessage) const override;

private:
	I_REF(imtdoc::IDocumentService, m_documentManagerCompPtr);
};


} // namespace imtauthgql
