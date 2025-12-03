#pragma once


// Qt includes
#include <QtCore/QMutex>

// ACF includes
#include <idoc/IDocumentManager.h>
#include <idoc/IUndoManager.h>
#include <imod/TMultiModelObserverBase.h>
#include <imod/CMultiModelDispatcherBase.h>

// ImtCore includes
#include <imtbase/IObjectCollection.h>

// GmgCore includes
#include <imtdoc/IDocumentManager.h>


namespace imtdoc
{


class CCollectionDocumentManager :
	protected imod::TMultiModelObserverBase<istd::IChangeable>,
	virtual public imtdoc::IDocumentManager
{
public:
	CCollectionDocumentManager();

	// reimplemented (imtdoc::ICollectionDocumentManager)
	virtual DocumentList GetOpenedDocumentList(const QByteArray& userId) const override;
	virtual QByteArray CreateNewDocument(const QByteArray& userId, const QByteArray& documentTypeId) override;
	virtual QByteArray OpenDocument(const QByteArray& userId, const QUrl& url) override;
	virtual OperationStatus GetDocumentName(const QByteArray& userId, const QByteArray& documentId, QString& documentName) const override;
	virtual OperationStatus SetDocumentName(const QByteArray& userId, const QByteArray& documentId, const QString& documentName) override;
	virtual const istd::IChangeable* GetDocumentPtr(const QByteArray& userId, const QByteArray& documentId) const override;
	virtual OperationStatus GetDocumentData(const QByteArray& userId, const QByteArray& documentId, istd::IChangeableSharedPtr& documentPtr) const override;
	virtual OperationStatus SetDocumentData(const QByteArray& userId, const QByteArray& documentId, const istd::IChangeable& document) override;
	virtual OperationStatus SaveDocument(const QByteArray& userId, const QByteArray& documentId, const QString& documentName) override;
	virtual OperationStatus CloseDocument(const QByteArray& userId, const QByteArray& documentId) override;
	virtual OperationStatus GetDocumentUndoManager(
		const QByteArray& userId, const QByteArray& documentId, idoc::IUndoManager*& undoManagerPtr) const override;

	// reimplemented (iser::ISerializable)
	virtual bool Serialize(iser::IArchive& archive) override;

	// reimplemented (imod::CMultiModelObserverBase)
	virtual void OnUpdate(imod::IModel* modelPtr, const istd::IChangeable::ChangeSet& changeSet) override;

protected:
	struct WorkingDocument;

	bool ValidateInputParams(const QByteArray& userId, const QByteArray& documentId, OperationStatus& status) const;
	void OnUndoManagerChanged(int modelId);
	int GetUndoManagerNextModelId(const QByteArray& userId);
	WorkingDocument* FindDocument(const QByteArray& userId, const QByteArray& documentId);
	const WorkingDocument* FindDocument(const QByteArray& userId, const QByteArray& documentId) const;
	bool FindDocument(int undoManagerModelId, QByteArray& outUserId, QByteArray& outDocumentId);
	void InitializeDocumentObservers(
		WorkingDocument& document,
		const QByteArray& userId);

	virtual imtbase::IObjectCollection* GetCollection() const = 0;
	virtual istd::IChangeableSharedPtr CreateObject(const QByteArray& typeId) const = 0;
	virtual idoc::IUndoManagerSharedPtr CreateUndoManager() const = 0;

protected:
	struct WorkingDocument
	{
		QByteArray objectId;
		QByteArray objectTypeId;
		QString documentName;
		istd::IChangeableSharedPtr objectPtr;
		idoc::IUndoManagerSharedPtr undoManagerPtr;
		bool isDirty;
		int undoManagerModelId = -1;
	};

	class UndoManagerObserver : public imod::CMultiModelDispatcherBase
	{
	public:
		UndoManagerObserver(CCollectionDocumentManager& parent);

	protected:
		virtual void OnModelChanged(int modelId, const istd::IChangeable::ChangeSet& changeSet) override;

	private:
		CCollectionDocumentManager& m_parent;
	};

	typedef QMap<QByteArray, WorkingDocument> WorkingDocumentList;
	mutable QMap<QByteArray, WorkingDocumentList> m_userDocuments;
	mutable QRecursiveMutex m_mutex;

	UndoManagerObserver m_undoManagerObserver;

private:
	DocumentNotificationPtr CreateDocumentNotification(const QByteArray& userId, const QByteArray& documentId) const;
};


} // namespace imtdoc
