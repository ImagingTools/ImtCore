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
	CSqlDatabaseObjectCollectionIterator(QSqlQuery& sqlQuery, ISqlDatabaseObjectDelegate* databaseDelegate);

	QSqlRecord GetRecord();

	// reimplemented (imtbase::IObjectCollectionIterator)
	virtual bool Next() const override;
	virtual bool Previous() const override;
	virtual QByteArray GetObjectId() const override;
	virtual QByteArray GetObjectTypeId() const override;
	virtual bool GetObjectData(imtbase::IObjectCollection::DataPtr& dataPtr) const override;
	virtual idoc::MetaInfoPtr GetDataMetaInfo() const override;
	virtual idoc::MetaInfoPtr GetCollectionMetaInfo() const override;
	virtual QVariant GetElementInfo(QByteArray infoId) const override;
private:
	QList<QSqlRecord> m_records;
	ISqlDatabaseObjectDelegate* m_databaseDelegate;

	mutable int m_currentIndex;
};


} // namespace imtdb


