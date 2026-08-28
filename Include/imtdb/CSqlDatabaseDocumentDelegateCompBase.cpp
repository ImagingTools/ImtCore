// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtdb/CSqlDatabaseDocumentDelegateCompBase.h>


// Qt includes
#include <QtCore/QDateTime>
#include <QtCore/QFile>
#include <QtCore/QRegularExpression>
#include <QtCore/QRegularExpressionMatch>
#include <QtCore/QUuid>

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
#include <imtbase/imtbase.h>
#include <imtbase/ICollectionFilter.h>
#include <imtdb/CComplexCollectionFilterConverter.h>
#include <imtdb/imtdb.h>
#include <imtcol/IObjectTypeIdFilter.h>
#include <imtbase/CComplexCollectionFilter.h>
#include <imtauth/ITenantFilterParam.h>


namespace imtdb
{

const QByteArray CSqlDatabaseDocumentDelegateCompBase::s_idColumn				 = QByteArrayLiteral("Id");
const QByteArray CSqlDatabaseDocumentDelegateCompBase::s_typeIdColumn			 = QByteArrayLiteral("TypeId");
const QByteArray CSqlDatabaseDocumentDelegateCompBase::s_documentIdColumn		 = QByteArrayLiteral("DocumentId");
const QByteArray CSqlDatabaseDocumentDelegateCompBase::s_nameColumn				 = QByteArrayLiteral("Name");
const QByteArray CSqlDatabaseDocumentDelegateCompBase::s_descriptionColumn		 = QByteArrayLiteral("Description");
const QByteArray CSqlDatabaseDocumentDelegateCompBase::s_documentColumn			 = QByteArrayLiteral("Document");
const QByteArray CSqlDatabaseDocumentDelegateCompBase::s_dataMetaInfoColumn		 = QByteArrayLiteral("DataMetaInfo");
const QByteArray CSqlDatabaseDocumentDelegateCompBase::s_addedColumn			 = QByteArrayLiteral("Added");
const QByteArray CSqlDatabaseDocumentDelegateCompBase::s_lastModifiedColumn		 = QByteArrayLiteral("TimeStamp");
const QByteArray CSqlDatabaseDocumentDelegateCompBase::s_stateColumn			 = QByteArrayLiteral("State");
const QByteArray CSqlDatabaseDocumentDelegateCompBase::s_revisionInfoColumn		 = QByteArrayLiteral("RevisionInfo");

const QByteArray CSqlDatabaseDocumentDelegateCompBase::s_ownerIdKey				 = QByteArrayLiteral("OwnerId");
const QByteArray CSqlDatabaseDocumentDelegateCompBase::s_ownerNameKey			 = QByteArrayLiteral("OwnerName");
const QByteArray CSqlDatabaseDocumentDelegateCompBase::s_operationDescriptionKey = QByteArrayLiteral("OperationDescription");
const QByteArray CSqlDatabaseDocumentDelegateCompBase::s_revisionNumberKey		 = QByteArrayLiteral("RevisionNumber");
const QByteArray CSqlDatabaseDocumentDelegateCompBase::s_checksumKey			 = QByteArrayLiteral("Checksum");

QSet<QString> CSqlDatabaseDocumentDelegateCompBase::s_filterableColumns ={
	s_typeIdColumn,
	s_documentIdColumn,
	s_nameColumn,
	s_descriptionColumn,
	s_stateColumn,
	s_addedColumn,
	s_lastModifiedColumn
};


// public methods

// reimplemented (icomp::CComponentBase)

void CSqlDatabaseDocumentDelegateCompBase::OnComponentCreated()
{
	BaseClass::OnComponentCreated();
	bool useTenantBindings = m_useTenantEntityBindingsAttrPtr.IsValid() ? *m_useTenantEntityBindingsAttrPtr : true;
	if (useTenantBindings){
		EnsureTenantBindingTableExists();
	}
}


// reimplemented (imtdb::ISqlDatabaseObjectDelegate)

QByteArray CSqlDatabaseDocumentDelegateCompBase::GetCountQuery(const iprm::IParamsSet* paramsPtr) const
{
	QString baseQuery = GetSelectionQuery(QByteArray(), 0, -1, paramsPtr);

	return QStringLiteral("SELECT COUNT(*) FROM (%1) as t").arg(baseQuery).toUtf8();
}


QByteArray CSqlDatabaseDocumentDelegateCompBase::GetObjectIdFromRecord(const QSqlRecord& record) const
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


QByteArray CSqlDatabaseDocumentDelegateCompBase::GetSelectionQuery(
			const QByteArray& objectId,
			int offset,
			int count,
			const iprm::IParamsSet* paramsPtr) const
{
	if (!objectId.isEmpty()){
		return GetObjectSelectionQuery(objectId, paramsPtr);
	}

	if (IsSqliteDriver()){
		QString sortQuery;
		QString filterQuery;
		if (count == 0){
			return QByteArray();
		}
		if (paramsPtr != nullptr){
			if (!CreateFilterQuery(*paramsPtr, filterQuery)){
				return QByteArray();
			}

			iprm::TParamsPtr<imtbase::IComplexCollectionFilter> complexFilterParamPtr(paramsPtr, QByteArrayLiteral("ComplexFilter"));
			if (complexFilterParamPtr.IsValid()){
				if (!CreateSortQuery(*complexFilterParamPtr, sortQuery)){
					return QByteArray();
				}
			}
		}

		QByteArray paginationQuery;
		if (!CreatePaginationQuery(offset, count, paginationQuery)){
			return QByteArray();
		}

		QString baseSelectionQuery = GetBaseSelectionQuery();

		// Due to a bug in qt in the context of resolving of an expression like this: '%<SOME_NUMBER>%'
		QString retVal = baseSelectionQuery;
		retVal += ' ';
		retVal += filterQuery;
		retVal += ' ';
		retVal += sortQuery;
		retVal += ' ';
		retVal += qPrintable(paginationQuery);

		return retVal.toUtf8();
	}

	QByteArray selectionQuery = BaseClass::GetSelectionQuery(objectId, offset, count, paramsPtr);

	if (paramsPtr != nullptr){
		const iprm::IParamsSet::Ids paramIds = paramsPtr->GetParamIds();
		if (paramIds.contains(QByteArrayLiteral("ComplexFilter"))){
			iprm::TParamsPtr<imtbase::CComplexCollectionFilter> complexFilterParamPtr(paramsPtr, QByteArrayLiteral("ComplexFilter"));
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


istd::IChangeableUniquePtr CSqlDatabaseDocumentDelegateCompBase::CreateObjectFromRecord(
			const QSqlRecord& record,
			const iprm::IParamsSet* /*paramsPtr*/) const
{
	if (!m_databaseEngineCompPtr.IsValid()){
		Q_ASSERT_X(false, "CSqlDatabaseDocumentDelegateCompBase::CreateObjectFromRecord", "No database engine was registered");

		return nullptr;
	}

	if (!m_documentFactoriesCompPtr.IsValid()){
		Q_ASSERT_X(false, "CSqlDatabaseDocumentDelegateCompBase::CreateObjectFromRecord", "No document factories were registered");

		return nullptr;
	}

	QByteArray typeId = GetObjectTypeIdFromRecord(record);

	istd::IChangeableUniquePtr documentPtr = CreateObject(typeId);
	if (!documentPtr.IsValid()){
		Q_ASSERT_X(false, "CSqlDatabaseDocumentDelegateCompBase::CreateObjectFromRecord", qPrintable(QStringLiteral("Document instance could not be created for the type: '%1'").arg(typeId)));

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


imtdb::IDatabaseObjectDelegate::NewObjectQuery CSqlDatabaseDocumentDelegateCompBase::CreateNewObjectQuery(
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


QByteArray CSqlDatabaseDocumentDelegateCompBase::CreateDeleteObjectsQuery(
			const imtbase::IObjectCollection& /*collection*/,
			const imtbase::ICollectionInfo::Ids& objectIds,
			const imtbase::IOperationContext* /*operationContextPtr*/) const
{
	if (objectIds.isEmpty()){
		return QByteArray();
	}

	QString schemaPrefix;
	if (m_tableSchemaAttrPtr.IsValid()){
		schemaPrefix = QStringLiteral("%1.").arg(*m_tableSchemaAttrPtr);
	}

	const QString quotedIds = objectIds.toList().join(u8"','").append('\'').prepend('\'');
	QString query = 
		QStringLiteral(R"(UPDATE %1 "%2" SET "%3" = 'Disabled' WHERE "%3" = 'Active' AND "%4" IN (%5);)")
					.arg(
								/*1*/ schemaPrefix,
								/*2*/ *m_tableNameAttrPtr,
								/*3*/ s_stateColumn,
								/*4*/ s_documentIdColumn,
								/*5*/ quotedIds);

	QByteArray bindingDeleteQuery = CreateTenantBindingDeleteQuery(objectIds);
	if (!bindingDeleteQuery.isEmpty()){
		query += QString::fromUtf8(bindingDeleteQuery);
	}

	return query.toUtf8();
}


QByteArray CSqlDatabaseDocumentDelegateCompBase::CreateDeleteObjectSetQuery(
			const imtbase::IObjectCollection& /*collection*/,
			const iprm::IParamsSet* paramsPtr,
			const imtbase::IOperationContext* /*operationContextPtr*/) const
{
	QString schemaPrefix;
	if (m_tableSchemaAttrPtr.IsValid()){
		schemaPrefix = QStringLiteral("%1.").arg(*m_tableSchemaAttrPtr);
	}
	QByteArray retVal = QStringLiteral(R"(UPDATE %1 "%2" as root SET "%3" = 'Disabled' WHERE "%3" = 'Active')").arg(schemaPrefix, *m_tableNameAttrPtr, s_stateColumn).toUtf8();

	QByteArrayList conditionList;
	if (paramsPtr != nullptr){
		iprm::TParamsPtr<imtbase::IComplexCollectionFilter> complexFilterParamPtr(paramsPtr, QByteArrayLiteral("ComplexFilter"));
		if (complexFilterParamPtr.IsValid()){
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

			retVal += QByteArrayLiteral(" AND (") += conditionList.join(QByteArrayLiteral(" AND ")) + ')';
		}
	}

	bool useTenantBindings = m_useTenantEntityBindingsAttrPtr.IsValid() ? *m_useTenantEntityBindingsAttrPtr : true;
	if (useTenantBindings){
		const QString bindingsTableName = CreateTenantBindingTableName();
		const QString escapedEntityType = SqlEncode(QString::fromUtf8(GetTableName()));
		QString bindingsDelete = QStringLiteral(R"(DELETE FROM %1 WHERE "EntityType" = '%2';)")
				.arg(bindingsTableName, escapedEntityType);
		if (!conditionList.isEmpty()){
			const QString tableName = QString::fromUtf8(*m_tableNameAttrPtr);
			const QString idCol = QString::fromUtf8(s_documentIdColumn);
			const QString stateCol = QString::fromUtf8(s_stateColumn);
			QString idSelect = QStringLiteral(R"(SELECT "%1" FROM %2"%3" as root WHERE "%4" = 'Active' AND (%5))")
					.arg(
						/*1*/ idCol,
						/*2*/ schemaPrefix,
						/*3*/ tableName,
						/*4*/ stateCol,
						/*5*/ QString::fromUtf8(conditionList.join(QByteArrayLiteral(" AND "))));
			bindingsDelete = QStringLiteral(R"(DELETE FROM %1 WHERE "EntityType" = '%2' AND "EntityId" IN (%3);)")
					.arg(bindingsTableName, escapedEntityType, idSelect);
		}
		retVal += ';' + bindingsDelete.toUtf8();
	}

	return retVal;
}


QByteArray CSqlDatabaseDocumentDelegateCompBase::CreateRestoreObjectsQuery(
			const imtbase::IObjectCollection& /*collection*/,
			const imtbase::ICollectionInfo::Ids& objectIds,
			const imtbase::IOperationContext* /*operationContextPtr*/) const
{
	if (objectIds.isEmpty()){
		return QByteArray();
	}

	const QString quotedIds = objectIds.toList().join(u8"','").append('\'').prepend('\'');

	QString schemaPrefix;
	if (m_tableSchemaAttrPtr.IsValid()){
		schemaPrefix = QStringLiteral("%1.").arg(*m_tableSchemaAttrPtr);
	}

	return QStringLiteral(R"(
				UPDATE %1 "%2"
				SET "%3" = 'Active'
				WHERE "%4" IN (%5)
				AND "State" = 'Disabled';)")
				.arg(
					/*1*/ schemaPrefix,
					/*2*/ *m_tableNameAttrPtr,
					/*3*/ s_stateColumn,
					/*4*/ s_documentIdColumn,
					/*5*/ quotedIds
			).toUtf8();
}


QByteArray CSqlDatabaseDocumentDelegateCompBase::CreateRestoreObjectSetQuery(
			const imtbase::IObjectCollection& /*collection*/,
			const iprm::IParamsSet* paramsPtr,
			const imtbase::IOperationContext* /*operationContextPtr*/) const
{
	QString schemaPrefix;
	if (m_tableSchemaAttrPtr.IsValid()){
		schemaPrefix = QStringLiteral("%1.").arg(*m_tableSchemaAttrPtr);
	}
	QByteArray retVal = 
				QStringLiteral(R"(UPDATE %1 "%2" as root SET "%3" = 'Active' WHERE "%3" = 'Disabled')")
							.arg(schemaPrefix, *m_tableNameAttrPtr, s_stateColumn).toUtf8();

	if (paramsPtr != nullptr){
		iprm::TParamsPtr<imtbase::IComplexCollectionFilter> complexFilterParamPtr(paramsPtr, QByteArrayLiteral("ComplexFilter"));
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

			retVal += QByteArrayLiteral(" AND (") += conditionList.join(QByteArrayLiteral(" AND ")) + ')';
		}
	}

	return retVal;
}


QByteArray CSqlDatabaseDocumentDelegateCompBase::CreateUpdateObjectQuery(
			const imtbase::IObjectCollection& collection,
			const QByteArray& objectId,
			const istd::IChangeable& object,
			const imtbase::IOperationContext* operationContextPtr,
			bool /*useExternDelegate*/) const
{
	// Get number of the revisions of the document in the database:
	QString schemaPrefix;
	if (m_tableSchemaAttrPtr.IsValid()){
		schemaPrefix = QStringLiteral("%1.").arg(*m_tableSchemaAttrPtr);
	}
	QByteArray countRevisionsQuery = 
				QStringLiteral(R"((SELECT MAX(%1) + 1 FROM %2 "%3" as root WHERE "%4" = '%5'))").arg(
							/*1*/ CreateJsonExtractSql(s_revisionInfoColumn, s_revisionNumberKey, QMetaType::Int),
							/*2*/ schemaPrefix,
							/*3*/ *m_tableNameAttrPtr,
							/*4*/ s_documentIdColumn,
							/*5*/ objectId
					).toUtf8();

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


QByteArray CSqlDatabaseDocumentDelegateCompBase::CreateRenameObjectQuery(
			const imtbase::IObjectCollection& /*collection*/,
			const QByteArray& objectId,
			const QString& newObjectName,
			const imtbase::IOperationContext* /*operationContextPtr*/) const
{
	QString schemaPrefix;
	if (m_tableSchemaAttrPtr.IsValid()){
		schemaPrefix = QStringLiteral("%1.").arg(*m_tableSchemaAttrPtr);
	}
	// Only the current revision is renamed, older revisions keep the name they were stored with.
	QByteArray retVal = QStringLiteral(R"(UPDATE %1 "%2" SET "%3" = '%4' WHERE "%5" = '%6' AND "%7" <> 'InActive';)")
						.arg(
							/*1*/ schemaPrefix,
							/*2*/ *m_tableNameAttrPtr,
							/*3*/ s_nameColumn,
							/*4*/ SqlEncode(newObjectName),
							/*5*/ s_documentIdColumn,
							/*6*/ objectId,
							/*7*/ s_stateColumn
						).toUtf8();

	return retVal;
}


QByteArray CSqlDatabaseDocumentDelegateCompBase::CreateDescriptionObjectQuery(
			const imtbase::IObjectCollection& /*collection*/,
			const QByteArray& objectId,
			const QString& description,
			const imtbase::IOperationContext* /*operationContextPtr*/) const
{
	QString schemaPrefix;
	if (m_tableSchemaAttrPtr.IsValid()){
		schemaPrefix = QStringLiteral("%1.").arg(*m_tableSchemaAttrPtr);
	}
	// Only the current revision is changed, older revisions keep the description they were stored with.
	QByteArray retVal = QStringLiteral(R"(UPDATE %1 "%2" SET "%3" = '%4' WHERE "%5" = '%6' AND "%7" <> 'InActive';)")
						.arg(
							/*1*/ schemaPrefix,
							/*2*/ *m_tableNameAttrPtr,
							/*3*/ s_descriptionColumn,
							/*4*/ SqlEncode(description),
							/*5*/ s_documentIdColumn,
							/*6*/ objectId,
							/*7*/ s_stateColumn
						).toUtf8();

	return retVal;
}


QByteArray CSqlDatabaseDocumentDelegateCompBase::GetSelectionByMetaInfoQuery(
			const QByteArray& metaInfoId,
			const QVariant& metaInfoValue) const
{
	QString schemaPrefix;
	if (m_tableSchemaAttrPtr.IsValid()){
		schemaPrefix = QStringLiteral("%1.").arg(*m_tableSchemaAttrPtr);
	}
	return QStringLiteral(R"(SELECT * FROM %1 "%2" WHERE ("%3" = 'Active') AND %4 = '%5';)")
			.arg(
				/*1*/ schemaPrefix,
				/*2*/ *m_tableNameAttrPtr,
				/*3*/ s_stateColumn,
				/*4*/ CreateJsonExtractSql(s_dataMetaInfoColumn, metaInfoId),
				/*5*/ metaInfoValue.toByteArray()
			).toUtf8();
}


QByteArray CSqlDatabaseDocumentDelegateCompBase::CreateUpdateMetaInfoQuery(const QSqlRecord& record) const
{
	istd::IChangeableUniquePtr objectPtr = CreateObjectFromRecord(record);
	if (!objectPtr.IsValid()){
		return QByteArray();
	}

	QByteArray objectId = imtdb::VariantToByteArray(record.value(qPrintable(s_documentIdColumn)));
	QByteArray typeId = imtdb::VariantToByteArray(record.value(qPrintable(s_typeIdColumn)));

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
		schemaPrefix = QStringLiteral("%1.").arg(*m_tableSchemaAttrPtr);
	}

	QByteArray query = QStringLiteral(R"(UPDATE %1 "%2" SET "%3" = '%4' WHERE "%5" = 'Active' AND "%6" = '%7';)")
						.arg(
							/*1*/ schemaPrefix,
							/*2*/ *m_tableNameAttrPtr,
							/*3*/ s_dataMetaInfoColumn,
							/*4*/ SqlEncode(metaInfoRepresentation),
							/*5*/ s_stateColumn,
							/*6*/ s_documentIdColumn,
							/*7*/ objectId
						).toUtf8();

	return query;
}


// reimplemented (imtbase::IRevisionController)

imtbase::IRevisionController::RevisionInfoList CSqlDatabaseDocumentDelegateCompBase::GetRevisionInfoList(
			const imtbase::IObjectCollection& /*collection*/,
			const QByteArray& objectId) const
{
	imtbase::IRevisionController::RevisionInfoList revisionInfoList;

	if (!m_databaseEngineCompPtr.IsValid()){
		return imtbase::IRevisionController::RevisionInfoList();
	}

	if (objectId.isEmpty()){
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


int CSqlDatabaseDocumentDelegateCompBase::BackupRevision(
			const imtbase::IObjectCollection& /*collection*/,
			const imtbase::ICollectionInfo::Id& /*objectId*/,
			const QString& /*userComment*/) const
{
	return -1;
}


bool CSqlDatabaseDocumentDelegateCompBase::RestoreRevision(
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
		schemaPrefix = QStringLiteral("%1.").arg(*m_tableSchemaAttrPtr);
	}

	QByteArray query = QStringLiteral(R"(UPDATE %1 "%2" SET "%3" = 'InActive' WHERE "%4" = '%5';)")
			.arg(
						/*1*/ schemaPrefix,
						/*2*/ *m_tableNameAttrPtr,
						/*3*/ s_stateColumn,
						/*4*/ s_documentIdColumn,
						/*5*/ objectId).toUtf8();

	query += QStringLiteral(R"(UPDATE %1 "%2" SET "%3" = 'Active' WHERE "%4" = '%5' AND %6 = %7;)")
				.arg(
							/*1*/ schemaPrefix,
							/*2*/ *m_tableNameAttrPtr,
							/*3*/ s_stateColumn,
							/*4*/ s_documentIdColumn,
							/*5*/ objectId,
							/*6*/ CreateJsonExtractSql(s_revisionInfoColumn, s_revisionNumberKey, QMetaType::Int))
			.arg(QString::number(revision)).toUtf8();

	QSqlError sqlError;
	m_databaseEngineCompPtr->ExecSqlQuery(query, &sqlError);
	if (sqlError.type() != QSqlError::NoError){
		SendErrorMessage(0, sqlError.text(), "Database collection");

		return false;
	}

	return true;
}


bool CSqlDatabaseDocumentDelegateCompBase::ExportRevision(
			const imtbase::IObjectCollection& /*collection*/,
			const imtbase::ICollectionInfo::Id& /*objectId*/,
			int /*revision*/,
			const QString& /*filePath*/) const
{
	return false;
}


bool CSqlDatabaseDocumentDelegateCompBase::DeleteRevision(
			imtbase::IObjectCollection& collection,
			const imtbase::ICollectionInfo::Id& objectId,
			int revision) const
{
	QString schemaPrefix;
	if (m_tableSchemaAttrPtr.IsValid()){
		schemaPrefix = QStringLiteral("%1.").arg(*m_tableSchemaAttrPtr);
	}
	QByteArray checkCurrentRevisionQuery = QStringLiteral(R"(SELECT * FROM %1 "%2" WHERE "%3" = '%4' AND "%5" = 'Active';)")
				.arg(
							/*1*/ schemaPrefix,
							/*2*/ *m_tableNameAttrPtr,
							/*3*/ s_documentIdColumn,
							/*4*/ objectId,
							/*5*/ s_stateColumn).toUtf8();

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
		SendErrorMessage(0, QStringLiteral("Unable to delete revision '%1' for document '%2'. Error: Revision '%1' is active").arg(QString::number(revision), objectId), "Database collection");
		return false;
	}

	QByteArray query = QStringLiteral(R"(DELETE  FROM %1 "%2" WHERE "%3" = '%4' AND %5 = %6;)")
						.arg(
							/*1*/ schemaPrefix,
							/*2*/ *m_tableNameAttrPtr,
							/*3*/ s_documentIdColumn,
							/*4*/ objectId,
							/*5*/ CreateJsonExtractSql(s_revisionInfoColumn, s_revisionNumberKey, QMetaType::Int)
							)
						.arg(QString::number(revision))
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

bool CSqlDatabaseDocumentDelegateCompBase::UpdateDependentMetaInfo(const DependentMetaInfo& metaInfo) const
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
		schemaPrefix = QStringLiteral("%1.").arg(*m_tableSchemaAttrPtr);
	}

	QString query = QStringLiteral(R"(UPDATE %1 "%2" SET "%3" = )").arg(schemaPrefix, tableName, s_dataMetaInfoColumn);

	QString jsonbUpdate = QStringLiteral(R"("%1")").arg(s_dataMetaInfoColumn);

	for (int i = 0; i < metaInfo.metaInfoIds.size(); i++){
		jsonbUpdate = QStringLiteral("jsonb_set(%1, '{%2}', to_jsonb(%3))").arg(jsonbUpdate, metaInfo.metaInfoIds[i], metaInfo.metaInfoValues[i]);
	}

	query += jsonbUpdate + QStringLiteral(" WHERE %1 = '%2';").arg(CreateJsonExtractSql(s_dataMetaInfoColumn, metaInfo.dependentKey), metaInfo.objectId);

	QSqlError sqlError;
	m_databaseEngineCompPtr->ExecSqlQuery(query.toUtf8(), &sqlError);
	if (sqlError.type() != QSqlError::NoError){
		SendErrorMessage(0, sqlError.text(), "CSqlDatabaseDocumentDelegateCompBase");
		qDebug() << "SQL-error" << sqlError.text();

		return false;
	}

	return true;
}

bool CSqlDatabaseDocumentDelegateCompBase::ClearDependentMetaInfo(const MetaFieldCleanupPlan& metaInfo) const
{
	if (!m_databaseEngineCompPtr.IsValid()){
		Q_ASSERT(false);
		return false;
	}

	if (metaInfo.objectIds.isEmpty()){
		return false;
	}

	QString tableName = GetTableName();

	QString jsonbExpr = QStringLiteral(R"("%1")").arg(s_dataMetaInfoColumn);

	for (const QString& metaInfoId : metaInfo.metaInfoIds){
		jsonbExpr = QStringLiteral("jsonb_set(%1, '{%2}', to_jsonb(''::text))")
			.arg(jsonbExpr, metaInfoId);
	}

	jsonbExpr = QStringLiteral("jsonb_set(%1, '{%2}', to_jsonb(''::text))")
		.arg(jsonbExpr, metaInfo.dependentKey);

	QStringList quotedIds;
	for (const QByteArray& id : metaInfo.objectIds) {
		QString escapedId = SqlEncode(QString(id));
		quotedIds << QStringLiteral("'%1'").arg(escapedId);
	}

	QString whereClause = QStringLiteral("%1 IN (%2)")
		.arg(CreateJsonExtractSql(s_dataMetaInfoColumn, metaInfo.dependentKey), quotedIds.join(", "));

	QString schemaPrefix;
	if (m_tableSchemaAttrPtr.IsValid()){
		schemaPrefix = QStringLiteral("%1.").arg(*m_tableSchemaAttrPtr);
	}

	QString fullQuery = QStringLiteral(R"(UPDATE %1 "%2" SET "%3" = %4 WHERE %5;)")
		.arg(
				/*1*/ schemaPrefix,
				/*2*/ tableName,
				/*3*/ s_dataMetaInfoColumn,
				/*4*/ jsonbExpr,
				/*5*/ whereClause);

	QSqlError sqlError;
	m_databaseEngineCompPtr->ExecSqlQuery(fullQuery.toUtf8(), &sqlError);

	if (sqlError.type() != QSqlError::NoError){
		SendErrorMessage(0, sqlError.text(), "CSqlDatabaseDocumentDelegateCompBase");
		qDebug() << "SQL-error:" << sqlError.text();
		return false;
	}

	return true;
}


// protected methods

QByteArray CSqlDatabaseDocumentDelegateCompBase::PrepareInsertNewObjectQuery(
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
		schemaPrefix = QStringLiteral("%1.").arg(*m_tableSchemaAttrPtr);
	}

	// Insert new entry into the document list table:
	QString query = QStringLiteral(R"(UPDATE %1 "%2" SET "%3" = 'InActive' WHERE "%4" = '%5';)")
					.arg(
						/*1*/ schemaPrefix,
						/*2*/ *m_tableNameAttrPtr,
						/*3*/ s_stateColumn,
						/*4*/ s_documentIdColumn,
						/*5*/ objectId
						);

	QByteArray metaInfoRepresentation;

	const bool useMetaData = m_useDataMetaInfoAttrPtr.IsValid() ? *m_useDataMetaInfoAttrPtr : false;
	if (useMetaData && m_metaInfoCreatorCompPtr.IsValid() && m_jsonBasedMetaInfoDelegateCompPtr.IsValid()){
		idoc::MetaInfoPtr metaInfoPtr;
		if (m_metaInfoCreatorCompPtr->CreateMetaInfo(&object, typeId, metaInfoPtr) && metaInfoPtr.IsValid()){
			if (!m_jsonBasedMetaInfoDelegateCompPtr->ToJsonRepresentation(*metaInfoPtr.GetPtr(), metaInfoRepresentation, typeId)){
				SendWarningMessage(0, QStringLiteral("Unable to create meta info representation for the object '%1' from the table '%2'").arg(objectId, GetTableName()));
			}
		}
	}

	QByteArray revisionInfoQuery;
	revisionInfoQuery = CreateRevisionInfoQuery(operationContextPtr, revisionArgument, checksum);

	static const QString nullDataLiteral = QStringLiteral("NULL");

	QByteArray tenantId;
	if (operationContextPtr != nullptr){
		tenantId = operationContextPtr->GetTenantId();
	}

	QString columnsClause = QStringLiteral(R"("%1", "%2", "%3", "%4", "%5", "%6", "%7", "%8", "%9", "%10")")
				.arg(
						/*1*/  s_idColumn,
						/*2*/  s_typeIdColumn,
						/*3*/  s_documentIdColumn,
						/*4*/  s_nameColumn,
						/*5*/  s_descriptionColumn,
						/*6*/  s_documentColumn,
						/*7*/  s_dataMetaInfoColumn,
						/*8*/  s_revisionInfoColumn,
						/*9*/  s_lastModifiedColumn,
						/*10*/ s_stateColumn
					);

	QString valuesClause = QStringLiteral("'%1', '%2', '%3', %4, %5, '%6', %7, %8, '%9', '%10'")
				.arg(
						/*1*/ QUuid::createUuid().toString(QUuid::WithoutBraces),
						/*2*/ typeId,
						/*3*/ objectId,
						/*4*/ objectName.isEmpty() ? nullDataLiteral : SqlEncode(objectName).append('\'').prepend('\''),
						/*5*/ objectDescription.isEmpty() ? nullDataLiteral : SqlEncode(objectDescription).append('\'').prepend('\''),
						/*6*/ SqlEncode(documentContentJson),
						/*7*/ metaInfoRepresentation.isEmpty() ? nullDataLiteral : SqlEncode(metaInfoRepresentation).append('\'').prepend('\''),
						/*8*/ revisionInfoQuery.isEmpty() ? nullDataLiteral : revisionInfoQuery,
						/*9*/ QDateTime::currentDateTimeUtc().toString(Qt::ISODateWithMs),
						/*10*/ QStringLiteral("Active")
					);

	query += QStringLiteral(R"(INSERT INTO %1 "%2"(%3) VALUES(%4);)")
				.arg(
					/*1*/ schemaPrefix,
					/*2*/ *m_tableNameAttrPtr,
					/*3*/ columnsClause,
					/*4*/ valuesClause
					);

	bool useTenantBindings = m_useTenantEntityBindingsAttrPtr.IsValid() ? *m_useTenantEntityBindingsAttrPtr : true;
	if (!tenantId.isEmpty() && useTenantBindings){
		query += CreateTenantBindingInsertQuery(tenantId, objectId, operationContextPtr);
	}

	retVal = query.toUtf8();

	return retVal;
}


istd::IChangeableUniquePtr CSqlDatabaseDocumentDelegateCompBase::CreateObject(const QByteArray& typeId) const
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


QByteArray CSqlDatabaseDocumentDelegateCompBase::CreateRevisionInfoQuery(
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
					qDebug() << QStringLiteral("Unable to serialize a change object collection");
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


QByteArray CSqlDatabaseDocumentDelegateCompBase::CreateJsonBuildObjectQuery(const QVariantMap& paramMap) const
{
	const bool isSqlite = IsSQLite();
	QString revisionInfo = isSqlite
				? QStringLiteral(R"(json_object()")
				: QStringLiteral(R"(jsonb_build_object()");

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
			revisionInfo += QStringLiteral("'%1', '%2'").arg(key, SqlEncode(value.toString()));
		}
		else if (typeId == QMetaType::Int){
			revisionInfo += QStringLiteral("'%1', %2").arg(key, QString::number(value.toInt()));
		}
		else if (typeId == QMetaType::Bool){
			revisionInfo += QStringLiteral("'%1', %2").arg(key, QString::number(value.toBool()));
		}
	}

	revisionInfo += ')';

	return revisionInfo.toUtf8();
}


QString CSqlDatabaseDocumentDelegateCompBase::CreateJsonExtractSql(
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


QString CSqlDatabaseDocumentDelegateCompBase::CreateTenantBindingTableName() const
{
	if (m_tableSchemaAttrPtr.IsValid() && !(*m_tableSchemaAttrPtr).isEmpty()){
		return QStringLiteral(R"(%1."TenantEntityBindings")").arg(*m_tableSchemaAttrPtr);
	}

	return QStringLiteral(R"("TenantEntityBindings")");
}


QByteArray CSqlDatabaseDocumentDelegateCompBase::CreateTenantBindingTableInitializationQuery() const
{
	if (!m_databaseEngineCompPtr.IsValid()){
		return QByteArray();
	}

	const QString scriptPath = GetSqlResourcePath(*m_databaseEngineCompPtr, QStringLiteral("CreateTenantEntityBindingsTable.sql"));
	QFile scriptFile(scriptPath);
	if (!scriptFile.open(QFile::ReadOnly)){
		qWarning() << __FILE__ << __LINE__
				   << "\n\t| CreateTenantEntityBindingsTable.sql could not be loaded from" << scriptPath;
		return QByteArray();
	}

	QByteArray createTableQuery = scriptFile.readAll();
	scriptFile.close();

	const QByteArray tableScheme = GetTableScheme();
	if (!tableScheme.isEmpty()){
		createTableQuery.replace("${TableScheme}", tableScheme);
	}
	else{
		createTableQuery.replace("${TableScheme}", "public");
	}

	return createTableQuery;
}


void CSqlDatabaseDocumentDelegateCompBase::EnsureTenantBindingTableExists() const
{
	if (m_tenantBindingTableInitialized){
		return;
	}

	if (!m_databaseEngineCompPtr.IsValid()){
		return;
	}

	QByteArray initQuery = CreateTenantBindingTableInitializationQuery();
	if (initQuery.isEmpty()){
		return;
	}

	QSqlError sqlError;
	m_databaseEngineCompPtr->ExecSqlQuery(initQuery, &sqlError);

	if (sqlError.type() != QSqlError::NoError){
		qWarning() << __FILE__ << __LINE__
					<< "\n\t| TenantEntityBindings table could not be initialized"
					<< "\n\t| Error: " << sqlError;
		return;
	}

	m_tenantBindingTableInitialized = true;
}


QString CSqlDatabaseDocumentDelegateCompBase::CreateTenantBindingFilterQuery(const QByteArray& tenantId, imtauth::TenantFilterMode filterMode) const
{
	bool useTenantBindings = m_useTenantEntityBindingsAttrPtr.IsValid() ? *m_useTenantEntityBindingsAttrPtr : true;
	if (!useTenantBindings){
		// Delegate is configured not to use TenantEntityBindings for ownership (e.g. Users).
		// No binding-based tenant scoping filter is applied for such collections (global view).
		return QString();
	}

	const QString bindingsTableName = CreateTenantBindingTableName();
	const QString escapedEntityType = SqlEncode(QString::fromUtf8(GetTableName()));
	const QByteArray databaseDriverId = m_databaseEngineCompPtr.IsValid() ? m_databaseEngineCompPtr->GetDatabaseDriverId() : QByteArray();
	const bool isSqlite = databaseDriverId.compare(QByteArrayLiteral("QSQLITE"), Qt::CaseInsensitive) == 0;
	const QString entityIdExpression = isSqlite
			? QStringLiteral(R"(root."%1")").arg(s_documentIdColumn)
			: QStringLiteral(R"(root."%1"::text)").arg(s_documentIdColumn);

	QString bindingsLookup = QStringLiteral(R"(SELECT 1 FROM %1 tenantBindings WHERE tenantBindings."EntityType" = '%2' AND tenantBindings."EntityId" = %3)")
			.arg(
					bindingsTableName,
					escapedEntityType,
					entityIdExpression);

	if (!tenantId.isEmpty()){
		bindingsLookup += QStringLiteral(R"( AND tenantBindings."TenantId" = '%1')")
				.arg(SqlEncode(QString::fromUtf8(tenantId)));

		if (filterMode == imtauth::TFM_EXCLUDE){
			return QStringLiteral("NOT EXISTS (%1)").arg(bindingsLookup);
		}

		return QStringLiteral("EXISTS (%1)").arg(bindingsLookup);
	}

	// NoOrganization mode: empty TenantId binding and missing binding are equivalent.
	// Keep only records that have no non-empty tenant binding.
	QString nonEmptyTenantBindingsLookup = bindingsLookup
			+ QStringLiteral(R"( AND COALESCE(tenantBindings."TenantId", '') <> '')");
	return QStringLiteral("NOT EXISTS (%1)").arg(nonEmptyTenantBindingsLookup);
}


QByteArray CSqlDatabaseDocumentDelegateCompBase::CreateTenantBindingInsertQuery(
			const QByteArray& tenantId,
			const QByteArray& entityId,
			const imtbase::IOperationContext* operationContextPtr) const
{
	bool useTenantBindings = m_useTenantEntityBindingsAttrPtr.IsValid() ? *m_useTenantEntityBindingsAttrPtr : true;
	if (!useTenantBindings || tenantId.isEmpty() || entityId.isEmpty()){
		return QByteArray();
	}

	QByteArray ownerId;
	if (operationContextPtr != nullptr){
		ownerId = operationContextPtr->GetOperationOwnerId().id;
	}

	const QByteArray databaseDriverId = m_databaseEngineCompPtr.IsValid() ? m_databaseEngineCompPtr->GetDatabaseDriverId() : QByteArray();
	const bool isSqlite = databaseDriverId.compare(QByteArrayLiteral("QSQLITE"), Qt::CaseInsensitive) == 0;
	const QString conflictClause = isSqlite
			? QStringLiteral("INSERT OR IGNORE INTO")
			: QStringLiteral("INSERT INTO");
	const QString onConflictClause = isSqlite
			? QString()
			: QStringLiteral(R"( ON CONFLICT ("TenantId", "EntityType", "EntityId") DO NOTHING)");

	const QString query = QStringLiteral(R"(%1 %2 ("Id", "TenantId", "EntityType", "EntityId", "CreatedAt", "CreatedByUserId") VALUES ('%3', '%4', '%5', '%6', '%7', %8)%9;)")
								.arg(
									/*1*/ conflictClause,
									/*2*/ CreateTenantBindingTableName(),
									/*3*/ QUuid::createUuid().toString(QUuid::WithoutBraces),
									/*4*/ SqlEncode(QString::fromUtf8(tenantId)),
									/*5*/ SqlEncode(QString::fromUtf8(GetTableName())),
									/*6*/ SqlEncode(QString::fromUtf8(entityId)),
									/*7*/ QDateTime::currentDateTimeUtc().toString(Qt::ISODateWithMs),
									/*8*/ ownerId.isEmpty() ? QStringLiteral("NULL") : QStringLiteral("'%1'").arg(SqlEncode(QString::fromUtf8(ownerId))),
									/*9*/ onConflictClause
								);

	EnsureTenantBindingTableExists();

	return query.toUtf8();
}


QByteArray CSqlDatabaseDocumentDelegateCompBase::CreateTenantBindingDeleteQuery(const QByteArrayList& entityIds) const
{
	bool useTenantBindings = m_useTenantEntityBindingsAttrPtr.IsValid() ? *m_useTenantEntityBindingsAttrPtr : true;
	if (!useTenantBindings || entityIds.isEmpty()){
		return QByteArray();
	}

	QStringList quotedIds;
	for (const QByteArray& id : entityIds){
		quotedIds.append(QStringLiteral("'%1'").arg(SqlEncode(QString::fromUtf8(id))));
	}

	const QString query = QStringLiteral(R"(DELETE FROM %1 WHERE "EntityType" = '%2' AND "EntityId" IN (%3);)")
			.arg(
					CreateTenantBindingTableName(),
					SqlEncode(QString::fromUtf8(GetTableName())),
					quotedIds.join(','));

	return query.toUtf8();
}


// reimplemented (imtdb::CSqlDatabaseObjectDelegateCompBase)

QString CSqlDatabaseDocumentDelegateCompBase::GetBaseSelectionQuery() const
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

	if (IsSQLite()){
		QString query = QStringLiteral(R"(
							SELECT
								root.*,
								root1."TimeStamp" AS "Added" %4
							FROM %1"%2" AS root
							LEFT JOIN (
								SELECT root1."DocumentId", root1."TimeStamp"
								FROM %1"%2" AS root1
								WHERE root1."RevisionInfo" IS NOT NULL
								  AND CAST(json_extract(root1."RevisionInfo", '$.RevisionNumber') AS INTEGER) = 1
							) AS root1
							ON root1."DocumentId" = root."DocumentId"
							%3)")
						.arg(
							/*1*/ schema,
							/*2*/ tableName,
								/*3*/ joinTablesQuery.isEmpty() ? QString() : QString::fromUtf8(joinTablesQuery),
							/*4*/ customColumns
						);

		return query;
	}

	QString query = QStringLiteral(R"(
			SELECT
				root.*,
				root1."%6" as "%7" %5
			FROM %2"%3" as root
			-- No LIMIT on the derived table: revision 1 is already one row per document, and
			-- limiting it to a single row left every other document with a NULL "Added".
			LEFT JOIN (
				SELECT
					"%8", "%6", "%9"
				FROM %2"%3"
				WHERE %1 = 1
			) AS root1 ON root1."%8" = root."%8"
			%4)")
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


idoc::MetaInfoPtr CSqlDatabaseDocumentDelegateCompBase::CreateObjectMetaInfo(const QByteArray& typeId) const
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


bool CSqlDatabaseDocumentDelegateCompBase::SetObjectMetaInfoFromRecord(
			const QSqlRecord& record,
			idoc::IDocumentMetaInfo& metaInfo) const
{
	if (!record.contains(s_dataMetaInfoColumn)){
		return true;
	}

	if (!m_jsonBasedMetaInfoDelegateCompPtr.IsValid()){
		return true;
	}

	const QByteArray metaInfoRepresentation = record.value(qPrintable(s_dataMetaInfoColumn)).toByteArray();
	if (metaInfoRepresentation.isEmpty()){
		return true;
	}

	const QByteArray typeId = imtdb::VariantToByteArray(record.value(qPrintable(s_typeIdColumn)));

	return m_jsonBasedMetaInfoDelegateCompPtr->FromJsonRepresentation(
				metaInfoRepresentation,
				metaInfo,
				typeId
				);
}


bool CSqlDatabaseDocumentDelegateCompBase::CreatePaginationQuery(int offset, int count, QByteArray& paginationQuery) const
{
	paginationQuery.clear();

	if (offset >= 0 && count > 0){
		if (IsSQLite()){
			paginationQuery = QStringLiteral("LIMIT %1 OFFSET %2").arg(QString::number(count), QString::number(offset)).toUtf8();
		}
		else{
			paginationQuery = QStringLiteral("OFFSET %1 ROWS FETCH NEXT %2 ROWS ONLY").arg(QString::number(offset), QString::number(count)).toUtf8();
		}
	}

	return true;
}


bool CSqlDatabaseDocumentDelegateCompBase::CreateObjectFilterQuery(const iprm::IParamsSet& filterParams, QString& filterQuery) const
{
	filterQuery.clear();

	iprm::IParamsSet::Ids paramIds = filterParams.GetParamIds();
	if (paramIds.contains("Filter")){
		iprm::TParamsPtr<imtbase::ICollectionFilter> collectionFilterParamPtr(&filterParams, "Filter");
		if (collectionFilterParamPtr.IsValid()){
			QByteArray typeId = collectionFilterParamPtr->GetObjectTypeId();
			if (!typeId.isEmpty()){
				filterQuery = QStringLiteral(R"("%1" = '%2')").arg(s_typeIdColumn, SqlEncode(QString::fromUtf8(typeId))).toUtf8();
			}
		}
	}

	return true;
}


bool CSqlDatabaseDocumentDelegateCompBase::CreateSortQuery(const imtbase::ICollectionFilter& collectionFilter, QString& sortQuery) const
{
	QByteArray columnId;
	QByteArray sortOrder;

	if (!collectionFilter.GetSortingInfoIds().isEmpty()){
		columnId = collectionFilter.GetSortingInfoIds().constFirst();
	}

	switch (collectionFilter.GetSortingOrder()){
	case imtbase::ICollectionFilter::SO_ASC:
		sortOrder = QByteArrayLiteral("ASC");
		break;
	case imtbase::ICollectionFilter::SO_DESC:
		sortOrder = QByteArrayLiteral("DESC");
		break;
	default:
		break;
	}

	if (!columnId.isEmpty() && !sortOrder.isEmpty()){
		if (columnId == s_lastModifiedColumn || columnId == s_addedColumn || columnId == s_nameColumn || columnId == s_descriptionColumn || columnId == s_typeIdColumn){
			sortQuery = QStringLiteral(R"(ORDER BY root."%1" %2)").arg(columnId, sortOrder);
		}
		else{
			sortQuery = QStringLiteral("ORDER BY %1 %2").arg(CreateJsonExtractSql(QStringLiteral(R"(root."%1")").arg(s_dataMetaInfoColumn), columnId), sortOrder);
		}
	}

	return true;
}


bool CSqlDatabaseDocumentDelegateCompBase::CreateSortQuery(const imtbase::IComplexCollectionFilter& collectionFilter, QString& sortQuery) const
{
	bool retVal = BaseClass::CreateSortQuery(collectionFilter, sortQuery);

	if (!sortQuery.isEmpty()) {
		SubstituteFieldIds(sortQuery, false);
	}

	return retVal;
}


bool CSqlDatabaseDocumentDelegateCompBase::CreateFilterQuery(const iprm::IParamsSet& filterParams, QString& filterQuery) const
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
				objectTypeIdQuery = QStringLiteral(R"("%1" = '%2')").arg(s_typeIdColumn, SqlEncode(QString::fromUtf8(objectTypeId))).toUtf8();
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
		documentFilterQuery = QStringLiteral(R"(root."%1" = 'Active')").arg(s_stateColumn);
	}

	QString tenantFilterQuery;
	if (paramIds.contains("TenantFilter")){
		iprm::TParamsPtr<imtauth::ITenantFilterParam> tenantFilterPtr(&filterParams, "TenantFilter");
		if (tenantFilterPtr.IsValid()){
			QByteArray tenantId = tenantFilterPtr->GetTenantId();
			imtauth::TenantFilterMode filterMode = tenantFilterPtr->GetFilterMode();
			bool useTenantBindings = m_useTenantEntityBindingsAttrPtr.IsValid() ? *m_useTenantEntityBindingsAttrPtr : true;
			if (useTenantBindings || !tenantId.isEmpty()){
				EnsureTenantBindingTableExists();
			}
			tenantFilterQuery = CreateTenantBindingFilterQuery(tenantId, filterMode);
		}
	}

	QString additionalFilters = CreateAdditionalFiltersQuery(filterParams);

	if (!tenantFilterQuery.isEmpty()){
		if (!filterQuery.isEmpty()){
			filterQuery += " AND ";
		}

		filterQuery += "(" + tenantFilterQuery + ")";
	}

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


QString CSqlDatabaseDocumentDelegateCompBase::CreateAdditionalFiltersQuery(const iprm::IParamsSet& filterParams) const
{
	return QString();
}


bool CSqlDatabaseDocumentDelegateCompBase::CreateTextFilterQuery(
		const imtbase::ICollectionFilter& collectionFilter,
		QString& textFilterQuery) const
{
	QByteArrayList filteringColumnIds = collectionFilter.GetFilteringInfoIds();
	if (filteringColumnIds.isEmpty()){
		return true;
	}

	const QString textFilter = collectionFilter.GetTextFilter();
	if (textFilter.isEmpty()){
		return true;
	}

	const QByteArray driverId = m_databaseEngineCompPtr->GetDatabaseDriverId().toUpper();
	const bool isSQLite = (driverId == "QSQLITE");

	QString escapedTextFilter = SqlEncode(textFilter);

	const QString likePattern = QStringLiteral("%%1%").arg(escapedTextFilter);

	auto appendTextCondition = [&](const QString& expression)
	{
		if (!textFilterQuery.isEmpty()){
			textFilterQuery += " OR ";
		}

		if (isSQLite){
			textFilterQuery += QStringLiteral("LOWER(%1) LIKE LOWER('%2')")
				.arg(expression, likePattern);
		}
		else{
			textFilterQuery += QStringLiteral("%1 ILIKE '%2'")
				.arg(expression, likePattern);
		}
	};

	if (filteringColumnIds.contains(s_typeIdColumn)){
		appendTextCondition(QStringLiteral(R"("%1")").arg(s_typeIdColumn));
		filteringColumnIds.removeOne(s_typeIdColumn);
	}

	if (filteringColumnIds.contains(s_nameColumn)){
		appendTextCondition(QStringLiteral(R"("%1")").arg(s_nameColumn));
		filteringColumnIds.removeOne(s_nameColumn);
	}

	if (filteringColumnIds.contains(s_descriptionColumn)){
		appendTextCondition(QStringLiteral(R"("%1")").arg(s_descriptionColumn));
		filteringColumnIds.removeOne(s_descriptionColumn);
	}

	for (int i = 0; i < filteringColumnIds.count(); ++i){
		appendTextCondition(
			CreateJsonExtractSql(
				QStringLiteral(R"(root."%1")").arg(s_dataMetaInfoColumn),
				filteringColumnIds[i]));
	}

	return true;
}


bool CSqlDatabaseDocumentDelegateCompBase::CreateTimeFilterQuery(const imtbase::ITimeFilterParam& timeFilter, QString& timeFilterQuery, const QString& timeFieldId) const
{
	if (IsSQLite()){
		QString addedStrQuery = QStringLiteral(R"((SELECT "TimeStamp" FROM "%1" as temp WHERE %2 = 1 AND root."DocumentId" = temp."DocumentId" LIMIT 1))")
		.arg(
			*m_tableNameAttrPtr,
			CreateJsonExtractSql(QStringLiteral("RevisionInfo"), QStringLiteral("RevisionNumber"), QMetaType::Int)
			);

		switch (timeFilter.GetTimeUnit()){
		case imtbase::ITimeFilterParam::TU_CUSTOM:
			break;
		case imtbase::ITimeFilterParam::TU_HOUR:
			switch (timeFilter.GetInterpretationMode()){
			case imtbase::ITimeFilterParam::IM_CURRENT:
				timeFilterQuery += QStringLiteral(R"((strftime('%Y-%m-%d %H', %1) = strftime('%Y-%m-%d %H', 'now')))").arg(addedStrQuery);
				break;
			case imtbase::ITimeFilterParam::IM_LAST:
				timeFilterQuery += QStringLiteral(R"((strftime('%Y-%m-%d %H', %1) = strftime('%Y-%m-%d %H', datetime('now', '-1 hour'))))").arg(addedStrQuery);
				break;
			case imtbase::ITimeFilterParam::IM_FOR:
				timeFilterQuery += QStringLiteral(R"((%1 >= datetime('now', '-1 hour') AND %1 <= datetime('now')))").arg(addedStrQuery);
				break;
			}
			break;
		case imtbase::ITimeFilterParam::TU_DAY:
			switch (timeFilter.GetInterpretationMode()){
			case imtbase::ITimeFilterParam::IM_CURRENT:
				timeFilterQuery += QStringLiteral(R"((date(%1) = date('now')))").arg(addedStrQuery);
				break;
			case imtbase::ITimeFilterParam::IM_LAST:
				timeFilterQuery += QStringLiteral(R"((date(%1) = date('now', '-1 day')))").arg(addedStrQuery);
				break;
			case imtbase::ITimeFilterParam::IM_FOR:
				timeFilterQuery += QStringLiteral(R"((%1 >= datetime('now', '-1 day') AND %1 <= datetime('now')))").arg(addedStrQuery);
				break;
			}
			break;
		case imtbase::ITimeFilterParam::TU_WEEK:
			switch (timeFilter.GetInterpretationMode()){
			case imtbase::ITimeFilterParam::IM_CURRENT:
				timeFilterQuery += QStringLiteral(R"(strftime('%Y-%W', %1) = strftime('%Y-%W', 'now'))").arg(addedStrQuery);
				break;
			case imtbase::ITimeFilterParam::IM_LAST:
				timeFilterQuery += QStringLiteral(R"(strftime('%Y-%W', %1) = strftime('%Y-%W', datetime('now', '-7 days')))").arg(addedStrQuery);
				break;
			case imtbase::ITimeFilterParam::IM_FOR:
				timeFilterQuery += QStringLiteral(R"((%1 >= datetime('now', '-7 days') AND %1 <= datetime('now')))").arg(addedStrQuery);
				break;
			}
			break;
		case imtbase::ITimeFilterParam::TU_MONTH:
			switch (timeFilter.GetInterpretationMode()){
			case imtbase::ITimeFilterParam::IM_CURRENT:
				timeFilterQuery += QStringLiteral(R"(strftime('%Y-%m', %1) = strftime('%Y-%m', 'now'))").arg(addedStrQuery);
				break;
			case imtbase::ITimeFilterParam::IM_LAST:
				timeFilterQuery += QStringLiteral(R"(strftime('%Y-%m', %1) = strftime('%Y-%m', datetime('now', '-1 month')))").arg(addedStrQuery);
				break;
			case imtbase::ITimeFilterParam::IM_FOR:
				timeFilterQuery += QStringLiteral(R"((%1 >= datetime('now', '-1 month') AND %1 <= datetime('now')))").arg(addedStrQuery);
				break;
			}
			break;
		case imtbase::ITimeFilterParam::TU_YEAR:
			switch (timeFilter.GetInterpretationMode()){
			case imtbase::ITimeFilterParam::IM_CURRENT:
				timeFilterQuery += QStringLiteral(R"(strftime('%Y', %1) = strftime('%Y', 'now'))").arg(addedStrQuery);
				break;
			case imtbase::ITimeFilterParam::IM_LAST:
				timeFilterQuery += QStringLiteral(R"(strftime('%Y', %1) = strftime('%Y', datetime('now', '-1 year')))").arg(addedStrQuery);
				break;
			case imtbase::ITimeFilterParam::IM_FOR:
				timeFilterQuery += QStringLiteral(R"((%1 >= datetime('now', '-1 year') AND %1 <= datetime('now')))").arg(addedStrQuery);
				break;
			}
			break;
		}

		if (timeFilterQuery.isEmpty()){
			imtbase::CTimeRange timeRange = timeFilter.GetTimeRange();
			if (!timeRange.IsNull()){
				timeFilterQuery += QStringLiteral(R"(date(%1) >= date('%2') AND date(%1) <= date('%3'))")
				.arg(addedStrQuery, timeRange.GetBeginTime().toString(Qt::ISODate), timeRange.GetEndTime().toString(Qt::ISODate));
			}
		}

		return true;
	}

	switch (timeFilter.GetTimeUnit()){
	case imtbase::ITimeFilterParam::TU_CUSTOM:
		break;
	case imtbase::ITimeFilterParam::TU_HOUR:
		switch(timeFilter.GetInterpretationMode()){
		case imtbase::ITimeFilterParam::IM_CURRENT:
			timeFilterQuery += QStringLiteral(R"((date_trunc('hour', %1) = date_trunc('hour', current_timestamp at time zone 'utc')))").arg(timeFieldId);
			break;
		case imtbase::ITimeFilterParam::IM_LAST:
			timeFilterQuery += QStringLiteral(R"((date_trunc('hour', %1) = date_trunc('hour', current_timestamp at time zone 'utc') - interval '1 hour'))").arg(timeFieldId);
			break;
		case imtbase::ITimeFilterParam::IM_FOR:
			timeFilterQuery += QStringLiteral(R"((%1 >= current_timestamp at time zone 'utc' - interval '1 hour' and %1 <= current_timestamp at time zone 'utc'))").arg(timeFieldId);
			break;
		}
		break;
	case imtbase::ITimeFilterParam::TU_DAY:
		switch(timeFilter.GetInterpretationMode()){
		case imtbase::ITimeFilterParam::IM_CURRENT:
			timeFilterQuery += QStringLiteral(R"((date_trunc('day', %1) = date_trunc('day', current_timestamp at time zone 'utc')))").arg(timeFieldId);
			break;
		case imtbase::ITimeFilterParam::IM_LAST:
			timeFilterQuery += QStringLiteral(R"((date_trunc('day', %1) = date_trunc('day', current_timestamp at time zone 'utc') - interval '1 day'))").arg(timeFieldId);
			break;
		case imtbase::ITimeFilterParam::IM_FOR:
			timeFilterQuery += QStringLiteral(R"((%1 >= current_timestamp at time zone 'utc' - interval '1 day' and %1 <= current_timestamp at time zone 'utc'))").arg(timeFieldId);
			break;
		}
		break;
	case imtbase::ITimeFilterParam::TU_WEEK:
		switch(timeFilter.GetInterpretationMode()){
		case imtbase::ITimeFilterParam::IM_CURRENT:
			timeFilterQuery += QStringLiteral(R"((date_trunc('week', %1) = date_trunc('week', current_timestamp at time zone 'utc')))").arg(timeFieldId);
			break;
		case imtbase::ITimeFilterParam::IM_LAST:
			timeFilterQuery += QStringLiteral(R"((date_trunc('week', %1) = date_trunc('week', current_timestamp at time zone 'utc') - interval '1 week'))").arg(timeFieldId);
			break;
		case imtbase::ITimeFilterParam::IM_FOR:
				timeFilterQuery += QStringLiteral(R"((date(%1) >= current_date - interval '6 days' AND date(%1) <= current_date))").arg(timeFieldId);
				break;
			}
		break;
	case imtbase::ITimeFilterParam::TU_MONTH:
		switch(timeFilter.GetInterpretationMode()){
		case imtbase::ITimeFilterParam::IM_CURRENT:
			timeFilterQuery += QStringLiteral(R"((date_trunc('month', %1) = date_trunc('month', current_timestamp at time zone 'utc')))").arg(timeFieldId);
			break;
		case imtbase::ITimeFilterParam::IM_LAST:
			timeFilterQuery += QStringLiteral(R"((date_trunc('month', %1) = date_trunc('month', current_timestamp at time zone 'utc') - interval '1 month'))").arg(timeFieldId);
			break;
		case imtbase::ITimeFilterParam::IM_FOR:
			timeFilterQuery += QStringLiteral(R"((%1 >= current_timestamp at time zone 'utc' - interval '1 month' and %1 <= current_timestamp at time zone 'utc'))").arg(timeFieldId);
			break;
		}
		break;
	case imtbase::ITimeFilterParam::TU_YEAR:
		switch(timeFilter.GetInterpretationMode()){
		case imtbase::ITimeFilterParam::IM_CURRENT:
			timeFilterQuery += QStringLiteral(R"((date_trunc('year', %1) = date_trunc('year', current_timestamp at time zone 'utc')))").arg(timeFieldId);
			break;
		case imtbase::ITimeFilterParam::IM_LAST:
			timeFilterQuery += QStringLiteral(R"((date_trunc('year', %1) = date_trunc('year', current_timestamp at time zone 'utc') - interval '1 year'))").arg(timeFieldId);
			break;
		case imtbase::ITimeFilterParam::IM_FOR:
			timeFilterQuery += QStringLiteral(R"((%1 >= current_timestamp at time zone 'utc' - interval '1 year' and %1 <= current_timestamp at time zone 'utc'))").arg(timeFieldId);
			break;
		}
		break;
	}

	if (timeFilterQuery.isEmpty()){
		imtbase::CTimeRange timeRange = timeFilter.GetTimeRange();
		if (!timeRange.IsNull()){
			timeFilterQuery += QStringLiteral(R"(date(%1) >= date('%2') AND date(%1) <= date('%3'))")
								.arg(timeFieldId, timeRange.GetBeginTime().toString(Qt::ISODateWithMs), timeRange.GetEndTime().toString(Qt::ISODateWithMs));
		}
	}

	return true;
}


// protected methods

bool CSqlDatabaseDocumentDelegateCompBase::CreateObjectFilterQuery(const imtbase::IComplexCollectionFilter& collectionFilter, QString& filterQuery) const
{
	if (IsSQLite()){
		filterQuery = CComplexCollectionFilterConverter::CreateSqlFilterQuery(collectionFilter);
		if (!filterQuery.isEmpty()){
			SubstituteFieldIds(filterQuery, false);
		}
	}
	else{
		filterQuery = CComplexCollectionFilterConverter::CreateSqlFilterQuery(collectionFilter, CComplexCollectionFilterConverter::SC_POSTGRES);
		if (!filterQuery.isEmpty()) {
			SubstituteFieldIds(filterQuery);
		}
	}

	return true;
}


bool CSqlDatabaseDocumentDelegateCompBase::CreateTextFilterQuery(
			const imtbase::IComplexCollectionFilter& collectionFilter,
			QString& textFilterQuery) const
{
	textFilterQuery.clear();

	const bool useDataMeta = m_useDataMetaInfoAttrPtr.IsValid() ? *m_useDataMetaInfoAttrPtr : false;
	const QString jsonKey = useDataMeta ? QString::fromUtf8(s_dataMetaInfoColumn) : QString::fromUtf8(s_documentColumn);

	const QByteArray driverId = m_databaseEngineCompPtr->GetDatabaseDriverId().toUpper();
	const bool isSQLite = (driverId == "QSQLITE");

	QString textFilter = collectionFilter.GetTextFilter();
	if (textFilter.isEmpty()){
		return true;
	}

	textFilter.replace('\'', QLatin1String("''"));
	const QString likePattern = QStringLiteral("%%%1%%").arg(textFilter);

	auto makeLikeCondition = [&](const QString& expression) -> QString
	{
		if (isSQLite){
			return QStringLiteral("LOWER(%1) LIKE LOWER('%2')")
				.arg(expression, likePattern);
		}

		return QStringLiteral("%1 ILIKE '%2'")
			.arg(expression, likePattern);
	};

	for (const imtbase::IComplexCollectionFilter::FieldInfo& info : collectionFilter.GetFields()){
		if (!(info.metaInfo.flags & imtbase::IComplexCollectionFilter::SO_TEXT_FILTER)){
			continue;
		}

		if (!textFilterQuery.isEmpty()){
			textFilterQuery += QStringLiteral(" OR ");
		}

		if (info.metaInfo.type == imtbase::IComplexCollectionFilter::FT_SCALAR){
			if (s_filterableColumns.contains(info.id)){
				textFilterQuery += makeLikeCondition(
					QStringLiteral(R"(root."%1")").arg(info.id));
			}
			else{
				textFilterQuery += makeLikeCondition(
					CreateJsonExtractSql(jsonKey, info.id, QMetaType::QString, "root"));
			}
		}
		else if (info.metaInfo.type == imtbase::IComplexCollectionFilter::FT_ARRAY){
			QString expr;
			if (isSQLite){
				expr = QStringLiteral(
					"EXISTS ("
					"SELECT 1 "
					"FROM json_each(%1) AS elem "
					"WHERE %2"
					")")
					.arg(
						CreateJsonExtractSql(jsonKey, info.id, QMetaType::QJsonArray, "root"),
						makeLikeCondition(QStringLiteral("elem.value"))
					);
			}
			else{
				expr = QStringLiteral(
					"EXISTS ("
					"SELECT 1 "
					"FROM jsonb_array_elements_text(%1) AS elem "
					"WHERE %2"
					")")
					.arg(
						CreateJsonExtractSql(jsonKey, info.id, QMetaType::QJsonArray, "root"),
						makeLikeCondition(QStringLiteral("elem"))
					);
			}

			textFilterQuery += expr;
		}
		else{
			qWarning() << "Unable to create text filter: unsupported field type:" << info.id << info.metaInfo.type;
			return false;
		}
	}

	return true;
}


void CSqlDatabaseDocumentDelegateCompBase::SubstituteFieldIds(QString& query, bool /*castToStr*/) const
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
			substitute = QStringLiteral("root.%1").arg(item);
			query.replace(item, substitute);
		}
	}
}


