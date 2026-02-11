// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtdb/CSqlDatabaseObjectDelegateCompBase.h>


// Qt includes
#include <QtSql/QSqlRecord>

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


namespace imtdb
{


// public methods

QString CSqlDatabaseObjectDelegateCompBase::SqlEncode(const QString& sqlQuery) const
{
	QString retVal = sqlQuery;

	return retVal.replace("'", "''").replace(";", "\b");
}


// reimplemented (imtdb::ISqlDatabaseObjectDelegate)

const iprm::IOptionsList* CSqlDatabaseObjectDelegateCompBase::GetObjectTypeInfos() const
{
	return m_typesCompPtr.GetPtr();
}


QByteArray CSqlDatabaseObjectDelegateCompBase::GetObjectTypeId(const QByteArray& objectId) const
{
	if (!objectId.isEmpty()){
		QByteArray objectSelectionQuery = GetSelectionQuery(objectId, -1, -1, nullptr);
		if (!objectSelectionQuery.isEmpty()){
			QSqlError sqlError;
			QSqlQuery sqlQuery = m_databaseEngineCompPtr->ExecSqlQuery(objectSelectionQuery, &sqlError);
			if (sqlError.type() == QSqlError::NoError){
				if (sqlQuery.last()){
					QString columnId = qPrintable(*m_objectTypeIdColumnAttrPtr);

					QSqlRecord record = sqlQuery.record();
					if (record.contains(columnId)){
						return record.value(columnId).toByteArray();
					}

					// Fallback:
					if (m_typesCompPtr.IsValid()){
						if (m_typesCompPtr->GetOptionsCount() == 1){
							return m_typesCompPtr->GetOptionId(0);
						}
					}
				}
			}
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
		return QString("SELECT COUNT(*) FROM \"%1\" %2").arg(qPrintable(*m_tableNameAttrPtr)).arg(filterQuery).toUtf8();
	}

	return QString("SELECT COUNT(*) FROM %0.\"%1\" %2")
			.arg(qPrintable(*m_tableSchemaAttrPtr))
			.arg(qPrintable(*m_tableNameAttrPtr))
			.arg(filterQuery)
			.toUtf8();
}


QByteArray CSqlDatabaseObjectDelegateCompBase::GetSelectionQuery(
			const QByteArray& objectId,
			int offset,
			int count,
			const iprm::IParamsSet* paramsPtr) const
{
	if (!objectId.isEmpty()){
		if (m_tableSchemaAttrPtr.IsValid()){
			return QString("SELECT * FROM %0.\"%1\" WHERE \"%2\" = '%3'")
				.arg(qPrintable(*m_tableSchemaAttrPtr))
				.arg(qPrintable(*m_tableNameAttrPtr))
				.arg(qPrintable(*m_objectIdColumnAttrPtr))
				.arg(qPrintable(objectId))
				.toUtf8();
		}

		return QString("SELECT * FROM \"%1\" WHERE \"%2\" = '%3'")
				.arg(qPrintable(*m_tableNameAttrPtr))
				.arg(qPrintable(*m_objectIdColumnAttrPtr))
				.arg(qPrintable(objectId))
				.toUtf8();
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
		if (paramIds.contains("ComplexFilter")){
			iprm::TParamsPtr<imtbase::IComplexCollectionFilter> complexFilterParamPtr(selectionParamsPtr.GetPtr(), "ComplexFilter");
			if (!CreateSortQuery(*complexFilterParamPtr, sortQuery)){
				return QByteArray();
			}
		}
		else if (paramIds.contains("Filter")){
			iprm::TParamsPtr<imtbase::ICollectionFilter> collectionFilterParamPtr(selectionParamsPtr.GetPtr(), "Filter");
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

	// Due to a bug in qt in the context of resolving of an expression like this: '%<SOME_NUMBER>%'
	QString retVal = "(" + baseSelelectionQuery;
	retVal += QString(" ") + filterQuery;
	retVal += QString(" ") + qPrintable(paginationQuery) + ")";
	retVal += QString(" ") + sortQuery;

	return retVal.toUtf8();
}


QByteArray CSqlDatabaseObjectDelegateCompBase::GetObjectIdFromRecord(const QSqlRecord& record) const
{
	QString columnId = qPrintable(*m_objectIdColumnAttrPtr);

	if (record.contains(columnId)){
		return record.value(columnId).toByteArray();
	}

	return QByteArray();
}


QByteArray CSqlDatabaseObjectDelegateCompBase::GetObjectTypeIdFromRecord(const QSqlRecord& record) const
{
	QString columnId = qPrintable(*m_objectTypeIdColumnAttrPtr);

	if (record.contains(columnId)){
		return record.value(columnId).toByteArray();
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
		return QString("DELETE FROM \"%1\";").arg(qPrintable(*m_tableNameAttrPtr)).toUtf8();
	}

	return QString("DELETE FROM %0.\"%1\";")
			.arg(qPrintable(*m_tableSchemaAttrPtr))
			.arg(qPrintable(*m_tableNameAttrPtr))
			.toUtf8();
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
		return QString("SELECT * FROM \"%1\"").arg(qPrintable(*m_tableNameAttrPtr));
	}

	return QString("SELECT * FROM %0.\"%1\"")
				.arg(qPrintable(*m_tableSchemaAttrPtr))
				.arg(qPrintable(*m_tableNameAttrPtr));
}


idoc::IDocumentMetaInfo* CSqlDatabaseObjectDelegateCompBase::CreateCollectionItemMetaInfo(const QByteArray& /*typeId*/) const
{
	return new imod::TModelWrap<idoc::CStandardDocumentMetaInfo>;
}


bool CSqlDatabaseObjectDelegateCompBase::SetCollectionItemMetaInfoFromRecord(const QSqlRecord& record, idoc::IDocumentMetaInfo& metaInfo) const
{
	if (record.contains("Name")){
		metaInfo.SetMetaInfo(idoc::IDocumentMetaInfo::MIT_TITLE, record.value("Name"));
	}

	if (record.contains("Description")){
		metaInfo.SetMetaInfo(idoc::IDocumentMetaInfo::MIT_DESCRIPTION, record.value("Description"));
	}

	if (record.contains("Added")){
		QDateTime insertionTime = record.value("Added").toDateTime();

		metaInfo.SetMetaInfo(idoc::IDocumentMetaInfo::MIT_CREATION_TIME, insertionTime);
		metaInfo.SetMetaInfo(imtbase::IObjectCollection::MIT_INSERTION_TIME, insertionTime);
	}

	if (record.contains("LastModified")){
		QDateTime lastModificationTime = record.value("LastModified").toDateTime();

		metaInfo.SetMetaInfo(idoc::IDocumentMetaInfo::MIT_MODIFICATION_TIME, lastModificationTime);
		metaInfo.SetMetaInfo(imtbase::IObjectCollection::MIT_LAST_OPERATION_TIME, lastModificationTime);
	}

	else if (record.contains("TimeStamp")){
		QDateTime lastModificationTime = record.value("TimeStamp").toDateTime();

		metaInfo.SetMetaInfo(idoc::IDocumentMetaInfo::MIT_MODIFICATION_TIME, lastModificationTime);
		metaInfo.SetMetaInfo(imtbase::IObjectCollection::MIT_LAST_OPERATION_TIME, lastModificationTime);
	}

	if (record.contains("RevisionNumber")){
		qlonglong revisionNumber = record.value("RevisionNumber").toLongLong();

		metaInfo.SetMetaInfo(imtbase::ICollectionInfo::MIT_REVISION, revisionNumber);
	}

	if (record.contains("Checksum")){
		int checksum = imtbase::narrow_cast<int>(record.value("Checksum").toUInt());

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
		paginationQuery = QStringLiteral("OFFSET %1 ROWS FETCH NEXT %2 ROWS ONLY").arg(offset).arg(count).toUtf8();
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
	if (objectFilterParamPtr.IsValid()){
		iprm::IParamsSet::Ids paramIds = objectFilterParamPtr->GetParamIds();
		if (!paramIds.isEmpty()){
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

			filterQuery = QStringLiteral(R"("%1" = '%2')").arg(qPrintable(key)).arg(value);
		}
	}

	return true;
}


bool CSqlDatabaseObjectDelegateCompBase::CreateObjectFilterQuery(const imtbase::IComplexCollectionFilter& collectionFilter, QString& filterQuery) const
{
	filterQuery = CComplexCollectionFilterConverter::CreateSqlFilterQuery(collectionFilter, CComplexCollectionFilterConverter::SC_POSTGRES);

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
	if (!textFilter.isEmpty()){
		textFilterQuery = QStringLiteral(R"("%1" ILIKE '%%2%')").arg(qPrintable(filteringColumnIds.first())).arg(textFilter);

		for (int i = 1; i < filteringColumnIds.count(); ++i){
			textFilterQuery += QStringLiteral(" OR ");

			textFilterQuery += QStringLiteral(R"("%1" ILIKE '%%2%')").arg(qPrintable(filteringColumnIds[i])).arg(textFilter);
		}
	}

	return true;
}


bool CSqlDatabaseObjectDelegateCompBase::CreateTextFilterQuery(const imtbase::IComplexCollectionFilter& collectionFilter, QString& textFilterQuery) const
{
	textFilterQuery.clear();

	QString textFilter = collectionFilter.GetTextFilter();
	if (!textFilter.isEmpty()){
		for (const imtbase::IComplexCollectionFilter::FieldInfo& info : collectionFilter.GetFields()){
			if (info.metaInfo.flags & imtbase::IComplexCollectionFilter::SO_TEXT_FILTER){
				if (!textFilterQuery.isEmpty()){
					textFilterQuery += QStringLiteral(" OR ");
				}

				textFilterQuery += QStringLiteral(R"("%1" ILIKE '%%2%')").arg(QString::fromUtf8(info.id), textFilter);
			}
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
		sortQuery = QString("ORDER BY \"%1\" %2").arg(qPrintable(columnId)).arg(qPrintable(sortOrder));
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
	QString retVal = argument;

	retVal.replace("'", "''");

	return retVal;
}


bool CSqlDatabaseObjectDelegateCompBase::TableExists(const QString& tableName) const
{
	if (!m_databaseEngineCompPtr.IsValid()){
		return false;
	}

	QString tableExistsQuery = QString(R"(SELECT EXISTS (SELECT FROM information_schema.tables WHERE table_name = '%1');)").arg(tableName);

	QSqlError sqlError;
	QSqlQuery sqlQuery = m_databaseEngineCompPtr->ExecSqlQuery(tableExistsQuery.toUtf8(), &sqlError);

	if (sqlError.type() != QSqlError::NoError){
		return false;
	}

	if (!sqlQuery.next()){
		return false;
	}

	QSqlRecord record = sqlQuery.record();
	if (record.contains("exists")){
		return record.value("exists").toBool();
	}

	return false;
}


QString CSqlDatabaseObjectDelegateCompBase::CreateAdditionalFiltersQuery(const iprm::IParamsSet& /*filterParams*/) const
{
	return QString();
}


} // namespace imtdb


