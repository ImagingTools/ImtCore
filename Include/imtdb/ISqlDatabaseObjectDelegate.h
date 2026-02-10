// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// Qt includes
#include <QtSql/QSqlRecord>

// ImtCore includes
#include <imtbase/IMetaInfoCreator.h>
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
	virtual QByteArray GetObjectIdFromRecord(const QSqlRecord& record) const = 0;

	/**
		Get type-ID of the object in the database from the SQL record.
	*/
	virtual QByteArray GetObjectTypeIdFromRecord(const QSqlRecord& record) const = 0;

	/**
		Create object meta-informations based on the SQL record.
	*/
	virtual bool CreateObjectInfoFromRecord(
				const QSqlRecord& record,
				idoc::MetaInfoPtr& objectMetaInfoPtr,
				idoc::MetaInfoPtr& collectionItemMetaInfoPtr) const = 0;

	/**
		Create a data object for the given SQL record.
	*/
	virtual istd::IChangeableUniquePtr CreateObjectFromRecord(
		const QSqlRecord& record,
		const iprm::IParamsSet* paramsPtr = nullptr) const = 0;

	/**
		Create object meta-information element based on the SQL record.
	*/
	virtual QVariant GetElementInfoFromRecord(const QSqlRecord& record, const QByteArray& infoId) const = 0;

	/**
		Create query for the updating the meta info by SQL record.
	*/
	virtual QByteArray CreateUpdateMetaInfoQuery(const QSqlRecord& record) const = 0;

	/**
		Get name of the collection table in the SQL database.
	*/
	virtual QByteArray GetTableName() const = 0;

	/**
		Get scheme of the collection table in the SQL database.
	*/
	virtual QByteArray GetTableScheme() const = 0;
};


} // namespace imtdb


