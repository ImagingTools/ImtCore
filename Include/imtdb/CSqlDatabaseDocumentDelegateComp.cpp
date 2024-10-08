#include <imtdb/CSqlDatabaseDocumentDelegateComp.h>


// Qt includes
#include <QtCore/QFile>

// ACF includes
#include <istd/TOptDelPtr.h>
#include <istd/CSystem.h>
#include <istd/CCrcCalculator.h>
#include <iprm/TParamsPtr.h>
#include <iser/CJsonMemWriteArchive.h>
#include <imod/TModelWrap.h>

// ImtCore includes
#include <imtbase/ICollectionFilter.h>
#include <imtbase/CObjectCollection.h>


namespace imtdb
{


static const QByteArray s_idColumn = QByteArrayLiteral("Id");
static const QByteArray s_typeIdColumn = QByteArrayLiteral("TypeId");
static const QByteArray s_documentIdColumn = QByteArrayLiteral("DocumentId");
static const QByteArray s_documentColumn = QByteArrayLiteral("Document");


// public methods

// reimplemented (imtdb::ISqlDatabaseObjectDelegate)

QByteArray CSqlDatabaseDocumentDelegateComp::GetSelectionQuery(
			const QByteArray& objectId,
			int offset,
			int count,
			const iprm::IParamsSet* paramsPtr) const
{
	if (!objectId.isEmpty()){
		QString baseQuery = GetBaseSelectionQuery();

		return QString(
			baseQuery + QString(" WHERE \"DocumentId\" = '%1' AND \"IsActive\" = true").arg(qPrintable(objectId))).toUtf8();
	}

	return BaseClass::GetSelectionQuery(objectId, offset, count, paramsPtr);
}


istd::IChangeable* CSqlDatabaseDocumentDelegateComp::CreateObjectFromRecord(const QSqlRecord& record) const
{
	if (!m_databaseEngineCompPtr.IsValid()){
		Q_ASSERT_X(false, "CSqlDatabaseDocumentDelegateComp::CreateObjectFromRecord", "No database engine was registered");

		return nullptr;
	}

	if (!m_documentFactoriesCompPtr.IsValid()){
		Q_ASSERT_X(false, "CSqlDatabaseDocumentDelegateComp::CreateObjectFromRecord", "No document factories were registered");

		return nullptr;
	}

	QByteArray typeId = GetObjectTypeId(GetObjectIdFromRecord(record));

	istd::TDelPtr<istd::IChangeable> documentPtr = CreateObject(typeId);
	if (!documentPtr.IsValid()){
		Q_ASSERT_X(false, "CSqlDatabaseDocumentDelegateComp::CreateObjectFromRecord", qPrintable(QString("Document instance could not be created for the type: '%1'").arg(qPrintable(typeId))));

		return nullptr;
	}

	if (record.contains(s_documentColumn)){
		QByteArray documentContent = record.value(qPrintable(s_documentColumn)).toByteArray();

		documentContent = QByteArray::fromBase64(documentContent);

		if (ReadDataFromMemory(typeId, documentContent, *documentPtr)){
			return documentPtr.PopPtr();
		}
	}

	return nullptr;
}


imtdb::IDatabaseObjectDelegate::NewObjectQuery CSqlDatabaseDocumentDelegateComp::CreateNewObjectQuery(
			const QByteArray& typeId,
			const QByteArray& proposedObjectId,
			const QString& objectName,
			const QString& objectDescription,
			const istd::IChangeable* valuePtr,
			const imtbase::IOperationContext* operationContextPtr) const
{
	NewObjectQuery retVal;

	istd::TOptDelPtr<const istd::IChangeable> workingDocumentPtr;

	// If the document value is not null, use this for saving into the database. This is the use case 'Insert an existing document into the database':
	if (valuePtr != nullptr){
		workingDocumentPtr.SetPtr(valuePtr, false);
	}
	// Otherwise create a new document instance of the related type:
	else{
		workingDocumentPtr.SetPtr(CreateObject(typeId), true);
	}

	quint32 checksum = 0;
	QByteArray documentContent;

	// Even if we use the external document storage, we should write the data to memory for CRC-checksum calculation:
	if (workingDocumentPtr.IsValid()){
		if (!WriteDataToMemory(typeId, *workingDocumentPtr, documentContent)){
			SendCriticalMessage(0, "Document data could not be written to the memory");

			return retVal;
		}

		checksum = istd::CCrcCalculator::GetCrcFromData((const quint8*)documentContent.constData(), documentContent.size());
	}
	else{
		SendCriticalMessage(0, "Document instance is invalid. SQL-query could not be created");
	}

	// Create the document-ID:
	QByteArray objectId = proposedObjectId.isEmpty() ? QUuid::createUuid().toByteArray(QUuid::WithoutBraces) : proposedObjectId;

	// Create ID for the document data revision:
	QByteArray revisionUuid = QUuid::createUuid().toByteArray(QUuid::WithoutBraces);

	// Insert new entry into the document list table:
	retVal.query = QString("UPDATE \"%1\" SET \"IsActive\" = false WHERE \"%2\" = '%3'")
		.arg(qPrintable(*m_tableNameAttrPtr))
		.arg(s_documentIdColumn)
		.arg(objectId)
		.toUtf8();

	QByteArray metaInfoRepresentation;

	if (m_metaInfoCreateorCompPtr.IsValid()){
		idoc::MetaInfoPtr metaInfoPtr;
		if (m_metaInfoCreateorCompPtr->CreateMetaInfo(valuePtr, typeId, metaInfoPtr) && metaInfoPtr.IsValid()){
			if (m_jsonBasedMetaInfoDelegateCompPtr.IsValid()){
				metaInfoRepresentation = m_jsonBasedMetaInfoDelegateCompPtr->ToJsonRepresentation(*metaInfoPtr.GetPtr());
			}
		}
	}

	if (metaInfoRepresentation.isEmpty()){
		return NewObjectQuery();
	}

	retVal.query += QString("; INSERT INTO \"%1\"(\"TypeId\", \"DocumentId\", \"Name\", \"Description\", \"Document\", \"DataMetaInfo\", \"CollectionMetaInfo\", \"Checksum\", \"LastModified\", \"IsActive\", \"RevisionNumber\") VALUES('%2', '%3', '%4', '%5', '%6', '%7', '%8', %9, '%10', %11, %12)")
				.arg(qPrintable(*m_tableNameAttrPtr))
				.arg(qPrintable(typeId))
				.arg(qPrintable(objectId))
				.arg(objectName)
				.arg(objectDescription)
				.arg(documentContent)
				.arg(metaInfoRepresentation)
				.arg("{}")
				.arg(checksum)
				.arg(QDateTime::currentDateTimeUtc().toString(Qt::ISODateWithMs))
				.arg("true")
				.arg(1)
				.toUtf8();

	QByteArray operationDescriptionQuery = CreateOperationDescriptionQuery(objectId, operationContextPtr);
	if (!operationDescriptionQuery.isEmpty()){
		retVal.query += "; " + operationDescriptionQuery;
	}

	retVal.objectName = objectName;

	return retVal;
}


QByteArray CSqlDatabaseDocumentDelegateComp::CreateDeleteObjectQuery(
			const imtbase::IObjectCollection& /*collection*/,
			const QByteArray& objectId,
			const imtbase::IOperationContext* /*operationContextPtr*/) const
{
	QByteArray retVal = QString("DELETE FROM \"%1\" WHERE \"%2\" = '%3';").arg(qPrintable(*m_tableNameAttrPtr)).arg(qPrintable(s_documentIdColumn)).arg(qPrintable(objectId)).toUtf8();

	return retVal;
}


QByteArray CSqlDatabaseDocumentDelegateComp::CreateUpdateObjectQuery(
			const imtbase::IObjectCollection& collection,
			const QByteArray& objectId,
			const istd::IChangeable& object,
			const imtbase::IOperationContext* operationContextPtr,
			bool /*useExternDelegate*/) const
{
	// Get number of the revisions of the document in the database:
	QByteArray countRevisionsQuery = QString("(SELECT COUNT(*) FROM \"%1\" WHERE \"%2\" = '%3') + 1")
				.arg(qPrintable(*m_tableNameAttrPtr))
				.arg(qPrintable(s_documentIdColumn))
				.arg(qPrintable(objectId))
				.toUtf8();

	QSqlError error;
	QSqlQuery countQuery = m_databaseEngineCompPtr->ExecSqlQuery(countRevisionsQuery, &error);

	quint64 revisionsCount = 0;
	if (error.type() == QSqlError::NoError){
		if (countQuery.first()){
			revisionsCount = countQuery.value(0).toULongLong();
		}
	}
	else{
		SendErrorMessage(0, "The number of revisions of the document could not be queried", "Database Manager");

		return QByteArray();
	}

	QByteArray retVal;

	QByteArray typeId = collection.GetObjectTypeId(objectId);
	QString objectName = collection.GetElementInfo(objectId, imtbase::ICollectionInfo::EIT_NAME).toString();
	QString objectDescription = collection.GetElementInfo(objectId, imtbase::ICollectionInfo::EIT_DESCRIPTION).toString();
	if (typeId.isEmpty()){
		return retVal;
	}

	QByteArray documentContent;
	if (WriteDataToMemory(typeId, object, documentContent)){
		quint32 checksum = istd::CCrcCalculator::GetCrcFromData((const quint8*)documentContent.constData(), documentContent.size());

		// Insert new entry into the document list table:
		retVal = QString("UPDATE \"%1\" SET \"IsActive\" = false WHERE \"%2\" = '%3'")
			.arg(qPrintable(*m_tableNameAttrPtr))
			.arg(s_documentIdColumn)
			.arg(objectId)
			.toUtf8();

		QByteArray metaInfoRepresentation;

		if (m_metaInfoCreateorCompPtr.IsValid()){
			idoc::MetaInfoPtr metaInfoPtr;
			if (m_metaInfoCreateorCompPtr->CreateMetaInfo(&object, typeId, metaInfoPtr) && metaInfoPtr.IsValid()){
				if (m_jsonBasedMetaInfoDelegateCompPtr.IsValid()){
					metaInfoRepresentation = m_jsonBasedMetaInfoDelegateCompPtr->ToJsonRepresentation(*metaInfoPtr.GetPtr());
				}
			}
		}

		if (metaInfoRepresentation.isEmpty()){
			return QByteArray();
		}

		retVal += QString("; INSERT INTO \"%1\"(\"TypeId\", \"DocumentId\", \"Name\", \"Description\", \"Document\", \"DataMetaInfo\", \"CollectionMetaInfo\", \"Checksum\", \"LastModified\", \"IsActive\", \"RevisionNumber\") VALUES('%2', '%3', '%4', '%5', '%6', '%7', '%8', %9, '%10', %11, %12)")
			.arg(qPrintable(*m_tableNameAttrPtr))
			.arg(qPrintable(typeId))
			.arg(qPrintable(objectId))
			.arg(objectName)
			.arg(objectDescription)
			.arg(documentContent)
			.arg(metaInfoRepresentation)
			.arg("{}")
			.arg(checksum)
			.arg(QDateTime::currentDateTimeUtc().toString(Qt::ISODateWithMs))
			.arg("true")
			.arg(countRevisionsQuery)
			.toUtf8();

		QByteArray operationDescriptionQuery = CreateOperationDescriptionQuery(objectId, operationContextPtr);
		if (!operationDescriptionQuery.isEmpty()){
			retVal += "; " + operationDescriptionQuery;
		}
	}

	return retVal;
}


QByteArray CSqlDatabaseDocumentDelegateComp::CreateRenameObjectQuery(
			const imtbase::IObjectCollection& /*collection*/,
			const QByteArray& objectId,
			const QString& newObjectName,
			const imtbase::IOperationContext* /*operationContextPtr*/) const
{
	QByteArray retVal = QString("UPDATE \"%1\" SET \"Name\" = '%2' WHERE \"%3\" = '%4';")
				.arg(qPrintable(*m_tableNameAttrPtr))
				.arg(newObjectName)
				.arg(qPrintable(s_idColumn))
				.arg(qPrintable(objectId))
				.toUtf8();

	return retVal;
}


QByteArray CSqlDatabaseDocumentDelegateComp::CreateDescriptionObjectQuery(
			const imtbase::IObjectCollection& /*collection*/,
			const QByteArray& /*objectId*/,
			const QString& /*description*/,
			const imtbase::IOperationContext* /*operationContextPtr*/) const
{
	QByteArray retVal;

	return retVal;
}


// reimplemented (imtbase::IRevisionController)

imtbase::IRevisionController::RevisionInfoList CSqlDatabaseDocumentDelegateComp::GetRevisionInfoList(
			const imtbase::IObjectCollection& /*collection*/,
			const QByteArray& objectId) const
{
	//if (!m_databaseEngineCompPtr.IsValid()){
	//	return imtbase::IRevisionController::RevisionInfoList();
	//}

	//if ((*m_revisionsTableNameAttrPtr).isEmpty()){
	//	return imtbase::IRevisionController::RevisionInfoList();
	//}

	//QString revisionListQuery = QString("SELECT \"RevisionNumber\", \"LastModified\", \"Comment\" from \"%1\" WHERE \"%2\" = '%3'")
	//			.arg(qPrintable(*m_revisionsTableNameAttrPtr))
	//			.arg(qPrintable(s_documentIdColumn))
	//			.arg(qPrintable(objectId));

	//QSqlError sqlError;
	//QSqlQuery sqlQuery = m_databaseEngineCompPtr->ExecSqlQuery(revisionListQuery.toUtf8(), &sqlError);
	//if (sqlError.type() != QSqlError::NoError){
	//	SendErrorMessage(0, sqlError.text(), "Database collection");

	//	return imtbase::IRevisionController::RevisionInfoList();
	//}

	imtbase::IRevisionController::RevisionInfoList revisionInfoList;

	//while (sqlQuery.next()){
	//	QSqlRecord revisionRecord = sqlQuery.record();
	//	RevisionInfo revisionInfo;

	//	if (revisionRecord.contains("RevisionNumber")){
	//		revisionInfo.revision = revisionRecord.value("RevisionNumber").toLongLong();
	//	}

	//	if (revisionRecord.contains("LastModified")){
	//		revisionInfo.timestamp = revisionRecord.value("LastModified").toDateTime();
	//	}

	//	if (revisionRecord.contains("Comment")){
	//		revisionInfo.comment = revisionRecord.value("Comment").toString();
	//	}

	//	revisionInfo.isRevisionAvailable = true;

	//	revisionInfoList.push_back(revisionInfo);
	//}

	return revisionInfoList;
}


int CSqlDatabaseDocumentDelegateComp::BackupObject(
			const imtbase::IObjectCollection& /*collection*/,
			const imtbase::ICollectionInfo::Id& objectId,
			const QString& userComment) const
{
	//if (!m_databaseEngineCompPtr.IsValid()){
	//	return -1;
	//}

	//if ((*m_revisionsTableNameAttrPtr).isEmpty()){
	//	return -1;
	//}

	//if (objectId.isEmpty()){
	//	return -1;
	//}

	//QByteArray lastRevisionQuery = QString("SELECT \"%1\" from \"%2\" WHERE \"%1\" in (SELECT \"LastRevisionId\" from \"%3\" WHERE \"%1\" = '%4')")
	//			.arg(qPrintable(s_idColumn))
	//			.arg(qPrintable(*m_revisionsTableNameAttrPtr))
	//			.arg(qPrintable(*m_tableNameAttrPtr))
	//			.arg(qPrintable(objectId))
	//			.toUtf8();

	//QByteArray lastRevisionNumberQuery = QString("SELECT \"RevisionNumber\" from \"%2\" WHERE \"%1\" in (SELECT \"LastRevisionId\" from \"%3\" WHERE \"%1\" = '%4')")
	//			.arg(qPrintable(s_idColumn))
	//			.arg(qPrintable(*m_revisionsTableNameAttrPtr))
	//			.arg(qPrintable(*m_tableNameAttrPtr))
	//			.arg(qPrintable(objectId))
	//			.toUtf8();

	//QByteArray updateCommentQuery = QString("UPDATE \"%1\" SET \"Comment\" = '%2' WHERE \"%3\" in (%4)")
	//			.arg(qPrintable(*m_revisionsTableNameAttrPtr))
	//			.arg(userComment)
	//			.arg(qPrintable(s_idColumn))
	//			.arg(qPrintable(lastRevisionQuery))
	//			.toUtf8();

	//m_databaseEngineCompPtr->ExecSqlQuery(updateCommentQuery);

	//QSqlQuery queryResult = m_databaseEngineCompPtr->ExecSqlQuery(lastRevisionNumberQuery);
	//if (queryResult.next()){
	//	QSqlRecord record = queryResult.record();
	//	if (record.contains("RevisionNumber")){
	//		return record.value("RevisionNumber").toInt();
	//	}
	//}

	return -1;
}


bool CSqlDatabaseDocumentDelegateComp::RestoreObject(
			imtbase::IObjectCollection& collection,
			const imtbase::ICollectionInfo::Id& objectId,
			int revision) const
{
	//if (!m_databaseEngineCompPtr.IsValid()){
	//	return false;
	//}

	//if (!m_documentFactoriesCompPtr.IsValid()){
	//	return false;
	//}

	//QString revisionIdQuery  = QString("SELECT \"%5\" from \"%1\" WHERE \"%2\" = '%3' AND \"RevisionNumber\" = %4")
	//			.arg(qPrintable(*m_revisionsTableNameAttrPtr))
	//			.arg(qPrintable(s_documentIdColumn))
	//			.arg(qPrintable(objectId))
	//			.arg(revision)
	//			.arg(qPrintable(s_idColumn));

	//QByteArray revisionUuid;
	//QSqlQuery queryResult = m_databaseEngineCompPtr->ExecSqlQuery(revisionIdQuery.toUtf8());
	//if (queryResult.next()){
	//	QSqlRecord record = queryResult.record();
	//	if (record.contains(qPrintable(s_idColumn))){
	//		revisionUuid = record.value(qPrintable(s_idColumn)).toByteArray();
	//	}
	//}

	//if (revisionUuid.isEmpty()){
	//	return false;
	//}

	//QString setActiveRevisionQuery = QString("UPDATE \"%1\" SET \"LastRevisionId\" = '%2' WHERE \"%3\" = '%4';")
	//			.arg(qPrintable(*m_tableNameAttrPtr))
	//			.arg(qPrintable(revisionUuid))
	//			.arg(qPrintable(s_idColumn))
	//			.arg(qPrintable(objectId))
	//			.toUtf8();

	//istd::CChangeNotifier changeNotifier(&collection);

	//QSqlError sqlError;
	//m_databaseEngineCompPtr->ExecSqlQuery(setActiveRevisionQuery.toUtf8(), &sqlError);

	//return sqlError.type() == QSqlError::NoError;

	return false;
}


bool CSqlDatabaseDocumentDelegateComp::ExportObject(
			const imtbase::IObjectCollection& /*collection*/,
			const imtbase::ICollectionInfo::Id& /*objectId*/,
			int /*revision*/,
			const QString& /*filePath*/) const
{
	return false;
}


// protected methods

QByteArray CSqlDatabaseDocumentDelegateComp::CreateOperationDescriptionQuery(const QByteArray& objectId, const imtbase::IOperationContext* operationContextPtr) const
{
	if (operationContextPtr != nullptr){
		imtbase::IOperationContext* operationPtr = const_cast<imtbase::IOperationContext*>(operationContextPtr);
		if (operationPtr != nullptr){
			imtbase::CObjectCollection* changeCollectionPtr = dynamic_cast<imtbase::CObjectCollection*>(operationPtr->GetChangesCollection());

			QByteArray json;
			{
				iser::CJsonMemWriteArchive archive(json, m_versionInfoCompPtr.GetPtr());
				if (!changeCollectionPtr->Serialize(archive)){
					qDebug() << QString("Unable to serialize a change object collection");
				}
			}

			QString operationDescription = json;

			imtbase::IOperationContext::IdentifableObjectInfo objectInfo = operationPtr->GetOperationOwnerId();
			return QString(R"(UPDATE "%1" SET "OwnerId" = '%2', "OwnerName" = '%3', "OperationDescription" = '%4' WHERE "IsActive" = true AND "DocumentId" = '%5')")
				.arg(qPrintable(*m_tableNameAttrPtr))
				.arg(qPrintable(objectInfo.id))
				.arg(objectInfo.name)
				.arg(operationDescription)
				.arg(qPrintable(objectId))
				.toUtf8();
		}
	}

	return QByteArray();
}


istd::IChangeable* CSqlDatabaseDocumentDelegateComp::CreateObject(const QByteArray& typeId) const
{
	if (!m_typesCompPtr.IsValid()){
		return nullptr;
	}

	Q_ASSERT_X(m_documentFactoriesCompPtr.GetCount() == m_typesCompPtr->GetOptionsCount(), __FILE__, "Number of factories and resource types doesn't match");

	for (int i = 0; i < m_typesCompPtr->GetOptionsCount(); ++i){
		if (i < m_documentFactoriesCompPtr.GetCount()){
			if (typeId == m_typesCompPtr->GetOptionId(i)){
				return m_documentFactoriesCompPtr.CreateInstance(i);
			}
		}
	}
	return nullptr;
}


bool CSqlDatabaseDocumentDelegateComp::WriteDataToMemory(const QByteArray& typeId, const istd::IChangeable& object, QByteArray& data) const
{
	const ifile::IFilePersistence* documentPersistencePtr = FindDocumentPersistence(typeId);
	if (documentPersistencePtr == nullptr){
		SendErrorMessage(0, QString("Document data could not be written due no persistence was found for the type: %1").arg(qPrintable(typeId)));

		return false;
	}

	QString tempFolder = QDir::tempPath() + "/ImtCore/SqlDatabaseDocumentDelegate/" + QUuid::createUuid().toString();
	if (!istd::CSystem::EnsurePathExists(tempFolder)){
		return false;
	}

	int flags = ifile::IFilePersistence::QF_FILE | ifile::IFilePersistence::QF_SAVE;

	QStringList supportedExts;
	documentPersistencePtr->GetFileExtensions(supportedExts, &object, flags);

	QString fileName = QUuid::createUuid().toString();
	QString workingExtension;
	for (const QString ext : supportedExts){
		QString filePath = fileName + "." + ext;
		if (documentPersistencePtr->IsOperationSupported(&object, &filePath, flags, false)){
			workingExtension = ext;
			break;
		}
	}

	fileName += "." + workingExtension;

	QString filePath = tempFolder + "/" + fileName;

	int operationState = documentPersistencePtr->SaveToFile(object, filePath);
	if (operationState != ifile::IFilePersistence::OS_OK){
		istd::CSystem::RemoveDirectory(tempFolder);

		return false;
	}

	QFile documentFile(filePath);
	if (!documentFile.open(QFile::ReadOnly)){
		istd::CSystem::RemoveDirectory(tempFolder);

		return false;
	}

	data = documentFile.readAll();

	documentFile.close();

	istd::CSystem::RemoveDirectory(tempFolder);

	return true;
}


bool CSqlDatabaseDocumentDelegateComp::ReadDataFromMemory(const QByteArray& typeId, const QByteArray& data, istd::IChangeable& object) const
{
	const ifile::IFilePersistence* documentPersistencePtr = FindDocumentPersistence(typeId);
	if (documentPersistencePtr == nullptr){
		SendErrorMessage(0, QString("Document data could not be read due no persistence was found for the type: %1").arg(qPrintable(typeId)));

		return false;
	}

	QString tempFolder = QDir::tempPath() + "/ImtCore/SqlDatabaseDocumentDelegate/" + QUuid::createUuid().toString();

	if (!istd::CSystem::EnsurePathExists(tempFolder)){
		return false;
	}

	int flags = ifile::IFilePersistence::QF_FILE | ifile::IFilePersistence::QF_SAVE;

	QStringList supportedExts;
	documentPersistencePtr->GetFileExtensions(supportedExts, &object, flags);

	QString fileName = QUuid::createUuid().toString();
	QString workingExtension;
	for (const QString ext : supportedExts){
		QString filePath = fileName + "." + ext;
		if (documentPersistencePtr->IsOperationSupported(&object, &filePath, flags, false)){
			workingExtension = ext;
			break;
		}
	}

	fileName += "." + workingExtension;

	QString filePath = tempFolder + "/" + fileName;

	QFile documentFile(filePath);
	if (!documentFile.open(QFile::WriteOnly)){
		istd::CSystem::RemoveDirectory(tempFolder);

		return false;
	}

	documentFile.write(data);

	documentFile.close();

	int operationState = documentPersistencePtr->LoadFromFile(object, filePath);
	if (operationState != ifile::IFilePersistence::OS_OK){
		istd::CSystem::RemoveDirectory(tempFolder);

		return false;
	}

	istd::CSystem::RemoveDirectory(tempFolder);

	return true;
}


// reimplemented (imtdb::CSqlDatabaseObjectDelegateCompBase)

QString CSqlDatabaseDocumentDelegateComp::GetBaseSelectionQuery() const
{

	QString query = R"(SELECT "TypeId", "DocumentId", "Name", "Description", "Document", "DataMetaInfo", "CollectionMetaInfo", "Checksum", "LastModified", "IsActive", "RevisionNumber",
			(SELECT "LastModified" FROM "%2" as t1 WHERE "RevisionNumber" = 1 AND %1"%2"."DocumentId" = t1."DocumentId" LIMIT 1) as "Added" FROM "%2")";

