// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtdoc/CCollectionDocumentServiceComp.h>


// Qt includes
#include <QtCore/QDir>
#include <QtCore/QDirIterator>
#include <QtCore/QFileInfo>
#include <QtCore/QSet>
#include <QtCore/QStringList>

// ImtCore includes
#include <imtbase/imtbase.h>


namespace imtdoc
{


// public methods

// reimplemented (iser::ISerializable)

bool CCollectionDocumentServiceComp::Serialize(iser::IArchive& archive)
{
	DocumentMap documentMap;

	if (archive.IsStoring()){
		PrepareDocumentMap(documentMap);

		return SerializeDocumentMap(archive, documentMap);
	}

	if (!SerializeDocumentMap(archive, documentMap)){
		return false;
	}

	return RestoreDocumentMap(documentMap);
}


// protected methods

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


IDocumentService::OperationStatus CCollectionDocumentServiceComp::CloseDocumentInternal(
	const QByteArray& userId,
	const QByteArray& documentId)
{
	bool shouldRemoveStorageDirectory = false;

	{
		QMutexLocker locker(&m_mutex);
		OperationStatus validationStatus;
		if (!ValidateInputParams(userId, documentId, validationStatus)){
			return validationStatus;
		}
		const WorkingDocument& workingDocument = m_userDocuments[userId][documentId];
		const WorkingDocument& workingDocument = m_userDocuments[userId][documentId];

		shouldRemoveStorageDirectory = true;
		if (IsSingleCopyMode()
			&& !workingDocument.objectId.isEmpty()
			&& m_sharedDocuments.contains(workingDocument.objectId)){
			shouldRemoveStorageDirectory = m_sharedDocuments[workingDocument.objectId].refCount <= 1;
		}
	}

	OperationStatus status = BaseClass::CloseDocumentInternal(userId, documentId);

	if (status == OS_OK && shouldRemoveStorageDirectory){
		RemoveUndoManagerDocumentDirectory(documentId);
	}

	return status;
}


// private methods

bool CCollectionDocumentServiceComp::SerializeDocumentMap(iser::IArchive& archive, DocumentMap& documentMap) const
{
	static const iser::CArchiveTag usersTag("Users", "List of users", iser::CArchiveTag::TT_MULTIPLE);
	static const iser::CArchiveTag userTag("User", "Single user", iser::CArchiveTag::TT_GROUP, &usersTag);
	static const iser::CArchiveTag userIdTag("UserId", "User ID", iser::CArchiveTag::TT_LEAF, &userTag);
	static const iser::CArchiveTag documentsTag("Documents", "List of opened documents", iser::CArchiveTag::TT_MULTIPLE, &userTag);
	static const iser::CArchiveTag documentTag("Document", "Single opened document", iser::CArchiveTag::TT_GROUP, &documentsTag);
	static const iser::CArchiveTag documentIdTag("DocumentId", "Document ID", iser::CArchiveTag::TT_LEAF, &documentTag);
	static const iser::CArchiveTag objectIdTag("ObjectId", "Collection object ID", iser::CArchiveTag::TT_LEAF, &documentTag);
	static const iser::CArchiveTag typeIdTag("TypeId", "Object type ID", iser::CArchiveTag::TT_LEAF, &documentTag);
	static const iser::CArchiveTag urlTag("Url", "Document URL", iser::CArchiveTag::TT_LEAF, &documentTag);
	static const iser::CArchiveTag nameTag("Name", "Document name", iser::CArchiveTag::TT_LEAF, &documentTag);
	static const iser::CArchiveTag singleDocumentInstanceTag(
		"SingleDocumentInstance",
		"Single-document-instance mode flag",
		iser::CArchiveTag::TT_LEAF,
		&documentTag);

	bool retVal = true;
	bool isStoring = archive.IsStoring();

	QList<QByteArray> userIds = isStoring ? documentMap.keys() : QList<QByteArray>();

	if (!isStoring){
		documentMap.clear();
	}

	int usersCount = isStoring ? imtbase::narrow_cast<int>(userIds.count()) : 0;
	retVal = retVal && archive.BeginMultiTag(usersTag, userTag, usersCount);

	for (int i = 0; i < usersCount; ++i){
		QByteArray userId = isStoring ? userIds[i] : QByteArray();

		retVal = retVal && archive.BeginTag(userTag);

		retVal = retVal && archive.TagAndProcess(userIdTag, userId);

		DocumentInfoList documents = isStoring ? documentMap.value(userId) : DocumentInfoList();

		int documentsCount = isStoring ? imtbase::narrow_cast<int>(documents.count()) : 0;
		retVal = retVal && archive.BeginMultiTag(documentsTag, documentTag, documentsCount);

		for (int j = 0; j < documentsCount; ++j){
			DocumentInfo doc;
			if (isStoring){
				doc = documents[j];
			}

			retVal = retVal && archive.BeginTag(documentTag);

			retVal = retVal && archive.TagAndProcess(documentIdTag, doc.documentId);
			retVal = retVal && archive.TagAndProcess(objectIdTag, doc.objectId);
			retVal = retVal && archive.TagAndProcess(typeIdTag, doc.typeId);
			retVal = retVal && archive.TagAndProcess(urlTag, doc.url);
			retVal = retVal && archive.TagAndProcess(nameTag, doc.name);
			retVal = retVal && archive.TagAndProcess(singleDocumentInstanceTag, doc.singleDocumentInstance);

			retVal = retVal && archive.EndTag(documentTag);

			if (!isStoring){
				documents.append(doc);
			}
		}

		retVal = retVal && archive.EndTag(documentsTag);

		retVal = retVal && archive.EndTag(userTag);

		if (!isStoring){
			documentMap.insert(userId, documents);
		}
	}

	retVal = retVal && archive.EndTag(usersTag);

	return retVal;
}


void CCollectionDocumentServiceComp::PrepareDocumentMap(DocumentMap& documentMap) const
{
	documentMap.clear();

	QMutexLocker locker(&m_mutex);
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


bool CCollectionDocumentServiceComp::RestoreDocumentMap(const DocumentMap& documentMap)
{
	istd::CChangeNotifier notifier(this);

	const bool isSingleCopyMode = IsSingleCopyMode();
	imtbase::IObjectCollection* collectionPtr = GetCollection();
	if (collectionPtr == nullptr){
		ClearRestoredDocuments();
		return false;
	}

	ClearRestoredDocuments();

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


void CCollectionDocumentServiceComp::ClearRestoredDocuments()
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
	if (isSingleCopyMode && RestoreSharedDocument(documentInfo, userId)){
		return true;
	}

	QByteArray typeId = documentInfo.typeId;
	if (typeId.isEmpty()){
		typeId = collectionPtr->GetObjectTypeId(documentInfo.objectId);
	}
	if (typeId.isEmpty()){
		return false;
	}

	imtbase::IObjectCollection::DataPtr objectPtr;
	if (!documentInfo.objectId.isEmpty()){
		if (!collectionPtr->GetObjectData(documentInfo.objectId, objectPtr) || !objectPtr.IsValid()){
			return false;
		}
	}

	if (!objectPtr.IsValid()){
		objectPtr = CreateObject(typeId).PopInterfacePtr();
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
	document.url = documentInfo.url.isEmpty() ? ObjectIdToUrl(documentInfo.objectId) : QUrl(documentInfo.url);
	document.name = documentInfo.name;
	document.objectPtr = objectPtr;
	document.undoManagerPtr = undoManagerPtr;
	document.isDirty = false;
	document.isLoading = true;
	document.singleDocumentInstance = documentInfo.singleDocumentInstance;
	document.undoManagerModelId = -1;

	if (isSingleCopyMode && !documentInfo.objectId.isEmpty()){
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

	RegisterRestoredDocument(documentInfo, userId, isSingleCopyMode, undoManagerPtr);

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


void CCollectionDocumentServiceComp::RemoveUndoManagerDocumentDirectory(const QByteArray& documentId) const
{
	if (!m_undoManagerFolderCompPtr.IsValid() || documentId.isEmpty()){
		return;
	}

	const QString rootFolderPath = m_undoManagerFolderCompPtr->GetPath();
	if (rootFolderPath.isEmpty()){
		return;
	}

	QDir rootDirectory(rootFolderPath);
	if (!rootDirectory.exists()){
		return;
	}

	const QString documentDirectoryName(documentId);
	QStringList directoriesToRemove;
	QDirIterator it(rootFolderPath, QDir::Dirs | QDir::NoDotAndDotDot, QDirIterator::Subdirectories);
	while (it.hasNext()){
		const QString directoryPath = it.next();
		if (QFileInfo(directoryPath).fileName() != documentDirectoryName){
			continue;
		}

		directoriesToRemove.push_back(directoryPath);
	}

	for (const QString& directoryPath : directoriesToRemove){
		QDir(directoryPath).removeRecursively();
	}
}


} // namespace imtdoc
