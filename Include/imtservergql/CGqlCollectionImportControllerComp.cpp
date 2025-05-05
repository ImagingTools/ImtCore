#include <imtservergql/CGqlCollectionImportControllerComp.h>


namespace imtservergql
{


// protected methods

// reimplemented (sdl::imtbase::CollectionImport::CGraphQlHandlerCompBase)

collectionImport::CTransactionStatus CGqlCollectionImportControllerComp::OnBeginCollectionImportTransaction(
	const collectionImport::CBeginCollectionImportTransactionGqlRequest& request,
	const ::imtgql::CGqlRequest& gqlRequest,
	QString& errorMessage) const
{
	Q_ASSERT(m_collectionImportControllerCompPtr.IsValid());

	collectionImport::CTransactionStatus retVal;
	retVal.Version_1_0.emplace();
	retVal.Version_1_0->status = collectionImport::Status::Failed;

	collectionImport::CImportParams sdlImportParams = request.GetRequestedArguments().input;
	if (sdlImportParams.Version_1_0){
		collectionImport::CImportParams::V1_0 sdlImportParamsV1_0 = *sdlImportParams.Version_1_0;

		if (sdlImportParamsV1_0.transactionId && sdlImportParamsV1_0.collectionId && sdlImportParamsV1_0.fileList && sdlImportParamsV1_0.fileList->count() > 0){
			imtcol::ICollectionImportController::TransactionInfo transaction;
			transaction.transactionId = *sdlImportParamsV1_0.transactionId;
			transaction.collectionId = *sdlImportParamsV1_0.collectionId;

			retVal.Version_1_0->status = collectionImport::Status::Success;

			for (const collectionImport::CFileInfo::V1_0& fileInfo : *sdlImportParamsV1_0.fileList){
				if (fileInfo.fileId && fileInfo.fileName && fileInfo.fileSize && fileInfo.objectTypeId){
					transaction.files.emplace_back();
					transaction.files.back().id = *fileInfo.fileId;
					transaction.files.back().name = *fileInfo.fileName;
					transaction.files.back().size = *fileInfo.fileSize;
					transaction.files.back().objectTypeId = *fileInfo.objectTypeId;
				}
				else{
					retVal.Version_1_0->status = collectionImport::Status::Failed;

					errorMessage = tr("Invalid file info in the request");

					break;
				}
			}

			if (transaction.files.size() == sdlImportParamsV1_0.fileList->count()){
				if (!m_collectionImportControllerCompPtr->BeginCollectionImportTransaction(transaction, errorMessage)){
					retVal.Version_1_0->status = collectionImport::Status::Failed;
				}
			}
		}
		else{
			errorMessage = tr("Invalid import request");
		}
	}
	else{
		errorMessage = tr("Invalid import request");
	}

	return retVal;
}


collectionImport::CTransactionStatus CGqlCollectionImportControllerComp::OnCancelCollectionImportTransaction(
	const collectionImport::CCancelCollectionImportTransactionGqlRequest& request,
	const::imtgql::CGqlRequest& gqlRequest,
	QString& errorMessage) const
{
	Q_ASSERT(m_collectionImportControllerCompPtr.IsValid());

	collectionImport::CTransactionStatus retVal;
	retVal.Version_1_0.emplace();
	retVal.Version_1_0->status = collectionImport::Status::Failed;

	collectionImport::CTransactionId sdlTransactionId = request.GetRequestedArguments().input;
	if (sdlTransactionId.Version_1_0 && sdlTransactionId.Version_1_0->transactionId){
		QByteArray transactionId = *sdlTransactionId.Version_1_0->transactionId;

		if (m_collectionImportControllerCompPtr->CancelCollectionImportTransaction(transactionId, errorMessage)){
			retVal.Version_1_0->status = collectionImport::Status::Success;
		}
	}

	return retVal;
}


} // namespace imtservergql


