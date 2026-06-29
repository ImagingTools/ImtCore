// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// STL includes
#include <memory>

// Qt includes
#include <QtCore/QUrl>

// ACF includes
#include <idoc/IUndoManager.h>
#include <iser/ISerializable.h>
#include <imod/IObserver.h>

// ImtCore includes
#include <imtdoc/IDocumentServiceEventHandler.h>


namespace imtbase { class IOperationContext; }


/**
	\defgroup imtdoc imtdoc
	\brief Server-side multi-user document management.

	The \c imtdoc module provides the interface and infrastructure for
	managing documents in a multi-user server environment.  It is built
	around the following main abstractions:

	- \c IDocumentService — the primary service interface through which
	  clients create, open, save, close, and observe documents.
	- \c CDocumentServiceBase — the thread-safe implementation base
	  that serialises task execution and maintains the per-user
	  document registry.
	- \c TCollectionDocumentServiceWrap — a CRTP-style mixin that adds
	  collection-backed open/save/close semantics on top of any
	  \c CDocumentServiceBase-derived class.
	- A family of typed \c CEventBase subclasses that carry document
	  lifecycle notifications to registered \c IDocumentServiceEventHandler
	  observers.
*/
namespace imtdoc
{


/**
	\ingroup imtdoc
	\brief Primary interface for server-side, multi-user document management.

	Clients interact with the document service exclusively through this
	interface.  All mutating operations (create, open, save, close) are
	\e asynchronous: \c BeginDocumentTask enqueues the work and returns a
	unique task-ID; the caller then blocks in \c WaitForTaskFinished until
	the operation completes.

	Every document instance is identified by the combination of a
	\e userId (the identity of the session that owns the instance) and a
	\e documentId (a UUID assigned when the instance is created or opened).
	Multiple users may hold independent instances of the same underlying
	object at the same time.

	Event notifications about document lifecycle changes are delivered to
	\c IDocumentServiceEventHandler observers registered via
	\c RegisterEventHandler / \c UnregisterEventHandler.

	\note Implementations must be thread-safe.
*/
class IDocumentService : virtual public iser::ISerializable
{
public:
	/**
		\brief Outcome codes returned by synchronous accessor methods.
	*/
	enum OperationStatus
	{
		OS_OK = 0,             ///< The operation completed successfully.
		OS_INVALID_USER_ID,    ///< The supplied \a userId is unknown or empty.
		OS_INVALID_DOCUMENT_ID,///< The supplied \a documentId is unknown or empty.
		OS_INVALID_DOCUMENT_DATA,///< The document data failed validation.
		OS_FAILED              ///< A generic, unspecified failure occurred.
	};

	/**
		\brief Identifies the kind of asynchronous document operation
		started by \c BeginDocumentTask.
	*/
	enum TaskType
	{
		TT_NEW,   ///< Create a new blank document
		TT_OPEN,  ///< Open an existing document from a URL
		TT_SAVE,  ///< Save a document
		TT_CLOSE  ///< Close a document
	};

	/**
		\brief Input parameters for \c BeginDocumentTask.

		Only the fields relevant to the chosen \c TaskType need to be
		filled in; the rest can be left at their default values.

		| Field                      | TT_NEW | TT_OPEN | TT_SAVE | TT_CLOSE |
		|----------------------------|:------:|:-------:|:-------:|:--------:|
		| userId                     |   X    |    X    |    X    |    X     |
		| documentTypeId             |   X    |         |         |          |
		| proposedSourceDocumentId   |  (opt) |         |         |          |
		| url                        |        |    X    |         |          |
		| documentId                 |        |         |    X    |    X     |
		| documentName               |        |         |  (opt)  |          |
	*/
	struct TaskParams
	{
		QByteArray userId;
		QByteArray documentTypeId;
		QByteArray proposedSourceDocumentId;
		QUrl url;
		QByteArray documentId;
		QString documentName;
		const imtbase::IOperationContext* operationContextPtr = nullptr; ///< Optional operation context for tenant/owner propagation during save.
		const istd::IChangeable* defaultDataPtr = nullptr;
	};

	/**
		\brief Result returned by \c WaitForTaskFinished.
	*/
	struct TaskResult
	{
		OperationStatus status = OS_OK;
		QByteArray documentId;    ///< Set for TT_NEW and TT_OPEN on success
		QString errorMessage;
	};

	/**
		\brief Immediate-error output for \c BeginDocumentTask.

		Populated only when \c BeginDocumentTask cannot even create
		the task (e.g.\ missing required parameters).  In that case
		the method returns an empty task ID.
	*/
	struct Error
	{
		OperationStatus status = OS_OK;
		QString message;
		bool HasError() const { return status != OS_OK; }
	};

	struct DocumentInfo
	{
		bool initialState = false; ///< \c true when the document was restored from archive and needs server update.
		QByteArray typeId;        ///< Registered object type-ID of the document.
		QUrl url;                 ///< Source URL (e.g.\ \c collection://\<objectId\>).
		QString name;             ///< Human-readable document name.
		bool isDirty = false;     ///< \c true when the document has unsaved changes.
		bool hasNameProvider = false; ///< \c true when a name provider is registered for this type.
		bool isLoading = false;   ///< \c true while the document data is still being loaded asynchronously.
	};

	/**
		\brief Per-instance document descriptor returned by \c GetOpenedDocumentList.
	*/
	struct DocumentListItem : public DocumentInfo
	{
		QByteArray documentId; ///< Unique instance ID assigned when the document was opened.
	};
	typedef QList<DocumentListItem> DocumentList; ///< Ordered list of open document instances.

