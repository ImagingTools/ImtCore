// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtclientgql/CRemoteDocumentRevisionControllerComp.h>


namespace imtclientgql
{


// protected methods

// reimplemented (sdl::imtbase::DocumentRevision::CGraphQlHandlerCompBase)

sdl::imtbase::DocumentRevision::CRevisionInfoList CRemoteDocumentRevisionControllerComp::OnGetRevisionInfoList(
			const sdl::imtbase::DocumentRevision::CGetRevisionInfoListGqlRequest& /*getRevisionInfoListRequest*/,
			const imtgql::CGqlRequest& gqlRequest,
			QString& errorMessage) const
{
	return SendModelRequest<sdl::imtbase::DocumentRevision::CRevisionInfoList>(gqlRequest, errorMessage);
}


sdl::imtbase::DocumentRevision::CBackupRevisionResponse CRemoteDocumentRevisionControllerComp::OnBackupRevision(
			const sdl::imtbase::DocumentRevision::CBackupRevisionGqlRequest& /*backupRevisionRequest*/,
			const imtgql::CGqlRequest& gqlRequest,
			QString& errorMessage) const
{
	return SendModelRequest<sdl::imtbase::DocumentRevision::CBackupRevisionResponse>(gqlRequest, errorMessage);
}


sdl::imtbase::DocumentRevision::CRestoreRevisionResponse CRemoteDocumentRevisionControllerComp::OnRestoreRevision(
			const sdl::imtbase::DocumentRevision::CRestoreRevisionGqlRequest& /*restoreRevisionRequest*/,
			const imtgql::CGqlRequest& gqlRequest,
			QString& errorMessage) const
{
	return SendModelRequest<sdl::imtbase::DocumentRevision::CRestoreRevisionResponse>(gqlRequest, errorMessage);
}


sdl::imtbase::DocumentRevision::CExportRevisionResponse CRemoteDocumentRevisionControllerComp::OnExportRevision(
			const sdl::imtbase::DocumentRevision::CExportRevisionGqlRequest& /*exportRevisionRequest*/,
			const imtgql::CGqlRequest& gqlRequest,
			QString& errorMessage) const
{
	return SendModelRequest<sdl::imtbase::DocumentRevision::CExportRevisionResponse>(gqlRequest, errorMessage);
}


sdl::imtbase::DocumentRevision::CDeleteRevisionResponse CRemoteDocumentRevisionControllerComp::OnDeleteRevision(
			const sdl::imtbase::DocumentRevision::CDeleteRevisionGqlRequest& /*deleteRevisionRequest*/,
			const imtgql::CGqlRequest& gqlRequest,
			QString& errorMessage) const
{
	return SendModelRequest<sdl::imtbase::DocumentRevision::CDeleteRevisionResponse>(gqlRequest, errorMessage);
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


