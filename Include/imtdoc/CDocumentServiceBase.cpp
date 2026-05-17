// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtdoc/CDocumentServiceBase.h>


// Qt includes
#include <QtCore/QCoreApplication>
#include <QtCore/QUuid>
#include <QtCore/QThread>

// ACF includes
#include <istd/CChangeNotifier.h>
#include <imod/IModel.h>
#include <imod/IObserver.h>

// ImtCore includes
#include <imtdoc/CDocumentChangedEvent.h>
#include <imtdoc/CDocumentClosedEvent.h>
#include <imtdoc/CDocumentCreatedEvent.h>
#include <imtdoc/CDocumentDataLoadedEvent.h>
#include <imtdoc/CDocumentOpenedEvent.h>
#include <imtdoc/CDocumentRenamedEvent.h>
#include <imtdoc/CDocumentSavedAsEvent.h>
#include <imtdoc/CDocumentSavedEvent.h>
#include <imtdoc/CDocumentUndoRedoChangedEvent.h>


namespace imtdoc
{


// public methods

CDocumentServiceBase::CDocumentServiceBase()
	:m_undoManagerObserver(*this),
	m_isAlive(std::make_shared<std::atomic<bool>>(true))
{
}


CDocumentServiceBase::~CDocumentServiceBase()
{
	m_isAlive->store(false);

	// Complete all pending tasks so that any thread blocked in
	// WaitForTaskFinished is unblocked before destruction continues.
	{
		QMutexLocker locker(&m_tasksMutex);
		for (auto it = m_pendingTasks.begin(); it != m_pendingTasks.end(); ++it){
			auto& taskContext = it.value();
			QMutexLocker taskLocker(&taskContext->mutex);
			if (!taskContext->isFinished){
				taskContext->result = TaskResult{OS_FAILED, QByteArray(), QStringLiteral("Service destroyed")};
				taskContext->isFinished = true;
				taskContext->condition.wakeAll();
			}
		}
		m_pendingTasks.clear();
	}
}


// reimplemented (imtdoc::IDocumentService) — asynchronous task API

QByteArray CDocumentServiceBase::BeginDocumentTask(
			TaskType taskType,
			const TaskParams& params,
			Error* errorPtr)
{
	QByteArray taskId = QUuid::createUuid().toByteArray(QUuid::WithoutBraces);

	auto context = std::make_shared<TaskContext>();
	{
		QMutexLocker locker(&m_tasksMutex);
		m_pendingTasks[taskId] = context;
	}

	switch (taskType){
		case TT_NEW:
			DoCreateNewDocument(taskId, params);
			break;
		case TT_OPEN:
			DoOpenDocument(taskId, params);
			break;
		case TT_SAVE:
			DoSaveDocument(taskId, params);
			break;
		case TT_CLOSE:
			DoCloseDocument(taskId, params);
			break;
		default:
			CompleteTask(taskId, TaskResult{OS_FAILED, QByteArray(), QStringLiteral("Unknown task type")});
			break;
	}

	// If the task already completed with an error during setup
	// and the caller provided an Error pointer, propagate it.
	{
		QMutexLocker locker(&context->mutex);
		if (context->isFinished && context->result.status != OS_OK){
			if (errorPtr != nullptr){
				errorPtr->status = context->result.status;
				errorPtr->message = context->result.errorMessage;
			}
		}
	}

	return taskId;
}


IDocumentService::TaskResult CDocumentServiceBase::WaitForTaskFinished(const QByteArray& taskId)
{
	std::shared_ptr<TaskContext> taskContext;
	{
		QMutexLocker locker(&m_tasksMutex);
		auto it = m_pendingTasks.find(taskId);
		if (it == m_pendingTasks.end()){
			return TaskResult{OS_FAILED, QByteArray(), QStringLiteral("Unknown task ID")};
		}
		taskContext = it.value();
	}

	// If called from the main / GUI thread we must keep the event loop
	// alive so that completion handlers posted via QueuedConnection
	// (QThread::finished → QCoreApplication) can still fire.
	QCoreApplication* appPtr = QCoreApplication::instance();
	if (appPtr != nullptr && QThread::currentThread() == appPtr->thread()){
		while (true){
			{
				QMutexLocker locker(&taskContext->mutex);
				if (taskContext->isFinished){
					break;
				}
			}
			QCoreApplication::processEvents(QEventLoop::AllEvents, 10);
		}
	}
	else{
		QMutexLocker locker(&taskContext->mutex);
		while (!taskContext->isFinished){
			taskContext->condition.wait(&taskContext->mutex);
		}
	}

	TaskResult result = taskContext->result;

	{
		QMutexLocker locker(&m_tasksMutex);
		m_pendingTasks.remove(taskId);
	}

	return result;
}


// reimplemented (imtdoc::IDocumentService) — synchronous helpers

IDocumentService::DocumentList CDocumentServiceBase::GetOpenedDocumentList(const QByteArray& userId) const
{
	QMutexLocker locker(&m_mutex);

	DocumentList list;

	if (m_userDocuments.contains(userId)) {
		const WorkingDocumentList& workingDocumentList = m_userDocuments[userId];
		for (const QByteArray& id : workingDocumentList.keys()) {
			const WorkingDocument& workingDocument = workingDocumentList[id];

			DocumentListItem info;
			info.documentId = id;
			info.typeId = workingDocument.typeId;
			info.url = workingDocument.url;
			info.name = workingDocument.name;
			info.isDirty = workingDocument.isDirty;
			info.hasNameProvider = HasDocumentNameProvider(workingDocument.typeId);
			info.isLoading = workingDocument.isLoading;

			list.append(info);
		}
	}

	return list;
}


// protected methods — task dispatch

void CDocumentServiceBase::DoCreateNewDocument(const QByteArray& taskId, const TaskParams& params)
{
	idoc::IUndoManagerSharedPtr undoManagerPtr = CreateUndoManager();
	if (!undoManagerPtr.IsValid()) {
		CompleteTask(taskId, TaskResult{OS_FAILED, QByteArray(), QStringLiteral("Failed to create undo manager")});
		return;
	}

	QByteArray documentId = QUuid::createUuid().toByteArray(QUuid::WithoutBraces);

	NewDocumentCreatedInfo info;
	info.userId = params.userId;
	info.documentId = documentId;
	info.typeId = params.documentTypeId;
	info.name = "";
	info.isDirty = false;

	istd::IChangeable::ChangeSet changeSet(CF_NEW_DOCUMENT_CREATED);
	changeSet.SetChangeInfo(CN_NEW_DOCUMENT_CREATED, QVariant::fromValue(info));

	QString documentName;
	{
		istd::CChangeNotifier notifier(this, &changeSet);
		{
			QMutexLocker locker(&m_mutex);
			WorkingDocument& doc = m_userDocuments[params.userId][documentId];
			doc.typeId = params.documentTypeId;
			doc.undoManagerPtr = undoManagerPtr;
			doc.isDirty = false;
			doc.name = "";
			doc.isLoading = true;
			documentName = doc.name;
		}
	}

	for (IDocumentServiceEventHandler* handlerPtr : GetDocumentServiceEventHandlers()){
		if (handlerPtr != nullptr){
			CDocumentCreatedEvent event(
						params.userId,
						documentId,
						params.documentTypeId,
						documentName,
						QUrl(),
						false);
			handlerPtr->ProcessEvent(&event);
		}
	}

	// Create object asynchronously in a separate thread
	QByteArray userId = params.userId;
	QByteArray documentTypeId = params.documentTypeId;
	QThread* thread = new QThread();
	QObject* worker = new QObject();
	worker->moveToThread(thread);

	std::weak_ptr<std::atomic<bool>> aliveGuard(m_isAlive);
	QObject::connect(thread, &QThread::started, worker, [this, aliveGuard, documentTypeId, userId, documentId, taskId, worker]() {
		auto isAlive = aliveGuard.lock();
		if (!isAlive || !isAlive->load()) {
			CompleteTask(taskId, TaskResult{OS_FAILED, QByteArray(), QStringLiteral("Service destroyed")});
			worker->deleteLater();
			return;
		}

		istd::IChangeableSharedPtr objectPtr = CreateObject(documentTypeId);

		isAlive = aliveGuard.lock();
		if (!isAlive || !isAlive->load()) {
			CompleteTask(taskId, TaskResult{OS_FAILED, QByteArray(), QStringLiteral("Service destroyed")});
			worker->deleteLater();
			return;
		}

		{
			QMutexLocker locker(&m_mutex);
			WorkingDocument* docPtr = FindDocument(userId, documentId);

			if (docPtr != nullptr && objectPtr.IsValid()) {
				docPtr->objectPtr = objectPtr;
			}
			else if (docPtr != nullptr) {
				// Creation failed - close the document and notify client
				docPtr->isLoading = false;
				CloseDocumentInternal(userId, documentId);
				CompleteTask(taskId, TaskResult{OS_FAILED, QByteArray(), QStringLiteral("Object creation failed")});
				worker->deleteLater();
				return;
			}
		}

		worker->deleteLater();
	});

	// Initialize observers and fire events in the main thread after background work completes
	QObject::connect(thread, &QThread::finished, QCoreApplication::instance(), [this, aliveGuard, userId, documentId, taskId]() {
		auto isAlive = aliveGuard.lock();
		if (!isAlive || !isAlive->load()) {
			return;
		}

		{
			QMutexLocker locker(&m_mutex);
			WorkingDocument* docPtr = FindDocument(userId, documentId);

			if (docPtr == nullptr || !docPtr->objectPtr.IsValid() || !docPtr->isLoading) {
				return;
			}

			InitializeDocumentObservers(*docPtr, userId);
		}

		OnDocumentDataLoaded(userId, documentId);

		CompleteTask(taskId, TaskResult{OS_OK, documentId, QString()});
	});

	QObject::connect(worker, &QObject::destroyed, thread, &QThread::quit);
	QObject::connect(thread, &QThread::finished, thread, &QObject::deleteLater);

	thread->start();
}


void CDocumentServiceBase::DoOpenDocument(const QByteArray& taskId, const TaskParams& /*params*/)
{
	CompleteTask(taskId, TaskResult{OS_FAILED, QByteArray(), QStringLiteral("Open not supported by base class")});
}


void CDocumentServiceBase::DoSaveDocument(const QByteArray& taskId, const TaskParams& /*params*/)
{
	CompleteTask(taskId, TaskResult{OS_FAILED, QByteArray(), QStringLiteral("Save not supported by base class")});
}


void CDocumentServiceBase::DoCloseDocument(const QByteArray& taskId, const TaskParams& params)
{
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


IDocumentService::OperationStatus CDocumentServiceBase::CloseDocumentInternal(
			const QByteArray& userId,
			const QByteArray& documentId)
{
	QByteArray typeId;
	QByteArray objectId;
	QString name;
	bool isDirty = false;
	WorkingDocument* workingDocumentPtr = nullptr;

	{
		QMutexLocker locker(&m_mutex);
		OperationStatus validationStatus;
		if (!ValidateInputParams(userId, documentId, validationStatus)){
			return validationStatus;
		}
	}

	DocumentClosedNotification notification;
	notification.userId = userId;
	notification.documentId = documentId;

	istd::IChangeable::ChangeSet changeSet(CF_DOCUMENT_CLOSED);
	changeSet.SetChangeInfo(CN_DOCUMENT_CLOSED, QVariant::fromValue(notification));

	{
		istd::CChangeNotifier notifier(this, &changeSet);
		{
			QMutexLocker locker(&m_mutex);
			if (!m_userDocuments.contains(userId) || !m_userDocuments[userId].contains(documentId)){
				return OS_INVALID_DOCUMENT_ID;
			}

			workingDocumentPtr = &m_userDocuments[userId][documentId];

			bool isLastSharedUser = true;
			if (IsSingleCopyMode() && !workingDocumentPtr->objectId.isEmpty()
				&& m_sharedDocuments.contains(workingDocumentPtr->objectId)) {
				SharedDocumentData& shared = m_sharedDocuments[workingDocumentPtr->objectId];
				shared.refCount--;
				isLastSharedUser = (shared.refCount <= 0);
				if (isLastSharedUser) {
					imod::IModel* undoModelPtr = dynamic_cast<imod::IModel*>(shared.undoManagerPtr.GetPtr());
					if (undoModelPtr != nullptr && shared.undoManagerModelId >= 0) {
						m_undoManagerObserver.UnregisterModel(shared.undoManagerModelId);
					}
					m_sharedDocuments.remove(workingDocumentPtr->objectId);
				}
			}

			if (isLastSharedUser) {
				imod::IModel* modelPtr = dynamic_cast<imod::IModel*>(workingDocumentPtr->undoManagerPtr.GetPtr());
				if (modelPtr != nullptr) {
					modelPtr->DetachAllObservers();
				}
			}

			typeId = workingDocumentPtr->typeId;
			objectId = workingDocumentPtr->objectId;
			name = workingDocumentPtr->name;
			isDirty = workingDocumentPtr->isDirty;

			m_userDocuments[userId].remove(documentId);
			if (m_userDocuments[userId].isEmpty()) {
				m_userDocuments.remove(userId);
			}
		}
	}

	for (IDocumentServiceEventHandler* handlerPtr : GetDocumentServiceEventHandlers()){
		if (handlerPtr != nullptr){
			CDocumentClosedEvent event(
				userId,
				documentId,
				typeId,
				name,
				ObjectIdToUrl(objectId),
				isDirty);
			handlerPtr->ProcessEvent(&event);
		}
	}

	return OS_OK;
}


void CDocumentServiceBase::CompleteTask(const QByteArray& taskId, const TaskResult& result)
{
	std::shared_ptr<TaskContext> context;
	{
		QMutexLocker locker(&m_tasksMutex);
		auto it = m_pendingTasks.find(taskId);
		if (it == m_pendingTasks.end()){
			return;
		}
		context = it.value();
	}

	QMutexLocker locker(&context->mutex);
	context->result = result;
	context->isFinished = true;
	context->condition.wakeAll();
}


IDocumentService::OperationStatus CDocumentServiceBase::GetDocumentName(
			const QByteArray& userId,
			const QByteArray& documentId,
			QString& documentName) const
{

	QMutexLocker locker(&m_mutex);

	OperationStatus validationStatus;
	if (!ValidateInputParams(userId, documentId, validationStatus)){
		return validationStatus;
	}

	documentName = m_userDocuments[userId][documentId].name;

	return OS_OK;
}


IDocumentService::OperationStatus CDocumentServiceBase::SetDocumentName(
			const QByteArray& userId,
			const QByteArray& documentId,
			const QString& documentName)
{
	return OS_FAILED;
}


const istd::IChangeable* CDocumentServiceBase::GetDocumentPtr(const QByteArray& userId, const QByteArray& documentId) const
{
	OperationStatus retVal;

	QMutexLocker locker(&m_mutex);

	if (!ValidateInputParams(userId, documentId, retVal)){
		return nullptr;
	}

	const WorkingDocument& document = m_userDocuments[userId][documentId];
	if (document.isLoading) {
		return nullptr;
	}

	return document.objectPtr.GetPtr();
}


IDocumentService::OperationStatus CDocumentServiceBase::GetDocumentData(
			const QByteArray& userId,
			const QByteArray& documentId,
			istd::IChangeableSharedPtr& documentPtr) const
{
	QMutexLocker locker(&m_mutex);
	OperationStatus validationStatus;
	if (!ValidateInputParams(userId, documentId, validationStatus)){
		return validationStatus;
	}

	WorkingDocument& workingDocument = m_userDocuments[userId][documentId];

	if (workingDocument.isLoading) {
		return OS_FAILED;
	}

	documentPtr = CreateObject(workingDocument.typeId);
	if (!documentPtr.IsValid()) {
		return OS_FAILED;
	}

	if (!documentPtr->CopyFrom(*m_userDocuments[userId][documentId].objectPtr)) {
		return OS_FAILED;
	}

	return OS_OK;
}


IDocumentService::OperationStatus CDocumentServiceBase::SetDocumentData(
			const QByteArray& userId,
			const QByteArray& documentId,
			const istd::IChangeable& document)
{
	QMutexLocker locker(&m_mutex);

	OperationStatus validationStatus;
	if (!ValidateInputParams(userId, documentId, validationStatus)){
		return validationStatus;
	}

	WorkingDocument* workingDocumentPtr = &m_userDocuments[userId][documentId];

	if (workingDocumentPtr->isLoading) {
		return OS_FAILED;
	}

	bool isCopySuccessful = workingDocumentPtr->objectPtr->CopyFrom(document);

	return isCopySuccessful ? OS_OK : OS_FAILED;
}


IDocumentService::OperationStatus CDocumentServiceBase::GetDocumentUndoManager(
			const QByteArray& userId,
			const QByteArray& documentId,
			idoc::IUndoManager*& undoManagerPtr) const
{
	undoManagerPtr = nullptr;

	QMutexLocker locker(&m_mutex);
	OperationStatus validationStatus;
	if (!ValidateInputParams(userId, documentId, validationStatus)){
		return validationStatus;
	}

	undoManagerPtr = m_userDocuments[userId][documentId].undoManagerPtr.GetPtr();

	return OS_OK;
}


IDocumentService::OperationStatus CDocumentServiceBase::RegisterDocumentObserver(
			const QByteArray& userId,
			const QByteArray& documentId,
			imod::IObserver& observer)
{
	istd::IChangeableSharedPtr objectPtr;
	{
		QMutexLocker locker(&m_mutex);

		OperationStatus validationStatus;
		if (!ValidateInputParams(userId, documentId, validationStatus)){
			return validationStatus;
		}

		objectPtr = m_userDocuments[userId][documentId].objectPtr;
	}

	imod::IModel* modelPtr = dynamic_cast<imod::IModel*>(objectPtr.GetPtr());
	if (modelPtr == nullptr){
		return OS_FAILED;
	}

	if (modelPtr->IsAttached(&observer)){
		return OS_FAILED;
	}

	bool isObserverAttached = modelPtr->AttachObserver(&observer);

	return isObserverAttached ? OS_OK : OS_FAILED;
}


IDocumentService::OperationStatus CDocumentServiceBase::UnregisterDocumentObserver(
			const QByteArray& userId,
			const QByteArray& documentId,
			imod::IObserver& observer)
{
	istd::IChangeableSharedPtr objectPtr;
	{
		QMutexLocker locker(&m_mutex);

		OperationStatus validationStatus;
		if (!ValidateInputParams(userId, documentId, validationStatus)){
			return validationStatus;
		}

		objectPtr = m_userDocuments[userId][documentId].objectPtr;
	}

	imod::IModel* modelPtr = dynamic_cast<imod::IModel*>(objectPtr.GetPtr());
	if (modelPtr == nullptr){
		return OS_FAILED;
	}

	if (!modelPtr->IsAttached(&observer)){
		return OS_FAILED;
	}

	modelPtr->DetachObserver(&observer);

	return OS_OK;
}


// reimplemented (iser::ISerializable)

bool CDocumentServiceBase::Serialize(iser::IArchive& /*archive*/)
{
	return false;
}


// protected methods

bool CDocumentServiceBase::ValidateInputParams(const QByteArray& userId, const QByteArray& documentId, OperationStatus& status) const
{
	if (!m_userDocuments.contains(userId)){
		status = OS_INVALID_USER_ID;

		return false;
	}

	if (!m_userDocuments[userId].contains(documentId)){
		status = OS_INVALID_DOCUMENT_ID;

		return false;
	}

	return true;
}


int CDocumentServiceBase::GetUndoManagerNextModelId(const QByteArray& userId)
{
	QSet<int> ids;

	if (!m_userDocuments.contains(userId)){
		return -1;
	}

	WorkingDocumentList& documents = m_userDocuments[userId];
	for (const QByteArray& documentId : documents.keys()){
		ids += m_userDocuments[userId][documentId].undoManagerModelId;
	}

	int retVal = 0;
	while (true){
		if (!ids.contains(retVal)){
			return retVal;
		}

		retVal++;
	}
}


void CDocumentServiceBase::InitializeDocumentObservers(
			WorkingDocument& document,
			const QByteArray& userId)
{
	imod::IModel* modelPtr = dynamic_cast<imod::IModel*>(document.objectPtr.GetPtr());
	imod::IModel* undoModelPtr = dynamic_cast<imod::IModel*>(document.undoManagerPtr.GetPtr());
	imod::IObserver* undoObserverPtr = dynamic_cast<imod::IObserver*>(document.undoManagerPtr.GetPtr());

	if (modelPtr != nullptr){
		if (undoObserverPtr != nullptr){
			modelPtr->AttachObserver(undoObserverPtr);
		}

		modelPtr->AttachObserver(this);
	}

	document.undoManagerPtr->StoreDocumentState();

	int undoManagerModelId = GetUndoManagerNextModelId(userId);
	document.undoManagerModelId = undoManagerModelId;
	if (undoModelPtr != nullptr){
		m_undoManagerObserver.RegisterModel(undoModelPtr, undoManagerModelId);
	}
}


CDocumentServiceBase::WorkingDocument* CDocumentServiceBase::FindDocument(
	const QByteArray& userId, const QByteArray& documentId)
{
	if (m_userDocuments.contains(userId)){
		if (m_userDocuments[userId].contains(documentId)){
			return &m_userDocuments[userId][documentId];
		}
	}

	return nullptr;
}


const CDocumentServiceBase::WorkingDocument* CDocumentServiceBase::FindDocument(
			const QByteArray& userId,
			const QByteArray& documentId) const
{
	return const_cast<CDocumentServiceBase*>(this)->FindDocument(userId, documentId);
}


bool CDocumentServiceBase::FindDocument(
			int undoManagerModelId,
			QByteArray& outUserId,
			QByteArray& outDocumentId)
{
	for (const QByteArray& userId : m_userDocuments.keys()){
		WorkingDocumentList& documents = m_userDocuments[userId];
		for (const QByteArray& documentId : documents.keys()){
			if (m_userDocuments[userId][documentId].undoManagerModelId == undoManagerModelId){
				outUserId = userId;
				outDocumentId = documentId;

				return true;
			}
		}
	}

	return false;
}


QUrl CDocumentServiceBase::ObjectIdToUrl(const QByteArray& objectId)
{
	QUrl url;
	url = "collection:///" + objectId;

	return url;
}


IDocumentService::DocumentNotificationPtr CDocumentServiceBase::CreateDocumentNotification(
			const QByteArray& userId,
			const QByteArray& documentId) const
{
	std::shared_ptr<DocumentNotification> retVal;

	if (m_userDocuments.contains(userId) && m_userDocuments[userId].contains(documentId)){
		const WorkingDocument document = m_userDocuments[userId][documentId];

		retVal.reset(new DocumentNotification);
		retVal->userId = userId;
		retVal->documentId = documentId;
		retVal->url = "collection:///" + document.objectId;
		retVal->name = document.name;
		retVal->isDirty = document.isDirty;
	}

	return retVal;
}


void CDocumentServiceBase::OnDocumentDataLoaded(
			const QByteArray& userId,
			const QByteArray& documentId)
{
	DocumentDataLoadedInfo info;
	QByteArray typeId;
	QString name;
	QByteArray objectId;
	bool isDirty = false;

	{
		QMutexLocker locker(&m_mutex);
		WorkingDocument* documentPtr = FindDocument(userId, documentId);
		if (documentPtr == nullptr) {
			return;
		}

		typeId = documentPtr->typeId;
		objectId = documentPtr->objectId;
		name = documentPtr->name;
		isDirty = documentPtr->isDirty;
	}

	info.userId = userId;
	info.documentId = documentId;
	info.typeId = typeId;
	info.url = ObjectIdToUrl(objectId);
	info.name = name;
	info.isDirty = isDirty;
	info.isLoading = false;

	istd::IChangeable::ChangeSet changeSet(CF_DOCUMENT_DATA_LOADED);
	changeSet.SetChangeInfo(CN_DOCUMENT_DATA_LOADED, QVariant::fromValue(info));

	{
		istd::CChangeNotifier notifier(this, &changeSet);
		{
			QMutexLocker locker(&m_mutex);
			WorkingDocument* documentPtr = FindDocument(userId, documentId);
			if (documentPtr != nullptr) {
				documentPtr->isLoading = false;
			}
		}
	}

	for (IDocumentServiceEventHandler* handlerPtr : GetDocumentServiceEventHandlers()){
		if (handlerPtr != nullptr){
			CDocumentDataLoadedEvent event(
						userId,
						documentId,
						typeId,
						name,
						ObjectIdToUrl(objectId),
						isDirty);
			handlerPtr->ProcessEvent(&event);
		}
	}
}


void CDocumentServiceBase::OnUndoManagerChanged(int modelId)
{
	if (IsSingleCopyMode()) {
		QByteArray sharedObjectId;
		UserDocumentPairList docs;

		{
			QMutexLocker locker(&m_mutex);
			// Find the shared document by modelId
			for (auto it = m_sharedDocuments.constBegin(); it != m_sharedDocuments.constEnd(); ++it) {
				if (it.value().undoManagerModelId == modelId) {
					sharedObjectId = it.key();
					break;
				}
			}

			if (!sharedObjectId.isEmpty()) {
				docs = FindDocumentsByObjectId(sharedObjectId);
			}
		}

		if (!sharedObjectId.isEmpty()) {
			for (const UserDocumentPair& pair : docs) {
				QByteArray docTypeId;
				QString docName;
				QByteArray docObjectId;
				bool newIsDirty = false;
				idoc::IUndoManagerSharedPtr docUndoManagerPtr;

				{
					QMutexLocker locker(&m_mutex);
					WorkingDocument* documentPtr = FindDocument(pair.first, pair.second);
					if (documentPtr == nullptr) {
						continue;
					}

					docTypeId = documentPtr->typeId;
					docName = documentPtr->name;
					docObjectId = documentPtr->objectId;
					docUndoManagerPtr = documentPtr->undoManagerPtr;
					newIsDirty = documentPtr->undoManagerPtr->GetDocumentChangeFlag() != idoc::IDocumentStateComparator::DCF_EQUAL;
				}

				DocumentUndoRedoNotification notification;
				notification.userId = pair.first;
				notification.documentId = pair.second;
				notification.isDirty = newIsDirty;
				if (docUndoManagerPtr.IsValid()) {
					notification.availableUndoSteps = docUndoManagerPtr->GetAvailableUndoSteps();
					notification.availableRedoSteps = docUndoManagerPtr->GetAvailableRedoSteps();
				}

				istd::IChangeable::ChangeSet changeSet(CF_DOCUMENT_UNDO_CHANGED);
				changeSet.SetChangeInfo(CN_DOCUMENT_UNDO_CHANGED, QVariant::fromValue(notification));

				{
					istd::CChangeNotifier notifier(this, &changeSet);
					{
						QMutexLocker locker(&m_mutex);
						WorkingDocument* documentPtr = FindDocument(pair.first, pair.second);
						if (documentPtr != nullptr) {
							documentPtr->isDirty = newIsDirty;
						}
					}
				}

				for (IDocumentServiceEventHandler* handlerPtr : GetDocumentServiceEventHandlers()){
					if (handlerPtr != nullptr){
						CDocumentUndoRedoChangedEvent event(
							pair.first,
							pair.second,
							docTypeId,
							docName,
							ObjectIdToUrl(docObjectId),
							newIsDirty,
							*docUndoManagerPtr);
						handlerPtr->ProcessEvent(&event);
					}
				}
			}

			return;
		}
	}

	QByteArray userId;
	QByteArray documentId;
	QByteArray docTypeId;
	QString docName;
	QByteArray docObjectId;
	bool newIsDirty = false;
	idoc::IUndoManagerSharedPtr docUndoManagerPtr;

	{
		QMutexLocker locker(&m_mutex);

		if (!FindDocument(modelId, userId, documentId)){
			Q_ASSERT(false);
			return;
		}

		WorkingDocument* documentPtr = FindDocument(userId, documentId);
		Q_ASSERT(documentPtr != nullptr);
		if (documentPtr == nullptr){
			return;
		}

		Q_ASSERT(documentPtr->undoManagerPtr.IsValid());
		docTypeId = documentPtr->typeId;
		docName = documentPtr->name;
		docObjectId = documentPtr->objectId;
		docUndoManagerPtr = documentPtr->undoManagerPtr;
		newIsDirty = documentPtr->undoManagerPtr->GetDocumentChangeFlag() != idoc::IDocumentStateComparator::DCF_EQUAL;
	}

	DocumentUndoRedoNotification notification;
	notification.userId = userId;
	notification.documentId = documentId;
	notification.isDirty = newIsDirty;
	if (docUndoManagerPtr.IsValid()) {
		notification.availableUndoSteps = docUndoManagerPtr->GetAvailableUndoSteps();
		notification.availableRedoSteps = docUndoManagerPtr->GetAvailableRedoSteps();
	}

	istd::IChangeable::ChangeSet changeSet(CF_DOCUMENT_UNDO_CHANGED);
	changeSet.SetChangeInfo(CN_DOCUMENT_UNDO_CHANGED, QVariant::fromValue(notification));

	{
		istd::CChangeNotifier notifier(this, &changeSet);
		{
			QMutexLocker locker(&m_mutex);
			WorkingDocument* documentPtr = FindDocument(userId, documentId);
			if (documentPtr != nullptr) {
				documentPtr->isDirty = newIsDirty;
			}
		}
	}

	for (IDocumentServiceEventHandler* handlerPtr : GetDocumentServiceEventHandlers()){
		if (handlerPtr != nullptr){
			CDocumentUndoRedoChangedEvent event(
						userId,
						documentId,
						docTypeId,
						docName,
						ObjectIdToUrl(docObjectId),
						newIsDirty,
						*docUndoManagerPtr);
			handlerPtr->ProcessEvent(&event);
		}
	}
}


QString CDocumentServiceBase::GetDefaultDocumentName(const WorkingDocument& /*document*/) const
{
	return QString();
}


bool CDocumentServiceBase::HasDocumentNameProvider(const QByteArray& /*typeId*/) const
{
	return false;
}


bool CDocumentServiceBase::ValidateDocumentData(
	const WorkingDocument& /*document*/,
	OperationStatus& status,
	QString* errorMessage) const
{
	status = OS_OK;
	if (errorMessage != nullptr) {
		errorMessage->clear();
	}

	return true;
}


QList<imtdoc::IDocumentServiceEventHandler*> CDocumentServiceBase::GetDocumentServiceEventHandlers() const
{
	return {};
}


QString CDocumentServiceBase::GetInvalidDocumentMessage()
{
	return QStringLiteral("Document data is invalid");
}


bool CDocumentServiceBase::IsSingleCopyMode() const
{
	return false;
}


CDocumentServiceBase::UserDocumentPairList CDocumentServiceBase::FindDocumentsByObjectId(
	const QByteArray& objectId) const
{
	UserDocumentPairList result;

	for (auto userIt = m_userDocuments.constBegin(); userIt != m_userDocuments.constEnd(); ++userIt) {
		for (auto docIt = userIt.value().constBegin(); docIt != userIt.value().constEnd(); ++docIt) {
			if (docIt.value().objectId == objectId) {
				result.append(qMakePair(userIt.key(), docIt.key()));
			}
		}
	}

	return result;
}


// reimplemented (imod::CMultiModelObserverBase)

void CDocumentServiceBase::OnUpdate(imod::IModel* modelPtr, const istd::IChangeable::ChangeSet& changeSet)
{
	for (const QByteArray& userId : m_userDocuments.keys()){
		WorkingDocumentList& documents = m_userDocuments[userId];
		for (const QByteArray& documentId : documents.keys()){
			istd::IChangeable* changeablePtr = dynamic_cast<istd::IChangeable*>(modelPtr);

			if (documents[documentId].objectPtr.GetPtr() == changeablePtr){
				for (IDocumentServiceEventHandler* handlerPtr : GetDocumentServiceEventHandlers()){
					if (handlerPtr != nullptr){
						WorkingDocument& workingDocument = documents[documentId];
						CDocumentChangedEvent event(
									userId,
									documentId,
									workingDocument.typeId,
									workingDocument.name,
									ObjectIdToUrl(workingDocument.objectId),
									workingDocument.isDirty,
									*changeablePtr,
									changeSet);
						handlerPtr->ProcessEvent(&event);
					}
				}

				if (!IsSingleCopyMode()) {
					return;
				}
			}
		}
	}
}


// public methods of the embedded class UndoManagerObserver

CDocumentServiceBase::UndoManagerObserver::UndoManagerObserver(CDocumentServiceBase& parent)
	:m_parent(parent)
{
}


// protected methods of the embedded class UndoManagerObserver

void CDocumentServiceBase::UndoManagerObserver::OnModelChanged(int modelId, const istd::IChangeable::ChangeSet& /*changeSet*/)
{
	m_parent.OnUndoManagerChanged(modelId);
}


} // namespace imtdoc
