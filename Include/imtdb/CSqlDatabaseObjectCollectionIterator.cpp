#include <imtdb/CSqlDatabaseObjectCollectionIterator.h>


// ImtCore includes
#include <imtbase/IObjectCollection.h>


namespace imtdb
{


// public methods

CSqlDatabaseObjectCollectionIterator::CSqlDatabaseObjectCollectionIterator(QSqlQuery sqlQuery, ISqlDatabaseObjectDelegate *databaseDelegate):
	m_currentIndex(-1)
{
	while(sqlQuery.next()){
		m_records.append(sqlQuery.record());
	}

	m_databaseDelegate = databaseDelegate;
}


// reimplemented (imtbase::IObjectCollectionIterator)

bool CSqlDatabaseObjectCollectionIterator::Next()
{
	if (m_currentIndex < m_records.size() - 1){
		m_currentIndex++;

		return true;
	}

	return false;
}


bool CSqlDatabaseObjectCollectionIterator::Previous()
{
	if (m_currentIndex > 0){
		m_currentIndex--;

		return true;
	}

	return false;
}


QByteArray CSqlDatabaseObjectCollectionIterator::GetObjectId() const
{
	if (m_databaseDelegate == nullptr){
		return QByteArray();
	}

	return 	m_databaseDelegate->GetObjectIdFromRecord(m_records[m_currentIndex]);
}


bool imtdb::CSqlDatabaseObjectCollectionIterator::GetObjectData(imtbase::IObjectCollection::DataPtr &dataPtr) const
{
	if (m_databaseDelegate == nullptr || m_currentIndex < 0 || m_currentIndex >= m_records.count()){
		return false;
	}

	istd::IChangeable* dataObjPtr = m_databaseDelegate->CreateObjectFromRecord(m_records[m_currentIndex]);
	dataPtr = DataPtr(DataPtr::RootObjectPtr(dataObjPtr), [dataObjPtr](){
		return dataObjPtr;
	});

	return dataPtr.IsValid();
}


idoc::MetaInfoPtr CSqlDatabaseObjectCollectionIterator::GetDataMetaInfo() const
{
	if (m_databaseDelegate == nullptr || m_currentIndex < 0 || m_currentIndex >= m_records.count()){
		return idoc::MetaInfoPtr();
	}

	idoc::MetaInfoPtr objectMetaInfoPtr;
	idoc::MetaInfoPtr collectionMetaInfoPtr;

	m_databaseDelegate->CreateObjectInfoFromRecord(m_records[m_currentIndex], objectMetaInfoPtr, collectionMetaInfoPtr);

	return collectionMetaInfoPtr;
}


QVariant CSqlDatabaseObjectCollectionIterator::GetElementInfo(QByteArray infoId) const
{
	if (m_databaseDelegate == nullptr || m_currentIndex < 0 || m_currentIndex >= m_records.count()){
		return QVariant();
	}

	return m_databaseDelegate->GetElementInfoFromRecord(m_records[m_currentIndex], infoId);
}


} // namespace imtdb


