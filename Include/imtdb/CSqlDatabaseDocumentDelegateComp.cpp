#include <imtdb/CSqlDatabaseDocumentDelegateComp.h>


// Qt includes
#include <QtCore/QFile>
#include <QtCore/QRegularExpression>
#include <QtCore/QRegularExpressionMatch>

// ACF includes
#include <istd/TOptDelPtr.h>
#include <istd/CSystem.h>
#include <istd/CCrcCalculator.h>
#include <imod/TModelWrap.h>
#include <iser/CJsonMemWriteArchive.h>
#include <iprm/CParamsSet.h>
#include <iprm/CIdParam.h>
#include <iprm/TParamsPtr.h>
#include <iprm/IIdParam.h>
#include <idoc/CStandardDocumentMetaInfo.h>

// ImtCore includes
#include <imtbase/ICollectionFilter.h>
#include <imtdb/CComplexCollectionFilterConverter.h>
#include <imtcol/CDocumentCollectionFilter.h>
#include <imtcol/IObjectTypeIdFilter.h>


namespace imtdb
{


static const QByteArray s_idColumn = QByteArrayLiteral("Id");
static const QByteArray s_typeIdColumn = QByteArrayLiteral("TypeId");
static const QByteArray s_documentIdColumn = QByteArrayLiteral("DocumentId");
static const QByteArray s_nameColumn = QByteArrayLiteral("Name");
static const QByteArray s_descriptionColumn = QByteArrayLiteral("Description");
static const QByteArray s_documentColumn = QByteArrayLiteral("Document");
static const QByteArray s_addedColumn = QByteArrayLiteral("Added");
static const QByteArray s_lastModifiedColumn = QByteArrayLiteral("TimeStamp");

static QSet<QString> s_filterableColumns = { s_idColumn, s_typeIdColumn, s_documentIdColumn, s_nameColumn, s_descriptionColumn, s_addedColumn, s_lastModifiedColumn};


// public methods

// reimplemented (imtdb::ISqlDatabaseObjectDelegate)

QByteArray CSqlDatabaseDocumentDelegateComp::GetCountQuery(const iprm::IParamsSet* paramsPtr) const
{
	QString baseQuery = GetSelectionQuery(QByteArray(), 0, -1, paramsPtr);
	
	return QString("SELECT COUNT(*) FROM (%1) as t").arg(baseQuery).toUtf8();
}


QByteArray CSqlDatabaseDocumentDelegateComp::GetSelectionQuery(
	const QByteArray& objectId,
	int offset,
	int count,
	const iprm::IParamsSet* paramsPtr) const
{
	if (!objectId.isEmpty()){
		return GetObjectSelectionQuery(objectId, paramsPtr);
	}
	
	QByteArray selectionQuery = BaseClass::GetSelectionQuery(objectId, offset, count, paramsPtr);
	
	iprm::TParamsPtr<imtbase::IComplexCollectionFilter> complexFilterParamPtr(paramsPtr, "ComplexFilter");
	if (complexFilterParamPtr.IsValid()){
		QByteArrayList fieldIds = complexFilterParamPtr->GetDistinctFieldsList();
		
		QString distinctString;
		Q_ASSERT(fieldIds.count() < 2);
		for (int i = 0; i < fieldIds.count(); i++){
			QString fieldId = fieldIds[i];
			fieldId = s_filterableColumns.contains(fieldId) ? QString("\"%1\"").arg(fieldId) : QString("\"DataMetaInfo\"->>'%1'").arg(fieldId);
			
			distinctString += i > 0 ? QString(", %1").arg(fieldId) : fieldId;
		}
		
		if (!distinctString.isEmpty()){
			selectionQuery = QString("SELECT DISTINCT ON (%1) * FROM %2 as collectiondata").arg(distinctString, selectionQuery).toUtf8();
		}
	}
	
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
		
		if (*m_useBase64AttrPtr){
			documentContent = QByteArray::fromBase64(documentContent);
		}
		
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
	QByteArray retVal = QString("UPDATE \"%1\" SET \"State\" = 'Disabled' WHERE \"%2\" = '%3';")
	.arg(qPrintable(*m_tableNameAttrPtr), qPrintable(s_documentIdColumn), qPrintable(objectId)).toUtf8();
	
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
	QByteArray countRevisionsQuery = QString("(SELECT MAX((\"RevisionInfo\"->>'RevisionNumber')::int) + 1 FROM \"%1\" WHERE \"%2\" = '%3')")
										 .arg(qPrintable(*m_tableNameAttrPtr), qPrintable(s_documentIdColumn), qPrintable(objectId)).toUtf8();
	
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
	.arg(qPrintable(*m_tableNameAttrPtr), newObjectName, qPrintable(s_documentIdColumn), qPrintable(objectId))
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
	.arg(qPrintable(*m_tableNameAttrPtr), description, qPrintable(s_documentIdColumn), qPrintable(objectId))
		.toUtf8();
	
	return retVal;
}


QByteArray CSqlDatabaseDocumentDelegateComp::GetSelectionByMetaInfoQuery(
	const QByteArray& metaInfoId,
	const QVariant& metaInfoValue) const
{
	return QString(R"(SELECT * FROM "%1" WHERE ("State" = 'Active' OR "State" = 'Disabled') AND "DataMetaInfo"->>'%2' = '%3';)")
	.arg(qPrintable(*m_tableNameAttrPtr), qPrintable(metaInfoId), qPrintable(metaInfoValue.toByteArray())).toUtf8();
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
				if (!m_jsonBasedMetaInfoDelegateCompPtr->ToJsonRepresentation(*metaInfoPtr.GetPtr(), metaInfoRepresentation)){
					SendErrorMessage(0, "Meta information could not be converted to the JSON-based representation");
					
					return QByteArray();
				}
			}
		}
	}

	QByteArray query = QString(R"(UPDATE "%1" SET "DataMetaInfo" = '%2' WHERE "State" = 'Active' AND "%3" = '%4';)")
							.arg(qPrintable(*m_tableNameAttrPtr), qPrintable(SqlEncode(metaInfoRepresentation)), qPrintable(s_documentIdColumn), qPrintable(objectId))
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

	iprm::CParamsSet filterParams;
	imtcol::CDocumentCollectionFilter documentFilter;
	documentFilter.SetDocumentId(objectId);
	documentFilter.AddDocumentState(imtcol::IDocumentCollectionFilter::DS_ACTIVE);
	documentFilter.AddDocumentState(imtcol::IDocumentCollectionFilter::DS_INACTIVE);
	filterParams.SetEditableParameter("DocumentFilter", &documentFilter);
	
	QByteArray query = GetSelectionQuery(objectId, 0, -1, &filterParams);
	
	QSqlError sqlError;
	QSqlQuery sqlQuery = m_databaseEngineCompPtr->ExecSqlQuery(query, &sqlError);
	if (sqlError.type() != QSqlError::NoError){
		SendErrorMessage(0, sqlError.text(), "Database collection");
		
		return imtbase::IRevisionController::RevisionInfoList();
	}
	
	while (sqlQuery.next()){
		QSqlRecord revisionRecord = sqlQuery.record();
		RevisionInfo revisionInfo;
		
		if (revisionRecord.contains("RevisionInfo")){
			QByteArray revisionValueJson = revisionRecord.value("RevisionInfo").toByteArray();
			QJsonObject infoData = QJsonDocument::fromJson(revisionValueJson).object();
			revisionInfo.revision = infoData.value("RevisionNumber").toInt();
			revisionInfo.user = infoData.value("OwnerName").toString();
			
			QJsonValue operationDescriptionValue = infoData.value("OperationDescription");
			if (operationDescriptionValue.isObject()){
				QJsonObject operationObj = operationDescriptionValue.toObject();
				
				revisionInfo.comment = QJsonDocument(operationObj).toJson();
			}
			else{
				revisionInfo.comment = infoData.value("OperationDescription").toString();
			}
		}
		
		if (revisionRecord.contains("TimeStamp")){
			revisionInfo.timestamp = revisionRecord.value("TimeStamp").toDateTime();
		}
		
		if (revisionRecord.contains("State")){
			revisionInfo.isRevisionAvailable = (revisionRecord.value("State").toString() == "Active" || revisionRecord.value("State").toString() == "Disabled");
		}
		
		revisionInfoList.push_back(revisionInfo);
	}
	
	return revisionInfoList;
}


