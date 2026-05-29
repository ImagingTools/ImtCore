// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtclientgql/CRemoteDocumentRevisionControllerComp.h>
#include <GeneratedFiles/imtbasesdl/SDL/1.0/CPP/DocumentRevision.h>


namespace imtclientgql
{


// protected methods

// reimplemented (sdl::V1_0::imtbase::CGraphQlHandlerCompBase)

sdl::V1_0::imtbase::CRevisionInfoList CRemoteDocumentRevisionControllerComp::OnGetRevisionInfoList(
			const sdl::V1_0::imtbase::CGetRevisionInfoListGqlRequest& /*getRevisionInfoListRequest*/,
			const imtgql::CGqlRequest& gqlRequest,
			QString& errorMessage) const
{
	return SendModelRequest<sdl::V1_0::imtbase::CRevisionInfoList>(gqlRequest, errorMessage);
}


sdl::V1_0::imtbase::CBackupRevisionResponse CRemoteDocumentRevisionControllerComp::OnBackupRevision(
			const sdl::V1_0::imtbase::CBackupRevisionGqlRequest& /*backupRevisionRequest*/,
			const imtgql::CGqlRequest& gqlRequest,
			QString& errorMessage) const
{
	return SendModelRequest<sdl::V1_0::imtbase::CBackupRevisionResponse>(gqlRequest, errorMessage);
}


sdl::V1_0::imtbase::CRestoreRevisionResponse CRemoteDocumentRevisionControllerComp::OnRestoreRevision(
			const sdl::V1_0::imtbase::CRestoreRevisionGqlRequest& /*restoreRevisionRequest*/,
			const imtgql::CGqlRequest& gqlRequest,
			QString& errorMessage) const
{
	return SendModelRequest<sdl::V1_0::imtbase::CRestoreRevisionResponse>(gqlRequest, errorMessage);
}


sdl::V1_0::imtbase::CExportRevisionResponse CRemoteDocumentRevisionControllerComp::OnExportRevision(
			const sdl::V1_0::imtbase::CExportRevisionGqlRequest& /*exportRevisionRequest*/,
			const imtgql::CGqlRequest& gqlRequest,
			QString& errorMessage) const
{
	return SendModelRequest<sdl::V1_0::imtbase::CExportRevisionResponse>(gqlRequest, errorMessage);
}


sdl::V1_0::imtbase::CDeleteRevisionResponse CRemoteDocumentRevisionControllerComp::OnDeleteRevision(
			const sdl::V1_0::imtbase::CDeleteRevisionGqlRequest& /*deleteRevisionRequest*/,
			const imtgql::CGqlRequest& gqlRequest,
			QString& errorMessage) const
{
	return SendModelRequest<sdl::V1_0::imtbase::CDeleteRevisionResponse>(gqlRequest, errorMessage);
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
				sdl::V1_0::imtbase::CGetRevisionInfoListInput::GetRevisionInfoListInputFields::CollectionId.toUtf8()).toByteArray();
	if (collectionId.isEmpty()){
		return false;
	}

	return m_collectionIdsAttrPtr.FindValue(collectionId) >= 0;
}


} // namespace imtclientgql


