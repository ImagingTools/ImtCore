// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtdoc/CDocumentServiceBase.h>


// STL includes
#include <functional>

// Qt includes
#include <QtCore/QCoreApplication>
#include <QtCore/QUuid>
#include <QtCore/QThread>

// ACF includes
#include <imod/IModel.h>
#include <imod/IObserver.h>
#include <istd/CChangeNotifier.h>

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
#include <imtdoc/IPersistentUndoManager.h>


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

	QList<QPointer<QThread>> workerThreads;
	{
		QMutexLocker locker(&m_workerThreadsMutex);
		workerThreads = m_workerThreads;
	}

	for (const QPointer<QThread>& threadPtr : workerThreads){
		if (threadPtr.isNull()){
			continue;
		}

		if (threadPtr.data() == QThread::currentThread()){
			continue;
		}

		threadPtr->quit();
		threadPtr->wait();
	}

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

	QCoreApplication::sendPostedEvents(nullptr, QEvent::MetaCall);
	QCoreApplication::processEvents(QEventLoop::AllEvents);
}


// reimplemented (imtdoc::IDocumentService)

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

	for (const QByteArray& id : GetUserDocumentIds(userId)){
		const WorkingDocument* workingDocumentPtr = FindDocument(userId, id);
		if (workingDocumentPtr == nullptr){
			continue;
		}

		DocumentListItem info;
		info.documentId = id;
		info.typeId = workingDocumentPtr->typeId;
		info.url = workingDocumentPtr->url;
		info.name = workingDocumentPtr->name;
		info.isDirty = workingDocumentPtr->isDirty;
		info.hasNameProvider = HasDocumentNameProvider(workingDocumentPtr->typeId);
		info.isLoading = workingDocumentPtr->isLoading;
		info.singleDocumentInstance = workingDocumentPtr->singleDocumentInstance;

		list.append(info);
	}

	return list;
}


// protected methods — task dispatch

void CDocumentServiceBase::DoCreateNewDocument(const QByteArray& taskId, const TaskParams& params)
{
	idoc::IUndoManagerSharedPtr undoManagerPtr;
	undoManagerPtr.FromUnique(std::move(CreateUndoManager()));
	if (!undoManagerPtr.IsValid()){
		CompleteTask(taskId, TaskResult{OS_FAILED, QByteArray(), QStringLiteral("Failed to create undo manager")});
		return;
	}

	QByteArray documentId = QUuid::createUuid().toByteArray(QUuid::WithoutBraces);

	istd::CChangeNotifier notifier(this);

	QString documentName;
	{
		QMutexLocker locker(&m_mutex);
		WorkingDocument& doc = InsertDocument(params.userId, documentId);
		doc.typeId = params.documentTypeId;
		doc.undoManagerPtr = undoManagerPtr;
		doc.isDirty = true;
		doc.name = params.documentName;
		doc.isLoading = true;
		doc.singleDocumentInstance = params.singleDocumentInstance;
		documentName = doc.name;
	}

	for (IDocumentServiceEventHandler* handlerPtr : GetDocumentServiceEventHandlers()){
		if (handlerPtr != nullptr){
			CDocumentCreatedEvent event(
						params.userId,
						documentId,
						params.documentTypeId,
						documentName,
						QUrl(),
						true);
			handlerPtr->ProcessEvent(&event);
		}
	}

	// Create object asynchronously in a separate thread
	QByteArray userId = params.userId;
	QByteArray documentTypeId = params.documentTypeId;
	const istd::IChangeable* defaultDataPtr = params.defaultDataPtr;
	const iprm::IParamsSet* initParamsPtr = params.initParamsPtr;
	QThread* thread = new QThread();
	QObject* worker = new QObject();
	worker->moveToThread(thread);
	{
		QMutexLocker locker(&m_workerThreadsMutex);
		m_workerThreads.append(QPointer<QThread>(thread));
	}

	std::weak_ptr<std::atomic<bool>> aliveGuard(m_isAlive);
	QObject::connect(
		thread,
		&QThread::started,
		worker,
		std::bind(
					&CDocumentServiceBase::OnCreateDocumentThreadStarted,
					this,
					aliveGuard,
					documentTypeId,
					userId,
					documentId,
					taskId,
					worker,
					defaultDataPtr));

	// Initialize observers and fire events in the main thread after background work completes
	QObject::connect(
		thread,
		&QThread::finished,
		QCoreApplication::instance(),
		std::bind(
					&CDocumentServiceBase::OnCreateDocumentThreadFinished,
					this,
					aliveGuard,
					userId,
					documentId,
					taskId,
					initParamsPtr));

	QObject::connect(worker, &QObject::destroyed, thread, &QThread::quit, Qt::DirectConnection);
	QObject::connect(thread, &QThread::finished, thread, &QObject::deleteLater);

	thread->start();
}


