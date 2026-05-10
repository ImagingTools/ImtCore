// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtclientgql/CRemoteCollectionDocumentManagerControllerComp.h>


namespace imtclientgql
{


// protected methods

// reimplemented (sdl::imtbase::CollectionDocumentManager::CGraphQlHandlerCompBase)

sdl::imtbase::CollectionDocumentManager::CDocumentList CRemoteCollectionDocumentManagerControllerComp::OnGetOpenedDocumentList(
			const sdl::imtbase::CollectionDocumentManager::CGetOpenedDocumentListGqlRequest& /*getOpenedDocumentListRequest*/,
			const imtgql::CGqlRequest& gqlRequest,
			QString& errorMessage) const
{
	return SendModelRequest<sdl::imtbase::CollectionDocumentManager::CDocumentList>(gqlRequest, errorMessage);
}


sdl::imtbase::CollectionDocumentManager::CDocumentInfo CRemoteCollectionDocumentManagerControllerComp::OnCreateNewDocument(
			const sdl::imtbase::CollectionDocumentManager::CCreateNewDocumentGqlRequest& /*createNewDocumentRequest*/,
			const imtgql::CGqlRequest& gqlRequest,
			QString& errorMessage) const
{
	return SendModelRequest<sdl::imtbase::CollectionDocumentManager::CDocumentInfo>(gqlRequest, errorMessage);
}


sdl::imtbase::CollectionDocumentManager::CDocumentInfo CRemoteCollectionDocumentManagerControllerComp::OnOpenDocument(
			const sdl::imtbase::CollectionDocumentManager::COpenDocumentGqlRequest& /*openDocumentRequest*/,
			const imtgql::CGqlRequest& gqlRequest,
			QString& errorMessage) const
{
	return SendModelRequest<sdl::imtbase::CollectionDocumentManager::CDocumentInfo>(gqlRequest, errorMessage);
}


sdl::imtbase::CollectionDocumentManager::CDocumentInfo CRemoteCollectionDocumentManagerControllerComp::OnGetDocumentName(
			const sdl::imtbase::CollectionDocumentManager::CGetDocumentNameGqlRequest& /*getDocumentNameRequest*/,
			const imtgql::CGqlRequest& gqlRequest,
			QString& errorMessage) const
{
	return SendModelRequest<sdl::imtbase::CollectionDocumentManager::CDocumentInfo>(gqlRequest, errorMessage);
}


sdl::imtbase::CollectionDocumentManager::CDocumentOperationStatus CRemoteCollectionDocumentManagerControllerComp::OnSetDocumentName(
			const sdl::imtbase::CollectionDocumentManager::CSetDocumentNameGqlRequest& /*setDocumentNameRequest*/,
			const imtgql::CGqlRequest& gqlRequest,
			QString& errorMessage) const
{
	return SendModelRequest<sdl::imtbase::CollectionDocumentManager::CDocumentOperationStatus>(gqlRequest, errorMessage);
}


sdl::imtbase::CollectionDocumentManager::CDocumentOperationStatus CRemoteCollectionDocumentManagerControllerComp::OnSaveDocument(
			const sdl::imtbase::CollectionDocumentManager::CSaveDocumentGqlRequest& /*saveDocumentRequest*/,
			const imtgql::CGqlRequest& gqlRequest,
			QString& errorMessage) const
{
	return SendModelRequest<sdl::imtbase::CollectionDocumentManager::CDocumentOperationStatus>(gqlRequest, errorMessage);
}


sdl::imtbase::CollectionDocumentManager::CDocumentOperationStatus CRemoteCollectionDocumentManagerControllerComp::OnCloseDocument(
			const sdl::imtbase::CollectionDocumentManager::CCloseDocumentGqlRequest& /*closeDocumentRequest*/,
			const imtgql::CGqlRequest& gqlRequest,
			QString& errorMessage) const
{
	return SendModelRequest<sdl::imtbase::CollectionDocumentManager::CDocumentOperationStatus>(gqlRequest, errorMessage);
}


sdl::imtbase::UndoManager::CUndoInfo CRemoteCollectionDocumentManagerControllerComp::OnGetUndoInfo(
			const sdl::imtbase::CollectionDocumentManager::CGetUndoInfoGqlRequest& /*getUndoInfoRequest*/,
			const imtgql::CGqlRequest& gqlRequest,
			QString& errorMessage) const
{
	return SendModelRequest<sdl::imtbase::UndoManager::CUndoInfo>(gqlRequest, errorMessage);
}


sdl::imtbase::UndoManager::CUndoStatus CRemoteCollectionDocumentManagerControllerComp::OnDoUndo(
			const sdl::imtbase::CollectionDocumentManager::CDoUndoGqlRequest& /*doUndoRequest*/,
			const imtgql::CGqlRequest& gqlRequest,
			QString& errorMessage) const
{
	return SendModelRequest<sdl::imtbase::UndoManager::CUndoStatus>(gqlRequest, errorMessage);
}


sdl::imtbase::UndoManager::CUndoStatus CRemoteCollectionDocumentManagerControllerComp::OnDoRedo(
			const sdl::imtbase::CollectionDocumentManager::CDoRedoGqlRequest& /*doRedoRequest*/,
			const imtgql::CGqlRequest& gqlRequest,
			QString& errorMessage) const
{
	return SendModelRequest<sdl::imtbase::UndoManager::CUndoStatus>(gqlRequest, errorMessage);
}


sdl::imtbase::UndoManager::CUndoStatus CRemoteCollectionDocumentManagerControllerComp::OnResetUndo(
			const sdl::imtbase::CollectionDocumentManager::CResetUndoGqlRequest& /*resetUndoRequest*/,
			const imtgql::CGqlRequest& gqlRequest,
			QString& errorMessage) const
{
	return SendModelRequest<sdl::imtbase::UndoManager::CUndoStatus>(gqlRequest, errorMessage);
}


// reimplemented (imtgql::IGqlRequestHandler)

bool CRemoteCollectionDocumentManagerControllerComp::IsRequestSupported(const imtgql::CGqlRequest& gqlRequest) const
{
	bool isSupported = BaseClass::IsRequestSupported(gqlRequest);
	if (!isSupported){
		return false;
	}

	const imtgql::CGqlParamObject* inputParamPtr = gqlRequest.GetParamObject("input");
	if (inputParamPtr == nullptr){
		return false;
	}

	QByteArray collectionId = inputParamPtr->GetParamArgumentValue("collectionId").toByteArray();
	if (collectionId.isEmpty()){
		return false;
	}

	return m_collectionIdsAttrPtr.FindValue(collectionId) >= 0;
}


} // namespace imtclientgql


