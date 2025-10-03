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
	DocumentList GetDocumentList(const QByteArray& userId) const override;
	QByteArray CreateDocument(const QByteArray& userId, const QByteArray& documentTypeId) override;
	QByteArray OpenDocument(const QByteArray& userId, const QByteArray& objectId) override;
	istd::IChangeable* GetDocument(const QByteArray& userId, const QByteArray& documentId) const override;
	OperationStatus SaveDocument(const QByteArray& userId, const QByteArray& documentId) override;
	OperationStatus CloseDocument(const QByteArray& userId, const QByteArray& documentId) override;
	idoc::IUndoManager* GetDocumentUndoManager(const QByteArray& userId, const QByteArray& documentId) const override;

	// reimplemented (iser::ISerializable)
	bool Serialize(iser::IArchive& archive) override;

protected:
	virtual imtbase::IObjectCollection* GetCollection() const = 0;
	virtual istd::IChangeableSharedPtr CreateObject(const QByteArray& typeId) const = 0;
	virtual idoc::IUndoManagerSharedPtr CreateUndoManager() const = 0;

protected:
	struct WorkingDocument
	{
		QByteArray objectId;
		QByteArray objectTypeId;
		QString name;
		istd::IChangeableSharedPtr objectPtr;
		idoc::IUndoManagerSharedPtr undoManagerPtr;
		bool hasChanges;
	};

	typedef QMap<QByteArray, WorkingDocument> WorkingDocumentList;
	QMap<QString, WorkingDocumentList> m_userDocuments;
	mutable QMutex m_mutex;

	imtbase::IObjectCollection* m_collectionPtr = nullptr;
};


} // namespace imtdoc
