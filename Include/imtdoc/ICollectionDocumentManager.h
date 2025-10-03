#pragma once


// ACF includes
#include <idoc/IUndoManager.h>


namespace imtdoc
{


class ICollectionDocumentManager : virtual public iser::ISerializable
{
public:
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
		QString name;
		bool hasChanges;
	};

	typedef QList<DocumentInfo> DocumentList;

	virtual DocumentList GetDocumentList(const QByteArray& userId) const = 0;
	virtual QByteArray CreateDocument(const QByteArray& userId, const QByteArray& documentTypeId) = 0;
	virtual QByteArray OpenDocument(const QByteArray& userId, const QByteArray& documentId) = 0;
	virtual istd::IChangeable* GetDocument(const QByteArray& userId, const QByteArray& documentId) const = 0;
	virtual OperationStatus SaveDocument(const QByteArray& userId, const QByteArray& documentId) = 0;
	virtual OperationStatus CloseDocument(const QByteArray& userId, const QByteArray& documentId) = 0;
	virtual idoc::IUndoManager* GetDocumentUndoManager(
		const QByteArray& userId, const QByteArray& documentId) const = 0;
};


} // namespace imtdoc
