#include <imtclientgql/CRemoteDocumentRevisionControllerComp.h>


namespace imtclientgql
{


// protected methods

// reimplemented (sdl::imtbase::DocumentRevision::CGraphQlHandlerCompBase)

sdl::imtbase::DocumentRevision::CRevisionInfoList CRemoteDocumentRevisionControllerComp::OnGetRevisionInfoList(
			const sdl::imtbase::DocumentRevision::CGetRevisionInfoListGqlRequest& /*getRevisionInfoListRequest*/,
			const ::imtgql::CGqlRequest& gqlRequest,
			QString& errorMessage) const
{
	sdl::imtbase::DocumentRevision::CRevisionInfoList::V1_0 response;

	if (!SendModelRequest<sdl::imtbase::DocumentRevision::CRevisionInfoList::V1_0, sdl::imtbase::DocumentRevision::CRevisionInfoList>(gqlRequest, response, errorMessage)){
		errorMessage = QString("Unable to get remote revision info list. Error: %1").arg(errorMessage);

		return sdl::imtbase::DocumentRevision::CRevisionInfoList();
	}

	sdl::imtbase::DocumentRevision::CRevisionInfoList retVal;
	retVal.Version_1_0 = std::move(response);

	return retVal;
}


sdl::imtbase::DocumentRevision::CBackupRevisionResponse CRemoteDocumentRevisionControllerComp::OnBackupRevision(
			const sdl::imtbase::DocumentRevision::CBackupRevisionGqlRequest& /*backupRevisionRequest*/,
			const ::imtgql::CGqlRequest& gqlRequest,
			QString& errorMessage) const
{
	sdl::imtbase::DocumentRevision::CBackupRevisionResponse::V1_0 response;

	if (!SendModelRequest<sdl::imtbase::DocumentRevision::CBackupRevisionResponse::V1_0, sdl::imtbase::DocumentRevision::CBackupRevisionResponse>(gqlRequest, response, errorMessage)){
		errorMessage = QString("Unable to remote backup revision. Error: %1").arg(errorMessage);

		return sdl::imtbase::DocumentRevision::CBackupRevisionResponse();
	}

	sdl::imtbase::DocumentRevision::CBackupRevisionResponse retVal;
	retVal.Version_1_0 = std::move(response);

	return retVal;
}


sdl::imtbase::DocumentRevision::CRestoreRevisionResponse CRemoteDocumentRevisionControllerComp::OnRestoreRevision(
			const sdl::imtbase::DocumentRevision::CRestoreRevisionGqlRequest& /*restoreRevisionRequest*/,
			const ::imtgql::CGqlRequest& gqlRequest,
			QString& errorMessage) const
{
	sdl::imtbase::DocumentRevision::CRestoreRevisionResponse::V1_0 response;

	if (!SendModelRequest<sdl::imtbase::DocumentRevision::CRestoreRevisionResponse::V1_0, sdl::imtbase::DocumentRevision::CRestoreRevisionResponse>(gqlRequest, response, errorMessage)){
		errorMessage = QString("Unable to remote restore revision. Error: %1").arg(errorMessage);

		return sdl::imtbase::DocumentRevision::CRestoreRevisionResponse();
	}

	sdl::imtbase::DocumentRevision::CRestoreRevisionResponse retVal;
	retVal.Version_1_0 = std::move(response);

	return retVal;
}


sdl::imtbase::DocumentRevision::CExportRevisionResponse CRemoteDocumentRevisionControllerComp::OnExportRevision(
			const sdl::imtbase::DocumentRevision::CExportRevisionGqlRequest& /*exportRevisionRequest*/,
			const ::imtgql::CGqlRequest& gqlRequest,
			QString& errorMessage) const
{
	sdl::imtbase::DocumentRevision::CExportRevisionResponse::V1_0 response;

	if (!SendModelRequest<sdl::imtbase::DocumentRevision::CExportRevisionResponse::V1_0, sdl::imtbase::DocumentRevision::CExportRevisionResponse>(gqlRequest, response, errorMessage)){
		errorMessage = QString("Unable to remote export revision. Error: %1").arg(errorMessage);
		return sdl::imtbase::DocumentRevision::CExportRevisionResponse();
	}

	sdl::imtbase::DocumentRevision::CExportRevisionResponse retVal;
	retVal.Version_1_0 = std::move(response);

	return retVal;
}


sdl::imtbase::DocumentRevision::CDeleteRevisionResponse CRemoteDocumentRevisionControllerComp::OnDeleteRevision(
			const sdl::imtbase::DocumentRevision::CDeleteRevisionGqlRequest& /*deleteRevisionRequest*/,
			const ::imtgql::CGqlRequest& gqlRequest,
			QString& errorMessage) const
{
	sdl::imtbase::DocumentRevision::CDeleteRevisionResponse::V1_0 response;

	if (!SendModelRequest<sdl::imtbase::DocumentRevision::CDeleteRevisionResponse::V1_0, sdl::imtbase::DocumentRevision::CDeleteRevisionResponse>(gqlRequest, response, errorMessage)){
		errorMessage = QString("Unable to remote delete revision. Error: %1").arg(errorMessage);

		return sdl::imtbase::DocumentRevision::CDeleteRevisionResponse();
	}


	sdl::imtbase::DocumentRevision::CDeleteRevisionResponse retVal;
	retVal.Version_1_0 = std::move(response);

	return retVal;
}


// reimplemented (imtgql::IGqlRequestHandler)

bool CRemoteDocumentRevisionControllerComp::IsRequestSupported(const imtgql::CGqlRequest& gqlRequest) const
{
	bool isSupported = BaseClass::IsRequestSupported(gqlRequest);
	if (!isSupported){
		return false;
	}

	const imtgql::CGqlParamObject* inputObjectPtr = gqlRequest.GetParamObject("input");
	if (inputObjectPtr == nullptr){
		return false;
	}

	QByteArray collectionId = inputObjectPtr->GetParamArgumentValue(
				sdl::imtbase::DocumentRevision::CGetRevisionInfoListInput::V1_0::GetRevisionInfoListInputFields::CollectionId.toUtf8()).toByteArray();
	if (collectionId.isEmpty()){
		return false;
	}

	return m_collectionIdsAttrPtr.FindValue(collectionId) >= 0;
}


} // namespace imtclientgql


