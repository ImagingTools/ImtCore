// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ACF includes
#include <idoc/IDocumentManager.h>

// ImtCore includes
#include <imtbase/IObjectCollection.h>
#include <imtdoc/IDocumentManager.h>
#include <imtbasesdl/SDL/1.0/CPP/CollectionDocumentManager.h>
#include <imtbasesdl/SDL/1.0/CPP/UndoManager.h>


namespace imtservergql
{


namespace CDM = sdl::imtbase::CollectionDocumentManager;


class CCollectionDocumentManagerControllerComp: public sdl::imtbase::CollectionDocumentManager::CGraphQlHandlerCompBase
{
public:
	typedef sdl::imtbase::CollectionDocumentManager::CGraphQlHandlerCompBase BaseClass;

	I_BEGIN_COMPONENT(CCollectionDocumentManagerControllerComp)
		I_ASSIGN(m_documentManagerCompPtr, "CollectionDocumentManager", "Collection-related document manager", true, "CollectionDocumentManager");
		I_ASSIGN(m_collectionIdAttrPtr, "CollectionId", "ID of the underlaying document collection", true, "");
	I_END_COMPONENT

protected:
	virtual sdl::imtbase::CollectionDocumentManager::CDocumentList OnGetOpenedDocumentList(
		const sdl::imtbase::CollectionDocumentManager::CGetOpenedDocumentListGqlRequest& getOpenedDocumentListRequest,
		const ::imtgql::CGqlRequest& gqlRequest,
		QString& errorMessage) const override;
	virtual sdl::imtbase::CollectionDocumentManager::CDocumentId OnCreateNewDocument(
		const sdl::imtbase::CollectionDocumentManager::CCreateNewDocumentGqlRequest& createNewDocumentRequest,
		const ::imtgql::CGqlRequest& gqlRequest,
		QString& errorMessage) const override;
	virtual sdl::imtbase::CollectionDocumentManager::CDocumentId OnOpenDocument(
		const sdl::imtbase::CollectionDocumentManager::COpenDocumentGqlRequest& openDocumentRequest,
		const ::imtgql::CGqlRequest& gqlRequest,
		QString& errorMessage) const override;
	virtual sdl::imtbase::CollectionDocumentManager::CDocumentInfo OnGetDocumentName(
		const sdl::imtbase::CollectionDocumentManager::CGetDocumentNameGqlRequest& getDocumentNameRequest,
		const ::imtgql::CGqlRequest& gqlRequest,
		QString& errorMessage) const override;
	virtual sdl::imtbase::CollectionDocumentManager::CDocumentOperationStatus OnSetDocumentName(
		const sdl::imtbase::CollectionDocumentManager::CSetDocumentNameGqlRequest& setDocumentNameRequest,
		const ::imtgql::CGqlRequest& gqlRequest,
		QString& errorMessage) const override;
	virtual sdl::imtbase::CollectionDocumentManager::CDocumentOperationStatus OnSaveDocument(
		const sdl::imtbase::CollectionDocumentManager::CSaveDocumentGqlRequest& saveDocumentRequest,
		const ::imtgql::CGqlRequest& gqlRequest,
		QString& errorMessage) const override;
	virtual sdl::imtbase::CollectionDocumentManager::CDocumentOperationStatus OnCloseDocument(
		const sdl::imtbase::CollectionDocumentManager::CCloseDocumentGqlRequest& closeDocumentRequest,
		const ::imtgql::CGqlRequest& gqlRequest,
		QString& errorMessage) const override;
	virtual sdl::imtbase::UndoManager::CUndoInfo OnGetUndoInfo(
		const sdl::imtbase::CollectionDocumentManager::CGetUndoInfoGqlRequest& getUndoInfoRequest,
		const ::imtgql::CGqlRequest& gqlRequest,
		QString& errorMessage) const override;
	virtual sdl::imtbase::UndoManager::CUndoStatus OnDoUndo(
		const sdl::imtbase::CollectionDocumentManager::CDoUndoGqlRequest& doUndoRequest,
		const ::imtgql::CGqlRequest& gqlRequest,
		QString& errorMessage) const override;
	virtual sdl::imtbase::UndoManager::CUndoStatus OnDoRedo(
		const sdl::imtbase::CollectionDocumentManager::CDoRedoGqlRequest& doRedoRequest,
		const ::imtgql::CGqlRequest& gqlRequest,
		QString& errorMessage) const override;
	virtual sdl::imtbase::UndoManager::CUndoStatus OnResetUndo(
		const sdl::imtbase::CollectionDocumentManager::CResetUndoGqlRequest& resetUndoRequest,
		const ::imtgql::CGqlRequest& gqlRequest,
		QString& errorMessage) const override;

	// reimplemented (::imtservergql::CPermissibleGqlRequestHandlerComp)
	bool IsRequestSupported(const imtgql::CGqlRequest& gqlRequest) const override;

private:
	int GetObjectFactoryIndex(const QByteArray& typeId) const;
	QByteArray GetUserId(const ::imtgql::CGqlRequest& gqlRequest) const;

private:
	I_REF(imtdoc::IDocumentManager, m_documentManagerCompPtr);
	I_ATTR(QByteArray, m_collectionIdAttrPtr);
};


} // namespace imtservergql


