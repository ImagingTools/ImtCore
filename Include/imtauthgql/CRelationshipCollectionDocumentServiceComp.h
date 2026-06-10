// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ImtCore includes
#include <imtdoc/IDocumentService.h>
#include <imtbase/IObjectCollection.h>

// Generated includes
#include <GeneratedFiles/imtauthsdl/SDL/1.0/CPP/RelationshipCollectionDocumentService_fwd.h>


namespace imtauthgql
{


/**
 * Server-side handler for the Relationship collection's per-document representation
 * GraphQL endpoints (analogous to CRoleCollectionDocumentServiceComp).
 *
 * Reads/writes the ITenantRelationshipInfo working copy that the server-side
 * CollectionDocumentManager holds for a given documentId.
 */
class CRelationshipCollectionDocumentServiceComp:
			public sdl::V1_0::imtauth::CRelationshipCollectionDocumentServiceGqlHandlerCompBase
{
public:
	typedef sdl::V1_0::imtauth::CRelationshipCollectionDocumentServiceGqlHandlerCompBase BaseClass;

	I_BEGIN_COMPONENT(CRelationshipCollectionDocumentServiceComp)
		I_ASSIGN(m_documentManagerCompPtr, "CollectionDocumentService", "Collection document manager", true, "CollectionDocumentService");
		I_ASSIGN(m_connectionCollectionCompPtr, "ConnectionCollection", "Tenant connections collection", true, "ConnectionCollection");
		I_ASSIGN(m_tenantCollectionCompPtr, "TenantCollection", "Tenant collection for name resolution", false, "TenantCollection");
	I_END_COMPONENT

protected:
	// reimplemented (CRelationshipCollectionDocumentServiceGqlHandlerCompBase)
	virtual sdl::V1_0::imtauth::CTenantRelationship OnGetRelationshipRepresentation(
				const sdl::V1_0::imtauth::CGetRelationshipRepresentationGqlRequest& getRelationshipRepresentationRequest,
				const ::imtgql::CGqlRequest& gqlRequest,
				QString& errorMessage) const override;
	virtual sdl::V1_0::imtbase::CDocumentOperationStatus OnUpdateRelationshipFromRepresentation(
				const sdl::V1_0::imtauth::CUpdateRelationshipFromRepresentationGqlRequest& updateRelationshipFromRepresentationRequest,
				const ::imtgql::CGqlRequest& gqlRequest,
				QString& errorMessage) const override;

private:
	I_REF(imtdoc::IDocumentService, m_documentManagerCompPtr);
	I_REF(imtbase::IObjectCollection, m_connectionCollectionCompPtr);
	I_REF(imtbase::IObjectCollection, m_tenantCollectionCompPtr);
};


} // namespace imtauthgql
