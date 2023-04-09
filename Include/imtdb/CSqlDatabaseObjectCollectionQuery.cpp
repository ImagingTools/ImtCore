#include <imtdb/CSqlDatabaseObjectCollectionQuery.h>


// ImtCore includes
#include <imtbase/IObjectCollection.h>
#include "CSqlDatabaseObjectCollectionQuery.h"


namespace imtdb
{
    
CSqlDatabaseObjectCollectionQuery::CSqlDatabaseObjectCollectionQuery(QSqlQuery sqlQuery, ISqlDatabaseObjectDelegate *databaseDelegate)
{
	m_sqlQuery = sqlQuery;
	m_databaseDelegate = databaseDelegate;
}

bool CSqlDatabaseObjectCollectionQuery::Next()
{
	return m_sqlQuery.next();
}


bool CSqlDatabaseObjectCollectionQuery::Previous()
{
	return m_sqlQuery.previous();
}


QByteArray CSqlDatabaseObjectCollectionQuery::GetObjectId() const
{
	if (m_databaseDelegate == nullptr){
		return QByteArray();
	}
    return 	m_databaseDelegate->GetObjectIdFromRecord(m_sqlQuery.record());
}


bool imtdb::CSqlDatabaseObjectCollectionQuery::GetObjectData(imtbase::IObjectCollection::DataPtr &dataPtr) const
{
	if (m_databaseDelegate == nullptr){
		return false;
	}

	istd::IChangeable* dataObjPtr = m_databaseDelegate->CreateObjectFromRecord(m_sqlQuery.record());
	dataPtr = DataPtr(DataPtr::RootObjectPtr(dataObjPtr), [dataObjPtr](){
		return dataObjPtr;
	});

	return dataPtr.IsValid();
}


idoc::MetaInfoPtr CSqlDatabaseObjectCollectionQuery::GetDataMetaInfo() const
{
	if (m_databaseDelegate == nullptr){
		return idoc::MetaInfoPtr();
	}

	idoc::MetaInfoPtr objectMetaInfoPtr;
	idoc::MetaInfoPtr collectionMetaInfoPtr;

	m_databaseDelegate->CreateObjectInfoFromRecord(m_sqlQuery.record(), objectMetaInfoPtr, collectionMetaInfoPtr);

	return objectMetaInfoPtr;
}


QVariant CSqlDatabaseObjectCollectionQuery::GetElementInfo(QByteArray infoId) const
{
	if (m_databaseDelegate == nullptr){
		return QVariant();
	}

	return m_databaseDelegate->GetElementInfoFromRecord(m_sqlQuery.record(), infoId);
}


} // namespace imtdb
