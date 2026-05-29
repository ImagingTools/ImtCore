// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ImtCore includes
#include <imtclientgql/TClientRequestManagerCompWrap.h>
#include <imtbasesdl/SDL/1.0/CPP/CollectionDocumentService>
#include <imtbasesdl/SDL/1.0/CPP/UndoManager>


namespace imtclientgql
{


class CRemoteCollectionDocumentServiceControllerComp:
			public TClientRequestManagerCompWrap<sdl::V1_0::imtbase::CGraphQlHandlerCompBase>
{
public:
	typedef TClientRequestManagerCompWrap<sdl::V1_0::imtbase::CGraphQlHandlerCompBase> BaseClass;

	I_BEGIN_COMPONENT(CRemoteCollectionDocumentServiceControllerComp);
		I_ASSIGN_MULTI_0(m_collectionIdsAttrPtr, "CollectionIds", "Collection ID-s", false);
	I_END_COMPONENT;

protected:
	// reimplemented (sdl::V1_0::imtbase::CGraphQlHandlerCompBase)
	virtual sdl::V1_0::imtbase::CDocumentList OnGetOpenedDocumentList(
				const sdl::V1_0::imtbase::CGetOpenedDocumentListGqlRequest& getOpenedDocumentListRequest,
				const ::imtgql::CGqlRequest& gqlRequest,
				QString& errorMessage) const override;
	virtual sdl::V1_0::imtbase::CDocumentInfo OnCreateNewDocument(
				const sdl::V1_0::imtbase::CCreateNewDocumentGqlRequest& createNewDocumentRequest,
				const ::imtgql::CGqlRequest& gqlRequest,
				QString& errorMessage) const override;
	virtual sdl::V1_0::imtbase::CDocumentInfo OnOpenDocument(
				const sdl::V1_0::imtbase::COpenDocumentGqlRequest& openDocumentRequest,
				const ::imtgql::CGqlRequest& gqlRequest,
				QString& errorMessage) const override;
	virtual sdl::V1_0::imtbase::CDocumentInfo OnGetDocumentName(
				const sdl::V1_0::imtbase::CGetDocumentNameGqlRequest& getDocumentNameRequest,
				const ::imtgql::CGqlRequest& gqlRequest,
				QString& errorMessage) const override;
	virtual sdl::V1_0::imtbase::CDocumentOperationStatus OnSetDocumentName(
				const sdl::V1_0::imtbase::CSetDocumentNameGqlRequest& setDocumentNameRequest,
				const ::imtgql::CGqlRequest& gqlRequest,
				QString& errorMessage) const override;
	virtual sdl::V1_0::imtbase::CDocumentOperationStatus OnSaveDocument(
				const sdl::V1_0::imtbase::CSaveDocumentGqlRequest& saveDocumentRequest,
				const ::imtgql::CGqlRequest& gqlRequest,
				QString& errorMessage) const override;
	virtual sdl::V1_0::imtbase::CDocumentOperationStatus OnCloseDocument(
				const sdl::V1_0::imtbase::CCloseDocumentGqlRequest& closeDocumentRequest,
				const ::imtgql::CGqlRequest& gqlRequest,
				QString& errorMessage) const override;
	virtual sdl::V1_0::imtbase::CUndoInfo OnGetUndoInfo(
				const sdl::V1_0::imtbase::CGetUndoInfoGqlRequest& getUndoInfoRequest,
				const ::imtgql::CGqlRequest& gqlRequest,
				QString& errorMessage) const override;
	virtual sdl::V1_0::imtbase::CUndoStatus OnDoUndo(
				const sdl::V1_0::imtbase::CDoUndoGqlRequest& doUndoRequest,
				const ::imtgql::CGqlRequest& gqlRequest,
				QString& errorMessage) const override;
	virtual sdl::V1_0::imtbase::CUndoStatus OnDoRedo(
				const sdl::V1_0::imtbase::CDoRedoGqlRequest& doRedoRequest,
				const ::imtgql::CGqlRequest& gqlRequest,
				QString& errorMessage) const override;
	virtual sdl::V1_0::imtbase::CUndoStatus OnResetUndo(
				const sdl::V1_0::imtbase::CResetUndoGqlRequest& resetUndoRequest,
				const ::imtgql::CGqlRequest& gqlRequest,
				QString& errorMessage) const override;

	// reimplemented (imtgql::IGqlRequestHandler)
	virtual bool IsRequestSupported(const imtgql::CGqlRequest& gqlRequest) const override;

protected:
	I_MULTIATTR(QByteArray, m_collectionIdsAttrPtr);
};


} // namespace imtclientgql


