#pragma once


// Qt includes
#include <QtSql/QtSql>

// ImtCore includes
#include <imtbase/IHierarchicalStructureIterator.h>
#include <imtdb/ISqlDatabaseObjectDelegate.h>


namespace imtdb
{


/**
	Common interface for a object collection query.
	\ingroup Collection
*/
class CSqlDatabaseCollectionStructureIterator: virtual public imtbase::IHierarchicalStructureIterator
{
	typedef imtbase::IObjectCollection::DataPtr DataPtr;
public:
	CSqlDatabaseCollectionStructureIterator(QSqlQuery& sqlQuery);

	// reimplemented (imtbase::IObjectCollectionIterator)
	virtual bool Next() override;
	virtual bool Previous() override;
	virtual imtbase::IHierarchicalStructureInfo::Id GetNodeId() const override;
	virtual imtbase::IHierarchicalStructureInfo::NodeInfo GetNodeInfo() const override;
private:
	QList<QSqlRecord> m_records;

	int m_currentIndex;
};


} // namespace imtdb


