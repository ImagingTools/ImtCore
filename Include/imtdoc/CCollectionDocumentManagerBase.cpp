// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtdoc/CCollectionDocumentManagerBase.h>


// Qt includes
#include <QtCore/QCoreApplication>
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


// reimplemented (imtdoc::IDocumentManager)

QByteArray CCollectionDocumentManagerBase::OpenDocument(const QByteArray& userId, const QUrl& url)
{
	QByteArray retVal;

	if (url.scheme() != "collection" || !url.host().isEmpty()){
		return retVal;
	}

	QString path = url.path();

	QStringList parts = path.split('/', Qt::SkipEmptyParts);
	if (parts.count() != 1){
		return retVal;
	}

	QByteArray objectId = parts.first().toUtf8();

	imtbase::IObjectCollection* collectionPtr = GetCollection();
	if (collectionPtr == nullptr) {
		return retVal;
	}

	QByteArray objectTypeId = collectionPtr->GetObjectTypeId(objectId);

	if (objectTypeId.isEmpty()) {
		return retVal;
	}

	// Single-copy mode: check if this object is already opened by any user
	if (IsSingleCopyMode()) {
		QMutexLocker locker(&m_mutex);
		if (m_sharedDocuments.contains(objectId)) {
			SharedDocumentData& shared = m_sharedDocuments[objectId];
			shared.refCount++;

			retVal = QUuid::createUuid().toByteArray(QUuid::WithoutBraces);

			WorkingDocument& doc = m_userDocuments[userId][retVal];
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
				info.documentId = retVal;
				info.typeId = shared.typeId;
				info.url = url;
				info.name = shared.name;
				info.isDirty = doc.isDirty;
				info.isLoading = shared.isLoading;

				istd::IChangeable::ChangeSet changeSet(CF_DOCUMENT_OPENED);
				changeSet.SetChangeInfo(CN_DOCUMENT_OPENED, QVariant::fromValue(info));
				istd::CChangeNotifier notifier(this, &changeSet);
			}

			for (IDocumentManagerEventHandler* handlerPtr : GetDocumentManagerEventHandlers()){
				if (handlerPtr != nullptr){
					CDocumentOpenedEvent event(
						userId,
						retVal,
						shared.typeId,
						shared.name,
						ObjectIdToUrl(objectId),
						doc.isDirty);
					handlerPtr->ProcessEvent(&event);
				}
			}

			if (!shared.isLoading) {
				OnDocumentDataLoaded(userId, retVal);
			}

			return retVal;
		}
	}

	idoc::IUndoManagerSharedPtr undoManagerPtr = CreateUndoManager();
	if (!undoManagerPtr.IsValid()){
		return retVal;
	}

	retVal = QUuid::createUuid().toByteArray(QUuid::WithoutBraces);

	WorkingDocument* documentPtr = nullptr;
	{
		QMutexLocker locker(&m_mutex);
		documentPtr = &m_userDocuments[userId][retVal];
	}

	documentPtr->objectId = objectId;
	documentPtr->typeId = objectTypeId;
	documentPtr->url = url;
	documentPtr->name = collectionPtr->GetElementInfo(objectId, imtbase::ICollectionInfo::EIT_NAME).toString();

	documentPtr->undoManagerPtr = undoManagerPtr;
	documentPtr->isDirty = false;
	documentPtr->isLoading = true;

	if (IsSingleCopyMode()) {
		QMutexLocker locker(&m_mutex);
		SharedDocumentData& shared = m_sharedDocuments[objectId];
		shared.typeId = objectTypeId;
		shared.name = documentPtr->name;
		shared.undoManagerPtr = undoManagerPtr;
		shared.refCount = 1;
		shared.isLoading = true;
	}

	{
		DocumentOpenedInfo info;
		info.userId = userId;
		info.documentId = retVal;
		info.typeId = objectTypeId;
		info.url = url;
		info.name = documentPtr->name;
		info.isDirty = false;
		info.isLoading = true;

		istd::IChangeable::ChangeSet changeSet(CF_DOCUMENT_OPENED);
		changeSet.SetChangeInfo(CN_DOCUMENT_OPENED, QVariant::fromValue(info));
		istd::CChangeNotifier notifier(this, &changeSet);
	}

	for (IDocumentManagerEventHandler* handlerPtr : GetDocumentManagerEventHandlers()){
		if (handlerPtr != nullptr){
			CDocumentOpenedEvent event(
				userId,
				retVal,
				objectTypeId,
				documentPtr->name,
				ObjectIdToUrl(documentPtr->objectId),
				documentPtr->isDirty);
			handlerPtr->ProcessEvent(&event);
		}
	}

	// Load document data asynchronously in a separate thread
	QByteArray documentId = retVal;
	QThread* thread = new QThread();
	QObject* worker = new QObject();
	worker->moveToThread(thread);

	bool singleCopyMode = IsSingleCopyMode();
	std::weak_ptr<std::atomic<bool>> aliveGuard(m_isAlive);
	QObject::connect(thread, &QThread::started, worker, [this, aliveGuard, singleCopyMode, objectId, userId, documentId, worker]() {
		auto isAlive = aliveGuard.lock();
		if (!isAlive || !isAlive->load()) {
			worker->deleteLater();
			return;
		}

		imtbase::IObjectCollection* collPtr = GetCollection();
		if (collPtr == nullptr) {
			worker->deleteLater();
			return;
		}

		imtbase::IObjectCollection::DataPtr dataPtr;
		bool success = collPtr->GetObjectData(objectId, dataPtr);

		isAlive = aliveGuard.lock();
		if (!isAlive || !isAlive->load()) {
			worker->deleteLater();
			return;
		}

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
							CloseDocument(pair.first, pair.second);
						}
					}
				}
			}
			else {
				WorkingDocument* docPtr = FindDocument(userId, documentId);

				if (docPtr != nullptr && success && dataPtr.IsValid()) {
					docPtr->objectPtr = dataPtr;
				}
				else if (docPtr != nullptr) {
					docPtr->isLoading = false;
					CloseDocument(userId, documentId);
				}
			}
		}

		worker->deleteLater();
	});

	// Initialize observers and fire events in the main thread after background work completes
	QObject::connect(thread, &QThread::finished, QCoreApplication::instance(), [this, aliveGuard, singleCopyMode, objectId, userId, documentId]() {
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
	});

	QObject::connect(worker, &QObject::destroyed, thread, &QThread::quit);
	QObject::connect(thread, &QThread::finished, thread, &QObject::deleteLater);

	thread->start();

	return retVal;
}