QByteArray CSqlDatabaseDocumentDelegateCompBase::GetObjectSelectionQuery(const QByteArray& objectId, const iprm::IParamsSet* paramsPtr) const
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
		documentFilterQuery = QStringLiteral(R"(root."%1" = 'Active')").arg(s_stateColumn);
	}

	QString schemaPrefix;
	if (m_tableSchemaAttrPtr.IsValid()){
		schemaPrefix = QStringLiteral("%1.").arg(*m_tableSchemaAttrPtr);
	}

	if (IsSQLite()){
		return QStringLiteral(R"(
		SELECT
			root.*,
			root1."TimeStamp" AS "Added"
		FROM %1 "%2" AS root
		LEFT JOIN %1 "%2" AS root1
			ON root1."%3" = root."%3"
		   AND %6 = 1
		WHERE (%2)
		  AND root."%3" = '%4'
		ORDER BY %5 DESC;
	)")
				.arg(
					/*1*/ schemaPrefix,
					/*2*/ QString::fromUtf8(*m_tableNameAttrPtr),
					/*3*/ documentFilterQuery,
					/*4*/ QString::fromUtf8(s_documentIdColumn),
					/*5*/ QString::fromUtf8(objectId),
					/*6*/ CreateJsonExtractSql(
						s_revisionInfoColumn,
						s_revisionNumberKey,
						QMetaType::Int,
						"root"),
					/*7*/ CreateJsonExtractSql(
						s_revisionInfoColumn,
						s_revisionNumberKey,
						QMetaType::Int,
						"root1"))
				.toUtf8();
	}

	return QStringLiteral(R"(
			SELECT root.*, root1."TimeStamp" as "Added"
			FROM %1 "%2" as root
			LEFT JOIN (
				SELECT
					"%4", "%5"
				FROM %1 "%2"
				WHERE %7 = 1
			) AS root1 ON root1."%4" = root."%4"
			WHERE (%3) AND root."%4" = '%6' ORDER BY %8 DESC)")
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


