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

// reimplemented (imtdoc::ICollectionDocumentManager)

imtdoc::ICollectionDocumentManager::DocumentList CCollectionDocumentManager::GetOpenedDocumentList(
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
			info.hasChanges = workingDocument.hasChanges;

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
	info.documentId = retVal;
	info.hasChanges = false;
	info.availableUndoSteps = 0;
	info.availableRedoSteps = 0;

	istd::IChangeable::ChangeSet changeSet(CF_NEW_DOCUMENT_CREATED);
	changeSet.SetChangeInfo(CN_NEW_DOCUMENT_CREATED, QVariant::fromValue(info));
	istd::CChangeNotifier notifier(this, &changeSet);

	QMutexLocker locker(&m_mutex);

	WorkingDocument& document = m_userDocuments[userId][retVal];
	document.objectTypeId = documentTypeId;
	document.objectPtr = objectPtr;
	document.undoManagerPtr = undoManagerPtr;
	document.hasChanges = false;

	InitializeDocumentObservers(document, userId);

	return retVal;
}


QByteArray CCollectionDocumentManager::OpenDocument(const QByteArray& userId, const QByteArray& objectId)
{
	QByteArray retVal;

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
			info.documentId = retVal;
			info.objectId = objectId;
			info.hasChanges = false;
			info.availableUndoSteps = 0;
			info.availableRedoSteps = 0;

			istd::IChangeable::ChangeSet changeSet(CF_DOCUMENT_OPENED);
			changeSet.SetChangeInfo(CN_DOCUMENT_OPENED, QVariant::fromValue(info));
			istd::CChangeNotifier notifier(this, &changeSet);

			QMutexLocker locker(&m_mutex);

			WorkingDocument& document = m_userDocuments[userId][retVal];
			document.objectId = objectId;
			document.objectTypeId = objectTypeId;
			document.objectPtr = dataPtr;
			document.undoManagerPtr = undoManagerPtr;
			document.hasChanges = false;

			InitializeDocumentObservers(document, userId);
		}
	}

	return retVal;
}


istd::IChangeable* CCollectionDocumentManager::GetDocument(const QByteArray& userId, const QByteArray& documentId) const
{
	QMutexLocker locker(&m_mutex);

	if (m_userDocuments.contains(userId) && m_userDocuments[userId].contains(documentId)) {
		return m_userDocuments[userId][documentId].objectPtr.GetPtr();
	}

	return nullptr;
}


imtdoc::ICollectionDocumentManager::OperationStatus CCollectionDocumentManager::SaveDocument(
	const QByteArray& userId, const QByteArray& documentId)
{
	imtbase::IObjectCollection* collectionPtr = GetCollection();
	if (collectionPtr == nullptr) {
		return OS_FAILED;
	}

	QMutexLocker locker(&m_mutex);

	if (!m_userDocuments.contains(userId)) {
		return OS_INVALID_USER_ID;
	}

	if (!m_userDocuments[userId].contains(documentId)) {
		return OS_INVALID_DOCUMENT_ID;
	}

	WorkingDocument& workingDocument = m_userDocuments[userId][documentId];

	if (!workingDocument.objectId.isEmpty()) {
		bool retVal = collectionPtr->SetObjectData(workingDocument.objectId, *workingDocument.objectPtr);

		if (retVal){
			workingDocument.hasChanges = false;

			DocumentNotificationPtr notificationPtr = CreateDocumentNotification(userId, documentId);
			Q_ASSERT(notificationPtr != nullptr);
			if (notificationPtr != nullptr){
				istd::IChangeable::ChangeSet changeSet(CF_DOCUMENT_SAVED);
				changeSet.SetChangeInfo(CN_DOCUMENT_SAVED, QVariant::fromValue(*notificationPtr));
				istd::CChangeNotifier notifier(this, &changeSet);
			}

			workingDocument.undoManagerPtr->StoreDocumentState();
		}

		return retVal ? OS_OK : OS_FAILED;
	}

	workingDocument.objectId =
		collectionPtr->InsertNewObject(workingDocument.objectTypeId, "", "", workingDocument.objectPtr.GetPtr());

	if (!workingDocument.objectId.isEmpty()){
		workingDocument.hasChanges = false;

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


imtdoc::ICollectionDocumentManager::OperationStatus CCollectionDocumentManager::CloseDocument(
	const QByteArray& userId, const QByteArray& documentId)
{
	QMutexLocker locker(&m_mutex);

	if (!m_userDocuments.contains(userId)) {
		return OS_INVALID_USER_ID;
	}

	if (!m_userDocuments[userId].contains(documentId)) {
		return OS_INVALID_DOCUMENT_ID;
	}

	istd::IChangeable::ChangeSet changeSet(CF_DOCUMENT_CLOSED);
	changeSet.SetChangeInfo(CN_DOCUMENT_CLOSED, documentId);
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


idoc::IUndoManager* CCollectionDocumentManager::GetDocumentUndoManager(
	const QByteArray& userId, const QByteArray& documentId) const
{
	QMutexLocker locker(&m_mutex);

	if (m_userDocuments.contains(userId) && m_userDocuments[userId].contains(documentId)) {
		return m_userDocuments[userId][documentId].undoManagerPtr.GetPtr();
	}

	return nullptr;
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

void CCollectionDocumentManager::OnUndoManagerChanged(int modelId)
{
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

	bool hasChanges = documentPtr->undoManagerPtr->GetDocumentChangeFlag() != idoc::IDocumentStateComparator::DCF_EQUAL;
	if (documentPtr->hasChanges != hasChanges){
		documentPtr->hasChanges = hasChanges;

		DocumentNotificationPtr notificationPtr = CreateDocumentNotification(userId, documentId);
		Q_ASSERT(notificationPtr != nullptr);
		if (notificationPtr != nullptr){
			istd::IChangeable::ChangeSet changeSet(CF_DOCUMENT_CHANGED);
			changeSet.SetChangeInfo(CN_DOCUMENT_CHANGED, QVariant::fromValue(*notificationPtr));
			istd::CChangeNotifier notifier(this, &changeSet);
		}
	}
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
	const QByteArray& userId,
	const QByteArray& documentId)
{
	if (m_userDocuments.contains(userId)){
		if (m_userDocuments[userId].contains(documentId)){
			return &m_userDocuments[userId][documentId];
		}
	}

	return nullptr;
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

std::shared_ptr<ICollectionDocumentManager::DocumentNotification> CCollectionDocumentManager::CreateDocumentNotification(
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
		retVal->hasChanges = document.hasChanges;

		Q_ASSERT(document.undoManagerPtr.IsValid());
		if (document.undoManagerPtr.IsValid()){
			retVal->availableUndoSteps = document.undoManagerPtr->GetAvailableUndoSteps();
			retVal->availableRedoSteps = document.undoManagerPtr->GetAvailableRedoSteps();

			int count = document.undoManagerPtr->GetAvailableUndoSteps();
			for (int i = 0; i < count; i++){
				retVal->undoLevelDescriptions.append(document.undoManagerPtr->GetUndoLevelDescription(i));
			}

			count = document.undoManagerPtr->GetAvailableRedoSteps();
			for (int i = 0; i < count; i++){
				retVal->redoLevelDescriptions.append(document.undoManagerPtr->GetRedoLevelDescription(i));
			}
		}
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
