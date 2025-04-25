#pragma once


// ImtCore includes
#include <GeneratedFiles/imtbasesdl/SDL/1.0/CPP/CollectionImport.h>


namespace imtservergql
{


namespace collectionImport = sdl::imtbase::CollectionImport;


class CCollectionImportControllerComp: public collectionImport::CGraphQlHandlerCompBase
{
public:
	typedef collectionImport::CGraphQlHandlerCompBase BaseClass;

	I_BEGIN_COMPONENT(CCollectionImportControllerComp);
	I_END_COMPONENT;

protected:
	// reimplemented (sdl::imtbase::CollectionImport::CGraphQlHandlerCompBase)
	virtual collectionImport::CTransactionList OnGetCollectionImportTransactionList(
		const collectionImport::CGetCollectionImportTransactionListGqlRequest& request,
		const ::imtgql::CGqlRequest& gqlRequest,
		QString& errorMessage) const override;
	virtual collectionImport::CTransactionId OnBeginCollectionImportTransaction(
		const collectionImport::CBeginCollectionImportTransactionGqlRequest& request,
		const ::imtgql::CGqlRequest& gqlRequest,
		QString& errorMessage) const override;
	virtual collectionImport::CTransactionStatus OnGetCollectionImportTransactionStatus(
		const collectionImport::CGetCollectionImportTransactionStatusGqlRequest& request,
		const ::imtgql::CGqlRequest& gqlRequest,
		QString& errorMessage) const override;
	virtual collectionImport::CTransactionStatus OnResumeCollectionImportTransaction(
		const collectionImport::CResumeCollectionImportTransactionGqlRequest& request,
		const ::imtgql::CGqlRequest& gqlRequest,
		QString& errorMessage) const override;
	virtual collectionImport::CTransactionStatus OnCancelCollectionImportTransaction(
		const collectionImport::CCancelCollectionImportTransactionGqlRequest& request,
		const ::imtgql::CGqlRequest& gqlRequest,
		QString& errorMessage) const override;
	virtual collectionImport::CTransactionStatus OnEndCollectionImportTransaction(
		const collectionImport::CEndCollectionImportTransactionGqlRequest& request,
		const ::imtgql::CGqlRequest& gqlRequest,
		QString& errorMessage) const override;
};


} // namespace imtservergql