IDocumentManager::OperationStatus CCollectionDocumentManagerBase::SetDocumentName(const QByteArray& userId, const QByteArray& documentId, const QString& documentName)
{
	WorkingDocument* workingDocumentPtr = nullptr;
	{
		QMutexLocker locker(&m_mutex);
		OperationStatus validationStatus;
		if (!ValidateInputParams(userId, documentId, validationStatus)){
			return validationStatus;
		}

		workingDocumentPtr = &m_userDocuments[userId][documentId];
	}

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
		QMutexLocker locker(&m_mutex);

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

				for (IDocumentManagerEventHandler* handlerPtr : GetDocumentManagerEventHandlers()){
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

	for (IDocumentManagerEventHandler* handlerPtr : GetDocumentManagerEventHandlers()){
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


IDocumentManager::OperationStatus CCollectionDocumentManagerBase::SaveDocument(
	const QByteArray& userId,
	const QByteArray& documentId,
	const QString& documentName,
	QString* errorMessage)
{
	imtbase::IObjectCollection* collectionPtr = GetCollection();
	if (collectionPtr == nullptr) {
		return OS_FAILED;
	}

	WorkingDocument* workingDocumentPtr = nullptr;
	OperationStatus validationStatus = OS_OK;
	WorkingDocument workingDocumentSnapshot;
	istd::IChangeableSharedPtr documentSnapshotPtr;
	{
		QMutexLocker locker(&m_mutex);
		if (!ValidateInputParams(userId, documentId, validationStatus)){
			return validationStatus;
		}

		workingDocumentPtr = &m_userDocuments[userId][documentId];

		if (workingDocumentPtr->isLoading) {
			return OS_FAILED;
		}

		documentSnapshotPtr = CreateObject(workingDocumentPtr->typeId);
		if (!documentSnapshotPtr.IsValid()){
			return OS_FAILED;
		}

		if (!documentSnapshotPtr->CopyFrom(*workingDocumentPtr->objectPtr)){
			return OS_FAILED;
		}

		workingDocumentSnapshot = *workingDocumentPtr;
		workingDocumentSnapshot.objectPtr = documentSnapshotPtr;
	}

	QString validationMessage;
	if (!ValidateDocumentData(workingDocumentSnapshot, validationStatus, &validationMessage)){
		if (errorMessage != nullptr) {
			*errorMessage = validationMessage.isEmpty() ? GetInvalidDocumentMessage() : validationMessage;
		}
		return validationStatus;
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
				return OS_FAILED;
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

			for (IDocumentManagerEventHandler* handlerPtr : GetDocumentManagerEventHandlers()){
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

			return OS_OK;
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

					for (IDocumentManagerEventHandler* handlerPtr : GetDocumentManagerEventHandlers()){
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

				for (IDocumentManagerEventHandler* handlerPtr : GetDocumentManagerEventHandlers()){
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
		}

		return res ? OS_OK : OS_FAILED;
	}

	// Create new object
	workingDocumentPtr->objectId =
		collectionPtr->InsertNewObject(workingDocumentPtr->typeId, resultDocumentName, "", documentSnapshotPtr.GetPtr());

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

		for (IDocumentManagerEventHandler* handlerPtr : GetDocumentManagerEventHandlers()){
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

	return workingDocumentPtr->objectId.isEmpty() ? OS_FAILED : OS_OK;
}


} // namespace imtdoc
