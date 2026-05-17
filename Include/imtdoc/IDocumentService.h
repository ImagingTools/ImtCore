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


namespace imtdoc
{


class IDocumentService : virtual public iser::ISerializable
{
public:
	enum OperationStatus
	{
		OS_OK = 0,
		OS_INVALID_USER_ID,
		OS_INVALID_DOCUMENT_ID,
		OS_INVALID_DOCUMENT_DATA,
		OS_FAILED
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
		QByteArray typeId;
		QUrl url;
		QString name;
		bool isDirty = false;
		bool hasNameProvider = false;
		bool isLoading = false;
	};

	struct DocumentListItem : public DocumentInfo
	{
		QByteArray documentId;
	};
	typedef QList<DocumentListItem> DocumentList;

	struct DocumentNotification : public DocumentListItem
	{
		QByteArray userId;
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
		Get a list of open document instances for a given user-ID
	*/
	virtual DocumentList GetOpenedDocumentList(const QByteArray& userId) const = 0;

	/**
		Get name of the document with the given user-ID and document-ID
	*/
	virtual OperationStatus GetDocumentName(const QByteArray& userId, const QByteArray& documentId, QString& documentName) const = 0;

	/**
		Set name of the document with the given user-ID and document-ID
	*/
	virtual OperationStatus SetDocumentName(const QByteArray& userId, const QByteArray& documentId, const QString& documentName) = 0;

	/**
		Get document data pointer with the given user-ID and document-ID
	*/
	virtual const istd::IChangeable* GetDocumentPtr(const QByteArray& userId, const QByteArray& documentId) const = 0;

	/**
		Get document data with the given user-ID and document-ID
	*/
	virtual OperationStatus GetDocumentData(const QByteArray & userId, const QByteArray & documentId, istd::IChangeableSharedPtr& documentPtr) const = 0;

	/**
		Set document data with the given user-ID and document-ID
	*/
	virtual OperationStatus SetDocumentData(const QByteArray& userId, const QByteArray& documentId, const istd::IChangeable& document) = 0;

	/**
		Get UndoManager for the document with the given user-ID and document-ID
	*/
	virtual OperationStatus GetDocumentUndoManager(
				const QByteArray& userId,
				const QByteArray& documentId,
				idoc::IUndoManager*& undoManagerPtr) const = 0;

	/**
		Register an observer for the document with the given user-ID and document-ID
	*/
	virtual OperationStatus RegisterDocumentObserver(const QByteArray& userId, const QByteArray& documentId, imod::IObserver& observer) = 0;

	/**
		Unregister an observer for the document with the given user-ID and document-ID
	*/
	virtual OperationStatus UnregisterDocumentObserver(const QByteArray& userId, const QByteArray& documentId, imod::IObserver& observer) = 0;
};


} // namespace imtdoc


Q_DECLARE_METATYPE(imtdoc::IDocumentService::DocumentNotification);


