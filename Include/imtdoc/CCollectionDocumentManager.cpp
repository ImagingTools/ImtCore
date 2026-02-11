// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtdoc/CCollectionDocumentManager.h>


// Qt includes
#include <QtCore/QUuid>

// ACF includes
#include <istd/CChangeNotifier.h>
#include <imod/IModel.h>
#include <imod/IObserver.h>

// ImtCore includes
#include <imtdoc/CDocumentChangedEvent.h>
#include <imtdoc/CDocumentClosedEvent.h>
#include <imtdoc/CDocumentCreatedEvent.h>
#include <imtdoc/CDocumentOpenedEvent.h>
#include <imtdoc/CDocumentRenamedEvent.h>
#include <imtdoc/CDocumentSavedAsEvent.h>
#include <imtdoc/CDocumentSavedEvent.h>
#include <imtdoc/CDocumentUndoRedoChangedEvent.h>


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

			DocumentListItem info;
			info.documentId = id;
			info.typeId = workingDocument.typeId;
			info.url = "collection:///" + workingDocument.objectId;
			info.name = workingDocument.name;
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

	WorkingDocument* documentPtr = nullptr;
	{
		QMutexLocker locker(&m_mutex);
		documentPtr = &m_userDocuments[userId][retVal];
	}

	documentPtr->typeId = documentTypeId;
	documentPtr->objectPtr = objectPtr;
	documentPtr->undoManagerPtr = undoManagerPtr;
	documentPtr->isDirty = false;

	InitializeDocumentObservers(*documentPtr, userId);

	{
		NewDocumentCreatedInfo info;
		info.userId = userId;
		info.documentId = retVal;
		info.typeId = documentTypeId;
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
				documentPtr->name,
				ObjectIdToUrl(documentPtr->objectId),
				documentPtr->isDirty);
			handlerPtr->ProcessEvent(&event);
		}
	}

	return retVal;
}


QByteArray CCollectionDocumentManager::OpenDocument(const QByteArray& userId, const QUrl& url)
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

	imtbase::IObjectCollection::DataPtr dataPtr;
	if (!objectTypeId.isEmpty() && collectionPtr->GetObjectData(objectId, dataPtr)) {
		if (dataPtr.IsValid()) {
			retVal = QUuid::createUuid().toByteArray(QUuid::WithoutBraces);

			WorkingDocument* documentPtr = nullptr;
			{
				QMutexLocker locker(&m_mutex);
				documentPtr = &m_userDocuments[userId][retVal];
			}

			documentPtr->objectId = objectId;
			documentPtr->typeId = objectTypeId;
			documentPtr->name = collectionPtr->GetElementInfo(objectId, imtbase::ICollectionInfo::EIT_NAME).toString();
			documentPtr->objectPtr = dataPtr;
			documentPtr->undoManagerPtr = undoManagerPtr;
			documentPtr->isDirty = false;

			InitializeDocumentObservers(*documentPtr, userId);

			{
				DocumentOpenedInfo info;
				info.userId = userId;
				info.documentId = retVal;
				info.typeId = objectTypeId;
				info.url = "collection:///" + objectId;
				info.name = documentPtr->name;
				info.isDirty = false;

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
		}
	}

	return retVal;
}


IDocumentManager::OperationStatus CCollectionDocumentManager::GetDocumentName(const QByteArray& userId, const QByteArray& documentId, QString& documentName) const
{

	QMutexLocker locker(&m_mutex);

	OperationStatus validationStatus;
	if (!ValidateInputParams(userId, documentId, validationStatus)){
		return validationStatus;
	}

	documentName = m_userDocuments[userId][documentId].name;

	return OS_OK;
}


IDocumentManager::OperationStatus CCollectionDocumentManager::SetDocumentName(const QByteArray& userId, const QByteArray& documentId, const QString& documentName)
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


const istd::IChangeable* CCollectionDocumentManager::GetDocumentPtr(const QByteArray& userId, const QByteArray& documentId) const
{
	OperationStatus retVal;

	QMutexLocker locker(&m_mutex);

	if (!ValidateInputParams(userId, documentId, retVal)){
		return nullptr;
	}

	return m_userDocuments[userId][documentId].objectPtr.GetPtr();
}


