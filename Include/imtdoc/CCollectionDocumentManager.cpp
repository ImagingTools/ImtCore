#include <imtdoc/CCollectionDocumentManager.h>


// Qt includes
#include <QtCore/QUuid>

// ACF includes
#include <istd/CChangeNotifier.h>
#include <imod/IModel.h>
#include <imod/IObserver.h>


namespace imtdoc
{


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

	imod::IModel* modelPtr = dynamic_cast<imod::IModel*>(objectPtr.GetPtr());
	imod::IObserver* undoObserverPtr = dynamic_cast<imod::IObserver*>(undoManagerPtr.GetPtr());

	if (modelPtr != nullptr){
		if (undoObserverPtr != nullptr){
			modelPtr->AttachObserver(undoObserverPtr);
		}

		modelPtr->AttachObserver(this);
	}

	retVal = QUuid::createUuid().toByteArray(QUuid::WithoutBraces);

	QMutexLocker locker(&m_mutex);

	NewDocumentCreatedInfo info;
	info.documentId = retVal;
	info.hasChanges = false;
	info.availableUndoSteps = 0;
	info.availableRedoSteps = 0;

	istd::IChangeable::ChangeSet changeSet(CF_NEW_DOCUMENT_CREATED);
	changeSet.SetChangeInfo(CN_NEW_DOCUMENT_CREATED, QVariant::fromValue(info));
	istd::CChangeNotifier notifier(this, &changeSet);

	m_userDocuments[userId][retVal].objectTypeId = documentTypeId;
	m_userDocuments[userId][retVal].objectPtr = objectPtr;
	m_userDocuments[userId][retVal].undoManagerPtr = undoManagerPtr;
	m_userDocuments[userId][retVal].hasChanges = false;

	return retVal;
}


QByteArray CCollectionDocumentManager::OpenDocument(const QByteArray& userId, const QByteArray& objectId)
{
	QByteArray retVal;

	imtbase::IObjectCollection* collectionPtr = GetCollection();
	if (collectionPtr == nullptr) {
		return retVal;
	}

	QByteArray objectTypeId = collectionPtr->GetObjectTypeId(objectId);

	imtbase::IObjectCollection::DataPtr dataPtr;
	if (!objectTypeId.isEmpty() && collectionPtr->GetObjectData(objectId, dataPtr)) {
		if (dataPtr.IsValid()) {
			idoc::IUndoManagerSharedPtr undoManagerPtr = CreateUndoManager();
			if (!undoManagerPtr.IsValid()) {
				return retVal;
			}

			imod::IModel* modelPtr = dynamic_cast<imod::IModel*>(dataPtr.GetPtr());
			imod::IObserver* undoObserverPtr = dynamic_cast<imod::IObserver*>(undoManagerPtr.GetPtr());

			if (modelPtr != nullptr){
				if (undoObserverPtr != nullptr){
					modelPtr->AttachObserver(undoObserverPtr);
				}

				modelPtr->AttachObserver(this);
			}

			retVal = QUuid::createUuid().toByteArray(QUuid::WithoutBraces);

			QMutexLocker locker(&m_mutex);

			NewDocumentCreatedInfo info;
			info.documentId = retVal;
			info.objectId = objectId;
			info.hasChanges = false;
			info.availableUndoSteps = 0;
			info.availableRedoSteps = 0;

			istd::IChangeable::ChangeSet changeSet(CF_NEW_DOCUMENT_CREATED);
			changeSet.SetChangeInfo(CN_NEW_DOCUMENT_CREATED, QVariant::fromValue(info));
			istd::CChangeNotifier notifier(this, &changeSet);

			m_userDocuments[userId][retVal].objectId = objectId;
			m_userDocuments[userId][retVal].objectTypeId = objectTypeId;
			m_userDocuments[userId][retVal].objectPtr = dataPtr;
			m_userDocuments[userId][retVal].undoManagerPtr = undoManagerPtr;
			m_userDocuments[userId][retVal].hasChanges = false;
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

	if (m_userDocuments.contains(userId)) {
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
				istd::IChangeable::ChangeSet changeSet(CF_DOCUMENT_CHANGED);
				changeSet.SetChangeInfo(CN_DOCUMENT_CHANGED, QVariant::fromValue(*notificationPtr));
				istd::CChangeNotifier notifier(this, &changeSet);
			}
		}

		return retVal ? OS_OK : OS_FAILED;
	}

	Q_ASSERT(false);

	workingDocument.objectId =
		collectionPtr->InsertNewObject(workingDocument.objectTypeId, "", "", workingDocument.objectPtr.GetPtr());

	if (!workingDocument.objectId.isEmpty()){
		workingDocument.hasChanges = false;

		DocumentNotificationPtr notificationPtr = CreateDocumentNotification(userId, documentId);
		Q_ASSERT(notificationPtr != nullptr);
		if (notificationPtr != nullptr){
			istd::IChangeable::ChangeSet changeSet(CF_DOCUMENT_CHANGED);
			changeSet.SetChangeInfo(CN_DOCUMENT_CHANGED, QVariant::fromValue(*notificationPtr));
			istd::CChangeNotifier notifier(this, &changeSet);
		}
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
				documents[documentId].hasChanges = true;

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


} // namespace imtdoc
