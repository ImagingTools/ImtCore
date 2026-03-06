#pragma once


// ImtCore includes
#include <imtdoc/IDocumentManager.h>
#include <imtbasesdl/SDL/1.0/CPP/CollectionDocumentManager.h>

// ControlsGallery includes
#include <controlsgallerysdl/SDL/1.0/CPP/ContactInfoCollectionDocumentManager.h>



namespace controlsgallerygql
{


class CContactInfoCollectionDocumentManagerComp: public sdl::controlsgallery::ContactInfoCollectionDocumentManager::CGraphQlHandlerCompBase
{
public:
	typedef sdl::controlsgallery::ContactInfoCollectionDocumentManager::CGraphQlHandlerCompBase BaseClass;

	I_BEGIN_COMPONENT(CContactInfoCollectionDocumentManagerComp)
		I_ASSIGN(m_documentManagerCompPtr, "CollectionDocumentManager", "Collection document manager", false, "CollectionDocumentManager");
	I_END_COMPONENT

protected:
	virtual sdl::controlsgallery::ContactInfos::CContactInfoData OnGetContactInfoRepresentation(
				const sdl::controlsgallery::ContactInfoCollectionDocumentManager::CGetContactInfoRepresentationGqlRequest& getContactInfoRepresentationRequest,
				const ::imtgql::CGqlRequest& gqlRequest,
				QString& errorMessage) const override;
	virtual sdl::imtbase::CollectionDocumentManager::CDocumentOperationStatus OnUpdateContactInfoFromRepresentation(
				const sdl::controlsgallery::ContactInfoCollectionDocumentManager::CUpdateContactInfoFromRepresentationGqlRequest& updateContactInfoFromRepresentationRequest,
				const ::imtgql::CGqlRequest& gqlRequest,
				QString& errorMessage) const override;
	virtual sdl::controlsgallery::ContactInfoCollectionDocumentManager::CEmailData OnGetContactInfoEmailRepresentation(
				const sdl::controlsgallery::ContactInfoCollectionDocumentManager::CGetContactInfoEmailRepresentationGqlRequest& getContactInfoEmailRepresentationRequest,
				const ::imtgql::CGqlRequest& gqlRequest,
				QString& errorMessage) const override;
	virtual sdl::imtbase::CollectionDocumentManager::CDocumentOperationStatus OnUpdateContactInfoEmailFromRepresentation(
				const sdl::controlsgallery::ContactInfoCollectionDocumentManager::CUpdateContactInfoEmailFromRepresentationGqlRequest& updateContactInfoEmailFromRepresentationRequest,
				const ::imtgql::CGqlRequest& gqlRequest,
				QString& errorMessage) const override;

private:
	I_REF(imtdoc::IDocumentManager, m_documentManagerCompPtr);
};


} // namespace controlsgallerygql
