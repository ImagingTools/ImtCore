// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ACF includes
#include <idoc/IDocumentManager.h>

// ImtCore includes
#include <imtbase/IObjectCollection.h>
#include <imtbase/IOperationContext.h>
#include <imtbase/IDocumentChangeGenerator.h>
#include <imtdoc/IDocumentService.h>
#include <imtbasesdl/SDL/1.0/CPP/CollectionDocumentService_fwd.h>
#include <imtbasesdl/SDL/1.0/CPP/UndoManager_fwd.h>


namespace imtservergql
{


namespace CDM = sdl::V1_0::imtbase;


class CCollectionDocumentServiceControllerComp: public sdl::V1_0::imtbase::CCollectionDocumentServiceGqlHandlerCompBase
{
public:
	typedef sdl::V1_0::imtbase::CCollectionDocumentServiceGqlHandlerCompBase BaseClass;

	I_BEGIN_COMPONENT(CCollectionDocumentServiceControllerComp)
		I_ASSIGN(m_documentManagerCompPtr, "CollectionDocumentService", "Collection-related document manager", true, "CollectionDocumentService");
		I_ASSIGN(m_documentChangeGeneratorCompPtr, "DocumentChangeGenerator", "Change generator producing the document history entries on save", false, "DocumentChangeGenerator");
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
	/**
		Get the ID of the collection element an open document instance is bound to.
		Returns an empty ID for documents which were never saved before.
	*/
	QByteArray GetCollectionObjectId(const QByteArray& userId, const QByteArray& documentId) const;
	/**
		Fill the changes collection of \a operationContext with the differences produced by saving
		the given document instance, so that the collection can store them as history.
	*/
	void GenerateDocumentChanges(
				const QByteArray& userId,
				const QByteArray& documentId,
				imtbase::IOperationContext& operationContext) const;

private:
	I_REF(imtdoc::IDocumentService, m_documentManagerCompPtr);
	I_REF(imtbase::IDocumentChangeGenerator, m_documentChangeGeneratorCompPtr);
	I_ATTR(QByteArray, m_collectionIdAttrPtr);
};


} // namespace imtservergql

