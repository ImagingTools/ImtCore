#include <imtclientgql/CRemoteDocumentRevisionControllerComp.h>


namespace imtclientgql
{


// protected methods

// reimplemented (sdl::imtbase::DocumentRevision::V1_0::CGraphQlHandlerCompBase)

sdl::imtbase::DocumentRevision::CRevisionInfoList::V1_0 CRemoteDocumentRevisionControllerComp::OnGetRevisionInfoList(
			const sdl::imtbase::DocumentRevision::V1_0::CGetRevisionInfoListGqlRequest& /*getRevisionInfoListRequest*/,
			const ::imtgql::CGqlRequest& gqlRequest,
			QString& errorMessage) const
{
	sdl::imtbase::DocumentRevision::CRevisionInfoList::V1_0 response;

	if (!SendModelRequest<sdl::imtbase::DocumentRevision::CRevisionInfoList::V1_0, sdl::imtbase::DocumentRevision::CRevisionInfoList>(gqlRequest, response)){
		errorMessage = QString("Unable to get remote revision info list. Error: Sending request is failed");
		return response;
	}

	return response;
}


sdl::imtbase::DocumentRevision::CBackupRevisionResponse::V1_0 CRemoteDocumentRevisionControllerComp::OnBackupRevision(
			const sdl::imtbase::DocumentRevision::V1_0::CBackupRevisionGqlRequest& /*backupRevisionRequest*/,
			const ::imtgql::CGqlRequest& gqlRequest,
			QString& errorMessage) const
{
	sdl::imtbase::DocumentRevision::CBackupRevisionResponse::V1_0 response;

	if (!SendModelRequest<sdl::imtbase::DocumentRevision::CBackupRevisionResponse::V1_0, sdl::imtbase::DocumentRevision::CBackupRevisionResponse>(gqlRequest, response)){
		errorMessage = QString("Unable to remote backup revision. Error: Sending request is failed");
		return response;
	}

	return response;
}


sdl::imtbase::DocumentRevision::CRestoreRevisionResponse::V1_0 CRemoteDocumentRevisionControllerComp::OnRestoreRevision(
			const sdl::imtbase::DocumentRevision::V1_0::CRestoreRevisionGqlRequest& /*restoreRevisionRequest*/,
			const ::imtgql::CGqlRequest& gqlRequest,
			QString& errorMessage) const
{
	sdl::imtbase::DocumentRevision::CRestoreRevisionResponse::V1_0 response;

	if (!SendModelRequest<sdl::imtbase::DocumentRevision::CRestoreRevisionResponse::V1_0, sdl::imtbase::DocumentRevision::CRestoreRevisionResponse>(gqlRequest, response)){
		errorMessage = QString("Unable to remote restore revision. Error: Sending request is failed");
		return response;
	}

	return response;
}


sdl::imtbase::DocumentRevision::CExportRevisionResponse::V1_0 CRemoteDocumentRevisionControllerComp::OnExportRevision(
			const sdl::imtbase::DocumentRevision::V1_0::CExportRevisionGqlRequest& /*exportRevisionRequest*/,
			const ::imtgql::CGqlRequest& gqlRequest,
			QString& errorMessage) const
{
	sdl::imtbase::DocumentRevision::CExportRevisionResponse::V1_0 response;

	if (!SendModelRequest<sdl::imtbase::DocumentRevision::CExportRevisionResponse::V1_0, sdl::imtbase::DocumentRevision::CExportRevisionResponse>(gqlRequest, response)){
		errorMessage = QString("Unable to remote export revision. Error: Sending request is failed");
		return response;
	}

	return response;
}


sdl::imtbase::DocumentRevision::CDeleteRevisionResponse::V1_0 CRemoteDocumentRevisionControllerComp::OnDeleteRevision(
			const sdl::imtbase::DocumentRevision::V1_0::CDeleteRevisionGqlRequest& /*deleteRevisionRequest*/,
			const ::imtgql::CGqlRequest& gqlRequest,
			QString& errorMessage) const
{
	sdl::imtbase::DocumentRevision::CDeleteRevisionResponse::V1_0 response;

	if (!SendModelRequest<sdl::imtbase::DocumentRevision::CDeleteRevisionResponse::V1_0, sdl::imtbase::DocumentRevision::CDeleteRevisionResponse>(gqlRequest, response)){
		errorMessage = QString("Unable to remote delete revision. Error: Sending request is failed");
		return response;
	}

	return response;
}


// reimplemented (imtgql::IGqlRequestHandler)

bool CRemoteDocumentRevisionControllerComp::IsRequestSupported(const imtgql::CGqlRequest& gqlRequest) const
{
	bool isSupported = BaseClass::IsRequestSupported(gqlRequest);
	if (!isSupported){
		return false;
	}

	const imtgql::CGqlObject* inputObjectPtr = gqlRequest.GetParamObject("input");
	if (inputObjectPtr == nullptr){
		return false;
	}

	QByteArray collectionId = inputObjectPtr->GetFieldArgumentValue("CollectionId").toByteArray();
	if (collectionId.isEmpty()){
		return false;
	}

	return m_collectionIdsAttrPtr.FindValue(collectionId) >= 0;
}


} // namespace imtclientgql


