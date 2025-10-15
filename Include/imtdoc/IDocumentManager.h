#pragma once


// Qt includes
#include <QtCore/QUrl>

// ACF includes
#include <idoc/IUndoManager.h>
#include <iser/ISerializable.h>


namespace imtdoc
{


class IDocumentManager : virtual public iser::ISerializable
{
public:
	enum ChangeFlags
	{
		CF_NEW_DOCUMENT_CREATED = 170000,
		CF_DOCUMENT_OPENED,
		CF_DOCUMENT_CHANGED,
		CF_DOCUMENT_UNDO_CHANGED,
		CF_DOCUMENT_SAVED,
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
		QByteArray documentId;
		QByteArray objectId;
		QByteArray objectTypeId;
		bool isDirty;
	};
	typedef QList<DocumentInfo> DocumentList;

	struct DocumentNotification : public DocumentInfo
	{
		QByteArray userId;
	};
	typedef std::shared_ptr<DocumentNotification> DocumentNotificationPtr;

	struct DocumentUndoNotification
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

	static const QByteArray CN_DOCUMENT_CHANGED;
	typedef DocumentNotification DocumentChangedInfo;

	static const QByteArray CN_DOCUMENT_UNDO_CHANGED;
	typedef DocumentUndoNotification DocumentUndoChangedInfo;

	static const QByteArray CN_DOCUMENT_SAVED;
	typedef DocumentNotification DocumentSavedInfo;

	static const QByteArray CN_DOCUMENT_CLOSED;
	typedef DocumentClosedNotification DocumentClosedInfo;

	virtual DocumentList GetOpenedDocumentList(const QByteArray& userId) const = 0;
	virtual QByteArray CreateNewDocument(const QByteArray& userId, const QByteArray& documentTypeId) = 0;
	/*
		From file:
			file:///etc/fstab					- *nix style path
			file:///c:/pagefile.sys				- Windows style path
		From object collection:
			collection:///objectId				- for single collection document manager (or default collection)
	*/
	virtual QByteArray OpenDocument(const QByteArray& userId, const QUrl& url) = 0;
	virtual bool GetDocumentData(const QByteArray & userId, const QByteArray & documentId, istd::IChangeableSharedPtr& documentPtr) const = 0;
	virtual bool SetDocumentData(const QByteArray& userId, const QByteArray& documentId, const istd::IChangeable& document) = 0;
	virtual OperationStatus SaveDocument(const QByteArray& userId, const QByteArray& documentId) = 0;
	virtual OperationStatus CloseDocument(const QByteArray& userId, const QByteArray& documentId) = 0;
	virtual idoc::IUndoManager* GetDocumentUndoManager(
		const QByteArray& userId, const QByteArray& documentId) const = 0;
};


} // namespace imtdoc


Q_DECLARE_METATYPE(imtdoc::IDocumentManager::DocumentNotification);
Q_DECLARE_METATYPE(imtdoc::IDocumentManager::DocumentUndoNotification);
Q_DECLARE_METATYPE(imtdoc::IDocumentManager::DocumentClosedNotification);
