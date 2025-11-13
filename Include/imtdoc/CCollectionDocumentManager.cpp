#include <imtdoc/CCollectionDocumentManager.h>


// Qt includes
#include <QtCore/QUuid>

// ACF includes
#include <istd/CChangeNotifier.h>
#include <imod/IModel.h>
#include <imod/IObserver.h>


namespace imtdoc
{


// public methods

CCollectionDocumentManager::CCollectionDocumentManager()
	:m_undoManagerObserver(*this)
{
}


// protected methods

// reimplemented (imtdoc::IDocumentManager)

IDocumentManager::DocumentList CCollectionDocumentManager::GetOpenedDocumentList(
	const QByteArray& userId) const
{
	QMutexLocker locker(&m_mutex);

	DocumentList list;

	if (m_userDocuments.contains(userId)) {
		const WorkingDocumentList& workingDocumentList = m_userDocuments[userId];
		for (const QByteArray& id : workingDocumentList.keys()) {
			const WorkingDocument& workingDocument = workingDocumentList[id];

			DocumentInfo info;
			info.documentId = id;
			info.objectId = workingDocument.objectId;
			info.objectTypeId = workingDocument.objectTypeId;
			info.isDirty = workingDocument.isDirty;

			list.append(info);
		}
	}

	return list;
}


QByteArray CCollectionDocumentManager::CreateNewDocument(const QByteArray& userId, const QByteArray& documentTypeId)
{
	QByteArray retVal;

	istd::IChangeableSharedPtr objectPtr = CreateObject(documentTypeId);
	if (!objectPtr.IsValid()) {
		return retVal;
	}

	idoc::IUndoManagerSharedPtr undoManagerPtr = CreateUndoManager();
	if (!undoManagerPtr.IsValid()) {
		return retVal;
	}

	retVal = QUuid::createUuid().toByteArray(QUuid::WithoutBraces);

	NewDocumentCreatedInfo info;
	info.userId = userId;
	info.documentId = retVal;
	info.isDirty = false;

	QMutexLocker locker(&m_mutex);

	WorkingDocument& document = m_userDocuments[userId][retVal];

	{
		istd::IChangeable::ChangeSet changeSet(CF_NEW_DOCUMENT_CREATED);
		changeSet.SetChangeInfo(CN_NEW_DOCUMENT_CREATED, QVariant::fromValue(info));
		istd::CChangeNotifier notifier(this, &changeSet);

		WorkingDocument& document = m_userDocuments[userId][retVal];
		document.objectTypeId = documentTypeId;
		document.objectPtr = objectPtr;
		document.undoManagerPtr = undoManagerPtr;
		document.isDirty = false;
	}

	InitializeDocumentObservers(document, userId);

	return retVal;
}


QByteArray CCollectionDocumentManager::OpenDocument(const QByteArray& userId, const QUrl& url)
{
	QByteArray retVal;

	if (url.scheme() != "collection"){
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

	imtbase::IObjectCollection::DataPtr dataPtr;
	if (!objectTypeId.isEmpty() && collectionPtr->GetObjectData(objectId, dataPtr)) {
		if (dataPtr.IsValid()) {
			retVal = QUuid::createUuid().toByteArray(QUuid::WithoutBraces);

			DocumentOpenedInfo info;
			info.userId = userId;
			info.documentId = retVal;
			info.objectId = objectId;
			info.documentName = collectionPtr->GetElementInfo(objectId, imtbase::ICollectionInfo::EIT_NAME).toString();
			info.isDirty = false;

			QMutexLocker locker(&m_mutex);

			WorkingDocument& document = m_userDocuments[userId][retVal];

			{
				istd::IChangeable::ChangeSet changeSet(CF_DOCUMENT_OPENED);
				changeSet.SetChangeInfo(CN_DOCUMENT_OPENED, QVariant::fromValue(info));
				istd::CChangeNotifier notifier(this, &changeSet);

				document.objectId = objectId;
				document.objectTypeId = objectTypeId;
				document.objectPtr = dataPtr;
				document.undoManagerPtr = undoManagerPtr;
				document.isDirty = false;
			}

			InitializeDocumentObservers(document, userId);
		}
	}

	return retVal;
}


IDocumentManager::OperationStatus CCollectionDocumentManager::GetDocumentName(const QByteArray& userId, const QByteArray& documentId, QString& documentName) const
{
	OperationStatus retVal;

	QMutexLocker locker(&m_mutex);

	if (!ValidateInputParams(userId, documentId, retVal)){
		return retVal;
	}

	documentName = m_userDocuments[userId][documentId].documentName;

	return retVal;
}


IDocumentManager::OperationStatus CCollectionDocumentManager::SetDocumentName(const QByteArray& userId, const QByteArray& documentId, const QString& documentName)
{
	OperationStatus retVal;

	QMutexLocker locker(&m_mutex);

	if (!ValidateInputParams(userId, documentId, retVal)){
		return retVal;
	}

	QByteArray objectId = m_userDocuments[userId][documentId].objectId;
	if (!objectId.isEmpty()){
		imtbase::IObjectCollection* collectionPtr = GetCollection();
		if (collectionPtr == nullptr){
			return OS_FAILED;
		}

		if (!collectionPtr->SetElementName(objectId, documentName)) {
			return OS_FAILED;
		}
	}

	DocumentNotificationPtr notificationPtr = CreateDocumentNotification(userId, documentId);
	Q_ASSERT(notificationPtr != nullptr);
	if (notificationPtr != nullptr){
		istd::IChangeable::ChangeSet changeSet(CF_DOCUMENT_RENAMED);
		changeSet.SetChangeInfo(CN_DOCUMENT_RENAMED, QVariant::fromValue(*notificationPtr));
		istd::CChangeNotifier notifier(this, &changeSet);
	}

	m_userDocuments[userId][documentId].documentName = documentName;

	return OS_OK;
}


IDocumentManager::OperationStatus CCollectionDocumentManager::GetDocumentData(const QByteArray& userId, const QByteArray& documentId, istd::IChangeableSharedPtr& documentPtr) const
{
	OperationStatus retVal;

	QMutexLocker locker(&m_mutex);

	if (!ValidateInputParams(userId, documentId, retVal)){
		return retVal;
	}

	documentPtr.SetPtr(m_userDocuments[userId][documentId].objectPtr->CloneMe());

	return documentPtr.IsValid() ? OS_OK : OS_FAILED;
}


IDocumentManager::OperationStatus CCollectionDocumentManager::SetDocumentData(const QByteArray& userId, const QByteArray& documentId, const istd::IChangeable& document)
{
	OperationStatus retVal;

	QMutexLocker locker(&m_mutex);

	if (!ValidateInputParams(userId, documentId, retVal)){
		return retVal;
	}

	return m_userDocuments[userId][documentId].objectPtr->CopyFrom(document) ? OS_OK : OS_FAILED;
}


IDocumentManager::OperationStatus CCollectionDocumentManager::SaveDocument(
	const QByteArray& userId, const QByteArray& documentId)
{
	OperationStatus retVal;

	imtbase::IObjectCollection* collectionPtr = GetCollection();
	if (collectionPtr == nullptr) {
		return OS_FAILED;
	}

	QMutexLocker locker(&m_mutex);

	if (!ValidateInputParams(userId, documentId, retVal)){
		return retVal;
	}

	WorkingDocument& workingDocument = m_userDocuments[userId][documentId];

	if (!workingDocument.objectId.isEmpty()) {
		bool res = collectionPtr->SetObjectData(workingDocument.objectId, *workingDocument.objectPtr);

		if (res){
			workingDocument.isDirty = false;

			DocumentNotificationPtr notificationPtr = CreateDocumentNotification(userId, documentId);
			Q_ASSERT(notificationPtr != nullptr);
			if (notificationPtr != nullptr){
				istd::IChangeable::ChangeSet changeSet(CF_DOCUMENT_SAVED);
				changeSet.SetChangeInfo(CN_DOCUMENT_SAVED, QVariant::fromValue(*notificationPtr));
				istd::CChangeNotifier notifier(this, &changeSet);
			}

			workingDocument.undoManagerPtr->StoreDocumentState();
		}

		return res ? OS_OK : OS_FAILED;
	}

	workingDocument.objectId =
		collectionPtr->InsertNewObject(workingDocument.objectTypeId, workingDocument.documentName, "", workingDocument.objectPtr.GetPtr());

	if (!workingDocument.objectId.isEmpty()){
		workingDocument.isDirty = false;

		DocumentNotificationPtr notificationPtr = CreateDocumentNotification(userId, documentId);
		Q_ASSERT(notificationPtr != nullptr);
		if (notificationPtr != nullptr){
			istd::IChangeable::ChangeSet changeSet(CF_DOCUMENT_SAVED);
			changeSet.SetChangeInfo(CN_DOCUMENT_SAVED, QVariant::fromValue(*notificationPtr));
			istd::CChangeNotifier notifier(this, &changeSet);
		}

		workingDocument.undoManagerPtr->StoreDocumentState();
	}

	return workingDocument.objectId.isEmpty() ? OS_FAILED : OS_OK;
}


IDocumentManager::OperationStatus CCollectionDocumentManager::CloseDocument(
	const QByteArray& userId, const QByteArray& documentId)
{
	OperationStatus retVal;

	QMutexLocker locker(&m_mutex);

	if (!ValidateInputParams(userId, documentId, retVal)){
		return retVal;
	}

	DocumentClosedNotification notification;
	notification.userId = userId;
	notification.documentId = documentId;
	istd::IChangeable::ChangeSet changeSet(CF_DOCUMENT_CLOSED);
	changeSet.SetChangeInfo(CN_DOCUMENT_CLOSED, QVariant::fromValue(notification));
	istd::CChangeNotifier notifier(this, &changeSet);

	imod::IModel* modelPtr = dynamic_cast<imod::IModel*>(m_userDocuments[userId][documentId].undoManagerPtr.GetPtr());
	if (modelPtr != nullptr) {
		modelPtr->DetachAllObservers();
	}

	m_userDocuments[userId].remove(documentId);
	if (m_userDocuments[userId].isEmpty()) {
		m_userDocuments.remove(userId);
	}

	return OS_OK;
}


IDocumentManager::OperationStatus CCollectionDocumentManager::GetDocumentUndoManager(
	const QByteArray& userId, const QByteArray& documentId, idoc::IUndoManager*& undoManagerPtr) const
{
	undoManagerPtr = nullptr;

	OperationStatus retVal;

	QMutexLocker locker(&m_mutex);

	if (!ValidateInputParams(userId, documentId, retVal)){
		return retVal;
	}

	undoManagerPtr = m_userDocuments[userId][documentId].undoManagerPtr.GetPtr();

	return OS_OK;
}


// reimplemented (iser::ISerializable)

bool CCollectionDocumentManager::Serialize(iser::IArchive& archive)
{
	return false;
}


// reimplemented (imod::CMultiModelObserverBase)

void CCollectionDocumentManager::OnUpdate(imod::IModel* modelPtr, const istd::IChangeable::ChangeSet& changeSet)
{
	for (const QByteArray& userId : m_userDocuments.keys()){
		WorkingDocumentList& documents = m_userDocuments[userId];
		for (const QByteArray& documentId : documents.keys()){
			istd::IChangeable* changeablePtr = dynamic_cast<istd::IChangeable*>(modelPtr);

			if (documents[documentId].objectPtr.GetPtr() == changeablePtr){
				DocumentNotificationPtr notificationPtr = CreateDocumentNotification(userId, documentId);
				Q_ASSERT(notificationPtr != nullptr);
				if (notificationPtr != nullptr){
					istd::IChangeable::ChangeSet changeSet(CF_DOCUMENT_CHANGED);
					changeSet.SetChangeInfo(CN_DOCUMENT_CHANGED, QVariant::fromValue(*notificationPtr));
					istd::CChangeNotifier notifier(this, &changeSet);
				}
			}
		}
	}
}


// protected methods

bool CCollectionDocumentManager::ValidateInputParams(const QByteArray& userId, const QByteArray& documentId, OperationStatus& status) const
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


void CCollectionDocumentManager::OnUndoManagerChanged(int modelId)
{
	QByteArray userId;
	QByteArray documentId;

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

	documentPtr->isDirty = documentPtr->undoManagerPtr->GetDocumentChangeFlag() != idoc::IDocumentStateComparator::DCF_EQUAL;

	DocumentUndoNotification notification;
	notification.userId = userId;
	notification.documentId = documentId;
	notification.isDirty = documentPtr->isDirty;

	Q_ASSERT(documentPtr->undoManagerPtr.IsValid());
	if (documentPtr->undoManagerPtr.IsValid()){
		notification.availableUndoSteps = documentPtr->undoManagerPtr->GetAvailableUndoSteps();
		notification.availableRedoSteps = documentPtr->undoManagerPtr->GetAvailableRedoSteps();

		int count = documentPtr->undoManagerPtr->GetAvailableUndoSteps();
		for (int i = 0; i < count; i++){
			notification.undoLevelDescriptions.append(documentPtr->undoManagerPtr->GetUndoLevelDescription(i));
		}

		count = documentPtr->undoManagerPtr->GetAvailableRedoSteps();
		for (int i = 0; i < count; i++){
			notification.redoLevelDescriptions.append(documentPtr->undoManagerPtr->GetRedoLevelDescription(i));
		}
	}

	istd::IChangeable::ChangeSet changeSet(CF_DOCUMENT_UNDO_CHANGED);
	changeSet.SetChangeInfo(CN_DOCUMENT_UNDO_CHANGED, QVariant::fromValue(notification));
	istd::CChangeNotifier notifier(this, &changeSet);
}


int CCollectionDocumentManager::GetUndoManagerNextModelId(const QByteArray& userId)
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


CCollectionDocumentManager::WorkingDocument* CCollectionDocumentManager::FindDocument(
	const QByteArray& userId, const QByteArray& documentId)
{
	if (m_userDocuments.contains(userId)){
		if (m_userDocuments[userId].contains(documentId)){
			return &m_userDocuments[userId][documentId];
		}
	}

	return nullptr;
}


const CCollectionDocumentManager::WorkingDocument* CCollectionDocumentManager::FindDocument(
	const QByteArray& userId, const QByteArray& documentId) const
{
	return const_cast<CCollectionDocumentManager*>(this)->FindDocument(userId, documentId);
}


bool CCollectionDocumentManager::FindDocument(
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


void CCollectionDocumentManager::InitializeDocumentObservers(
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


// private methods

IDocumentManager::DocumentNotificationPtr CCollectionDocumentManager::CreateDocumentNotification(
	const QByteArray& userId,
	const QByteArray& documentId) const
{
	std::shared_ptr<DocumentNotification> retVal;

	if (m_userDocuments.contains(userId) && m_userDocuments[userId].contains(documentId)){
		const WorkingDocument document = m_userDocuments[userId][documentId];

		retVal.reset(new DocumentNotification);
		retVal->userId = userId;
		retVal->documentId = documentId;
		retVal->objectId = document.objectId;
		retVal->documentName = document.documentName;
		retVal->isDirty = document.isDirty;
	}

	return retVal;
}


// public methods of the embedded class UndoManagerObserver

CCollectionDocumentManager::UndoManagerObserver::UndoManagerObserver(CCollectionDocumentManager& parent)
	:m_parent(parent)
{
}


// protected methods of the embedded class UndoManagerObserver

void CCollectionDocumentManager::UndoManagerObserver::OnModelChanged(int modelId, const istd::IChangeable::ChangeSet& changeSet)
{
	m_parent.OnUndoManagerChanged(modelId);
}



} // namespace imtdoc
