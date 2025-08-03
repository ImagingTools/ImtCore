#include <imtdb/CSqliteDatabaseDocumentDelegateComp.h>


// ACF includes
#include <iprm/TParamsPtr.h>

// ImtCore includes
#include <imtbase/IComplexCollectionFilter.h>
#include <imtdb/CComplexCollectionFilterConverter.h>


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
	if (m_tableSchemaAttrPtr.IsValid()){
		schema = *m_tableSchemaAttrPtr + ".";
	}

	Q_ASSERT(!(*m_tableNameAttrPtr).isEmpty());

	QString tableName = *m_tableNameAttrPtr;
	QByteArray joinTablesQuery = CreateJoinTablesQuery();

	QString customColumns = GetCustomColumnsQuery();
	if (!customColumns.isEmpty()){
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
		if (value.canConvert<RawSqlExpression>()){
			RawSqlExpression raw = value.value<RawSqlExpression>();
			revisionInfo += QString("'%1', %2").arg(key, raw.sql);
		}
		else if (value.type() == QMetaType::QString || value.type() == QMetaType::QByteArray){
			revisionInfo += QString("'%1', '%2'").arg(key, value.toString());
		}
		else if (value.type() == QMetaType::Int){
			revisionInfo += QString("'%1', %2").arg(key).arg(value.toInt());
		}
		else if (value.type() == QMetaType::Bool){
			revisionInfo += QString("'%1', %2").arg(key).arg(value.toBool());
		}
	}

	revisionInfo += ")";

	return revisionInfo.toUtf8();
}



bool CSqliteDatabaseDocumentDelegateComp::CreatePaginationQuery(int offset, int count, QByteArray& paginationQuery) const
{
	paginationQuery.clear();

	if (offset >= 0 && count > 0){
		paginationQuery = QString("LIMIT %1 OFFSET %2").arg(count).arg(offset).toUtf8();
	}

	return true;
}


