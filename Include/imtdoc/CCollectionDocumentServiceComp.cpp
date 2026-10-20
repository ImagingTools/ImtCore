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
#include <imtbase/IRevisionController.h>
#include <imtbase/imtbase.h>


namespace imtdoc
{


static const QString s_usersKey = QStringLiteral("Users");
static const QString s_userIdKey = QStringLiteral("UserId");
static const QString s_documentsKey = QStringLiteral("Documents");
static const QString s_documentIdKey = QStringLiteral("DocumentId");
static const QString s_objectIdKey = QStringLiteral("ObjectId");
static const QString s_typeIdKey = QStringLiteral("TypeId");
static const QString s_urlKey = QStringLiteral("Url");
static const QString s_nameKey = QStringLiteral("Name");
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

	DocumentMap documentMap;
	if (!DocumentMapFromJson(jsonDocument.object(), documentMap)){
		SendErrorMessage(0, QStringLiteral("Invalid content of document service state file '%1'").arg(filePath));

		return false;
	}

	// The loaded state must not be written back to the file while it is being restored
	m_isStateLoading = true;
	const bool retVal = RestoreDocumentMap(documentMap);
	m_isStateLoading = false;

	return retVal;
}


bool CCollectionDocumentServiceComp::SaveToFile(const QString& filePath) const
{
	if (filePath.isEmpty()){
		SendErrorMessage(0, QStringLiteral("Document service state file path is empty"));

		return false;
	}

	DocumentMap documentMap;
	QMutexLocker documentLocker(&m_mutex);
	PrepareDocumentMap(documentMap);

	QMutexLocker locker(&m_stateFileMutex);
	documentLocker.unlock();

	const QByteArray fileData = QJsonDocument(DocumentMapToJson(documentMap)).toJson(QJsonDocument::Indented);

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

void CCollectionDocumentServiceComp::PrepareDocumentMap(DocumentMap& documentMap) const
{
	documentMap.clear();

	for (auto userIt = m_userDocuments.constBegin(); userIt != m_userDocuments.constEnd(); ++userIt){
		DocumentInfoList documents;
		const WorkingDocumentList& docs = userIt.value();
		for (auto docIt = docs.constBegin(); docIt != docs.constEnd(); ++docIt){
			DocumentInfo doc;
			doc.documentId = docIt.key();
			doc.objectId = docIt.value().objectId;
			doc.typeId = docIt.value().typeId;
			doc.url = docIt.value().url.toString();
			doc.name = docIt.value().name;
			doc.singleDocumentInstance = docIt.value().singleDocumentInstance;
			documents.append(doc);
		}

		documentMap.insert(userIt.key(), documents);
	}
}


QJsonObject CCollectionDocumentServiceComp::DocumentMapToJson(const DocumentMap& documentMap)
{
	QJsonArray usersArray;

	for (auto userIt = documentMap.constBegin(); userIt != documentMap.constEnd(); ++userIt){
		QJsonArray documentsArray;
		for (const DocumentInfo& documentInfo : userIt.value()){
			QJsonObject documentObject;
			documentObject.insert(s_documentIdKey, QString(documentInfo.documentId));
			documentObject.insert(s_objectIdKey, QString(documentInfo.objectId));
			documentObject.insert(s_typeIdKey, QString(documentInfo.typeId));
			documentObject.insert(s_urlKey, documentInfo.url);
			documentObject.insert(s_nameKey, documentInfo.name);
			documentObject.insert(s_singleDocumentInstanceKey, documentInfo.singleDocumentInstance);

			documentsArray.append(documentObject);
		}

		QJsonObject userObject;
		userObject.insert(s_userIdKey, QString(userIt.key()));
		userObject.insert(s_documentsKey, documentsArray);

		usersArray.append(userObject);
	}

	QJsonObject rootObject;
	rootObject.insert(s_usersKey, usersArray);

	return rootObject;
}


bool CCollectionDocumentServiceComp::DocumentMapFromJson(const QJsonObject& rootObject, DocumentMap& documentMap)
{
	documentMap.clear();

	const QJsonValue usersValue = rootObject.value(s_usersKey);
	if (!usersValue.isArray()){
		return false;
	}

	const QJsonArray usersArray = usersValue.toArray();
	for (const QJsonValue& userValue : usersArray){
		if (!userValue.isObject()){
			return false;
		}

		const QJsonObject userObject = userValue.toObject();
		if (!userObject.contains(s_userIdKey) || !userObject.contains(s_documentsKey)){
			return false;
		}

		const QJsonValue userIdValue = userObject.value(s_userIdKey);
		if (!userIdValue.isString()){
			return false;
		}

		const QByteArray userId = userIdValue.toString().toUtf8();

		const QJsonValue documentsValue = userObject.value(s_documentsKey);
		if (!documentsValue.isArray()){
			return false;
		}

		DocumentInfoList documents;

		const QJsonArray documentsArray = documentsValue.toArray();
		for (const QJsonValue& documentValue : documentsArray){
			if (!documentValue.isObject()){
				return false;
			}

			const QJsonObject documentObject = documentValue.toObject();
			if (!documentObject.value(s_documentIdKey).isString()
						|| documentObject.value(s_documentIdKey).toString().isEmpty()
						|| !documentObject.value(s_objectIdKey).isString()
						|| !documentObject.value(s_typeIdKey).isString()
						|| !documentObject.value(s_urlKey).isString()
						|| !documentObject.value(s_nameKey).isString()
						|| !documentObject.value(s_singleDocumentInstanceKey).isBool()){
				return false;
			}

			DocumentInfo documentInfo;
			documentInfo.documentId = documentObject.value(s_documentIdKey).toString().toUtf8();
			documentInfo.objectId = documentObject.value(s_objectIdKey).toString().toUtf8();
			documentInfo.typeId = documentObject.value(s_typeIdKey).toString().toUtf8();
			documentInfo.url = documentObject.value(s_urlKey).toString();
			documentInfo.name = documentObject.value(s_nameKey).toString();
			documentInfo.singleDocumentInstance = documentObject.value(s_singleDocumentInstanceKey).toBool(false);

			documents.append(documentInfo);
		}

		documentMap.insert(userId, documents);
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


bool CCollectionDocumentServiceComp::RestoreDocumentMap(const DocumentMap& documentMap)
{
	istd::CChangeNotifier notifier(this);

	RemoveAllDocuments();

	const bool isSingleCopyMode = IsSingleCopyMode();
	imtbase::IObjectCollection* collectionPtr = GetCollection();

	if (collectionPtr == nullptr){
		return false;
	}

	for (auto userIt = documentMap.constBegin(); userIt != documentMap.constEnd(); ++userIt){
		const QByteArray& userId = userIt.key();
		for (const DocumentInfo& documentInfo : userIt.value()){
			if (documentInfo.documentId.isEmpty()){
				continue;
			}

			RestoreDocument(documentInfo, userId, isSingleCopyMode, collectionPtr);
		}
	}

	return true;
}


void CCollectionDocumentServiceComp::RemoveAllDocuments()
{
	QMutexLocker locker(&m_mutex);
	QSet<int> undoModelIds;

	for (auto it = m_sharedDocuments.constBegin(); it != m_sharedDocuments.constEnd(); ++it){
		if (it.value().undoManagerModelId >= 0){
			undoModelIds.insert(it.value().undoManagerModelId);
		}
	}
	for (auto userIt = m_userDocuments.constBegin(); userIt != m_userDocuments.constEnd(); ++userIt){
		for (auto docIt = userIt.value().constBegin(); docIt != userIt.value().constEnd(); ++docIt){
			if (docIt.value().undoManagerModelId >= 0){
				undoModelIds.insert(docIt.value().undoManagerModelId);
			}
		}
	}
	for (int modelId : undoModelIds){
		m_undoManagerObserver.UnregisterModel(modelId);
	}

	m_userDocuments.clear();
	m_sharedDocuments.clear();
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


bool CCollectionDocumentServiceComp::RestoreSharedDocument(const DocumentInfo& documentInfo, const QByteArray& userId)
{
	if (documentInfo.objectId.isEmpty()){
		return false;
	}

	QMutexLocker locker(&m_mutex);
	auto sharedIt = m_sharedDocuments.find(documentInfo.objectId);
	if (sharedIt == m_sharedDocuments.end()){
		return false;
	}

	SharedDocumentData& shared = sharedIt.value();
	WorkingDocument& document = m_userDocuments[userId][documentInfo.documentId];

	document.objectId = documentInfo.objectId;
	document.typeId = shared.typeId;
	document.url = documentInfo.url.isEmpty() ? ObjectIdToUrl(documentInfo.objectId) : QUrl(documentInfo.url);
	document.name = documentInfo.name;
	document.objectPtr = shared.objectPtr;
	document.undoManagerPtr = shared.undoManagerPtr;
	document.isLoading = shared.isLoading;
	document.singleDocumentInstance = documentInfo.singleDocumentInstance;
	document.undoManagerModelId = -1;
	document.isDirty = shared.undoManagerPtr.IsValid()
				? (shared.undoManagerPtr->GetDocumentChangeFlag() != idoc::IDocumentStateComparator::DCF_EQUAL)
				: false;

	shared.refCount++;

	return true;
}


bool CCollectionDocumentServiceComp::RestoreDocument(
			const DocumentInfo& documentInfo,
			const QByteArray& userId,
			bool isSingleCopyMode,
			imtbase::IObjectCollection* collectionPtr)
{
	QUrl documentUrl = documentInfo.url.isEmpty() ? ObjectIdToUrl(documentInfo.objectId) : QUrl(documentInfo.url);
	int requestedRevision = -1;
	if (!documentInfo.objectId.isEmpty() && !documentInfo.url.isEmpty()){
		const QStringList pathParts = documentUrl.path().split('/', Qt::SkipEmptyParts);
		if (documentUrl.scheme() != QStringLiteral("collection")
				|| !documentUrl.host().isEmpty()
				|| pathParts.isEmpty()
				|| pathParts.first().toUtf8() != documentInfo.objectId
				|| pathParts.count() > 2){
			return false;
		}

		if (pathParts.count() == 2){
			bool conversionOk = false;
			requestedRevision = pathParts.last().toInt(&conversionOk);
			if (!conversionOk || requestedRevision < 0){
				return false;
			}
		}
	}

	if (isSingleCopyMode && requestedRevision < 0 && RestoreSharedDocument(documentInfo, userId)){
		return true;
	}

	QByteArray typeId = documentInfo.typeId;
	if (typeId.isEmpty()){
		typeId = collectionPtr->GetObjectTypeId(documentInfo.objectId);
	}
	if (typeId.isEmpty()){
		return false;
	}

	istd::IChangeableSharedPtr objectPtr;
	if (!documentInfo.objectId.isEmpty()){
		if (requestedRevision >= 0){
			const imtbase::IRevisionController* revisionControllerPtr = collectionPtr->GetRevisionController();
			if (revisionControllerPtr == nullptr){
				return false;
			}

			bool hasAvailableRevision = false;
			const imtbase::IRevisionController::RevisionInfoList revisionInfoList =
					revisionControllerPtr->GetRevisionInfoList(*collectionPtr, documentInfo.objectId);
			for (const imtbase::IRevisionController::RevisionInfo& revisionInfo : revisionInfoList){
				if (revisionInfo.revision == requestedRevision && revisionInfo.isRevisionAvailable){
					hasAvailableRevision = true;
					break;
				}
			}

			if (!hasAvailableRevision){
				return false;
			}

			idoc::MetaInfoPtr dataMetaInfoPtr;
			if (!revisionControllerPtr->GetRevisionData(*collectionPtr, documentInfo.objectId, requestedRevision, objectPtr, dataMetaInfoPtr)
					|| !objectPtr.IsValid()){
				return false;
			}
		}
		else if (!collectionPtr->GetObjectData(documentInfo.objectId, objectPtr) || !objectPtr.IsValid()){
			return false;
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
	document.objectId = documentInfo.objectId;
	document.typeId = typeId;
	document.url = documentUrl;
	document.name = documentInfo.name;
	document.objectPtr = objectPtr;
	document.undoManagerPtr = undoManagerPtr;
	document.isDirty = false;
	document.isLoading = true;
	document.singleDocumentInstance = documentInfo.singleDocumentInstance;
	document.undoManagerModelId = -1;

	if (isSingleCopyMode && requestedRevision < 0 && !documentInfo.objectId.isEmpty()){
		SharedDocumentData shared;
		shared.typeId = document.typeId;
		shared.name = document.name;
		shared.objectPtr = document.objectPtr;
		shared.undoManagerPtr = document.undoManagerPtr;
		shared.refCount = 1;
		shared.isLoading = false;
		shared.undoManagerModelId = document.undoManagerModelId;

		QMutexLocker locker(&m_mutex);
		m_sharedDocuments[documentInfo.objectId] = shared;
		m_userDocuments[userId][documentInfo.documentId] = document;
	}
	else{
		QMutexLocker locker(&m_mutex);
		m_userDocuments[userId][documentInfo.documentId] = document;
	}

	RegisterRestoredDocument(documentInfo, userId, isSingleCopyMode && requestedRevision < 0, undoManagerPtr);

	return true;
}


void CCollectionDocumentServiceComp::RegisterRestoredDocument(
			const DocumentInfo& documentInfo,
			const QByteArray& userId,
			bool isSingleCopyMode,
			const idoc::IUndoManagerSharedPtr& undoManagerPtr)
{
	auto& registeredDocument = m_userDocuments[userId][documentInfo.documentId];

	InitializeDocumentObservers(registeredDocument, userId, documentInfo.documentId);
	if (isSingleCopyMode && !documentInfo.objectId.isEmpty()){
		QMutexLocker locker(&m_mutex);
		if (m_sharedDocuments.contains(documentInfo.objectId)){
			m_sharedDocuments[documentInfo.objectId].undoManagerModelId = registeredDocument.undoManagerModelId;
		}
	}
	registeredDocument.isDirty = undoManagerPtr->GetDocumentChangeFlag() != idoc::IDocumentStateComparator::DCF_EQUAL;
	registeredDocument.isLoading = false;
}
} // namespace imtdoc
