// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtdb/CSqlDatabaseDocumentDelegateLegacyComp.h>


// Qt includes
#include <QtCore/QFile>
#include <QtCore/QBuffer>

// ACF includes
#include <istd/TOptDelPtr.h>
#include <istd/CSystem.h>
#include <istd/CCrcCalculator.h>
#include <iser/IVersionInfo.h>
#include <iprm/TParamsPtr.h>
#include <iser/CJsonMemWriteArchive.h>

// ImtCore includes
#include <imtbase/imtbase.h>
#include <imtbase/ICollectionFilter.h>
#include <imtdb/imtdb.h>

namespace imtdb
{


static const QByteArray s_documentIdColumn = "DocumentId";
static const QByteArray s_idColumn = "Id";
static const QByteArray s_ownerNameColumn = "OwnerName";
static const QByteArray s_softwareVersionColumn = "SoftwareVersion";


// public methods

// reimplemented (imtdb::ISqlDatabaseObjectDelegate)

QByteArray CSqlDatabaseDocumentDelegateLegacyComp::GetSelectionQuery(
			const QByteArray& objectId,
			int offset,
			int count,
			const iprm::IParamsSet* paramsPtr) const
{
	if (!objectId.isEmpty()){
		QString baseQuery = GetBaseSelectionQuery();

		return QString(
			baseQuery + QStringLiteral(" AND \"%1\".\"Id\" = '%2'").arg(*m_tableNameAttrPtr, objectId)).toUtf8();
	}

	return BaseClass::GetSelectionQuery(objectId, offset, count, paramsPtr);
}


QString CSqlDatabaseDocumentDelegateLegacyComp::GetRevisionUserName(const imtbase::IOperationContext* operationContextPtr) const
{
	if (operationContextPtr != nullptr){
		const imtbase::IOperationContext::IdentifableObjectInfo ownerInfo = operationContextPtr->GetOperationOwnerId();
		if (!ownerInfo.name.isEmpty()){
			return ownerInfo.name;
		}
	}

	if (m_loginCompPtr.IsValid()){
		const iauth::CUser* userPtr = m_loginCompPtr->GetLoggedUser();
		if (userPtr != nullptr){
			return userPtr->GetUserName();
		}
	}

	return QString();
}


QString CSqlDatabaseDocumentDelegateLegacyComp::GetRevisionSoftwareVersion() const
{
	if (!m_versionInfoCompPtr.IsValid()){
		return QString();
	}

	quint32 versionNumber = 0;
	if (m_versionInfoCompPtr->GetVersionNumber(*m_mainSoftwareVersionIdAttrPtr, versionNumber)){
		return m_versionInfoCompPtr->GetEncodedVersionName(*m_mainSoftwareVersionIdAttrPtr, versionNumber);
	}

	return QString();
}


istd::IChangeableUniquePtr CSqlDatabaseDocumentDelegateLegacyComp::CreateObjectFromRecord(
			const QSqlRecord& record,
			const iprm::IParamsSet* /*paramsPtr*/) const
{
	if (!m_databaseEngineCompPtr.IsValid()){
		Q_ASSERT_X(false, "CSqlDatabaseDocumentDelegateLegacyComp::CreateObjectFromRecord", "No database engine was registered");

		return nullptr;
	}

	if (!m_documentFactoriesCompPtr.IsValid()){
		Q_ASSERT_X(false, "CSqlDatabaseDocumentDelegateLegacyComp::CreateObjectFromRecord", "No document factories were registered");

		return nullptr;
	}

	QByteArray typeId = GetObjectTypeId(GetObjectIdFromRecord(record));

	istd::IChangeableUniquePtr documentPtr = CreateObject(typeId);
	if (!documentPtr.IsValid()){
		const QByteArray errorMessage = QStringLiteral("Document instance could not be created for the type: '%1'").arg(typeId).toUtf8();
		Q_ASSERT_X(false, "CSqlDatabaseDocumentDelegateLegacyComp::CreateObjectFromRecord", errorMessage.constData());

		return nullptr;
	}

	if (record.contains(QString(*m_documentContentColumnIdAttrPtr))){
		QByteArray documentContent = record.value(QString(*m_documentContentColumnIdAttrPtr)).toByteArray();

		documentContent = QByteArray::fromBase64(documentContent);

		if (ReadDataFromMemory(typeId, documentContent, *documentPtr)){
			return documentPtr;
		}
	}

	return nullptr;
}


imtdb::IDatabaseObjectDelegate::NewObjectQuery CSqlDatabaseDocumentDelegateLegacyComp::CreateNewObjectQuery(
			const QByteArray& typeId,
			const QByteArray& proposedObjectId,
			const QString& objectName,
			const QString& objectDescription,
			const istd::IChangeable* valuePtr,
			const imtbase::IOperationContext* operationContextPtr) const
{
	NewObjectQuery retVal;

	istd::TOptDelPtr<const istd::IChangeable> workingDocumentPtr;
	istd::IChangeableUniquePtr documentInstancePtr;

	// If the document value is not null, use this for saving into the database. This is the use case 'Insert an existing document into the database':
	if (valuePtr != nullptr){
		workingDocumentPtr.SetPtr(valuePtr, false);
	}
	// Otherwise create a new document instance of the related type:
	else{
		documentInstancePtr = CreateObject(typeId);

		workingDocumentPtr.SetPtr(documentInstancePtr.GetPtr(), false);
	}

	quint32 checksum = 0;
	QByteArray documentContent;

	// Even if we use the external document storage, we should write the data to memory for CRC-checksum calculation:
	if (workingDocumentPtr.IsValid()){
		if (!WriteDataToMemory(typeId, *workingDocumentPtr, documentContent)){
			SendCriticalMessage(0, "Document data could not be written to the memory");

			return retVal;
		}

		checksum = istd::CCrcCalculator::GetCrcFromData((const quint8*)documentContent.constData(), imtbase::narrow_cast<int>(documentContent.size()));
	}
	else{
		SendCriticalMessage(0, "Document instance is invalid. SQL-query could not be created");
	}

	// Create the document-ID:
	QByteArray objectId = proposedObjectId.isEmpty() ? QUuid::createUuid().toByteArray(QUuid::WithoutBraces) : proposedObjectId;

	// Create ID for the document data revision:
	QByteArray revisionUuid = QUuid::createUuid().toByteArray(QUuid::WithoutBraces);

	// Insert new entry into the document list table:
	retVal.query = QStringLiteral("INSERT INTO \"%1\"(\"Id\", \"%2\", \"Name\", \"Description\", \"Added\", \"LastRevisionId\") VALUES('%3', '%4', '%5', '%6', '%7', '%8');")
				.arg(*m_tableNameAttrPtr)
				.arg(*m_objectTypeIdColumnAttrPtr)
				.arg(objectId)
				.arg(typeId)
				.arg(SqlEncode(objectName))
				.arg(SqlEncode(objectDescription))
				.arg(QDateTime::currentDateTime().toString(Qt::ISODate))
				.arg(revisionUuid)
				.toUtf8();

	// Insert new entry into the document data revision table:
	QString revisionColumns = QStringLiteral("\"Id\", \"%1\", \"%2\", \"RevisionNumber\", \"Comment\", \"LastModified\", \"Checksum\"")
				.arg(s_documentIdColumn)
				.arg(*m_documentContentColumnIdAttrPtr);
	QString revisionValues = QStringLiteral("'%1', '%2', '%3', '%4', '%5', '%6', %7")
				.arg(revisionUuid)
				.arg(objectId)
				.arg(documentContent.toBase64())
				.arg(1)
				.arg(SqlEncode(QObject::tr("Initial revision")))
				.arg(QDateTime::currentDateTime().toString(Qt::ISODate))
				.arg(checksum);
	revisionColumns += QStringLiteral(", \"%1\"").arg(s_ownerNameColumn);
	revisionValues += QStringLiteral(", '%1'").arg(SqlEncode(GetRevisionUserName(operationContextPtr)));
	revisionColumns += QStringLiteral(", \"%1\"").arg(s_softwareVersionColumn);
	revisionValues += QStringLiteral(", '%1'").arg(SqlEncode(GetRevisionSoftwareVersion()));
	retVal.query += QStringLiteral("INSERT INTO \"%1\"(%2) VALUES(%3);")
				.arg(*m_revisionsTableNameAttrPtr)
				.arg(revisionColumns)
				.arg(revisionValues)
				.toUtf8();

	if (m_metaInfoTableDelegateCompPtr.IsValid()){
		idoc::MetaInfoPtr metaInfoPtr = m_metaInfoTableDelegateCompPtr->CreateMetaInfo(valuePtr, typeId);
		if (metaInfoPtr.IsValid()){
			retVal.query += "\n";

			QByteArrayList columnIds = { "\"" + s_idColumn + "\"", "\"RevisionId\"" };
			columnIds += m_metaInfoTableDelegateCompPtr->GetColumnIds();

			QStringList tableValues;
			tableValues.push_back("'" + QUuid::createUuid().toString(QUuid::WithoutBraces) + "'");
			tableValues.push_back("'" + QString(revisionUuid) + "'");

			for (const QByteArray& columnId : m_metaInfoTableDelegateCompPtr->GetColumnIds()){
				QVariant data = metaInfoPtr->GetMetaInfo(m_metaInfoTableDelegateCompPtr->GetMetaInfoType(columnId));

				QString value = m_metaInfoTableDelegateCompPtr->ToTableRepresentation(data, columnId).toString();

				tableValues.push_back("'" + SqlEncode(value) + "'");
			}

			// Insert new entry into the document' meta info table:
			retVal.query += QStringLiteral("INSERT INTO \"%1\"(%2) VALUES(%3);")
						.arg(*m_metaInfoTableNameAttrPtr)
						.arg(columnIds.join(", "))
						.arg(tableValues.join(", "))
						.toUtf8();
		}
		else {
			SendErrorMessage(0, "Meta information of the document could not be created", "SQL Database Delegate");

			return imtdb::IDatabaseObjectDelegate::NewObjectQuery();
		}
	}

	retVal.objectName = objectName;

	return retVal;
}


QByteArray CSqlDatabaseDocumentDelegateLegacyComp::CreateDeleteObjectsQuery(
			const imtbase::IObjectCollection& /*collection*/,
			const imtbase::ICollectionInfo::Ids& objectIds,
			const imtbase::IOperationContext* /*operationContextPtr*/) const
{
	if (objectIds.isEmpty()){
		return QByteArray();
	}

	QStringList quotedIds;
	for (const QByteArray& objectId : objectIds){
		quotedIds << QStringLiteral("'%1'").arg(objectId);
	}

	QString query = QString(
						"DELETE FROM \"%1\" WHERE \"%2\" IN (%3);")
						.arg(
							QString::fromUtf8(*m_tableNameAttrPtr),
							QString::fromUtf8(s_idColumn),
							quotedIds.join(", ")
							);

	return query.toUtf8();
}


QByteArray CSqlDatabaseDocumentDelegateLegacyComp::CreateDeleteObjectSetQuery(
			const imtbase::IObjectCollection& /*collection*/,
			const iprm::IParamsSet* /*paramsPtr*/,
			const imtbase::IOperationContext* /*operationContextPtr*/) const
{
	return QByteArray();
}


QByteArray CSqlDatabaseDocumentDelegateLegacyComp::CreateUpdateObjectQuery(
			const imtbase::IObjectCollection& collection,
			const QByteArray& objectId,
			const istd::IChangeable& object,
			const imtbase::IOperationContext* operationContextPtr,
			bool /*useExternDelegate*/) const
{
	// Get number of the revisions of the document in the database:
	QByteArray countRevisionsQuery = QStringLiteral("SELECT COUNT(*) FROM \"%1\" WHERE \"%2\" = '%3';")
				.arg(*m_revisionsTableNameAttrPtr)
				.arg(s_documentIdColumn)
				.arg(objectId)
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

	QByteArray documentContent;
	if (WriteDataToMemory(typeId, object, documentContent)){
		quint32 checksum = istd::CCrcCalculator::GetCrcFromData((const quint8*)documentContent.constData(), imtbase::narrow_cast<int>(documentContent.size()));
		QByteArray revisionUuid = QUuid::createUuid().toByteArray(QUuid::WithoutBraces);

		retVal = QStringLiteral("UPDATE \"%1\" SET \"LastRevisionId\" = '%2' WHERE \"%3\" = '%4';")
					.arg(*m_tableNameAttrPtr)
					.arg(revisionUuid)
					.arg(s_idColumn)
					.arg(objectId)
					.toUtf8();

		QString operationComment = operationContextPtr != nullptr ? operationContextPtr->GetOperationDescription() : QString();
		QString revisionColumns = QStringLiteral("\"Id\", \"%1\", \"%2\", \"RevisionNumber\", \"Comment\", \"LastModified\", \"Checksum\"")
					.arg(s_documentIdColumn)
					.arg(*m_documentContentColumnIdAttrPtr);
		QString revisionValues = QStringLiteral("'%1', '%2', '%3', '%4', '%5', '%6', %7")
					.arg(revisionUuid)
					.arg(objectId)
					.arg(documentContent.toBase64())
					.arg(revisionsCount + 1)
					.arg(SqlEncode(operationComment))
					.arg(QDateTime::currentDateTime().toString(Qt::ISODate))
					.arg(checksum);
		revisionColumns += QStringLiteral(", \"%1\"").arg(s_ownerNameColumn);
		revisionValues += QStringLiteral(", '%1'").arg(SqlEncode(GetRevisionUserName(operationContextPtr)));
		revisionColumns += QStringLiteral(", \"%1\"").arg(s_softwareVersionColumn);
		revisionValues += QStringLiteral(", '%1'").arg(SqlEncode(GetRevisionSoftwareVersion()));
		retVal += QStringLiteral("INSERT INTO \"%1\"(%2) VALUES(%3);")
					.arg(*m_revisionsTableNameAttrPtr)
					.arg(revisionColumns)
					.arg(revisionValues)
					.toUtf8();

		if (m_metaInfoTableDelegateCompPtr.IsValid()){
			idoc::MetaInfoPtr metaInfoPtr = m_metaInfoTableDelegateCompPtr->CreateMetaInfo(&object, collection.GetObjectTypeId(objectId));
			if (metaInfoPtr.IsValid()){
				retVal += "\n";

				QByteArrayList columnIds = { "\"" + s_idColumn + "\"", "\"RevisionId\"" };
				columnIds += m_metaInfoTableDelegateCompPtr->GetColumnIds();

				QStringList tableValues;
				tableValues.push_back("'" + QUuid::createUuid().toString(QUuid::WithoutBraces) + "'");
				tableValues.push_back("'" + QString(revisionUuid) + "'");

				for (const QByteArray& columnId : m_metaInfoTableDelegateCompPtr->GetColumnIds()){
					QVariant data = metaInfoPtr->GetMetaInfo(m_metaInfoTableDelegateCompPtr->GetMetaInfoType(columnId));

					QString value = m_metaInfoTableDelegateCompPtr->ToTableRepresentation(data, columnId).toString();

					tableValues.push_back("'" + SqlEncode(value) + "'");
				}

				retVal += QStringLiteral("INSERT INTO \"%1\"(%2) VALUES(%3);")
							.arg(*m_metaInfoTableNameAttrPtr)
							.arg(columnIds.join(", "))
							.arg(tableValues.join(", "))
							.toUtf8();
			}
		}
	}

	return retVal;
}


QByteArray CSqlDatabaseDocumentDelegateLegacyComp::CreateRenameObjectQuery(
			const imtbase::IObjectCollection& /*collection*/,
			const QByteArray& objectId,
			const QString& newObjectName,
			const imtbase::IOperationContext* /*operationContextPtr*/) const
{
	QByteArray retVal = QStringLiteral("UPDATE \"%1\" SET \"Name\" = '%2' WHERE \"%3\" = '%4';")
				.arg(*m_tableNameAttrPtr)
				.arg(SqlEncode(newObjectName))
				.arg(s_idColumn)
				.arg(objectId)
				.toUtf8();

	return retVal;
}


QByteArray CSqlDatabaseDocumentDelegateLegacyComp::CreateDescriptionObjectQuery(
			const imtbase::IObjectCollection& /*collection*/,
			const QByteArray& /*objectId*/,
			const QString& /*description*/,
			const imtbase::IOperationContext* /*operationContextPtr*/) const
{
	QByteArray retVal;

	return retVal;
}


// reimplemented (imtbase::IRevisionController)

imtbase::IRevisionController::RevisionInfoList CSqlDatabaseDocumentDelegateLegacyComp::GetRevisionInfoList(
			const imtbase::IObjectCollection& /*collection*/,
			const QByteArray& objectId) const
{
	if (!m_databaseEngineCompPtr.IsValid()){
		return imtbase::IRevisionController::RevisionInfoList();
	}

	if ((QString(*m_revisionsTableNameAttrPtr)).isEmpty()){
		return imtbase::IRevisionController::RevisionInfoList();
	}

	QString revisionListQuery = QStringLiteral("SELECT \"RevisionNumber\", \"LastModified\", \"Comment\", \"%1\", \"%2\" from \"%3\" WHERE \"%4\" = '%5' ORDER BY \"RevisionNumber\" DESC")
			.arg(s_ownerNameColumn)
				.arg(s_softwareVersionColumn)
				.arg(*m_revisionsTableNameAttrPtr)
				.arg(s_documentIdColumn)
				.arg(objectId);

	QSqlError sqlError;
	QSqlQuery sqlQuery = m_databaseEngineCompPtr->ExecSqlQuery(revisionListQuery.toUtf8(), &sqlError);
	if (sqlError.type() != QSqlError::NoError){
		SendErrorMessage(0, sqlError.text(), "Database collection");

		return imtbase::IRevisionController::RevisionInfoList();
	}

	imtbase::IRevisionController::RevisionInfoList revisionInfoList;

	while (sqlQuery.next()){
		QSqlRecord revisionRecord = sqlQuery.record();
		RevisionInfo revisionInfo;

		if (revisionRecord.contains("RevisionNumber")){
			revisionInfo.revision = revisionRecord.value("RevisionNumber").toLongLong();
		}

		if (revisionRecord.contains("LastModified")){
			revisionInfo.timestamp = revisionRecord.value("LastModified").toDateTime();
		}

		if (revisionRecord.contains("Comment")){
			revisionInfo.comment = revisionRecord.value("Comment").toString();
		}

		revisionInfo.user = revisionRecord.value(QString::fromUtf8(s_ownerNameColumn)).toString();
		revisionInfo.softwareVersion = revisionRecord.value(QString::fromUtf8(s_softwareVersionColumn)).toString();

		revisionInfo.isRevisionAvailable = true;

		revisionInfoList.push_back(revisionInfo);
	}

	return revisionInfoList;
}


int CSqlDatabaseDocumentDelegateLegacyComp::BackupRevision(
			const imtbase::IObjectCollection& /*collection*/,
			const imtbase::ICollectionInfo::Id& objectId,
			const QString& userComment) const
{
	if (!m_databaseEngineCompPtr.IsValid()){
		return -1;
	}

	if ((QString(*m_revisionsTableNameAttrPtr)).isEmpty()){
		return -1;
	}

	if (objectId.isEmpty()){
		return -1;
	}

	QByteArray lastRevisionQuery = QStringLiteral("SELECT \"%1\" from \"%2\" WHERE \"%1\" in (SELECT \"LastRevisionId\" from \"%3\" WHERE \"%1\" = '%4')")
				.arg(s_idColumn)
				.arg(*m_revisionsTableNameAttrPtr)
				.arg(*m_tableNameAttrPtr)
				.arg(objectId)
				.toUtf8();

	QByteArray lastRevisionNumberQuery = QStringLiteral("SELECT \"RevisionNumber\" from \"%2\" WHERE \"%1\" in (SELECT \"LastRevisionId\" from \"%3\" WHERE \"%1\" = '%4')")
				.arg(s_idColumn)
				.arg(*m_revisionsTableNameAttrPtr)
				.arg(*m_tableNameAttrPtr)
				.arg(objectId)
				.toUtf8();

	QString escapedComment = SqlEncode(userComment);
	if (escapedComment.length() > *m_maxLengthRevisionCommentAttrPtr){
		escapedComment = escapedComment.left(*m_maxLengthRevisionCommentAttrPtr);
		// Ensure we don't split an escaped quote pair ('')
		while (escapedComment.endsWith(QLatin1Char('\'')) && escapedComment.count(QLatin1Char('\'')) % 2 != 0){
			escapedComment.chop(1);
		}
	}
	QByteArray updateCommentQuery = QStringLiteral("UPDATE \"%1\" SET \"Comment\" = '%2' WHERE \"%3\" in (%4)")
				.arg(*m_revisionsTableNameAttrPtr)
				.arg(escapedComment)
				.arg(s_idColumn)
				.arg(lastRevisionQuery)
				.toUtf8();

	m_databaseEngineCompPtr->ExecSqlQuery(updateCommentQuery);

	QSqlQuery queryResult = m_databaseEngineCompPtr->ExecSqlQuery(lastRevisionNumberQuery);
	if (queryResult.next()){
		QSqlRecord record = queryResult.record();
		if (record.contains("RevisionNumber")){
			return record.value("RevisionNumber").toInt();
		}
	}

	return -1;
}


bool CSqlDatabaseDocumentDelegateLegacyComp::RestoreRevision(
			imtbase::IObjectCollection& collection,
			const imtbase::ICollectionInfo::Id& objectId,
			int revision) const
{
	if (!m_databaseEngineCompPtr.IsValid()){
		return false;
	}

	if (!m_documentFactoriesCompPtr.IsValid()){
		return false;
	}

	QString revisionIdQuery  = QStringLiteral("SELECT \"%5\" from \"%1\" WHERE \"%2\" = '%3' AND \"RevisionNumber\" = %4")
				.arg(*m_revisionsTableNameAttrPtr)
				.arg(s_documentIdColumn)
				.arg(objectId)
				.arg(revision)
				.arg(s_idColumn);

	QByteArray revisionUuid;
	QSqlQuery queryResult = m_databaseEngineCompPtr->ExecSqlQuery(revisionIdQuery.toUtf8());
	if (queryResult.next()){
		QSqlRecord record = queryResult.record();
		if (record.contains(s_idColumn)){
			revisionUuid = imtdb::VariantToByteArray(record.value(s_idColumn));
		}
	}

	if (revisionUuid.isEmpty()){
		return false;
	}

	QString setActiveRevisionQuery = QStringLiteral("UPDATE \"%1\" SET \"LastRevisionId\" = '%2' WHERE \"%3\" = '%4';")
				.arg(*m_tableNameAttrPtr)
				.arg(revisionUuid)
				.arg(s_idColumn)
				.arg(objectId)
				.toUtf8();

	istd::CChangeNotifier changeNotifier(&collection);

	QSqlError sqlError;
	m_databaseEngineCompPtr->ExecSqlQuery(setActiveRevisionQuery.toUtf8(), &sqlError);

	return sqlError.type() == QSqlError::NoError;
}


bool CSqlDatabaseDocumentDelegateLegacyComp::ExportRevision(
			const imtbase::IObjectCollection& /*collection*/,
			const imtbase::ICollectionInfo::Id& /*objectId*/,
			int /*revision*/,
			const QString& /*filePath*/) const
{
	return false;
}


bool CSqlDatabaseDocumentDelegateLegacyComp::DeleteRevision(
			imtbase::IObjectCollection& /*collection*/,
			const imtbase::ICollectionInfo::Id& /*objectId*/,
			int /*revision*/) const
{
	return false;
}


// protected methods

istd::IChangeableUniquePtr CSqlDatabaseDocumentDelegateLegacyComp::CreateObject(const QByteArray& typeId) const
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


bool CSqlDatabaseDocumentDelegateLegacyComp::WriteDataToMemory(const QByteArray& typeId, const istd::IChangeable& object, QByteArray& data) const
{
	const ifile::IDeviceBasedPersistence* documentPersistencePtr = FindDocumentPersistence(typeId);
	if (documentPersistencePtr == nullptr){
		SendErrorMessage(0, QStringLiteral("Document data could not be written due no persistence was found for the type: %1").arg(typeId));

		return false;
	}

	QBuffer buffer(&data);
	if (!buffer.open(QIODevice::WriteOnly)){
		SendErrorMessage(0, "Failed to open QBuffer for writing");
		return false;
	}

	int operationState = documentPersistencePtr->WriteToDevice(object, buffer, nullptr);
	buffer.close();

	if (operationState != ifile::IDeviceBasedPersistence::Successful){
		return false;
	}

	return true;
}


bool CSqlDatabaseDocumentDelegateLegacyComp::ReadDataFromMemory(const QByteArray& typeId, const QByteArray& data, istd::IChangeable& object) const
{
	const ifile::IDeviceBasedPersistence* documentPersistencePtr = FindDocumentPersistence(typeId);
	if (documentPersistencePtr == nullptr){
		SendErrorMessage(0, QStringLiteral("Document data could not be read due no persistence was found for the type: %1").arg(typeId));

		return false;
	}

	// Use copy to avoid const_cast. QByteArray uses copy-on-write, so this shares data until modified.
	// QBuffer in ReadOnly mode doesn't modify the data, only tracks position internally.
	QByteArray dataCopy = data;
	QBuffer buffer(&dataCopy);
	if (!buffer.open(QIODevice::ReadOnly)){
		SendErrorMessage(0, "Failed to open QBuffer for reading");
		return false;
	}

	int operationState = documentPersistencePtr->ReadFromDevice(object, buffer, nullptr);
	buffer.close();

	if (operationState != ifile::IDeviceBasedPersistence::Successful){
		return false;
	}

	return true;
}


// reimplemented (imtdb::CSqlDatabaseObjectDelegateCompBase)

QString CSqlDatabaseDocumentDelegateLegacyComp::GetBaseSelectionQuery() const
{
	QString metaInfoValuesQuery;
	QString joinMetaInfoQuery;
	if (m_metaInfoTableDelegateCompPtr.IsValid()){
		QByteArrayList columnIds = m_metaInfoTableDelegateCompPtr->GetColumnIds();
		for (int i = 0; i < columnIds.count(); ++i){
			metaInfoValuesQuery += QStringLiteral(", \"%1\".%2").arg(*m_metaInfoTableNameAttrPtr, columnIds[i]);
		}

		joinMetaInfoQuery = QStringLiteral("JOIN \"%1\" ON \"%2\".\"LastRevisionId\" = \"%1\".\"RevisionId\"")
				.arg(*m_metaInfoTableNameAttrPtr)
				.arg(*m_tableNameAttrPtr);
	}

	return QStringLiteral("SELECT \"%1\".*, \"%2\".\"Document\", \"%2\".\"LastModified\", \"%2\".\"Checksum\", \"%2\".\"RevisionNumber\", \"%2\".\"Comment\" %3 FROM \"%1\" JOIN \"%2\" ON \"%1\".\"LastRevisionId\" = \"%2\".\"Id\" %4")
		.arg(*m_tableNameAttrPtr)
		.arg(*m_revisionsTableNameAttrPtr)
		.arg(metaInfoValuesQuery)
		.arg(joinMetaInfoQuery);
}



bool CSqlDatabaseDocumentDelegateLegacyComp::CreateObjectFilterQuery(const iprm::IParamsSet& filterParams, QString& filterQuery) const
{
	filterQuery.clear();

	iprm::TParamsPtr<imtbase::ICollectionFilter> collectionFilterParamPtr(&filterParams, "Filter");
	if (collectionFilterParamPtr.IsValid()){
		QByteArray typeId = collectionFilterParamPtr->GetObjectTypeId();

		filterQuery = QStringLiteral("\"TypeId\" = '%1'").arg(typeId).toUtf8();
	}

	return true;
}


bool CSqlDatabaseDocumentDelegateLegacyComp::CreateObjectInfoFromRecord(
			const QByteArray& typeId,
			const QSqlRecord& record,
			idoc::MetaInfoPtr& objectMetaInfoPtr,
			idoc::MetaInfoPtr& collectionItemMetaInfoPtr) const
{
	if (!m_databaseEngineCompPtr.IsValid()){
		return false;
	}

	QByteArray objectId = imtdb::VariantToByteArray(record.value(s_idColumn));

	QByteArray sqlMetaInfoQuery = QStringLiteral("SELECT * FROM \"%1\" WHERE \"RevisionId\" = (SELECT \"LastRevisionId\" FROM \"%3\" WHERE \"Id\" = '%2')")
				.arg(*m_metaInfoTableNameAttrPtr)
				.arg(objectId)
				.arg(*m_tableNameAttrPtr)
				.toUtf8();

	QSqlQuery metaInfoQuery = m_databaseEngineCompPtr->ExecSqlQuery(sqlMetaInfoQuery);
	if (metaInfoQuery.next()){
		QSqlRecord metaInfoRecord = metaInfoQuery.record();

		objectMetaInfoPtr = CreateObjectMetaInfo(typeId);
		if (objectMetaInfoPtr.IsValid()){
			if (!SetObjectMetaInfoFromRecord(metaInfoRecord, *objectMetaInfoPtr)){
				objectMetaInfoPtr.Reset();

				return false;
			}
		}
	}

	collectionItemMetaInfoPtr.SetPtr(CreateCollectionItemMetaInfo(typeId));
	if (collectionItemMetaInfoPtr.IsValid()){
		if (!SetCollectionItemMetaInfoFromRecord(record, *collectionItemMetaInfoPtr)){
			collectionItemMetaInfoPtr.Reset();

			return false;
		}
	}

	return true;
}



idoc::MetaInfoPtr CSqlDatabaseDocumentDelegateLegacyComp::CreateObjectMetaInfo(const QByteArray& typeId) const
{
	if (m_metaInfoTableDelegateCompPtr.IsValid()){
		return m_metaInfoTableDelegateCompPtr->CreateMetaInfo(nullptr, typeId);
	}

	return idoc::MetaInfoPtr();
}


bool CSqlDatabaseDocumentDelegateLegacyComp::SetObjectMetaInfoFromRecord(const QSqlRecord& record, idoc::IDocumentMetaInfo& metaInfo) const
{
	if (m_metaInfoTableDelegateCompPtr.IsValid()){
		QByteArrayList columnIds = m_metaInfoTableDelegateCompPtr->GetColumnIds();
		for (const QByteArray& columnId : columnIds){
			int metaInfoType = m_metaInfoTableDelegateCompPtr->GetMetaInfoType(columnId);
			if (metaInfoType >= 0){
				QByteArray metaInfoId = QString(columnId).remove("\"").toUtf8();

				if (record.contains(metaInfoId)){
					QVariant data = m_metaInfoTableDelegateCompPtr->FromTableRepresentation(record.value(metaInfoId), metaInfoId);
					if (data.isValid()){
						metaInfo.SetMetaInfo(metaInfoType, data);
					}
				}
			}
		}

		return true;
	}

	return false;
}


// protected methods


QByteArray CSqlDatabaseDocumentDelegateLegacyComp::CreateOperationDescriptionQuery(const QByteArray& objectId, const imtbase::IOperationContext* operationContextPtr) const
{
	if (operationContextPtr != nullptr){
		imtbase::IOperationContext* operationPtr = const_cast<imtbase::IOperationContext*>(operationContextPtr);
		if (operationPtr != nullptr){
			iser::ISerializable* changeCollectionPtr = dynamic_cast<iser::ISerializable*>(operationPtr->GetChangesCollection());

			QByteArray json;
			{
				iser::CJsonMemWriteArchive archive(m_versionInfoCompPtr.GetPtr());
				if (changeCollectionPtr->Serialize(archive)){
					json = archive.GetData();
				}
				else{
					qDebug() << QStringLiteral("Unable to serialize a change object collection");
				}
			}

			QString operationDescription = json;

			imtbase::IOperationContext::IdentifableObjectInfo objectInfo = operationPtr->GetOperationOwnerId();
			return QString(R"(UPDATE "%1" SET "OwnerId" = '%2', "OwnerName" = '%3', "OperationDescription" = '%4' WHERE "IsActive" = true AND "DocumentId" = '%5')")
				.arg(*m_tableNameAttrPtr)
				.arg(objectInfo.id)
				.arg(SqlEncode(objectInfo.name))
				.arg(SqlEncode(operationDescription))
				.arg(objectId)
				.toUtf8();
		}
	}

	return QByteArray();
}


const ifile::IDeviceBasedPersistence* CSqlDatabaseDocumentDelegateLegacyComp::FindDocumentPersistence(const QByteArray& typeId) const
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
