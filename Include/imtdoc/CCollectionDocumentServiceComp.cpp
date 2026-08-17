// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtdoc/CCollectionDocumentServiceComp.h>


// Qt includes
#include <QtCore/QSet>

// ImtCore includes
#include <imtbase/imtbase.h>


namespace imtdoc
{


// public methods

// reimplemented (iser::ISerializable)

bool CCollectionDocumentServiceComp::Serialize(iser::IArchive& archive)
{
	struct SerializedDocument
	{
		QByteArray userId;
		QByteArray documentId;
		QByteArray objectId;
		QByteArray typeId;
		QString url;
		QString name;
		bool singleDocumentInstance = false;
	};

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

	if (archive.IsStoring()){
		QList<QByteArray> userIds;
		QMap<QByteArray, QList<SerializedDocument>> userDocuments;

		{
			QMutexLocker locker(&m_mutex);
			for (auto userIt = m_userDocuments.constBegin(); userIt != m_userDocuments.constEnd(); ++userIt){
				userIds.append(userIt.key());

				QList<SerializedDocument> documents;
				const WorkingDocumentList& docs = userIt.value();
				for (auto docIt = docs.constBegin(); docIt != docs.constEnd(); ++docIt){
					SerializedDocument doc;
					doc.userId = userIt.key();
					doc.documentId = docIt.key();
					doc.objectId = docIt.value().objectId;
					doc.typeId = docIt.value().typeId;
					doc.url = docIt.value().url.toString();
					doc.name = docIt.value().name;
					doc.singleDocumentInstance = docIt.value().singleDocumentInstance;
					documents.append(doc);
				}

				userDocuments.insert(userIt.key(), documents);
			}
		}

		int usersCount = imtbase::narrow_cast<int>(userIds.count());
		retVal = retVal && archive.BeginMultiTag(usersTag, userTag, usersCount);
		for (int i = 0; i < usersCount; ++i){
			const QByteArray& userId = userIds[i];
			QList<SerializedDocument> documents = userDocuments.value(userId);

			retVal = retVal && archive.BeginTag(userTag);

			retVal = retVal && archive.BeginTag(userIdTag);
			QByteArray storedUserId = userId;
			retVal = retVal && archive.Process(storedUserId);
			retVal = retVal && archive.EndTag(userIdTag);

			int documentsCount = imtbase::narrow_cast<int>(documents.count());
			retVal = retVal && archive.BeginMultiTag(documentsTag, documentTag, documentsCount);
			for (int j = 0; j < documentsCount; ++j){
				SerializedDocument& doc = documents[j];

				retVal = retVal && archive.BeginTag(documentTag);

				retVal = retVal && archive.BeginTag(documentIdTag);
				retVal = retVal && archive.Process(doc.documentId);
				retVal = retVal && archive.EndTag(documentIdTag);

				retVal = retVal && archive.BeginTag(objectIdTag);
				retVal = retVal && archive.Process(doc.objectId);
				retVal = retVal && archive.EndTag(objectIdTag);

				retVal = retVal && archive.BeginTag(typeIdTag);
				retVal = retVal && archive.Process(doc.typeId);
				retVal = retVal && archive.EndTag(typeIdTag);

				retVal = retVal && archive.BeginTag(urlTag);
				retVal = retVal && archive.Process(doc.url);
				retVal = retVal && archive.EndTag(urlTag);

				retVal = retVal && archive.BeginTag(nameTag);
				retVal = retVal && archive.Process(doc.name);
				retVal = retVal && archive.EndTag(nameTag);

				retVal = retVal && archive.BeginTag(singleDocumentInstanceTag);
				retVal = retVal && archive.Process(doc.singleDocumentInstance);
				retVal = retVal && archive.EndTag(singleDocumentInstanceTag);

				retVal = retVal && archive.EndTag(documentTag);
			}
			retVal = retVal && archive.EndTag(documentsTag);

			retVal = retVal && archive.EndTag(userTag);
		}
		retVal = retVal && archive.EndTag(usersTag);

		return retVal;
	}

	QList<SerializedDocument> serializedDocuments;
	int usersCount = 0;
	retVal = retVal && archive.BeginMultiTag(usersTag, userTag, usersCount);
	for (int i = 0; i < usersCount; ++i){
		retVal = retVal && archive.BeginTag(userTag);

		QByteArray userId;
		retVal = retVal && archive.BeginTag(userIdTag);
		retVal = retVal && archive.Process(userId);
		retVal = retVal && archive.EndTag(userIdTag);

		int documentsCount = 0;
		retVal = retVal && archive.BeginMultiTag(documentsTag, documentTag, documentsCount);
		for (int j = 0; j < documentsCount; ++j){
			SerializedDocument doc;
			doc.userId = userId;

			retVal = retVal && archive.BeginTag(documentTag);

			retVal = retVal && archive.BeginTag(documentIdTag);
			retVal = retVal && archive.Process(doc.documentId);
			retVal = retVal && archive.EndTag(documentIdTag);

			retVal = retVal && archive.BeginTag(objectIdTag);
			retVal = retVal && archive.Process(doc.objectId);
			retVal = retVal && archive.EndTag(objectIdTag);

			retVal = retVal && archive.BeginTag(typeIdTag);
			retVal = retVal && archive.Process(doc.typeId);
			retVal = retVal && archive.EndTag(typeIdTag);

			retVal = retVal && archive.BeginTag(urlTag);
			retVal = retVal && archive.Process(doc.url);
			retVal = retVal && archive.EndTag(urlTag);

			retVal = retVal && archive.BeginTag(nameTag);
			retVal = retVal && archive.Process(doc.name);
			retVal = retVal && archive.EndTag(nameTag);

			retVal = retVal && archive.BeginTag(singleDocumentInstanceTag);
			retVal = retVal && archive.Process(doc.singleDocumentInstance);
			retVal = retVal && archive.EndTag(singleDocumentInstanceTag);

			retVal = retVal && archive.EndTag(documentTag);

			serializedDocuments.append(doc);
		}
		retVal = retVal && archive.EndTag(documentsTag);

		retVal = retVal && archive.EndTag(userTag);
	}
	retVal = retVal && archive.EndTag(usersTag);

	if (!retVal){
		return false;
	}

	istd::CChangeNotifier notifier(this);

	const bool isSingleCopyMode = IsSingleCopyMode();
	imtbase::IObjectCollection* collectionPtr = GetCollection();
	if (collectionPtr == nullptr){
		QMutexLocker locker(&m_mutex);
		m_userDocuments.clear();
		m_sharedDocuments.clear();
		return false;
	}

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

	for (const SerializedDocument& serialized : serializedDocuments){
		if (serialized.documentId.isEmpty()){
			continue;
		}

		if (isSingleCopyMode){
			bool isSharedDocumentFound = false;
			{
				QMutexLocker locker(&m_mutex);
				auto sharedIt = m_sharedDocuments.find(serialized.objectId);
				if (sharedIt != m_sharedDocuments.end()){
					SharedDocumentData& shared = sharedIt.value();
					WorkingDocument& document = m_userDocuments[serialized.userId][serialized.documentId];

					document.objectId = serialized.objectId;
					document.typeId = shared.typeId;
					document.url = serialized.url.isEmpty() ? ObjectIdToUrl(serialized.objectId) : QUrl(serialized.url);
					document.name = serialized.name;
					document.objectPtr = shared.objectPtr;
					document.undoManagerPtr = shared.undoManagerPtr;
					document.isLoading = shared.isLoading;
					document.singleDocumentInstance = serialized.singleDocumentInstance;
					document.undoManagerModelId = -1;
					document.isDirty = shared.undoManagerPtr.IsValid()
						? (shared.undoManagerPtr->GetDocumentChangeFlag() != idoc::IDocumentStateComparator::DCF_EQUAL)
						: false;

					shared.refCount++;
					isSharedDocumentFound = true;
				}
			}

			if (isSharedDocumentFound){
				continue;
			}
		}

		QByteArray typeId = serialized.typeId;
		if (typeId.isEmpty()){
			typeId = collectionPtr->GetObjectTypeId(serialized.objectId);
		}
		if (typeId.isEmpty()){
			continue;
		}

		imtbase::IObjectCollection::DataPtr objectPtr;
		if (!serialized.objectId.isEmpty()){
			if (!collectionPtr->GetObjectData(serialized.objectId, objectPtr) || !objectPtr.IsValid()){
				continue;
			}
		}

		if (!objectPtr.IsValid()){
			objectPtr = CreateObject(typeId).PopInterfacePtr();
		}

		if (!objectPtr.IsValid()){
			continue;
		}

		idoc::IUndoManagerSharedPtr undoManagerPtr;
		undoManagerPtr.FromUnique(std::move(CreateUndoManager()));
		if (!undoManagerPtr.IsValid()){
			continue;
		}

		WorkingDocument document;
		document.objectId = serialized.objectId;
		document.typeId = typeId;
		document.url = serialized.url.isEmpty() ? ObjectIdToUrl(serialized.objectId) : QUrl(serialized.url);
		document.name = serialized.name;
		document.objectPtr = objectPtr;
		document.undoManagerPtr = undoManagerPtr;
		document.isDirty = false;
		document.isLoading = true;
		document.singleDocumentInstance = serialized.singleDocumentInstance;
		document.undoManagerModelId = -1;

		if (isSingleCopyMode){
			SharedDocumentData shared;
			shared.typeId = document.typeId;
			shared.name = document.name;
			shared.objectPtr = document.objectPtr;
			shared.undoManagerPtr = document.undoManagerPtr;
			shared.refCount = 1;
			shared.isLoading = false;
			shared.undoManagerModelId = document.undoManagerModelId;

			QMutexLocker locker(&m_mutex);
			m_sharedDocuments[serialized.objectId] = shared;
			m_userDocuments[serialized.userId][serialized.documentId] = document;
		}
		else{
			QMutexLocker locker(&m_mutex);
			m_userDocuments[serialized.userId][serialized.documentId] = document;
		}

		auto& registeredDocument = m_userDocuments[serialized.userId][serialized.documentId];

		InitializeDocumentObservers(registeredDocument, serialized.userId, serialized.documentId);
		if (isSingleCopyMode && !serialized.objectId.isEmpty()){
			QMutexLocker locker(&m_mutex);
			if (m_sharedDocuments.contains(serialized.objectId)){
				m_sharedDocuments[serialized.objectId].undoManagerModelId = registeredDocument.undoManagerModelId;
			}
		}
		registeredDocument.isDirty = undoManagerPtr->GetDocumentChangeFlag() != idoc::IDocumentStateComparator::DCF_EQUAL;
		registeredDocument.isLoading = false;
	}

	return true;
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


} // namespace imtdoc
