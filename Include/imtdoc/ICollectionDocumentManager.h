#pragma once


// ACF includes
#include <idoc/IUndoManager.h>
#include <iser/ISerializable.h>


namespace imtdoc
{


class ICollectionDocumentManager : virtual public iser::ISerializable
{
public:
	enum ChangeFlags
	{
		CF_NEW_DOCUMENT_CREATED = 170000,
		CF_DOCUMENT_OPENED,
		CF_DOCUMENT_CHANGED,
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
		bool hasChanges;
		int availableUndoSteps;
		int availableRedoSteps;
		QStringList undoLevelDescriptions;
		QStringList redoLevelDescriptions;
	};
	typedef QList<DocumentInfo> DocumentList;

	struct DocumentNotification : public DocumentInfo
	{
		QByteArray userId;
	};
	typedef std::shared_ptr<DocumentNotification> DocumentNotificationPtr;

	static const QByteArray CN_NEW_DOCUMENT_CREATED;
	typedef DocumentNotification NewDocumentCreatedInfo;

	static const QByteArray CN_DOCUMENT_OPENED;
	typedef DocumentNotification DocumentOpenedInfo;

	static const QByteArray CN_DOCUMENT_CHANGED;
	typedef DocumentNotification DocumentChangedInfo;

	static const QByteArray CN_DOCUMENT_CLOSED;
	typedef QByteArray DocumentClosedInfo;

	virtual DocumentList GetOpenedDocumentList(const QByteArray& userId) const = 0;
	virtual QByteArray CreateNewDocument(const QByteArray& userId, const QByteArray& documentTypeId) = 0;
	virtual QByteArray OpenDocument(const QByteArray& userId, const QByteArray& documentId) = 0;
	virtual istd::IChangeable* GetDocument(const QByteArray& userId, const QByteArray& documentId) const = 0;
	virtual OperationStatus SaveDocument(const QByteArray& userId, const QByteArray& documentId) = 0;
	virtual OperationStatus CloseDocument(const QByteArray& userId, const QByteArray& documentId) = 0;
	virtual idoc::IUndoManager* GetDocumentUndoManager(
		const QByteArray& userId, const QByteArray& documentId) const = 0;
};


} // namespace imtdoc


Q_DECLARE_METATYPE(imtdoc::ICollectionDocumentManager::DocumentNotification);