int CSqlDatabaseDocumentDelegateComp::BackupRevision(
	const imtbase::IObjectCollection& /*collection*/,
	const imtbase::ICollectionInfo::Id& /*objectId*/,
	const QString& /*userComment*/) const
{
	return -1;
}


bool CSqlDatabaseDocumentDelegateComp::RestoreRevision(
	imtbase::IObjectCollection& collection,
	const imtbase::ICollectionInfo::Id& objectId,
	int revision) const
{
	if (!m_databaseEngineCompPtr.IsValid()){
		return false;
	}
	
	istd::IChangeable::ChangeSet changeSet(imtbase::IObjectCollection::CF_OBJECT_DATA_CHANGED);
	changeSet.SetChangeInfo(imtbase::IObjectCollection::CN_OBJECT_DATA_CHANGED, objectId);
	
	istd::CChangeNotifier changeNotifier(&collection, &changeSet);
	
	QByteArray query = QString("UPDATE \"%1\" SET \"State\" = 'InActive' WHERE \"%2\" = '%3';")
							.arg(qPrintable(*m_tableNameAttrPtr), qPrintable(s_documentIdColumn), qPrintable(objectId)).toUtf8();
	
	query += QString(R"(UPDATE "%1" SET "State" = 'Active' WHERE "%2" = '%3' AND ("RevisionInfo"->>'RevisionNumber')::int = %4;)")
				 .arg(qPrintable(*m_tableNameAttrPtr), qPrintable(s_documentIdColumn), qPrintable(objectId)).arg(revision).toUtf8();
	
	QSqlError sqlError;
	m_databaseEngineCompPtr->ExecSqlQuery(query, &sqlError);
	if (sqlError.type() != QSqlError::NoError){
		SendErrorMessage(0, sqlError.text(), "Database collection");
		
		return false;
	}
	
	return true;
}


