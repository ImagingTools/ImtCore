// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtbasesdl/SDL/1.0/CPP/CollectionDocumentService.h>
#include <imtclientgql/CRemoteCollectionDocumentServiceControllerComp.h>
#include <imtbasesdl/SDL/1.0/CPP/UndoManager.h>


namespace imtclientgql
{


// protected methods

// reimplemented (sdl::V1_0::imtbase::CCollectionDocumentServiceGqlHandlerCompBase)

sdl::V1_0::imtbase::CDocumentList CRemoteCollectionDocumentServiceControllerComp::OnGetOpenedDocumentList(
			const sdl::V1_0::imtbase::CGetOpenedDocumentListGqlRequest& /*getOpenedDocumentListRequest*/,
			const imtgql::CGqlRequest& gqlRequest,
			QString& errorMessage) const
{
	return SendModelRequest<sdl::V1_0::imtbase::CDocumentList>(gqlRequest, errorMessage);
}


sdl::V1_0::imtbase::CDocumentInfo CRemoteCollectionDocumentServiceControllerComp::OnCreateNewDocument(
			const sdl::V1_0::imtbase::CCreateNewDocumentGqlRequest& /*createNewDocumentRequest*/,
			const imtgql::CGqlRequest& gqlRequest,
			QString& errorMessage) const
{
	return SendModelRequest<sdl::V1_0::imtbase::CDocumentInfo>(gqlRequest, errorMessage);
}


sdl::V1_0::imtbase::CDocumentInfo CRemoteCollectionDocumentServiceControllerComp::OnOpenDocument(
			const sdl::V1_0::imtbase::COpenDocumentGqlRequest& /*openDocumentRequest*/,
			const imtgql::CGqlRequest& gqlRequest,
			QString& errorMessage) const
{
	return SendModelRequest<sdl::V1_0::imtbase::CDocumentInfo>(gqlRequest, errorMessage);
}


sdl::V1_0::imtbase::CDocumentInfo CRemoteCollectionDocumentServiceControllerComp::OnGetDocumentName(
			const sdl::V1_0::imtbase::CGetDocumentNameGqlRequest& /*getDocumentNameRequest*/,
			const imtgql::CGqlRequest& gqlRequest,
			QString& errorMessage) const
{
	return SendModelRequest<sdl::V1_0::imtbase::CDocumentInfo>(gqlRequest, errorMessage);
}


sdl::V1_0::imtbase::CDocumentOperationStatus CRemoteCollectionDocumentServiceControllerComp::OnSetDocumentName(
			const sdl::V1_0::imtbase::CSetDocumentNameGqlRequest& /*setDocumentNameRequest*/,
			const imtgql::CGqlRequest& gqlRequest,
			QString& errorMessage) const
{
	return SendModelRequest<sdl::V1_0::imtbase::CDocumentOperationStatus>(gqlRequest, errorMessage);
}


sdl::V1_0::imtbase::CDocumentOperationStatus CRemoteCollectionDocumentServiceControllerComp::OnSaveDocument(
			const sdl::V1_0::imtbase::CSaveDocumentGqlRequest& /*saveDocumentRequest*/,
			const imtgql::CGqlRequest& gqlRequest,
			QString& errorMessage) const
{
	return SendModelRequest<sdl::V1_0::imtbase::CDocumentOperationStatus>(gqlRequest, errorMessage);
}


sdl::V1_0::imtbase::CDocumentOperationStatus CRemoteCollectionDocumentServiceControllerComp::OnCloseDocument(
			const sdl::V1_0::imtbase::CCloseDocumentGqlRequest& /*closeDocumentRequest*/,
			const imtgql::CGqlRequest& gqlRequest,
			QString& errorMessage) const
{
	return SendModelRequest<sdl::V1_0::imtbase::CDocumentOperationStatus>(gqlRequest, errorMessage);
}


sdl::V1_0::imtbase::CUndoInfo CRemoteCollectionDocumentServiceControllerComp::OnGetUndoInfo(
			const sdl::V1_0::imtbase::CGetUndoInfoGqlRequest& /*getUndoInfoRequest*/,
			const imtgql::CGqlRequest& gqlRequest,
			QString& errorMessage) const
{
	return SendModelRequest<sdl::V1_0::imtbase::CUndoInfo>(gqlRequest, errorMessage);
}


sdl::V1_0::imtbase::CUndoStatus CRemoteCollectionDocumentServiceControllerComp::OnDoUndo(
			const sdl::V1_0::imtbase::CDoUndoGqlRequest& /*doUndoRequest*/,
			const imtgql::CGqlRequest& gqlRequest,
			QString& errorMessage) const
{
	return SendModelRequest<sdl::V1_0::imtbase::CUndoStatus>(gqlRequest, errorMessage);
}


sdl::V1_0::imtbase::CUndoStatus CRemoteCollectionDocumentServiceControllerComp::OnDoRedo(
			const sdl::V1_0::imtbase::CDoRedoGqlRequest& /*doRedoRequest*/,
			const imtgql::CGqlRequest& gqlRequest,
			QString& errorMessage) const
{
	return SendModelRequest<sdl::V1_0::imtbase::CUndoStatus>(gqlRequest, errorMessage);
}


sdl::V1_0::imtbase::CUndoStatus CRemoteCollectionDocumentServiceControllerComp::OnResetUndo(
			const sdl::V1_0::imtbase::CResetUndoGqlRequest& /*resetUndoRequest*/,
			const imtgql::CGqlRequest& gqlRequest,
			QString& errorMessage) const
{
	return SendModelRequest<sdl::V1_0::imtbase::CUndoStatus>(gqlRequest, errorMessage);
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


