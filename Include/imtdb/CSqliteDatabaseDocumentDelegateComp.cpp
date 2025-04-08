#include <imtdb/CSqliteDatabaseDocumentDelegateComp.h>


// ACF includes
#include <iprm/TParamsPtr.h>
#include <iser/CJsonMemWriteArchive.h>

// ImtCore includes
#include <imtbase/IComplexCollectionFilter.h>


namespace imtdb
{


// reimplemented (imtdb::CSqlDatabaseObjectDelegateCompBase)

QByteArray CSqliteDatabaseDocumentDelegateComp::GetSelectionQuery(
	const QByteArray& objectId,
	int offset,
	int count,
	const iprm::IParamsSet* paramsPtr) const
{
	if (!objectId.isEmpty()){
		return GetObjectSelectionQuery(objectId, paramsPtr);
	}
	
	QString sortQuery;
	QString filterQuery;
	if (count == 0){
		return QByteArray();
	}
	if (paramsPtr != nullptr){
		if (!CreateFilterQuery(*paramsPtr, filterQuery)){
			return QByteArray();
		}
		
		iprm::TParamsPtr<imtbase::IComplexCollectionFilter> complexFilterParamPtr(paramsPtr, "ComplexFilter");
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
	
	QString baseSelelectionQuery = GetBaseSelectionQuery();
	
	// Due to a bug in qt in the context of resolving of an expression like this: '%<SOME_NUMBER>%'
	QString retVal = baseSelelectionQuery;
	retVal += QString(" ") + filterQuery;
	retVal += QString(" ") + sortQuery;
	retVal += QString(" ") + qPrintable(paginationQuery);
	
	return retVal.toUtf8();
}


QString CSqliteDatabaseDocumentDelegateComp::GetBaseSelectionQuery() const
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
				root1."TimeStamp" AS "Added" %4
			FROM %1"%2" AS root
			LEFT JOIN (
				SELECT root1."DocumentId", root1."TimeStamp"
				FROM %1"%2" AS root1
				WHERE root1."RevisionInfo" IS NOT NULL
				  AND CAST(json_extract(root1."RevisionInfo", '$.RevisionNumber') AS INTEGER) = 1
				ORDER BY root1."TimeStamp" DESC
				LIMIT 1
			) AS root1 
			ON root1."DocumentId" = root."DocumentId"
			%3
			WHERE root."State" = 'Active'
			)")
			.arg(
				schema,
				tableName,
				joinTablesQuery.isEmpty() ? "" : QString::fromUtf8(joinTablesQuery),
				customColumns
				);

	return query;
}


QByteArray CSqliteDatabaseDocumentDelegateComp::CreateJsonBuildObjectQuery(const QVariantMap& paramMap) const
{
	QString revisionInfo = QString(R"(json_object()");
	
	QStringList keys = paramMap.keys();
	
	for (int i = 0; i < keys.size(); i++){
		if (i > 0){
			revisionInfo += ", ";
		}
		
		QString key = keys[i];
		QVariant value = paramMap[key];
		if (value.canConvert<RawSqlExpression>()) {
			RawSqlExpression raw = value.value<RawSqlExpression>();
			revisionInfo += QString("'%1', %2").arg(key, raw.sql);
		}
		else if (value.typeId() == QMetaType::QString || value.typeId() == QMetaType::QByteArray){
			revisionInfo += QString("'%1', '%2'").arg(key, value.toString());
		}
		else if (value.typeId() == QMetaType::Int){
			revisionInfo += QString("'%1', %2").arg(key).arg(value.toInt());
		}
		else if (value.typeId() == QMetaType::Bool){
			revisionInfo += QString("'%1', %2").arg(key).arg(value.toBool());
		}
	}
	
	revisionInfo += ")";
	
	return revisionInfo.toUtf8();
}


QString CSqliteDatabaseDocumentDelegateComp::CreateJsonExtractSql(
			const QString& jsonName,
			const QString& key,
			QMetaType::Type metaType,
			const QString& tableAlias) const
{
	QString jsonPath = QString("$.%1").arg(key);
	QString fieldWithAlias = QString(R"(%1."%2")").arg(tableAlias, jsonName);
	
	switch (metaType) {
	case QMetaType::QString:
	case QMetaType::QByteArray:
		return QString(R"(json_extract(%1, '%2'))").arg(fieldWithAlias, jsonPath);
		
	case QMetaType::Int:
	case QMetaType::UInt:
	case QMetaType::Short:
	case QMetaType::UShort:
	case QMetaType::LongLong:
	case QMetaType::ULongLong:
		return QString(R"(CAST(json_extract(%1, '%2') AS INTEGER))").arg(fieldWithAlias, jsonPath);
		
	case QMetaType::Double:
	case QMetaType::Float:
		return QString(R"(CAST(json_extract(%1, '%2') AS REAL))").arg(fieldWithAlias, jsonPath);
		
	case QMetaType::Bool:
		return QString(R"(CAST(json_extract(%1, '%2') AS BOOLEAN))").arg(fieldWithAlias, jsonPath);
		
	case QMetaType::QJsonObject:
		return QString(R"(json_extract(%1, '%2'))").arg(fieldWithAlias, jsonPath);
		
	default:
		qWarning() << "Unsupported meta type for SQLite JSON extract:" << metaType;
		Q_ASSERT(false);
		return QString();
	}
}


bool CSqliteDatabaseDocumentDelegateComp::CreatePaginationQuery(int offset, int count, QByteArray& paginationQuery) const
{
	paginationQuery.clear();
	
	if (offset >= 0 && count > 0){
		paginationQuery = QString("LIMIT %1 OFFSET %2").arg(count).arg(offset).toUtf8();
	}
	
	return true;
}


bool CSqliteDatabaseDocumentDelegateComp::CreateTimeFilterQuery(const imtbase::ITimeFilterParam& /*timeFilter*/, QString& /*timeFilterQuery*/) const
{
	return true;
}


} // namespace imtdb