QByteArray CSqlDatabaseDocumentDelegateCompBase::CreateJoinTablesQuery() const
{
	return QByteArray();
}


QByteArray CSqlDatabaseDocumentDelegateCompBase::GetCustomColumnsQuery() const
{
	return QByteArray();
}


bool CSqlDatabaseDocumentDelegateCompBase::CreateDocumentCollectionFilterQuery(
			const imtcol::IDocumentCollectionFilter& documentCollectionFilter,
			QString& documentFilterQuery) const
{
	QStringList conditions;

	// DocumentId
	QByteArray documentId = documentCollectionFilter.GetDocumentId();
	if (!documentId.isEmpty()){
		conditions << QStringLiteral(R"(root."%1" = '%2')").arg(s_documentIdColumn, SqlEncode(QString::fromUtf8(documentId)));
	}

	// Document States
	QStringList stateConditions;
	imtcol::IDocumentCollectionFilter::DocumentStates states = documentCollectionFilter.GetDocumentStates();

	if (states.contains(imtcol::IDocumentCollectionFilter::DS_ACTIVE)){
		stateConditions << QStringLiteral(R"(root."%1" = 'Active')").arg(s_stateColumn);
	}

	if (states.contains(imtcol::IDocumentCollectionFilter::DS_INACTIVE)){
		stateConditions << QStringLiteral(R"(root."%1" = 'InActive')").arg(s_stateColumn);
	}

	if (states.contains(imtcol::IDocumentCollectionFilter::DS_DISABLED)){
		stateConditions << QStringLiteral(R"(root."%1" = 'Disabled')").arg(s_stateColumn);
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


bool CSqlDatabaseDocumentDelegateCompBase::CreateDocumentIdFilterQuery(
			const imtcol::IDocumentIdFilter& documentIdFilter,
			QString& documentIdFilterQuery) const
{
	imtcol::IDocumentIdFilter::ConditionType conditionType = documentIdFilter.GetConditionType();
	if (conditionType == imtcol::IDocumentIdFilter::CT_NONE){
		// No filter requested — caller skips adding the clause.
		return true;
	}

	const QByteArrayList documentIds = documentIdFilter.GetDocumentIds();
	if (documentIds.isEmpty()){
		return true;
	}

	QStringList idStrings;
	idStrings.reserve(documentIds.size());

	for (const QByteArray& id : documentIds){
		idStrings << QStringLiteral("'%1'").arg(SqlEncode(QString::fromUtf8(id)));
	}

	QString conditionStr;
	switch (conditionType){
	case imtcol::IDocumentIdFilter::CT_IN:
		conditionStr = QStringLiteral("IN");
		break;
	case imtcol::IDocumentIdFilter::CT_NOT_IN:
		conditionStr = QStringLiteral("NOT IN");
		break;
	default:
		break;
	}
	
	documentIdFilterQuery = QStringLiteral(R"(root."DocumentId" %1 (%2))").arg(conditionStr, idStrings.join(", "));

	return true;
}


bool CSqlDatabaseDocumentDelegateCompBase::IsArrayOperation(
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


bool CSqlDatabaseDocumentDelegateCompBase::IsSQLite() const
{
	if (!m_databaseEngineCompPtr.IsValid()){
		return false;
	}

	return m_databaseEngineCompPtr->GetDatabaseDriverId().compare(
				QByteArrayLiteral("QSQLITE"), Qt::CaseInsensitive) == 0;
}


} // namespace imtdb
