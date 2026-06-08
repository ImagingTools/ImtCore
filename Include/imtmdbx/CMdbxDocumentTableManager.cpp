#include <imtmdbx/CMdbxDocumentTableManager.h>


namespace imtmdbx
{


// public methods

CMdbxDocumentTableManager::CMdbxDocumentTableManager(mdbx::txn_managed& txn, const QHash<QString/*tableName*/, DocumentTableConfig>& tableConfigHash)
	: m_txn(txn),
	  m_tableConfigHash(tableConfigHash)
{
}


DocumentTablePtr CMdbxDocumentTableManager::get(const QString& tableId)
{
	auto it = m_initializedTableHash.find(tableId);
	if (it != m_initializedTableHash.end() && !it.value().isNull()) {
		return it.value();
	}

	if (!m_tableConfigHash.contains(tableId)) {
		return DocumentTablePtr();
	}

	return InitTable(tableId, m_tableConfigHash[tableId]);
}


bool CMdbxDocumentTableManager::remove(const QString& tableId)
{
	return m_initializedTableHash.remove(tableId);
}


void CMdbxDocumentTableManager::reset()
{
	m_initializedTableHash.clear();
}


void CMdbxDocumentTableManager::reopen()
{
	for(auto [key, value] : m_initializedTableHash.asKeyValueRange()){
		if (value.isNull()) {
			continue;
		}

		value->Reopen();
	}
}


bool CMdbxDocumentTableManager::AreInitTablesValid() const{
	if(m_initializedTableHash.isEmpty()){
		return false;
	}

	bool retVal = true;
	for(auto [key, value] : m_initializedTableHash.asKeyValueRange()){
		retVal = retVal && !value.isNull();
	}

	return retVal;
}


const QHash<QString/*tableName*/, CMdbxDocumentTableManager::DocumentTableConfig>& CMdbxDocumentTableManager::getTableConfigHash() const
{
	return m_tableConfigHash;
}


void CMdbxDocumentTableManager::setTableConfigHash(const QHash<QString/*tableName*/, DocumentTableConfig>& tableConfigHash)
{
	m_tableConfigHash = tableConfigHash;
}


void CMdbxDocumentTableManager::addTableConfig(const QString& tableName, const DocumentTableConfig& config)
{
	m_tableConfigHash.insert(tableName, config);
}



} // namespace imtmdbx


