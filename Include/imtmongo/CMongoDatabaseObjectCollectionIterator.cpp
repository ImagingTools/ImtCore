#include <imtmongo/CMongoDatabaseObjectCollectionIterator.h>


// ImtCore includes
#include <imtbase/IObjectCollection.h>


namespace imtmongo
{



// public methods

CMongoDatabaseObjectCollectionIterator::CMongoDatabaseObjectCollectionIterator(mongocxx::cursor& cur, IMongoDatabaseObjectDelegate* databaseDelegate)
	:m_cursor(std::move(cur)),
	m_iterator(m_cursor.begin())
{
	m_databaseDelegate = databaseDelegate;
}


// reimplemented (imtbase::IObjectCollectionIterator)

bool CMongoDatabaseObjectCollectionIterator::Next()
{
	if(m_iterator != m_cursor.end()){
		m_iterator++;

		return m_iterator != m_cursor.end();
	}

	return false;
}


bool CMongoDatabaseObjectCollectionIterator::Previous()
{
	return false;
}


QByteArray CMongoDatabaseObjectCollectionIterator::GetObjectId() const
{
	if (m_databaseDelegate == nullptr){
		return QByteArray();
	}

	return 	m_databaseDelegate->GetObjectIdFromRecord(*m_iterator);
}


bool CMongoDatabaseObjectCollectionIterator::GetObjectData(imtbase::IObjectCollection::DataPtr &dataPtr) const
{
	if (m_databaseDelegate == nullptr){
		return false;
	}

	istd::IChangeable* dataObjPtr = m_databaseDelegate->CreateObjectFromRecord(*m_iterator);
	dataPtr = DataPtr(DataPtr::RootObjectPtr(dataObjPtr), [dataObjPtr](){
		return dataObjPtr;
	});

	return dataPtr.IsValid();
}


idoc::MetaInfoPtr CMongoDatabaseObjectCollectionIterator::GetDataMetaInfo() const
{
	if (m_databaseDelegate == nullptr){
		return idoc::MetaInfoPtr();
	}

	idoc::MetaInfoPtr objectMetaInfoPtr;
	idoc::MetaInfoPtr collectionMetaInfoPtr;

	m_databaseDelegate->CreateObjectInfoFromRecord(*m_iterator, objectMetaInfoPtr, collectionMetaInfoPtr);

	return collectionMetaInfoPtr;
}


QVariant CMongoDatabaseObjectCollectionIterator::GetElementInfo(QByteArray infoId) const
{
	if (m_databaseDelegate == nullptr){
		return QVariant();
	}

	return m_databaseDelegate->GetElementInfoFromRecord(*m_iterator, infoId);
}


} // namespace imtmongo


