// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// Qt includes
#include <QtSql/QtSql>

// ImtCore includes
#include <imtbase/IObjectCollectionIterator.h>
#include <imtdb/ISqlDatabaseObjectDelegate.h>


namespace imtdb
{


/**
	Common interface for a object collection query.
	\ingroup Collection
*/
class CSqlDatabaseObjectCollectionIterator: virtual public imtbase::IObjectCollectionIterator
{
	typedef imtbase::IObjectCollection::DataPtr DataPtr;
public:
	CSqlDatabaseObjectCollectionIterator(QSqlQuery& sqlQuery, ISqlDatabaseObjectDelegate* databaseDelegate, int elementsCount = -1);

	QSqlRecord GetRecord();
	void SetElementsCount(int elementsCount);

	// reimplemented (imtbase::IObjectCollectionIterator)
	virtual bool Next() const override;
	virtual bool Previous() const override;
	virtual QByteArray GetObjectId() const override;
	virtual QByteArray GetObjectTypeId() const override;
	virtual bool GetObjectData(imtbase::IObjectCollection::DataPtr& dataPtr) const override;
	virtual idoc::MetaInfoPtr GetDataMetaInfo() const override;
	virtual int GetElementsCount() const override;
	virtual idoc::MetaInfoPtr GetCollectionMetaInfo() const override;
	virtual QVariant GetElementInfo(int infoType) const override;
	virtual QVariant GetElementInfo(QByteArray infoId) const override;
private:
	/**
		Cached data extracted once per record in Next()/Previous()
		to avoid repeated delegate calls and QSqlRecord field lookups.
	*/
	struct CachedRecordData
	{
		QByteArray objectId;
		QByteArray objectTypeId;
		idoc::MetaInfoPtr objectMetaInfo;
		idoc::MetaInfoPtr collectionMetaInfo;
		bool metaInfoResolved = false;
	};

	void ResolveCurrentCache() const;
	void ResolveMetaInfoCache() const;

	QVector<QSqlRecord> m_records;
	ISqlDatabaseObjectDelegate* m_databaseDelegate;

	int m_elementsCount;
	mutable int m_currentIndex;
	mutable CachedRecordData m_cache;
};


} // namespace imtdb
