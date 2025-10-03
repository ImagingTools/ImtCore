#include <imtdoc/CCollectionDocumentManager.h>


// Qt includes
#include <QtCore/QUuid>

// ACF includes
#include <imod/IModel.h>
#include <imod/IObserver.h>


namespace imtdoc
{


// protected methods

// reimplemented (imtdoc::IDocumentManager)

imtdoc::ICollectionDocumentManager::DocumentList CCollectionDocumentManager::GetDocumentList(
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
			info.name = workingDocument.name;
			info.hasChanges = workingDocument.hasChanges;

			list.append(info);
		}
	}

	return list;
}


QByteArray CCollectionDocumentManager::CreateDocument(const QByteArray& userId, const QByteArray& documentTypeId)
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

	QMutexLocker locker(&m_mutex);

	m_userDocuments[userId][retVal].objectTypeId = documentTypeId;
	m_userDocuments[userId][retVal].objectPtr = objectPtr;
	m_userDocuments[userId][retVal].undoManagerPtr = undoManagerPtr;
	m_userDocuments[userId][retVal].hasChanges = false;

	return retVal;
}


QByteArray CCollectionDocumentManager::OpenDocument(const QByteArray& userId, const QByteArray& objectId)
{
	QByteArray retVal;

	if (m_collectionPtr == nullptr) {
		return retVal;
	}

	QByteArray objectTypeId = m_collectionPtr->GetObjectTypeId(objectId);

	imtbase::IObjectCollection::DataPtr dataPtr;
	if (!objectTypeId.isEmpty() && m_collectionPtr->GetObjectData(objectId, dataPtr)) {
		if (dataPtr.IsValid()) {
			idoc::IUndoManagerSharedPtr undoManagerPtr = CreateUndoManager();
			if (!undoManagerPtr.IsValid()) {
				return retVal;
			}

			imod::IModel* modelPtr = dynamic_cast<imod::IModel*>(dataPtr.GetPtr());
			imod::IObserver* observerPtr = dynamic_cast<imod::IObserver*>(undoManagerPtr.GetPtr());

			if (modelPtr != nullptr && observerPtr != nullptr) {
				modelPtr->AttachObserver(observerPtr);
			}

			retVal = QUuid::createUuid().toByteArray(QUuid::WithoutBraces);

			QMutexLocker locker(&m_mutex);

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
	if (m_collectionPtr == nullptr) {
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
		return m_collectionPtr->SetObjectData(workingDocument.objectId, *workingDocument.objectPtr) ? OS_OK : OS_FAILED;
	}

	Q_ASSERT(false);

	workingDocument.objectId =
		m_collectionPtr->InsertNewObject(workingDocument.objectTypeId, "", "", workingDocument.objectPtr.GetPtr());

	return workingDocument.objectId.isEmpty() ? OS_FAILED : OS_OK;
}


imtdoc::ICollectionDocumentManager::OperationStatus CCollectionDocumentManager::CloseDocument(
	const QByteArray& userId, const QByteArray& documentId)
{
	QMutexLocker locker(&m_mutex);

	if (m_userDocuments.contains(userId)) {
		return OS_INVALID_USER_ID;
	}

	if (!m_userDocuments[userId].contains(documentId)) {
		return OS_INVALID_DOCUMENT_ID;
	}

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


} // namespace imtdoc
