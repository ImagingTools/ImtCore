// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtdb/CSqlDatabaseObjectDelegateCompBase.h>


// Qt includes
#include <QtCore/QFile>
#include <QtSql/QSqlRecord>
#include <QtSql/QSqlError>

// ACF includes
#include <istd/TOptDelPtr.h>
#include <iprm/CParamsSet.h>
#include <imod/TModelWrap.h>
#include <iprm/TParamsPtr.h>
#include <iprm/ITextParam.h>
#include <idoc/CStandardDocumentMetaInfo.h>

// ImtCore includes
#include <imtbase/imtbase.h>
#include <imtdb/CComplexCollectionFilterConverter.h>
#include <imtdb/imtdb.h>


namespace imtdb
{


// public methods

QString CSqlDatabaseObjectDelegateCompBase::SqlEncode(const QString& sqlQuery) const
{
	return imtdb::SqlEncode(sqlQuery);
}

// reimplemented (imtdb::ISqlDatabaseObjectDelegate)

const iprm::IOptionsList* CSqlDatabaseObjectDelegateCompBase::GetObjectTypeInfos() const
{
	return m_typesCompPtr.GetPtr();
}


IDatabaseObjectDelegate::NewObjectQuery CSqlDatabaseObjectDelegateCompBase::CreateUpdateObjectQueryWithParameters(
			const imtbase::IObjectCollection& collection,
			const QByteArray& objectId,
			const istd::IChangeable& object,
			const imtbase::IOperationContext* operationContextPtr,
			bool useExternDelegate) const
{
	NewObjectQuery result;
	result.query = CreateUpdateObjectQuery(collection, objectId, object, operationContextPtr, useExternDelegate);
	return result;
}


QByteArray CSqlDatabaseObjectDelegateCompBase::GetObjectTypeId(const QByteArray& objectId) const
{
	if (objectId.isEmpty()){
		return QByteArray();
	}

	QByteArray objectSelectionQuery = GetSelectionQuery(objectId, -1, -1, nullptr);
	if (objectSelectionQuery.isEmpty()){
		return QByteArray();
	}

	QSqlError sqlError;
	QSqlQuery sqlQuery = m_databaseEngineCompPtr->ExecSqlQuery(objectSelectionQuery, &sqlError);
	if (sqlError.type() != QSqlError::NoError){
		return QByteArray();
	}

	if (!sqlQuery.last()){
		return QByteArray();
	}

	QString columnId = *m_objectTypeIdColumnAttrPtr;

	QSqlRecord record = sqlQuery.record();
	if (record.contains(columnId)){
		return imtdb::VariantToByteArray(record.value(columnId));
	}

	// Fallback:
	if (m_typesCompPtr.IsValid()){
		if (m_typesCompPtr->GetOptionsCount() == 1){
			return m_typesCompPtr->GetOptionId(0);
		}
	}

	return QByteArray();
}


QByteArray CSqlDatabaseObjectDelegateCompBase::GetCountQuery(const iprm::IParamsSet* paramsPtr) const
{
	QString filterQuery;
	if (paramsPtr != nullptr){
		if (!CreateFilterQuery(*paramsPtr, filterQuery)){
			return QByteArray();
		}
	}

	if (!m_tableSchemaAttrPtr.IsValid()){
		return QStringLiteral(R"(SELECT COUNT(*) FROM "%1" %2)").arg(*m_tableNameAttrPtr, filterQuery).toUtf8();
	}

	return QStringLiteral(R"(SELECT COUNT(*) FROM %0."%1" %2)")
					.arg(
						*m_tableSchemaAttrPtr,
						*m_tableNameAttrPtr,
						filterQuery
					).toUtf8();
}


QByteArray CSqlDatabaseObjectDelegateCompBase::GetSelectionQuery(
			const QByteArray& objectId,
			int offset,
			int count,
			const iprm::IParamsSet* paramsPtr) const
{
	if (!objectId.isEmpty()){
		if (m_tableSchemaAttrPtr.IsValid()){
			return QStringLiteral(R"(SELECT * FROM %1."%2" WHERE "%3" = '%4')")
						.arg(
							/*1*/ *m_tableSchemaAttrPtr,
							/*2*/ *m_tableNameAttrPtr,
							/*3*/ *m_objectIdColumnAttrPtr,
							/*4*/ objectId
						).toUtf8();
		}

		return QStringLiteral(R"(SELECT * FROM "%1" WHERE "%2" = '%3')")
						.arg(
							*m_tableNameAttrPtr,
							*m_objectIdColumnAttrPtr,
							objectId
						).toUtf8();
	}

	QString sortQuery;
	QString filterQuery;
	if (count == 0){
		return QByteArray();
	}

	istd::TOptDelPtr<const iprm::IParamsSet> selectionParamsPtr;
	if (paramsPtr != nullptr){
		selectionParamsPtr.SetPtr(paramsPtr, false);
	}
	else{
		selectionParamsPtr.SetPtr(new iprm::CParamsSet(), true);
	}

	if (selectionParamsPtr.IsValid()){
		if (!CreateFilterQuery(*selectionParamsPtr, filterQuery)){
			return QByteArray();
		}

		iprm::IParamsSet::Ids paramIds = selectionParamsPtr->GetParamIds();
		if (paramIds.contains(QByteArrayLiteral("ComplexFilter"))){
			iprm::TParamsPtr<imtbase::IComplexCollectionFilter> complexFilterParamPtr(selectionParamsPtr.GetPtr(), QByteArrayLiteral("ComplexFilter"));
			if (!CreateSortQuery(*complexFilterParamPtr, sortQuery)){
				return QByteArray();
			}
		}
		else if (paramIds.contains(QByteArrayLiteral("Filter"))){
			iprm::TParamsPtr<imtbase::ICollectionFilter> collectionFilterParamPtr(selectionParamsPtr.GetPtr(), QByteArrayLiteral("Filter"));
			if (!CreateSortQuery(*collectionFilterParamPtr, sortQuery)){
				return QByteArray();
			}
		}
	}

	QByteArray paginationQuery;
	if (!CreatePaginationQuery(offset, count, paginationQuery)){
		return QByteArray();
	}

	QString baseSelelectionQuery = GetBaseSelectionQuery();

	// SQLite does not support a top-level parenthesized SELECT statement, so skip the
	// Qt '%<NUMBER>%' bug workaround wrapping on SQLite — it only affects Postgres drivers.
	QString retVal;
	if (IsSqliteDriver()){
		retVal = baseSelelectionQuery;
		retVal += QStringLiteral(" ") + filterQuery;
		retVal += QStringLiteral(" ") + sortQuery;
		retVal += QStringLiteral(" ") + paginationQuery;
	}
	else{
		// Due to a bug in qt in the context of resolving of an expression like this: '%<SOME_NUMBER>%'
		retVal = '(' + baseSelelectionQuery;
		retVal += QStringLiteral(" ") + filterQuery;
		retVal += QStringLiteral(" ") + paginationQuery + ')';
		retVal += QStringLiteral(" ") + sortQuery;
	}

	return retVal.toUtf8();
}


QByteArray CSqlDatabaseObjectDelegateCompBase::GetObjectIdFromRecord(const QSqlRecord& record) const
{
	QString columnId = *m_objectIdColumnAttrPtr;

	if (record.contains(columnId)){
		return imtdb::VariantToByteArray(record.value(columnId));
	}

	return QByteArray();
}


QByteArray CSqlDatabaseObjectDelegateCompBase::GetObjectTypeIdFromRecord(const QSqlRecord& record) const
{
	QString columnId = *m_objectTypeIdColumnAttrPtr;

	if (record.contains(columnId)){
		return imtdb::VariantToByteArray(record.value(columnId));
	}

	return QByteArray();
}



bool CSqlDatabaseObjectDelegateCompBase::CreateObjectInfoFromRecord(
			const QSqlRecord& record,
			idoc::MetaInfoPtr& objectMetaInfoPtr,
			idoc::MetaInfoPtr& collectionItemMetaInfoPtr) const
{
	QByteArray typeId = GetObjectTypeIdFromRecord(record);
	objectMetaInfoPtr = CreateObjectMetaInfo(typeId);
	if (objectMetaInfoPtr.IsValid()){
		if (!SetObjectMetaInfoFromRecord(record, *objectMetaInfoPtr)){
			objectMetaInfoPtr.Reset();

			return false;
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


QVariant CSqlDatabaseObjectDelegateCompBase::GetElementInfoFromRecord(const QSqlRecord& sqlRecord, const QByteArray& infoId) const
{
	if (sqlRecord.contains(infoId)){
		return sqlRecord.value(QString(infoId));
	}

	return QVariant();
}


QByteArray CSqlDatabaseObjectDelegateCompBase::CreateResetQuery(const imtbase::IObjectCollection& /*collection*/) const
{
	if (!m_tableSchemaAttrPtr.IsValid()){
		return QStringLiteral(R"(DELETE FROM "%1";)").arg(*m_tableNameAttrPtr).toUtf8();
	}

	return QStringLiteral(R"(DELETE FROM %0."%1";)")
			.arg(*m_tableSchemaAttrPtr, *m_tableNameAttrPtr).toUtf8();
}


QByteArray CSqlDatabaseObjectDelegateCompBase::CreateDataMetaInfoQuery(
			const imtbase::IObjectCollection& /*collection*/,
			const QByteArray& /*objectId*/,
			const idoc::IDocumentMetaInfo* /*dataMetaInfoPtr*/) const
{
	QByteArray retVal;

	return retVal;
}


QByteArray CSqlDatabaseObjectDelegateCompBase::CreateCollectionItemMetaInfoQuery(
			const imtbase::IObjectCollection& /*collection*/,
			const QByteArray& /*objectId*/,
			const idoc::IDocumentMetaInfo* /*collectionItemMetaInfoPtr*/) const
{
	QByteArray retVal;

	return retVal;
}


QByteArray CSqlDatabaseObjectDelegateCompBase::GetSelectionByMetaInfoQuery(
			const QByteArray& /*metaInfoId*/,
			const QVariant& /*metaInfoValue*/) const
{
	return QByteArray();
}


QByteArray CSqlDatabaseObjectDelegateCompBase::CreateUpdateMetaInfoQuery(const QSqlRecord& /*record*/) const
{
	return QByteArray();
}


QByteArray CSqlDatabaseObjectDelegateCompBase::GetTableName() const
{
	return *m_tableNameAttrPtr;
}


QByteArray CSqlDatabaseObjectDelegateCompBase::GetTableScheme() const
{
	if (m_tableSchemaAttrPtr.IsValid()){
		return *m_tableSchemaAttrPtr;
	}

	return QByteArray();
}


QByteArray CSqlDatabaseObjectDelegateCompBase::CreateRestoreObjectsQuery(
			const imtbase::IObjectCollection& /*collection*/,
			const imtbase::ICollectionInfo::Ids& /*objectIds*/,
			const imtbase::IOperationContext* /*operationContextPtr*/) const
{
	return QByteArray();
}


QByteArray CSqlDatabaseObjectDelegateCompBase::CreateRestoreObjectSetQuery(
			const imtbase::IObjectCollection& /*collection*/,
			const iprm::IParamsSet* /*paramsPtr*/,
			const imtbase::IOperationContext* /*operationContextPtr*/) const
{
	return QByteArray();
}


// protected methods

QString CSqlDatabaseObjectDelegateCompBase::GetBaseSelectionQuery() const
{
	if (!m_tableSchemaAttrPtr.IsValid()){
		return QStringLiteral(R"(SELECT * FROM "%1")").arg(*m_tableNameAttrPtr);
	}

	return QStringLiteral(R"(SELECT * FROM %0."%1")").arg(*m_tableSchemaAttrPtr, *m_tableNameAttrPtr);
}


idoc::IDocumentMetaInfo* CSqlDatabaseObjectDelegateCompBase::CreateCollectionItemMetaInfo(const QByteArray& /*typeId*/) const
{
	return new imod::TModelWrap<idoc::CStandardDocumentMetaInfo>;
}


bool CSqlDatabaseObjectDelegateCompBase::SetCollectionItemMetaInfoFromRecord(const QSqlRecord& record, idoc::IDocumentMetaInfo& metaInfo) const
{
	if (record.contains(QStringLiteral("Name"))){
		metaInfo.SetMetaInfo(idoc::IDocumentMetaInfo::MIT_TITLE, record.value(QStringLiteral("Name")));
	}

	if (record.contains(QStringLiteral("Description"))){
		metaInfo.SetMetaInfo(idoc::IDocumentMetaInfo::MIT_DESCRIPTION, record.value(QStringLiteral("Description")));
	}

	if (record.contains(QStringLiteral("Added"))){
		QDateTime insertionTime = record.value(QStringLiteral("Added")).toDateTime();

		metaInfo.SetMetaInfo(idoc::IDocumentMetaInfo::MIT_CREATION_TIME, insertionTime);
		metaInfo.SetMetaInfo(imtbase::IObjectCollection::MIT_INSERTION_TIME, insertionTime);
	}

	if (record.contains(QStringLiteral("LastModified"))){
		QDateTime lastModificationTime = record.value(QStringLiteral("LastModified")).toDateTime();

		metaInfo.SetMetaInfo(idoc::IDocumentMetaInfo::MIT_MODIFICATION_TIME, lastModificationTime);
		metaInfo.SetMetaInfo(imtbase::IObjectCollection::MIT_LAST_OPERATION_TIME, lastModificationTime);
	}

	else if (record.contains(QStringLiteral("TimeStamp"))){
		QDateTime lastModificationTime = record.value(QStringLiteral("TimeStamp")).toDateTime();

		metaInfo.SetMetaInfo(idoc::IDocumentMetaInfo::MIT_MODIFICATION_TIME, lastModificationTime);
		metaInfo.SetMetaInfo(imtbase::IObjectCollection::MIT_LAST_OPERATION_TIME, lastModificationTime);
	}

	if (record.contains(QStringLiteral("RevisionNumber"))){
		qlonglong revisionNumber = record.value(QStringLiteral("RevisionNumber")).toLongLong();

		metaInfo.SetMetaInfo(imtbase::ICollectionInfo::MIT_REVISION, revisionNumber);
	}

	if (record.contains(QStringLiteral("Checksum"))){
		int checksum = imtbase::narrow_cast<int>(record.value(QStringLiteral("Checksum")).toUInt());

		metaInfo.SetMetaInfo(idoc::IDocumentMetaInfo::MIT_CONTENT_CHECKSUM, checksum);
	}

	return true;
}


idoc::MetaInfoPtr CSqlDatabaseObjectDelegateCompBase::CreateObjectMetaInfo(const QByteArray& /*typeId*/) const
{
	return idoc::MetaInfoPtr();
}


bool CSqlDatabaseObjectDelegateCompBase::SetObjectMetaInfoFromRecord(const QSqlRecord& /*record*/, idoc::IDocumentMetaInfo& /*metaInfo*/) const
{
	return false;
}


bool CSqlDatabaseObjectDelegateCompBase::CreatePaginationQuery(int offset, int count, QByteArray& paginationQuery) const
{
	paginationQuery.clear();

	if (offset >= 0 && count > 0){
		if (IsSqliteDriver()){
			paginationQuery = QStringLiteral("LIMIT %1 OFFSET %2").arg(count, offset).toUtf8();
		}
		else{
			paginationQuery = QStringLiteral("OFFSET %1 ROWS FETCH NEXT %2 ROWS ONLY").arg(offset, count).toUtf8();
		}
	}

	return true;
}


bool CSqlDatabaseObjectDelegateCompBase::CreateFilterQuery(const iprm::IParamsSet& filterParams, QString& filterQuery) const
{
	bool retVal = true;

	QString objectFilterQuery;
	QString timeFilterQuery;
	QString textFilterQuery;

	iprm::IParamsSet::Ids paramIds = filterParams.GetParamIds();
	if (!paramIds.contains(QByteArrayLiteral("ComplexFilter"))){
		retVal = CreateObjectFilterQuery(filterParams, objectFilterQuery);
		if (!retVal){
			return false;
		}
	}

	if (paramIds.contains(QByteArrayLiteral("Filter"))){
		iprm::TParamsPtr<imtbase::ICollectionFilter> collectionFilterParamPtr(&filterParams, QByteArrayLiteral("Filter"), false);
		if (collectionFilterParamPtr.IsValid()){
			retVal = CreateTextFilterQuery(*collectionFilterParamPtr, textFilterQuery);
			if (!retVal){
				return false;
			}
		}
	}
	else if (paramIds.contains(QByteArrayLiteral("ComplexFilter"))){
		iprm::TParamsPtr<imtbase::IComplexCollectionFilter> complexFilterParamPtr(&filterParams, QByteArrayLiteral("ComplexFilter"), false);
		if(complexFilterParamPtr.IsValid()){
			CreateTextFilterQuery(*complexFilterParamPtr, textFilterQuery);
			CreateObjectFilterQuery(*complexFilterParamPtr, objectFilterQuery);
			CreateTimeFilterQuery(complexFilterParamPtr->GetTimeFilter(), timeFilterQuery);
		}
	}

	const QString additionalFilters = CreateAdditionalFiltersQuery(filterParams);

	if (!objectFilterQuery.isEmpty() || !textFilterQuery.isEmpty() || !additionalFilters.isEmpty()){
		filterQuery = QStringLiteral(" WHERE ");
	}

	filterQuery += objectFilterQuery;
	if (!objectFilterQuery.isEmpty() && !textFilterQuery.isEmpty()){
		filterQuery += QStringLiteral(" AND ");
	}

	if (!textFilterQuery.isEmpty()){
		filterQuery += '(' + textFilterQuery + ')';
	}

	if ((!objectFilterQuery.isEmpty() || !textFilterQuery.isEmpty()) && !timeFilterQuery.isEmpty()){
		filterQuery += QStringLiteral(" AND ");
	}

	if (!timeFilterQuery.isEmpty()){
		filterQuery += '(' + timeFilterQuery + ')';
	}

	if ((!objectFilterQuery.isEmpty() || !textFilterQuery.isEmpty() || !timeFilterQuery.isEmpty()) && !additionalFilters.isEmpty()){
		filterQuery += QStringLiteral(" AND ");
	}

	if(!additionalFilters.isEmpty()){
		filterQuery += '(' + additionalFilters + ')';
	}

	return true;
}


bool CSqlDatabaseObjectDelegateCompBase::CreateObjectFilterQuery(
			const iprm::IParamsSet& filterParams,
			QString& filterQuery) const
{
	QString objectFilterQuery;
	iprm::TParamsPtr<iprm::IParamsSet> objectFilterParamPtr(&filterParams, QByteArrayLiteral("ObjectFilter"), false);
	if (!objectFilterParamPtr.IsValid()){
		return true;
	}

	iprm::IParamsSet::Ids paramIds = objectFilterParamPtr->GetParamIds();
	if (paramIds.isEmpty()){
		return true;
	}

#if QT_VERSION < 0x060000
	QByteArrayList idsList(paramIds.toList());
#else
	QByteArrayList idsList(paramIds.cbegin(), paramIds.cend());
#endif
	QByteArray key = idsList[0];

	iprm::TParamsPtr<iprm::ITextParam> textParamPtr(objectFilterParamPtr.GetPtr(), key);
	if (!textParamPtr.IsValid()){
		return false;
	}

	QString value = textParamPtr->GetText();
	filterQuery = QStringLiteral(R"("%1" = '%2')").arg(key, SqlEncode(value));

	return true;
}


bool CSqlDatabaseObjectDelegateCompBase::CreateObjectFilterQuery(const imtbase::IComplexCollectionFilter& collectionFilter, QString& filterQuery) const
{
	filterQuery = CComplexCollectionFilterConverter::CreateSqlFilterQuery(
				collectionFilter,
				IsSqliteDriver() ? CComplexCollectionFilterConverter::SC_GENERAL : CComplexCollectionFilterConverter::SC_POSTGRES);

	return true;
}


bool CSqlDatabaseObjectDelegateCompBase::CreateTextFilterQuery(
			const imtbase::ICollectionFilter& collectionFilter,
			QString& textFilterQuery) const
{
	QByteArrayList filteringColumnIds = collectionFilter.GetFilteringInfoIds();
	if (filteringColumnIds.isEmpty()){
		return true;
	}

	QString textFilter = collectionFilter.GetTextFilter();
	if (textFilter.isEmpty()){
		return true;
	}

	QString encodedFilter = SqlEncode(textFilter);
	textFilterQuery = QStringLiteral(R"("%1" ILIKE '%%2%')").arg(filteringColumnIds.first(), encodedFilter);

	for (int i = 1; i < filteringColumnIds.count(); ++i){
		textFilterQuery += QStringLiteral(" OR ");
		textFilterQuery += QStringLiteral(R"("%1" ILIKE '%%2%')").arg(filteringColumnIds[i], encodedFilter);
	}

	return true;
}


bool CSqlDatabaseObjectDelegateCompBase::CreateTextFilterQuery(const imtbase::IComplexCollectionFilter& collectionFilter, QString& textFilterQuery) const
{
	textFilterQuery.clear();

	QString textFilter = collectionFilter.GetTextFilter();
	if (textFilter.isEmpty()){
		return true;
	}

	QString encodedFilter = SqlEncode(textFilter);
	for (const imtbase::IComplexCollectionFilter::FieldInfo& info : collectionFilter.GetFields()){
		if (info.metaInfo.flags & imtbase::IComplexCollectionFilter::SO_TEXT_FILTER){
			if (!textFilterQuery.isEmpty()){
				textFilterQuery += QStringLiteral(" OR ");
			}

			textFilterQuery += QStringLiteral(R"("%1" ILIKE '%%2%')").arg(info.id, encodedFilter);
		}
	}

	return true;
}


bool CSqlDatabaseObjectDelegateCompBase::CreateTimeFilterQuery(const imtbase::ITimeFilterParam& /*timeFilter*/, QString& /*timeFilterQuery*/, const QString& /*timeFieldId*/) const
{
	/// Implementation meant for derived classes

	return true;
}


bool CSqlDatabaseObjectDelegateCompBase::CreateSortQuery(
			const imtbase::ICollectionFilter& collectionFilter,
			QString& sortQuery) const
{
	QByteArray columnId;
	QByteArray sortOrder;

	const QByteArrayList sortingInfoIds = collectionFilter.GetSortingInfoIds();
	if (!sortingInfoIds.isEmpty()){
		columnId = sortingInfoIds.first();
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
		sortQuery = QStringLiteral(R"(ORDER BY "%1" %2)").arg(columnId, sortOrder);
	}

	return true;
}


bool CSqlDatabaseObjectDelegateCompBase::CreateSortQuery(const imtbase::IComplexCollectionFilter& collectionFilter, QString& sortQuery) const
{
	sortQuery = CComplexCollectionFilterConverter::CreateSqlSortQuery(collectionFilter);

	return true;
}


QString CSqlDatabaseObjectDelegateCompBase::EncodeTextArgument(const QString& argument) const
{
	return SqlEncode(argument);
}


bool CSqlDatabaseObjectDelegateCompBase::IsSqliteDriver() const
{
	return m_databaseEngineCompPtr.IsValid() && m_databaseEngineCompPtr->GetDatabaseDriverId().compare(QByteArrayLiteral("QSQLITE"), Qt::CaseInsensitive) == 0;
}


bool CSqlDatabaseObjectDelegateCompBase::TableExists(const QString& tableName) const
{
	if (!m_databaseEngineCompPtr.IsValid()){
		return false;
	}

	QString driverId = m_databaseEngineCompPtr->GetDatabaseDriverId();
	QString tableExistsQuery;

	if (driverId == QByteArrayLiteral("QPSQL")){
		const QByteArray tableSchema = GetTableScheme();
		const QString schemaName = tableSchema.isEmpty() ? QStringLiteral("public") : tableSchema;

		tableExistsQuery = QStringLiteral("SELECT EXISTS (SELECT 1 FROM information_schema.tables WHERE table_schema = '%1' AND table_name = '%2');")
								.arg(schemaName, tableName);
	}
	else if (IsSqliteDriver()){
		tableExistsQuery = QStringLiteral("SELECT EXISTS (SELECT 1 FROM sqlite_master WHERE type='table' AND name='%1');")
								.arg(tableName);
	}
	else{
		return false;
	}

	QSqlError sqlError;
	QSqlQuery sqlQuery = m_databaseEngineCompPtr->ExecSqlQuery(tableExistsQuery.toUtf8(), &sqlError);
	
	if (sqlError.type() != QSqlError::NoError){
		return false;
	}

	if (!sqlQuery.next()){
		return false;
	}

	return sqlQuery.value(0).toBool();
}


QString CSqlDatabaseObjectDelegateCompBase::CreateAdditionalFiltersQuery(const iprm::IParamsSet& /*filterParams*/) const
{
	return QString();
}


// reimplemented (icomp::CComponentBase)

void CSqlDatabaseObjectDelegateCompBase::OnComponentCreated()
{
	BaseClass::OnComponentCreated();

	if (!CreateTableIfNeeded()){
		SendWarningMessage(0, QT_TR_NOOP("Collection table auto-creation failed; see previous errors. Component initialization was stopped"));
	}
}


bool CSqlDatabaseObjectDelegateCompBase::CreateTableIfNeeded()
{
	const bool autoCreateTable = m_autoCreateTableAttrPtr.IsValid() ? *m_autoCreateTableAttrPtr : false;
	if (!autoCreateTable){
		return true;
	}

	if (!m_databaseEngineCompPtr.IsValid()){
		return false;
	}

	const QString tableName = QString::fromUtf8(GetTableName());
	if (tableName.isEmpty()){
		return false;
	}

	if (TableExists(tableName)){
		return true;
	}

	const QByteArray scriptPath = m_createTableScriptPathAttrPtr.IsValid() ? *m_createTableScriptPathAttrPtr : QByteArray();
	if (scriptPath.isEmpty()){
		SendErrorMessage(0, QT_TR_NOOP("Table creation script path is empty"));
		return false;
	}

	const QByteArray prerequisiteScriptPath = m_prerequisiteTableScriptPathAttrPtr.IsValid()
			? *m_prerequisiteTableScriptPathAttrPtr : QByteArray();
	if (!prerequisiteScriptPath.isEmpty() && !ExecuteTableScript(prerequisiteScriptPath, tableName)){
		return false;
	}

	return ExecuteTableScript(scriptPath, tableName);
}


bool CSqlDatabaseObjectDelegateCompBase::ExecuteTableScript(const QByteArray& scriptPath, const QString& tableName)
{
	QString resourcePath = QString::fromUtf8(scriptPath);
	if (!resourcePath.startsWith(QStringLiteral(":/"))){
		resourcePath = GetSqlResourcePath(*m_databaseEngineCompPtr, resourcePath);
	}

	QFile scriptFile(resourcePath);
	if (!scriptFile.open(QFile::ReadOnly)){
		SendErrorMessage(0, QString::fromUtf8(QT_TR_NOOP("Collection table creation script '%1' could not be loaded"))
								.arg(scriptFile.fileName()));
		return false;
	}

	QByteArray createTableQuery = scriptFile.readAll();
	scriptFile.close();

	QByteArray tableScheme = GetTableScheme();
	if (!tableScheme.isEmpty()){
		createTableQuery.replace(QByteArrayLiteral("${TableScheme}"), tableScheme);
	}
	else{
		createTableQuery.replace(QByteArrayLiteral("${TableScheme}"), QByteArrayLiteral("public"));
	}

	createTableQuery.replace(QByteArrayLiteral("${TableName}"), tableName.toUtf8());

	QSqlError sqlError;
	QByteArray executedQuery = createTableQuery;
	if (!IsSqliteDriver()){
		m_databaseEngineCompPtr->ExecSqlQuery(createTableQuery, &sqlError);
	}
	else{
		sqlError = QSqlError();
		const QList<QByteArray> statements = createTableQuery.split(';');
		for (QByteArray statement : statements){
			statement = statement.trimmed();
			if (statement.isEmpty()){
				continue;
			}

			executedQuery = statement;
			m_databaseEngineCompPtr->ExecSqlQuery(statement, &sqlError);
			if (sqlError.type() != QSqlError::NoError){
				break;
			}
		}
	}

	if (sqlError.type() != QSqlError::NoError){
		qCritical() << __FILE__ << __LINE__
					<< "\n\t| Table could not be created"
					<< "\n\t| Error: " << sqlError
					<< "\n\t| Query: " << executedQuery;

		SendErrorMessage(0, QString::fromUtf8(QT_TR_NOOP("\n\t| Table could not be created"
														"\n\t| Error: %1"
														"\n\t| Query: %2"))
								.arg(sqlError.text(), executedQuery));
		return false;
	}

	return true;
}


} // namespace imtdb
