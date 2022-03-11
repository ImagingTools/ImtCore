#include <imtdb/CSqlDatabaseObjectDelegateCompBase.h>


// ACF includes
#include <imod/TModelWrap.h>
#include <idoc/CStandardDocumentMetaInfo.h>


namespace imtdb
{


// public methods

// reimplemented (imtdb::ISqlDatabaseObjectDelegate)

QByteArray CSqlDatabaseObjectDelegateCompBase::GetSelectionQuery(
			const QByteArray& objectId,
			int offset,
			int count,
			const iprm::IParamsSet* /*paramsPtr*/) const
{
	QString paginationString;
	if (offset > -1 && count > 0){
		paginationString = QString("OFFSET %1 ROWS FETCH NEXT %2 ROWS ONLY").arg(offset).arg(count);
	}

	if (objectId.isEmpty()){
		return QString("SELECT * from %1 %2").arg(qPrintable(*m_tableNameAttrPtr)).arg(paginationString).toLocal8Bit();
	}
	else{
		return QString("SELECT * from %1 WHERE %2 = '%3' %4")
					.arg(qPrintable(*m_tableNameAttrPtr))
					.arg(qPrintable(*m_objectIdColumnAttrPtr))
					.arg(qPrintable(objectId))
					.arg(paginationString)
					.toLocal8Bit();
	}
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


} // namespace imtdb


