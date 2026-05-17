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


// reimplemented (imtdoc::CDocumentServiceBase)

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
		bool isSharedDocument = false;
		QByteArray sharedTypeId;
		QString sharedName;
		bool sharedIsLoading = false;
		bool docIsDirty = false;

		{
			QMutexLocker locker(&m_mutex);
			if (m_sharedDocuments.contains(objectId)) {
				isSharedDocument = true;
				SharedDocumentData& shared = m_sharedDocuments[objectId];
				sharedTypeId = shared.typeId;
				sharedName = shared.name;
				sharedIsLoading = shared.isLoading;
				docIsDirty = shared.undoManagerPtr.IsValid()
					? (shared.undoManagerPtr->GetDocumentChangeFlag() != idoc::IDocumentStateComparator::DCF_EQUAL)
					: false;
			}
		}

		if (isSharedDocument) {
			QByteArray documentId = QUuid::createUuid().toByteArray(QUuid::WithoutBraces);

			DocumentOpenedInfo info;
			info.userId = userId;
			info.documentId = documentId;
			info.typeId = sharedTypeId;
			info.url = url;
			info.name = sharedName;
			info.isDirty = docIsDirty;
			info.isLoading = sharedIsLoading;

			istd::IChangeable::ChangeSet changeSet(CF_DOCUMENT_OPENED);
			changeSet.SetChangeInfo(CN_DOCUMENT_OPENED, QVariant::fromValue(info));

			{
				istd::CChangeNotifier notifier(this, &changeSet);
				{
					QMutexLocker locker(&m_mutex);
					if (m_sharedDocuments.contains(objectId)) {
						SharedDocumentData& shared = m_sharedDocuments[objectId];
						shared.refCount++;

						WorkingDocument& doc = m_userDocuments[userId][documentId];
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
			}

			for (IDocumentServiceEventHandler* handlerPtr : GetDocumentServiceEventHandlers()){
				if (handlerPtr != nullptr){
					CDocumentOpenedEvent event(
								userId,
								documentId,
								sharedTypeId,
								sharedName,
								ObjectIdToUrl(objectId),
								docIsDirty);
					handlerPtr->ProcessEvent(&event);
				}
			}

			if (!sharedIsLoading) {
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

	{
		istd::CChangeNotifier notifier(this, &changeSet);
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

		UserDocumentPairList docsToNotify;

		{
			QMutexLocker locker(&m_mutex);

			if (singleCopyMode) {
				if (!m_sharedDocuments.contains(objectId)) {
					return;
				}

				SharedDocumentData& shared = m_sharedDocuments[objectId];
				if (!shared.objectPtr.IsValid() || !shared.isLoading) {
					return;
				}

				shared.isLoading = false;

				bool observersInitialized = false;
				UserDocumentPairList docs = FindDocumentsByObjectId(objectId);
				for (const UserDocumentPair& pair : docs) {
					WorkingDocument* dp = FindDocument(pair.first, pair.second);
					if (dp != nullptr && dp->isLoading) {
						if (!observersInitialized) {
							InitializeDocumentObservers(*dp, pair.first);
							shared.undoManagerModelId = dp->undoManagerModelId;
							observersInitialized = true;
						}

						docsToNotify.append(pair);
					}
				}
			}
			else {
				WorkingDocument* docPtr = FindDocument(userId, documentId);

				if (docPtr == nullptr || !docPtr->objectPtr.IsValid() || !docPtr->isLoading) {
					return;
				}

				InitializeDocumentObservers(*docPtr, userId);
				docsToNotify.append(qMakePair(userId, documentId));
			}
		}

		// OnDocumentDataLoaded sets isLoading=false inside its CChangeNotifier scope
		for (const UserDocumentPair& pair : docsToNotify) {
			OnDocumentDataLoaded(pair.first, pair.second);
		}

		CompleteTask(taskId, TaskResult{OS_OK, documentId, QString()});
	});

	QObject::connect(worker, &QObject::destroyed, thread, &QThread::quit);
	QObject::connect(thread, &QThread::finished, thread, &QObject::deleteLater);

	thread->start();
}


IDocumentService::OperationStatus CCollectionDocumentServiceBase::SetDocumentName(
			const QByteArray& userId,
			const QByteArray& documentId,
			const QString& documentName)
{
	QByteArray objectId;
	QByteArray typeId;
	bool isDirty = false;

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

		objectId = workingDocumentPtr->objectId;
		typeId = workingDocumentPtr->typeId;
		isDirty = workingDocumentPtr->isDirty;
	}

	if (!objectId.isEmpty()){
		imtbase::IObjectCollection* collectionPtr = GetCollection();
		if (collectionPtr == nullptr){
			return OS_FAILED;
		}

		if (!collectionPtr->SetElementName(objectId, documentName)) {
			return OS_FAILED;
		}
	}

	// Shared docs: rename and notify each user sharing this object
	if (IsSingleCopyMode() && !objectId.isEmpty()) {
		UserDocumentPairList sharedDocs;
		{
			QMutexLocker locker(&m_mutex);
			if (m_sharedDocuments.contains(objectId)) {
				m_sharedDocuments[objectId].name = documentName;
			}
			sharedDocs = FindDocumentsByObjectId(objectId);
		}

		for (const UserDocumentPair& pair : sharedDocs) {
			if (pair.first == userId && pair.second == documentId) {
				continue;
			}

			QByteArray dpTypeId;
			QByteArray dpObjectId;
			bool dpIsDirty = false;
			bool dpExists = false;

			{
				QMutexLocker locker(&m_mutex);
				WorkingDocument* dp = FindDocument(pair.first, pair.second);
				if (dp != nullptr) {
					dpExists = true;
					dpTypeId = dp->typeId;
					dpObjectId = dp->objectId;
					dpIsDirty = dp->isDirty;
				}
			}

			if (!dpExists) {
				continue;
			}

			DocumentNotification notification;
			notification.userId = pair.first;
			notification.documentId = pair.second;
			notification.typeId = dpTypeId;
			notification.url = ObjectIdToUrl(dpObjectId);
			notification.name = documentName;
			notification.isDirty = dpIsDirty;

			istd::IChangeable::ChangeSet changeSet(CF_DOCUMENT_RENAMED);
			changeSet.SetChangeInfo(CN_DOCUMENT_RENAMED, QVariant::fromValue(notification));

			{
				istd::CChangeNotifier notifier(this, &changeSet);
				{
					QMutexLocker locker(&m_mutex);
					WorkingDocument* dp = FindDocument(pair.first, pair.second);
					if (dp != nullptr) {
						dp->name = documentName;
					}
				}
			}

			for (IDocumentServiceEventHandler* handlerPtr : GetDocumentServiceEventHandlers()){
				if (handlerPtr != nullptr){
					CDocumentRenamedEvent event(
								pair.first,
								pair.second,
								dpTypeId,
								documentName,
								ObjectIdToUrl(dpObjectId),
								dpIsDirty);
					handlerPtr->ProcessEvent(&event);
				}
			}
		}
	}

	// Primary doc: rename and notify
	DocumentNotification notification;
	notification.userId = userId;
	notification.documentId = documentId;
	notification.typeId = typeId;
	notification.url = ObjectIdToUrl(objectId);
	notification.name = documentName;
	notification.isDirty = isDirty;

	istd::IChangeable::ChangeSet changeSet(CF_DOCUMENT_RENAMED);
	changeSet.SetChangeInfo(CN_DOCUMENT_RENAMED, QVariant::fromValue(notification));

	{
		istd::CChangeNotifier notifier(this, &changeSet);
		{
			QMutexLocker locker(&m_mutex);
			WorkingDocument* workingDocumentPtr = FindDocument(userId, documentId);
			if (workingDocumentPtr != nullptr) {
				workingDocumentPtr->name = documentName;
			}
		}
	}

	for (IDocumentServiceEventHandler* handlerPtr : GetDocumentServiceEventHandlers()){
		if (handlerPtr != nullptr){
			CDocumentRenamedEvent event(
						userId,
						documentId,
						typeId,
						documentName,
						ObjectIdToUrl(objectId),
						isDirty);
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

			// Prepare notification with expected new state
			QByteArray savedTypeId = workingDocumentPtr->typeId;

			DocumentNotification notification;
			notification.userId = userId;
			notification.documentId = documentId;
			notification.typeId = savedTypeId;
			notification.url = ObjectIdToUrl(newObjectId);
			notification.name = resultDocumentName;
			notification.isDirty = false;

			istd::IChangeable::ChangeSet changeSet(CF_DOCUMENT_SAVED_AS);
			changeSet.SetChangeInfo(CN_DOCUMENT_SAVED_AS, QVariant::fromValue(notification));

			locker.unlock();
			{
				istd::CChangeNotifier notifier(this, &changeSet);
				{
					locker.relock();
					workingDocumentPtr = FindDocument(userId, documentId);
					if (workingDocumentPtr != nullptr) {
						workingDocumentPtr->objectId = newObjectId;
						workingDocumentPtr->name = resultDocumentName;
						workingDocumentPtr->isDirty = false;
						workingDocumentPtr->undoManagerPtr->StoreDocumentState();
					}
					locker.unlock();
				}
			}

			for (IDocumentServiceEventHandler* handlerPtr : GetDocumentServiceEventHandlers()){
				if (handlerPtr != nullptr){
					CDocumentSavedAsEvent event(
								userId,
								documentId,
								savedTypeId,
								resultDocumentName,
								ObjectIdToUrl(newObjectId),
								false);
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

					QByteArray dpTypeId = dp->typeId;
					QByteArray dpObjectId = dp->objectId;
					QString dpNewName = !updatedName.isEmpty() ? updatedName : dp->name;

					DocumentNotification dpNotification;
					dpNotification.userId = pair.first;
					dpNotification.documentId = pair.second;
					dpNotification.typeId = dpTypeId;
					dpNotification.url = ObjectIdToUrl(dpObjectId);
					dpNotification.name = dpNewName;
					dpNotification.isDirty = false;

					istd::IChangeable::ChangeSet dpChangeSet(CF_DOCUMENT_SAVED);
					dpChangeSet.SetChangeInfo(CN_DOCUMENT_SAVED, QVariant::fromValue(dpNotification));

					locker.unlock();
					{
						istd::CChangeNotifier notifier(this, &dpChangeSet);
						{
							locker.relock();
							dp = FindDocument(pair.first, pair.second);
							if (dp != nullptr) {
								dp->isDirty = false;
								if (!updatedName.isEmpty()) {
									dp->name = updatedName;
								}
							}
							locker.unlock();
						}
					}

					for (IDocumentServiceEventHandler* handlerPtr : GetDocumentServiceEventHandlers()){
						if (handlerPtr != nullptr){
							CDocumentSavedEvent event(
										pair.first,
										pair.second,
										dpTypeId,
										dpNewName,
										ObjectIdToUrl(dpObjectId),
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

				DocumentNotification notification;
				notification.userId = userId;
				notification.documentId = documentId;
				notification.typeId = savedTypeId;
				notification.url = ObjectIdToUrl(savedObjectId);
				notification.name = newName;
				notification.isDirty = false;

				istd::IChangeable::ChangeSet changeSet(CF_DOCUMENT_SAVED);
				changeSet.SetChangeInfo(CN_DOCUMENT_SAVED, QVariant::fromValue(notification));

				locker.unlock();
				{
					istd::CChangeNotifier notifier(this, &changeSet);
					{
						locker.relock();
						workingDocumentPtr = FindDocument(userId, documentId);
						if (workingDocumentPtr != nullptr) {
							if (!updatedName.isEmpty()) {
								workingDocumentPtr->name = updatedName;
							}
							workingDocumentPtr->isDirty = false;
						}
						locker.unlock();
					}
				}

				for (IDocumentServiceEventHandler* handlerPtr : GetDocumentServiceEventHandlers()){
					if (handlerPtr != nullptr){
						CDocumentSavedEvent event(
									userId,
									documentId,
									savedTypeId,
									newName,
									ObjectIdToUrl(savedObjectId),
									false);
						handlerPtr->ProcessEvent(&event);
					}
				}

				locker.relock();
			}
		}

		CompleteTask(taskId, TaskResult{res ? OS_OK : OS_FAILED, documentId, res ? QString() : QStringLiteral("Failed to update object data")});
		return;
	}

	// Create new object
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
		QByteArray savedTypeId = workingDocumentPtr->typeId;
		QByteArray savedObjectId = workingDocumentPtr->objectId;

		DocumentNotification notification;
		notification.userId = userId;
		notification.documentId = documentId;
		notification.typeId = savedTypeId;
		notification.url = ObjectIdToUrl(savedObjectId);
		notification.name = resultDocumentName;
		notification.isDirty = false;

		istd::IChangeable::ChangeSet changeSet(CF_DOCUMENT_SAVED);
		changeSet.SetChangeInfo(CN_DOCUMENT_SAVED, QVariant::fromValue(notification));

		locker.unlock();
		{
			istd::CChangeNotifier notifier(this, &changeSet);
			{
				locker.relock();
				workingDocumentPtr = FindDocument(userId, documentId);
				if (workingDocumentPtr != nullptr) {
					workingDocumentPtr->name = resultDocumentName;
					workingDocumentPtr->isDirty = false;
					workingDocumentPtr->undoManagerPtr->StoreDocumentState();
				}
				locker.unlock();
			}
		}

		for (IDocumentServiceEventHandler* handlerPtr : GetDocumentServiceEventHandlers()){
			if (handlerPtr != nullptr){
				CDocumentSavedEvent event(
							userId,
							documentId,
							savedTypeId,
							resultDocumentName,
							ObjectIdToUrl(savedObjectId),
							false);
				handlerPtr->ProcessEvent(&event);
			}
		}

		locker.relock();
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
	QString message;
	if (status != OS_OK){
		switch (status){
			case OS_INVALID_USER_ID:
				message = QStringLiteral("Invalid user ID");
				break;
			case OS_INVALID_DOCUMENT_ID:
				message = QStringLiteral("Invalid document ID");
				break;
			default:
				message = QStringLiteral("Close failed");
				break;
		}
	}
	CompleteTask(taskId, TaskResult{status, params.documentId, message});
}


} // namespace imtdoc