	QString schemaPrefix;
	if (m_tableSchemaAttrPtr.IsValid()){
		schemaPrefix = QString("%1.").arg(qPrintable(*m_tableSchemaAttrPtr));
	}

	return query.arg(schemaPrefix).arg(*m_tableNameAttrPtr);
}


idoc::MetaInfoPtr CSqlDatabaseDocumentDelegateComp::CreateObjectMetaInfo(const QByteArray& typeId) const
{
	return idoc::MetaInfoPtr(new imod::TModelWrap<idoc::CStandardDocumentMetaInfo>);
}


bool CSqlDatabaseDocumentDelegateComp::SetObjectMetaInfoFromRecord(const QSqlRecord& record, idoc::IDocumentMetaInfo& metaInfo) const
{
	if (record.contains("DataMetaInfo")){
		QByteArray metaInfoRepresentation = record.value("DataMetaInfo").toByteArray();
		if (m_jsonBasedMetaInfoDelegateCompPtr.IsValid()){
			idoc::MetaInfoPtr metaInfoPtr = m_jsonBasedMetaInfoDelegateCompPtr->FromJsonRepresentation(metaInfoRepresentation);
			if (metaInfoPtr.IsValid()){
				metaInfo.CopyFrom(*metaInfoPtr);
			}
		}
	}

	return true;
}


