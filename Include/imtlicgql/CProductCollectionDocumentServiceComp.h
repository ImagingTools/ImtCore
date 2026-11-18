// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ImtCore includes
#include <imtdoc/IDocumentService.h>
#include <imtbase/IObjectCollection.h>
#include <GeneratedFiles/imtbasesdl/SDL/1.0/CPP/CollectionDocumentService_fwd.h>
#include <GeneratedFiles/imtlicsdl/SDL/1.0/CPP/ProductCollectionDocumentService_fwd.h>


namespace imtlicgql
{


/**
	Reads and writes the representation of a product opened in the server document service.
	Several pages show products, each with its own document service: a request is handled by the one
	of its collection.
*/
class CProductCollectionDocumentServiceComp: public sdl::V1_0::imtlic::CProductCollectionDocumentServiceGqlHandlerCompBase
{
public:
	typedef sdl::V1_0::imtlic::CProductCollectionDocumentServiceGqlHandlerCompBase BaseClass;

	I_BEGIN_COMPONENT(CProductCollectionDocumentServiceComp)
		I_ASSIGN(m_documentServiceCompPtr, "CollectionDocumentService", "Document service of the products", true, "CollectionDocumentService");
		I_ASSIGN(m_featureCollectionCompPtr, "FeatureCollection", "Features: a product gets the features it is given and the ones they require", true, "FeatureCollection");
		I_ASSIGN(m_collectionIdAttrPtr, "CollectionId", "ID of the collection whose documents this service holds", true, "");
	I_END_COMPONENT

protected:
	// reimplemented (imtservergql::CGqlRequestHandlerCompBase)
	virtual bool IsRequestSupported(const imtgql::CGqlRequest& gqlRequest) const override;

	// reimplemented (sdl::V1_0::imtlic::CProductCollectionDocumentServiceGqlHandlerCompBase)
	virtual sdl::V1_0::imtlic::CProductData OnGetProductRepresentation(
				const sdl::V1_0::imtlic::CGetProductRepresentationGqlRequest& getRepresentationRequest,
				const ::imtgql::CGqlRequest& gqlRequest,
				QString& errorMessage) const override;
	virtual sdl::V1_0::imtbase::CDocumentOperationStatus OnUpdateProductFromRepresentation(
				const sdl::V1_0::imtlic::CUpdateProductFromRepresentationGqlRequest& updateRequest,
				const ::imtgql::CGqlRequest& gqlRequest,
				QString& errorMessage) const override;

private:
	I_REF(imtdoc::IDocumentService, m_documentServiceCompPtr);
	I_REF(imtbase::IObjectCollection, m_featureCollectionCompPtr);
	I_ATTR(QByteArray, m_collectionIdAttrPtr);
};


} // namespace imtlicgql