bool CSqlDatabaseDocumentDelegateComp::ExportRevision(
	const imtbase::IObjectCollection& /*collection*/,
	const imtbase::ICollectionInfo::Id& /*objectId*/,
	int /*revision*/,
	const QString& /*filePath*/) const
{
	return false;
}


bool CSqlDatabaseDocumentDelegateComp::DeleteRevision(
	imtbase::IObjectCollection& collection,
	const imtbase::ICollectionInfo::Id& objectId,
	int revision) const
{
	QByteArray checkCurrentRevisionQuery = QString("SELECT * FROM \"%1\" WHERE \"DocumentId\" = '%2' AND \"State\" = 'InActive';")
											.arg(qPrintable(*m_tableNameAttrPtr), qPrintable(objectId)).toUtf8();
	
	QSqlError sqlError;
	QSqlQuery sqlQuery = m_databaseEngineCompPtr->ExecSqlQuery(checkCurrentRevisionQuery, &sqlError);
	if (sqlError.type() != QSqlError::NoError){
		SendErrorMessage(0, sqlError.text(), "Database collection");
		
		return false;
	}
	
	if (!sqlQuery.next()){
		return false;
	}
	
	QSqlRecord revisionRecord = sqlQuery.record();
	
	int currentRevision = -1;
	if (revisionRecord.contains("RevisionNumber")){
		currentRevision = revisionRecord.value("RevisionNumber").toInt();
	}
	
	if (currentRevision == revision){
		SendErrorMessage(0, QString("Unable to delete revision '%1' for document '%2'. Error: Revision '%1' is active").arg(revision).arg(qPrintable(objectId)), "Database collection");
		return false;
	}
	
	QByteArray query = QString("DELETE  FROM \"%1\" WHERE \"DocumentId\" = '%2' AND (\"RevisionInfo\"->>'RevisionNumber')::int = %3;")
								.arg(qPrintable(*m_tableNameAttrPtr), qPrintable(objectId))
								.arg(revision)
								.toUtf8();
	
	sqlQuery = m_databaseEngineCompPtr->ExecSqlQuery(query, &sqlError);
	if (sqlError.type() != QSqlError::NoError){
		SendErrorMessage(0, sqlError.text(), "Database collection");
		
		return false;
	}
	
	istd::IChangeable::ChangeSet changeSet(imtbase::IObjectCollection::CF_OBJECT_DATA_CHANGED);
	changeSet.SetChangeInfo(imtbase::IObjectCollection::CN_OBJECT_DATA_CHANGED, objectId);
	
	istd::CChangeNotifier changeNotifier(&collection, &changeSet);
	
	return true;
}


