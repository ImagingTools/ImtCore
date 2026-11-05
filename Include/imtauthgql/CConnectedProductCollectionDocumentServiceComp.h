// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ImtCore includes
#include <imtdoc/IDocumentService.h>
#include <GeneratedFiles/imtauthsdl/SDL/1.0/CPP/ConnectedProductCollectionDocumentService_fwd.h>


namespace imtauthgql
{


/**
	Representation of the working copy of a catalog entry of a product. Available to a superuser only.
*/
class CConnectedProductCollectionDocumentServiceComp:
			public sdl::V1_0::imtauth::CConnectedProductCollectionDocumentServiceGqlHandlerCompBase
{
public:
	typedef sdl::V1_0::imtauth::CConnectedProductCollectionDocumentServiceGqlHandlerCompBase BaseClass;

	I_BEGIN_COMPONENT(CConnectedProductCollectionDocumentServiceComp)
		I_ASSIGN(m_documentManagerCompPtr, "CollectionDocumentService", "Collection document manager", false, "CollectionDocumentService");
	I_END_COMPONENT

protected:
	// reimplemented (CConnectedProductCollectionDocumentServiceGqlHandlerCompBase)
	virtual sdl::V1_0::imtauth::CConnectedProductData OnGetConnectedProductRepresentation(
				const sdl::V1_0::imtauth::CGetConnectedProductRepresentationGqlRequest& getRepresentationRequest,
				const ::imtgql::CGqlRequest& gqlRequest,
				QString& errorMessage) const override;
	virtual sdl::V1_0::imtbase::CDocumentOperationStatus OnUpdateConnectedProductFromRepresentation(
				const sdl::V1_0::imtauth::CUpdateConnectedProductFromRepresentationGqlRequest& updateRequest,
				const ::imtgql::CGqlRequest& gqlRequest,
				QString& errorMessage) const override;

private:
	I_REF(imtdoc::IDocumentService, m_documentManagerCompPtr);
};


} // namespace imtauthgql
