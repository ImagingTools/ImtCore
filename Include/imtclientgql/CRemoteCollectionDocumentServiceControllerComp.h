// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ImtCore includes
#include <imtclientgql/TClientRequestManagerCompWrap.h>
#include <imtbasesdl/SDL/1.0/CPP/CollectionDocumentService>
#include <imtbasesdl/SDL/1.0/CPP/UndoManager>


namespace imtclientgql
{


class CRemoteCollectionDocumentServiceControllerComp:
			public TClientRequestManagerCompWrap<sdl::imtbase::CollectionDocumentService::CGraphQlHandlerCompBase>
{
public:
	typedef TClientRequestManagerCompWrap<sdl::imtbase::CollectionDocumentService::CGraphQlHandlerCompBase> BaseClass;

	I_BEGIN_COMPONENT(CRemoteCollectionDocumentServiceControllerComp);
		I_ASSIGN_MULTI_0(m_collectionIdsAttrPtr, "CollectionIds", "Collection ID-s", false);
	I_END_COMPONENT;

protected:
	// reimplemented (sdl::imtbase::CollectionDocumentService::CGraphQlHandlerCompBase)
	virtual sdl::imtbase::CollectionDocumentService::CDocumentList OnGetOpenedDocumentList(
				const sdl::imtbase::CollectionDocumentService::CGetOpenedDocumentListGqlRequest& getOpenedDocumentListRequest,
				const ::imtgql::CGqlRequest& gqlRequest,
				QString& errorMessage) const override;
	virtual sdl::imtbase::CollectionDocumentService::CDocumentInfo OnCreateNewDocument(
				const sdl::imtbase::CollectionDocumentService::CCreateNewDocumentGqlRequest& createNewDocumentRequest,
				const ::imtgql::CGqlRequest& gqlRequest,
				QString& errorMessage) const override;
	virtual sdl::imtbase::CollectionDocumentService::CDocumentInfo OnOpenDocument(
				const sdl::imtbase::CollectionDocumentService::COpenDocumentGqlRequest& openDocumentRequest,
				const ::imtgql::CGqlRequest& gqlRequest,
				QString& errorMessage) const override;
	virtual sdl::imtbase::CollectionDocumentService::CDocumentInfo OnGetDocumentName(
				const sdl::imtbase::CollectionDocumentService::CGetDocumentNameGqlRequest& getDocumentNameRequest,
				const ::imtgql::CGqlRequest& gqlRequest,
				QString& errorMessage) const override;
	virtual sdl::imtbase::CollectionDocumentService::CDocumentOperationStatus OnSetDocumentName(
				const sdl::imtbase::CollectionDocumentService::CSetDocumentNameGqlRequest& setDocumentNameRequest,
				const ::imtgql::CGqlRequest& gqlRequest,
				QString& errorMessage) const override;
	virtual sdl::imtbase::CollectionDocumentService::CDocumentOperationStatus OnSaveDocument(
				const sdl::imtbase::CollectionDocumentService::CSaveDocumentGqlRequest& saveDocumentRequest,
				const ::imtgql::CGqlRequest& gqlRequest,
				QString& errorMessage) const override;
	virtual sdl::imtbase::CollectionDocumentService::CDocumentOperationStatus OnCloseDocument(
				const sdl::imtbase::CollectionDocumentService::CCloseDocumentGqlRequest& closeDocumentRequest,
				const ::imtgql::CGqlRequest& gqlRequest,
				QString& errorMessage) const override;
	virtual sdl::imtbase::UndoManager::CUndoInfo OnGetUndoInfo(
				const sdl::imtbase::CollectionDocumentService::CGetUndoInfoGqlRequest& getUndoInfoRequest,
				const ::imtgql::CGqlRequest& gqlRequest,
				QString& errorMessage) const override;
	virtual sdl::imtbase::UndoManager::CUndoStatus OnDoUndo(
				const sdl::imtbase::CollectionDocumentService::CDoUndoGqlRequest& doUndoRequest,
				const ::imtgql::CGqlRequest& gqlRequest,
				QString& errorMessage) const override;
	virtual sdl::imtbase::UndoManager::CUndoStatus OnDoRedo(
				const sdl::imtbase::CollectionDocumentService::CDoRedoGqlRequest& doRedoRequest,
				const ::imtgql::CGqlRequest& gqlRequest,
				QString& errorMessage) const override;
	virtual sdl::imtbase::UndoManager::CUndoStatus OnResetUndo(
				const sdl::imtbase::CollectionDocumentService::CResetUndoGqlRequest& resetUndoRequest,
				const ::imtgql::CGqlRequest& gqlRequest,
				QString& errorMessage) const override;

	// reimplemented (imtgql::IGqlRequestHandler)
	virtual bool IsRequestSupported(const imtgql::CGqlRequest& gqlRequest) const override;

protected:
	I_MULTIATTR(QByteArray, m_collectionIdsAttrPtr);
};


} // namespace imtclientgql


