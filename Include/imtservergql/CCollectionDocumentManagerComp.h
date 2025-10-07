#pragma once


// ACF includes
#include <idoc/IDocumentManager.h>
#include <idoc/IUndoManager.h>

// ImtCore includes
#include <imtbase/IObjectCollection.h>
#include <imtdoc/CCollectionDocumentManager.h>
#include <imtbasesdl/SDL/1.0/CPP/CollectionDocumentManager.h>
#include <imtbasesdl/SDL/1.0/CPP/UndoManager.h>


namespace imtservergql
{


namespace CDM = sdl::imtbase::CollectionDocumentManager;
namespace UM = sdl::imtbase::UndoManager;


class CCollectionDocumentManagerComp:
	public sdl::imtbase::CollectionDocumentManager::CGraphQlHandlerCompBase,
	public imtdoc::CCollectionDocumentManager
{
public:
	typedef sdl::imtbase::CollectionDocumentManager::CGraphQlHandlerCompBase BaseClass;
	typedef imtdoc::CCollectionDocumentManager BaseClass2;

	I_BEGIN_COMPONENT(CCollectionDocumentManagerComp)
		I_REGISTER_INTERFACE(imtdoc::ICollectionDocumentManager)
		I_ASSIGN(m_collectionIdAttrPtr, "CollectionId", "Collection ID", true, "CollectiondId");
		I_ASSIGN_MULTI_0(m_objectTypeIdAttrPtr, "ObjectTypeId", "Object type ID", false);
		I_ASSIGN(m_collectionCompPtr, "Collection", "Collection", false, "");
		I_ASSIGN(m_undoManagerFactPtr, "UndoManager", "Undo manager", false, "UndoManager");
		I_ASSIGN_MULTI_0(m_objectFactPtr, "ObjectFactory", "Object factory", false);
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

	// reimplemented (imtdoc::CCollectionDocumentManager)
	virtual imtbase::IObjectCollection* GetCollection() const override;
	virtual istd::IChangeableSharedPtr CreateObject(const QByteArray& typeId) const override;
	virtual idoc::IUndoManagerSharedPtr CreateUndoManager() const override;

	// reimplemented (::imtservergql::CPermissibleGqlRequestHandlerComp)
	bool IsRequestSupported(const imtgql::CGqlRequest& gqlRequest) const override;

private:
	imtdoc::CCollectionDocumentManager* GetNonConstThis() const;
	int GetObjectFactoryIndex(const QByteArray& typeId) const;
	QByteArray GetUserId(const ::imtgql::CGqlRequest& gqlRequest) const;

private:
	I_ATTR(QByteArray, m_collectionIdAttrPtr);
	I_MULTIATTR(QByteArray, m_objectTypeIdAttrPtr);
	I_REF(imtbase::IObjectCollection, m_collectionCompPtr);
	I_FACT(idoc::IUndoManager, m_undoManagerFactPtr);
	I_MULTIFACT(istd::IChangeable, m_objectFactPtr);
};


} // namespace imtservergql
