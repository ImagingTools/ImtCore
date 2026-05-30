// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ACF includes
#include <idoc/IDocumentManager.h>

// ImtCore includes
#include <imtbase/IObjectCollection.h>
#include <imtbase/IOperationContext.h>
#include <imtdoc/IDocumentService.h>
#include <imtbasesdl/SDL/1.0/CPP/CollectionDocumentService>
#include <imtbasesdl/SDL/1.0/CPP/UndoManager>


namespace imtservergql
{


namespace CDM = sdl::V1_0::imtbase;


class CCollectionDocumentServiceControllerComp: public sdl::V1_0::imtbase::CGraphQlHandlerCompBase
{
public:
	typedef sdl::V1_0::imtbase::CGraphQlHandlerCompBase BaseClass;

	I_BEGIN_COMPONENT(CCollectionDocumentServiceControllerComp)
		I_ASSIGN(m_documentManagerCompPtr, "CollectionDocumentService", "Collection-related document manager", true, "CollectionDocumentService");
		I_ASSIGN(m_collectionIdAttrPtr, "CollectionId", "ID of the underlaying document collection", true, "");
	I_END_COMPONENT

protected:
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

	// reimplemented (::imtservergql::CPermissibleGqlRequestHandlerComp)
	bool IsRequestSupported(const imtgql::CGqlRequest& gqlRequest) const override;

private:
	int GetObjectFactoryIndex(const QByteArray& typeId) const;
	QByteArray GetUserId(const ::imtgql::CGqlRequest& gqlRequest) const;
	imtbase::IOperationContext* CreateOperationContextFromGqlRequest(const ::imtgql::CGqlRequest& gqlRequest) const;

private:
	I_REF(imtdoc::IDocumentService, m_documentManagerCompPtr);
	I_ATTR(QByteArray, m_collectionIdAttrPtr);
};


} // namespace imtservergql


