#include <imtdb/CSqlDatabaseDocumentDelegateComp.h>


// Qt includes
#include <QtCore/QFile>
#include <QtCore/QRegularExpression>
#include <QtCore/QRegularExpressionMatch>

// ACF includes
#include <istd/TOptDelPtr.h>
#include <istd/CSystem.h>
#include <istd/CCrcCalculator.h>
#include <iprm/CParamsSet.h>
#include <iprm/CIdParam.h>
#include <iprm/TParamsPtr.h>
#include <iser/CJsonMemWriteArchive.h>
#include <imod/TModelWrap.h>
#include <iprm/IIdParam.h>

// ImtCore includes
#include <imtbase/CObjectCollection.h>
#include <imtbase/ICollectionFilter.h>
#include <imtdb/CComplexCollectionFilterConverter.h>


namespace imtdb
{


static const QByteArray s_idColumn = QByteArrayLiteral("Id");
static const QByteArray s_typeIdColumn = QByteArrayLiteral("TypeId");
static const QByteArray s_nameColumn = QByteArrayLiteral("Name");
static const QByteArray s_descriptionColumn = QByteArrayLiteral("Description");
static const QByteArray s_documentIdColumn = QByteArrayLiteral("DocumentId");
static const QByteArray s_documentColumn = QByteArrayLiteral("Document");
static const QByteArray s_addedColumn = QByteArrayLiteral("Added");
static const QByteArray s_lastModifiedColumn = QByteArrayLiteral("LastModified");

static QSet<QString> s_filterableColumns = { s_typeIdColumn, s_nameColumn, s_descriptionColumn, s_addedColumn, s_lastModifiedColumn};


// public methods

// reimplemented (imtdb::ISqlDatabaseObjectDelegate)

QByteArray CSqlDatabaseDocumentDelegateComp::GetCountQuery(const iprm::IParamsSet* paramsPtr) const
{
	QString filterQuery;
	if (paramsPtr != nullptr){
		if (!CreateFilterQuery(*paramsPtr, filterQuery)){
			return QByteArray();
		}
	}

	if (!m_tableSchemaAttrPtr.IsValid()){
		return QString("SELECT COUNT(*) FROM \"%1\" WHERE \"IsActive\" = true %2").arg(qPrintable(*m_tableNameAttrPtr)).arg(filterQuery).toUtf8();
	}

	return QString("SELECT COUNT(*) FROM %0.\"%1\" WHERE \"IsActive\" %2")
				.arg(qPrintable(*m_tableSchemaAttrPtr))
				.arg(qPrintable(*m_tableNameAttrPtr))
				.arg(filterQuery)
				.toUtf8();
}


QByteArray CSqlDatabaseDocumentDelegateComp::GetSelectionQuery(
			const QByteArray& objectId,
			int offset,
			int count,
			const iprm::IParamsSet* paramsPtr) const
{
	if (!objectId.isEmpty()){
		QString baseQuery = GetBaseSelectionQuery();

		return QString(
			baseQuery + QString(" AND \"DocumentId\" = '%1'").arg(qPrintable(objectId))).toUtf8();
	}

	QByteArray selectionQuery = BaseClass::GetSelectionQuery(objectId, offset, count, paramsPtr);

	return selectionQuery;
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

	if (!workingDocumentPtr.IsValid()){
		return retVal;
	}

	QByteArray objectId = proposedObjectId.isEmpty() ? QUuid::createUuid().toByteArray(QUuid::WithoutBraces) : proposedObjectId;

	retVal.query = PrepareInsertNewObjectQuery(
		typeId,
		objectId,
		objectName,
		objectDescription,
		*workingDocumentPtr,
		operationContextPtr,
		1);

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

	QByteArray retVal;

	QByteArray typeId = collection.GetObjectTypeId(objectId);
	QString objectName = collection.GetElementInfo(objectId, imtbase::ICollectionInfo::EIT_NAME).toString();
	QString objectDescription = collection.GetElementInfo(objectId, imtbase::ICollectionInfo::EIT_DESCRIPTION).toString();
	if (typeId.isEmpty()){
		return retVal;
	}

	retVal = PrepareInsertNewObjectQuery(
		typeId, objectId, objectName, objectDescription, object, operationContextPtr, countRevisionsQuery);

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
			const QByteArray& objectId,
			const QString& description,
			const imtbase::IOperationContext* /*operationContextPtr*/) const
{
	QByteArray retVal = QString("UPDATE \"%1\" SET \"Description\" = '%2' WHERE \"%3\" = '%4';")
				.arg(qPrintable(*m_tableNameAttrPtr))
				.arg(description)
				.arg(qPrintable(s_idColumn))
				.arg(qPrintable(objectId))
				.toUtf8();

	return retVal;
}


QByteArray CSqlDatabaseDocumentDelegateComp::GetSelectionByMetaInfoQuery(
			const QByteArray& metaInfoId,
			const QVariant& metaInfoValue) const
{
	return QString(R"(SELECT * FROM "%1" WHERE "IsActive" = true AND "DataMetaInfo"->>'%2' = '%3')")
				.arg(qPrintable(*m_tableNameAttrPtr))
				.arg(qPrintable(metaInfoId))
				.arg(qPrintable(metaInfoValue.toByteArray())).toUtf8();
}



QByteArray CSqlDatabaseDocumentDelegateComp::CreateUpdateMetaInfoQuery(const QSqlRecord& record) const
{
	istd::TDelPtr<istd::IChangeable> objectPtr = CreateObjectFromRecord(record);
	if (objectPtr == nullptr){
		return QByteArray();
	}

	QByteArray objectId = record.value(qPrintable(s_documentIdColumn)).toByteArray();
	QByteArray typeId = record.value(qPrintable(s_typeIdColumn)).toByteArray();

	QByteArray metaInfoRepresentation = QByteArrayLiteral("{}");

	if (m_metaInfoCreatorCompPtr.IsValid()){
		idoc::MetaInfoPtr metaInfoPtr;
		if (m_metaInfoCreatorCompPtr->CreateMetaInfo(objectPtr.GetPtr(), typeId, metaInfoPtr) && metaInfoPtr.IsValid()){
			if (m_jsonBasedMetaInfoDelegateCompPtr.IsValid()){
				metaInfoRepresentation = m_jsonBasedMetaInfoDelegateCompPtr->ToJsonRepresentation(*metaInfoPtr.GetPtr());
			}
		}
	}

	QByteArray query = QString(R"(UPDATE "%1" SET "DataMetaInfo" = '%2' WHERE "IsActive" = true AND "%3" = '%4')")
						.arg(qPrintable(*m_tableNameAttrPtr))
						.arg(qPrintable(metaInfoRepresentation))
						.arg(qPrintable(s_documentIdColumn))
						.arg(qPrintable(objectId))
						.toUtf8();

	return query;
}


// reimplemented (imtbase::IRevisionController)

imtbase::IRevisionController::RevisionInfoList CSqlDatabaseDocumentDelegateComp::GetRevisionInfoList(
			const imtbase::IObjectCollection& /*collection*/,
			const QByteArray& objectId) const
{
	imtbase::IRevisionController::RevisionInfoList revisionInfoList;

	if (!m_databaseEngineCompPtr.IsValid()){
		return imtbase::IRevisionController::RevisionInfoList();
	}

	iprm::CIdParam idParam;
	idParam.SetId(objectId);
	iprm::CParamsSet paramSet;
	paramSet.SetEditableParameter("Id", &idParam);
	QByteArray query = CreateObjectHistoryQuery(0, -1, &paramSet);

	QSqlError sqlError;
	QSqlQuery sqlQuery = m_databaseEngineCompPtr->ExecSqlQuery(query, &sqlError);
	if (sqlError.type() != QSqlError::NoError){
		SendErrorMessage(0, sqlError.text(), "Database collection");

		return imtbase::IRevisionController::RevisionInfoList();
	}

	while (sqlQuery.next()){
		QSqlRecord revisionRecord = sqlQuery.record();
		RevisionInfo revisionInfo;

		if (revisionRecord.contains("RevisionNumber")){
			revisionInfo.revision = revisionRecord.value("RevisionNumber").toInt();
		}

		if (revisionRecord.contains("LastModified")){
			revisionInfo.timestamp = revisionRecord.value("LastModified").toDateTime();
		}

		revisionInfo.isRevisionAvailable = true;

		revisionInfoList.push_back(revisionInfo);
	}

	return revisionInfoList;
}


int CSqlDatabaseDocumentDelegateComp::BackupObject(
			const imtbase::IObjectCollection& /*collection*/,
			const imtbase::ICollectionInfo::Id& /*objectId*/,
			const QString& /*userComment*/) const
{
	return -1;
}


bool CSqlDatabaseDocumentDelegateComp::RestoreObject(
			imtbase::IObjectCollection& collection,
			const imtbase::ICollectionInfo::Id& objectId,
			int revision) const
{
	Q_ASSERT_X(false, "CSqlDatabaseDocumentDelegateComp::RestoreObject", "TODO: Method operation needs to be checked. Method not tested.");

	if (!m_databaseEngineCompPtr.IsValid()){
		return false;
	}

	imtbase::IRevisionController::RevisionInfoList revisionInfoList = GetRevisionInfoList(collection, objectId);
	for (const imtbase::IRevisionController::RevisionInfo& revisionInfo : revisionInfoList){
		if (revisionInfo.revision == revision){
			iprm::CIdParam idParam;
			idParam.SetId(objectId);
			iprm::CParamsSet paramSet;
			paramSet.SetEditableParameter("Id", &idParam);
			QByteArray query = QString("UPDATE \"%1\" SET \"IsActive\" = false WHERE \"%2\" = '%3'")
				.arg(qPrintable(*m_tableNameAttrPtr))
				.arg(qPrintable(s_documentIdColumn))
				.arg(qPrintable(objectId))
				.toUtf8();

			query += QString(R"(; UPDATE "%1" SET "IsActive" = true WHERE "%2" = '%3' AND "RevisionNumber" = %4)")
				.arg(qPrintable(*m_tableNameAttrPtr))
				.arg(qPrintable(s_documentIdColumn))
				.arg(qPrintable(objectId))
				.arg(revision)
				.toUtf8();

			QSqlError sqlError;
			QSqlQuery sqlQuery = m_databaseEngineCompPtr->ExecSqlQuery(query, &sqlError);
			if (sqlError.type() != QSqlError::NoError){
				SendErrorMessage(0, sqlError.text(), "Database collection");

				return false;
			}

			return true;
		}
	}

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

QByteArray CSqlDatabaseDocumentDelegateComp::PrepareInsertNewObjectQuery(
	const QByteArray& typeId,
	const QByteArray& objectId,
	const QString& objectName,
	const QString& objectDescription,
	const istd::IChangeable& object,
	const imtbase::IOperationContext* operationContextPtr,
	const QVariant& revisionArgument) const
{
	QByteArray retVal;

	Q_ASSERT(revisionArgument.type() == QMetaType::Int || revisionArgument.type() == QMetaType::QByteArray);

	QByteArray documentContent;
	if (!WriteDataToMemory(typeId, object, documentContent)){
		SendCriticalMessage(0, "Document data could not be written to the memory");

		return retVal;
	}

	documentContent = documentContent.toBase64();

	quint32 checksum = istd::CCrcCalculator::GetCrcFromData((const quint8*)documentContent.constData(), documentContent.size());

	// Insert new entry into the document list table:
	QString query = QString("UPDATE \"%1\" SET \"IsActive\" = false WHERE \"%2\" = '%3'")
		.arg(qPrintable(*m_tableNameAttrPtr))
		.arg(qPrintable(s_documentIdColumn))
		.arg(qPrintable(objectId));

	QByteArray metaInfoRepresentation = QByteArrayLiteral("{}");

	if (m_metaInfoCreatorCompPtr.IsValid()){
		idoc::MetaInfoPtr metaInfoPtr;
		if (m_metaInfoCreatorCompPtr->CreateMetaInfo(&object, typeId, metaInfoPtr) && metaInfoPtr.IsValid()){
			if (m_jsonBasedMetaInfoDelegateCompPtr.IsValid()){
				metaInfoRepresentation = m_jsonBasedMetaInfoDelegateCompPtr->ToJsonRepresentation(*metaInfoPtr.GetPtr());
			}
		}
	}
	
	query += QString("; INSERT INTO \"%1\"(\"TypeId\", \"DocumentId\", \"Name\", \"Description\", \"Document\", \"DataMetaInfo\", \"CollectionMetaInfo\", \"Checksum\", \"LastModified\", \"IsActive\", \"RevisionNumber\") VALUES('%2', '%3', '%4', '%5', '%6', '%7', '%8', %9, '%10', %11, %12)")
		.arg(qPrintable(*m_tableNameAttrPtr))
		.arg(qPrintable(typeId))
		.arg(qPrintable(objectId))
		.arg(objectName)
		.arg(objectDescription)
		.arg(qPrintable(documentContent))
		.arg(SqlEncode(metaInfoRepresentation))
		.arg("{}")
		.arg(checksum)
		.arg(QDateTime::currentDateTimeUtc().toString(Qt::ISODateWithMs))
		.arg("true");

	if (revisionArgument.type() == QMetaType::Int){
		query = query.arg(revisionArgument.toInt());
	}
	else{
		query = query.arg(qPrintable(revisionArgument.toByteArray()));
	}

	retVal = query.toUtf8();

	QByteArray operationDescriptionQuery = CreateOperationDescriptionQuery(objectId, operationContextPtr);
	if (!operationDescriptionQuery.isEmpty()){
		retVal += "; " + operationDescriptionQuery;
	}

	return retVal;
}


QByteArray CSqlDatabaseDocumentDelegateComp::CreateOperationDescriptionQuery(const QByteArray& objectId, const imtbase::IOperationContext* operationContextPtr) const
{
	if (operationContextPtr != nullptr){
		imtbase::IOperationContext* operationPtr = const_cast<imtbase::IOperationContext*>(operationContextPtr);
		if (operationPtr != nullptr){
			imtbase::CObjectCollection* changeCollectionPtr = dynamic_cast<imtbase::CObjectCollection*>(operationPtr->GetChangesCollection());

			QByteArray json;
			{
				iser::CJsonMemWriteArchive archive(m_versionInfoCompPtr.GetPtr());
				if (!changeCollectionPtr->Serialize(archive)){
					qDebug() << QString("Unable to serialize a change object collection");
				}
				else{
					json = archive.GetData();
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
	for (const QString& ext : supportedExts){
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
	for (const QString& ext : supportedExts){
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
			(SELECT "LastModified" FROM "%2" as t1 WHERE "RevisionNumber" = 1 AND %1"%2"."DocumentId" = t1."DocumentId" LIMIT 1) as "Added" FROM "%2" WHERE "IsActive" = true)";

	QString schemaPrefix;
	if (m_tableSchemaAttrPtr.IsValid()){
		schemaPrefix = QString("%1.").arg(qPrintable(*m_tableSchemaAttrPtr));
	}

	return query.arg(schemaPrefix).arg(qPrintable(*m_tableNameAttrPtr));
}


idoc::MetaInfoPtr CSqlDatabaseDocumentDelegateComp::CreateObjectMetaInfo(const QByteArray& /*typeId*/) const
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
		if (!typeId.isEmpty()){
			filterQuery = QString("\"TypeId\" = '%1'").arg(qPrintable(typeId)).toUtf8();
		}
	}

	return true;
}


bool CSqlDatabaseDocumentDelegateComp::CreateSortQuery(const imtbase::ICollectionFilter& collectionFilter, QString& sortQuery) const
{
	QByteArray columnId;
	QByteArray sortOrder;

	if (!collectionFilter.GetSortingInfoIds().isEmpty()){
		columnId = collectionFilter.GetSortingInfoIds().first();
	}

	switch (collectionFilter.GetSortingOrder()){
	case imtbase::ICollectionFilter::SO_ASC:
		sortOrder = "ASC";
		break;
	case imtbase::ICollectionFilter::SO_DESC:
		sortOrder = "DESC";
		break;
	default:
		break;
	}

	if (!columnId.isEmpty() && !sortOrder.isEmpty()){
		if (columnId == s_lastModifiedColumn || columnId == s_addedColumn || columnId == s_nameColumn || columnId == s_descriptionColumn || columnId == s_typeIdColumn){
			sortQuery = QString("ORDER BY \"%1\" %2").arg(qPrintable(columnId)).arg(qPrintable(sortOrder));
		}
		else{
			sortQuery = QString("ORDER BY \"DataMetaInfo\"->>\'%1\' %2").arg(qPrintable(columnId)).arg(qPrintable(sortOrder));
		}
	}

	return true;
}


bool CSqlDatabaseDocumentDelegateComp::CreateSortQuery(const imtbase::IComplexCollectionFilter& collectionFilter, QString& sortQuery) const
{
	bool retVal = BaseClass::CreateSortQuery(collectionFilter, sortQuery);

	SubstituteFieldIds(sortQuery);

	return retVal;
}


bool CSqlDatabaseDocumentDelegateComp::CreateFilterQuery(const iprm::IParamsSet& filterParams, QString& filterQuery) const
{
	bool retVal = true;
	QString objectFilterQuery;

	iprm::IParamsSet::Ids paramIds = filterParams.GetParamIds();

	QString textFilterQuery;
	QString timeFilterQuery;
	QString additionalFilters;

	if (paramIds.contains("ComplexFilter")){
		iprm::TParamsPtr<imtbase::IComplexCollectionFilter> complexFilterParamPtr(&filterParams, "ComplexFilter");
		if (complexFilterParamPtr.IsValid()){
			retVal = CreateTextFilterQuery(*complexFilterParamPtr, textFilterQuery);

			if (!retVal){
				return false;
			}

			SubstituteFieldIds(textFilterQuery);

			retVal = CreateTimeFilterQuery(complexFilterParamPtr->GetTimeFilter(), timeFilterQuery);
			if (!retVal){
				return false;
			}
		}
	}
	else{
		if (paramIds.contains("ObjectFilter")){
			iprm::TParamsPtr<iprm::IParamsSet> objectFilterParamPtr(&filterParams, "ObjectFilter");
			if (objectFilterParamPtr.IsValid()){
				retVal = CreateObjectFilterQuery(*objectFilterParamPtr, objectFilterQuery);
				if (!retVal){
					return false;
				}
			}
		}

		if (paramIds.contains("Filter")){
			iprm::TParamsPtr<imtbase::ICollectionFilter> collectionFilterParamPtr(&filterParams, "Filter");
			if (collectionFilterParamPtr.IsValid()){
				retVal = CreateTextFilterQuery(*collectionFilterParamPtr, textFilterQuery);
				if (!retVal){
					return false;
				}
			}
		}

		if (paramIds.contains("TimeFilter")){
			iprm::TParamsPtr<imtbase::ITimeFilterParam> timeFilterParamPtr(&filterParams, "TimeFilter");
			if (timeFilterParamPtr.IsValid()){
				retVal = CreateTimeFilterQuery(*timeFilterParamPtr, timeFilterQuery);
				if (!retVal){
					return false;
				}
			}
		}

		additionalFilters = CreateAdditionalFiltersQuery(filterParams);
	}


	if (!objectFilterQuery.isEmpty()){
		filterQuery += " AND (" + objectFilterQuery + ")";
	}

	if (!textFilterQuery.isEmpty()){
		filterQuery += " AND (" + textFilterQuery + ")";
	}

	if (!timeFilterQuery.isEmpty()){
		filterQuery += " AND (" + timeFilterQuery + ")";
	}

	if (!additionalFilters.isEmpty()){
		filterQuery += " AND (" + additionalFilters + ")";
	}

	return true;
}


bool CSqlDatabaseDocumentDelegateComp::CreateTextFilterQuery(const imtbase::ICollectionFilter& collectionFilter, QString& textFilterQuery) const
{
	QByteArrayList filteringColumnIds = collectionFilter.GetFilteringInfoIds();
	if (filteringColumnIds.isEmpty()){
		return true;
	}

	QString textFilter = collectionFilter.GetTextFilter();
	if (!textFilter.isEmpty()){
		if (filteringColumnIds.contains(s_typeIdColumn)){
			textFilterQuery += QString("%0\"%1\" ILIKE '%%2%'").arg(textFilterQuery.isEmpty() ? "" : " OR ").arg(qPrintable(s_typeIdColumn)).arg(textFilter);
			filteringColumnIds.removeOne(s_typeIdColumn);
		}

		if (filteringColumnIds.contains(s_nameColumn)){
			textFilterQuery += QString("%0\"%1\" ILIKE '%%2%'").arg(textFilterQuery.isEmpty() ? "" : " OR ").arg(qPrintable(s_nameColumn)).arg(textFilter);
			filteringColumnIds.removeOne(s_nameColumn);
		}

		if (filteringColumnIds.contains(s_descriptionColumn)){
			textFilterQuery += QString("%0\"%1\" ILIKE '%%2%'").arg(textFilterQuery.isEmpty() ? "" : " OR ").arg(qPrintable(s_descriptionColumn)).arg(textFilter);
			filteringColumnIds.removeOne(s_descriptionColumn);
		}

		for (int i = 0; i < filteringColumnIds.count(); ++i){
			textFilterQuery += QString("%0\"DataMetaInfo\"->>\'%1\' ILIKE '%%2%'").arg(textFilterQuery.isEmpty() ? "" : " OR ").arg(qPrintable(filteringColumnIds[i])).arg(textFilter);
		}
	}

	return true;
}


bool CSqlDatabaseDocumentDelegateComp::CreateTimeFilterQuery(const imtbase::ITimeFilterParam& timeFilter, QString& timeFilterQuery) const
{
	QString addedStrQuery = QString(R"((SELECT "LastModified" FROM "%1" as temp WHERE "RevisionNumber" = 1 AND "%1"."DocumentId" = temp."DocumentId" LIMIT 1))").arg(qPrintable(*m_tableNameAttrPtr));

	switch (timeFilter.GetTimeUnit()){
	case imtbase::ITimeFilterParam::TU_CUSTOM:
		break;
	case imtbase::ITimeFilterParam::TU_HOUR:
		switch(timeFilter.GetInterpretationMode()){
		case imtbase::ITimeFilterParam::IM_CURRENT:
			timeFilterQuery += QString(R"((date_trunc('hour', %1) = date_trunc('hour', current_timestamp at time zone 'utc')))").arg(addedStrQuery);
			break;
		case imtbase::ITimeFilterParam::IM_LAST:
			timeFilterQuery += QString(R"((date_trunc('hour', %1) = date_trunc('hour', current_timestamp at time zone 'utc') - interval '1 hour'))").arg(addedStrQuery);
			break;
		case imtbase::ITimeFilterParam::IM_FOR:
			timeFilterQuery += QString(R"((%1 >= current_timestamp at time zone 'utc' - interval '1 hour' and %1 <= current_timestamp at time zone 'utc'))").arg(addedStrQuery);
			break;
		}
		break;
	case imtbase::ITimeFilterParam::TU_DAY:
		switch(timeFilter.GetInterpretationMode()){
		case imtbase::ITimeFilterParam::IM_CURRENT:
			timeFilterQuery += QString(R"((date_trunc('day', %1) = date_trunc('day', current_timestamp at time zone 'utc')))").arg(addedStrQuery);
			break;
		case imtbase::ITimeFilterParam::IM_LAST:
			timeFilterQuery += QString(R"((date_trunc('day', %1) = date_trunc('day', current_timestamp at time zone 'utc') - interval '1 day'))").arg(addedStrQuery);
			break;
		case imtbase::ITimeFilterParam::IM_FOR:
			timeFilterQuery += QString(R"((%1 >= current_timestamp at time zone 'utc' - interval '1 day' and %1 <= current_timestamp at time zone 'utc'))").arg(addedStrQuery);
			break;
		}
		break;
	case imtbase::ITimeFilterParam::TU_WEEK:
		switch(timeFilter.GetInterpretationMode()){
		case imtbase::ITimeFilterParam::IM_CURRENT:
			timeFilterQuery += QString(R"((date_trunc('week', %1) = date_trunc('week', current_timestamp at time zone 'utc')))").arg(addedStrQuery);
			break;
		case imtbase::ITimeFilterParam::IM_LAST:
			timeFilterQuery += QString(R"((date_trunc('week', %1) = date_trunc('week', current_timestamp at time zone 'utc') - interval '1 week'))").arg(addedStrQuery);
			break;
		case imtbase::ITimeFilterParam::IM_FOR:
			timeFilterQuery += QString(R"((%1 >= current_timestamp at time zone 'utc' - interval '1 week' and %1 <= current_timestamp at time zone 'utc'))").arg(addedStrQuery);
			break;
		}
		break;
	case imtbase::ITimeFilterParam::TU_MONTH:
		switch(timeFilter.GetInterpretationMode()){
		case imtbase::ITimeFilterParam::IM_CURRENT:
			timeFilterQuery += QString(R"((date_trunc('month', %1) = date_trunc('month', current_timestamp at time zone 'utc')))").arg(addedStrQuery);
			break;
		case imtbase::ITimeFilterParam::IM_LAST:
			timeFilterQuery += QString(R"((date_trunc('month', %1) = date_trunc('month', current_timestamp at time zone 'utc') - interval '1 month'))").arg(addedStrQuery);
			break;
		case imtbase::ITimeFilterParam::IM_FOR:
			timeFilterQuery += QString(R"((%1 >= current_timestamp at time zone 'utc' - interval '1 month' and %1 <= current_timestamp at time zone 'utc'))").arg(addedStrQuery);
			break;
		}

		break;
	case imtbase::ITimeFilterParam::TU_YEAR:
		switch(timeFilter.GetInterpretationMode()){
		case imtbase::ITimeFilterParam::IM_CURRENT:
			timeFilterQuery += QString(R"((date_trunc('year', %1) = date_trunc('year', current_timestamp at time zone 'utc')))").arg(addedStrQuery);
			break;
		case imtbase::ITimeFilterParam::IM_LAST:
			timeFilterQuery += QString(R"((date_trunc('year', %1) = date_trunc('year', current_timestamp at time zone 'utc') - interval '1 year'))").arg(addedStrQuery);
			break;
		case imtbase::ITimeFilterParam::IM_FOR:
			timeFilterQuery += QString(R"((%1 >= current_timestamp at time zone 'utc' - interval '1 year' and %1 <= current_timestamp at time zone 'utc'))").arg(addedStrQuery);
			break;
		}
		break;
	}

	if (timeFilterQuery.isEmpty()){
		imtbase::CTimeRange timeRange = timeFilter.GetTimeRange();
		if (!timeRange.IsNull()){
			timeFilterQuery += QString(R"(date(%0) >= date('%1') AND date(%0) <= date('%2'))")
			.arg(addedStrQuery)
				.arg(timeRange.GetBeginTime().toString(Qt::ISODateWithMs))
				.arg(timeRange.GetEndTime().toString(Qt::ISODateWithMs));
		}
	}

	return true;
}


QByteArray CSqlDatabaseDocumentDelegateComp::CreateObjectHistoryQuery(
			int offset,
			int count,
			const iprm::IParamsSet* paramsPtr) const
{
	iprm::TParamsPtr<iprm::IIdParam> idParamPtr(paramsPtr, "Id");
	if (idParamPtr.IsValid()){
		QByteArray objectId = idParamPtr->GetId();

		QByteArray paginationQuery;
		CreatePaginationQuery(offset, count, paginationQuery);

		return QString(R"((SELECT * FROM "%1" WHERE "%2" = '%3' %4) ORDER BY "RevisionNumber" DESC;)")
			.arg(qPrintable(*m_tableNameAttrPtr))
			.arg(qPrintable(*m_objectIdColumnAttrPtr))
			.arg(qPrintable(objectId))
			.arg(qPrintable(paginationQuery))
			.toUtf8();
	}

	return QByteArray();
}


// protected methods

bool CSqlDatabaseDocumentDelegateComp::CreateTextFilterQuery(const imtbase::IComplexCollectionFilter& collectionFilter, QString& textFilterQuery) const
{
	textFilterQuery = CComplexCollectionFilterConverter::CreateSqlFilterQuery(collectionFilter);

	return true;
}


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


void CSqlDatabaseDocumentDelegateComp::SubstituteFieldIds(QString& query) const
{
	static QRegularExpression regexp("(\\\"[^\\\"]{1,}\\\")");

	QStringList list;
	for (const QRegularExpressionMatch& match : regexp.globalMatch(query)){
		QStringList capturedList = match.capturedTexts();
		list.append(capturedList.first());
	}

	for (const QString& item : list){
		QString substitute = item;
		substitute.replace("\"", "");

		if (!s_filterableColumns.contains(substitute)){
			substitute = QString("\"DataMetaInfo\"->>'%1'").arg(substitute);
			query.replace(item, substitute);
		}
	}
}


} // namespace imtdb