// reimplemented (imtdb::IDependentMetaInfoController)

bool CSqlDatabaseDocumentDelegateComp::UpdateDependentMetaInfo(const DependentMetaInfo& metaInfo) const
{
	if (!m_databaseEngineCompPtr.IsValid()){
		Q_ASSERT(false);
		return false;
	}
	
	if (metaInfo.metaInfoIds.size() != metaInfo.metaInfoValues.size()){
		return false;
	}
	
	QString tableName = GetTableName();
	
	QString query = QString("UPDATE \"%1\" SET \"DataMetaInfo\" = ").arg(tableName);
	
	QString jsonbUpdate = "\"DataMetaInfo\"";
	
	for (int i = 0; i < metaInfo.metaInfoIds.size(); i++) {
		jsonbUpdate = QString("jsonb_set(%1, '{%2}', to_jsonb(%3))")
		.arg(jsonbUpdate, metaInfo.metaInfoIds[i], metaInfo.metaInfoValues[i]);
	}
	
	query += jsonbUpdate + QString(" WHERE \"DataMetaInfo\"->>'%1' = '%2';")
								.arg(metaInfo.dependentKey, metaInfo.objectId);
	
	QSqlError sqlError;
	m_databaseEngineCompPtr->ExecSqlQuery(query.toUtf8(), &sqlError);
	if (sqlError.type() != QSqlError::NoError){
		SendErrorMessage(0, sqlError.text(), "CSqlDatabaseDocumentDelegateComp");
		qDebug() << "SQL-error" << sqlError.text();
		
		return false;
	}
	
	return true;
}


bool CSqlDatabaseDocumentDelegateComp::ClearDependentMetaInfo(const DependentMetaInfo& metaInfo) const
{
	if (!m_databaseEngineCompPtr.IsValid()){
		Q_ASSERT(false);
		return false;
	}
	
	QString tableName = GetTableName();
	
	QString query = QString("UPDATE \"%1\" SET \"DataMetaInfo\" = ").arg(tableName);
	
	QString jsonbUpdate = "\"DataMetaInfo\"";
	
	for (const QString& metaInfoId : metaInfo.metaInfoIds){
		jsonbUpdate = QString("jsonb_set(%1, '{%2}', to_jsonb(''::text))")
		.arg(jsonbUpdate, metaInfoId);
	}
	
	jsonbUpdate = QString("jsonb_set(%1, '{%2}', to_jsonb(''::text))")
					  .arg(jsonbUpdate, metaInfo.dependentKey);
	
	query += jsonbUpdate + QString(" WHERE \"DataMetaInfo\"->>'%1' = '%2';")
								.arg(metaInfo.dependentKey, metaInfo.objectId);
	
	QSqlError sqlError;
	m_databaseEngineCompPtr->ExecSqlQuery(query.toUtf8(), &sqlError);
	if (sqlError.type() != QSqlError::NoError){
		SendErrorMessage(0, sqlError.text(), "CSqlDatabaseDocumentDelegateComp");
		qDebug() << "SQL-error" << sqlError.text();
		
		return false;
	}
	
	return true;
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
	
	Q_ASSERT(revisionArgument.typeId() == QMetaType::Int || revisionArgument.typeId() == QMetaType::QByteArray);
	
	QByteArray documentContentJson;
	if (!WriteDataToMemory(typeId, object, documentContentJson)){
		SendCriticalMessage(0, "Document data could not be written to the memory");
		
		return retVal;
	}
	
	QByteArray documentContentBase64;
	if (*m_useBase64AttrPtr){
		documentContentBase64 = documentContentJson.toBase64();
	}
	
	quint32 checksum = istd::CCrcCalculator::GetCrcFromData((const quint8*)documentContentJson.constData(), documentContentJson.size());
	
	// Insert new entry into the document list table:
	QString query = QString("UPDATE \"%1\" SET \"State\" = 'InActive' WHERE \"%2\" = '%3';")
						.arg(qPrintable(*m_tableNameAttrPtr), qPrintable(s_documentIdColumn), qPrintable(objectId));
	
	QByteArray metaInfoRepresentation = QByteArrayLiteral("{}");
	
	bool ok = false;
	if (m_metaInfoCreatorCompPtr.IsValid()){
		idoc::MetaInfoPtr metaInfoPtr;
		if (m_metaInfoCreatorCompPtr->CreateMetaInfo(&object, typeId, metaInfoPtr) && metaInfoPtr.IsValid()){
			if (m_jsonBasedMetaInfoDelegateCompPtr.IsValid()){
				if (m_jsonBasedMetaInfoDelegateCompPtr->ToJsonRepresentation(*metaInfoPtr.GetPtr(), metaInfoRepresentation)) {
					ok = true;
				}
			}
		}
	}
	
	if (!ok){
		metaInfoRepresentation = documentContentJson;
	}
	
	QString revisionInfo = "jsonb_build_object('OwnerId', '', 'OwnerName', '', 'Checksum', ";
	revisionInfo += QString::number(checksum) + ", 'RevisionNumber', %1)";
	
	if (revisionArgument.typeId() == QMetaType::Int){
		revisionInfo = revisionInfo.arg(revisionArgument.toInt());
	}
	else{
		revisionInfo = revisionInfo.arg(qPrintable(revisionArgument.toByteArray()));
	}
	
	query += QString("INSERT INTO \"%1\"(\"TypeId\", \"DocumentId\", \"Name\", \"Description\", \"Document\", \"DataMetaInfo\", \"RevisionInfo\", \"TimeStamp\", \"State\") VALUES('%2', '%3', '%4', '%5', '%6', '%7', %8, '%9', '%10');")
				 .arg(
					qPrintable(*m_tableNameAttrPtr),
					qPrintable(typeId),
					qPrintable(objectId),
					objectName,
					objectDescription,
					*m_useBase64AttrPtr ? qPrintable(documentContentBase64) : qPrintable(documentContentJson),
					SqlEncode(metaInfoRepresentation),
					revisionInfo,
					QDateTime::currentDateTimeUtc().toString(Qt::ISODateWithMs),
					"Active"
					);
	
	
	retVal = query.toUtf8();
	
	QByteArray operationDescriptionQuery = CreateOperationDescriptionQuery(objectId, operationContextPtr);
	if (!operationDescriptionQuery.isEmpty()){
		retVal += operationDescriptionQuery;
	}
	
	return retVal;
}


