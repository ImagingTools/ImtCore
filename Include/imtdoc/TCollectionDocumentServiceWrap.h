// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// Qt includes
#include <QtCore/QCoreApplication>
#include <QtCore/QMap>
#include <QtCore/QMetaObject>
#include <QtCore/QUuid>
#include <QtCore/QThread>

// STL includes
#include <functional>

// ACF includes
#include <istd/CChangeGroup.h>
#include <istd/CChangeNotifier.h>

// ImtCore includes
#include <imtbase/IObjectCollection.h>
#include <imtdoc/CDocumentServiceBase.h>
#include <imtdoc/CDocumentClosedEvent.h>
#include <imtdoc/CDocumentOpenedEvent.h>
#include <imtdoc/CDocumentRenamedEvent.h>
#include <imtdoc/CDocumentSavedAsEvent.h>
#include <imtdoc/CDocumentSavedEvent.h>


namespace imtdoc
{


/**
	\ingroup imtdoc
	\brief CRTP mixin that adds collection-backed document operations to any \c CDocumentServiceBase-derived class.

	\c TCollectionDocumentServiceWrap layers the open/save/close semantics of
	an \c imtbase::IObjectCollection on top of any class that is derived (directly
	or indirectly) from \c CDocumentServiceBase.  It overrides the four Do*Document
	virtual methods and the \c SetDocumentName method:

	- **DoCreateNewDocument** — delegates to the base-class implementation (which
	  creates a blank object via \c CreateObject) and records an optional proposed
	  source element ID for later use during save.
	- **DoOpenDocument** — resolves the URL to a collection object ID, fetches
	  the object data in a background thread, and supports single-copy sharing of
	  data across user sessions.
	- **DoSaveDocument** — validates the data, then either updates an existing
	  collection element or inserts a new one (creating a copy when the name
	  changes); fires \c CDocumentSavedEvent or \c CDocumentSavedAsEvent.
	- **DoCloseDocument** — removes the document entry and fires
	  \c CDocumentClosedEvent.
	- **SetDocumentName** — renames the document in the collection and fires
	  \c CDocumentRenamedEvent for every affected user session.

	Subclasses must implement \c GetCollection() to provide the backing
	collection.

	\tparam Base  A class that is derived (directly or indirectly) from
	              \c CDocumentServiceBase (and therefore from
	              \c IDocumentService).  Typically \c CDocumentServiceCompBase
	              for ACF-wired components.

	\note The backward-compatibility typedef
	      \code
	      typedef TCollectionDocumentServiceWrap<CDocumentServiceBase> CCollectionDocumentServiceBase;
	      \endcode
	      is provided so that existing code using the old name continues to
	      compile.
*/
template<class Base>
class TCollectionDocumentServiceWrap : public Base
{
public:
	typedef typename Base::WorkingDocument WorkingDocument;
	typedef typename Base::WorkingDocumentMap WorkingDocumentMap;
	typedef QPair<QByteArray, QByteArray> UserDocumentPair;
	typedef QList<UserDocumentPair> UserDocumentPairList;

	using TaskResult = typename Base::TaskResult;
	using TaskParams = typename Base::TaskParams;
	using OperationStatus = typename Base::OperationStatus;
	using DocumentList = typename Base::DocumentList;
	using DocumentListItem = typename Base::DocumentListItem;

	// reimplemented (imtdoc::CDocumentServiceBase)
	virtual void DoCreateNewDocument(const QByteArray& taskId, const TaskParams& params) override;
	virtual void DoOpenDocument(const QByteArray& taskId, const TaskParams& params) override;
	virtual void DoSaveDocument(const QByteArray& taskId, const TaskParams& params) override;
	virtual void DoCloseDocument(const QByteArray& taskId, const TaskParams& params) override;

	// reimplemented (imtdoc::IDocumentService)
	virtual OperationStatus SetDocumentName(const QByteArray& userId, const QByteArray& documentId, const QString& documentName) override;

protected:
	/**
		\brief Return the backing object collection.

		Must be implemented by concrete subclasses to provide access to the
		\c IObjectCollection that stores documents.

		\return  Non-null pointer to the collection, or \c nullptr when the
		         collection is not yet available (tasks will fail gracefully).
	*/
	virtual imtbase::IObjectCollection* GetCollection() const = 0;

