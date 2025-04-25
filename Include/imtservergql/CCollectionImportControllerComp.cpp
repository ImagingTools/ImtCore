#include <imtservergql/CCollectionImportControllerComp.h>


namespace imtservergql
{


// protected methods

// reimplemented (sdl::imtbase::CollectionImport::CGraphQlHandlerCompBase)

collectionImport::CTransactionList CCollectionImportControllerComp::OnGetCollectionImportTransactionList(
	const collectionImport::CGetCollectionImportTransactionListGqlRequest& request,
	const::imtgql::CGqlRequest& gqlRequest,
	QString& errorMessage) const
{
	return collectionImport::CTransactionList();
}


collectionImport::CTransactionId CCollectionImportControllerComp::OnBeginCollectionImportTransaction(const collectionImport::CBeginCollectionImportTransactionGqlRequest& request, const::imtgql::CGqlRequest& gqlRequest, QString& errorMessage) const
{
	return collectionImport::CTransactionId();
}


collectionImport::CTransactionStatus CCollectionImportControllerComp::OnGetCollectionImportTransactionStatus(
	const collectionImport::CGetCollectionImportTransactionStatusGqlRequest& request,
	const::imtgql::CGqlRequest& gqlRequest,
	QString& errorMessage) const
{
	return collectionImport::CTransactionStatus();
}


collectionImport::CTransactionStatus CCollectionImportControllerComp::OnResumeCollectionImportTransaction(
	const collectionImport::CResumeCollectionImportTransactionGqlRequest& request,
	const::imtgql::CGqlRequest& gqlRequest,
	QString& errorMessage) const
{
	return collectionImport::CTransactionStatus();
}


collectionImport::CTransactionStatus CCollectionImportControllerComp::OnCancelCollectionImportTransaction(
	const collectionImport::CCancelCollectionImportTransactionGqlRequest& request,
	const::imtgql::CGqlRequest& gqlRequest,
	QString& errorMessage) const
{
	return collectionImport::CTransactionStatus();
}


collectionImport::CTransactionStatus CCollectionImportControllerComp::OnEndCollectionImportTransaction(
	const collectionImport::CEndCollectionImportTransactionGqlRequest& request,
	const::imtgql::CGqlRequest& gqlRequest,
	QString& errorMessage) const
{
	return collectionImport::CTransactionStatus();
}


} // namespace imtservergql