QByteArray CSqlDatabaseDocumentDelegateComp::CreateOperationDescriptionQuery(const QByteArray& objectId, const imtbase::IOperationContext* operationContextPtr) const
{
	if (operationContextPtr != nullptr){
		imtbase::IOperationContext* operationPtr = const_cast<imtbase::IOperationContext*>(operationContextPtr);
		if (operationPtr != nullptr){
			iser::ISerializable* changeCollectionPtr = dynamic_cast<iser::ISerializable*>(operationPtr->GetChangesCollection());
			
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
			return QString(R"(UPDATE "%1" SET "RevisionInfo"="RevisionInfo"||'{"OwnerId": "%2", "OwnerName": "%3","OperationDescription": %4}' WHERE ("State" = 'Active' OR "State" = 'Disabled') AND "DocumentId" = '%5';)")
				.arg(qPrintable(*m_tableNameAttrPtr), qPrintable(objectInfo.id), objectInfo.name, SqlEncode(operationDescription), qPrintable(objectId)).toUtf8();
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
	QString schema;
	if (m_tableSchemaAttrPtr.IsValid()) {
		schema = *m_tableSchemaAttrPtr + ".";
	}
	
	Q_ASSERT(!(*m_tableNameAttrPtr).isEmpty());
	
	QString tableName = *m_tableNameAttrPtr;
	QByteArray joinTablesQuery = CreateJoinTablesQuery();
	
	QString customColumns = GetCustomColumnsQuery();
	if (!customColumns.isEmpty()) {
		customColumns = ", " + customColumns;
	}
	
	QString query = QString(R"(
			SELECT
				root.*,
				root1."TimeStamp" as "Added" %4
			FROM %1"%2" as root
			LEFT JOIN %1"%2" AS root1 
				ON root."DocumentId" = root1."DocumentId" 
				AND (root1."RevisionInfo"->>'RevisionNumber')::int = 1
			%3
			WHERE root."State" = 'Active'
		)")
	.arg(schema, tableName, joinTablesQuery.isEmpty() ? "" : QString::fromUtf8(joinTablesQuery), customColumns);
	
	return query;
}


idoc::MetaInfoPtr CSqlDatabaseDocumentDelegateComp::CreateObjectMetaInfo(const QByteArray& typeId) const
{
	if (m_metaInfoCreatorCompPtr.IsValid()){
		QByteArrayList supportedTypeIds = m_metaInfoCreatorCompPtr->GetSupportedTypeIds();
		if (!supportedTypeIds.contains(typeId)){
			Q_ASSERT(false);
			
			return idoc::MetaInfoPtr();
		}
		
		idoc::MetaInfoPtr metaInfoPtr;
		if (m_metaInfoCreatorCompPtr->CreateMetaInfo(nullptr, typeId, metaInfoPtr)){
			Q_ASSERT(metaInfoPtr.IsValid());
			
			return metaInfoPtr;
		}
		
		Q_ASSERT(false);
		
		return idoc::MetaInfoPtr();
	}
	
	return idoc::MetaInfoPtr(new imod::TModelWrap<idoc::CStandardDocumentMetaInfo>);
}


bool CSqlDatabaseDocumentDelegateComp::SetObjectMetaInfoFromRecord(const QSqlRecord& record, idoc::IDocumentMetaInfo& metaInfo) const
{
	if (record.contains("DataMetaInfo")){
		QByteArray metaInfoRepresentation = record.value("DataMetaInfo").toByteArray();
		if (m_jsonBasedMetaInfoDelegateCompPtr.IsValid()){
			return m_jsonBasedMetaInfoDelegateCompPtr->FromJsonRepresentation(metaInfoRepresentation, metaInfo);
		}
	}
	
	return false;
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
		columnId = collectionFilter.GetSortingInfoIds().constFirst();
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
			sortQuery = QString("ORDER BY root.\"%1\" %2").arg(qPrintable(columnId), qPrintable(sortOrder));
		}
		else{
			sortQuery = QString("ORDER BY root.\"DataMetaInfo\"->>\'%1\' %2").arg(qPrintable(columnId), qPrintable(sortOrder));
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
	
	iprm::IParamsSet::Ids paramIds = filterParams.GetParamIds();
	
	QString objectTypeIdQuery;
	QString textFilterQuery;
	QString timeFilterQuery;
	
	if (paramIds.contains("ObjectTypeIdFilter")){
		iprm::TParamsPtr<imtcol::IObjectTypeIdFilter> objectTypeIdFilterPtr(&filterParams, "ObjectTypeIdFilter");
		if (objectTypeIdFilterPtr.IsValid()){
			QByteArray objectTypeId = objectTypeIdFilterPtr->GetObjectTypeId();
			if (!objectTypeId.isEmpty()){
				objectTypeIdQuery = QString("\"TypeId\" = '%1'").arg(qPrintable(objectTypeId)).toUtf8();
			}
		}
	}
	
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
	
	QString additionalFilters = CreateAdditionalFiltersQuery(filterParams);
	
	if (!objectTypeIdQuery.isEmpty()){
		filterQuery += " AND (" + objectTypeIdQuery + ")";
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
			textFilterQuery += QString("%0\"%1\" ILIKE '%%2%'").arg(textFilterQuery.isEmpty() ? "" : " OR ", qPrintable(s_typeIdColumn), textFilter);
			filteringColumnIds.removeOne(s_typeIdColumn);
		}
		
		if (filteringColumnIds.contains(s_nameColumn)){
			textFilterQuery += QString("%0\"%1\" ILIKE '%%2%'").arg(textFilterQuery.isEmpty() ? "" : " OR ", qPrintable(s_nameColumn), textFilter);
			filteringColumnIds.removeOne(s_nameColumn);
		}
		
		if (filteringColumnIds.contains(s_descriptionColumn)){
			textFilterQuery += QString("%0\"%1\" ILIKE '%%2%'").arg(textFilterQuery.isEmpty() ? "" : " OR ", qPrintable(s_descriptionColumn), textFilter);
			filteringColumnIds.removeOne(s_descriptionColumn);
		}
		
		for (int i = 0; i < filteringColumnIds.count(); ++i){
			textFilterQuery += QString("%0\"DataMetaInfo\"->>\'%1\' ILIKE '%%2%'").arg(textFilterQuery.isEmpty() ? "" : " OR ", qPrintable(filteringColumnIds[i]), textFilter);
		}
	}
	
	return true;
}


bool CSqlDatabaseDocumentDelegateComp::CreateTimeFilterQuery(const imtbase::ITimeFilterParam& timeFilter, QString& timeFilterQuery) const
{
	QString addedStrQuery = QString(R"((SELECT "TimeStamp" FROM "%1" as temp WHERE ("RevisionInfo"->>'RevisionNumber')::int = 1 AND root."DocumentId" = temp."DocumentId" LIMIT 1))").arg(qPrintable(*m_tableNameAttrPtr));
	
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
								.arg(addedStrQuery, timeRange.GetBeginTime().toString(Qt::ISODateWithMs), timeRange.GetEndTime().toString(Qt::ISODateWithMs));
		}
	}
	
	return true;
}


