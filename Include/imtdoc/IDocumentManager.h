// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// Qt includes
#include <QtCore/QUrl>

// ACF includes
#include <idoc/IUndoManager.h>
#include <iser/ISerializable.h>
#include <imod/IObserver.h>


namespace imtdoc
{


class IDocumentManager : virtual public iser::ISerializable
{
public:
	enum ChangeFlags
	{
		CF_NEW_DOCUMENT_CREATED = 170000,
		CF_DOCUMENT_OPENED,
		CF_DOCUMENT_RENAMED,
		CF_DOCUMENT_CHANGED,
		CF_DOCUMENT_UNDO_CHANGED,
		CF_DOCUMENT_SAVED,
		CF_DOCUMENT_SAVED_AS,
		CF_DOCUMENT_CLOSED
	};

	enum OperationStatus
	{
		OS_OK = 0,
		OS_INVALID_USER_ID,
		OS_INVALID_DOCUMENT_ID,
		OS_FAILED
	};

	struct DocumentInfo
	{
		QByteArray typeId;
		QUrl url;
		QString name;
		bool isDirty = false;
	};

	struct DocumentListItem : public DocumentInfo
	{
		QByteArray documentId;
	};
	typedef QList<DocumentListItem> DocumentList;

	struct DocumentNotification : public DocumentListItem
	{
		QByteArray userId;
	};
	typedef std::shared_ptr<DocumentNotification> DocumentNotificationPtr;

	struct DocumentUndoRedoNotification
	{
		QByteArray userId;
		QByteArray documentId;
		bool isDirty = false;
		int availableUndoSteps = 0;
		int availableRedoSteps = 0;
		QStringList undoLevelDescriptions;
		QStringList redoLevelDescriptions;
	};

	struct DocumentClosedNotification
	{
		QByteArray userId;
		QByteArray documentId;
	};

	static const QByteArray CN_NEW_DOCUMENT_CREATED;
	typedef DocumentNotification NewDocumentCreatedInfo;

	static const QByteArray CN_DOCUMENT_OPENED;
	typedef DocumentNotification DocumentOpenedInfo;

	static const QByteArray CN_DOCUMENT_RENAMED;
	typedef DocumentNotification DocumentRenamedInfo;

	static const QByteArray CN_DOCUMENT_CHANGED;
	typedef DocumentNotification DocumentChangedInfo;

	static const QByteArray CN_DOCUMENT_UNDO_CHANGED;
	typedef DocumentUndoRedoNotification DocumentUndoRedoChangedInfo;

	static const QByteArray CN_DOCUMENT_SAVED;
	typedef DocumentNotification DocumentSavedInfo;

	static const QByteArray CN_DOCUMENT_SAVED_AS;
	typedef DocumentNotification DocumentSavedAsInfo;

	static const QByteArray CN_DOCUMENT_CLOSED;
	typedef DocumentClosedNotification DocumentClosedInfo;

	/**
		Get a list of open document instances for a given user-ID
	*/
	virtual DocumentList GetOpenedDocumentList(const QByteArray& userId) const = 0;

	/**
		Create a document of the given type (documentTypeId) for the given user-ID
		/return		Document instance ID in the document manager
	*/
	virtual QByteArray CreateNewDocument(const QByteArray& userId, const QByteArray& documentTypeId) = 0;

	/**
		Open a document from a given URL for a given user-ID
		/return		Document instance ID in the document manager

		From file:
			file:///etc/fstab					- *nix style path
			file:///c:/pagefile.sys				- Windows style path
		From object collection:
			collection:///objectId				- for single collection document manager (or default collection)
	*/
	virtual QByteArray OpenDocument(const QByteArray& userId, const QUrl& url) = 0;

	/**
		Get name of the document with the given user-ID and document-ID
	*/
	virtual OperationStatus GetDocumentName(const QByteArray& userId, const QByteArray& documentId, QString& documentName) const = 0;

	/**
		Set name of the document with the given user-ID and document-ID
	*/
	virtual OperationStatus SetDocumentName(const QByteArray& userId, const QByteArray& documentId, const QString& documentName) = 0;

	/**
		Get document data pointer with the given user-ID and document-ID
	*/
	virtual const istd::IChangeable* GetDocumentPtr(const QByteArray& userId, const QByteArray& documentId) const = 0;

	/**
		Get document data with the given user-ID and document-ID
	*/
	virtual OperationStatus GetDocumentData(const QByteArray & userId, const QByteArray & documentId, istd::IChangeableSharedPtr& documentPtr) const = 0;

	/**
		Set document data with the given user-ID and document-ID
	*/
	virtual OperationStatus SetDocumentData(const QByteArray& userId, const QByteArray& documentId, const istd::IChangeable& document) = 0;

	/**
		Save document with the given user-ID and document-ID
	*/
	virtual OperationStatus SaveDocument(const QByteArray& userId, const QByteArray& documentId, const QString& documentName) = 0;

	/**
		Close document with the given user-ID and document-ID
	*/
	virtual OperationStatus CloseDocument(const QByteArray& userId, const QByteArray& documentId) = 0;

	/**
		Get UndoManager for the document with the given user-ID and document-ID
	*/
	virtual OperationStatus GetDocumentUndoManager(
		const QByteArray& userId, const QByteArray& documentId, idoc::IUndoManager*& undoManagerPtr) const = 0;

	/**
		Register an obserer for the document with the given user-ID and document-ID
	*/
	virtual OperationStatus RegisterDocumentObserver(const QByteArray& userId, const QByteArray& documentId, imod::IObserver& observer) = 0;

	/**
		Unregister an obserer for the document with the given user-ID and document-ID
	*/
	virtual OperationStatus UnregisterDocumentObserver(const QByteArray& userId, const QByteArray& documentId, imod::IObserver& observer) = 0;
};


} // namespace imtdoc


Q_DECLARE_METATYPE(imtdoc::IDocumentManager::DocumentNotification);
Q_DECLARE_METATYPE(imtdoc::IDocumentManager::DocumentUndoRedoNotification);
Q_DECLARE_METATYPE(imtdoc::IDocumentManager::DocumentClosedNotification);
