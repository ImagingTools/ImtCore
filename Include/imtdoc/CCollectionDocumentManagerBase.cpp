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

	idoc::IUndoManagerSharedPtr undoManagerPtr = CreateUndoManager();
	if (!undoManagerPtr.IsValid()){
		return retVal;
	}

	QByteArray objectTypeId = collectionPtr->GetObjectTypeId(objectId);

	if (objectTypeId.isEmpty()) {
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

	std::weak_ptr<std::atomic<bool>> aliveGuard(m_isAlive);
	QObject::connect(thread, &QThread::started, worker, [this, aliveGuard, objectId, userId, documentId, worker]() {
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
			WorkingDocument* docPtr = FindDocument(userId, documentId);

			if (docPtr != nullptr && success && dataPtr.IsValid()) {
				docPtr->objectPtr = dataPtr;
			}
			else if (docPtr != nullptr) {
				// Loading failed - close the document and notify client
				docPtr->isLoading = false;
				CloseDocument(userId, documentId);
			}
		}

		worker->deleteLater();
	});

	// Initialize observers and fire events in the main thread after background work completes
	QObject::connect(thread, &QThread::finished, QCoreApplication::instance(), [this, aliveGuard, userId, documentId]() {
		auto isAlive = aliveGuard.lock();
		if (!isAlive || !isAlive->load()) {
			return;
		}

		QMutexLocker locker(&m_mutex);
		WorkingDocument* docPtr = FindDocument(userId, documentId);

		if (docPtr != nullptr && docPtr->objectPtr.IsValid() && docPtr->isLoading) {
			docPtr->isLoading = false;

			InitializeDocumentObservers(*docPtr, userId);

			OnDocumentDataLoaded(userId, documentId);
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
			QByteArray newObjectId = collectionPtr->InsertNewObject(
				workingDocumentPtr->typeId, resultDocumentName, "", documentSnapshotPtr.GetPtr());

			if (newObjectId.isEmpty()){
				return OS_FAILED;
			}

			if (HasDocumentNameProvider(workingDocumentPtr->typeId)){
				resultDocumentName = GetDefaultDocumentName(*workingDocumentPtr);
				collectionPtr->SetElementName(newObjectId, resultDocumentName);
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
			if (HasDocumentNameProvider(workingDocumentPtr->typeId)){
				workingDocumentPtr->name = GetDefaultDocumentName(*workingDocumentPtr);
				collectionPtr->SetElementName(workingDocumentPtr->objectId, workingDocumentPtr->name);
			}

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
