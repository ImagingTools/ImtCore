// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// STL includes
#include <atomic>
#include <memory>

// Qt includes
#include <QtCore/QMap>
#include <QtCore/QMutex>
#include <QtCore/QObject>
#include <QtCore/QPointer>
#include <QtCore/QString>
#include <QtCore/QThread>
#include <QtCore/QWaitCondition>

// ACF includes
#include <idoc/IUndoManager.h>
#include <imod/TMultiModelObserverBase.h>
#include <imod/CMultiModelDispatcherBase.h>

// ImtCore includes
#include <imtdoc/IDocumentService.h>
#include <imtdoc/IDocumentServiceEventHandler.h>


namespace imtdoc
{


/**
	\ingroup imtdoc
	\brief Thread-safe implementation base for \c IDocumentService.

	\c CDocumentServiceBase provides the complete \c IDocumentService API
	except for the two pure-virtual factory methods (\c CreateObject and
	\c CreateUndoManager) that concrete subclasses must supply.

	## Task dispatch

	All mutating operations are serialised through a task queue.
	\c BeginDocumentTask creates a \c TaskContext entry, enqueues the
	appropriate Do*-method call onto the current thread, and returns a
	task-ID.  \c WaitForTaskFinished blocks (using a \c QWaitCondition)
	until \c CompleteTask is called, either synchronously inside the
	Do*-method or from a background thread.

	## Document registry

	Every open document is described by a \c WorkingDocument that holds the
	data object, undo manager, metadata, and model-observer registration.
	Documents are stored in \c m_documents keyed by documentId;
	\c m_userDocumentIds maps each userId to the list of its documentIds and
	is the only source of document ownership.
	Use \c InsertDocument / \c RemoveDocument to keep both maps in sync.

	## Single-copy mode

	When \c IsSingleCopyMode() returns \c true, all users that open the
	same underlying collection object share a single \c WorkingDocument:
	its documentId is added to the document list of every such user.
	The first user to open the object triggers the asynchronous load;
	subsequent opens attach immediately and receive a
	\c CDocumentDataLoadedEvent when the load completes.  The document is
	really closed when the last user holding it closes it.

	## Subclassing

	Concrete subclasses override:
	- \c CreateObject / \c CreateUndoManager — mandatory factories.
	- Do*Document — to implement collection-specific open/save/close logic
	  (see \c TCollectionDocumentServiceWrap).
	- \c GetDocumentUrl — converts the document source reference to the public URL.
	- \c GetDefaultDocumentName, \c HasDocumentNameProvider,
	  \c ValidateDocumentData, \c GetDocumentServiceEventHandlers,
	  \c IsSingleCopyMode — optional policy hooks.
*/
class CDocumentServiceBase:
			public QObject,
			protected imod::TMultiModelObserverBase<istd::IChangeable>,
			public imtdoc::IDocumentService
{
public:
	CDocumentServiceBase();
	~CDocumentServiceBase();

	// reimplemented (imtdoc::IDocumentService)
	virtual QByteArray BeginDocumentTask(
				TaskType taskType,
				const TaskParams& params,
				Error* errorPtr = nullptr) override;
	virtual TaskResult WaitForTaskFinished(const QByteArray& taskId) override;
	virtual DocumentList GetOpenedDocumentList(const QByteArray& userId) const override;
	virtual OperationStatus GetDocumentName(const QByteArray& userId, const QByteArray& documentId, QString& documentName) const override;
	virtual OperationStatus SetDocumentName(const QByteArray& userId, const QByteArray& documentId, const QString& documentName) override;
	virtual const istd::IChangeable* GetDocumentPtr(const QByteArray& userId, const QByteArray& documentId) const override;
	virtual OperationStatus GetDocumentData(const QByteArray& userId, const QByteArray& documentId, istd::IChangeableSharedPtr& documentPtr) const override;
	virtual OperationStatus SetDocumentData(const QByteArray& userId, const QByteArray& documentId, const istd::IChangeable& document) override;
	virtual OperationStatus ExecuteDocumentMethod(const QByteArray& userId, const QByteArray& documentId, const CDocumentMethod& method) override;
	virtual OperationStatus GetDocumentUndoManager(
				const QByteArray& userId,
				const QByteArray& documentId,
				idoc::IUndoManager*& undoManagerPtr) const override;
	virtual OperationStatus RegisterDocumentObserver(const QByteArray& userId, const QByteArray& documentId, imod::IObserver& observer) override;
	virtual OperationStatus UnregisterDocumentObserver(const QByteArray& userId, const QByteArray& documentId, imod::IObserver& observer) override;
	virtual void RegisterEventHandler(IDocumentServiceEventHandler& handler) override;
	virtual void UnregisterEventHandler(IDocumentServiceEventHandler& handler) override;

	// reimplemented (iser::ISerializable)
	virtual bool Serialize(iser::IArchive& archive) override;

protected:
	struct WorkingDocument;

	// Protected virtual methods for task dispatch — override in
	// subclasses to provide task-type-specific behaviour.
	// Each method must call \c CompleteTask exactly once (synchronously
	// or asynchronously) to signal completion.

	/**
		\brief Create a new blank document for \a params.userId.

		The base implementation creates a new data object via \c CreateObject
		and an undo manager via \c CreateUndoManager, registers observers,
		and fires \c CDocumentCreatedEvent followed by
		\c CDocumentDataLoadedEvent.
	*/
	virtual void DoCreateNewDocument(const QByteArray& taskId, const TaskParams& params);

	/**
		\brief Open an existing document addressed by \a params.url.

		Resolves the URL to a collection object ID, starts a background
		thread to fetch the data, and fires \c CDocumentOpenedEvent
		immediately.  Once the data is available \c CDocumentDataLoadedEvent
		is fired from the main thread.
	*/
	virtual void DoOpenDocument(const QByteArray& taskId, const TaskParams& params);

	/**
		\brief Persist the document identified by \a params.documentId.

		Validates the data, writes it back to the collection, and fires
		\c CDocumentSavedEvent (or \c CDocumentSavedAsEvent when a new
		collection element is created).
	*/
	virtual void DoSaveDocument(const QByteArray& taskId, const TaskParams& params);

	/**
		\brief Close the document identified by \a params.documentId.

		Removes the document from the user's document list (the entry in
		\c m_documents is released together with the last user holding it),
		and fires \c CDocumentClosedEvent.
	*/
	virtual void DoCloseDocument(const QByteArray& taskId, const TaskParams& params);

	/**
		\brief Close a document internally (shared implementation used by
		both \c DoCloseDocument and failure paths of \c DoCreateNewDocument /
		\c DoOpenDocument).
	*/
	virtual OperationStatus CloseDocumentInternal(const QByteArray& userId, const QByteArray& documentId);

	/**
		\brief Mark a pending task as finished.

		Wakes any thread that is blocked in \c WaitForTaskFinished
		for this task ID.
	*/
	void CompleteTask(const QByteArray& taskId, const TaskResult& result);

	bool ValidateInputParams(const QByteArray& userId, const QByteArray& documentId, OperationStatus& status) const;
	int GetUndoManagerNextModelId(const QByteArray& userId);
	void InitializeDocumentObservers(WorkingDocument& document, const QByteArray& userId, const QByteArray& documentId);
	WorkingDocument* FindDocument(const QByteArray& userId, const QByteArray& documentId);
	const WorkingDocument* FindDocument(const QByteArray& userId, const QByteArray& documentId) const;
	bool FindDocument(int undoManagerModelId, QByteArray& outDocumentId) const;
	/**
		Returns the document \a documentId, creating it if it does not exist yet,
		and registers its ID in the document list of \a userId.
	*/
	WorkingDocument& InsertDocument(const QByteArray& userId, const QByteArray& documentId);
	/**
		Removes \a documentId from the document list of \a userId.
		The document itself is removed from \c m_documents when no other user holds it.
	*/
	void RemoveDocument(const QByteArray& userId, const QByteArray& documentId);
	void ClearDocuments();
	QByteArrayList GetUserDocumentIds(const QByteArray& userId) const;
	/**
		Returns the IDs of all users holding the document \a documentId.
	*/
	QByteArrayList GetDocumentUserIds(const QByteArray& documentId) const;
	/**
		A document that was never saved (no backing URL) is dirty regardless of its undo state;
		otherwise it is dirty when the undo manager reports changes since the last save.
	*/
	static bool IsDocumentDirty(const WorkingDocument& document);
	void OnDocumentDataLoaded(const QByteArray& userId, const QByteArray& documentId);
	void OnUndoManagerChanged(int modelId);
	void OnCreateDocumentThreadStarted(
				const std::weak_ptr<std::atomic<bool>>& aliveGuard,
				const QByteArray& documentTypeId,
				const QByteArray& userId,
				const QByteArray& documentId,
				const QByteArray& taskId,
				QObject* worker,
				const istd::IChangeable* defaultDataPtr);
	void OnCreateDocumentThreadFinished(
				const std::weak_ptr<std::atomic<bool>>& aliveGuard,
				const QByteArray& userId,
				const QByteArray& documentId,
				const QByteArray& taskId,
				const iprm::IParamsSet* initParamsPtr);

	/**
		Returns the public URL of the document source.
		The base implementation returns an empty URL, storage-specific subclasses
		build it from \c WorkingDocument::documentSourceRef.
	*/
	virtual QUrl GetDocumentUrl(const WorkingDocument& document) const;
	virtual QString GetDefaultDocumentName(const WorkingDocument& document) const;
	virtual bool HasDocumentNameProvider(const QByteArray& typeId) const;
	virtual bool ValidateDocumentData(
				const WorkingDocument& document,
				OperationStatus& status,
				QString* errorMessage = nullptr,
				const imtbase::IOperationContext* operationContextPtr = nullptr) const;
	virtual QList<imtdoc::IDocumentServiceEventHandler*> GetDocumentServiceEventHandlers() const;

	virtual istd::IChangeableUniquePtr CreateObject(const QByteArray& typeId) const = 0;
	virtual idoc::IUndoManagerUniquePtr CreateUndoManager() const = 0;

	/**
		\brief Called after a new document is created from the factory but before
		the undo manager is attached.

		Subclasses must implement to perform initialization that should not
		generate undo history.

		\param typeId       Registered object type-ID of the document.
		\param initParams   Optional initialization parameters (may be \c nullptr).
		\param document     Mutable reference to the document data object.
		\param errorMessage Out-parameter for a human-readable error
		                    description when the method returns \c false.
		\return             \c true on success; \c false if initialization failed
		                    (e.g.\ init params provided but no delegate registered,
		                    or the delegate rejected the parameters).
	*/
	virtual bool OnDocumentCreated(const QByteArray& typeId, const iprm::IParamsSet* initParams, istd::IChangeable& document, QString& errorMessage) = 0;

	static QString GetInvalidDocumentMessage();

	// reimplemented (imod::CMultiModelObserverBase)
	virtual void OnUpdate(imod::IModel* modelPtr, const istd::IChangeable::ChangeSet& changeSet) override;

protected:
	/**
		\brief In-memory state of a single open document instance.

		One \c WorkingDocument is kept per documentId.  In single-copy mode it
		may be held by several users.
	*/
	struct WorkingDocument
	{
		QByteArray typeId;                         ///< Registered object type-ID.
		QByteArray documentSourceRef;              ///< Storage-specific reference to the document source, set by the subclass (empty for unsaved documents).
		QString name;                              ///< Current display name.
		istd::IChangeableSharedPtr objectPtr;      ///< The document's data object.
		idoc::IUndoManagerSharedPtr undoManagerPtr;///< Associated undo/redo manager.
		bool isDirty;                              ///< \c true when there are unsaved changes.
		bool isLoading = false;                    ///< \c true while the background load is in progress.
		bool singleDocumentInstance = false;       ///< \c true when the document was opened with \c TaskParams::singleDocumentInstance.
		int undoManagerModelId = -1;               ///< Model registration ID in \c UndoManagerObserver.
	};

	/**
		\brief Internal observer that relays undo-manager model change events.

		Registered once per document undo manager and calls
		\c CDocumentServiceBase::OnUndoManagerChanged when the manager's
		state changes (e.g.\ when the dirty flag flips after an edit or a
		save).
	*/
	class UndoManagerObserver : public imod::CMultiModelDispatcherBase
	{
	public:
		UndoManagerObserver(CDocumentServiceBase& parent);

	protected:
		virtual void OnModelChanged(int modelId, const istd::IChangeable::ChangeSet& changeSet) override;

	private:
		CDocumentServiceBase& m_parent;
	};

	/**
		\brief Internal context for a single pending task.

		Shared between \c BeginDocumentTask (producer) and
		\c WaitForTaskFinished (consumer).
	*/
	struct TaskContext
	{
		TaskResult result;
		bool isFinished = false;
		QMutex mutex;
		QWaitCondition condition;
	};

	virtual bool IsSingleCopyMode() const;

	typedef QMap<QByteArray, WorkingDocument> WorkingDocumentMap;
	typedef QMap<QByteArray, QByteArrayList> UserDocumentIdsMap;
	mutable WorkingDocumentMap m_documents; ///< All open documents: documentId → WorkingDocument.
	UserDocumentIdsMap m_userDocumentIds; ///< userId → IDs of the documents opened by the user.
	mutable QRecursiveMutex m_mutex; ///< Guards \c m_documents and \c m_userDocumentIds.

	UndoManagerObserver m_undoManagerObserver; ///< Receives undo-manager model change callbacks.
	std::shared_ptr<std::atomic<bool>> m_isAlive; ///< Shared alive-guard passed to background threads to detect early service destruction.

	QMap<QByteArray, std::shared_ptr<TaskContext>> m_pendingTasks; ///< Currently executing or pending tasks (taskId → context).
	mutable QMutex m_tasksMutex; ///< Guards \c m_pendingTasks.

	QList<QPointer<QThread>> m_workerThreads; ///< Background threads spawned for async document creation.
	mutable QMutex m_workerThreadsMutex; ///< Guards \c m_workerThreads.

	QList<IDocumentServiceEventHandler*> m_registeredEventHandlers; ///< Runtime-registered event handlers.
};


} // namespace imtdoc
