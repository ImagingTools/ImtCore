#include <imtdb/CSqlDatabaseObjectDelegateCompBase.h>


// Qt includes
#include <QtSql/QSqlRecord>

// ACF includes
#include <imod/TModelWrap.h>
#include <iprm/TParamsPtr.h>
#include <iprm/ITextParam.h>
#include <idoc/CStandardDocumentMetaInfo.h>


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
					else{
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

	return QString("SELECT COUNT(*) FROM \"%1\" %2").arg(qPrintable(*m_tableNameAttrPtr)).arg(filterQuery).toLocal8Bit();
}


QByteArray CSqlDatabaseObjectDelegateCompBase::GetSelectionQuery(
			const QByteArray& objectId,
			int offset,
			int count,
			const iprm::IParamsSet* paramsPtr) const
{
	if (!objectId.isEmpty()){
		return QString("SELECT * FROM \"%1\" WHERE \"%2\" = '%3'")
					.arg(qPrintable(*m_tableNameAttrPtr))
					.arg(qPrintable(*m_objectIdColumnAttrPtr))
					.arg(qPrintable(objectId))
					.toLocal8Bit();
	}
	else{
		QString sortQuery;
		QString filterQuery;
		if (count == 0){
			return QByteArray();
		}
		if (paramsPtr != nullptr){
			if (!CreateFilterQuery(*paramsPtr, filterQuery)){
				return QByteArray();
			}

			iprm::TParamsPtr<imtbase::ICollectionFilter> collectionFilterParamPtr(paramsPtr, "Filter");
			if (collectionFilterParamPtr.IsValid()){
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

		return retVal.toLocal8Bit();
	}

	return QByteArray();
}


QByteArray CSqlDatabaseObjectDelegateCompBase::GetObjectIdFromRecord(const QSqlRecord& record) const
{
	QString columnId = qPrintable(*m_objectIdColumnAttrPtr);

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
	QByteArray typeId = GetObjectTypeId(GetObjectIdFromRecord(record));
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
	QByteArray retVal = QString("DELETE FROM \"%1\";").arg(qPrintable(*m_tableNameAttrPtr)).toLocal8Bit();

	return retVal;
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


// protected methods

QString CSqlDatabaseObjectDelegateCompBase::GetBaseSelectionQuery() const
{
	return QString("SELECT * FROM \"%1\"").arg(qPrintable(*m_tableNameAttrPtr));
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

	if (record.contains("RevisionNumber")){
		qlonglong revisionNumber = record.value("RevisionNumber").toLongLong();

		metaInfo.SetMetaInfo(imtbase::ICollectionInfo::MIT_REVISION, revisionNumber);
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
		paginationQuery = QString("OFFSET %1 ROWS FETCH NEXT %2 ROWS ONLY").arg(offset).arg(count).toLocal8Bit();
	}

	return true;
}


bool CSqlDatabaseObjectDelegateCompBase::CreateFilterQuery(const iprm::IParamsSet& filterParams, QString& filterQuery) const
{
	bool retVal = true;

	QString objectFilterQuery;
	retVal = CreateObjectFilterQuery(filterParams, objectFilterQuery);
	if (!retVal){
		return false;
	}

	QString textFilterQuery;
	iprm::TParamsPtr<imtbase::ICollectionFilter> collectionFilterParamPtr(&filterParams, "Filter");
	if (collectionFilterParamPtr.IsValid()){
		retVal = CreateTextFilterQuery(*collectionFilterParamPtr, textFilterQuery);
		if (!retVal){
			return false;
		}
	}

	if (!objectFilterQuery.isEmpty() || !textFilterQuery.isEmpty()){
		filterQuery = " WHERE ";
	}

	filterQuery += objectFilterQuery;
	if (!objectFilterQuery.isEmpty() && !textFilterQuery.isEmpty()){
		filterQuery += " AND ";
	}

	if (!textFilterQuery.isEmpty()){
		filterQuery += "(" + textFilterQuery + ")";
	}

	return true;
}


bool CSqlDatabaseObjectDelegateCompBase::CreateObjectFilterQuery(
			const iprm::IParamsSet& filterParams,
			QString& filterQuery) const
{
	bool retVal = true;
	QString objectFilterQuery;
	iprm::TParamsPtr<iprm::IParamsSet> objectFilterParamPtr(&filterParams, "ObjectFilter");
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

			filterQuery = QString("%1 = '%2'").arg(qPrintable(key)).arg(value);
		}
	}

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
		textFilterQuery = QString("\"%1\" ILIKE '%%2%'").arg(qPrintable(filteringColumnIds.first())).arg(textFilter);

		for (int i = 1; i < filteringColumnIds.count(); ++i){
			textFilterQuery += " OR ";

			textFilterQuery += QString("\"%1\" ILIKE '%%2%'").arg(qPrintable(filteringColumnIds[i])).arg(textFilter);
		}
	}

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
	}

	if (!columnId.isEmpty() && !sortOrder.isEmpty()){
		sortQuery = QString("ORDER BY \"%1\" %2").arg(qPrintable(columnId)).arg(qPrintable(sortOrder));
	}

	return true;
}


} // namespace imtdb


