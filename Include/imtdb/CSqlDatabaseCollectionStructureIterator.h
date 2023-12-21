#pragma once


// Qt includes
#include <QtSql/QtSql>

// ImtCore includes
#include <imtbase/ICollectionStructureIterator.h>
#include <imtdb/ISqlDatabaseObjectDelegate.h>


namespace imtdb
{


/**
	Common interface for a object collection query.
	\ingroup Collection
*/
class CSqlDatabaseCollectionStructureIterator: virtual public imtbase::ICollectionStructureIterator
{
	typedef imtbase::IObjectCollection::DataPtr DataPtr;
public:
	CSqlDatabaseCollectionStructureIterator(QSqlQuery& sqlQuery);

	// reimplemented (imtbase::IObjectCollectionIterator)
	virtual bool Next() override;
	virtual bool Previous() override;
	virtual imtbase::ICollectionStructureInfo::Id GetNodeId() const override;
	virtual imtbase::ICollectionStructureInfo::NodeInfo GetNodeInfo() const override;
private:
	QList<QSqlRecord> m_records;

	int m_currentIndex;
};


} // namespace imtdb


