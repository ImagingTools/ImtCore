#include <imtdb/CSqlDatabaseCollectionStructureIterator.h>


// ImtCore includes
#include <imtbase/IObjectCollection.h>


namespace imtdb
{


// public methods

CSqlDatabaseCollectionStructureIterator::CSqlDatabaseCollectionStructureIterator(QSqlQuery& sqlQuery)
	:m_currentIndex(-1)
{
	while(sqlQuery.next()){
		m_records.append(sqlQuery.record());
	}
}


// reimplemented (imtbase::IHierarchicalStructureIterator)

bool CSqlDatabaseCollectionStructureIterator::Next()
{
	int recordsSize = m_records.size();
	if (m_currentIndex < recordsSize - 1){
		m_currentIndex++;

		return true;
	}

	return false;
}


bool CSqlDatabaseCollectionStructureIterator::Previous()
{
	if (m_currentIndex > 0){
		m_currentIndex--;

		return true;
	}

	return false;
}


imtbase::IHierarchicalStructureInfo::Id CSqlDatabaseCollectionStructureIterator::GetNodeId() const
{
	imtbase::IHierarchicalStructureInfo::Id nodeId = m_records[m_currentIndex].value("Id").toByteArray();

	return nodeId;
}


imtbase::IHierarchicalStructureInfo::ItemInfo CSqlDatabaseCollectionStructureIterator::GetItemInfo() const
{
	imtbase::IHierarchicalStructureInfo::ItemInfo itemInfo;
	itemInfo.itemId = m_records[m_currentIndex].value("Id").toByteArray();
	itemInfo.name = m_records[m_currentIndex].value("Name").toByteArray();
	// itemInfo.description = m_records[m_currentIndex].value("Description").toByteArray();

	return itemInfo;
}


int CSqlDatabaseCollectionStructureIterator::GetItemCount() const
{
	return m_records.count();
}



} // namespace imtdb


