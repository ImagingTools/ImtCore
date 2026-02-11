// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
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
	virtual imtbase::IHierarchicalStructureInfo::ItemInfo GetItemInfo() const override;
	virtual int GetItemCount() const override;
private:
	QList<QSqlRecord> m_records;

	int m_currentIndex;
};


} // namespace imtdb


