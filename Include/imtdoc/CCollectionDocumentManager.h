#pragma once


// Qt includes
#include <QtCore/QMutex>

// ACF includes
#include <idoc/IDocumentManager.h>
#include <idoc/IUndoManager.h>

// ImtCore includes
#include <imtbase/IObjectCollection.h>

// GmgCore includes
#include <imtdoc/ICollectionDocumentManager.h>


namespace imtdoc
{


class CCollectionDocumentManager : virtual public imtdoc::ICollectionDocumentManager
{
public:
	// reimplemented (imtdoc::IDocumentManager)
	virtual DocumentList GetOpenedDocumentList(const QByteArray& userId) const override;
	virtual QByteArray CreateNewDocument(const QByteArray& userId, const QByteArray& documentTypeId) override;
	virtual QByteArray OpenDocument(const QByteArray& userId, const QByteArray& objectId) override;
	virtual istd::IChangeable* GetDocument(const QByteArray& userId, const QByteArray& documentId) const override;
	virtual OperationStatus SaveDocument(const QByteArray& userId, const QByteArray& documentId) override;
	virtual OperationStatus CloseDocument(const QByteArray& userId, const QByteArray& documentId) override;
	virtual idoc::IUndoManager* GetDocumentUndoManager(const QByteArray& userId, const QByteArray& documentId) const override;

	// reimplemented (iser::ISerializable)
	virtual bool Serialize(iser::IArchive& archive) override;

protected:
	virtual imtbase::IObjectCollection* GetCollection() const = 0;
	virtual istd::IChangeableSharedPtr CreateObject(const QByteArray& typeId) const = 0;
	virtual idoc::IUndoManagerSharedPtr CreateUndoManager() const = 0;

protected:
	struct WorkingDocument
	{
		QByteArray objectId;
		QByteArray objectTypeId;
		istd::IChangeableSharedPtr objectPtr;
		idoc::IUndoManagerSharedPtr undoManagerPtr;
		bool hasChanges;
	};

	typedef QMap<QByteArray, WorkingDocument> WorkingDocumentList;
	QMap<QString, WorkingDocumentList> m_userDocuments;
	mutable QMutex m_mutex;

private:
	std::shared_ptr<DocumentNotification> CreateDocumentNotification(const QByteArray& userId, const QByteArray& documentId) const;
};


} // namespace imtdoc
