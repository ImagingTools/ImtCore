// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ACF includes
#include <idoc/IDocumentManager.h>

// ImtCore includes
#include <imtbase/IObjectCollection.h>
#include <imtdoc/IDocumentService.h>
#include <imtbasesdl/SDL/1.0/CPP/CollectionDocumentService.h>
#include <imtbasesdl/SDL/1.0/CPP/UndoManager.h>


namespace imtservergql
{


namespace CDM = sdl::imtbase::CollectionDocumentService;


class CCollectionDocumentServiceControllerComp: public sdl::imtbase::CollectionDocumentService::CGraphQlHandlerCompBase
{
public:
	typedef sdl::imtbase::CollectionDocumentService::CGraphQlHandlerCompBase BaseClass;

	I_BEGIN_COMPONENT(CCollectionDocumentServiceControllerComp)
		I_ASSIGN(m_documentManagerCompPtr, "CollectionDocumentService", "Collection-related document manager", true, "CollectionDocumentService");
		I_ASSIGN(m_collectionIdAttrPtr, "CollectionId", "ID of the underlaying document collection", true, "");
	I_END_COMPONENT

protected:
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

	// reimplemented (::imtservergql::CPermissibleGqlRequestHandlerComp)
	bool IsRequestSupported(const imtgql::CGqlRequest& gqlRequest) const override;

private:
	int GetObjectFactoryIndex(const QByteArray& typeId) const;
	QByteArray GetUserId(const ::imtgql::CGqlRequest& gqlRequest) const;

private:
	I_REF(imtdoc::IDocumentService, m_documentManagerCompPtr);
	I_ATTR(QByteArray, m_collectionIdAttrPtr);
};


} // namespace imtservergql


