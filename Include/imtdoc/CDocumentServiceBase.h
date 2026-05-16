// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// Qt includes
#include <QtCore/QMutex>
#include <QtCore/QString>
#include <QtCore/QThread>

// STL includes
#include <atomic>
#include <memory>

// ACF includes
#include <idoc/IDocumentManager.h>
#include <idoc/IUndoManager.h>
#include <imod/TMultiModelObserverBase.h>
#include <imod/CMultiModelDispatcherBase.h>

// ImtCore includes
#include <imtdoc/IDocumentService.h>
#include <imtdoc/IDocumentServiceEventHandler.h>


namespace imtdoc
{


class CDocumentServiceBase:
			protected imod::TMultiModelObserverBase<istd::IChangeable>,
			virtual public imtdoc::IDocumentService
{
public:
	CDocumentServiceBase();
	~CDocumentServiceBase();

	// reimplemented (imtdoc::IDocumentService)
	virtual DocumentList GetOpenedDocumentList(const QByteArray& userId) const override;
	virtual QByteArray CreateNewDocument(
		const QByteArray& userId,
		const QByteArray& documentTypeId,
		const QByteArray& proposedSourceDocumentId = QByteArray()) override;
	virtual QByteArray OpenDocument(const QByteArray& userId, const QUrl& url) override;
	virtual OperationStatus GetDocumentName(const QByteArray& userId, const QByteArray& documentId, QString& documentName) const override;
	virtual OperationStatus SetDocumentName(const QByteArray& userId, const QByteArray& documentId, const QString& documentName) override;
	virtual const istd::IChangeable* GetDocumentPtr(const QByteArray& userId, const QByteArray& documentId) const override;
	virtual OperationStatus GetDocumentData(const QByteArray& userId, const QByteArray& documentId, istd::IChangeableSharedPtr& documentPtr) const override;
	virtual OperationStatus SetDocumentData(const QByteArray& userId, const QByteArray& documentId, const istd::IChangeable& document) override;
	virtual OperationStatus SaveDocument(
		const QByteArray& userId,
		const QByteArray& documentId,
		const QString& documentName = QString(),
		QString* errorMessage = nullptr) override;
	virtual OperationStatus CloseDocument(const QByteArray& userId, const QByteArray& documentId) override;
	virtual OperationStatus GetDocumentUndoManager(
		const QByteArray& userId, const QByteArray& documentId, idoc::IUndoManager*& undoManagerPtr) const override;
	virtual OperationStatus RegisterDocumentObserver(const QByteArray& userId, const QByteArray& documentId, imod::IObserver& observer) override;
	virtual OperationStatus UnregisterDocumentObserver(const QByteArray& userId, const QByteArray& documentId, imod::IObserver& observer) override;

	// reimplemented (iser::ISerializable)
	virtual bool Serialize(iser::IArchive& archive) override;

protected:
	struct WorkingDocument;

	bool ValidateInputParams(const QByteArray& userId, const QByteArray& documentId, OperationStatus& status) const;
	int GetUndoManagerNextModelId(const QByteArray& userId);
	void InitializeDocumentObservers(WorkingDocument& document, const QByteArray& userId);
	WorkingDocument* FindDocument(const QByteArray& userId, const QByteArray& documentId);
	const WorkingDocument* FindDocument(const QByteArray& userId, const QByteArray& documentId) const;
	bool FindDocument(int undoManagerModelId, QByteArray& outUserId, QByteArray& outDocumentId);
	QUrl ObjectIdToUrl(const QByteArray& objectId);
	DocumentNotificationPtr CreateDocumentNotification(const QByteArray& userId, const QByteArray& documentId) const;

	void OnDocumentDataLoaded(const QByteArray& userId, const QByteArray& documentId);
	void OnUndoManagerChanged(int modelId);

	virtual QString GetDefaultDocumentName(const WorkingDocument& document) const;
	virtual bool HasDocumentNameProvider(const QByteArray& typeId) const;
	virtual bool ValidateDocumentData(
		const WorkingDocument& document,
		OperationStatus& status,
		QString* errorMessage = nullptr) const;
	virtual QList<imtdoc::IDocumentServiceEventHandler*> GetDocumentServiceEventHandlers() const;

	virtual istd::IChangeableUniquePtr CreateObject(const QByteArray& typeId) const = 0;
	virtual idoc::IUndoManagerUniquePtr CreateUndoManager() const = 0;

	static QString GetInvalidDocumentMessage();

	// reimplemented (imod::CMultiModelObserverBase)
	virtual void OnUpdate(imod::IModel* modelPtr, const istd::IChangeable::ChangeSet& changeSet) override;

protected:
	struct WorkingDocument
	{
		QByteArray objectId;
		QByteArray typeId;
		QUrl url;
		QString name;
		istd::IChangeableSharedPtr objectPtr;
		idoc::IUndoManagerSharedPtr undoManagerPtr;
		bool isDirty;
		bool isLoading = false;
		int undoManagerModelId = -1;
	};

	struct SharedDocumentData
	{
		QByteArray typeId;
		QString name;
		istd::IChangeableSharedPtr objectPtr;
		idoc::IUndoManagerSharedPtr undoManagerPtr;
		int refCount = 0;
		bool isLoading = false;
		int undoManagerModelId = -1;
	};

	class UndoManagerObserver : public imod::CMultiModelDispatcherBase
	{
	public:
		UndoManagerObserver(CDocumentServiceBase& parent);

	protected:
		virtual void OnModelChanged(int modelId, const istd::IChangeable::ChangeSet& changeSet) override;

	private:
		CDocumentServiceBase& m_parent;
	};

	virtual bool IsSingleCopyMode() const;

	typedef QPair<QByteArray, QByteArray> UserDocumentPair;
	typedef QList<UserDocumentPair> UserDocumentPairList;
	UserDocumentPairList FindDocumentsByObjectId(const QByteArray& objectId) const;

	typedef QMap<QByteArray, WorkingDocument> WorkingDocumentList;
	mutable QMap<QByteArray, WorkingDocumentList> m_userDocuments;
	mutable QRecursiveMutex m_mutex;

	QMap<QByteArray, SharedDocumentData> m_sharedDocuments;

	UndoManagerObserver m_undoManagerObserver;
	std::shared_ptr<std::atomic<bool>> m_isAlive;
};


} // namespace imtdoc
