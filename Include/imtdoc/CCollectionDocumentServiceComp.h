// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// Qt includes
#include <QtCore/QJsonObject>
#include <QtCore/QMutex>

// ACF includes
#include <ifile/IFileNameParam.h>
#include <iprm/IEnableableParam.h>

// ImtCore includes
#include <imtdoc/TCollectionDocumentServiceWrap.h>
#include <imtdoc/CDocumentServiceComp.h>


namespace imtdoc
{


/**
	\ingroup imtdoc
	\brief Concrete ACF component that implements \c IDocumentService on top of an \c IObjectCollection.

	\c CCollectionDocumentServiceComp is the standard ready-to-use document
	service component for server-side multi-user scenarios.  It inherits
	the full implementation from
	\c TCollectionDocumentServiceWrap<CDocumentServiceComp> and adds only
	the two ACF component attributes that complete the concrete class:

	- \c "Collection" — reference to the \c IObjectCollection that provides
	  persistent storage (required).
	- \c "IsSingleCopyMode" — when \c true, all users that open the same
	  collection element share a single in-memory data object and undo
	  manager (default: \c false).
	- \c "StateFilePath" — optional file path used by \c LoadFromFile /
	  \c SaveToFile for persisting the list of opened documents in JSON format.
	- \c "StatePersistenceEnabler" — optional switch.  When enabled, the
	  list of opened documents is restored from \c "StateFilePath" on
	  component creation and saved to it after each change of the service
	  data model (open/create/close/save/rename of documents).

	The JSON file contains the same information as \c Serialize (user IDs
	and the metadata of their opened documents); document data is not stored.

	Register it in the component descriptor with the interface
	\c imtdoc::IDocumentService so that clients can obtain it via the ACF
	service locator.
*/
class CCollectionDocumentServiceComp: public TCollectionDocumentServiceWrap<CDocumentServiceComp>
{
public:
	typedef TCollectionDocumentServiceWrap<CDocumentServiceComp> BaseClass;

	I_BEGIN_COMPONENT(CCollectionDocumentServiceComp)
		I_REGISTER_INTERFACE(imtdoc::IDocumentService)
		I_REGISTER_INTERFACE(iser::ISerializable)
		I_ASSIGN(m_collectionCompPtr, "Collection", "Document collection containing related documents", true, "Collection");
		I_ASSIGN(m_isSingleCopyModeAttrPtr, "IsSingleCopyMode", "When enabled, all users share a single copy of each document", false, false);
		I_ASSIGN(m_stateFilePathCompPtr, "StateFilePath", "File path used to save/restore the list of opened documents", false, "StateFilePath");
		I_ASSIGN(m_statePersistenceEnablerCompPtr, "StatePersistenceEnabler", "Enables restoring of opened documents on component creation and saving on each data model change", false, "StatePersistenceEnabler");
	I_END_COMPONENT

	/**
		\brief Restore the list of opened documents from a JSON file.

		All currently opened documents are discarded and replaced by the
		documents described in the file.  Nothing is changed if the file
		cannot be read or parsed.
		\return \c true on success.
	*/
	bool LoadFromFile(const QString& filePath);

	/**
		\brief Save the list of opened documents to a JSON file.

		The file contains the same information as \c Serialize.
		\return \c true on success.
	*/
	bool SaveToFile(const QString& filePath) const;

	// reimplemented (iser::ISerializable)
	virtual bool Serialize(iser::IArchive& archive) override;

protected:
	// reimplemented (icomp::CComponentBase)
	virtual void OnComponentCreated() override;

	// reimplemented (istd::IChangeable)
	virtual void OnEndChanges(const ChangeSet& changeSet) override;

	// reimplemented (imtdoc::TCollectionDocumentServiceWrap)
	virtual imtbase::IObjectCollection* GetCollection() const override;

	// reimplemented (imtdoc::CDocumentServiceBase)
	virtual bool IsSingleCopyMode() const override;

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
	static QJsonObject DocumentMapToJson(const DocumentMap& documentMap);
	static bool DocumentMapFromJson(const QJsonObject& rootObject, DocumentMap& documentMap);
	bool IsStatePersistenceEnabled() const;
	QString GetStateFilePath() const;
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

private:
	I_REF(imtbase::IObjectCollection, m_collectionCompPtr);
	I_ATTR(bool, m_isSingleCopyModeAttrPtr);
	I_REF(ifile::IFileNameParam, m_stateFilePathCompPtr);
	I_REF(iprm::IEnableableParam, m_statePersistenceEnablerCompPtr);

	bool m_isStateLoading = false;
	mutable QMutex m_stateFileMutex;
};


} // namespace imtdoc
