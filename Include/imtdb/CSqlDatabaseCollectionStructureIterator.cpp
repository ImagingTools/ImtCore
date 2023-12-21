#include <imtdb/CSqlDatabaseCollectionStructureIterator.h>


// ImtCore includes
#include <imtbase/IObjectCollection.h>


namespace imtdb
{


// public methods

CSqlDatabaseCollectionStructureIterator::CSqlDatabaseCollectionStructureIterator(
			QSqlQuery& sqlQuery)
	:m_currentIndex(-1)
{
	while(sqlQuery.next()){
		m_records.append(sqlQuery.record());
	}
}


// reimplemented (imtbase::ICollectionStructureIterator)

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


imtbase::ICollectionStructureInfo::Id CSqlDatabaseCollectionStructureIterator::GetNodeId() const
{
	imtbase::ICollectionStructureInfo::Id nodeId = m_records[m_currentIndex].value("Id").toByteArray();

	return nodeId;
}


imtbase::ICollectionStructureInfo::NodeInfo CSqlDatabaseCollectionStructureIterator::GetNodeInfo() const
{
	imtbase::ICollectionStructureInfo::NodeInfo nodeInfo;
	nodeInfo.id = m_records[m_currentIndex].value("Id").toByteArray();
	nodeInfo.name = m_records[m_currentIndex].value("Name").toByteArray();
	nodeInfo.description = m_records[m_currentIndex].value("Description").toByteArray();

	return nodeInfo;
}


} // namespace imtdb


