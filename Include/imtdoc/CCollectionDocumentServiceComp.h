// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ACF includes
#include <ifile/IFileNameParam.h>

// ImtCore includes
#include <imtdoc/TCollectionDocumentServiceWrap.h>
#include <imtdoc/CDocumentServiceCompBase.h>


namespace imtdoc
{


/**
	\ingroup imtdoc
	\brief Concrete ACF component that implements \c IDocumentService on top of an \c IObjectCollection.

	\c CCollectionDocumentServiceComp is the standard ready-to-use document
	service component for server-side multi-user scenarios.  It inherits
	the full implementation from
	\c TCollectionDocumentServiceWrap<CDocumentServiceCompBase> and adds only
	the two ACF component attributes that complete the concrete class:

	- \c "Collection" — reference to the \c IObjectCollection that provides
	  persistent storage (required).
	- \c "IsSingleCopyMode" — when \c true, all users that open the same
	  collection element share a single in-memory data object and undo
	  manager (default: \c false).
	- \c "UndoManagerFolder" — optional root folder used to locate and
	  remove undo snapshot subfolders by document ID on close.

	Register it in the component descriptor with the interface
	\c imtdoc::IDocumentService so that clients can obtain it via the ACF
	service locator.
*/
class CCollectionDocumentServiceComp: public TCollectionDocumentServiceWrap<CDocumentServiceCompBase>
{
public:
	typedef TCollectionDocumentServiceWrap<CDocumentServiceCompBase> BaseClass;

	I_BEGIN_COMPONENT(CCollectionDocumentServiceComp)
		I_REGISTER_INTERFACE(imtdoc::IDocumentService)
		I_REGISTER_INTERFACE(iser::ISerializable)
		I_ASSIGN(m_collectionCompPtr, "Collection", "Document collection containing related documents", true, "Collection");
		I_ASSIGN(m_isSingleCopyModeAttrPtr, "IsSingleCopyMode", "When enabled, all users share a single copy of each document", false, false);
		I_ASSIGN(m_undoManagerFolderCompPtr, "UndoManagerFolder", "Root folder for undo manager snapshots", false, "UndoManagerFolder");
	I_END_COMPONENT

	// reimplemented (iser::ISerializable)
	virtual bool Serialize(iser::IArchive& archive) override;

protected:
	// reimplemented (imtdoc::TCollectionDocumentServiceWrap)
	virtual imtbase::IObjectCollection* GetCollection() const override;

	// reimplemented (imtdoc::CDocumentServiceBase)
	virtual bool IsSingleCopyMode() const override;
	virtual OperationStatus CloseDocumentInternal(const QByteArray& userId, const QByteArray& documentId) override;

private:
	struct DocumentInfo
	{
		QByteArray documentId;
		QByteArray objectId;
		QByteArray typeId;
		QString url;
		QString name;
		bool singleDocumentInstance = false;
	};
	typedef QList<DocumentInfo> DocumentInfoList;
	typedef QMap<QByteArray, DocumentInfoList> DocumentMap;

	bool SerializeDocumentMap(iser::IArchive& archive, DocumentMap& documentMap) const;
	void PrepareDocumentMap(DocumentMap& documentMap) const;
	bool RestoreDocumentMap(const DocumentMap& documentMap);
	void RemoveAllDocuments();
	bool RestoreSharedDocument(const DocumentInfo& documentInfo, const QByteArray& userId);
	bool RestoreDocument(
				const DocumentInfo& documentInfo,
				const QByteArray& userId,
				bool isSingleCopyMode,
				imtbase::IObjectCollection* collectionPtr);
	void RegisterRestoredDocument(
				const DocumentInfo& documentInfo,
				const QByteArray& userId,
				bool isSingleCopyMode,
				const idoc::IUndoManagerSharedPtr& undoManagerPtr);
	void RemoveUndoManagerDocumentDirectory(const QByteArray& documentId) const;

private:
	I_REF(imtbase::IObjectCollection, m_collectionCompPtr);
	I_ATTR(bool, m_isSingleCopyModeAttrPtr);
	I_REF(ifile::IFileNameParam, m_undoManagerFolderCompPtr);
};


} // namespace imtdoc