	/**
		Returns the \c collection:///<objectId> URL of the collection object.
	*/
	static QUrl ObjectIdToUrl(const QByteArray& objectId);
	/**
		Extracts the collection object ID from a \c collection:///<objectId> URL.
		Returns an empty ID for unsaved documents (empty URL) or non-collection URLs.
	*/
	static QByteArray UrlToObjectId(const QUrl& url);
	/**
		Returns the (userId, documentId) pairs of all opened documents backed by the collection object \a objectId.
		\note \c m_mutex must be locked by the caller.
	*/
	UserDocumentPairList FindDocumentsByObjectId(const QByteArray& objectId) const;

private:
	void OnDeferredDocumentDataLoaded(
				const std::weak_ptr<std::atomic<bool>>& aliveGuard,
				const QByteArray& userId,
				const QByteArray& documentId);
	void ClearReservedOpen(const QByteArray& userId, const QByteArray& documentId);
	void OnOpenDocumentThreadStarted(
				const std::weak_ptr<std::atomic<bool>>& aliveGuard,
				const QByteArray& objectId,
				const QByteArray& userId,
				const QByteArray& documentId,
				QObject* worker);
	void OnOpenDocumentThreadFinished(
				const std::weak_ptr<std::atomic<bool>>& aliveGuard,
				const QByteArray& objectId,
				const QByteArray& userId,
				const QByteArray& documentId);

