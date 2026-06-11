// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ImtCore includes
#include <imtdoc/IDocumentService.h>
#include <imtcrypt/IHashGenerator.h>

// Generated includes
#include <GeneratedFiles/imtauthsdl/SDL/1.0/CPP/UserCollectionDocumentService_fwd.h>


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
			public sdl::V1_0::imtauth::CUserCollectionDocumentServiceGqlHandlerCompBase
{
public:
	typedef sdl::V1_0::imtauth::CUserCollectionDocumentServiceGqlHandlerCompBase BaseClass;

	I_BEGIN_COMPONENT(CUserCollectionDocumentServiceComp)
		I_ASSIGN(m_documentManagerCompPtr, "CollectionDocumentService", "Collection document manager", false, "CollectionDocumentService");
		I_ASSIGN(m_hashCalculatorCompPtr, "HashCalculator", "Hash calculator for password hashing", true, "HashCalculator");
	I_END_COMPONENT

protected:
	// reimplemented (CUserCollectionDocumentServiceGqlHandlerCompBase)
	virtual sdl::V1_0::imtauth::CUserData OnGetUserRepresentation(
				const sdl::V1_0::imtauth::CGetUserRepresentationGqlRequest& getUserRepresentationRequest,
				const ::imtgql::CGqlRequest& gqlRequest,
				QString& errorMessage) const override;
	virtual sdl::V1_0::imtbase::CDocumentOperationStatus OnUpdateUserFromRepresentation(
				const sdl::V1_0::imtauth::CUpdateUserFromRepresentationGqlRequest& updateUserFromRepresentationRequest,
				const ::imtgql::CGqlRequest& gqlRequest,
				QString& errorMessage) const override;

private:
	I_REF(imtdoc::IDocumentService, m_documentManagerCompPtr);
	I_REF(imtcrypt::IHashGenerator, m_hashCalculatorCompPtr);
};


} // namespace imtauthgql
