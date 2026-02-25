// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
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
#include <iser/CJsonMemReadArchive.h>
#include <iprm/CParamsSet.h>
#include <iprm/CIdParam.h>
#include <iprm/TParamsPtr.h>
#include <iprm/IIdParam.h>
#include <idoc/CStandardDocumentMetaInfo.h>

// ImtCore includes
#include <imtbase/imtbase.h>
#include <imtbase/ICollectionFilter.h>
#include <imtdb/CComplexCollectionFilterConverter.h>
#include <imtcol/IObjectTypeIdFilter.h>
#include <imtbase/CComplexCollectionFilter.h>


namespace imtdb
{


const QByteArray CSqlDatabaseDocumentDelegateComp::s_idColumn = QByteArrayLiteral("Id");
const QByteArray CSqlDatabaseDocumentDelegateComp::s_typeIdColumn = QByteArrayLiteral("TypeId");
const QByteArray CSqlDatabaseDocumentDelegateComp::s_documentIdColumn = QByteArrayLiteral("DocumentId");
const QByteArray CSqlDatabaseDocumentDelegateComp::s_nameColumn = QByteArrayLiteral("Name");
const QByteArray CSqlDatabaseDocumentDelegateComp::s_descriptionColumn = QByteArrayLiteral("Description");
const QByteArray CSqlDatabaseDocumentDelegateComp::s_documentColumn = QByteArrayLiteral("Document");
const QByteArray CSqlDatabaseDocumentDelegateComp::s_dataMetaInfoColumn = QByteArrayLiteral("DataMetaInfo");
const QByteArray CSqlDatabaseDocumentDelegateComp::s_addedColumn = QByteArrayLiteral("Added");
const QByteArray CSqlDatabaseDocumentDelegateComp::s_lastModifiedColumn = QByteArrayLiteral("TimeStamp");
const QByteArray CSqlDatabaseDocumentDelegateComp::s_stateColumn = QByteArrayLiteral("State");
const QByteArray CSqlDatabaseDocumentDelegateComp::s_revisionInfoColumn = QByteArrayLiteral("RevisionInfo");

const QByteArray CSqlDatabaseDocumentDelegateComp::s_ownerIdKey = QByteArrayLiteral("OwnerId");
const QByteArray CSqlDatabaseDocumentDelegateComp::s_ownerNameKey = QByteArrayLiteral("OwnerName");
const QByteArray CSqlDatabaseDocumentDelegateComp::s_operationDescriptionKey = QByteArrayLiteral("OperationDescription");
const QByteArray CSqlDatabaseDocumentDelegateComp::s_revisionNumberKey = QByteArrayLiteral("RevisionNumber");
const QByteArray CSqlDatabaseDocumentDelegateComp::s_checksumKey = QByteArrayLiteral("Checksum");

QSet<QString> CSqlDatabaseDocumentDelegateComp::s_filterableColumns ={
	s_typeIdColumn,
	s_documentIdColumn,
	s_nameColumn,
	s_descriptionColumn,
	s_stateColumn,
	s_addedColumn,
	s_lastModifiedColumn
};


// public methods

// reimplemented (imtdb::ISqlDatabaseObjectDelegate)

QByteArray CSqlDatabaseDocumentDelegateComp::GetCountQuery(const iprm::IParamsSet* paramsPtr) const
{
	QString baseQuery = GetSelectionQuery(QByteArray(), 0, -1, paramsPtr);

	return QString("SELECT COUNT(*) FROM (%1) as t").arg(baseQuery).toUtf8();
}


QByteArray CSqlDatabaseDocumentDelegateComp::GetObjectIdFromRecord(const QSqlRecord& record) const
{
	const QString columnId = QString::fromUtf8(*m_objectIdColumnAttrPtr);
	if (!record.contains(columnId)){
		return QByteArray();
	}

	const QUuid objectId = record.value(columnId).toUuid();
	if(objectId.isNull()){
		return QByteArray();
	}

	return objectId.toByteArray(QUuid::WithoutBraces);
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

	if (paramsPtr != nullptr){
		const iprm::IParamsSet::Ids paramIds = paramsPtr->GetParamIds();
		if (paramIds.contains("ComplexFilter")){
			iprm::TParamsPtr<imtbase::CComplexCollectionFilter> complexFilterParamPtr(paramsPtr, "ComplexFilter");
			if (complexFilterParamPtr.IsValid()){
				QByteArrayList fieldIds = complexFilterParamPtr->GetDistinctFieldsList();
	
				QString distinctString;
				Q_ASSERT(fieldIds.count() < 2);
				for (int i = 0; i < fieldIds.count(); i++){
					QString fieldId = fieldIds[i];

					const QByteArray& dataColumn = m_useDataMetaInfoAttrPtr.IsValid() && *m_useDataMetaInfoAttrPtr
														? s_dataMetaInfoColumn : s_documentColumn;
					fieldId = s_filterableColumns.contains(fieldId) ? QStringLiteral(R"("%1")").arg(fieldId) : CreateJsonExtractSql(dataColumn, fieldId);

					distinctString += i > 0 ? QStringLiteral(", %1").arg(fieldId) : fieldId;
				}

				if (!distinctString.isEmpty()){
					selectionQuery = QStringLiteral("SELECT DISTINCT ON (%1) * FROM (%2) as collectiondata").arg(distinctString, selectionQuery).toUtf8();
				}
			}
		}
	}

	return selectionQuery;
}


istd::IChangeableUniquePtr CSqlDatabaseDocumentDelegateComp::CreateObjectFromRecord(
			const QSqlRecord& record,
			const iprm::IParamsSet* /*paramsPtr*/) const
{
	if (!m_databaseEngineCompPtr.IsValid()){
		Q_ASSERT_X(false, "CSqlDatabaseDocumentDelegateComp::CreateObjectFromRecord", "No database engine was registered");

		return nullptr;
	}

	if (!m_documentFactoriesCompPtr.IsValid()){
		Q_ASSERT_X(false, "CSqlDatabaseDocumentDelegateComp::CreateObjectFromRecord", "No document factories were registered");

		return nullptr;
	}

	QByteArray typeId = GetObjectTypeIdFromRecord(record);

	istd::IChangeableUniquePtr documentPtr = CreateObject(typeId);
	if (!documentPtr.IsValid()){
		Q_ASSERT_X(false, "CSqlDatabaseDocumentDelegateComp::CreateObjectFromRecord", qPrintable(QString("Document instance could not be created for the type: '%1'").arg(qPrintable(typeId))));

		return nullptr;
	}

	if (record.contains(s_documentColumn)){
		QByteArray documentContent = record.value(qPrintable(s_documentColumn)).toByteArray();
		if (ReadDataFromMemory(typeId, documentContent, *documentPtr)){
			return documentPtr;
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

	istd::IChangeableUniquePtr documentInstancePtr;

	istd::TOptDelPtr<const istd::IChangeable> workingDocumentPtr;

	// If the document value is not null, use this for saving into the database. This is the use case 'Insert an existing document into the database':
	if (valuePtr != nullptr){
		workingDocumentPtr.SetPtr(valuePtr, false);
	}

	// Otherwise create a new document instance of the related type:
	else{
		documentInstancePtr = CreateObject(typeId);

		workingDocumentPtr.SetPtr(documentInstancePtr.GetPtr(), false);
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


QByteArray CSqlDatabaseDocumentDelegateComp::CreateDeleteObjectsQuery(
			const imtbase::IObjectCollection& /*collection*/,
			const imtbase::ICollectionInfo::Ids& objectIds,
			const imtbase::IOperationContext* /*operationContextPtr*/) const
{
	if (objectIds.isEmpty()){
		return QByteArray();
	}

	QString schemaPrefix;
	if (m_tableSchemaAttrPtr.IsValid()){
		schemaPrefix = QString("%1.").arg(qPrintable(*m_tableSchemaAttrPtr));
	}

	const QString quotedIds = objectIds.toList().join("','").append('\'').prepend('\'');
	QString query = 
		QStringLiteral(R"sql(UPDATE %0 "%1" SET "%2" = 'Disabled' WHERE "%2" = 'Active' AND "%3" IN (%4);)sql")
					.arg(
								schemaPrefix,
								QString::fromUtf8(*m_tableNameAttrPtr),
								QString::fromUtf8(s_stateColumn),
								QString::fromUtf8(s_documentIdColumn),
								quotedIds);

	return query.toUtf8();
}


QByteArray CSqlDatabaseDocumentDelegateComp::CreateDeleteObjectSetQuery(
			const imtbase::IObjectCollection& /*collection*/,
			const iprm::IParamsSet* paramsPtr,
			const imtbase::IOperationContext* /*operationContextPtr*/) const
{
	QString schemaPrefix;
	if (m_tableSchemaAttrPtr.IsValid()){
		schemaPrefix = QString("%1.").arg(qPrintable(*m_tableSchemaAttrPtr));
	}
	QByteArray retVal = QString("UPDATE %0 \"%1\" as root SET \"%2\" = 'Disabled' WHERE \"%2\" = 'Active'").arg(schemaPrefix, QString::fromUtf8(*m_tableNameAttrPtr), QString::fromUtf8(s_stateColumn)).toUtf8();

	if (paramsPtr != nullptr){
		iprm::TParamsPtr<imtbase::IComplexCollectionFilter> complexFilterParamPtr(paramsPtr, "ComplexFilter");
		if (complexFilterParamPtr.IsValid()){
			QByteArrayList conditionList;
			QString textFilterQuery;
			QString objectFilterQuery;
			QString timeFilterQuery;

			CreateTextFilterQuery(*complexFilterParamPtr, textFilterQuery);
			if (!textFilterQuery.isEmpty()){
				conditionList << textFilterQuery.toUtf8();
			}

			CreateObjectFilterQuery(*complexFilterParamPtr, objectFilterQuery);
			if (!objectFilterQuery.isEmpty()){
				conditionList << objectFilterQuery.toUtf8();
			}

			CreateTimeFilterQuery(complexFilterParamPtr->GetTimeFilter(), timeFilterQuery);
			if (!timeFilterQuery.isEmpty()){
				conditionList << timeFilterQuery.toUtf8();
			}

			if (conditionList.isEmpty()){
				return QByteArray();
			}

			retVal += QByteArrayLiteral(" AND (") += conditionList.join(" AND ") +  QByteArrayLiteral(")");
		}
	}

	return retVal;
}


QByteArray CSqlDatabaseDocumentDelegateComp::CreateRestoreObjectsQuery(
			const imtbase::IObjectCollection& /*collection*/,
			const imtbase::ICollectionInfo::Ids& objectIds,
			const imtbase::IOperationContext* /*operationContextPtr*/) const
{
	if (objectIds.isEmpty()){
		return QByteArray();
	}

	const QString quotedIds = objectIds.toList().join("','").append('\'').prepend('\'');

	QString schemaPrefix;
	if (m_tableSchemaAttrPtr.IsValid()){
		schemaPrefix = QString("%1.").arg(qPrintable(*m_tableSchemaAttrPtr));
	}

	return QString(R"(
				UPDATE %0 "%1"
				SET "%2" = 'Active'
				WHERE "%3" IN (%4)
				AND "State" = 'Disabled';)")
				.arg(
					schemaPrefix,
					QString::fromUtf8(*m_tableNameAttrPtr),
					QString::fromUtf8(s_stateColumn),
					QString::fromUtf8(s_documentIdColumn),
					quotedIds
			).toUtf8();
}


QByteArray CSqlDatabaseDocumentDelegateComp::CreateRestoreObjectSetQuery(
			const imtbase::IObjectCollection& /*collection*/,
			const iprm::IParamsSet* paramsPtr,
			const imtbase::IOperationContext* /*operationContextPtr*/) const
{
	QString schemaPrefix;
	if (m_tableSchemaAttrPtr.IsValid()){
		schemaPrefix = QString("%1.").arg(qPrintable(*m_tableSchemaAttrPtr));
	}
	QByteArray retVal = 
				QString("UPDATE %0 \"%1\" as root SET \"%2\" = 'Active' WHERE \"%2\" = 'Disabled'")
							.arg(schemaPrefix, QString::fromUtf8(*m_tableNameAttrPtr), QString::fromUtf8(s_stateColumn)).toUtf8();

	if (paramsPtr != nullptr){
		iprm::TParamsPtr<imtbase::IComplexCollectionFilter> complexFilterParamPtr(paramsPtr, "ComplexFilter");
		if (complexFilterParamPtr.IsValid()){
			QByteArrayList conditionList;
			QString textFilterQuery;
			QString objectFilterQuery;
			QString timeFilterQuery;

			CreateTextFilterQuery(*complexFilterParamPtr, textFilterQuery);
			if (!textFilterQuery.isEmpty()){
				conditionList << textFilterQuery.toUtf8();
			}

			CreateObjectFilterQuery(*complexFilterParamPtr, objectFilterQuery);
			if (!objectFilterQuery.isEmpty()){
				conditionList << objectFilterQuery.toUtf8();
			}

			CreateTimeFilterQuery(complexFilterParamPtr->GetTimeFilter(), timeFilterQuery);
			if (!timeFilterQuery.isEmpty()){
				conditionList << timeFilterQuery.toUtf8();
			}

			if (conditionList.isEmpty()){
				return QByteArray();
			}

			retVal += QByteArrayLiteral(" AND (") += conditionList.join(" AND ") +  QByteArrayLiteral(")");
		}
	}

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
	QString schemaPrefix;
	if (m_tableSchemaAttrPtr.IsValid()){
		schemaPrefix = QString("%1.").arg(qPrintable(*m_tableSchemaAttrPtr));
	}
	QByteArray countRevisionsQuery = 
				QString("(SELECT MAX(%0) + 1 FROM %1 \"%2\" as root WHERE \"%3\" = '%4')").arg(
							CreateJsonExtractSql(s_revisionInfoColumn, s_revisionNumberKey, QMetaType::Int),
										schemaPrefix,
										qPrintable(*m_tableNameAttrPtr),
										qPrintable(s_documentIdColumn),
										qPrintable(objectId)).toUtf8();

	RawSqlExpression sqlExpression;
	sqlExpression.sql = countRevisionsQuery;

	QByteArray retVal;

	QByteArray typeId = collection.GetObjectTypeId(objectId);
	QString objectName = collection.GetElementInfo(objectId, imtbase::ICollectionInfo::EIT_NAME).toString();
	QString objectDescription = collection.GetElementInfo(objectId, imtbase::ICollectionInfo::EIT_DESCRIPTION).toString();
	if (typeId.isEmpty()){
		return retVal;
	}

	retVal = PrepareInsertNewObjectQuery(
				typeId, objectId, objectName, objectDescription, object, operationContextPtr, QVariant::fromValue(sqlExpression));

	return retVal;
}


QByteArray CSqlDatabaseDocumentDelegateComp::CreateRenameObjectQuery(
			const imtbase::IObjectCollection& /*collection*/,
			const QByteArray& objectId,
			const QString& newObjectName,
			const imtbase::IOperationContext* /*operationContextPtr*/) const
{
	QString schemaPrefix;
	if (m_tableSchemaAttrPtr.IsValid()){
		schemaPrefix = QString("%1.").arg(qPrintable(*m_tableSchemaAttrPtr));
	}
	QByteArray retVal = QString("UPDATE %0 \"%1\" SET \"%2\" = '%3' WHERE \"%4\" = '%5';")
						.arg(
							schemaPrefix,
							qPrintable(*m_tableNameAttrPtr),
							qPrintable(s_nameColumn),
							newObjectName,
							qPrintable(s_documentIdColumn),
							qPrintable(objectId)
							).toUtf8();

	return retVal;
}


QByteArray CSqlDatabaseDocumentDelegateComp::CreateDescriptionObjectQuery(
			const imtbase::IObjectCollection& /*collection*/,
			const QByteArray& objectId,
			const QString& description,
			const imtbase::IOperationContext* /*operationContextPtr*/) const
{
	QString schemaPrefix;
	if (m_tableSchemaAttrPtr.IsValid()){
		schemaPrefix = QString("%1.").arg(qPrintable(*m_tableSchemaAttrPtr));
	}
	QByteArray retVal = QString("UPDATE %0 \"%1\" SET \"%2\" = '%3' WHERE \"%4\" = '%5';")
						.arg(
							schemaPrefix,
							qPrintable(*m_tableNameAttrPtr),
							qPrintable(s_descriptionColumn),
							description,
							qPrintable(s_documentIdColumn),
							qPrintable(objectId)
							).toUtf8();

	return retVal;
}


QByteArray CSqlDatabaseDocumentDelegateComp::GetSelectionByMetaInfoQuery(
			const QByteArray& metaInfoId,
			const QVariant& metaInfoValue) const
{
	QString schemaPrefix;
	if (m_tableSchemaAttrPtr.IsValid()){
		schemaPrefix = QString("%1.").arg(qPrintable(*m_tableSchemaAttrPtr));
	}
	return QString(R"(SELECT * FROM %0 "%1" WHERE ("%2" = 'Active') AND %3 = '%4';)")
			.arg(
				schemaPrefix,
				qPrintable(*m_tableNameAttrPtr),
				qPrintable(s_stateColumn),
				CreateJsonExtractSql(s_dataMetaInfoColumn, metaInfoId),
				qPrintable(metaInfoValue.toByteArray())
				).toUtf8();
}


QByteArray CSqlDatabaseDocumentDelegateComp::CreateUpdateMetaInfoQuery(const QSqlRecord& record) const
{
	istd::IChangeableUniquePtr objectPtr = CreateObjectFromRecord(record);
	if (!objectPtr.IsValid()){
		return QByteArray();
	}

	QByteArray objectId = record.value(qPrintable(s_documentIdColumn)).toByteArray();
	QByteArray typeId = record.value(qPrintable(s_typeIdColumn)).toByteArray();

	QByteArray metaInfoRepresentation = QByteArrayLiteral("{}");

	if (m_metaInfoCreatorCompPtr.IsValid()){
		idoc::MetaInfoPtr metaInfoPtr = CreateObjectMetaInfo(typeId);
		if (metaInfoPtr.IsValid()){
			if (m_jsonBasedMetaInfoDelegateCompPtr.IsValid()){
				if (!m_jsonBasedMetaInfoDelegateCompPtr->ToJsonRepresentation(*metaInfoPtr.GetPtr(), metaInfoRepresentation, typeId)){
					SendErrorMessage(0, "Meta information could not be converted to the JSON-based representation");

					return QByteArray();
				}
			}
		}
	}

	QString schemaPrefix;
	if (m_tableSchemaAttrPtr.IsValid()){
		schemaPrefix = QString("%1.").arg(qPrintable(*m_tableSchemaAttrPtr));
	}

	QByteArray query = QString(R"(UPDATE %0 "%1" SET "%2" = '%3' WHERE "%4" = 'Active' AND "%5" = '%6';)")
						.arg(
							schemaPrefix,
							qPrintable(*m_tableNameAttrPtr),
							qPrintable(s_dataMetaInfoColumn),
							qPrintable(SqlEncode(metaInfoRepresentation)),
							qPrintable(s_stateColumn),
							qPrintable(s_documentIdColumn),
							qPrintable(objectId)
							)
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

		if (revisionRecord.contains(s_revisionInfoColumn)){
			QByteArray revisionValueJson = revisionRecord.value(qPrintable(s_revisionInfoColumn)).toByteArray();
			QJsonObject infoData = QJsonDocument::fromJson(revisionValueJson).object();
			revisionInfo.revision = infoData.value(s_revisionNumberKey).toInt();
			revisionInfo.user = infoData.value(s_ownerNameKey).toString();

			QJsonValue operationDescriptionValue = infoData.value(s_operationDescriptionKey);
			if (operationDescriptionValue.isObject()){
				QJsonObject operationObj = operationDescriptionValue.toObject();

				revisionInfo.comment = QJsonDocument(operationObj).toJson();
			}
			else{
				revisionInfo.comment = infoData.value(s_operationDescriptionKey).toString();
			}
		}

		if (revisionRecord.contains(s_lastModifiedColumn)){
			revisionInfo.timestamp = revisionRecord.value(qPrintable(s_lastModifiedColumn)).toDateTime();
		}

		if (revisionRecord.contains(s_stateColumn)){
			revisionInfo.isRevisionAvailable = (revisionRecord.value(qPrintable(s_stateColumn)).toString() == "Active" || revisionRecord.value(qPrintable(s_stateColumn)).toString() == "Disabled");
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

	QString schemaPrefix;
	if (m_tableSchemaAttrPtr.IsValid()){
		schemaPrefix = QString("%1.").arg(qPrintable(*m_tableSchemaAttrPtr));
	}

	QByteArray query = QString("UPDATE %0 \"%1\" SET \"%2\" = 'InActive' WHERE \"%3\" = '%4';")
			.arg(
						schemaPrefix,
						qPrintable(*m_tableNameAttrPtr),
						qPrintable(s_stateColumn),
						qPrintable(s_documentIdColumn),
						qPrintable(objectId)).toUtf8();

	query += QString(R"(UPDATE %0 "%1" SET "%2" = 'Active' WHERE "%3" = '%4' AND %5 = %6;)")
				.arg(
							schemaPrefix,
							qPrintable(*m_tableNameAttrPtr),
							qPrintable(s_stateColumn),
							qPrintable(s_documentIdColumn),
							qPrintable(objectId),
							CreateJsonExtractSql(s_revisionInfoColumn, s_revisionNumberKey, QMetaType::Int))
			.arg(revision).toUtf8();

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
	QString schemaPrefix;
	if (m_tableSchemaAttrPtr.IsValid()){
		schemaPrefix = QString("%1.").arg(qPrintable(*m_tableSchemaAttrPtr));
	}
	QByteArray checkCurrentRevisionQuery = QString("SELECT * FROM %0 \"%1\" WHERE \"%2\" = '%3' AND \"%4\" = 'Active';")
				.arg(
							schemaPrefix,
							qPrintable(*m_tableNameAttrPtr),
							qPrintable(s_documentIdColumn),
							qPrintable(objectId),
							qPrintable(s_stateColumn)).toUtf8();

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

	if (revisionRecord.contains(s_revisionInfoColumn)){
		QByteArray revisionValueJson = revisionRecord.value(qPrintable(s_revisionInfoColumn)).toByteArray();
		QJsonObject infoData = QJsonDocument::fromJson(revisionValueJson).object();
		currentRevision = infoData.value(s_revisionNumberKey).toInt();
	}

	if (currentRevision == revision){
		SendErrorMessage(0, QString("Unable to delete revision '%1' for document '%2'. Error: Revision '%1' is active").arg(revision).arg(qPrintable(objectId)), "Database collection");
		return false;
	}

	QByteArray query = QString("DELETE  FROM %0 \"%1\" WHERE \"%2\" = '%3' AND %4 = %5;")
						.arg(
							schemaPrefix,
							qPrintable(*m_tableNameAttrPtr),
							qPrintable(s_documentIdColumn),
							qPrintable(objectId),
							CreateJsonExtractSql(s_revisionInfoColumn, s_revisionNumberKey, QMetaType::Int)
							)
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

	QString schemaPrefix;
	if (m_tableSchemaAttrPtr.IsValid()){
		schemaPrefix = QString("%1.").arg(qPrintable(*m_tableSchemaAttrPtr));
	}

	QString query = QString("UPDATE %0 \"%1\" SET \"%2\" = ").arg(schemaPrefix, tableName, qPrintable(s_dataMetaInfoColumn));

	QString jsonbUpdate = QString("\"%1\"").arg(qPrintable(s_dataMetaInfoColumn));

	for (int i = 0; i < metaInfo.metaInfoIds.size(); i++){
		jsonbUpdate = QString("jsonb_set(%1, '{%2}', to_jsonb(%3))").arg(jsonbUpdate, metaInfo.metaInfoIds[i], metaInfo.metaInfoValues[i]);
	}

	query += jsonbUpdate + QString(" WHERE %1 = '%2';").arg(CreateJsonExtractSql(s_dataMetaInfoColumn, metaInfo.dependentKey), metaInfo.objectId);

	QSqlError sqlError;
	m_databaseEngineCompPtr->ExecSqlQuery(query.toUtf8(), &sqlError);
	if (sqlError.type() != QSqlError::NoError){
		SendErrorMessage(0, sqlError.text(), "CSqlDatabaseDocumentDelegateComp");
		qDebug() << "SQL-error" << sqlError.text();

		return false;
	}

	return true;
}

bool CSqlDatabaseDocumentDelegateComp::ClearDependentMetaInfo(const MetaFieldCleanupPlan& metaInfo) const
{
	if (!m_databaseEngineCompPtr.IsValid()){
		Q_ASSERT(false);
		return false;
	}

	if (metaInfo.objectIds.isEmpty()){
		return false;
	}

	QString tableName = GetTableName();

	QString jsonbExpr = QString("\"%1\"").arg(qPrintable(s_dataMetaInfoColumn));

	for (const QString& metaInfoId : metaInfo.metaInfoIds){
		jsonbExpr = QString("jsonb_set(%1, '{%2}', to_jsonb(''::text))")
			.arg(jsonbExpr, metaInfoId);
	}

	jsonbExpr = QString("jsonb_set(%1, '{%2}', to_jsonb(''::text))")
		.arg(jsonbExpr, metaInfo.dependentKey);

	QStringList quotedIds;
	for (const QByteArray& id : metaInfo.objectIds) {
		QString escapedId = QString(id).replace("'", "''");
		quotedIds << QString("'%1'").arg(escapedId);
	}

	QString whereClause = QString("%1 IN (%2)")
		.arg(CreateJsonExtractSql(s_dataMetaInfoColumn, metaInfo.dependentKey), quotedIds.join(", "));

	QString schemaPrefix;
	if (m_tableSchemaAttrPtr.IsValid()){
		schemaPrefix = QString("%1.").arg(qPrintable(*m_tableSchemaAttrPtr));
	}

	QString fullQuery = QString("UPDATE %0 \"%1\" SET \"%2\" = %3 WHERE %4;")
		.arg(
				schemaPrefix,
				tableName,
				qPrintable(s_dataMetaInfoColumn),
				jsonbExpr,
				whereClause);

	QSqlError sqlError;
	m_databaseEngineCompPtr->ExecSqlQuery(fullQuery.toUtf8(), &sqlError);

	if (sqlError.type() != QSqlError::NoError){
		SendErrorMessage(0, sqlError.text(), "CSqlDatabaseDocumentDelegateComp");
		qDebug() << "SQL-error:" << sqlError.text();
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

	QByteArray documentContentJson;
	if (!WriteDataToMemory(typeId, object, documentContentJson)){
		SendCriticalMessage(0, QStringLiteral("Document data could not be written to the memory"), __FILE__);

		return retVal;
	}

	const quint32 checksum = istd::CCrcCalculator::GetCrcFromData((const quint8*)documentContentJson.constData(), imtbase::narrow_cast<int>(documentContentJson.size()));

	QString schemaPrefix;
	if (m_tableSchemaAttrPtr.IsValid()){
		schemaPrefix = QString("%1.").arg(qPrintable(*m_tableSchemaAttrPtr));
	}

	// Insert new entry into the document list table:
	QString query = QString("UPDATE %0 \"%1\" SET \"%2\" = 'InActive' WHERE \"%3\" = '%4';")
					.arg(
						schemaPrefix,
						qPrintable(*m_tableNameAttrPtr),
						qPrintable(s_stateColumn),
						qPrintable(s_documentIdColumn),
						qPrintable(objectId)
						);

	QByteArray metaInfoRepresentation;

	const bool useMetaData = m_useDataMetaInfoAttrPtr.IsValid() ? *m_useDataMetaInfoAttrPtr : false;
	if (useMetaData && m_metaInfoCreatorCompPtr.IsValid() && m_jsonBasedMetaInfoDelegateCompPtr.IsValid()){
		idoc::MetaInfoPtr metaInfoPtr;
		if (m_metaInfoCreatorCompPtr->CreateMetaInfo(&object, typeId, metaInfoPtr) && metaInfoPtr.IsValid()){
			if (!m_jsonBasedMetaInfoDelegateCompPtr->ToJsonRepresentation(*metaInfoPtr.GetPtr(), metaInfoRepresentation, typeId)){
				SendWarningMessage(0, QString("Unable to create meta info representation for the object '%1' from the table '%2'").arg(objectId, GetTableName()));
			}
		}
	}

	QByteArray revisionInfoQuery;
	revisionInfoQuery = CreateRevisionInfoQuery(operationContextPtr, revisionArgument, checksum);

	static const QString nullDataLiteral = QStringLiteral("NULL");

	query += QString("INSERT INTO %0 \"%1\"(\"%2\", \"%3\", \"%4\", \"%5\", \"%6\", \"%7\", \"%8\", \"%9\", \"%10\", \"%11\") VALUES('%12', '%13', '%14', %15, %16, '%17', %18, %19, '%20', '%21');")
				.arg(
						schemaPrefix,
						qPrintable(*m_tableNameAttrPtr),
						qPrintable(s_idColumn),
						qPrintable(s_typeIdColumn),
						qPrintable(s_documentIdColumn),
						qPrintable(s_nameColumn),
						qPrintable(s_descriptionColumn),
						qPrintable(s_documentColumn),
						qPrintable(s_dataMetaInfoColumn),
						qPrintable(s_revisionInfoColumn),
						qPrintable(s_lastModifiedColumn),
						qPrintable(s_stateColumn),
						QUuid::createUuid().toString(QUuid::WithoutBraces),
						qPrintable(typeId),
						qPrintable(objectId),
						objectName.isEmpty() ? nullDataLiteral : SqlEncode(objectName).append('\'').prepend('\''),
						objectDescription.isEmpty() ? nullDataLiteral : SqlEncode(objectDescription).append('\'').prepend('\''),
						SqlEncode(documentContentJson),
						metaInfoRepresentation.isEmpty() ? nullDataLiteral : SqlEncode(metaInfoRepresentation).append('\'').prepend('\''),
						revisionInfoQuery.isEmpty() ? nullDataLiteral : revisionInfoQuery,
						QDateTime::currentDateTimeUtc().toString(Qt::ISODateWithMs),
						QStringLiteral("Active")
					);

	retVal = query.toUtf8();

	return retVal;
}


istd::IChangeableUniquePtr CSqlDatabaseDocumentDelegateComp::CreateObject(const QByteArray& typeId) const
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


bool CSqlDatabaseDocumentDelegateComp::WriteDataToMemory(const QByteArray& /*typeId*/, const istd::IChangeable& object, QByteArray& data) const
{
	iser::ISerializable* serializableObjectPtr = const_cast<iser::ISerializable*>(dynamic_cast<const iser::ISerializable*>(&object));
	if (serializableObjectPtr == nullptr){
		Q_ASSERT(0);
		return false;
	}

	iser::CJsonMemWriteArchive archive(m_versionInfoCompPtr.GetPtr());
	if (!serializableObjectPtr->Serialize(archive)){
		SendErrorMessage(0, "Unable to write data to memory. Error: Serialization failed", "CSqlJsonDatabaseDelegateComp");
		return false;
	}

	data = archive.GetData();

	return true;
}


bool CSqlDatabaseDocumentDelegateComp::ReadDataFromMemory(const QByteArray& /*typeId*/, const QByteArray& data, istd::IChangeable& object) const
{
	iser::ISerializable* serializableObjectPtr = dynamic_cast<iser::ISerializable*>(&object);
	if (serializableObjectPtr == nullptr){
		Q_ASSERT(0);
		return false;
	}

	iser::CJsonMemReadArchive archive(data);

	if (!serializableObjectPtr->Serialize(archive)){
		SendErrorMessage(0, "Unable to read data from memory. Error: Serialization failed", "CSqlJsonDatabaseDelegateComp");
		return false;
	}

	return true;
}


QByteArray CSqlDatabaseDocumentDelegateComp::CreateRevisionInfoQuery(
			const imtbase::IOperationContext* operationContextPtr,
			const QVariant& revisionArgument,
			quint32 checksum) const
{
	QVariantMap paramMap;
	paramMap[s_checksumKey] = QString::number(checksum);
	paramMap[s_revisionNumberKey] = revisionArgument;

	if (operationContextPtr != nullptr){
		imtbase::IOperationContext::IdentifableObjectInfo objectInfo = operationContextPtr->GetOperationOwnerId();
		paramMap[s_ownerIdKey] = objectInfo.id;
		paramMap[s_ownerNameKey] = objectInfo.name;

		QByteArray json;

		imtbase::IOperationContext* nonConstOperationPtr = const_cast<imtbase::IOperationContext*>(operationContextPtr);
		if (nonConstOperationPtr != nullptr){
			iser::ISerializable* changeCollectionPtr = dynamic_cast<iser::ISerializable*>(nonConstOperationPtr->GetChangesCollection());
			if (changeCollectionPtr != nullptr){
				iser::CJsonMemWriteArchive archive(m_versionInfoCompPtr.GetPtr());
				if (!changeCollectionPtr->Serialize(archive)){
					qDebug() << QString("Unable to serialize a change object collection");
				}
				else{
					json = archive.GetData();
				}
			}
		}

		paramMap[s_operationDescriptionKey] = SqlEncode(json);
	}

	QString revisionInfo = CreateJsonBuildObjectQuery(paramMap);

	return revisionInfo.toUtf8();
}


QByteArray CSqlDatabaseDocumentDelegateComp::CreateJsonBuildObjectQuery(const QVariantMap& paramMap) const
{
	QString revisionInfo = QStringLiteral(R"(jsonb_build_object()");

	const QStringList keys = paramMap.keys();

	for (int i = 0; i < keys.size(); i++){
		if (i > 0){
			revisionInfo += QStringLiteral(", ");
		}

		QString key = keys[i];
		QVariant value = paramMap[key];

#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
		const int typeId = value.typeId();
#else
		const int typeId = value.type();
#endif
		if (value.canConvert<RawSqlExpression>()){
			RawSqlExpression raw = value.value<RawSqlExpression>();
			revisionInfo += QStringLiteral("'%1', %2").arg(key, raw.sql);
		}
		else if (typeId == QMetaType::QString || typeId == QMetaType::QByteArray){
			revisionInfo += QStringLiteral("'%1', '%2'").arg(key, value.toString());
		}
		else if (typeId == QMetaType::Int){
			revisionInfo += QStringLiteral("'%1', %2").arg(key).arg(value.toInt());
		}
		else if (typeId == QMetaType::Bool){
			revisionInfo += QStringLiteral("'%1', %2").arg(key).arg(value.toBool());
		}
	}

	revisionInfo += ')';

	return revisionInfo.toUtf8();
}


QString CSqlDatabaseDocumentDelegateComp::CreateJsonExtractSql(
			const QString& jsonName,
			const QString& key,
			QMetaType::Type metaType,
			const QString& tableAlias) const
{
	if (m_sqlJsonXPathExtractorCompPtr.IsValid()){
		return m_sqlJsonXPathExtractorCompPtr->ExtractXPath(jsonName, key, metaType, tableAlias);
	}

	return QString();
}


// reimplemented (imtdb::CSqlDatabaseObjectDelegateCompBase)

QString CSqlDatabaseDocumentDelegateComp::GetBaseSelectionQuery() const
{
	QString schema;
	if (m_tableSchemaAttrPtr.IsValid()){
		schema = *m_tableSchemaAttrPtr + '.';
	}

	Q_ASSERT(!(*m_tableNameAttrPtr).isEmpty());

	QString tableName = *m_tableNameAttrPtr;
	QByteArray joinTablesQuery = CreateJoinTablesQuery();

	QString customColumns = GetCustomColumnsQuery();
	if (!customColumns.isEmpty()){
		customColumns = QStringLiteral(", ") + customColumns;
	}

	QString query = QStringLiteral(R"(
			SELECT
				root.*,
				root1."%6" as "%7" %5
			FROM %2"%3" as root
			LEFT JOIN (
				SELECT DISTINCT ON ("%8")
					"%8", "%6", "%9"
				FROM %2"%3"
				WHERE %1 = 1
				ORDER BY "%8", "%6" DESC
			) AS root1 ON root1."%8" = root."%8"
			%4
		)")
					.arg(
						/*1*/ CreateJsonExtractSql(s_revisionInfoColumn, s_revisionNumberKey, QMetaType::Int),
						/*2*/ schema,
						/*3*/ tableName,
						/*4*/ joinTablesQuery.isEmpty() ? QString() : QString::fromUtf8(joinTablesQuery),
						/*5*/ customColumns,
						/*6*/ QString::fromUtf8(s_lastModifiedColumn),
						/*7*/ QString::fromUtf8(s_addedColumn),
						/*8*/ QString::fromUtf8(s_documentIdColumn),
						/*9*/ QString::fromUtf8(s_revisionInfoColumn)
					);

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
	if (record.contains(s_dataMetaInfoColumn)){
		QByteArray metaInfoRepresentation = record.value(qPrintable(s_dataMetaInfoColumn)).toByteArray();
		QByteArray typeId = record.value(qPrintable(s_typeIdColumn)).toByteArray();
		if (m_jsonBasedMetaInfoDelegateCompPtr.IsValid()){
			return m_jsonBasedMetaInfoDelegateCompPtr->FromJsonRepresentation(metaInfoRepresentation, metaInfo, typeId);
		}
	}

	return true;
}


bool CSqlDatabaseDocumentDelegateComp::CreateObjectFilterQuery(const iprm::IParamsSet& filterParams, QString& filterQuery) const
{
	filterQuery.clear();

	iprm::IParamsSet::Ids paramIds = filterParams.GetParamIds();
	if (paramIds.contains("Filter")){
		iprm::TParamsPtr<imtbase::ICollectionFilter> collectionFilterParamPtr(&filterParams, "Filter");
		if (collectionFilterParamPtr.IsValid()){
			QByteArray typeId = collectionFilterParamPtr->GetObjectTypeId();
			if (!typeId.isEmpty()){
				filterQuery = QString("\"%0\" = '%1'").arg(qPrintable(s_typeIdColumn), qPrintable(typeId)).toUtf8();
			}
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
			sortQuery = QString("ORDER BY %1 %2").arg(CreateJsonExtractSql(QString(R"(root."%1")").arg(qPrintable(s_dataMetaInfoColumn)), columnId), qPrintable(sortOrder));
		}
	}

	return true;
}


bool CSqlDatabaseDocumentDelegateComp::CreateSortQuery(const imtbase::IComplexCollectionFilter& collectionFilter, QString& sortQuery) const
{
	bool retVal = BaseClass::CreateSortQuery(collectionFilter, sortQuery);

	if (!sortQuery.isEmpty()) {
		SubstituteFieldIds(sortQuery, false);
	}

	return retVal;
}


bool CSqlDatabaseDocumentDelegateComp::CreateFilterQuery(const iprm::IParamsSet& filterParams, QString& filterQuery) const
{
	iprm::IParamsSet::Ids paramIds = filterParams.GetParamIds();

	QString objectTypeIdQuery;
	QString textFilterQuery;
	QString objectFilterQuery;
	QString timeFilterQuery;

	if (paramIds.contains("ObjectTypeIdFilter")){
		iprm::TParamsPtr<imtcol::IObjectTypeIdFilter> objectTypeIdFilterPtr(&filterParams, "ObjectTypeIdFilter");
		if (objectTypeIdFilterPtr.IsValid()){
			QByteArray objectTypeId = objectTypeIdFilterPtr->GetObjectTypeId();
			if (!objectTypeId.isEmpty()){
				objectTypeIdQuery = QString("\"%0\" = '%1'").arg(qPrintable(s_typeIdColumn), qPrintable(objectTypeId)).toUtf8();
			}
		}
	}

	if (paramIds.contains("ComplexFilter")){
		iprm::TParamsPtr<imtbase::IComplexCollectionFilter> complexFilterParamPtr(&filterParams, "ComplexFilter");
		if (complexFilterParamPtr.IsValid()){
			CreateTextFilterQuery(*complexFilterParamPtr, textFilterQuery);
			CreateObjectFilterQuery(*complexFilterParamPtr, objectFilterQuery);
			CreateTimeFilterQuery(complexFilterParamPtr->GetTimeFilter(), timeFilterQuery);
		}
	}

	QString documentIdFilterQuery;
	if (paramIds.contains("DocumentIdFilter")){
		iprm::TParamsPtr<imtcol::IDocumentIdFilter> documentFilterParamPtr(&filterParams, "DocumentIdFilter");
		if (documentFilterParamPtr.IsValid()){
			CreateDocumentIdFilterQuery(*documentFilterParamPtr.GetPtr(), documentIdFilterQuery);
		}
	}

	QString documentFilterQuery;
	if (paramIds.contains("DocumentFilter")){
		iprm::TParamsPtr<imtcol::IDocumentCollectionFilter> documentFilterParamPtr(&filterParams, "DocumentFilter");
		if (documentFilterParamPtr.IsValid()){
			CreateDocumentCollectionFilterQuery(*documentFilterParamPtr.GetPtr(), documentFilterQuery);
		}
	}

	if (documentFilterQuery.isEmpty()){
		documentFilterQuery = QString("root.\"%0\" = 'Active'").arg(QString::fromUtf8(s_stateColumn));
	}

	QString additionalFilters = CreateAdditionalFiltersQuery(filterParams);

	if (!objectTypeIdQuery.isEmpty()){
		if (!filterQuery.isEmpty()){
			filterQuery += " AND ";
		}

		filterQuery += "(" + objectTypeIdQuery + ")";
	}

	if (!textFilterQuery.isEmpty()){
		if (!filterQuery.isEmpty()){
			filterQuery += " AND ";
		}

		filterQuery += "(" + textFilterQuery + ")";
	}

	if (!objectFilterQuery.isEmpty()){
		if (!filterQuery.isEmpty()){
			filterQuery += " AND ";
		}

		filterQuery += "(" + objectFilterQuery + ")";
	}

	if (!timeFilterQuery.isEmpty()){
		if (!filterQuery.isEmpty()){
			filterQuery += " AND ";
		}

		filterQuery += "(" + timeFilterQuery + ")";
	}

	if (!documentFilterQuery.isEmpty()){
		if (!filterQuery.isEmpty()){
			filterQuery += " AND ";
		}

		filterQuery += "(" + documentFilterQuery + ")";
	}

	if (!documentIdFilterQuery.isEmpty()){
		if (!filterQuery.isEmpty()){
			filterQuery += " AND ";
		}

		filterQuery += "(" + documentIdFilterQuery + ")";
	}

	if (!additionalFilters.isEmpty()){
		if (!filterQuery.isEmpty()){
			filterQuery += " AND ";
		}

		filterQuery += "(" + additionalFilters + ")";
	}

	filterQuery = " WHERE " + filterQuery;

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
			textFilterQuery += QString("%0%1 ILIKE '%%2%'")
								.arg(
									textFilterQuery.isEmpty() ? "" : " OR ",
									CreateJsonExtractSql(QString(R"(root."%1")").arg(qPrintable(s_dataMetaInfoColumn)), filteringColumnIds[i]),
									textFilter
									);
		}
	}

	return true;
}


bool CSqlDatabaseDocumentDelegateComp::CreateTimeFilterQuery(const imtbase::ITimeFilterParam& timeFilter, QString& timeFilterQuery, const QString& timeFieldId) const
{
	switch (timeFilter.GetTimeUnit()){
	case imtbase::ITimeFilterParam::TU_CUSTOM:
		break;
	case imtbase::ITimeFilterParam::TU_HOUR:
		switch(timeFilter.GetInterpretationMode()){
		case imtbase::ITimeFilterParam::IM_CURRENT:
			timeFilterQuery += QString(R"((date_trunc('hour', %1) = date_trunc('hour', current_timestamp at time zone 'utc')))").arg(timeFieldId);
			break;
		case imtbase::ITimeFilterParam::IM_LAST:
			timeFilterQuery += QString(R"((date_trunc('hour', %1) = date_trunc('hour', current_timestamp at time zone 'utc') - interval '1 hour'))").arg(timeFieldId);
			break;
		case imtbase::ITimeFilterParam::IM_FOR:
			timeFilterQuery += QString(R"((%1 >= current_timestamp at time zone 'utc' - interval '1 hour' and %1 <= current_timestamp at time zone 'utc'))").arg(timeFieldId);
			break;
		}
		break;
	case imtbase::ITimeFilterParam::TU_DAY:
		switch(timeFilter.GetInterpretationMode()){
		case imtbase::ITimeFilterParam::IM_CURRENT:
			timeFilterQuery += QString(R"((date_trunc('day', %1) = date_trunc('day', current_timestamp at time zone 'utc')))").arg(timeFieldId);
			break;
		case imtbase::ITimeFilterParam::IM_LAST:
			timeFilterQuery += QString(R"((date_trunc('day', %1) = date_trunc('day', current_timestamp at time zone 'utc') - interval '1 day'))").arg(timeFieldId);
			break;
		case imtbase::ITimeFilterParam::IM_FOR:
			timeFilterQuery += QString(R"((%1 >= current_timestamp at time zone 'utc' - interval '1 day' and %1 <= current_timestamp at time zone 'utc'))").arg(timeFieldId);
			break;
		}
		break;
	case imtbase::ITimeFilterParam::TU_WEEK:
		switch(timeFilter.GetInterpretationMode()){
		case imtbase::ITimeFilterParam::IM_CURRENT:
			timeFilterQuery += QString(R"((date_trunc('week', %1) = date_trunc('week', current_timestamp at time zone 'utc')))").arg(timeFieldId);
			break;
		case imtbase::ITimeFilterParam::IM_LAST:
			timeFilterQuery += QString(R"((date_trunc('week', %1) = date_trunc('week', current_timestamp at time zone 'utc') - interval '1 week'))").arg(timeFieldId);
			break;
		case imtbase::ITimeFilterParam::IM_FOR:
				timeFilterQuery += QString(R"((date(%1) >= current_date - interval '6 days' AND date(%1) <= current_date))").arg(timeFieldId);
				break;
			}
		break;
	case imtbase::ITimeFilterParam::TU_MONTH:
		switch(timeFilter.GetInterpretationMode()){
		case imtbase::ITimeFilterParam::IM_CURRENT:
			timeFilterQuery += QString(R"((date_trunc('month', %1) = date_trunc('month', current_timestamp at time zone 'utc')))").arg(timeFieldId);
			break;
		case imtbase::ITimeFilterParam::IM_LAST:
			timeFilterQuery += QString(R"((date_trunc('month', %1) = date_trunc('month', current_timestamp at time zone 'utc') - interval '1 month'))").arg(timeFieldId);
			break;
		case imtbase::ITimeFilterParam::IM_FOR:
			timeFilterQuery += QString(R"((%1 >= current_timestamp at time zone 'utc' - interval '1 month' and %1 <= current_timestamp at time zone 'utc'))").arg(timeFieldId);
			break;
		}
		break;
	case imtbase::ITimeFilterParam::TU_YEAR:
		switch(timeFilter.GetInterpretationMode()){
		case imtbase::ITimeFilterParam::IM_CURRENT:
			timeFilterQuery += QString(R"((date_trunc('year', %1) = date_trunc('year', current_timestamp at time zone 'utc')))").arg(timeFieldId);
			break;
		case imtbase::ITimeFilterParam::IM_LAST:
			timeFilterQuery += QString(R"((date_trunc('year', %1) = date_trunc('year', current_timestamp at time zone 'utc') - interval '1 year'))").arg(timeFieldId);
			break;
		case imtbase::ITimeFilterParam::IM_FOR:
			timeFilterQuery += QString(R"((%1 >= current_timestamp at time zone 'utc' - interval '1 year' and %1 <= current_timestamp at time zone 'utc'))").arg(timeFieldId);
			break;
		}
		break;
	}

	if (timeFilterQuery.isEmpty()){
		imtbase::CTimeRange timeRange = timeFilter.GetTimeRange();
		if (!timeRange.IsNull()){
			timeFilterQuery += QString(R"(date(%0) >= date('%1') AND date(%0) <= date('%2'))")
								.arg(timeFieldId, timeRange.GetBeginTime().toString(Qt::ISODateWithMs), timeRange.GetEndTime().toString(Qt::ISODateWithMs));
		}
	}

	return true;
}


// protected methods

bool CSqlDatabaseDocumentDelegateComp::CreateObjectFilterQuery(const imtbase::IComplexCollectionFilter& collectionFilter, QString& filterQuery) const
{
	filterQuery = CComplexCollectionFilterConverter::CreateSqlFilterQuery(collectionFilter, CComplexCollectionFilterConverter::SC_POSTGRES);
	if (!filterQuery.isEmpty()) {
		SubstituteFieldIds(filterQuery);
	}

	return true;
}


bool CSqlDatabaseDocumentDelegateComp::CreateTextFilterQuery(const imtbase::IComplexCollectionFilter& collectionFilter, QString& textFilterQuery) const
{
	textFilterQuery.clear();

	bool useDataMeta = m_useDataMetaInfoAttrPtr.IsValid() ? *m_useDataMetaInfoAttrPtr : false;
	QString jsonKey = useDataMeta ? s_dataMetaInfoColumn : s_documentColumn;

	QString textFilter = collectionFilter.GetTextFilter();
	if (!textFilter.isEmpty()){
		for (const imtbase::IComplexCollectionFilter::FieldInfo& info : collectionFilter.GetFields()){
			if (info.metaInfo.flags & imtbase::IComplexCollectionFilter::SO_TEXT_FILTER){
				if (!textFilterQuery.isEmpty()){
					textFilterQuery += QStringLiteral(" OR ");
				}

				if (info.metaInfo.type == imtbase::IComplexCollectionFilter::FT_SCALAR){
					if (s_filterableColumns.contains(info.id)){
						textFilterQuery += QStringLiteral(R"(root."%1" ILIKE '%%2%')").arg(QString::fromUtf8(info.id), textFilter);
					}
					else{
						textFilterQuery += QStringLiteral(R"(%1 ILIKE '%%2%')")
									.arg(
										CreateJsonExtractSql(jsonKey, info.id, QMetaType::QString, "root"),
										textFilter
										);
					}
				}
				else if (info.metaInfo.type == imtbase::IComplexCollectionFilter::FT_ARRAY){
					QString expr = QString(
						"EXISTS ("
						"SELECT 1 "
						"FROM jsonb_array_elements_text(%1) AS elem "
						"WHERE %2"
						")"
					).arg(
							CreateJsonExtractSql(jsonKey, info.id, QMetaType::QJsonArray, "root"),
							QString("elem ILIKE '%%%1%%'").arg(textFilter)
						);

					textFilterQuery += expr;
				}
				else{
					qWarning() << "Unable to create text filter: unsupported field type:" << info.id << info.metaInfo.type;
					return false;
				}
			}
		}
	}

	return true;
}


void CSqlDatabaseDocumentDelegateComp::SubstituteFieldIds(QString& query, bool /*castToStr*/) const
{
	Q_ASSERT(!query.isEmpty());

	const QRegularExpression regexp(
		R"re((?=(?:[^']*'[^']*')*[^']*$)(?:(?<=\s)|(?<=\()|^)"([^"]+)"(?=(?:::[A-Za-z_][A-Za-z0-9_]*)?[\s=)\,]|$))re"
	);

	QStringList list;

#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
	QRegularExpressionMatchIterator iterator = regexp.globalMatch(query);
	while (iterator.hasNext()){
		const QRegularExpressionMatch& match = iterator.next();
		
		QStringList capturedList = match.capturedTexts();
		list.append(capturedList.first());
	}
#else
	auto results = regexp.globalMatch(query);
	for (const QRegularExpressionMatch& match : results){
		QStringList capturedList = match.capturedTexts();
		list.append(capturedList.first());
	}
#endif
	list.removeDuplicates();

	bool useDataMeta = m_useDataMetaInfoAttrPtr.IsValid() ? *m_useDataMetaInfoAttrPtr : false;
	QString jsonKey = useDataMeta ? s_dataMetaInfoColumn : s_documentColumn;

	for (const QString& item : list){
		QString substitute = item;
		substitute.replace("\"", "");

		if (!s_filterableColumns.contains(substitute)){
			if (IsArrayOperation(query, substitute)){
				substitute = CreateJsonExtractSql(jsonKey, substitute, QMetaType::QJsonArray, "root");
			}
			else{
				substitute = CreateJsonExtractSql(jsonKey, substitute, QMetaType::QString, "root");
			}
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
	QString documentFilterQuery;
	if (paramsPtr != nullptr){
		const iprm::IParamsSet::Ids paramIds = paramsPtr->GetParamIds();
		if (paramIds.contains(QByteArrayLiteral("DocumentFilter"))){
			iprm::TParamsPtr<imtcol::IDocumentCollectionFilter> documentFilterParamPtr(paramsPtr, QByteArrayLiteral("DocumentFilter"));
			if (documentFilterParamPtr.IsValid()){
				CreateDocumentCollectionFilterQuery(*documentFilterParamPtr.GetPtr(), documentFilterQuery);
			}
		}
	}

	if (documentFilterQuery.isEmpty()){
		documentFilterQuery = QStringLiteral(R"(root."%1" = 'Active')").arg(QString::fromUtf8(s_stateColumn));
	}

	QString schemaPrefix;
	if (m_tableSchemaAttrPtr.IsValid()){
		schemaPrefix = QStringLiteral("%1.").arg(qPrintable(*m_tableSchemaAttrPtr));
	}

	return QStringLiteral(R"(
			SELECT root.*, root1."TimeStamp" as "Added"
			FROM %1 "%2" as root
			LEFT JOIN (
				SELECT DISTINCT ON ("%4")
					"%4", "%5"
				FROM %1 "%2"
				WHERE %7 = 1
				ORDER BY "%4", "%5" DESC
			) AS root1 ON root1."%4" = root."%4"
			WHERE (%3) AND root."%4" = '%6' ORDER BY %8 DESC;)")
			.arg(
				/*1*/ schemaPrefix,
				/*2*/ QString::fromUtf8(*m_tableNameAttrPtr),
				/*3*/ documentFilterQuery,
				/*4*/ QString::fromUtf8(s_documentIdColumn),
				/*5*/ QString::fromUtf8(s_lastModifiedColumn),
				/*6*/ QString::fromUtf8(objectId),
				/*7*/ CreateJsonExtractSql(s_revisionInfoColumn, s_revisionNumberKey, QMetaType::Int),
				/*8*/ CreateJsonExtractSql(s_revisionInfoColumn, s_revisionNumberKey, QMetaType::Int, QStringLiteral("root"))
			).toUtf8();
}


QByteArray CSqlDatabaseDocumentDelegateComp::CreateJoinTablesQuery() const
{
	return QByteArray();
}


QByteArray CSqlDatabaseDocumentDelegateComp::GetCustomColumnsQuery() const
{
	return QByteArray();
}


bool CSqlDatabaseDocumentDelegateComp::CreateDocumentCollectionFilterQuery(
			const imtcol::IDocumentCollectionFilter& documentCollectionFilter,
			QString& documentFilterQuery) const
{
	QStringList conditions;

	// DocumentId
	QByteArray documentId = documentCollectionFilter.GetDocumentId();
	if (!documentId.isEmpty()){
		conditions << QString("root.\"%1\" = '%2'").arg(QString::fromUtf8(s_documentIdColumn), QString::fromUtf8(documentId));
	}

	// Document States
	QStringList stateConditions;
	imtcol::IDocumentCollectionFilter::DocumentStates states = documentCollectionFilter.GetDocumentStates();

	if (states.contains(imtcol::IDocumentCollectionFilter::DS_ACTIVE)){
		stateConditions << QString("root.\"%1\" = 'Active'").arg(QString::fromUtf8(s_stateColumn));
	}

	if (states.contains(imtcol::IDocumentCollectionFilter::DS_INACTIVE)){
		stateConditions << QString("root.\"%1\" = 'InActive'").arg(QString::fromUtf8(s_stateColumn));
	}

	if (states.contains(imtcol::IDocumentCollectionFilter::DS_DISABLED)){
		stateConditions << QString("root.\"%1\" = 'Disabled'").arg(QString::fromUtf8(s_stateColumn));
	}

	if (!stateConditions.isEmpty()){
		conditions << "(" + stateConditions.join(" OR ") + ")";
	}

	if (!conditions.isEmpty()){
		documentFilterQuery = conditions.join(" AND ");
		return true;
	}

	documentFilterQuery.clear();
	return false;
}


bool CSqlDatabaseDocumentDelegateComp::CreateDocumentIdFilterQuery(
			const imtcol::IDocumentIdFilter& documentIdFilter,
			QString& documentIdFilterQuery) const
{
	const QByteArrayList documentIds = documentIdFilter.GetDocumentIds();
	if (documentIds.isEmpty()){
		if (documentIdFilter.GetConditionType() == imtcol::IDocumentIdFilter::ConditionType::CT_IN){
			documentIdFilterQuery = QStringLiteral("1 = 0");
		}
		else{
			documentIdFilterQuery = QStringLiteral("1 = 1");
		}

		return true;
	}

	QStringList idStrings;
	idStrings.reserve(documentIds.size());

	for (const QByteArray& id : documentIds){
		idStrings << QString("'%1'").arg(QString::fromUtf8(id));
	}

	imtcol::IDocumentIdFilter::ConditionType conditionType = documentIdFilter.GetConditionType();
	QString conditionStr;
	switch (conditionType){
	case imtcol::IDocumentIdFilter::CT_IN:
		conditionStr = QStringLiteral("IN");
		break;
	case imtcol::IDocumentIdFilter::CT_NOT_IN:
		conditionStr = QStringLiteral("NOT IN");
		break;
	}
	
	documentIdFilterQuery = QString(R"(root."DocumentId" %0 (%1))").arg(conditionStr, idStrings.join(", "));

	return true;
}


bool CSqlDatabaseDocumentDelegateComp::IsArrayOperation(
			const QString& query,
			const QString& field) const
{
	const QString escapedField = QRegularExpression::escape(field);

	static const QString operatorsPattern =
			R"("%1"\s*(\?\&|\?\||\?|@>|<@))";
	QRegularExpression reOperators(operatorsPattern.arg(escapedField));
	if (reOperators.match(query).hasMatch()) {
		return true;
	}

	static const QString functionsPattern =
				R"(jsonb_array_elements(_text)?\s*\(\s*"%1"\s*\))"
				R"(|jsonb_array_length\s*\(\s*"%1"\s*\))";

	QRegularExpression reFunctions(
				functionsPattern.arg(escapedField),
				QRegularExpression::CaseInsensitiveOption
				);

	return reFunctions.match(query).hasMatch();
}


} // namespace imtdb