	QMap<QByteArray, QByteArray> m_proposedSourceDocumentIds; ///< Maps new documentId → proposed source element ID for copy-on-create.
};


// ─── Inline implementations ────────────────────────────────────────────────


template<class Base>
inline QUrl TCollectionDocumentServiceWrap<Base>::ObjectIdToUrl(const QByteArray& objectId)
{
	return QUrl(QStringLiteral("collection:///") + objectId);
}


template<class Base>
inline QByteArray TCollectionDocumentServiceWrap<Base>::UrlToObjectId(const QUrl& url)
{
	if (url.scheme() != QStringLiteral("collection")){
		return QByteArray();
	}

	const QStringList parts = url.path().split('/', Qt::SkipEmptyParts);
	if (parts.count() != 1){
		return QByteArray();
	}

	return parts.first().toUtf8();
}


template<class Base>
inline typename TCollectionDocumentServiceWrap<Base>::UserDocumentPairList TCollectionDocumentServiceWrap<Base>::FindDocumentsByObjectId(
			const QByteArray& objectId) const
{
	UserDocumentPairList result;

	for (auto userIt = this->m_userDocumentIds.constBegin(); userIt != this->m_userDocumentIds.constEnd(); ++userIt){
		for (const QByteArray& documentId : userIt.value()){
			auto documentIt = this->m_documents.constFind(documentId);
			if (documentIt != this->m_documents.constEnd() && UrlToObjectId(documentIt.value().url) == objectId){
				result.append(qMakePair(userIt.key(), documentId));
			}
		}
	}

	return result;
}


template<class Base>
inline void TCollectionDocumentServiceWrap<Base>::OnDeferredDocumentDataLoaded(
			const std::weak_ptr<std::atomic<bool>>& aliveGuard,
			const QByteArray& userId,
			const QByteArray& documentId)
{
	auto isAlive = aliveGuard.lock();
	if (!isAlive || !isAlive->load()){
		return;
	}

	this->OnDocumentDataLoaded(userId, documentId);
}


template<class Base>
inline void TCollectionDocumentServiceWrap<Base>::ClearReservedOpen(
			const QByteArray& userId,
			const QByteArray& documentId)
{
	QMutexLocker locker(&this->m_mutex);
	this->RemoveDocument(userId, documentId);
}


template<class Base>
inline void TCollectionDocumentServiceWrap<Base>::OnOpenDocumentThreadStarted(
			const std::weak_ptr<std::atomic<bool>>& aliveGuard,
			const QByteArray& objectId,
			const QByteArray& userId,
			const QByteArray& documentId,
			QObject* worker)
{
	auto isAlive = aliveGuard.lock();
	if (!isAlive || !isAlive->load()){
		worker->deleteLater();
		return;
	}

	imtbase::IObjectCollection* collPtr = GetCollection();
	imtbase::IObjectCollection::DataPtr dataPtr;
	bool success = (collPtr != nullptr) && collPtr->GetObjectData(objectId, dataPtr);
	isAlive = aliveGuard.lock();
	if (!isAlive || !isAlive->load()){
		worker->deleteLater();
		return;
	}

	{
		istd::CChangeNotifier notifier(this);
		QMutexLocker locker(&this->m_mutex);

		auto documentIt = this->m_documents.find(documentId);
		if (documentIt != this->m_documents.end()){
			if (success && dataPtr.IsValid()){
				documentIt.value().objectPtr = dataPtr;
			}
			else{
				documentIt.value().isLoading = false;

				// Loading failed: close the document for every user holding it
				for (const QByteArray& documentUserId : this->GetDocumentUserIds(documentId)){
					this->CloseDocumentInternal(documentUserId, documentId);
				}
			}
		}
	}

	worker->deleteLater();
}


template<class Base>
inline void TCollectionDocumentServiceWrap<Base>::OnOpenDocumentThreadFinished(
			const std::weak_ptr<std::atomic<bool>>& aliveGuard,
			const QByteArray& /*objectId*/,
			const QByteArray& userId,
			const QByteArray& documentId)
{
	auto isAlive = aliveGuard.lock();
	if (!isAlive || !isAlive->load()){
		return;
	}

	QByteArrayList userIdsToNotify;

	{
		istd::CChangeNotifier notifier(this);
		QMutexLocker locker(&this->m_mutex);

		auto documentIt = this->m_documents.find(documentId);
		if (documentIt != this->m_documents.end() && documentIt.value().objectPtr.IsValid() && documentIt.value().isLoading){
			this->InitializeDocumentObservers(documentIt.value(), userId, documentId);
			userIdsToNotify = this->GetDocumentUserIds(documentId);
		}
	}

	// OnDocumentDataLoaded sets isLoading=false
	for (const QByteArray& notifyUserId : userIdsToNotify){
		this->OnDocumentDataLoaded(notifyUserId, documentId);
	}

	if (userIdsToNotify.isEmpty()){
		return;
	}

	// A document stored without a name shows the one its type derives from the content; opening does not write it
	QByteArray typeId;
	QString documentName;
	QUrl documentUrl;
	bool isDirty = false;
	{
		QMutexLocker locker(&this->m_mutex);
		auto documentIt = this->m_documents.find(documentId);
		if (documentIt == this->m_documents.end()){
			return;
		}

		WorkingDocument& document = documentIt.value();
		if (!document.name.isEmpty() || !document.objectPtr.IsValid() || !this->HasDocumentNameProvider(document.typeId)){
			return;
		}

		documentName = this->GetDefaultDocumentName(UrlToObjectId(document.url), document);
		if (documentName.isEmpty()){
			return;
		}

		document.name = documentName;
		typeId = document.typeId;
		documentUrl = document.url;
		isDirty = document.isDirty;
		userIdsToNotify = this->GetDocumentUserIds(documentId);
	}

	for (const QByteArray& notifyUserId : userIdsToNotify){
		for (IDocumentServiceEventHandler* handlerPtr : this->GetDocumentServiceEventHandlers()){
			if (handlerPtr != nullptr){
				CDocumentRenamedEvent event(notifyUserId, documentId, typeId, documentName, documentUrl, isDirty);
				handlerPtr->ProcessEvent(&event);
			}
		}
	}
}


template<class Base>
inline void TCollectionDocumentServiceWrap<Base>::DoCreateNewDocument(
			const QByteArray& taskId,
			const TaskParams& params)
{
	QMutexLocker locker(&this->m_mutex);
	istd::CChangeNotifier notifier(this);

	// Delegate to base class which handles UUID generation, events, and
	// spawning the async object-creation thread.  On completion the base
	// class calls CompleteTask.
	Base::DoCreateNewDocument(taskId, params);

	// If the base class already produced a document entry, record the
	// proposed source document ID for later use by SaveDocument.
	if (!params.proposedSourceDocumentId.isEmpty()){
		// Find the document that was just created — it will be the most
		// recently added entry for this user.
		for (const QByteArray& userDocumentId : this->GetUserDocumentIds(params.userId)){
			const WorkingDocument* docPtr = this->FindDocument(params.userId, userDocumentId);
			if (docPtr != nullptr && docPtr->typeId == params.documentTypeId && docPtr->isLoading){
				m_proposedSourceDocumentIds[userDocumentId] = params.proposedSourceDocumentId;
				break;
			}
		}
	}
}


template<class Base>
inline void TCollectionDocumentServiceWrap<Base>::DoOpenDocument(
			const QByteArray& taskId,
			const TaskParams& params)
{
	const QUrl& url = params.url;
	const QByteArray& userId = params.userId;

	if (url.scheme() != "collection" || !url.host().isEmpty()){
		this->CompleteTask(taskId, TaskResult{IDocumentService::OS_FAILED, QByteArray(), QStringLiteral("Invalid URL scheme")});
		return;
	}

	QString path = url.path();

	QStringList parts = path.split('/', Qt::SkipEmptyParts);
	if (parts.count() != 1){
		this->CompleteTask(taskId, TaskResult{IDocumentService::OS_FAILED, QByteArray(), QStringLiteral("Invalid URL path")});
		return;
	}

	QByteArray objectId = parts.first().toUtf8();
	QUrl documentUrl = this->ObjectIdToUrl(objectId);
	QByteArray documentId = QUuid::createUuid().toByteArray(QUuid::WithoutBraces);

	QByteArray sharedDocumentId;
	QByteArray sharedTypeId;
	QString sharedName;
	bool sharedIsDirty = false;
	bool sharedIsLoading = false;

	// Single-instance constraints (per user/object):
	// - opening in single-instance mode is forbidden when the object is already open;
	// - opening without single-instance mode is forbidden when the object is already
	//   open in single-instance mode.
	{
		QMutexLocker locker(&this->m_mutex);
		for (const QByteArray& userDocumentId : this->GetUserDocumentIds(userId)){
			const WorkingDocument* userDocPtr = this->FindDocument(userId, userDocumentId);
			if (userDocPtr == nullptr || this->UrlToObjectId(userDocPtr->url) != objectId){
				continue;
			}

			if (params.singleDocumentInstance){
				locker.unlock();
				this->CompleteTask(taskId, TaskResult{IDocumentService::OS_FAILED, QByteArray(), QStringLiteral("Document is already open")});
				return;
			}

			if (userDocPtr->singleDocumentInstance){
				locker.unlock();
				this->CompleteTask(taskId, TaskResult{IDocumentService::OS_FAILED, QByteArray(), QStringLiteral("Document is already open in single-instance mode")});
				return;
			}
		}

		// Single-copy mode: an object already opened by any user is represented by one document
		if (this->IsSingleCopyMode()){
			const UserDocumentPairList openedDocuments = this->FindDocumentsByObjectId(objectId);
			if (!openedDocuments.isEmpty()){
				sharedDocumentId = openedDocuments.first().second;

				const WorkingDocument& sharedDoc = this->InsertDocument(userId, sharedDocumentId);
				sharedTypeId = sharedDoc.typeId;
				sharedName = sharedDoc.name;
				sharedIsDirty = sharedDoc.isDirty;
				sharedIsLoading = sharedDoc.isLoading;
			}
		}

		if (sharedDocumentId.isEmpty()){
			WorkingDocument& reservedDoc = this->InsertDocument(userId, documentId);
			reservedDoc.url = documentUrl;
			reservedDoc.isLoading = true;
			reservedDoc.singleDocumentInstance = params.singleDocumentInstance;
		}
	}

	if (!sharedDocumentId.isEmpty()){
		istd::CChangeNotifier notifier(this);

		for (IDocumentServiceEventHandler* handlerPtr : this->GetDocumentServiceEventHandlers()){
			if (handlerPtr != nullptr){
				CDocumentOpenedEvent event(
							userId,
							sharedDocumentId,
							sharedTypeId,
							sharedName,
							documentUrl,
							sharedIsDirty);
				handlerPtr->ProcessEvent(&event);
			}
		}

		if (!sharedIsLoading){
			// Defer the notification to ensure the mutation response is sent
			// to the client before the subscription notification arrives.
			// Use invokeMethod (not QTimer::singleShot(..., qApp, ...)): singleShot
			// creates a QObject parented to qApp; from a worker thread that logs
			// "Cannot create children for a parent that is in a different thread".
			std::weak_ptr<std::atomic<bool>> deferredAliveGuard(this->m_isAlive);
			QMetaObject::invokeMethod(
						QCoreApplication::instance(),
						std::bind(
									&TCollectionDocumentServiceWrap<Base>::OnDeferredDocumentDataLoaded,
									this,
									deferredAliveGuard,
									userId,
									sharedDocumentId),
						Qt::QueuedConnection);
		}

		this->CompleteTask(taskId, TaskResult{IDocumentService::OS_OK, sharedDocumentId, QString()});
		return;
	}

	imtbase::IObjectCollection* collectionPtr = GetCollection();
	if (collectionPtr == nullptr){
		this->ClearReservedOpen(userId, documentId);
		this->CompleteTask(taskId, TaskResult{IDocumentService::OS_FAILED, QByteArray(), QStringLiteral("No collection available")});
		return;
	}

	QByteArray objectTypeId = collectionPtr->GetObjectTypeId(objectId);

	if (objectTypeId.isEmpty()){
		this->ClearReservedOpen(userId, documentId);
		this->CompleteTask(taskId, TaskResult{IDocumentService::OS_FAILED, QByteArray(), QStringLiteral("Unknown object type")});
		return;
	}

	idoc::IUndoManagerSharedPtr undoManagerPtr;
	undoManagerPtr.FromUnique(std::move(this->CreateUndoManager()));
	if (!undoManagerPtr.IsValid()){
		this->ClearReservedOpen(userId, documentId);
		this->CompleteTask(taskId, TaskResult{IDocumentService::OS_FAILED, QByteArray(), QStringLiteral("Failed to create undo manager")});
		return;
	}

	QString documentName = collectionPtr->GetElementInfo(objectId, imtbase::ICollectionInfo::EIT_NAME).toString();
	istd::CChangeNotifier notifier(this);

	{
		QMutexLocker locker(&this->m_mutex);
		WorkingDocument& doc = this->InsertDocument(userId, documentId);
		doc.typeId = objectTypeId;
		doc.url = documentUrl;
		doc.name = documentName;
		doc.undoManagerPtr = undoManagerPtr;
		doc.isDirty = false;
		doc.isLoading = true;
		doc.singleDocumentInstance = params.singleDocumentInstance;
	}

	for (IDocumentServiceEventHandler* handlerPtr : this->GetDocumentServiceEventHandlers()){
		if (handlerPtr != nullptr){
			CDocumentOpenedEvent event(
						userId,
						documentId,
						objectTypeId,
						documentName,
						documentUrl,
						false);
			handlerPtr->ProcessEvent(&event);
		}
	}

	// Opening is complete once the document is registered.  Loading continues
	// asynchronously and is reported through CDocumentDataLoadedEvent.
	this->CompleteTask(taskId, TaskResult{IDocumentService::OS_OK, documentId, QString()});

	// Load document data asynchronously in a separate thread
	QThread* thread = new QThread();
	QObject* worker = new QObject();
	worker->moveToThread(thread);
	{
		QMutexLocker locker(&this->m_workerThreadsMutex);
		this->m_workerThreads.append(QPointer<QThread>(thread));
	}

	std::weak_ptr<std::atomic<bool>> aliveGuard(this->m_isAlive);
	QObject::connect(
		thread,
		&QThread::started,
		worker,
		[this, aliveGuard, objectId, userId, documentId, worker](){
			auto isAlive = aliveGuard.lock();
			if (!isAlive || !isAlive->load()){
				worker->deleteLater();
				return;
			}

			this->OnOpenDocumentThreadStarted(
						aliveGuard,
						objectId,
						userId,
						documentId,
						worker);
		});

	// Initialize observers and fire events in the main thread after background work completes
	QObject::connect(
		thread,
		&QThread::finished,
		QCoreApplication::instance(),
		[this, aliveGuard, objectId, userId, documentId](){
			auto isAlive = aliveGuard.lock();
			if (!isAlive || !isAlive->load()){
				return;
			}

			this->OnOpenDocumentThreadFinished(
						aliveGuard,
						objectId,
						userId,
						documentId);
		});

	QObject::connect(worker, &QObject::destroyed, thread, &QThread::quit, Qt::DirectConnection);
	QObject::connect(thread, &QThread::finished, thread, &QObject::deleteLater);

	thread->start();
}


template<class Base>
inline typename TCollectionDocumentServiceWrap<Base>::OperationStatus
TCollectionDocumentServiceWrap<Base>::SetDocumentName(
			const QByteArray& userId,
			const QByteArray& documentId,
			const QString& documentName)
{
	QByteArray objectId;
	QByteArray typeId;
	bool isDirty = false;

	{
		QMutexLocker locker(&this->m_mutex);

		OperationStatus validationStatus;
		if (!this->ValidateInputParams(userId, documentId, validationStatus)){
			return validationStatus;
		}

		WorkingDocument* workingDocumentPtr = &this->m_documents[documentId];

		if (workingDocumentPtr->name == documentName){
			return IDocumentService::OS_OK;
		}

		objectId = this->UrlToObjectId(workingDocumentPtr->url);
		typeId = workingDocumentPtr->typeId;
		isDirty = workingDocumentPtr->isDirty;
	}

	if (!objectId.isEmpty()){
		imtbase::IObjectCollection* collectionPtr = GetCollection();
		if (collectionPtr == nullptr){
			return IDocumentService::OS_FAILED;
		}

		if (!collectionPtr->SetElementName(objectId, documentName)){
			return IDocumentService::OS_FAILED;
		}
	}

	istd::CChangeNotifier notifier(this);

	QUrl documentUrl;
	QByteArrayList userIds;
	{
		QMutexLocker locker(&this->m_mutex);
		WorkingDocument* workingDocumentPtr = this->FindDocument(userId, documentId);
		if (workingDocumentPtr == nullptr){
			return IDocumentService::OS_INVALID_DOCUMENT_ID;
		}

		workingDocumentPtr->name = documentName;
		documentUrl = workingDocumentPtr->url;
		userIds = this->GetDocumentUserIds(documentId);
	}

	// Notify every user holding the document
	for (const QByteArray& documentUserId : userIds){
		for (IDocumentServiceEventHandler* handlerPtr : this->GetDocumentServiceEventHandlers()){
			if (handlerPtr != nullptr){
				CDocumentRenamedEvent event(
							documentUserId,
							documentId,
							typeId,
							documentName,
							documentUrl,
							isDirty);
				handlerPtr->ProcessEvent(&event);
			}
		}
	}

	return IDocumentService::OS_OK;
}


template<class Base>
inline void TCollectionDocumentServiceWrap<Base>::DoSaveDocument(
			const QByteArray& taskId,
			const TaskParams& params)
{
	const QByteArray& userId = params.userId;
	const QByteArray& documentId = params.documentId;
	const QString& documentName = params.documentName;

	imtbase::IObjectCollection* collectionPtr = GetCollection();
	if (collectionPtr == nullptr){
		this->CompleteTask(taskId, TaskResult{IDocumentService::OS_FAILED, documentId, QStringLiteral("No collection available")});
		return;
	}

	QMutexLocker locker(&this->m_mutex);

	OperationStatus validationStatus = IDocumentService::OS_OK;
	if (!this->ValidateInputParams(userId, documentId, validationStatus)){
		this->CompleteTask(taskId, TaskResult{validationStatus, documentId, QString()});
		return;
	}

	WorkingDocument* workingDocumentPtr = &this->m_documents[documentId];

	if (workingDocumentPtr->isLoading){
		this->CompleteTask(taskId, TaskResult{IDocumentService::OS_FAILED, documentId, QStringLiteral("Document is still loading")});
		return;
	}

	istd::IChangeableSharedPtr documentSnapshotPtr = this->CreateObject(workingDocumentPtr->typeId);
	if (!documentSnapshotPtr.IsValid()){
		this->CompleteTask(taskId, TaskResult{IDocumentService::OS_FAILED, documentId, QStringLiteral("Failed to create snapshot")});
		return;
	}

	if (!documentSnapshotPtr->CopyFrom(*workingDocumentPtr->objectPtr)){
		this->CompleteTask(taskId, TaskResult{IDocumentService::OS_FAILED, documentId, QStringLiteral("Failed to copy document data")});
		return;
	}

	WorkingDocument workingDocumentSnapshot = *workingDocumentPtr;
	workingDocumentSnapshot.objectPtr = documentSnapshotPtr;

	const QByteArray currentObjectId = this->UrlToObjectId(workingDocumentPtr->url);

	QString validationMessage;
	if (!this->ValidateDocumentData(currentObjectId, workingDocumentSnapshot, validationStatus, &validationMessage, params.operationContextPtr)){
		QString msg = validationMessage.isEmpty() ? this->GetInvalidDocumentMessage() : validationMessage;
		this->CompleteTask(taskId, TaskResult{validationStatus, documentId, msg});
		return;
	}

	istd::CChangeGroup changeGroup(collectionPtr);

	QString resultDocumentName = documentName;
	if (!currentObjectId.isEmpty()){
		// Create copy of the object
		if (!resultDocumentName.isEmpty() && workingDocumentPtr->name != resultDocumentName){
			QByteArray newObjectId = collectionPtr->InsertNewObject(
						workingDocumentPtr->typeId,
						resultDocumentName,
						"",
						documentSnapshotPtr.GetPtr(),
						QByteArray(),
						nullptr,
						nullptr,
						params.operationContextPtr);

			if (newObjectId.isEmpty()){
				this->CompleteTask(taskId, TaskResult{IDocumentService::OS_FAILED, documentId, QStringLiteral("Failed to insert copy")});
				return;
			}

			if (this->HasDocumentNameProvider(workingDocumentPtr->typeId)){
				resultDocumentName = this->GetDefaultDocumentName(UrlToObjectId(workingDocumentPtr->url), *workingDocumentPtr);
				collectionPtr->SetElementName(newObjectId, resultDocumentName);
			}

			// Prepare notification with expected new state
			QByteArray savedTypeId = workingDocumentPtr->typeId;
			QByteArrayList userIds;

			workingDocumentPtr = this->FindDocument(userId, documentId);
			if (workingDocumentPtr != nullptr){
				istd::CChangeNotifier notifier(this);

				// The document is now backed by the new object for every user holding it,
				// so GetOpenedDocumentList() must report its URL
				workingDocumentPtr->url = this->ObjectIdToUrl(newObjectId);
				workingDocumentPtr->name = resultDocumentName;
				workingDocumentPtr->isDirty = false;
				workingDocumentPtr->undoManagerPtr->StoreDocumentState();

				userIds = this->GetDocumentUserIds(documentId);
			}

			locker.unlock();

			for (const QByteArray& documentUserId : userIds){
				for (IDocumentServiceEventHandler* handlerPtr : this->GetDocumentServiceEventHandlers()){
					if (handlerPtr != nullptr){
						CDocumentSavedAsEvent event(
									documentUserId,
									documentId,
									savedTypeId,
									resultDocumentName,
									this->ObjectIdToUrl(newObjectId),
									false);
						handlerPtr->ProcessEvent(&event);
					}
				}
			}

			this->CompleteTask(taskId, TaskResult{IDocumentService::OS_OK, documentId, QString()});
			return;
		}

		// Update object
		bool res = collectionPtr->SetObjectData(currentObjectId, *documentSnapshotPtr, istd::IChangeable::CM_WITHOUT_REFS, params.operationContextPtr);

		if (res){
			QString updatedName;
			if (this->HasDocumentNameProvider(workingDocumentPtr->typeId)){
				updatedName = this->GetDefaultDocumentName(UrlToObjectId(workingDocumentPtr->url), *workingDocumentPtr);
				collectionPtr->SetElementName(currentObjectId, updatedName);
			}

			workingDocumentPtr->undoManagerPtr->StoreDocumentState();

			istd::CChangeNotifier notifier(this);

			if (!updatedName.isEmpty()){
				workingDocumentPtr->name = updatedName;
			}
			workingDocumentPtr->isDirty = false;

			QByteArray savedTypeId = workingDocumentPtr->typeId;
			QUrl savedUrl = workingDocumentPtr->url;
			QString newName = workingDocumentPtr->name;
			QByteArrayList userIds = this->GetDocumentUserIds(documentId);

			locker.unlock();

			// Notify every user holding the document
			for (const QByteArray& documentUserId : userIds){
				for (IDocumentServiceEventHandler* handlerPtr : this->GetDocumentServiceEventHandlers()){
					if (handlerPtr != nullptr){
						CDocumentSavedEvent event(
									documentUserId,
									documentId,
									savedTypeId,
									newName,
									savedUrl,
									false);
						handlerPtr->ProcessEvent(&event);
					}
				}
			}

			locker.relock();
		}

		this->CompleteTask(taskId, TaskResult{res ? IDocumentService::OS_OK : IDocumentService::OS_FAILED, documentId, res ? QString() : QStringLiteral("Failed to update object data")});
		return;
	}

	// Create new object
	QByteArray proposedElementId;
	{
		auto it = m_proposedSourceDocumentIds.find(documentId);
		if (it != m_proposedSourceDocumentIds.end()){
			proposedElementId = it.value();
			m_proposedSourceDocumentIds.erase(it);
		}
	}

	const QByteArray savedObjectId = collectionPtr->InsertNewObject(
				workingDocumentPtr->typeId,
				resultDocumentName,
				"",
				documentSnapshotPtr.GetPtr(),
				proposedElementId,
				nullptr,
				nullptr,
				params.operationContextPtr);

	if (!savedObjectId.isEmpty()){
		// A new document had no URL; it is now backed by the inserted object
		workingDocumentPtr->url = this->ObjectIdToUrl(savedObjectId);
	}

	if (this->HasDocumentNameProvider(workingDocumentPtr->typeId)){
		resultDocumentName = this->GetDefaultDocumentName(UrlToObjectId(workingDocumentPtr->url), *workingDocumentPtr);
		collectionPtr->SetElementName(savedObjectId, resultDocumentName);
	}

	if (!savedObjectId.isEmpty()){
		QByteArray savedTypeId = workingDocumentPtr->typeId;

		workingDocumentPtr = this->FindDocument(userId, documentId);
		if (workingDocumentPtr != nullptr){
			istd::CChangeNotifier notifier(this);

			workingDocumentPtr->name = resultDocumentName;
			workingDocumentPtr->isDirty = false;
			workingDocumentPtr->undoManagerPtr->StoreDocumentState();
		}

		locker.unlock();

		for (IDocumentServiceEventHandler* handlerPtr : this->GetDocumentServiceEventHandlers()){
			if (handlerPtr != nullptr){
				CDocumentSavedEvent event(
							userId,
							documentId,
							savedTypeId,
							resultDocumentName,
							this->ObjectIdToUrl(savedObjectId),
							false);
				handlerPtr->ProcessEvent(&event);
			}
		}

		locker.relock();
	}

	OperationStatus saveStatus = savedObjectId.isEmpty() ? IDocumentService::OS_FAILED : IDocumentService::OS_OK;
	this->CompleteTask(taskId, TaskResult{saveStatus, documentId, saveStatus == IDocumentService::OS_OK ? QString() : QStringLiteral("Failed to insert new object")});
}


template<class Base>
inline void TCollectionDocumentServiceWrap<Base>::DoCloseDocument(
			const QByteArray& taskId,
			const TaskParams& params)
{
	QMutexLocker locker(&this->m_mutex);

	m_proposedSourceDocumentIds.remove(params.documentId);

	OperationStatus status = this->CloseDocumentInternal(params.userId, params.documentId);
	QString message;
	if (status != IDocumentService::OS_OK){
		switch (status){
			case IDocumentService::OS_INVALID_USER_ID:
				message = QStringLiteral("Invalid user ID");
				break;
			case IDocumentService::OS_INVALID_DOCUMENT_ID:
				message = QStringLiteral("Invalid document ID");
				break;
			default:
				message = QStringLiteral("Close failed");
				break;
		}
	}
	this->CompleteTask(taskId, TaskResult{status, params.documentId, message});
}


typedef TCollectionDocumentServiceWrap<CDocumentServiceBase> CCollectionDocumentServiceBase;


} // namespace imtdoc
