// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtdoc/CCollectionDocumentServiceComp.h>


// Qt includes
#include <QtCore/QDir>
#include <QtCore/QFile>
#include <QtCore/QFileInfo>
#include <QtCore/QJsonArray>
#include <QtCore/QJsonDocument>
#include <QtCore/QJsonParseError>
#include <QtCore/QMetaObject>
#include <QtCore/QMutexLocker>
#include <QtCore/QSaveFile>
#include <QtCore/QSet>

// ImtCore includes
#include <imtbase/imtbase.h>


namespace imtdoc
{


static const QString s_documentsKey = QStringLiteral("Documents");
static const QString s_documentIdKey = QStringLiteral("DocumentId");
static const QString s_typeIdKey = QStringLiteral("TypeId");
static const QString s_documentSourceRefKey = QStringLiteral("DocumentSourceRef");
static const QString s_nameKey = QStringLiteral("Name");
static const QString s_isReadOnlyKey = QStringLiteral("IsReadOnly");
static const QString s_usersKey = QStringLiteral("Users");
static const QString s_userIdKey = QStringLiteral("UserId");
static const QString s_documentIdsKey = QStringLiteral("DocumentIds");
static const QString s_singleDocumentInstanceKey = QStringLiteral("SingleDocumentInstance");


// public methods

bool CCollectionDocumentServiceComp::LoadFromFile(const QString& filePath)
{
	if (filePath.isEmpty()){
		SendErrorMessage(0, QStringLiteral("Document service state file path is empty"));

		return false;
	}

	QFile file(filePath);
	if (!file.open(QIODevice::ReadOnly)){
		SendErrorMessage(0, QStringLiteral("Cannot open document service state file '%1': %2").arg(filePath, file.errorString()));

		return false;
	}

	const QByteArray fileData = file.readAll();
	file.close();

	QJsonParseError parseError;
	const QJsonDocument jsonDocument = QJsonDocument::fromJson(fileData, &parseError);
	if (parseError.error != QJsonParseError::NoError || !jsonDocument.isObject()){
		SendErrorMessage(0, QStringLiteral("Cannot parse document service state file '%1': %2").arg(filePath, parseError.errorString()));

		return false;
	}

	ServiceState state;
	if (!ServiceStateFromJson(jsonDocument.object(), state)){
		SendErrorMessage(0, QStringLiteral("Invalid content of document service state file '%1'").arg(filePath));

		return false;
	}

	// The loaded state must not be written back to the file while it is being restored
	m_isStateLoading = true;
	const bool retVal = RestoreServiceState(state);
	m_isStateLoading = false;

	return retVal;
}


bool CCollectionDocumentServiceComp::SaveToFile(const QString& filePath) const
{
	if (filePath.isEmpty()){
		SendErrorMessage(0, QStringLiteral("Document service state file path is empty"));

		return false;
	}

	ServiceState state;
	QMutexLocker documentLocker(&m_mutex);
	PrepareServiceState(state);

	QMutexLocker locker(&m_stateFileMutex);
	documentLocker.unlock();

	const QByteArray fileData = QJsonDocument(ServiceStateToJson(state)).toJson(QJsonDocument::Indented);

	const QString directoryPath = QFileInfo(filePath).absolutePath();
	if (!QDir().mkpath(directoryPath)){
		SendErrorMessage(0, QStringLiteral("Cannot create directory '%1' for document service state file").arg(directoryPath));

		return false;
	}

	QSaveFile file(filePath);
	if (!file.open(QIODevice::WriteOnly)){
		SendErrorMessage(0, QStringLiteral("Cannot open document service state file '%1' for writing: %2").arg(filePath, file.errorString()));

		return false;
	}

	if (file.write(fileData) != fileData.size()){
		SendErrorMessage(0, QStringLiteral("Cannot write document service state file '%1': %2").arg(filePath, file.errorString()));
		file.cancelWriting();

		return false;
	}

	if (!file.commit()){
		SendErrorMessage(0, QStringLiteral("Cannot save document service state file '%1': %2").arg(filePath, file.errorString()));

		return false;
	}

	return true;
}


// protected methods

// reimplemented (icomp::CComponentBase)

void CCollectionDocumentServiceComp::OnComponentCreated()
{
	BaseClass::OnComponentCreated();

	if (!IsStatePersistenceEnabled()){
		return;
	}

	const QString filePath = GetStateFilePath();
	if (filePath.isEmpty() || !QFileInfo::exists(filePath)){
		return;
	}

	LoadFromFile(filePath);
}


void CCollectionDocumentServiceComp::OnComponentDestroyed()
{
	if (IsStatePersistenceEnabled()){
		SaveToFile(GetStateFilePath());
	}

	BaseClass::OnComponentDestroyed();
}


// reimplemented (istd::IChangeable)

void CCollectionDocumentServiceComp::OnEndChanges(const ChangeSet& changeSet)
{
	if (m_isStateLoading || !IsStatePersistenceEnabled()){
		BaseClass::OnEndChanges(changeSet);

		return;
	}

	const QString filePath = GetStateFilePath();
	if (!filePath.isEmpty()){
		RequestSaveToFile(filePath);
	}

	BaseClass::OnEndChanges(changeSet);
}


// reimplemented (imtdoc::TCollectionDocumentServiceWrap)

imtbase::IObjectCollection* CCollectionDocumentServiceComp::GetCollection() const
{
	return m_collectionCompPtr.IsValid() ? m_collectionCompPtr.GetPtr() : nullptr;
}


// reimplemented (imtdoc::CDocumentServiceBase)

bool CCollectionDocumentServiceComp::IsSingleCopyMode() const
{
	return m_isSingleCopyModeAttrPtr.IsValid() && *m_isSingleCopyModeAttrPtr;
}


// private methods

void CCollectionDocumentServiceComp::PrepareServiceState(ServiceState& state) const
{
	state.documents.clear();
	state.userDocumentIds = m_userDocumentIds;

	for (auto documentIt = m_documents.constBegin(); documentIt != m_documents.constEnd(); ++documentIt){
		const WorkingDocument& workingDocument = documentIt.value();

		DocumentInfo documentInfo;
		documentInfo.documentId = documentIt.key();
		documentInfo.typeId = workingDocument.typeId;
		documentInfo.documentSourceRef = workingDocument.documentSourceRef;
		documentInfo.name = workingDocument.name;
		documentInfo.isReadOnly = workingDocument.isReadOnly;
		documentInfo.singleDocumentInstance = workingDocument.singleDocumentInstance;

		state.documents.insert(documentInfo.documentId, documentInfo);
	}
}


QJsonObject CCollectionDocumentServiceComp::ServiceStateToJson(const ServiceState& state)
{
	QJsonArray documentsArray;
	for (const DocumentInfo& documentInfo : state.documents){
		QJsonObject documentObject;
		documentObject.insert(s_documentIdKey, QString(documentInfo.documentId));
		documentObject.insert(s_typeIdKey, QString(documentInfo.typeId));
		documentObject.insert(s_documentSourceRefKey, QString(documentInfo.documentSourceRef));
		documentObject.insert(s_nameKey, documentInfo.name);
		documentObject.insert(s_isReadOnlyKey, documentInfo.isReadOnly);
		documentObject.insert(s_singleDocumentInstanceKey, documentInfo.singleDocumentInstance);

		documentsArray.append(documentObject);
	}

	QJsonArray usersArray;
	for (auto userIt = state.userDocumentIds.constBegin(); userIt != state.userDocumentIds.constEnd(); ++userIt){
		QJsonArray documentIdsArray;
		for (const QByteArray& documentId : userIt.value()){
			documentIdsArray.append(QString(documentId));
		}

		QJsonObject userObject;
		userObject.insert(s_userIdKey, QString(userIt.key()));
		userObject.insert(s_documentIdsKey, documentIdsArray);

		usersArray.append(userObject);
	}

	QJsonObject rootObject;
	rootObject.insert(s_documentsKey, documentsArray);
	rootObject.insert(s_usersKey, usersArray);

	return rootObject;
}


bool CCollectionDocumentServiceComp::ServiceStateFromJson(const QJsonObject& rootObject, ServiceState& state)
{
	state.documents.clear();
	state.userDocumentIds.clear();

	const QJsonValue documentsValue = rootObject.value(s_documentsKey);
	const QJsonValue usersValue = rootObject.value(s_usersKey);
	if (!documentsValue.isArray() || !usersValue.isArray()){
		return false;
	}

	const QJsonArray documentsArray = documentsValue.toArray();
	for (const QJsonValue& documentValue : documentsArray){
		if (!documentValue.isObject()){
			return false;
		}

		const QJsonObject documentObject = documentValue.toObject();
		if (!documentObject.value(s_documentIdKey).isString()
					|| documentObject.value(s_documentIdKey).toString().isEmpty()
					|| !documentObject.value(s_typeIdKey).isString()
					|| !documentObject.value(s_documentSourceRefKey).isString()
					|| !documentObject.value(s_nameKey).isString()
					|| !documentObject.value(s_isReadOnlyKey).isBool()
					|| !documentObject.value(s_singleDocumentInstanceKey).isBool()){
			return false;
		}

		DocumentInfo documentInfo;
		documentInfo.documentId = documentObject.value(s_documentIdKey).toString().toUtf8();
		documentInfo.typeId = documentObject.value(s_typeIdKey).toString().toUtf8();
		documentInfo.documentSourceRef = documentObject.value(s_documentSourceRefKey).toString().toUtf8();
		documentInfo.name = documentObject.value(s_nameKey).toString();
		documentInfo.isReadOnly = documentObject.value(s_isReadOnlyKey).toBool();
		documentInfo.singleDocumentInstance = documentObject.value(s_singleDocumentInstanceKey).toBool();

		if (state.documents.contains(documentInfo.documentId)){
			return false;
		}

		state.documents.insert(documentInfo.documentId, documentInfo);
	}

	const QJsonArray usersArray = usersValue.toArray();
	for (const QJsonValue& userValue : usersArray){
		if (!userValue.isObject()){
			return false;
		}

		const QJsonObject userObject = userValue.toObject();
		const QJsonValue userIdValue = userObject.value(s_userIdKey);
		const QJsonValue documentIdsValue = userObject.value(s_documentIdsKey);
		if (!userIdValue.isString() || !documentIdsValue.isArray()){
			return false;
		}

		QByteArrayList documentIds;

		const QJsonArray documentIdsArray = documentIdsValue.toArray();
		for (const QJsonValue& documentIdValue : documentIdsArray){
			if (!documentIdValue.isString()){
				return false;
			}

			const QByteArray documentId = documentIdValue.toString().toUtf8();
			if (!state.documents.contains(documentId)){
				return false;
			}

			documentIds.append(documentId);
		}

		state.userDocumentIds.insert(userIdValue.toString().toUtf8(), documentIds);
	}

	return true;
}


bool CCollectionDocumentServiceComp::IsStatePersistenceEnabled() const
{
	return m_statePersistenceEnablerCompPtr.IsValid() && m_statePersistenceEnablerCompPtr->IsEnabled();
}


QString CCollectionDocumentServiceComp::GetStateFilePath() const
{
	return m_stateFilePathCompPtr.IsValid() ? m_stateFilePathCompPtr->GetPath() : QString();
}


bool CCollectionDocumentServiceComp::RestoreServiceState(const ServiceState& state)
{
	istd::CChangeNotifier notifier(this);

	RemoveAllDocuments();

	const bool isSingleCopyMode = IsSingleCopyMode();
	imtbase::IObjectCollection* collectionPtr = GetCollection();

	if (collectionPtr == nullptr){
		return false;
	}

	for (auto userIt = state.userDocumentIds.constBegin(); userIt != state.userDocumentIds.constEnd(); ++userIt){
		const QByteArray& userId = userIt.key();
		for (const QByteArray& documentId : userIt.value()){
			RestoreDocument(state.documents.value(documentId), userId, isSingleCopyMode, collectionPtr);
		}
	}

	return true;
}


void CCollectionDocumentServiceComp::RemoveAllDocuments()
{
	QMutexLocker locker(&m_mutex);
	QSet<int> undoModelIds;

	for (auto docIt = m_documents.constBegin(); docIt != m_documents.constEnd(); ++docIt){
		if (docIt.value().undoManagerModelId >= 0){
			undoModelIds.insert(docIt.value().undoManagerModelId);
		}
	}
	for (int modelId : undoModelIds){
		m_undoManagerObserver.UnregisterModel(modelId);
	}

	ClearDocuments();
}


void CCollectionDocumentServiceComp::RequestSaveToFile(const QString& filePath)
{
	if (filePath.isEmpty()){
		return;
	}

	bool shouldQueueProcessing = false;
	{
		QMutexLocker locker(&m_saveRequestMutex);
		m_lastRequestedStateFilePath = filePath;
		if (!m_isSaveRequestProcessingQueued){
			m_isSaveRequestProcessingQueued = true;
			shouldQueueProcessing = true;
		}
	}

	if (!shouldQueueProcessing){
		return;
	}

	std::weak_ptr<std::atomic<bool>> aliveGuard(m_isAlive);
	QMetaObject::invokeMethod(
		this,
		[this, aliveGuard] {
			auto isAlive = aliveGuard.lock();
			if (!isAlive || !isAlive->load()){
				return;
			}

			ProcessSaveRequests();
		},
		Qt::QueuedConnection);
}


void CCollectionDocumentServiceComp::ProcessSaveRequests()
{
	QString filePath;
	{
		QMutexLocker locker(&m_saveRequestMutex);
		filePath = m_lastRequestedStateFilePath;
		m_lastRequestedStateFilePath.clear();
		m_isSaveRequestProcessingQueued = false;
	}

	if (!filePath.isEmpty()){
		SaveToFile(filePath);
	}
}


bool CCollectionDocumentServiceComp::RestoreSharedDocument(const DocumentInfo& documentInfo, const QByteArray& userId, bool isSingleCopyMode)
{
	QMutexLocker locker(&m_mutex);

	// The same document was already restored for another user
	if (m_documents.contains(documentInfo.documentId)){
		InsertDocument(userId, documentInfo.documentId);

		return true;
	}

	if (!isSingleCopyMode || documentInfo.documentSourceRef.isEmpty()){
		return false;
	}

	const UserDocumentPairList documents = FindDocumentsBySourceRef(documentInfo.documentSourceRef);
	if (documents.isEmpty()){
		return false;
	}

	InsertDocument(userId, documents.first().second);

	return true;
}


bool CCollectionDocumentServiceComp::RestoreDocument(
			const DocumentInfo& documentInfo,
			const QByteArray& userId,
			bool isSingleCopyMode,
			imtbase::IObjectCollection* collectionPtr)
{
	if (RestoreSharedDocument(documentInfo, userId, isSingleCopyMode)){
		return true;
	}

	const QByteArray objectId = imtdoc::SourceRefToObjectId(documentInfo.documentSourceRef);
	if (objectId.isEmpty() && !documentInfo.documentSourceRef.isEmpty()){
		return false;
	}

	QByteArray typeId = documentInfo.typeId;
	if (typeId.isEmpty() && !objectId.isEmpty()){
		typeId = collectionPtr->GetObjectTypeId(objectId);
	}
	if (typeId.isEmpty()){
		return false;
	}

	const int revision = imtdoc::SourceRefToRevision(documentInfo.documentSourceRef);

	istd::IChangeableSharedPtr objectPtr;
	if (!objectId.isEmpty()){
		if (revision < 0){
			if (!collectionPtr->GetObjectData(objectId, objectPtr) || !objectPtr.IsValid()){
				return false;
			}
		}
		else{
			const imtbase::IRevisionController* revisionControllerPtr = collectionPtr->GetRevisionController();
			idoc::MetaInfoPtr metaInfoPtr;
			if (revisionControllerPtr == nullptr
						|| !revisionControllerPtr->GetRevisionData(*collectionPtr, objectId, revision, objectPtr, metaInfoPtr)
						|| !objectPtr.IsValid()){
				return false;
			}
		}
	}

	if (!objectPtr.IsValid()){
		objectPtr.FromUnique(CreateObject(typeId));
	}

	if (!objectPtr.IsValid()){
		return false;
	}

	idoc::IUndoManagerSharedPtr undoManagerPtr;
	undoManagerPtr.FromUnique(std::move(CreateUndoManager()));
	if (!undoManagerPtr.IsValid()){
		return false;
	}

	WorkingDocument document;
	document.typeId = typeId;
	document.documentSourceRef = imtdoc::ObjectIdToSourceRef(objectId, revision);
	document.name = documentInfo.name;
	document.isReadOnly = documentInfo.isReadOnly;
	document.objectPtr = objectPtr;
	document.undoManagerPtr = undoManagerPtr;
	document.isDirty = false;
	document.isLoading = true;
	document.singleDocumentInstance = documentInfo.singleDocumentInstance;
	document.undoManagerModelId = -1;

	{
		QMutexLocker locker(&m_mutex);
		InsertDocument(userId, documentInfo.documentId) = document;
	}

	RegisterRestoredDocument(documentInfo, userId, undoManagerPtr);

	return true;
}


void CCollectionDocumentServiceComp::RegisterRestoredDocument(
			const DocumentInfo& documentInfo,
			const QByteArray& userId,
			const idoc::IUndoManagerSharedPtr& undoManagerPtr)
{
	WorkingDocument& registeredDocument = m_documents[documentInfo.documentId];

	InitializeDocumentObservers(registeredDocument, userId, documentInfo.documentId);
	registeredDocument.isDirty = undoManagerPtr->GetDocumentChangeFlag() != idoc::IDocumentStateComparator::DCF_EQUAL;
	registeredDocument.isLoading = false;
}
} // namespace imtdoc
