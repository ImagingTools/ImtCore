// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ImtCore includes
#include <imtdoc/IDocumentService.h>
#include <imtauth/ITenantConnectionRequestManager.h>
#include <imtauth/ITenantManager.h>

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
		I_ASSIGN(m_documentManagerCompPtr, "CollectionDocumentService", "Collection document manager", false, "CollectionDocumentService");
		I_ASSIGN(m_connectionRequestManagerCompPtr, "ConnectionRequestManager", "Connection request manager for relationship data", false, "TenantConnectionRequestManager");
		I_ASSIGN(m_tenantManagerCompPtr, "TenantManager", "Tenant manager for relationship updates", false, "TenantManager");
		I_ASSIGN(m_proposalFactoryCompPtr, "ProposalFactory", "Relationship proposal factory", false, "TenantRelationshipProposalInfo");
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
	I_REF(imtauth::ITenantConnectionRequestManager, m_connectionRequestManagerCompPtr);
	I_REF(imtauth::ITenantManager, m_tenantManagerCompPtr);
	I_FACT(imtauth::ITenantRelationshipProposalInfo, m_proposalFactoryCompPtr);
};


} // namespace imtauthgql
