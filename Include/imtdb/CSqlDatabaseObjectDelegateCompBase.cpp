#include <imtdb/CSqlDatabaseObjectDelegateCompBase.h>


// ACF includes
#include <imod/TModelWrap.h>
#include <iprm/TParamsPtr.h>
#include <idoc/CStandardDocumentMetaInfo.h>


namespace imtdb
{


// public methods

// reimplemented (imtdb::ISqlDatabaseObjectDelegate)

QByteArray CSqlDatabaseObjectDelegateCompBase::GetCountQuery(const iprm::IParamsSet* paramsPtr) const
{
	QString filterQuery;
	if (paramsPtr != nullptr){
		if (!CreateFilterQuery(*paramsPtr, filterQuery)){
			return QByteArray();
		}
	}

	return QString("SELECT COUNT(*) FROM %1 %2").arg(qPrintable(*m_tableNameAttrPtr)).arg(filterQuery).toLocal8Bit();
}


QByteArray CSqlDatabaseObjectDelegateCompBase::GetSelectionQuery(
			const QByteArray& objectId,
			int offset,
			int count,
			const iprm::IParamsSet* paramsPtr) const
{
	if (!objectId.isEmpty()){
		return QString("SELECT * from %1 WHERE %2 = '%3'")
					.arg(qPrintable(*m_tableNameAttrPtr))
					.arg(qPrintable(*m_objectIdColumnAttrPtr))
					.arg(qPrintable(objectId))
					.toLocal8Bit();
	}
	else{
		QString sortQuery;
		QString filterQuery;
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

		return QString("(SELECT * FROM %1 %2 %3) %4")
					.arg(qPrintable(*m_tableNameAttrPtr))
					.arg(filterQuery)
					.arg(qPrintable(paginationQuery))
					.arg(sortQuery)
					.toLocal8Bit();
	}

	return QByteArray();
}


QByteArray CSqlDatabaseObjectDelegateCompBase::GetObjectIdFromRecord(const QByteArray& /*typeId*/, const QSqlRecord& record) const
{
	QString keyId = qPrintable(*m_objectIdColumnAttrPtr);

	if (record.contains(keyId)){
		return record.value(keyId).toByteArray();
	}

	return QByteArray();
}


bool CSqlDatabaseObjectDelegateCompBase::CreateObjectInfoFromRecord(
			const QByteArray& typeId,
			const QSqlRecord& record,
			imtbase::IMetaInfoCreator::MetaInfoPtr& objectMetaInfoPtr,
			imtbase::IMetaInfoCreator::MetaInfoPtr& collectionItemMetaInfoPtr) const
{
	objectMetaInfoPtr.SetPtr(CreateObjectMetaInfo(typeId));
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


// protected methods

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

	return true;
}


idoc::IDocumentMetaInfo* CSqlDatabaseObjectDelegateCompBase::CreateObjectMetaInfo(const QByteArray& /*typeId*/) const
{
	return nullptr;
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
	QString objectFilterQuery;
	bool retVal = CreateObjectFilterQuery(filterParams, objectFilterQuery);
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
		filterQuery += textFilterQuery;
	}

	return true;
}


bool CSqlDatabaseObjectDelegateCompBase::CreateObjectFilterQuery(const iprm::IParamsSet& /*filterParams*/, QString& /*filterQuery*/) const
{
	return true;
}


bool CSqlDatabaseObjectDelegateCompBase::CreateTextFilterQuery(const imtbase::ICollectionFilter& collectionFilter, QString& textFilterQuery) const
{
	QByteArrayList filteringColumnIds = collectionFilter.GetFilteringInfoIds();
	if (filteringColumnIds.isEmpty()){
		return true;
	}

	QString textFilter = collectionFilter.GetTextFilter();

	textFilterQuery = QString("%1 LIKE '%%2%'").arg(qPrintable(filteringColumnIds.first())).arg(textFilter);

	for (int i = 1; i < filteringColumnIds.count(); ++i){
		textFilterQuery += " OR ";

		textFilterQuery += QString("%1 LIKE '%%2%'").arg(qPrintable(filteringColumnIds[i])).arg(textFilter);
	}

	return true;
}


bool CSqlDatabaseObjectDelegateCompBase::CreateSortQuery(const imtbase::ICollectionFilter& collectionFilter, QString& sortQuery) const
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
		sortQuery = QString("ORDER BY %1 %2").arg(qPrintable(columnId)).arg(qPrintable(sortOrder));
	}

	return true;
}


} // namespace imtdb


