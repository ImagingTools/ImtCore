#include <imtservergql/CGqlCollectionImportControllerComp.h>


namespace imtservergql
{


// protected methods

// reimplemented (sdl::imtbase::CollectionImport::CGraphQlHandlerCompBase)

collectionImport::CSessionStatus CGqlCollectionImportControllerComp::OnBeginCollectionImportSession(
	const collectionImport::CBeginCollectionImportSessionGqlRequest& request,
	const ::imtgql::CGqlRequest& /*gqlRequest*/,
	QString& errorMessage) const
{
	collectionImport::CSessionStatus retVal;

	Q_ASSERT(m_collectionImportControllerCompPtr.IsValid());
	if (!m_collectionImportControllerCompPtr.IsValid()){
		errorMessage = tr("Collection import controller unavailable");

		return retVal;
	}

	retVal.Version_1_0.emplace();
	retVal.Version_1_0->status = collectionImport::Status::Failed;

	collectionImport::CImportParams sdlImportParams = request.GetRequestedArguments().input;
	if (sdlImportParams.Version_1_0){
		collectionImport::CImportParams::V1_0 sdlImportParamsV1_0 = *sdlImportParams.Version_1_0;

		if (sdlImportParamsV1_0.sessionId && sdlImportParamsV1_0.collectionId && sdlImportParamsV1_0.fileList && sdlImportParamsV1_0.fileList->count() > 0){
			imtservergql::ICollectionImportController::SessionInfo session;
			session.sessionId = *sdlImportParamsV1_0.sessionId;
			session.collectionId = *sdlImportParamsV1_0.collectionId;

			retVal.Version_1_0->status = collectionImport::Status::Success;

			for (const collectionImport::CFileInfo::V1_0& fileInfo : *sdlImportParamsV1_0.fileList){
				if (fileInfo.fileId && fileInfo.fileName && fileInfo.fileSize && fileInfo.objectTypeId){
					session.files.emplace_back();
					session.files.back().id = *fileInfo.fileId;
					session.files.back().name = *fileInfo.fileName;
					session.files.back().size = *fileInfo.fileSize;
					session.files.back().objectTypeId = *fileInfo.objectTypeId;
				}
				else{
					retVal.Version_1_0->status = collectionImport::Status::Failed;

					errorMessage = tr("Invalid file info in the request");

					break;
				}
			}

			if (session.files.size() == size_t(sdlImportParamsV1_0.fileList->count())){
				if (!m_collectionImportControllerCompPtr->BeginCollectionImportSession(session, errorMessage)){
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


collectionImport::CSessionStatus CGqlCollectionImportControllerComp::OnCancelCollectionImportSession(
	const collectionImport::CCancelCollectionImportSessionGqlRequest& request,
	const::imtgql::CGqlRequest& /*gqlRequest*/,
	QString& errorMessage) const
{
	Q_ASSERT(m_collectionImportControllerCompPtr.IsValid());

	collectionImport::CSessionStatus retVal;
	retVal.Version_1_0.emplace();
	retVal.Version_1_0->status = collectionImport::Status::Failed;

	collectionImport::CSessionId sdlSessionId = request.GetRequestedArguments().input;
	if (sdlSessionId.Version_1_0 && sdlSessionId.Version_1_0->sessionId){
		QByteArray sessionId = *sdlSessionId.Version_1_0->sessionId;

		if (m_collectionImportControllerCompPtr->CancelCollectionImportSession(sessionId, errorMessage)){
			retVal.Version_1_0->status = collectionImport::Status::Success;
		}
	}

	return retVal;
}


} // namespace imtservergql


