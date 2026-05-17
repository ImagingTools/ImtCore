// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// Qt includes
#include <QtCore/QCoreApplication>
#include <QtCore/QMap>
#include <QtCore/QTimer>
#include <QtCore/QUuid>
#include <QtCore/QThread>

// ACF includes
#include <istd/CChangeGroup.h>

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


template<class Base>
class TCollectionDocumentServiceWrap : public Base
{
public:
	typedef typename Base::WorkingDocument WorkingDocument;
	typedef typename Base::WorkingDocumentList WorkingDocumentList;
	typedef typename Base::SharedDocumentData SharedDocumentData;
	typedef typename Base::UserDocumentPair UserDocumentPair;
	typedef typename Base::UserDocumentPairList UserDocumentPairList;

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
	virtual imtbase::IObjectCollection* GetCollection() const = 0;

private:
	QMap<QByteArray, QByteArray> m_proposedSourceDocumentIds;
};


// ─── Inline implementations ────────────────────────────────────────────────


template<class Base>
inline void TCollectionDocumentServiceWrap<Base>::DoCreateNewDocument(
			const QByteArray& taskId,
			const TaskParams& params)
{
	QMutexLocker locker(&this->m_mutex);

	// Delegate to base class which handles UUID generation, events, and
	// spawning the async object-creation thread.  On completion the base
	// class calls CompleteTask.
	Base::DoCreateNewDocument(taskId, params);

	// If the base class already produced a document entry, record the
	// proposed source document ID for later use by SaveDocument.
	if (!params.proposedSourceDocumentId.isEmpty()){
		// Find the document that was just created — it will be the most
		// recently added entry for this user.
		const WorkingDocumentList& docs = this->m_userDocuments[params.userId];
		for (auto it = docs.constBegin(); it != docs.constEnd(); ++it){
			if (it.value().typeId == params.documentTypeId && it.value().isLoading){
				m_proposedSourceDocumentIds[it.key()] = params.proposedSourceDocumentId;
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

	imtbase::IObjectCollection* collectionPtr = GetCollection();
	if (collectionPtr == nullptr){
		this->CompleteTask(taskId, TaskResult{IDocumentService::OS_FAILED, QByteArray(), QStringLiteral("No collection available")});
		return;
	}

	QByteArray objectTypeId = collectionPtr->GetObjectTypeId(objectId);

	if (objectTypeId.isEmpty()){
		this->CompleteTask(taskId, TaskResult{IDocumentService::OS_FAILED, QByteArray(), QStringLiteral("Unknown object type")});
		return;
	}

	// Single-copy mode: check if this object is already opened by any user
	if (this->IsSingleCopyMode()){
		bool isSharedDocument = false;
		QByteArray sharedTypeId;
		QString sharedName;
		bool sharedIsLoading = false;
		bool docIsDirty = false;

		{
			QMutexLocker locker(&this->m_mutex);
			if (this->m_sharedDocuments.contains(objectId)){
				isSharedDocument = true;
				SharedDocumentData& shared = this->m_sharedDocuments[objectId];
				sharedTypeId = shared.typeId;
				sharedName = shared.name;
				sharedIsLoading = shared.isLoading;
				docIsDirty = shared.undoManagerPtr.IsValid()
					? (shared.undoManagerPtr->GetDocumentChangeFlag() != idoc::IDocumentStateComparator::DCF_EQUAL)
					: false;
			}
		}

		if (isSharedDocument){
			QByteArray documentId = QUuid::createUuid().toByteArray(QUuid::WithoutBraces);

			{
				QMutexLocker locker(&this->m_mutex);
				if (this->m_sharedDocuments.contains(objectId)){
					SharedDocumentData& shared = this->m_sharedDocuments[objectId];
					shared.refCount++;

					WorkingDocument& doc = this->m_userDocuments[userId][documentId];
					doc.objectId = objectId;
					doc.typeId = shared.typeId;
					doc.url = url;
					doc.name = shared.name;
					doc.objectPtr = shared.objectPtr;
					doc.undoManagerPtr = shared.undoManagerPtr;
					doc.isDirty = docIsDirty;
					doc.isLoading = shared.isLoading;
					doc.undoManagerModelId = -1;
				}
			}

			for (IDocumentServiceEventHandler* handlerPtr : this->GetDocumentServiceEventHandlers()){
				if (handlerPtr != nullptr){
					CDocumentOpenedEvent event(
								userId,
								documentId,
								sharedTypeId,
								sharedName,
								this->ObjectIdToUrl(objectId),
								docIsDirty);
					handlerPtr->ProcessEvent(&event);
				}
			}

			if (!sharedIsLoading){
				// Defer the notification to ensure the mutation response is sent
				// to the client before the subscription notification arrives.
				QByteArray deferredUserId = userId;
				QByteArray deferredDocumentId = documentId;
				std::weak_ptr<std::atomic<bool>> deferredAliveGuard(this->m_isAlive);
				QTimer::singleShot(0, QCoreApplication::instance(), [this, deferredAliveGuard, deferredUserId, deferredDocumentId](){
					auto isAlive = deferredAliveGuard.lock();
					if (!isAlive || !isAlive->load()){
						return;
					}
					this->OnDocumentDataLoaded(deferredUserId, deferredDocumentId);
				});
			}

			this->CompleteTask(taskId, TaskResult{IDocumentService::OS_OK, documentId, QString()});
			return;
		}
	}

	idoc::IUndoManagerSharedPtr undoManagerPtr = this->CreateUndoManager();
	if (!undoManagerPtr.IsValid()){
		this->CompleteTask(taskId, TaskResult{IDocumentService::OS_FAILED, QByteArray(), QStringLiteral("Failed to create undo manager")});
		return;
	}

	QByteArray documentId = QUuid::createUuid().toByteArray(QUuid::WithoutBraces);

	QString documentName = collectionPtr->GetElementInfo(objectId, imtbase::ICollectionInfo::EIT_NAME).toString();

	{
		QMutexLocker locker(&this->m_mutex);
		WorkingDocument& doc = this->m_userDocuments[userId][documentId];
		doc.objectId = objectId;
		doc.typeId = objectTypeId;
		doc.url = url;
		doc.name = documentName;
		doc.undoManagerPtr = undoManagerPtr;
		doc.isDirty = false;
		doc.isLoading = true;

		if (this->IsSingleCopyMode()){
			SharedDocumentData& shared = this->m_sharedDocuments[objectId];
			shared.typeId = objectTypeId;
			shared.name = documentName;
			shared.undoManagerPtr = undoManagerPtr;
			shared.refCount = 1;
			shared.isLoading = true;
		}
	}

	for (IDocumentServiceEventHandler* handlerPtr : this->GetDocumentServiceEventHandlers()){
		if (handlerPtr != nullptr){
			CDocumentOpenedEvent event(
						userId,
						documentId,
						objectTypeId,
						documentName,
						this->ObjectIdToUrl(objectId),
						false);
			handlerPtr->ProcessEvent(&event);
		}
	}

	// Load document data asynchronously in a separate thread
	QThread* thread = new QThread();
	QObject* worker = new QObject();
	worker->moveToThread(thread);

	bool singleCopyMode = this->IsSingleCopyMode();
	std::weak_ptr<std::atomic<bool>> aliveGuard(this->m_isAlive);
	QObject::connect(thread, &QThread::started, worker, [this, aliveGuard, singleCopyMode, objectId, userId, documentId, taskId, worker](){
		auto isAlive = aliveGuard.lock();
		if (!isAlive || !isAlive->load()){
			this->CompleteTask(taskId, TaskResult{IDocumentService::OS_FAILED, QByteArray(), QStringLiteral("Service destroyed")});
			worker->deleteLater();
			return;
		}

		imtbase::IObjectCollection* collPtr = GetCollection();
		if (collPtr == nullptr){
			this->CompleteTask(taskId, TaskResult{IDocumentService::OS_FAILED, QByteArray(), QStringLiteral("No collection available")});
			worker->deleteLater();
			return;
		}

		imtbase::IObjectCollection::DataPtr dataPtr;
		bool success = collPtr->GetObjectData(objectId, dataPtr);

		isAlive = aliveGuard.lock();
		if (!isAlive || !isAlive->load()){
			this->CompleteTask(taskId, TaskResult{IDocumentService::OS_FAILED, QByteArray(), QStringLiteral("Service destroyed")});
			worker->deleteLater();
			return;
		}

		bool loadSuccess = false;
		{
			QMutexLocker locker(&this->m_mutex);

			if (singleCopyMode){
				if (success && dataPtr.IsValid()){
					if (this->m_sharedDocuments.contains(objectId)){
						this->m_sharedDocuments[objectId].objectPtr = dataPtr;
					}

					UserDocumentPairList docs = this->FindDocumentsByObjectId(objectId);
					for (const UserDocumentPair& pair : docs){
						WorkingDocument* dp = this->FindDocument(pair.first, pair.second);
						if (dp != nullptr){
							dp->objectPtr = dataPtr;
						}
					}
					loadSuccess = true;
				}
				else {
					if (this->m_sharedDocuments.contains(objectId)){
						this->m_sharedDocuments[objectId].isLoading = false;
					}

					UserDocumentPairList docs = this->FindDocumentsByObjectId(objectId);
					for (const UserDocumentPair& pair : docs){
						WorkingDocument* dp = this->FindDocument(pair.first, pair.second);
						if (dp != nullptr){
							dp->isLoading = false;
							this->CloseDocumentInternal(pair.first, pair.second);
						}
					}
				}
			}
			else {
				WorkingDocument* docPtr = this->FindDocument(userId, documentId);

				if (docPtr != nullptr && success && dataPtr.IsValid()){
					docPtr->objectPtr = dataPtr;
					loadSuccess = true;
				}
				else if (docPtr != nullptr){
					docPtr->isLoading = false;
					this->CloseDocumentInternal(userId, documentId);
				}
			}
		}

		if (!loadSuccess){
			this->CompleteTask(taskId, TaskResult{IDocumentService::OS_FAILED, QByteArray(), QStringLiteral("Failed to load document data")});
		}

		worker->deleteLater();
	});

	// Initialize observers and fire events in the main thread after background work completes
	QObject::connect(thread, &QThread::finished, QCoreApplication::instance(), [this, aliveGuard, singleCopyMode, objectId, userId, documentId, taskId](){
		auto isAlive = aliveGuard.lock();
		if (!isAlive || !isAlive->load()){
			return;
		}

		UserDocumentPairList docsToNotify;

		{
			QMutexLocker locker(&this->m_mutex);

			if (singleCopyMode){
				if (!this->m_sharedDocuments.contains(objectId)){
					return;
				}

				SharedDocumentData& shared = this->m_sharedDocuments[objectId];
				if (!shared.objectPtr.IsValid() || !shared.isLoading){
					return;
				}

				shared.isLoading = false;

				bool observersInitialized = false;
				UserDocumentPairList docs = this->FindDocumentsByObjectId(objectId);
				for (const UserDocumentPair& pair : docs){
					WorkingDocument* dp = this->FindDocument(pair.first, pair.second);
					if (dp != nullptr && dp->isLoading){
						if (!observersInitialized){
							this->InitializeDocumentObservers(*dp, pair.first);
							shared.undoManagerModelId = dp->undoManagerModelId;
							observersInitialized = true;
						}

						docsToNotify.append(pair);
					}
				}
			}
			else {
				WorkingDocument* docPtr = this->FindDocument(userId, documentId);

				if (docPtr == nullptr || !docPtr->objectPtr.IsValid() || !docPtr->isLoading){
					return;
				}

				this->InitializeDocumentObservers(*docPtr, userId);
				docsToNotify.append(qMakePair(userId, documentId));
			}
		}

		// OnDocumentDataLoaded sets isLoading=false
		for (const UserDocumentPair& pair : docsToNotify){
			this->OnDocumentDataLoaded(pair.first, pair.second);
		}

		this->CompleteTask(taskId, TaskResult{IDocumentService::OS_OK, documentId, QString()});
	});

	QObject::connect(worker, &QObject::destroyed, thread, &QThread::quit);
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

		WorkingDocument* workingDocumentPtr = &this->m_userDocuments[userId][documentId];

		if (workingDocumentPtr->name == documentName){
			return IDocumentService::OS_OK;
		}

		objectId = workingDocumentPtr->objectId;
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

	// Shared docs: rename and notify each user sharing this object
	if (this->IsSingleCopyMode() && !objectId.isEmpty()){
		UserDocumentPairList sharedDocs;
		{
			QMutexLocker locker(&this->m_mutex);
			if (this->m_sharedDocuments.contains(objectId)){
				this->m_sharedDocuments[objectId].name = documentName;
			}
			sharedDocs = this->FindDocumentsByObjectId(objectId);
		}

		for (const UserDocumentPair& pair : sharedDocs){
			if (pair.first == userId && pair.second == documentId){
				continue;
			}

			QByteArray dpTypeId;
			QByteArray dpObjectId;
			bool dpIsDirty = false;
			bool dpExists = false;

			{
				QMutexLocker locker(&this->m_mutex);
				WorkingDocument* dp = this->FindDocument(pair.first, pair.second);
				if (dp != nullptr){
					dpExists = true;
					dpTypeId = dp->typeId;
					dpObjectId = dp->objectId;
					dpIsDirty = dp->isDirty;
				}
			}

			if (!dpExists){
				continue;
			}

			{
				QMutexLocker locker(&this->m_mutex);
				WorkingDocument* dp = this->FindDocument(pair.first, pair.second);
				if (dp != nullptr){
					dp->name = documentName;
				}
			}

			for (IDocumentServiceEventHandler* handlerPtr : this->GetDocumentServiceEventHandlers()){
				if (handlerPtr != nullptr){
					CDocumentRenamedEvent event(
								pair.first,
								pair.second,
								dpTypeId,
								documentName,
								this->ObjectIdToUrl(dpObjectId),
								dpIsDirty);
					handlerPtr->ProcessEvent(&event);
				}
			}
		}
	}

	// Primary doc: rename and notify
	{
		QMutexLocker locker(&this->m_mutex);
		WorkingDocument* workingDocumentPtr = this->FindDocument(userId, documentId);
		if (workingDocumentPtr != nullptr){
			workingDocumentPtr->name = documentName;
		}
	}

	for (IDocumentServiceEventHandler* handlerPtr : this->GetDocumentServiceEventHandlers()){
		if (handlerPtr != nullptr){
			CDocumentRenamedEvent event(
						userId,
						documentId,
						typeId,
						documentName,
						this->ObjectIdToUrl(objectId),
						isDirty);
			handlerPtr->ProcessEvent(&event);
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

	WorkingDocument* workingDocumentPtr = &this->m_userDocuments[userId][documentId];

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

	QString validationMessage;
	if (!this->ValidateDocumentData(workingDocumentSnapshot, validationStatus, &validationMessage)){
		QString msg = validationMessage.isEmpty() ? this->GetInvalidDocumentMessage() : validationMessage;
		this->CompleteTask(taskId, TaskResult{validationStatus, documentId, msg});
		return;
	}

	istd::CChangeGroup changeGroup(collectionPtr);

	QString resultDocumentName = documentName;
	if (!workingDocumentPtr->objectId.isEmpty()){
		// Create copy of the object
		if (!resultDocumentName.isEmpty() && workingDocumentPtr->name != resultDocumentName){
			QByteArray oldObjectId = workingDocumentPtr->objectId;

			QByteArray newObjectId = collectionPtr->InsertNewObject(
				workingDocumentPtr->typeId, resultDocumentName, "", documentSnapshotPtr.GetPtr());

			if (newObjectId.isEmpty()){
				this->CompleteTask(taskId, TaskResult{IDocumentService::OS_FAILED, documentId, QStringLiteral("Failed to insert copy")});
				return;
			}

			if (this->HasDocumentNameProvider(workingDocumentPtr->typeId)){
				resultDocumentName = this->GetDefaultDocumentName(*workingDocumentPtr);
				collectionPtr->SetElementName(newObjectId, resultDocumentName);
			}

			// In single-copy mode, detach from shared document before changing objectId
			if (this->IsSingleCopyMode() && this->m_sharedDocuments.contains(oldObjectId)){
				SharedDocumentData& shared = this->m_sharedDocuments[oldObjectId];
				shared.refCount--;
				bool isLastUser = (shared.refCount <= 0);
				if (isLastUser){
					imod::IModel* undoModelPtr = dynamic_cast<imod::IModel*>(shared.undoManagerPtr.GetPtr());
					if (undoModelPtr != nullptr && shared.undoManagerModelId >= 0){
						this->m_undoManagerObserver.UnregisterModel(shared.undoManagerModelId);
					}
					this->m_sharedDocuments.remove(oldObjectId);
				}

				// Create new private objectPtr and undoManagerPtr for this user
				istd::IChangeableSharedPtr newObjectPtr = this->CreateObject(workingDocumentPtr->typeId);
				if (newObjectPtr.IsValid()){
					newObjectPtr->CopyFrom(*documentSnapshotPtr);
				}
				idoc::IUndoManagerSharedPtr newUndoManagerPtr = this->CreateUndoManager();

				workingDocumentPtr->objectPtr = newObjectPtr;
				workingDocumentPtr->undoManagerPtr = newUndoManagerPtr;

				this->InitializeDocumentObservers(*workingDocumentPtr, userId);
			}

			// Prepare notification with expected new state
			QByteArray savedTypeId = workingDocumentPtr->typeId;

			locker.unlock();

			{
				QMutexLocker locker(&this->m_mutex);
				workingDocumentPtr = this->FindDocument(userId, documentId);
				if (workingDocumentPtr != nullptr){
					workingDocumentPtr->objectId = newObjectId;
					workingDocumentPtr->name = resultDocumentName;
					workingDocumentPtr->isDirty = false;
					workingDocumentPtr->undoManagerPtr->StoreDocumentState();
				}
			}

			for (IDocumentServiceEventHandler* handlerPtr : this->GetDocumentServiceEventHandlers()){
				if (handlerPtr != nullptr){
					CDocumentSavedAsEvent event(
								userId,
								documentId,
								savedTypeId,
								resultDocumentName,
								this->ObjectIdToUrl(newObjectId),
								false);
					handlerPtr->ProcessEvent(&event);
				}
			}

			this->CompleteTask(taskId, TaskResult{IDocumentService::OS_OK, documentId, QString()});
			return;
		}

		// Update object
		bool res = collectionPtr->SetObjectData(workingDocumentPtr->objectId, *documentSnapshotPtr);

		if (res){
			QString updatedName;
			if (this->HasDocumentNameProvider(workingDocumentPtr->typeId)){
				updatedName = this->GetDefaultDocumentName(*workingDocumentPtr);
				collectionPtr->SetElementName(workingDocumentPtr->objectId, updatedName);
			}

			workingDocumentPtr->undoManagerPtr->StoreDocumentState();

			if (this->IsSingleCopyMode() && !workingDocumentPtr->objectId.isEmpty()){
				// Update all users sharing this document
				QByteArray sharedObjectId = workingDocumentPtr->objectId;

				if (this->m_sharedDocuments.contains(sharedObjectId) && !updatedName.isEmpty()){
					this->m_sharedDocuments[sharedObjectId].name = updatedName;
				}

				UserDocumentPairList docs = this->FindDocumentsByObjectId(sharedObjectId);
				for (const UserDocumentPair& pair : docs){
					WorkingDocument* dp = this->FindDocument(pair.first, pair.second);
					if (dp == nullptr){
						continue;
					}

					QByteArray dpTypeId = dp->typeId;
					QByteArray dpObjectId = dp->objectId;
					QString dpNewName = !updatedName.isEmpty() ? updatedName : dp->name;

					locker.unlock();

					{
						QMutexLocker locker(&this->m_mutex);
						dp = this->FindDocument(pair.first, pair.second);
						if (dp != nullptr){
							dp->isDirty = false;
							if (!updatedName.isEmpty()){
								dp->name = updatedName;
							}
						}
					}

					for (IDocumentServiceEventHandler* handlerPtr : this->GetDocumentServiceEventHandlers()){
						if (handlerPtr != nullptr){
							CDocumentSavedEvent event(
										pair.first,
										pair.second,
										dpTypeId,
										dpNewName,
										this->ObjectIdToUrl(dpObjectId),
										false);
							handlerPtr->ProcessEvent(&event);
						}
					}

					locker.relock();
				}
			}
			else {
				QByteArray savedTypeId = workingDocumentPtr->typeId;
				QByteArray savedObjectId = workingDocumentPtr->objectId;
				QString newName = !updatedName.isEmpty() ? updatedName : workingDocumentPtr->name;

				locker.unlock();

				{
					QMutexLocker locker(&this->m_mutex);
					workingDocumentPtr = this->FindDocument(userId, documentId);
					if (workingDocumentPtr != nullptr){
						if (!updatedName.isEmpty()){
							workingDocumentPtr->name = updatedName;
						}
						workingDocumentPtr->isDirty = false;
					}
				}

				for (IDocumentServiceEventHandler* handlerPtr : this->GetDocumentServiceEventHandlers()){
					if (handlerPtr != nullptr){
						CDocumentSavedEvent event(
									userId,
									documentId,
									savedTypeId,
									newName,
									this->ObjectIdToUrl(savedObjectId),
									false);
						handlerPtr->ProcessEvent(&event);
					}
				}

				locker.relock();
			}
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

	workingDocumentPtr->objectId =
		collectionPtr->InsertNewObject(workingDocumentPtr->typeId, resultDocumentName, "", documentSnapshotPtr.GetPtr(), proposedElementId);

	if (this->HasDocumentNameProvider(workingDocumentPtr->typeId)){
		resultDocumentName = this->GetDefaultDocumentName(*workingDocumentPtr);
		collectionPtr->SetElementName(workingDocumentPtr->objectId, resultDocumentName);
	}

	if (!workingDocumentPtr->objectId.isEmpty()){
		QByteArray savedTypeId = workingDocumentPtr->typeId;
		QByteArray savedObjectId = workingDocumentPtr->objectId;

		locker.unlock();

		{
			QMutexLocker locker(&this->m_mutex);
			workingDocumentPtr = this->FindDocument(userId, documentId);
			if (workingDocumentPtr != nullptr){
				workingDocumentPtr->name = resultDocumentName;
				workingDocumentPtr->isDirty = false;
				workingDocumentPtr->undoManagerPtr->StoreDocumentState();
			}
		}

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

	OperationStatus saveStatus = workingDocumentPtr->objectId.isEmpty() ? IDocumentService::OS_FAILED : IDocumentService::OS_OK;
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


// Backward-compatibility typedef
typedef TCollectionDocumentServiceWrap<CDocumentServiceBase> CCollectionDocumentServiceBase;


} // namespace imtdoc