// protected methods

bool CSqlDatabaseDocumentDelegateComp::CreateObjectFilterQuery(const imtbase::IComplexCollectionFilter& collectionFilter, QString& filterQuery) const
{
	filterQuery = CComplexCollectionFilterConverter::CreateSqlFilterQuery(collectionFilter);
	
	SubstituteFieldIds(filterQuery);
	
	return true;
}


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

#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
	QRegularExpressionMatchIterator iterator = regexp.globalMatch(query);
	while (iterator.hasNext()){
		const QRegularExpressionMatch& match = iterator.next();
		
		QStringList capturedList = match.capturedTexts();
		list.append(capturedList.first());
	}
#else
	for (const QRegularExpressionMatch& match : regexp.globalMatch(query)){
		QStringList capturedList = match.capturedTexts();
		list.append(capturedList.first());
	}
#endif
	list.removeDuplicates();
	
	for (const QString& item : list){
		QString substitute = item;
		substitute.replace("\"", "");
		
		if (!s_filterableColumns.contains(substitute)){
			substitute = QString("root.\"DataMetaInfo\"->>'%1'").arg(substitute);
			query.replace(item, substitute);
		}
		else if (substitute != s_addedColumn){
			substitute = QString("root.%1").arg(item);
			query.replace(item, substitute);
		}
	}
}


