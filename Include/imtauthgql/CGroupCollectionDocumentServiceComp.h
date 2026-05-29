// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ImtCore includes
#include <imtdoc/IDocumentService.h>

// Generated includes
#include <GeneratedFiles/imtauthsdl/SDL/1.0/CPP/GroupCollectionDocumentService.h>


namespace imtauthgql
{


/**
 * Server-side handler for the Group collection's per-document representation
 * GraphQL endpoints (analogous to CTenantCollectionDocumentServiceComp).
 *
 * Reads/writes the IUserGroupInfo working copy that the server-side
 * CollectionDocumentManager holds for a given documentId.
 */
class CGroupCollectionDocumentServiceComp:
			public sdl::V1_0::imtauth::CGraphQlHandlerCompBase
{
public:
	typedef sdl::V1_0::imtauth::CGraphQlHandlerCompBase BaseClass;

	I_BEGIN_COMPONENT(CGroupCollectionDocumentServiceComp)
		I_ASSIGN(m_documentManagerCompPtr, "CollectionDocumentService", "Collection document manager", false, "CollectionDocumentService");
	I_END_COMPONENT

protected:
	// reimplemented (CGraphQlHandlerCompBase)
	virtual sdl::V1_0::imtauth::CGroupData OnGetGroupRepresentation(
				const sdl::V1_0::imtauth::CGetGroupRepresentationGqlRequest& getGroupRepresentationRequest,
				const ::imtgql::CGqlRequest& gqlRequest,
				QString& errorMessage) const override;
	virtual sdl::V1_0::imtbase::CDocumentOperationStatus OnUpdateGroupFromRepresentation(
				const sdl::V1_0::imtauth::CUpdateGroupFromRepresentationGqlRequest& updateGroupFromRepresentationRequest,
				const ::imtgql::CGqlRequest& gqlRequest,
				QString& errorMessage) const override;

private:
	I_REF(imtdoc::IDocumentService, m_documentManagerCompPtr);
};


} // namespace imtauthgql
