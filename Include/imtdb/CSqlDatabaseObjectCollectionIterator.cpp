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
			ISqlDatabaseObjectDelegate* databaseDelegatePtr)
	:m_currentIndex(-1)
{
	while(sqlQuery.next()){
		m_records.append(sqlQuery.record());
	}

	m_databaseDelegate = databaseDelegatePtr;
}


QSqlRecord CSqlDatabaseObjectCollectionIterator::GetRecord()
{
	if (m_currentIndex > -1){
		return m_records[m_currentIndex];
	}

	return QSqlRecord();
}


// reimplemented (imtbase::IObjectCollectionIterator)

bool CSqlDatabaseObjectCollectionIterator::Next() const
{
	int recordsSize = imtbase::narrow_cast<int>(m_records.size());
	if (m_currentIndex < recordsSize - 1){
		m_currentIndex++;

		return true;
	}

	return false;
}


bool CSqlDatabaseObjectCollectionIterator::Previous() const
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

	return m_databaseDelegate->GetObjectIdFromRecord(m_records[m_currentIndex]);
}


QByteArray CSqlDatabaseObjectCollectionIterator::GetObjectTypeId() const
{
	if (m_databaseDelegate == nullptr || m_currentIndex < 0 || m_currentIndex >= m_records.count()){
		return QByteArray();
	}

	return m_databaseDelegate->GetObjectTypeIdFromRecord(m_records[m_currentIndex]);
}


bool imtdb::CSqlDatabaseObjectCollectionIterator::GetObjectData(imtbase::IObjectCollection::DataPtr& dataPtr) const
{
	if (m_databaseDelegate == nullptr || m_currentIndex < 0 || m_currentIndex >= m_records.count()){
		return false;
	}

	dataPtr.FromUnique(m_databaseDelegate->CreateObjectFromRecord(m_records[m_currentIndex]));

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

	return objectMetaInfoPtr;
}


idoc::MetaInfoPtr CSqlDatabaseObjectCollectionIterator::GetCollectionMetaInfo() const
{
	if (m_databaseDelegate == nullptr || m_currentIndex < 0 || m_currentIndex >= m_records.count()){
		return idoc::MetaInfoPtr();
	}

	idoc::MetaInfoPtr objectMetaInfoPtr;
	idoc::MetaInfoPtr collectionMetaInfoPtr;

	m_databaseDelegate->CreateObjectInfoFromRecord(m_records[m_currentIndex], objectMetaInfoPtr, collectionMetaInfoPtr);

	return collectionMetaInfoPtr;
}


QVariant CSqlDatabaseObjectCollectionIterator::GetElementInfo(int /*infoType*/) const
{
	return QVariant();
}


QVariant CSqlDatabaseObjectCollectionIterator::GetElementInfo(QByteArray infoId) const
{
	if (m_databaseDelegate == nullptr || m_currentIndex < 0 || m_currentIndex >= m_records.count()){
		return QVariant();
	}

	return m_databaseDelegate->GetElementInfoFromRecord(m_records[m_currentIndex], infoId);
}


} // namespace imtdb


