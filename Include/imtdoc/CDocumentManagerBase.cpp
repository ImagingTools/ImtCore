// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtdoc/CDocumentManagerBase.h>


// Qt includes
#include <QtCore/QCoreApplication>
#include <QtCore/QDeadlineTimer>
#include <QtCore/QUuid>
#include <QtCore/QThread>

// STL includes
#include <algorithm>


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

CDocumentManagerBase::CDocumentManagerBase()
	:m_undoManagerObserver(*this)
	,m_isAlive(std::make_shared<std::atomic<bool>>(true))
{
}


CDocumentManagerBase::~CDocumentManagerBase()
{
	m_isAlive->store(false);
}


// reimplemented (imtdoc::IDocumentManager)

IDocumentManager::DocumentList CDocumentManagerBase::GetOpenedDocumentList(
	const QByteArray& userId) const
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


QByteArray CDocumentManagerBase::CreateNewDocument(
	const QByteArray& userId,
	const QByteArray& documentTypeId,
	const QByteArray& proposedSourceDocumentId)
{
	// Base implementation creates a blank document; source-based initialization is handled by derived managers if needed.
	Q_UNUSED(proposedSourceDocumentId);

	QByteArray retVal;

	idoc::IUndoManagerSharedPtr undoManagerPtr = CreateUndoManager();
	if (!undoManagerPtr.IsValid()) {
		return retVal;
	}

	retVal = QUuid::createUuid().toByteArray(QUuid::WithoutBraces);

	QString documentName;
	{
		QMutexLocker locker(&m_mutex);
		WorkingDocument& doc = m_userDocuments[userId][retVal];
		doc.typeId = documentTypeId;
		doc.undoManagerPtr = undoManagerPtr;
		doc.isDirty = false;
		doc.name = "";
		doc.isLoading = true;
		documentName = doc.name;
	}

	{
		NewDocumentCreatedInfo info;
		info.userId = userId;
		info.documentId = retVal;
		info.typeId = documentTypeId;
		info.name = documentName;
		info.isDirty = false;

		istd::IChangeable::ChangeSet changeSet(CF_NEW_DOCUMENT_CREATED);
		changeSet.SetChangeInfo(CN_NEW_DOCUMENT_CREATED, QVariant::fromValue(info));
		istd::CChangeNotifier notifier(this, &changeSet);
	}

	for (IDocumentManagerEventHandler* handlerPtr : GetDocumentManagerEventHandlers()){
		if (handlerPtr != nullptr){
			CDocumentCreatedEvent event(
				userId,
				retVal,
				documentTypeId,
				documentName,
				QUrl(),
				false);
			handlerPtr->ProcessEvent(&event);
		}
	}

	// Create object asynchronously in a separate thread
	QByteArray documentId = retVal;
	QThread* thread = new QThread();
	QObject* worker = new QObject();
	worker->moveToThread(thread);

	std::weak_ptr<std::atomic<bool>> aliveGuard(m_isAlive);
	QObject::connect(thread, &QThread::started, worker, [this, aliveGuard, documentTypeId, userId, documentId, worker]() {
		auto isAlive = aliveGuard.lock();
		if (!isAlive || !isAlive->load()) {
			worker->deleteLater();
			return;
		}

		istd::IChangeableSharedPtr objectPtr = CreateObject(documentTypeId);

		isAlive = aliveGuard.lock();
		if (!isAlive || !isAlive->load()) {
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


QByteArray CDocumentManagerBase::OpenDocument(const QByteArray& /*userId*/, const QUrl& /*url*/)
{
	return QByteArray();
}


IDocumentManager::OperationStatus CDocumentManagerBase::IsDocumentReady(
	const QByteArray& userId, const QByteArray& documentId) const
{
	QMutexLocker locker(&m_mutex);

	OperationStatus validationStatus;
	if (!ValidateInputParams(userId, documentId, validationStatus)){
		return validationStatus;
	}

	const WorkingDocument& document = m_userDocuments[userId][documentId];
	return document.isLoading ? OS_LOADING : OS_OK;
}


IDocumentManager::OperationStatus CDocumentManagerBase::WaitForDocumentReady(
	const QByteArray& userId, const QByteArray& documentId, int timeoutMs)
{
	// Fast path: if the document is already ready, missing, or this manager
	// loads documents synchronously, return immediately.
	{
		OperationStatus status = IsDocumentReady(userId, documentId);
		if (status != OS_LOADING) {
			return status;
		}
	}

	if (!IsAsynchronousDocumentCreation() && !IsAsynchronousDocumentOpen()) {
		// The manager does not load asynchronously; nothing to wait for.
		// IsDocumentReady() above already returned OS_LOADING, which would
		// be misleading for a synchronous manager - treat it as ready.
		return OS_OK;
	}

	QDeadlineTimer deadline = (timeoutMs < 0)
		? QDeadlineTimer(QDeadlineTimer::Forever)
		: QDeadlineTimer(timeoutMs);

	// Cap the per-iteration wait so we periodically recheck the document
	// state. This also closes the race between IsDocumentReady() (which
	// takes m_mutex) and acquiring m_loadingWaitMutex without locking both
	// simultaneously, which would invert the lock order taken by
	// OnDocumentDataLoaded() / CloseDocument().
	const unsigned long maxWaitChunkMs = 100;

	while (true) {
		OperationStatus status = IsDocumentReady(userId, documentId);
		if (status != OS_LOADING) {
			return status;
		}

		if (deadline.hasExpired()) {
			qWarning("CDocumentManagerBase::WaitForDocumentReady: timeout while waiting for document '%s' of user '%s' to finish loading",
				documentId.constData(), userId.constData());
			return OS_LOADING;
		}

		unsigned long waitMs = maxWaitChunkMs;
		if (timeoutMs >= 0) {
			qint64 remaining = deadline.remainingTime();
			if (remaining <= 0) {
				continue; // re-check expiry at the top
			}
			waitMs = std::min<unsigned long>(maxWaitChunkMs, static_cast<unsigned long>(remaining));
		}

		QMutexLocker waitLocker(&m_loadingWaitMutex);
		m_loadingWaitCondition.wait(&m_loadingWaitMutex, waitMs);
	}
}


IDocumentManager::OperationStatus CDocumentManagerBase::GetDocumentName(const QByteArray& userId, const QByteArray& documentId, QString& documentName) const
{

	QMutexLocker locker(&m_mutex);

	OperationStatus validationStatus;
	if (!ValidateInputParams(userId, documentId, validationStatus)){
		return validationStatus;
	}

	documentName = m_userDocuments[userId][documentId].name;

	return OS_OK;
}


IDocumentManager::OperationStatus CDocumentManagerBase::SetDocumentName(const QByteArray& userId, const QByteArray& documentId, const QString& documentName)
{
	return OS_FAILED;
}


const istd::IChangeable* CDocumentManagerBase::GetDocumentPtr(const QByteArray& userId, const QByteArray& documentId) const
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


IDocumentManager::OperationStatus CDocumentManagerBase::GetDocumentData(const QByteArray& userId, const QByteArray& documentId, istd::IChangeableSharedPtr& documentPtr) const
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


IDocumentManager::OperationStatus CDocumentManagerBase::SetDocumentData(const QByteArray& userId, const QByteArray& documentId, const istd::IChangeable& document)
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


IDocumentManager::OperationStatus CDocumentManagerBase::SaveDocument(
	const QByteArray& /*userId*/,
	const QByteArray& /*documentId*/,
	const QString& /*documentName*/,
	QString* /*errorMessage*/)
{
	return OS_FAILED;
}


IDocumentManager::OperationStatus CDocumentManagerBase::CloseDocument(
	const QByteArray& userId, const QByteArray& documentId)
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

	{
		// Wake any threads waiting in WaitForDocumentReady() — the document
		// is gone now, so they should re-evaluate and return OS_INVALID_*.
		QMutexLocker waitLocker(&m_loadingWaitMutex);
		m_loadingWaitCondition.wakeAll();
	}

	{
		DocumentClosedNotification notification;
		notification.userId = userId;
		notification.documentId = documentId;

		istd::IChangeable::ChangeSet changeSet(CF_DOCUMENT_CLOSED);
		changeSet.SetChangeInfo(CN_DOCUMENT_CLOSED, QVariant::fromValue(notification));
		istd::CChangeNotifier notifier(this, &changeSet);
	}

	for (IDocumentManagerEventHandler* handlerPtr : GetDocumentManagerEventHandlers()){
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


IDocumentManager::OperationStatus CDocumentManagerBase::GetDocumentUndoManager(
	const QByteArray& userId, const QByteArray& documentId, idoc::IUndoManager*& undoManagerPtr) const
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


IDocumentManager::OperationStatus CDocumentManagerBase::RegisterDocumentObserver(
	const QByteArray& userId, const QByteArray& documentId, imod::IObserver& observer)
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


IDocumentManager::OperationStatus CDocumentManagerBase::UnregisterDocumentObserver(
	const QByteArray& userId, const QByteArray& documentId, imod::IObserver& observer)
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

bool CDocumentManagerBase::Serialize(iser::IArchive& /*archive*/)
{
	return false;
}


// protected methods

bool CDocumentManagerBase::ValidateInputParams(const QByteArray& userId, const QByteArray& documentId, OperationStatus& status) const
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


int CDocumentManagerBase::GetUndoManagerNextModelId(const QByteArray& userId)
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


void CDocumentManagerBase::InitializeDocumentObservers(
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


CDocumentManagerBase::WorkingDocument* CDocumentManagerBase::FindDocument(
	const QByteArray& userId, const QByteArray& documentId)
{
	if (m_userDocuments.contains(userId)){
		if (m_userDocuments[userId].contains(documentId)){
			return &m_userDocuments[userId][documentId];
		}
	}

	return nullptr;
}


const CDocumentManagerBase::WorkingDocument* CDocumentManagerBase::FindDocument(
	const QByteArray& userId, const QByteArray& documentId) const
{
	return const_cast<CDocumentManagerBase*>(this)->FindDocument(userId, documentId);
}


bool CDocumentManagerBase::FindDocument(
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


QUrl CDocumentManagerBase::ObjectIdToUrl(const QByteArray& objectId)
{
	QUrl url;
	url = "collection:///" + objectId;

	return url;
}


IDocumentManager::DocumentNotificationPtr CDocumentManagerBase::CreateDocumentNotification(
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


void CDocumentManagerBase::OnDocumentDataLoaded(
	const QByteArray& userId,
	const QByteArray& documentId)
{
	WorkingDocument* documentPtr = FindDocument(userId, documentId);
	if (documentPtr == nullptr) {
		return;
	}

	{
		// Wake any threads waiting in WaitForDocumentReady() — loading just
		// completed for this document.
		QMutexLocker waitLocker(&m_loadingWaitMutex);
		m_loadingWaitCondition.wakeAll();
	}

	{
		DocumentDataLoadedInfo info;
		info.userId = userId;
		info.documentId = documentId;
		info.typeId = documentPtr->typeId;
		info.url = ObjectIdToUrl(documentPtr->objectId);
		info.name = documentPtr->name;
		info.isDirty = documentPtr->isDirty;

		istd::IChangeable::ChangeSet changeSet(CF_DOCUMENT_DATA_LOADED);
		changeSet.SetChangeInfo(CN_DOCUMENT_DATA_LOADED, QVariant::fromValue(info));
		istd::CChangeNotifier notifier(this, &changeSet);
	}

	for (IDocumentManagerEventHandler* handlerPtr : GetDocumentManagerEventHandlers()){
		if (handlerPtr != nullptr){
			CDocumentDataLoadedEvent event(
				userId,
				documentId,
				documentPtr->typeId,
				documentPtr->name,
				ObjectIdToUrl(documentPtr->objectId),
				documentPtr->isDirty);
			handlerPtr->ProcessEvent(&event);
		}
	}
}


void CDocumentManagerBase::OnUndoManagerChanged(int modelId)
{
	QMutexLocker locker(&m_mutex);

	if (IsSingleCopyMode()) {
		// Find the shared document by modelId
		QByteArray sharedObjectId;
		for (auto it = m_sharedDocuments.constBegin(); it != m_sharedDocuments.constEnd(); ++it) {
			if (it.value().undoManagerModelId == modelId) {
				sharedObjectId = it.key();
				break;
			}
		}

		if (!sharedObjectId.isEmpty()) {
			UserDocumentPairList docs = FindDocumentsByObjectId(sharedObjectId);
			for (const UserDocumentPair& pair : docs) {
				WorkingDocument* documentPtr = FindDocument(pair.first, pair.second);
				if (documentPtr == nullptr) {
					continue;
				}

				documentPtr->isDirty = documentPtr->undoManagerPtr->GetDocumentChangeFlag() != idoc::IDocumentStateComparator::DCF_EQUAL;

				for (IDocumentManagerEventHandler* handlerPtr : GetDocumentManagerEventHandlers()){
					if (handlerPtr != nullptr){
						CDocumentUndoRedoChangedEvent event(
							pair.first,
							pair.second,
							documentPtr->typeId,
							documentPtr->name,
							ObjectIdToUrl(documentPtr->objectId),
							documentPtr->isDirty,
							*documentPtr->undoManagerPtr);
						handlerPtr->ProcessEvent(&event);
					}
				}
			}

			return;
		}
	}

	QByteArray userId;
	QByteArray documentId;

	if (!FindDocument(modelId, userId, documentId)){
		Q_ASSERT(false);

		return;
	}

	WorkingDocument* documentPtr = FindDocument(userId, documentId);
	Q_ASSERT(documentPtr != nullptr);
	if (documentPtr == nullptr){
		return;
	}

	documentPtr->isDirty = documentPtr->undoManagerPtr->GetDocumentChangeFlag() != idoc::IDocumentStateComparator::DCF_EQUAL;
	Q_ASSERT(documentPtr->undoManagerPtr.IsValid());

	for (IDocumentManagerEventHandler* handlerPtr : GetDocumentManagerEventHandlers()){
		if (handlerPtr != nullptr){
			CDocumentUndoRedoChangedEvent event(
				userId,
				documentId,
				documentPtr->typeId,
				documentPtr->name,
				ObjectIdToUrl(documentPtr->objectId),
				documentPtr->isDirty,
				*documentPtr->undoManagerPtr);
			handlerPtr->ProcessEvent(&event);
		}
	}
}


QString CDocumentManagerBase::GetDefaultDocumentName(const WorkingDocument& /*document*/) const
{
	return QString();
}


bool CDocumentManagerBase::HasDocumentNameProvider(const QByteArray& /*typeId*/) const
{
	return false;
}


bool CDocumentManagerBase::ValidateDocumentData(
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


QList<imtdoc::IDocumentManagerEventHandler*> CDocumentManagerBase::GetDocumentManagerEventHandlers() const
{
	return {};
}


QString CDocumentManagerBase::GetInvalidDocumentMessage()
{
	return QStringLiteral("Document data is invalid");
}


bool CDocumentManagerBase::IsSingleCopyMode() const
{
	return false;
}


CDocumentManagerBase::UserDocumentPairList CDocumentManagerBase::FindDocumentsByObjectId(
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

void CDocumentManagerBase::OnUpdate(imod::IModel* modelPtr, const istd::IChangeable::ChangeSet& changeSet)
{
	for (const QByteArray& userId : m_userDocuments.keys()){
		WorkingDocumentList& documents = m_userDocuments[userId];
		for (const QByteArray& documentId : documents.keys()){
			istd::IChangeable* changeablePtr = dynamic_cast<istd::IChangeable*>(modelPtr);

			if (documents[documentId].objectPtr.GetPtr() == changeablePtr){
				for (IDocumentManagerEventHandler* handlerPtr : GetDocumentManagerEventHandlers()){
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

CDocumentManagerBase::UndoManagerObserver::UndoManagerObserver(CDocumentManagerBase& parent)
	:m_parent(parent)
{
}


// protected methods of the embedded class UndoManagerObserver

void CDocumentManagerBase::UndoManagerObserver::OnModelChanged(int modelId, const istd::IChangeable::ChangeSet& /*changeSet*/)
{
	m_parent.OnUndoManagerChanged(modelId);
}



} // namespace imtdoc