	/**
		\brief Snapshot of an open document instance, extended with the owning user-ID.

		Used as the payload of document-lifecycle notifications (e.g.\ when the
		QML layer subscribes to document-change events via GraphQL subscriptions).
	*/
	struct DocumentNotification : public DocumentListItem
	{
		QByteArray userId; ///< Identity of the user session that owns this document instance.
	};
	typedef std::shared_ptr<DocumentNotification> DocumentNotificationPtr;

	/**
		\brief Start an asynchronous document operation.

		\param taskType  The kind of operation to perform.
		\param params    Input parameters (see \c TaskParams for which
		                 fields are required per task type).
		\param errorPtr  Optional.  Receives an immediate error description
		                 when the task cannot be created at all.  In that
		                 case the method returns an empty \c QByteArray.
		\return          A unique task ID that can be passed to
		                 \c WaitForTaskFinished, or empty on immediate
		                 failure.
	*/
	virtual QByteArray BeginDocumentTask(
				TaskType taskType,
				const TaskParams& params,
				Error* errorPtr = nullptr) = 0;

	/**
		\brief Block until the task identified by \a taskId has finished.

		\param taskId  A task ID previously returned by
		               \c BeginDocumentTask.
		\return        The result of the completed task.

		\note If called from the GUI / main thread the implementation
		      must keep the Qt event loop alive (e.g.\ via
		      \c QCoreApplication::processEvents) to avoid dead-locks
		      with completion handlers that are posted to the main
		      thread.
	*/
	virtual TaskResult WaitForTaskFinished(const QByteArray& taskId) = 0;

	/**
		\brief Return all open document instances for the given user session.

		\param userId  Identity of the session whose documents should be listed.
		\return        Ordered list of \c DocumentListItem descriptors; empty
		               when no documents are open for \a userId.
	*/
	virtual DocumentList GetOpenedDocumentList(const QByteArray& userId) const = 0;

	/**
		\brief Retrieve the human-readable name of an open document.

		\param userId        Owning user session.
		\param documentId    Instance ID of the document.
		\param documentName  Receives the current name on success.
		\return              \c OS_OK, \c OS_INVALID_USER_ID, or
		                     \c OS_INVALID_DOCUMENT_ID.
	*/
	virtual OperationStatus GetDocumentName(const QByteArray& userId, const QByteArray& documentId, QString& documentName) const = 0;

	/**
		\brief Change the human-readable name of an open document.

		\param userId        Owning user session.
		\param documentId    Instance ID of the document.
		\param documentName  New name to assign.
		\return              \c OS_OK on success, or an appropriate error code.
	*/
	virtual OperationStatus SetDocumentName(const QByteArray& userId, const QByteArray& documentId, const QString& documentName) = 0;

	/**
		\brief Return a non-owning pointer to the document's data object.

		The pointer remains valid only as long as the document is open.
		Prefer \c GetDocumentData when shared ownership is needed.

		\return  Pointer to the data object, or \c nullptr when not found.
	*/
	virtual const istd::IChangeable* GetDocumentPtr(const QByteArray& userId, const QByteArray& documentId) const = 0;

	/**
		\brief Retrieve a shared-ownership handle to the document's data object.

		\param documentPtr  Receives the shared pointer on success.
		\return             \c OS_OK, \c OS_INVALID_USER_ID, or
		                    \c OS_INVALID_DOCUMENT_ID.
	*/
	virtual OperationStatus GetDocumentData(const QByteArray & userId, const QByteArray & documentId, istd::IChangeableSharedPtr& documentPtr) const = 0;

	/**
		\brief Replace the in-memory data of an open document.

		A deep copy of \a document is stored; the undo manager is \e not
		automatically updated.

		\return  \c OS_OK on success, or an appropriate error code.
	*/
	virtual OperationStatus SetDocumentData(const QByteArray& userId, const QByteArray& documentId, const istd::IChangeable& document) = 0;

	/**
		\brief Retrieve the undo manager associated with an open document.

		\param undoManagerPtr  Receives a raw (non-owning) pointer to the undo
		                       manager on success.
		\return                \c OS_OK on success, or an appropriate error code.
	*/
	virtual OperationStatus GetDocumentUndoManager(
				const QByteArray& userId,
				const QByteArray& documentId,
				idoc::IUndoManager*& undoManagerPtr) const = 0;

	/**
		\brief Subscribe \a observer to model-change notifications for an open document.

		\return  \c OS_OK on success, or an appropriate error code.
	*/
	virtual OperationStatus RegisterDocumentObserver(const QByteArray& userId, const QByteArray& documentId, imod::IObserver& observer) = 0;

	/**
		\brief Unsubscribe \a observer from model-change notifications.

		\return  \c OS_OK on success, or an appropriate error code.
	*/
	virtual OperationStatus UnregisterDocumentObserver(const QByteArray& userId, const QByteArray& documentId, imod::IObserver& observer) = 0;

	/**
		\brief Register a service-level event handler.

		The handler will receive all document lifecycle events (created,
		opened, loaded, changed, saved, saved-as, renamed, closed,
		undo/redo state changed) via \c IDocumentServiceEventHandler::ProcessEvent.
		Registration is additive: the same handler may not be registered twice.

		\param handler  The handler to add; must outlive the service or be
		                unregistered before it is destroyed.
	*/
	virtual void RegisterEventHandler(IDocumentServiceEventHandler& handler) = 0;

	/**
		\brief Unregister a previously registered event handler.

		\param handler  The handler to remove.  Has no effect when \a handler
		                is not currently registered.
	*/
	virtual void UnregisterEventHandler(IDocumentServiceEventHandler& handler) = 0;
};


} // namespace imtdoc


Q_DECLARE_METATYPE(imtdoc::IDocumentService::DocumentNotification);


