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
	CSqlDatabaseObjectCollectionIterator(QSqlQuery sqlQuery, ISqlDatabaseObjectDelegate* databaseDelegate);

// reimplemented (imtbase::IObjectCollectionIterator)
	virtual bool Next() override;
	virtual bool Previous() override;
	virtual QByteArray GetObjectId() const override;
	virtual bool GetObjectData(imtbase::IObjectCollection::DataPtr& dataPtr) const override;
	virtual idoc::MetaInfoPtr GetDataMetaInfo() const override;
	virtual QVariant GetElementInfo(QByteArray infoId) const override;
private:
	QSqlQuery m_sqlQuery;
	ISqlDatabaseObjectDelegate* m_databaseDelegate;
};


} // namespace imtdb