QByteArray CSqlDatabaseDocumentDelegateComp::GetObjectSelectionQuery(const QByteArray& objectId, const iprm::IParamsSet* paramsPtr) const
{
	QString stateDocumentFilter;
	if (paramsPtr != nullptr){
		iprm::IParamsSet::Ids paramIds = paramsPtr->GetParamIds();
		if (paramIds.contains("DocumentFilter")){
			iprm::TParamsPtr<imtcol::IDocumentCollectionFilter> documentFilterParamPtr(paramsPtr, "DocumentFilter");
			if (documentFilterParamPtr.IsValid()){
				imtcol::IDocumentCollectionFilter::DocumentStates states = documentFilterParamPtr->GetDocumentStates();
				
				if (states.contains(imtcol::IDocumentCollectionFilter::DS_ACTIVE)){
					stateDocumentFilter += QString("\"State\" = 'Active' OR \"State\" = 'Disabled'");
				}
				
				if (states.contains(imtcol::IDocumentCollectionFilter::DS_INACTIVE)){
					if (!stateDocumentFilter.isEmpty()){
						stateDocumentFilter += QString(" OR ");
					}
					
					stateDocumentFilter += QString("\"State\" = 'InActive'");
				}
			}
		}
	}
	
	if (stateDocumentFilter.isEmpty()){
		stateDocumentFilter = QString("\"State\" = 'Active' OR \"State\" = 'Disabled'");
	}
	
	QString schemaPrefix;
	if (m_tableSchemaAttrPtr.IsValid()){
		schemaPrefix = QString("%1.").arg(qPrintable(*m_tableSchemaAttrPtr));
	}
	
	return QString("(SELECT * FROM %0\"%1\" WHERE (%2) AND \"%3\" = '%4') ORDER BY (\"RevisionInfo\"->>'RevisionNumber')::int DESC;")
		.arg(schemaPrefix, qPrintable(*m_tableNameAttrPtr), stateDocumentFilter, qPrintable(s_documentIdColumn), qPrintable(objectId)).toUtf8();
}


QByteArray CSqlDatabaseDocumentDelegateComp::CreateJoinTablesQuery() const
{
	return QByteArray();
}


QByteArray CSqlDatabaseDocumentDelegateComp::GetCustomColumnsQuery() const
{
	return QByteArray();
}


} // namespace imtdb