bool CSqliteDatabaseDocumentDelegateComp::CreateTimeFilterQuery(const imtbase::ITimeFilterParam& timeFilter, QString& timeFilterQuery) const
{
	QString addedStrQuery = QString(R"((SELECT "TimeStamp" FROM "%1" as temp WHERE %2 = 1 AND root."DocumentId" = temp."DocumentId" LIMIT 1))")
	.arg(
		qPrintable(*m_tableNameAttrPtr),
		CreateJsonExtractSql("RevisionInfo", "RevisionNumber", QMetaType::Int)
		);

	switch (timeFilter.GetTimeUnit()){
	case imtbase::ITimeFilterParam::TU_CUSTOM:
		break;
	case imtbase::ITimeFilterParam::TU_HOUR:
		switch (timeFilter.GetInterpretationMode()){
		case imtbase::ITimeFilterParam::IM_CURRENT:
			timeFilterQuery += QString(R"((strftime('%Y-%m-%d %H', %1) = strftime('%Y-%m-%d %H', 'now')))").arg(addedStrQuery);
			break;
		case imtbase::ITimeFilterParam::IM_LAST:
			timeFilterQuery += QString(R"((strftime('%Y-%m-%d %H', %1) = strftime('%Y-%m-%d %H', datetime('now', '-1 hour'))))").arg(addedStrQuery);
			break;
		case imtbase::ITimeFilterParam::IM_FOR:
			timeFilterQuery += QString(R"((%1 >= datetime('now', '-1 hour') AND %1 <= datetime('now')))").arg(addedStrQuery);
			break;
		}
		break;
	case imtbase::ITimeFilterParam::TU_DAY:
		switch (timeFilter.GetInterpretationMode()){
		case imtbase::ITimeFilterParam::IM_CURRENT:
			timeFilterQuery += QString(R"((date(%1) = date('now')))").arg(addedStrQuery);
			break;
		case imtbase::ITimeFilterParam::IM_LAST:
			timeFilterQuery += QString(R"((date(%1) = date('now', '-1 day')))").arg(addedStrQuery);
			break;
		case imtbase::ITimeFilterParam::IM_FOR:
			timeFilterQuery += QString(R"((%1 >= datetime('now', '-1 day') AND %1 <= datetime('now')))").arg(addedStrQuery);
			break;
		}
		break;
	case imtbase::ITimeFilterParam::TU_WEEK:
		switch (timeFilter.GetInterpretationMode()){
		case imtbase::ITimeFilterParam::IM_CURRENT:
			timeFilterQuery += QString(R"(strftime('%Y-%W', %1) = strftime('%Y-%W', 'now'))").arg(addedStrQuery);
			break;
		case imtbase::ITimeFilterParam::IM_LAST:
			timeFilterQuery += QString(R"(strftime('%Y-%W', %1) = strftime('%Y-%W', datetime('now', '-7 days')))").arg(addedStrQuery);
			break;
		case imtbase::ITimeFilterParam::IM_FOR:
			timeFilterQuery += QString(R"((%1 >= datetime('now', '-7 days') AND %1 <= datetime('now')))").arg(addedStrQuery);
			break;
		}
		break;
	case imtbase::ITimeFilterParam::TU_MONTH:
		switch (timeFilter.GetInterpretationMode()){
		case imtbase::ITimeFilterParam::IM_CURRENT:
			timeFilterQuery += QString(R"(strftime('%Y-%m', %1) = strftime('%Y-%m', 'now'))").arg(addedStrQuery);
			break;
		case imtbase::ITimeFilterParam::IM_LAST:
			timeFilterQuery += QString(R"(strftime('%Y-%m', %1) = strftime('%Y-%m', datetime('now', '-1 month')))").arg(addedStrQuery);
			break;
		case imtbase::ITimeFilterParam::IM_FOR:
			timeFilterQuery += QString(R"((%1 >= datetime('now', '-1 month') AND %1 <= datetime('now')))").arg(addedStrQuery);
			break;
		}
		break;
	case imtbase::ITimeFilterParam::TU_YEAR:
		switch (timeFilter.GetInterpretationMode()){
		case imtbase::ITimeFilterParam::IM_CURRENT:
			timeFilterQuery += QString(R"(strftime('%Y', %1) = strftime('%Y', 'now'))").arg(addedStrQuery);
			break;
		case imtbase::ITimeFilterParam::IM_LAST:
			timeFilterQuery += QString(R"(strftime('%Y', %1) = strftime('%Y', datetime('now', '-1 year')))").arg(addedStrQuery);
			break;
		case imtbase::ITimeFilterParam::IM_FOR:
			timeFilterQuery += QString(R"((%1 >= datetime('now', '-1 year') AND %1 <= datetime('now')))").arg(addedStrQuery);
			break;
		}
		break;
	}

	if (timeFilterQuery.isEmpty()){
		imtbase::CTimeRange timeRange = timeFilter.GetTimeRange();
		if (!timeRange.IsNull()){
			timeFilterQuery += QString(R"(date(%0) >= date('%1') AND date(%0) <= date('%2'))")
			.arg(addedStrQuery, timeRange.GetBeginTime().toString(Qt::ISODate), timeRange.GetEndTime().toString(Qt::ISODate));
		}
	}

	return true;
}


bool CSqliteDatabaseDocumentDelegateComp::CreateTextFilterQuery(const imtbase::IComplexCollectionFilter& collectionFilter, QString& textFilterQuery) const
{
	textFilterQuery = CComplexCollectionFilterConverter::CreateSqlFilterQuery(collectionFilter);
	SubstituteFieldIds(textFilterQuery, false);
	return true;
}


bool CSqliteDatabaseDocumentDelegateComp::CreateObjectFilterQuery(const imtbase::IComplexCollectionFilter& collectionFilter, QString& filterQuery) const
{
	filterQuery = CComplexCollectionFilterConverter::CreateSqlFilterQuery(collectionFilter);
	SubstituteFieldIds(filterQuery, false);
	return true;
}


} // namespace imtdb


