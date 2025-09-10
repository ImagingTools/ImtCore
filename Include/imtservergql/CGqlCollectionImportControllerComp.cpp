#include <imtservergql/CGqlCollectionImportControllerComp.h>


namespace imtservergql
{


// protected methods


template<typename GqlRequest>
bool CGqlCollectionImportControllerComp::FillSessionInfo(
	const GqlRequest& request,
	imtservergql::ICollectionImportController::SessionInfo& sessionInfo,
	QString& errorMessage)
{
	auto sdlImportParams = request.GetRequestedArguments().input;
	if (sdlImportParams.Version_1_0){
		auto sdlImportParamsV1_0 = *sdlImportParams.Version_1_0;

		if (sdlImportParamsV1_0.sessionId && sdlImportParamsV1_0.collectionId && sdlImportParamsV1_0.fileList && sdlImportParamsV1_0.fileList->count() > 0){
			sessionInfo.sessionId = *sdlImportParamsV1_0.sessionId;
			sessionInfo.collectionId = *sdlImportParamsV1_0.collectionId;

			for (const auto& fileInfo : *sdlImportParamsV1_0.fileList){
				if (fileInfo.fileId && fileInfo.fileName && fileInfo.fileSize && fileInfo.objectTypeId){
					sessionInfo.files.emplace_back();
					sessionInfo.files.back().id = *fileInfo.fileId;
					sessionInfo.files.back().name = *fileInfo.fileName;
					sessionInfo.files.back().size = *fileInfo.fileSize;
					sessionInfo.files.back().objectTypeId = *fileInfo.objectTypeId;
				}
				else{
					errorMessage = tr("Invalid file info in the request");

					return false;
				}
			}

			return true;
		}
		else{
			errorMessage = tr("Invalid import request");
		}
	}
	else{
		errorMessage = tr("Invalid import request");
	}

	return false;
}


template<typename GqlRequest>
QByteArray CGqlCollectionImportControllerComp::GetCancelSessionId(
	const GqlRequest& request)
{
	auto sdlSessionId = request.GetRequestedArguments().input;
	if (sdlSessionId.Version_1_0 && sdlSessionId.Version_1_0->sessionId){
		QByteArray sessionId = *sdlSessionId.Version_1_0->sessionId;

		return sessionId;
	}

	return QByteArray();
}


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

	imtservergql::ICollectionImportController::SessionInfo sessionInfo;
	if (FillSessionInfo(request, sessionInfo, errorMessage)){
		if (m_collectionImportControllerCompPtr->BeginCollectionImportSession(sessionInfo, errorMessage)){
			retVal.Version_1_0->status = collectionImport::Status::Success;
		}
	}

	return retVal;
}


collectionImport::CSessionStatus CGqlCollectionImportControllerComp::OnCancelCollectionImportSession(
	const collectionImport::CCancelCollectionImportSessionGqlRequest& request,
	const::imtgql::CGqlRequest& /*gqlRequest*/,
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

	QByteArray sessionId = GetCancelSessionId(request);
	if (!sessionId.isEmpty()){
		if (m_collectionImportControllerCompPtr->CancelCollectionImportSession(sessionId, errorMessage)){
			retVal.Version_1_0->status = collectionImport::Status::Success;
		}
	}
	else{
		errorMessage = tr("Invalid import request");
	}

	return retVal;
}


} // namespace imtservergql


