#pragma once


// Qt includes
#include <QtSql/QSqlRecord>

// ImtCore includes
#include <imtdb/IDatabaseObjectDelegate.h>


namespace imtdb
{


/**
	Common interface for a SQL-based database object delegate.
*/
class ISqlDatabaseObjectDelegate: virtual public imtdb::IDatabaseObjectDelegate
{
public:
	/**
		Get ID of the object in the database from the SQL record.
	*/
	virtual QByteArray GetObjectIdFromRecord(const QByteArray& typeId, const QSqlRecord& record) const = 0;

	/**
		Create object meta-informations based on the SQL record.
	*/
	virtual bool CreateObjectInfoFromRecord(
				const QByteArray& typeId,
				const QSqlRecord& record,
				imtbase::IMetaInfoCreator::MetaInfoPtr& objectMetaInfoPtr,
				imtbase::IMetaInfoCreator::MetaInfoPtr& collectionItemMetaInfoPtr) const = 0;

	/**
		Create a data object for the given SQL record.
	*/
	virtual istd::IChangeable* CreateObjectFromRecord(
				const QByteArray& typeId,
				const QSqlRecord& record,
				const QSqlQuery& query) const = 0;

};


} // namespace imtdb


