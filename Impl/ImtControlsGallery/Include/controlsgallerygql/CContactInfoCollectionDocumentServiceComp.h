#pragma once


// ImtCore includes
#include <imtdoc/IDocumentService.h>
#include <imtbasesdl/SDL/1.0/CPP/CollectionDocumentService.h>

// ControlsGallery includes
#include <controlsgallerysdl/SDL/1.0/CPP/ContactInfoCollectionDocumentService.h>



namespace controlsgallerygql
{


class CContactInfoCollectionDocumentServiceComp: public sdl::controlsgallery::ContactInfoCollectionDocumentService::CGraphQlHandlerCompBase
{
public:
	typedef sdl::controlsgallery::ContactInfoCollectionDocumentService::CGraphQlHandlerCompBase BaseClass;

	I_BEGIN_COMPONENT(CContactInfoCollectionDocumentServiceComp)
		I_ASSIGN(m_documentManagerCompPtr, "CollectionDocumentService", "Collection document manager", false, "CollectionDocumentService");
	I_END_COMPONENT

protected:
	virtual sdl::controlsgallery::ContactInfos::CContactInfoData OnGetContactInfoRepresentation(
				const sdl::controlsgallery::ContactInfoCollectionDocumentService::CGetContactInfoRepresentationGqlRequest& getContactInfoRepresentationRequest,
				const ::imtgql::CGqlRequest& gqlRequest,
				QString& errorMessage) const override;
	virtual sdl::imtbase::CollectionDocumentService::CDocumentOperationStatus OnUpdateContactInfoFromRepresentation(
				const sdl::controlsgallery::ContactInfoCollectionDocumentService::CUpdateContactInfoFromRepresentationGqlRequest& updateContactInfoFromRepresentationRequest,
				const ::imtgql::CGqlRequest& gqlRequest,
				QString& errorMessage) const override;
	virtual sdl::controlsgallery::ContactInfoCollectionDocumentService::CEmailData OnGetContactInfoEmailRepresentation(
				const sdl::controlsgallery::ContactInfoCollectionDocumentService::CGetContactInfoEmailRepresentationGqlRequest& getContactInfoEmailRepresentationRequest,
				const ::imtgql::CGqlRequest& gqlRequest,
				QString& errorMessage) const override;
	virtual sdl::imtbase::CollectionDocumentService::CDocumentOperationStatus OnUpdateContactInfoEmailFromRepresentation(
				const sdl::controlsgallery::ContactInfoCollectionDocumentService::CUpdateContactInfoEmailFromRepresentationGqlRequest& updateContactInfoEmailFromRepresentationRequest,
				const ::imtgql::CGqlRequest& gqlRequest,
				QString& errorMessage) const override;

private:
	I_REF(imtdoc::IDocumentService, m_documentManagerCompPtr);
};


} // namespace controlsgallerygql