void CDocumentServiceBase::OnCreateDocumentThreadStarted(
	const std::weak_ptr<std::atomic<bool>>& aliveGuard,
	const QByteArray& documentTypeId,
	const QByteArray& userId,
	const QByteArray& documentId,
	const QByteArray& taskId,
	QObject* worker,
	const istd::IChangeable* defaultDataPtr)
{
	auto isAlive = aliveGuard.lock();
	if (!isAlive || !isAlive->load()){
		CompleteTask(taskId, TaskResult{OS_FAILED, QByteArray(), QStringLiteral("Service destroyed")});
		worker->deleteLater();
		return;
	}

	istd::IChangeableSharedPtr objectPtr = CreateObject(documentTypeId);

	isAlive = aliveGuard.lock();
	if (!isAlive || !isAlive->load()){
		CompleteTask(taskId, TaskResult{OS_FAILED, QByteArray(), QStringLiteral("Service destroyed")});
		worker->deleteLater();
		return;
	}

	{
		QMutexLocker locker(&m_mutex);
		WorkingDocument* docPtr = FindDocument(userId, documentId);

		if (docPtr != nullptr){
			if (objectPtr.IsValid()){
				if (defaultDataPtr != nullptr){
					bool retVal = objectPtr->CopyFrom(*defaultDataPtr);
					Q_ASSERT_X(retVal, Q_FUNC_INFO, "Unable to copy from default data");

					if (!retVal){
						docPtr->isLoading = false;
						CompleteTask(taskId, TaskResult{ OS_FAILED, QByteArray(), QStringLiteral("Object creation failed") });
						worker->deleteLater();
						return;
					}
				}

				docPtr->objectPtr = objectPtr;
			}
			else{
				// Creation failed - close the document and notify client
				docPtr->isLoading = false;
				CloseDocumentInternal(userId, documentId);
				CompleteTask(taskId, TaskResult{ OS_FAILED, QByteArray(), QStringLiteral("Object creation failed") });
				worker->deleteLater();
				return;
			}
		}
	}

	worker->deleteLater();
}


