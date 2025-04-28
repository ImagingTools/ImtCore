#pragma once


// ImtCore includes
#include <imtcol/ICollectionImportController.h>
#include <GeneratedFiles/imtbasesdl/SDL/1.0/CPP/CollectionImport.h>


namespace imtservergql
{


namespace collectionImport = sdl::imtbase::CollectionImport;


class CGqlCollectionImportControllerComp: public collectionImport::CGraphQlHandlerCompBase
{
	Q_DECLARE_TR_FUNCTIONS(CGqlCollectionImportControllerComp)
public:
	typedef collectionImport::CGraphQlHandlerCompBase BaseClass;

	I_BEGIN_COMPONENT(CGqlCollectionImportControllerComp);
		I_ASSIGN(m_collectionImportControllerCompPtr, "CollectionImportController", "Collection import controller", true, "CollectionImportController");
	I_END_COMPONENT;

protected:
	// reimplemented (sdl::imtbase::CollectionImport::CGraphQlHandlerCompBase)
	virtual collectionImport::CTransactionStatus OnBeginCollectionImportTransaction(
		const collectionImport::CBeginCollectionImportTransactionGqlRequest& request,
		const ::imtgql::CGqlRequest& gqlRequest,
		QString& errorMessage) const override;
	virtual collectionImport::CTransactionStatus OnCancelCollectionImportTransaction(
		const collectionImport::CCancelCollectionImportTransactionGqlRequest& request,
		const ::imtgql::CGqlRequest& gqlRequest,
		QString& errorMessage) const override;

private:
	I_REF(imtcol::ICollectionImportController, m_collectionImportControllerCompPtr);
};


} // namespace imtservergql


