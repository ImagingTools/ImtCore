// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// Qt includes
#include <QtCore/QMap>
#include <QtCore/QMutex>
#include <QtCore/QString>
#include <QtCore/QThread>
#include <QtCore/QWaitCondition>

// STL includes
#include <atomic>
#include <memory>

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

	Open documents are stored in a two-level map:
	\c m_userDocuments[userId][documentId].  Each entry is a
	\c WorkingDocument that holds the data object, undo manager, metadata,
	and model-observer registration.

	## Single-copy mode

	When \c IsSingleCopyMode() returns \c true, all users that open the
	same underlying collection object share a single \c SharedDocumentData
	entry.  The first user to open the object triggers the asynchronous
	load; subsequent opens attach immediately and receive a
	\c CDocumentDataLoadedEvent when the first load completes.

	## Subclassing

	Concrete subclasses override:
	- \c CreateObject / \c CreateUndoManager — mandatory factories.
	- Do*Document — to implement collection-specific open/save/close logic
	  (see \c TCollectionDocumentServiceWrap).
	- \c GetDefaultDocumentName, \c HasDocumentNameProvider,
	  \c ValidateDocumentData, \c GetDocumentServiceEventHandlers,
	  \c IsSingleCopyMode — optional policy hooks.
*/
class CDocumentServiceBase:
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

		Removes the entry from \c m_userDocuments, decrements the shared
		reference count (in single-copy mode), and fires
		\c CDocumentClosedEvent.
	*/
	virtual void DoCloseDocument(const QByteArray& taskId, const TaskParams& params);

	/**
		\brief Close a document internally (shared implementation used by
		both \c DoCloseDocument and failure paths of \c DoCreateNewDocument /
		\c DoOpenDocument).
	*/
	OperationStatus CloseDocumentInternal(const QByteArray& userId, const QByteArray& documentId);

	/**
		\brief Mark a pending task as finished.

		Wakes any thread that is blocked in \c WaitForTaskFinished
		for this task ID.
	*/
	void CompleteTask(const QByteArray& taskId, const TaskResult& result);

	bool ValidateInputParams(const QByteArray& userId, const QByteArray& documentId, OperationStatus& status) const;
	int GetUndoManagerNextModelId(const QByteArray& userId);
	void InitializeDocumentObservers(WorkingDocument& document, const QByteArray& userId);
	WorkingDocument* FindDocument(const QByteArray& userId, const QByteArray& documentId);
	const WorkingDocument* FindDocument(const QByteArray& userId, const QByteArray& documentId) const;
	bool FindDocument(int undoManagerModelId, QByteArray& outUserId, QByteArray& outDocumentId);
	QUrl ObjectIdToUrl(const QByteArray& objectId);
	void OnDocumentDataLoaded(const QByteArray& userId, const QByteArray& documentId);
	void OnUndoManagerChanged(int modelId);

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

		One \c WorkingDocument is kept per (userId, documentId) pair.
	*/
	struct WorkingDocument
	{
		QByteArray objectId;                       ///< Backing collection element ID (empty for unsaved documents).
		QByteArray typeId;                         ///< Registered object type-ID.
		QUrl url;                                  ///< Source URL (mirrors the open URL).
		QString name;                              ///< Current display name.
		istd::IChangeableSharedPtr objectPtr;      ///< The document's data object.
		idoc::IUndoManagerSharedPtr undoManagerPtr;///< Associated undo/redo manager.
		bool isDirty;                              ///< \c true when there are unsaved changes.
		bool isLoading = false;                    ///< \c true while the background load is in progress.
		int undoManagerModelId = -1;               ///< Model registration ID in \c UndoManagerObserver.
	};

	/**
		\brief Shared state for a collection object opened in single-copy mode.

		Multiple \c WorkingDocument entries for different users may point to
		the same underlying data via a \c SharedDocumentData entry.
	*/
	struct SharedDocumentData
	{
		QByteArray typeId;                         ///< Registered object type-ID.
		QString name;                              ///< Current display name.
		istd::IChangeableSharedPtr objectPtr;      ///< Shared data object (loaded once).
		idoc::IUndoManagerSharedPtr undoManagerPtr;///< Shared undo/redo manager.
		int refCount = 0;                          ///< Number of user sessions holding this entry.
		bool isLoading = false;                    ///< \c true while the background load is in progress.
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

	typedef QPair<QByteArray, QByteArray> UserDocumentPair;
	typedef QList<UserDocumentPair> UserDocumentPairList;
	UserDocumentPairList FindDocumentsByObjectId(const QByteArray& objectId) const;

	typedef QMap<QByteArray, WorkingDocument> WorkingDocumentList;
	mutable QMap<QByteArray, WorkingDocumentList> m_userDocuments; ///< Two-level map: userId → documentId → WorkingDocument.
	mutable QRecursiveMutex m_mutex; ///< Guards \c m_userDocuments and \c m_sharedDocuments.

	QMap<QByteArray, SharedDocumentData> m_sharedDocuments; ///< Single-copy-mode shared entries (objectId → SharedDocumentData).

	UndoManagerObserver m_undoManagerObserver; ///< Receives undo-manager model change callbacks.
	std::shared_ptr<std::atomic<bool>> m_isAlive; ///< Shared alive-guard passed to background threads to detect early service destruction.

	QMap<QByteArray, std::shared_ptr<TaskContext>> m_pendingTasks; ///< Currently executing or pending tasks (taskId → context).
	mutable QMutex m_tasksMutex; ///< Guards \c m_pendingTasks.

	QList<IDocumentServiceEventHandler*> m_registeredEventHandlers; ///< Runtime-registered event handlers.
};


} // namespace imtdoc