bool CSqlDatabaseDocumentDelegateComp::CreateObjectFilterQuery(const iprm::IParamsSet& filterParams, QString& filterQuery) const
{
	filterQuery.clear();

	iprm::TParamsPtr<imtbase::ICollectionFilter> collectionFilterParamPtr(&filterParams, "Filter");
	if (collectionFilterParamPtr.IsValid()){
		QByteArray typeId = collectionFilterParamPtr->GetObjectTypeId();

		filterQuery = QString("\"TypeId\" = '%1'").arg(qPrintable(typeId)).toUtf8();
	}

	return true;
}


// protected methods

const ifile::IFilePersistence* CSqlDatabaseDocumentDelegateComp::FindDocumentPersistence(const QByteArray& typeId) const
{
	int persistenceIndex = -1;

	if (m_typesCompPtr.IsValid()){
		for (int i = 0; i < m_typesCompPtr->GetOptionsCount(); ++i){
			if (typeId == m_typesCompPtr->GetOptionId(i)){
				persistenceIndex = i;
				break;
			}
		}
	}

	if ((persistenceIndex >= 0) && persistenceIndex < m_documentPersistenceListCompPtr.GetCount()){
		return m_documentPersistenceListCompPtr[persistenceIndex];
	}

	return nullptr;
}


} // namespace imtdb


