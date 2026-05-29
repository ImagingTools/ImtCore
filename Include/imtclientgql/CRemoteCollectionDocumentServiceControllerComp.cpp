// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtclientgql/CRemoteCollectionDocumentServiceControllerComp.h>
#include <imtbasesdl/SDL/1.0/CPP/CollectionDocumentService.h>
#include <imtbasesdl/SDL/1.0/CPP/UndoManager.h>


namespace imtclientgql
{


// protected methods

// reimplemented (sdl::imtbase::CollectionDocumentService::CGraphQlHandlerCompBase)

sdl::imtbase::CollectionDocumentService::CDocumentList CRemoteCollectionDocumentServiceControllerComp::OnGetOpenedDocumentList(
			const sdl::imtbase::CollectionDocumentService::CGetOpenedDocumentListGqlRequest& /*getOpenedDocumentListRequest*/,
			const imtgql::CGqlRequest& gqlRequest,
			QString& errorMessage) const
{
	return SendModelRequest<sdl::imtbase::CollectionDocumentService::CDocumentList>(gqlRequest, errorMessage);
}


sdl::imtbase::CollectionDocumentService::CDocumentInfo CRemoteCollectionDocumentServiceControllerComp::OnCreateNewDocument(
			const sdl::imtbase::CollectionDocumentService::CCreateNewDocumentGqlRequest& /*createNewDocumentRequest*/,
			const imtgql::CGqlRequest& gqlRequest,
			QString& errorMessage) const
{
	return SendModelRequest<sdl::imtbase::CollectionDocumentService::CDocumentInfo>(gqlRequest, errorMessage);
}


sdl::imtbase::CollectionDocumentService::CDocumentInfo CRemoteCollectionDocumentServiceControllerComp::OnOpenDocument(
			const sdl::imtbase::CollectionDocumentService::COpenDocumentGqlRequest& /*openDocumentRequest*/,
			const imtgql::CGqlRequest& gqlRequest,
			QString& errorMessage) const
{
	return SendModelRequest<sdl::imtbase::CollectionDocumentService::CDocumentInfo>(gqlRequest, errorMessage);
}


sdl::imtbase::CollectionDocumentService::CDocumentInfo CRemoteCollectionDocumentServiceControllerComp::OnGetDocumentName(
			const sdl::imtbase::CollectionDocumentService::CGetDocumentNameGqlRequest& /*getDocumentNameRequest*/,
			const imtgql::CGqlRequest& gqlRequest,
			QString& errorMessage) const
{
	return SendModelRequest<sdl::imtbase::CollectionDocumentService::CDocumentInfo>(gqlRequest, errorMessage);
}


sdl::imtbase::CollectionDocumentService::CDocumentOperationStatus CRemoteCollectionDocumentServiceControllerComp::OnSetDocumentName(
			const sdl::imtbase::CollectionDocumentService::CSetDocumentNameGqlRequest& /*setDocumentNameRequest*/,
			const imtgql::CGqlRequest& gqlRequest,
			QString& errorMessage) const
{
	return SendModelRequest<sdl::imtbase::CollectionDocumentService::CDocumentOperationStatus>(gqlRequest, errorMessage);
}


sdl::imtbase::CollectionDocumentService::CDocumentOperationStatus CRemoteCollectionDocumentServiceControllerComp::OnSaveDocument(
			const sdl::imtbase::CollectionDocumentService::CSaveDocumentGqlRequest& /*saveDocumentRequest*/,
			const imtgql::CGqlRequest& gqlRequest,
			QString& errorMessage) const
{
	return SendModelRequest<sdl::imtbase::CollectionDocumentService::CDocumentOperationStatus>(gqlRequest, errorMessage);
}


sdl::imtbase::CollectionDocumentService::CDocumentOperationStatus CRemoteCollectionDocumentServiceControllerComp::OnCloseDocument(
			const sdl::imtbase::CollectionDocumentService::CCloseDocumentGqlRequest& /*closeDocumentRequest*/,
			const imtgql::CGqlRequest& gqlRequest,
			QString& errorMessage) const
{
	return SendModelRequest<sdl::imtbase::CollectionDocumentService::CDocumentOperationStatus>(gqlRequest, errorMessage);
}


sdl::imtbase::UndoManager::CUndoInfo CRemoteCollectionDocumentServiceControllerComp::OnGetUndoInfo(
			const sdl::imtbase::CollectionDocumentService::CGetUndoInfoGqlRequest& /*getUndoInfoRequest*/,
			const imtgql::CGqlRequest& gqlRequest,
			QString& errorMessage) const
{
	return SendModelRequest<sdl::imtbase::UndoManager::CUndoInfo>(gqlRequest, errorMessage);
}


sdl::imtbase::UndoManager::CUndoStatus CRemoteCollectionDocumentServiceControllerComp::OnDoUndo(
			const sdl::imtbase::CollectionDocumentService::CDoUndoGqlRequest& /*doUndoRequest*/,
			const imtgql::CGqlRequest& gqlRequest,
			QString& errorMessage) const
{
	return SendModelRequest<sdl::imtbase::UndoManager::CUndoStatus>(gqlRequest, errorMessage);
}


sdl::imtbase::UndoManager::CUndoStatus CRemoteCollectionDocumentServiceControllerComp::OnDoRedo(
			const sdl::imtbase::CollectionDocumentService::CDoRedoGqlRequest& /*doRedoRequest*/,
			const imtgql::CGqlRequest& gqlRequest,
			QString& errorMessage) const
{
	return SendModelRequest<sdl::imtbase::UndoManager::CUndoStatus>(gqlRequest, errorMessage);
}


sdl::imtbase::UndoManager::CUndoStatus CRemoteCollectionDocumentServiceControllerComp::OnResetUndo(
			const sdl::imtbase::CollectionDocumentService::CResetUndoGqlRequest& /*resetUndoRequest*/,
			const imtgql::CGqlRequest& gqlRequest,
			QString& errorMessage) const
{
	return SendModelRequest<sdl::imtbase::UndoManager::CUndoStatus>(gqlRequest, errorMessage);
}


// reimplemented (imtgql::IGqlRequestHandler)

bool CRemoteCollectionDocumentServiceControllerComp::IsRequestSupported(const imtgql::CGqlRequest& gqlRequest) const
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