void CDocumentServiceBase::OnCreateDocumentThreadFinished(
	const std::weak_ptr<std::atomic<bool>>& aliveGuard,
	const QByteArray& userId,
	const QByteArray& documentId,
	const QByteArray& taskId,
	const iprm::IParamsSet* initParamsPtr)
{
	auto isAlive = aliveGuard.lock();
	if (!isAlive || !isAlive->load()){
		return;
	}

	{
		QMutexLocker locker(&m_mutex);
		WorkingDocument* docPtr = FindDocument(userId, documentId);

		if (docPtr == nullptr || !docPtr->objectPtr.IsValid() || !docPtr->isLoading){
			return;
		}

		QString errorMessage;
		if (!OnDocumentCreated(docPtr->typeId, initParamsPtr, *docPtr->objectPtr, errorMessage)){
			docPtr->isLoading = false;
			CloseDocumentInternal(userId, documentId);
			if (errorMessage.isEmpty()){
				errorMessage = QStringLiteral("Document initialization failed");
			}
			CompleteTask(taskId, TaskResult{OS_FAILED, QByteArray(), errorMessage});
			return;
		}

		InitializeDocumentObservers(*docPtr, userId, documentId);
	}

	OnDocumentDataLoaded(userId, documentId);

	CompleteTask(taskId, TaskResult{OS_OK, documentId, QString()});
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
	QUrl url;
	QString name;
	bool isDirty = false;

	{
		QMutexLocker locker(&m_mutex);

		OperationStatus validationStatus;
		if (!ValidateInputParams(userId, documentId, validationStatus)){
			return validationStatus;
		}

		istd::CChangeNotifier notifier(this);

		WorkingDocument* workingDocumentPtr = &m_documents[documentId];

		const bool isLastUser = GetDocumentUserIds(documentId).count() <= 1;
		if (isLastUser){
			if (workingDocumentPtr->undoManagerModelId >= 0){
				m_undoManagerObserver.UnregisterModel(workingDocumentPtr->undoManagerModelId);
				workingDocumentPtr->undoManagerModelId = -1;
			}

			imod::IModel* modelPtr = dynamic_cast<imod::IModel*>(workingDocumentPtr->undoManagerPtr.GetPtr());
			if (modelPtr != nullptr){
				modelPtr->DetachAllObservers();
			}
		}

		typeId = workingDocumentPtr->typeId;
		url = workingDocumentPtr->url;
		name = workingDocumentPtr->name;
		isDirty = workingDocumentPtr->isDirty;

		RemoveDocument(userId, documentId);
	}

	for (IDocumentServiceEventHandler* handlerPtr : GetDocumentServiceEventHandlers()){
		if (handlerPtr != nullptr){
			CDocumentClosedEvent event(
				userId,
				documentId,
				typeId,
				name,
				url,
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

	documentName = m_documents[documentId].name;

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

	const WorkingDocument& document = m_documents[documentId];
	if (document.isLoading){
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

	WorkingDocument& workingDocument = m_documents[documentId];

	if (workingDocument.isLoading){
		return OS_FAILED;
	}

	documentPtr = CreateObject(workingDocument.typeId);
	if (!documentPtr.IsValid()){
		return OS_FAILED;
	}

	if (!documentPtr->CopyFrom(*workingDocument.objectPtr)){
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

	WorkingDocument* workingDocumentPtr = &m_documents[documentId];

	if (workingDocumentPtr->isLoading){
		return OS_FAILED;
	}

	bool isCopySuccessful = workingDocumentPtr->objectPtr->CopyFrom(document);

	return isCopySuccessful ? OS_OK : OS_FAILED;
}


IDocumentService::OperationStatus CDocumentServiceBase::ExecuteDocumentMethod(
			const QByteArray& userId,
			const QByteArray& documentId,
			const CDocumentMethod& method)
{
	QMutexLocker locker(&m_mutex);

	OperationStatus validationStatus;
	if (!ValidateInputParams(userId, documentId, validationStatus)){
		return validationStatus;
	}

	WorkingDocument& workingDocument = m_documents[documentId];

	if (workingDocument.isLoading){
		return OS_FAILED;
	}

	return method(*workingDocument.objectPtr) ? OS_OK : OS_FAILED;
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

	undoManagerPtr = m_documents[documentId].undoManagerPtr.GetPtr();

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

		objectPtr = m_documents[documentId].objectPtr;
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

		objectPtr = m_documents[documentId].objectPtr;
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
	if (!m_userDocumentIds.contains(userId)){
		status = OS_INVALID_USER_ID;

		return false;
	}

	if (FindDocument(userId, documentId) == nullptr){
		status = OS_INVALID_DOCUMENT_ID;

		return false;
	}

	return true;
}


int CDocumentServiceBase::GetUndoManagerNextModelId(const QByteArray& /*userId*/)
{
	QSet<int> ids;

	// Collect model IDs from ALL documents to avoid collisions:
	// m_undoManagerObserver is shared globally.
	for (auto docIt = m_documents.constBegin(); docIt != m_documents.constEnd(); ++docIt){
		ids += docIt.value().undoManagerModelId;
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
			const QByteArray& userId,
			const QByteArray& documentId)
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

	imtdoc::IPersistentUndoManager* persistentUndoManagerPtr = dynamic_cast<imtdoc::IPersistentUndoManager*>(document.undoManagerPtr.GetPtr());
	if (persistentUndoManagerPtr != nullptr){
		persistentUndoManagerPtr->InitializeDocumentContext(documentId, document.typeId);
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
	if (!m_userDocumentIds.value(userId).contains(documentId)){
		return nullptr;
	}

	auto it = m_documents.find(documentId);
	if (it == m_documents.end()){
		return nullptr;
	}

	return &it.value();
}


const CDocumentServiceBase::WorkingDocument* CDocumentServiceBase::FindDocument(
			const QByteArray& userId,
			const QByteArray& documentId) const
{
	return const_cast<CDocumentServiceBase*>(this)->FindDocument(userId, documentId);
}


bool CDocumentServiceBase::FindDocument(int undoManagerModelId, QByteArray& outDocumentId) const
{
	for (auto documentIt = m_documents.constBegin(); documentIt != m_documents.constEnd(); ++documentIt){
		if (documentIt.value().undoManagerModelId == undoManagerModelId){
			outDocumentId = documentIt.key();

			return true;
		}
	}

	return false;
}


CDocumentServiceBase::WorkingDocument& CDocumentServiceBase::InsertDocument(
			const QByteArray& userId,
			const QByteArray& documentId)
{
	QByteArrayList& userDocumentIds = m_userDocumentIds[userId];
	if (!userDocumentIds.contains(documentId)){
		userDocumentIds.append(documentId);
	}

	return m_documents[documentId];
}


void CDocumentServiceBase::RemoveDocument(const QByteArray& userId, const QByteArray& documentId)
{
	auto userIt = m_userDocumentIds.find(userId);
	if (userIt == m_userDocumentIds.end() || userIt.value().removeAll(documentId) == 0){
		return;
	}

	if (userIt.value().isEmpty()){
		m_userDocumentIds.erase(userIt);
	}

	if (GetDocumentUserIds(documentId).isEmpty()){
		m_documents.remove(documentId);
	}
}


void CDocumentServiceBase::ClearDocuments()
{
	m_documents.clear();
	m_userDocumentIds.clear();
}


QByteArrayList CDocumentServiceBase::GetUserDocumentIds(const QByteArray& userId) const
{
	return m_userDocumentIds.value(userId);
}


QByteArrayList CDocumentServiceBase::GetDocumentUserIds(const QByteArray& documentId) const
{
	QByteArrayList retVal;

	for (auto userIt = m_userDocumentIds.constBegin(); userIt != m_userDocumentIds.constEnd(); ++userIt){
		if (userIt.value().contains(documentId)){
			retVal.append(userIt.key());
		}
	}

	return retVal;
}


QUrl CDocumentServiceBase::ObjectIdToUrl(const QByteArray& objectId)
{
	QUrl url;
	url = "collection:///" + objectId;

	return url;
}


QByteArray CDocumentServiceBase::UrlToObjectId(const QUrl& url)
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


bool CDocumentServiceBase::IsDocumentDirty(const WorkingDocument& document)
{
	if (UrlToObjectId(document.url).isEmpty()){
		return true;
	}

	return document.undoManagerPtr.IsValid()
				&& document.undoManagerPtr->GetDocumentChangeFlag() != idoc::IDocumentStateComparator::DCF_EQUAL;
}


void CDocumentServiceBase::OnDocumentDataLoaded(
			const QByteArray& userId,
			const QByteArray& documentId)
{
	QByteArray typeId;
	QString name;
	QUrl url;
	bool isDirty = false;

	{
		QMutexLocker locker(&m_mutex);
		WorkingDocument* documentPtr = FindDocument(userId, documentId);
		if (documentPtr == nullptr){
			return;
		}

		typeId = documentPtr->typeId;
		url = documentPtr->url;
		name = documentPtr->name;
		isDirty = documentPtr->isDirty;
	}

	{
		QMutexLocker locker(&m_mutex);
		WorkingDocument* documentPtr = FindDocument(userId, documentId);
		if (documentPtr != nullptr){
			documentPtr->isLoading = false;
		}
	}

	for (IDocumentServiceEventHandler* handlerPtr : GetDocumentServiceEventHandlers()){
		if (handlerPtr != nullptr){
			CDocumentDataLoadedEvent event(
						userId,
						documentId,
						typeId,
						name,
						url,
						isDirty);
			handlerPtr->ProcessEvent(&event);
		}
	}
}


void CDocumentServiceBase::OnUndoManagerChanged(int modelId)
{
	QByteArray documentId;
	QByteArrayList userIds;
	QByteArray docTypeId;
	QString docName;
	QUrl docUrl;
	bool newIsDirty = false;
	idoc::IUndoManagerSharedPtr docUndoManagerPtr;

	{
		QMutexLocker locker(&m_mutex);

		if (!FindDocument(modelId, documentId)){
			Q_ASSERT(false);
			return;
		}

		WorkingDocument& document = m_documents[documentId];
		Q_ASSERT(document.undoManagerPtr.IsValid());

		newIsDirty = IsDocumentDirty(document);
		document.isDirty = newIsDirty;

		userIds = GetDocumentUserIds(documentId);
		docTypeId = document.typeId;
		docName = document.name;
		docUrl = document.url;
		docUndoManagerPtr = document.undoManagerPtr;
	}

	for (const QByteArray& userId : userIds){
		for (IDocumentServiceEventHandler* handlerPtr : GetDocumentServiceEventHandlers()){
			if (handlerPtr != nullptr){
				CDocumentUndoRedoChangedEvent event(
							userId,
							documentId,
							docTypeId,
							docName,
							docUrl,
							newIsDirty,
							*docUndoManagerPtr);
				handlerPtr->ProcessEvent(&event);
			}
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
			QString* errorMessage,
			const imtbase::IOperationContext* /*operationContextPtr*/) const
{
	status = OS_OK;
	if (errorMessage != nullptr){
		errorMessage->clear();
	}

	return true;
}


QList<imtdoc::IDocumentServiceEventHandler*> CDocumentServiceBase::GetDocumentServiceEventHandlers() const
{
	QMutexLocker locker(&m_mutex);
	return m_registeredEventHandlers;
}


void CDocumentServiceBase::RegisterEventHandler(IDocumentServiceEventHandler& handler)
{
	QMutexLocker locker(&m_mutex);

	if (!m_registeredEventHandlers.contains(&handler)){
		m_registeredEventHandlers.append(&handler);
	}
}


void CDocumentServiceBase::UnregisterEventHandler(IDocumentServiceEventHandler& handler)
{
	QMutexLocker locker(&m_mutex);

	m_registeredEventHandlers.removeAll(&handler);
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

	for (auto userIt = m_userDocumentIds.constBegin(); userIt != m_userDocumentIds.constEnd(); ++userIt){
		for (const QByteArray& documentId : userIt.value()){
			auto documentIt = m_documents.constFind(documentId);
			if (documentIt != m_documents.constEnd() && UrlToObjectId(documentIt.value().url) == objectId){
				result.append(qMakePair(userIt.key(), documentId));
			}
		}
	}

	return result;
}


// reimplemented (imod::CMultiModelObserverBase)

void CDocumentServiceBase::OnUpdate(imod::IModel* modelPtr, const istd::IChangeable::ChangeSet& changeSet)
{
	istd::IChangeable* changeablePtr = dynamic_cast<istd::IChangeable*>(modelPtr);

	// Iterate over a copy: event handlers may open or close documents
	const UserDocumentIdsMap userDocumentIds = m_userDocumentIds;
	for (auto userIt = userDocumentIds.constBegin(); userIt != userDocumentIds.constEnd(); ++userIt){
		const QByteArray& userId = userIt.key();
		for (const QByteArray& documentId : userIt.value()){
			const WorkingDocument* workingDocumentPtr = FindDocument(userId, documentId);
			if (workingDocumentPtr == nullptr || workingDocumentPtr->objectPtr.GetPtr() != changeablePtr){
				continue;
			}

			const WorkingDocument workingDocument = *workingDocumentPtr;
			for (IDocumentServiceEventHandler* handlerPtr : GetDocumentServiceEventHandlers()){
				if (handlerPtr != nullptr){
					CDocumentChangedEvent event(
								userId,
								documentId,
								workingDocument.typeId,
								workingDocument.name,
								workingDocument.url,
								workingDocument.isDirty,
								*changeablePtr,
								changeSet);
					handlerPtr->ProcessEvent(&event);
				}
			}

			if (!IsSingleCopyMode()){
				return;
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
