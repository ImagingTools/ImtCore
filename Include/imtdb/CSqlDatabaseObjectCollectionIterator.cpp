// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtdb/CSqlDatabaseObjectCollectionIterator.h>


// ImtCore includes
#include <imtbase/imtbase.h>
#include <imtbase/IObjectCollection.h>


namespace imtdb
{


// public methods

CSqlDatabaseObjectCollectionIterator::CSqlDatabaseObjectCollectionIterator(
			QSqlQuery& sqlQuery,
			ISqlDatabaseObjectDelegate* databaseDelegatePtr,
			int elementsCount)
	:m_elementsCount(elementsCount),
	m_currentIndex(-1),
	m_databaseDelegate(databaseDelegatePtr)
{
	// Pre-calculate the result size if the driver supports it,
	// otherwise fall back to growing the vector.
	int querySize = sqlQuery.size();  // -1 if driver doesn't support size()
	if (querySize > 0){
		m_records.reserve(querySize);
	}

	while (sqlQuery.next()){
		m_records.append(sqlQuery.record());
	}

	if (m_elementsCount < 0){
		// Try to extract TotalCount from the first record (set by COUNT(*) OVER() in the query).
		// This gives the total count across all pages, not just the current page size.
		if (!m_records.isEmpty()){
			const int fieldIndex = m_records.first().indexOf("TotalCount");
			if (fieldIndex >= 0){
				m_elementsCount = m_records.first().value(fieldIndex).toInt();
			}
			else{
				m_elementsCount = imtbase::narrow_cast<int>(m_records.size());
			}
		}
		else{
			m_elementsCount = 0;
		}
	}
}


QSqlRecord CSqlDatabaseObjectCollectionIterator::GetRecord()
{
	if (m_currentIndex > -1){
		return m_records[m_currentIndex];
	}

	return QSqlRecord();
}


void CSqlDatabaseObjectCollectionIterator::SetElementsCount(int elementsCount)
{
	m_elementsCount = elementsCount;
}


// reimplemented (imtbase::IObjectCollectionIterator)

bool CSqlDatabaseObjectCollectionIterator::Next() const
{
	int recordsSize = imtbase::narrow_cast<int>(m_records.size());
	if (m_currentIndex < recordsSize - 1){
		m_currentIndex++;

		// Resolve lightweight cache (objectId, objectTypeId) immediately.
		// MetaInfo is resolved lazily on first access.
		ResolveCurrentCache();

		return true;
	}

	return false;
}


bool CSqlDatabaseObjectCollectionIterator::Previous() const
{
	if (m_currentIndex > 0){
		m_currentIndex--;

		ResolveCurrentCache();

		return true;
	}

	return false;
}


QByteArray CSqlDatabaseObjectCollectionIterator::GetObjectId() const
{
	if (m_currentIndex < 0 || m_currentIndex >= m_records.size()){
		return QByteArray();
	}

	return m_cache.objectId;
}


QByteArray CSqlDatabaseObjectCollectionIterator::GetObjectTypeId() const
{
	if (m_currentIndex < 0 || m_currentIndex >= m_records.size()){
		return QByteArray();
	}

	return m_cache.objectTypeId;
}


bool CSqlDatabaseObjectCollectionIterator::GetObjectData(imtbase::IObjectCollection::DataPtr& dataPtr) const
{
	if (m_databaseDelegate == nullptr || m_currentIndex < 0 || m_currentIndex >= m_records.size()){
		return false;
	}

	dataPtr.FromUnique(m_databaseDelegate->CreateObjectFromRecord(m_records[m_currentIndex]));

	return dataPtr.IsValid();
}


idoc::MetaInfoPtr CSqlDatabaseObjectCollectionIterator::GetDataMetaInfo() const
{
	if (m_databaseDelegate == nullptr || m_currentIndex < 0 || m_currentIndex >= m_records.size()){
		return idoc::MetaInfoPtr();
	}

	ResolveMetaInfoCache();

	return m_cache.objectMetaInfo;
}


int CSqlDatabaseObjectCollectionIterator::GetElementsCount() const
{
	return m_elementsCount;
}


idoc::MetaInfoPtr CSqlDatabaseObjectCollectionIterator::GetCollectionMetaInfo() const
{
	if (m_databaseDelegate == nullptr || m_currentIndex < 0 || m_currentIndex >= m_records.size()){
		return idoc::MetaInfoPtr();
	}

	ResolveMetaInfoCache();

	return m_cache.collectionMetaInfo;
}


QVariant CSqlDatabaseObjectCollectionIterator::GetElementInfo(int /*infoType*/) const
{
	return QVariant();
}


QVariant CSqlDatabaseObjectCollectionIterator::GetElementInfo(QByteArray infoId) const
{
	if (m_databaseDelegate == nullptr || m_currentIndex < 0 || m_currentIndex >= m_records.size()){
		return QVariant();
	}

	return m_databaseDelegate->GetElementInfoFromRecord(m_records[m_currentIndex], infoId);
}


// private methods

void CSqlDatabaseObjectCollectionIterator::ResolveCurrentCache() const
{
	// Reset the cache for the new position
	m_cache.metaInfoResolved = false;
	m_cache.objectMetaInfo.Reset();
	m_cache.collectionMetaInfo.Reset();

	if (m_databaseDelegate == nullptr || m_currentIndex < 0 || m_currentIndex >= m_records.size()){
		m_cache.objectId.clear();
		m_cache.objectTypeId.clear();
		return;
	}

	const QSqlRecord& record = m_records[m_currentIndex];

	// Extract objectId and objectTypeId once per record instead of
	// calling delegate->GetObjectIdFromRecord (which does record.contains +
	// record.value = two O(n) field lookups) every time GetObjectId() is called.
	m_cache.objectId = m_databaseDelegate->GetObjectIdFromRecord(record);
	m_cache.objectTypeId = m_databaseDelegate->GetObjectTypeIdFromRecord(record);
}


void CSqlDatabaseObjectCollectionIterator::ResolveMetaInfoCache() const
{
	if (m_cache.metaInfoResolved){
		return;
	}

	m_cache.metaInfoResolved = true;

	if (m_databaseDelegate == nullptr || m_currentIndex < 0 || m_currentIndex >= m_records.size()){
		return;
	}

	// CreateObjectInfoFromRecord creates BOTH objectMetaInfo and collectionMetaInfo
	// in a single call. Previously, GetDataMetaInfo() and GetCollectionMetaInfo()
	// each called this independently, creating two objects and discarding one.
	// Now we call it once and cache both results.
	m_databaseDelegate->CreateObjectInfoFromRecord(
				m_records[m_currentIndex],
				m_cache.objectMetaInfo,
				m_cache.collectionMetaInfo);
}


} // namespace imtdb