IDocumentManager::OperationStatus CCollectionDocumentManager::GetDocumentData(const QByteArray& userId, const QByteArray& documentId, istd::IChangeableSharedPtr& documentPtr) const
{
	
	QMutexLocker locker(&m_mutex);
	
	OperationStatus validationStatus;
	if (!ValidateInputParams(userId, documentId, validationStatus)){
		return validationStatus;
	}

	WorkingDocument& workingDocument = m_userDocuments[userId][documentId];

	documentPtr = CreateObject(workingDocument.typeId);
	if (!documentPtr.IsValid()) {
		return OS_FAILED;
	}

	if (!documentPtr->CopyFrom(*m_userDocuments[userId][documentId].objectPtr)) {
		return OS_FAILED;
	}

	return OS_OK;
}


IDocumentManager::OperationStatus CCollectionDocumentManager::SetDocumentData(const QByteArray& userId, const QByteArray& documentId, const istd::IChangeable& document)
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

	bool isCopySuccessful = workingDocumentPtr->objectPtr->CopyFrom(document);

	return isCopySuccessful ? OS_OK : OS_FAILED;
}


IDocumentManager::OperationStatus CCollectionDocumentManager::SaveDocument(
	const QByteArray& userId, const QByteArray& documentId, const QString& documentName)
{
	imtbase::IObjectCollection* collectionPtr = GetCollection();
	if (collectionPtr == nullptr) {
		return OS_FAILED;
	}
	
	WorkingDocument* workingDocumentPtr = nullptr;
	{
		OperationStatus validationStatus;
		QMutexLocker locker(&m_mutex);
		if (!ValidateInputParams(userId, documentId, validationStatus)){
			return validationStatus;
		}

		workingDocumentPtr = &m_userDocuments[userId][documentId];
	}

	if (!workingDocumentPtr->objectId.isEmpty()) {
		// Create copy of the object
		if (!documentName.isEmpty() && workingDocumentPtr->name != documentName){
			QByteArray newObjectId = collectionPtr->InsertNewObject(
				workingDocumentPtr->typeId, documentName, "", workingDocumentPtr->objectPtr.GetPtr());

			if (newObjectId.isEmpty()){
				return OS_FAILED;
			}

			workingDocumentPtr->objectId = newObjectId;
			workingDocumentPtr->name = documentName;
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
		bool res = collectionPtr->SetObjectData(workingDocumentPtr->objectId, *workingDocumentPtr->objectPtr);

		if (res){
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
		collectionPtr->InsertNewObject(workingDocumentPtr->typeId, documentName, "", workingDocumentPtr->objectPtr.GetPtr());

	if (!workingDocumentPtr->objectId.isEmpty()){
		workingDocumentPtr->name = documentName;
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


IDocumentManager::OperationStatus CCollectionDocumentManager::CloseDocument(
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
		imod::IModel* modelPtr = dynamic_cast<imod::IModel*>(workingDocumentPtr->undoManagerPtr.GetPtr());
		if (modelPtr != nullptr) {
			modelPtr->DetachAllObservers();
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


IDocumentManager::OperationStatus CCollectionDocumentManager::GetDocumentUndoManager(
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


IDocumentManager::OperationStatus CCollectionDocumentManager::RegisterDocumentObserver(
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


IDocumentManager::OperationStatus CCollectionDocumentManager::UnregisterDocumentObserver(
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

bool CCollectionDocumentManager::Serialize(iser::IArchive& /*archive*/)
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

				return;
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


QUrl CCollectionDocumentManager::ObjectIdToUrl(const QByteArray& objectId)
{
	QUrl url;
	url = "collection:///" + objectId;

	return url;
}


QList<imtdoc::IDocumentManagerEventHandler*> CCollectionDocumentManager::GetDocumentManagerEventHandlers() const
{
	return {};
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
		retVal->url = "collection:///" + document.objectId;
		retVal->name = document.name;
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

void CCollectionDocumentManager::UndoManagerObserver::OnModelChanged(int modelId, const istd::IChangeable::ChangeSet& /*changeSet*/)
{
	m_parent.OnUndoManagerChanged(modelId);
}



} // namespace imtdoc
