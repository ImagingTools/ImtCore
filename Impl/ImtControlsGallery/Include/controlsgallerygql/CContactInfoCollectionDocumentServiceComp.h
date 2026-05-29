#pragma once


// ImtCore includes
#include <imtdoc/IDocumentService.h>
#include <imtbasesdl/SDL/1.0/CPP/CollectionDocumentService.h>

// ControlsGallery includes
#include <controlsgallerysdl/SDL/1.0/CPP/ContactInfoCollectionDocumentService.h>



namespace controlsgallerygql
{


class CContactInfoCollectionDocumentServiceComp: public sdl::V1_0::controlsgallery::CGraphQlHandlerCompBase
{
public:
	typedef sdl::V1_0::controlsgallery::CGraphQlHandlerCompBase BaseClass;

	I_BEGIN_COMPONENT(CContactInfoCollectionDocumentServiceComp)
		I_ASSIGN(m_documentManagerCompPtr, "CollectionDocumentService", "Collection document manager", false, "CollectionDocumentService");
	I_END_COMPONENT

protected:
	virtual sdl::V1_0::controlsgallery::CContactInfoData OnGetContactInfoRepresentation(
				const sdl::V1_0::controlsgallery::CGetContactInfoRepresentationGqlRequest& getContactInfoRepresentationRequest,
				const ::imtgql::CGqlRequest& gqlRequest,
				QString& errorMessage) const override;
	virtual sdl::V1_0::imtbase::CDocumentOperationStatus OnUpdateContactInfoFromRepresentation(
				const sdl::V1_0::controlsgallery::CUpdateContactInfoFromRepresentationGqlRequest& updateContactInfoFromRepresentationRequest,
				const ::imtgql::CGqlRequest& gqlRequest,
				QString& errorMessage) const override;
	virtual sdl::V1_0::controlsgallery::CEmailData OnGetContactInfoEmailRepresentation(
				const sdl::V1_0::controlsgallery::CGetContactInfoEmailRepresentationGqlRequest& getContactInfoEmailRepresentationRequest,
				const ::imtgql::CGqlRequest& gqlRequest,
				QString& errorMessage) const override;
	virtual sdl::V1_0::imtbase::CDocumentOperationStatus OnUpdateContactInfoEmailFromRepresentation(
				const sdl::V1_0::controlsgallery::CUpdateContactInfoEmailFromRepresentationGqlRequest& updateContactInfoEmailFromRepresentationRequest,
				const ::imtgql::CGqlRequest& gqlRequest,
				QString& errorMessage) const override;

private:
	I_REF(imtdoc::IDocumentService, m_documentManagerCompPtr);
};


} // namespace controlsgallerygql
