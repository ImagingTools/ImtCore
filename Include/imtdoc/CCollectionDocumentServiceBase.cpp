// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtdoc/CCollectionDocumentServiceBase.h>


// Qt includes
#include <QtCore/QCoreApplication>
#include <QtCore/QTimer>
#include <QtCore/QUuid>
#include <QtCore/QThread>

// ACF includes
#include <istd/CChangeNotifier.h>
#include <istd/CChangeGroup.h>

// ImtCore includes
#include <imtdoc/CDocumentClosedEvent.h>
#include <imtdoc/CDocumentOpenedEvent.h>
#include <imtdoc/CDocumentRenamedEvent.h>
#include <imtdoc/CDocumentSavedAsEvent.h>
#include <imtdoc/CDocumentSavedEvent.h>


namespace imtdoc
{


// reimplemented (imtdoc::CDocumentServiceBase) — task dispatch

void CCollectionDocumentServiceBase::DoCreateNewDocument(
	const QByteArray& taskId,
	const TaskParams& params)
{
	QMutexLocker locker(&m_mutex);

	// Delegate to base class which handles UUID generation, events, and
	// spawning the async object-creation thread.  On completion the base
	// class calls CompleteTask.
	CDocumentServiceBase::DoCreateNewDocument(taskId, params);

	// If the base class already produced a document entry, record the
	// proposed source document ID for later use by SaveDocument.
	if (!params.proposedSourceDocumentId.isEmpty()){
		// Find the document that was just created — it will be the most
		// recently added entry for this user.
		const WorkingDocumentList& docs = m_userDocuments[params.userId];
		for (auto it = docs.constBegin(); it != docs.constEnd(); ++it){
			if (it.value().typeId == params.documentTypeId && it.value().isLoading){
				m_proposedSourceDocumentIds[it.key()] = params.proposedSourceDocumentId;
				break;
			}
		}
	}
}


void CCollectionDocumentServiceBase::DoOpenDocument(
	const QByteArray& taskId,
	const TaskParams& params)
{
	const QUrl& url = params.url;
	const QByteArray& userId = params.userId;

	if (url.scheme() != "collection" || !url.host().isEmpty()){
		CompleteTask(taskId, TaskResult{OS_FAILED, QByteArray(), QStringLiteral("Invalid URL scheme")});
		return;
	}

	QString path = url.path();

	QStringList parts = path.split('/', Qt::SkipEmptyParts);
	if (parts.count() != 1){
		CompleteTask(taskId, TaskResult{OS_FAILED, QByteArray(), QStringLiteral("Invalid URL path")});
		return;
	}

	QByteArray objectId = parts.first().toUtf8();

	imtbase::IObjectCollection* collectionPtr = GetCollection();
	if (collectionPtr == nullptr) {
		CompleteTask(taskId, TaskResult{OS_FAILED, QByteArray(), QStringLiteral("No collection available")});
		return;
	}

	QByteArray objectTypeId = collectionPtr->GetObjectTypeId(objectId);

	if (objectTypeId.isEmpty()) {
		CompleteTask(taskId, TaskResult{OS_FAILED, QByteArray(), QStringLiteral("Unknown object type")});
		return;
	}

	// Single-copy mode: check if this object is already opened by any user
	if (IsSingleCopyMode()) {
		QMutexLocker locker(&m_mutex);
		if (m_sharedDocuments.contains(objectId)) {
			SharedDocumentData& shared = m_sharedDocuments[objectId];
			shared.refCount++;

			QByteArray documentId = QUuid::createUuid().toByteArray(QUuid::WithoutBraces);

			WorkingDocument& doc = m_userDocuments[userId][documentId];
			doc.objectId = objectId;
			doc.typeId = shared.typeId;
			doc.url = url;
			doc.name = shared.name;
			doc.objectPtr = shared.objectPtr;
			doc.undoManagerPtr = shared.undoManagerPtr;
			doc.isDirty = shared.undoManagerPtr.IsValid()
				? (shared.undoManagerPtr->GetDocumentChangeFlag() != idoc::IDocumentStateComparator::DCF_EQUAL)
				: false;
			doc.isLoading = shared.isLoading;
			doc.undoManagerModelId = -1;

			{
				DocumentOpenedInfo info;
				info.userId = userId;
				info.documentId = documentId;
				info.typeId = shared.typeId;
				info.url = url;
				info.name = shared.name;
				info.isDirty = doc.isDirty;
				info.isLoading = shared.isLoading;

				istd::IChangeable::ChangeSet changeSet(CF_DOCUMENT_OPENED);
				changeSet.SetChangeInfo(CN_DOCUMENT_OPENED, QVariant::fromValue(info));
				istd::CChangeNotifier notifier(this, &changeSet);
			}

			for (IDocumentServiceEventHandler* handlerPtr : GetDocumentServiceEventHandlers()){
				if (handlerPtr != nullptr){
					CDocumentOpenedEvent event(
						userId,
						documentId,
						shared.typeId,
						shared.name,
						ObjectIdToUrl(objectId),
						doc.isDirty);
					handlerPtr->ProcessEvent(&event);
				}
			}

			if (!shared.isLoading) {
				// Defer the notification to ensure the mutation response is sent
				// to the client before the subscription notification arrives.
				QByteArray deferredUserId = userId;
				QByteArray deferredDocumentId = documentId;
				std::weak_ptr<std::atomic<bool>> deferredAliveGuard(m_isAlive);
				QTimer::singleShot(0, QCoreApplication::instance(), [this, deferredAliveGuard, deferredUserId, deferredDocumentId]() {
					auto isAlive = deferredAliveGuard.lock();
					if (!isAlive || !isAlive->load()) {
						return;
					}
					OnDocumentDataLoaded(deferredUserId, deferredDocumentId);
				});
			}

			CompleteTask(taskId, TaskResult{OS_OK, documentId, QString()});
			return;
		}
	}

	idoc::IUndoManagerSharedPtr undoManagerPtr = CreateUndoManager();
	if (!undoManagerPtr.IsValid()){
		CompleteTask(taskId, TaskResult{OS_FAILED, QByteArray(), QStringLiteral("Failed to create undo manager")});
		return;
	}

	QByteArray documentId = QUuid::createUuid().toByteArray(QUuid::WithoutBraces);

	QString documentName = collectionPtr->GetElementInfo(objectId, imtbase::ICollectionInfo::EIT_NAME).toString();

	{
		QMutexLocker locker(&m_mutex);
		WorkingDocument& doc = m_userDocuments[userId][documentId];
		doc.objectId = objectId;
		doc.typeId = objectTypeId;
		doc.url = url;
		doc.name = documentName;
		doc.undoManagerPtr = undoManagerPtr;
		doc.isDirty = false;
		doc.isLoading = true;

		if (IsSingleCopyMode()) {
			SharedDocumentData& shared = m_sharedDocuments[objectId];
			shared.typeId = objectTypeId;
			shared.name = documentName;
			shared.undoManagerPtr = undoManagerPtr;
			shared.refCount = 1;
			shared.isLoading = true;
		}
	}

	{
		DocumentOpenedInfo info;
		info.userId = userId;
		info.documentId = documentId;
		info.typeId = objectTypeId;
		info.url = url;
		info.name = documentName;
		info.isDirty = false;
		info.isLoading = true;

		istd::IChangeable::ChangeSet changeSet(CF_DOCUMENT_OPENED);
		changeSet.SetChangeInfo(CN_DOCUMENT_OPENED, QVariant::fromValue(info));
		istd::CChangeNotifier notifier(this, &changeSet);
	}

	for (IDocumentServiceEventHandler* handlerPtr : GetDocumentServiceEventHandlers()){
		if (handlerPtr != nullptr){
			CDocumentOpenedEvent event(
				userId,
				documentId,
				objectTypeId,
				documentName,
				ObjectIdToUrl(objectId),
				false);
			handlerPtr->ProcessEvent(&event);
		}
	}

	// Load document data asynchronously in a separate thread
	QThread* thread = new QThread();
	QObject* worker = new QObject();
	worker->moveToThread(thread);

	bool singleCopyMode = IsSingleCopyMode();
	std::weak_ptr<std::atomic<bool>> aliveGuard(m_isAlive);
	QObject::connect(thread, &QThread::started, worker, [this, aliveGuard, singleCopyMode, objectId, userId, documentId, taskId, worker]() {
		auto isAlive = aliveGuard.lock();
		if (!isAlive || !isAlive->load()) {
			CompleteTask(taskId, TaskResult{OS_FAILED, QByteArray(), QStringLiteral("Service destroyed")});
			worker->deleteLater();
			return;
		}

		imtbase::IObjectCollection* collPtr = GetCollection();
		if (collPtr == nullptr) {
			CompleteTask(taskId, TaskResult{OS_FAILED, QByteArray(), QStringLiteral("No collection available")});
			worker->deleteLater();
			return;
		}

		imtbase::IObjectCollection::DataPtr dataPtr;
		bool success = collPtr->GetObjectData(objectId, dataPtr);

		isAlive = aliveGuard.lock();
		if (!isAlive || !isAlive->load()) {
			CompleteTask(taskId, TaskResult{OS_FAILED, QByteArray(), QStringLiteral("Service destroyed")});
			worker->deleteLater();
			return;
		}

		bool loadSuccess = false;
		{
			QMutexLocker locker(&m_mutex);

			if (singleCopyMode) {
				if (success && dataPtr.IsValid()) {
					if (m_sharedDocuments.contains(objectId)) {
						m_sharedDocuments[objectId].objectPtr = dataPtr;
					}

					UserDocumentPairList docs = FindDocumentsByObjectId(objectId);
					for (const UserDocumentPair& pair : docs) {
						WorkingDocument* dp = FindDocument(pair.first, pair.second);
						if (dp != nullptr) {
							dp->objectPtr = dataPtr;
						}
					}
					loadSuccess = true;
				}
				else {
					if (m_sharedDocuments.contains(objectId)) {
						m_sharedDocuments[objectId].isLoading = false;
					}

					UserDocumentPairList docs = FindDocumentsByObjectId(objectId);
					for (const UserDocumentPair& pair : docs) {
						WorkingDocument* dp = FindDocument(pair.first, pair.second);
						if (dp != nullptr) {
							dp->isLoading = false;
							CloseDocumentInternal(pair.first, pair.second);
						}
					}
				}
			}
			else {
				WorkingDocument* docPtr = FindDocument(userId, documentId);

				if (docPtr != nullptr && success && dataPtr.IsValid()) {
					docPtr->objectPtr = dataPtr;
					loadSuccess = true;
				}
				else if (docPtr != nullptr) {
					docPtr->isLoading = false;
					CloseDocumentInternal(userId, documentId);
				}
			}
		}

		if (!loadSuccess){
			CompleteTask(taskId, TaskResult{OS_FAILED, QByteArray(), QStringLiteral("Failed to load document data")});
		}

		worker->deleteLater();
	});

	// Initialize observers and fire events in the main thread after background work completes
	QObject::connect(thread, &QThread::finished, QCoreApplication::instance(), [this, aliveGuard, singleCopyMode, objectId, userId, documentId, taskId]() {
		auto isAlive = aliveGuard.lock();
		if (!isAlive || !isAlive->load()) {
			return;
		}

		QMutexLocker locker(&m_mutex);

		if (singleCopyMode) {
			if (!m_sharedDocuments.contains(objectId)) {
				return;
			}

			SharedDocumentData& shared = m_sharedDocuments[objectId];
			if (shared.objectPtr.IsValid() && shared.isLoading) {
				shared.isLoading = false;

				bool observersInitialized = false;
				UserDocumentPairList docs = FindDocumentsByObjectId(objectId);
				for (const UserDocumentPair& pair : docs) {
					WorkingDocument* dp = FindDocument(pair.first, pair.second);
					if (dp != nullptr && dp->isLoading) {
						dp->isLoading = false;

						if (!observersInitialized) {
							InitializeDocumentObservers(*dp, pair.first);
							shared.undoManagerModelId = dp->undoManagerModelId;
							observersInitialized = true;
						}

						OnDocumentDataLoaded(pair.first, pair.second);
					}
				}
			}
		}
		else {
			WorkingDocument* docPtr = FindDocument(userId, documentId);

			if (docPtr != nullptr && docPtr->objectPtr.IsValid() && docPtr->isLoading) {
				docPtr->isLoading = false;

				InitializeDocumentObservers(*docPtr, userId);

				OnDocumentDataLoaded(userId, documentId);
			}
		}

		CompleteTask(taskId, TaskResult{OS_OK, documentId, QString()});
	});

	QObject::connect(worker, &QObject::destroyed, thread, &QThread::quit);
	QObject::connect(thread, &QThread::finished, thread, &QObject::deleteLater);

	thread->start();
}


IDocumentService::OperationStatus CCollectionDocumentServiceBase::SetDocumentName(const QByteArray& userId, const QByteArray& documentId, const QString& documentName)
{
	QMutexLocker locker(&m_mutex);

	OperationStatus validationStatus;
	if (!ValidateInputParams(userId, documentId, validationStatus)){
		return validationStatus;
	}

	WorkingDocument* workingDocumentPtr = &m_userDocuments[userId][documentId];

	if (workingDocumentPtr->name == documentName){
		return OS_OK;
	}

	QByteArray objectId = workingDocumentPtr->objectId;
	if (!objectId.isEmpty()){
		imtbase::IObjectCollection* collectionPtr = GetCollection();
		if (collectionPtr == nullptr){
			return OS_FAILED;
		}

		if (!collectionPtr->SetElementName(objectId, documentName)) {
			return OS_FAILED;
		}
	}

	workingDocumentPtr->name = documentName;

	if (IsSingleCopyMode() && !objectId.isEmpty()) {
		if (m_sharedDocuments.contains(objectId)) {
			m_sharedDocuments[objectId].name = documentName;
		}

		UserDocumentPairList docs = FindDocumentsByObjectId(objectId);
		for (const UserDocumentPair& pair : docs) {
			if (pair.first == userId && pair.second == documentId) {
				continue;
			}
			WorkingDocument* dp = FindDocument(pair.first, pair.second);
			if (dp != nullptr) {
				dp->name = documentName;

				DocumentNotificationPtr notificationPtr = CreateDocumentNotification(pair.first, pair.second);
				if (notificationPtr != nullptr){
					istd::IChangeable::ChangeSet changeSet(CF_DOCUMENT_RENAMED);
					changeSet.SetChangeInfo(CN_DOCUMENT_RENAMED, QVariant::fromValue(*notificationPtr));
					istd::CChangeNotifier notifier(this, &changeSet);
				}

				for (IDocumentServiceEventHandler* handlerPtr : GetDocumentServiceEventHandlers()){
					if (handlerPtr != nullptr){
						CDocumentRenamedEvent event(
							pair.first,
							pair.second,
							dp->typeId,
							dp->name,
							ObjectIdToUrl(dp->objectId),
							dp->isDirty);
						handlerPtr->ProcessEvent(&event);
					}
				}
			}
		}
	}

	DocumentNotificationPtr notificationPtr = CreateDocumentNotification(userId, documentId);
	Q_ASSERT(notificationPtr != nullptr);
	if (notificationPtr != nullptr){
		istd::IChangeable::ChangeSet changeSet(CF_DOCUMENT_RENAMED);
		changeSet.SetChangeInfo(CN_DOCUMENT_RENAMED, QVariant::fromValue(*notificationPtr));
		istd::CChangeNotifier notifier(this, &changeSet);
	}

	for (IDocumentServiceEventHandler* handlerPtr : GetDocumentServiceEventHandlers()){
		if (handlerPtr != nullptr){
			CDocumentRenamedEvent event(
				userId,
				documentId,
				workingDocumentPtr->typeId,
				workingDocumentPtr->name,
				ObjectIdToUrl(workingDocumentPtr->objectId),
				workingDocumentPtr->isDirty);
			handlerPtr->ProcessEvent(&event);
		}
	}

	return OS_OK;
}


void CCollectionDocumentServiceBase::DoSaveDocument(
	const QByteArray& taskId,
	const TaskParams& params)
{
	const QByteArray& userId = params.userId;
	const QByteArray& documentId = params.documentId;
	const QString& documentName = params.documentName;

	imtbase::IObjectCollection* collectionPtr = GetCollection();
	if (collectionPtr == nullptr) {
		CompleteTask(taskId, TaskResult{OS_FAILED, documentId, QStringLiteral("No collection available")});
		return;
	}

	QMutexLocker locker(&m_mutex);

	OperationStatus validationStatus = OS_OK;
	if (!ValidateInputParams(userId, documentId, validationStatus)){
		CompleteTask(taskId, TaskResult{validationStatus, documentId, QString()});
		return;
	}

	WorkingDocument* workingDocumentPtr = &m_userDocuments[userId][documentId];

	if (workingDocumentPtr->isLoading) {
		CompleteTask(taskId, TaskResult{OS_FAILED, documentId, QStringLiteral("Document is still loading")});
		return;
	}

	istd::IChangeableSharedPtr documentSnapshotPtr = CreateObject(workingDocumentPtr->typeId);
	if (!documentSnapshotPtr.IsValid()){
		CompleteTask(taskId, TaskResult{OS_FAILED, documentId, QStringLiteral("Failed to create snapshot")});
		return;
	}

	if (!documentSnapshotPtr->CopyFrom(*workingDocumentPtr->objectPtr)){
		CompleteTask(taskId, TaskResult{OS_FAILED, documentId, QStringLiteral("Failed to copy document data")});
		return;
	}

	WorkingDocument workingDocumentSnapshot = *workingDocumentPtr;
	workingDocumentSnapshot.objectPtr = documentSnapshotPtr;

	QString validationMessage;
	if (!ValidateDocumentData(workingDocumentSnapshot, validationStatus, &validationMessage)){
		QString msg = validationMessage.isEmpty() ? GetInvalidDocumentMessage() : validationMessage;
		CompleteTask(taskId, TaskResult{validationStatus, documentId, msg});
		return;
	}

	istd::CChangeGroup changeGroup(collectionPtr);

	QString resultDocumentName = documentName;
	if (!workingDocumentPtr->objectId.isEmpty()) {
		// Create copy of the object
		if (!resultDocumentName.isEmpty() && workingDocumentPtr->name != resultDocumentName){
			QByteArray oldObjectId = workingDocumentPtr->objectId;

			QByteArray newObjectId = collectionPtr->InsertNewObject(
				workingDocumentPtr->typeId, resultDocumentName, "", documentSnapshotPtr.GetPtr());

			if (newObjectId.isEmpty()){
				CompleteTask(taskId, TaskResult{OS_FAILED, documentId, QStringLiteral("Failed to insert copy")});
				return;
			}

			if (HasDocumentNameProvider(workingDocumentPtr->typeId)){
				resultDocumentName = GetDefaultDocumentName(*workingDocumentPtr);
				collectionPtr->SetElementName(newObjectId, resultDocumentName);
			}

			// In single-copy mode, detach from shared document before changing objectId
			if (IsSingleCopyMode() && m_sharedDocuments.contains(oldObjectId)) {
				SharedDocumentData& shared = m_sharedDocuments[oldObjectId];
				shared.refCount--;
				bool isLastUser = (shared.refCount <= 0);
				if (isLastUser) {
					imod::IModel* undoModelPtr = dynamic_cast<imod::IModel*>(shared.undoManagerPtr.GetPtr());
					if (undoModelPtr != nullptr && shared.undoManagerModelId >= 0) {
						m_undoManagerObserver.UnregisterModel(shared.undoManagerModelId);
					}
					m_sharedDocuments.remove(oldObjectId);
				}

				// Create new private objectPtr and undoManagerPtr for this user
				istd::IChangeableSharedPtr newObjectPtr = CreateObject(workingDocumentPtr->typeId);
				if (newObjectPtr.IsValid()) {
					newObjectPtr->CopyFrom(*documentSnapshotPtr);
				}
				idoc::IUndoManagerSharedPtr newUndoManagerPtr = CreateUndoManager();

				workingDocumentPtr->objectPtr = newObjectPtr;
				workingDocumentPtr->undoManagerPtr = newUndoManagerPtr;

				InitializeDocumentObservers(*workingDocumentPtr, userId);
			}

			workingDocumentPtr->objectId = newObjectId;
			workingDocumentPtr->name = resultDocumentName;
			workingDocumentPtr->isDirty = false;
			workingDocumentPtr->undoManagerPtr->StoreDocumentState();

			DocumentNotificationPtr notificationPtr = CreateDocumentNotification(userId, documentId);
			Q_ASSERT(notificationPtr != nullptr);
			if (notificationPtr != nullptr){
				istd::IChangeable::ChangeSet changeSet(CF_DOCUMENT_SAVED_AS);
				changeSet.SetChangeInfo(CN_DOCUMENT_SAVED_AS, QVariant::fromValue(*notificationPtr));
				istd::CChangeNotifier notifier(this, &changeSet);
			}

			for (IDocumentServiceEventHandler* handlerPtr : GetDocumentServiceEventHandlers()){
				if (handlerPtr != nullptr){
					CDocumentSavedAsEvent event(
						userId,
						documentId,
						workingDocumentPtr->typeId,
						workingDocumentPtr->name,
						ObjectIdToUrl(workingDocumentPtr->objectId),
						workingDocumentPtr->isDirty);
					handlerPtr->ProcessEvent(&event);
				}
			}

			CompleteTask(taskId, TaskResult{OS_OK, documentId, QString()});
			return;
		}

		// Update object
		bool res = collectionPtr->SetObjectData(workingDocumentPtr->objectId, *documentSnapshotPtr);

		if (res){
			QString updatedName;
			if (HasDocumentNameProvider(workingDocumentPtr->typeId)){
				updatedName = GetDefaultDocumentName(*workingDocumentPtr);
				collectionPtr->SetElementName(workingDocumentPtr->objectId, updatedName);
			}

			workingDocumentPtr->undoManagerPtr->StoreDocumentState();

			if (IsSingleCopyMode() && !workingDocumentPtr->objectId.isEmpty()) {
				// Update all users sharing this document
				QByteArray sharedObjectId = workingDocumentPtr->objectId;

				if (m_sharedDocuments.contains(sharedObjectId) && !updatedName.isEmpty()) {
					m_sharedDocuments[sharedObjectId].name = updatedName;
				}

				UserDocumentPairList docs = FindDocumentsByObjectId(sharedObjectId);
				for (const UserDocumentPair& pair : docs) {
					WorkingDocument* dp = FindDocument(pair.first, pair.second);
					if (dp == nullptr) {
						continue;
					}

					dp->isDirty = false;
					if (!updatedName.isEmpty()) {
						dp->name = updatedName;
					}

					DocumentNotificationPtr nPtr = CreateDocumentNotification(pair.first, pair.second);
					if (nPtr != nullptr){
						istd::IChangeable::ChangeSet changeSet(CF_DOCUMENT_SAVED);
						changeSet.SetChangeInfo(CN_DOCUMENT_SAVED, QVariant::fromValue(*nPtr));
						istd::CChangeNotifier notifier(this, &changeSet);
					}

					for (IDocumentServiceEventHandler* handlerPtr : GetDocumentServiceEventHandlers()){
						if (handlerPtr != nullptr){
							CDocumentSavedEvent event(
								pair.first,
								pair.second,
								dp->typeId,
								dp->name,
								ObjectIdToUrl(dp->objectId),
								dp->isDirty);
							handlerPtr->ProcessEvent(&event);
						}
					}
				}
			}
			else {
				if (!updatedName.isEmpty()) {
					workingDocumentPtr->name = updatedName;
				}

				workingDocumentPtr->isDirty = false;

				DocumentNotificationPtr notificationPtr = CreateDocumentNotification(userId, documentId);
				Q_ASSERT(notificationPtr != nullptr);
				if (notificationPtr != nullptr){
					istd::IChangeable::ChangeSet changeSet(CF_DOCUMENT_SAVED);
					changeSet.SetChangeInfo(CN_DOCUMENT_SAVED, QVariant::fromValue(*notificationPtr));
					istd::CChangeNotifier notifier(this, &changeSet);
				}

				for (IDocumentServiceEventHandler* handlerPtr : GetDocumentServiceEventHandlers()){
					if (handlerPtr != nullptr){
						CDocumentSavedEvent event(
							userId,
							documentId,
							workingDocumentPtr->typeId,
							workingDocumentPtr->name,
							ObjectIdToUrl(workingDocumentPtr->objectId),
							workingDocumentPtr->isDirty);
						handlerPtr->ProcessEvent(&event);
					}
				}
			}
		}

		CompleteTask(taskId, TaskResult{res ? OS_OK : OS_FAILED, documentId, res ? QString() : QStringLiteral("Failed to update object data")});
		return;
	}

	// Create new object
	// m_mutex is already held by the QMutexLocker above (acquired at function entry)
	QByteArray proposedElementId;
	{
		auto it = m_proposedSourceDocumentIds.find(documentId);
		if (it != m_proposedSourceDocumentIds.end()) {
			proposedElementId = it.value();
			m_proposedSourceDocumentIds.erase(it);
		}
	}

	workingDocumentPtr->objectId =
		collectionPtr->InsertNewObject(workingDocumentPtr->typeId, resultDocumentName, "", documentSnapshotPtr.GetPtr(), proposedElementId);

	if (HasDocumentNameProvider(workingDocumentPtr->typeId)){
		resultDocumentName = GetDefaultDocumentName(*workingDocumentPtr);
		collectionPtr->SetElementName(workingDocumentPtr->objectId, resultDocumentName);
	}

	if (!workingDocumentPtr->objectId.isEmpty()){
		workingDocumentPtr->name = resultDocumentName;
		workingDocumentPtr->isDirty = false;
		workingDocumentPtr->undoManagerPtr->StoreDocumentState();

		DocumentNotificationPtr notificationPtr = CreateDocumentNotification(userId, documentId);
		Q_ASSERT(notificationPtr != nullptr);
		if (notificationPtr != nullptr){
			istd::IChangeable::ChangeSet changeSet(CF_DOCUMENT_SAVED);
			changeSet.SetChangeInfo(CN_DOCUMENT_SAVED, QVariant::fromValue(*notificationPtr));
			istd::CChangeNotifier notifier(this, &changeSet);
		}

		for (IDocumentServiceEventHandler* handlerPtr : GetDocumentServiceEventHandlers()){
			if (handlerPtr != nullptr){
				CDocumentSavedEvent event(
					userId,
					documentId,
					workingDocumentPtr->typeId,
					workingDocumentPtr->name,
					ObjectIdToUrl(workingDocumentPtr->objectId),
					workingDocumentPtr->isDirty);
				handlerPtr->ProcessEvent(&event);
			}
		}

	}

	OperationStatus saveStatus = workingDocumentPtr->objectId.isEmpty() ? OS_FAILED : OS_OK;
	CompleteTask(taskId, TaskResult{saveStatus, documentId, saveStatus == OS_OK ? QString() : QStringLiteral("Failed to insert new object")});
}


void CCollectionDocumentServiceBase::DoCloseDocument(
	const QByteArray& taskId,
	const TaskParams& params)
{
	QMutexLocker locker(&m_mutex);

	m_proposedSourceDocumentIds.remove(params.documentId);

	OperationStatus status = CloseDocumentInternal(params.userId, params.documentId);
	QString msg;
	if (status != OS_OK){
		switch (status){
			case OS_INVALID_USER_ID:  msg = QStringLiteral("Invalid user ID"); break;
			case OS_INVALID_DOCUMENT_ID: msg = QStringLiteral("Invalid document ID"); break;
			default: msg = QStringLiteral("Close failed"); break;
		}
	}
	CompleteTask(taskId, TaskResult{status, params.documentId, msg});
}


} // namespace imtdoc
