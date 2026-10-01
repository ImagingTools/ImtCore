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
		For tenant-owned collections this is the shared schema (e.g. for the shared schema migrations);
		queries of tenant-owned data address the tenant storage via s_tenantSchemePrefixPlaceholder.
	*/
	virtual QByteArray GetTableScheme() const = 0;

	/**
		Placeholder for the schema qualifier of the tenant storage in queries built by a tenant-owned delegate.
	*/
	static constexpr const char* s_tenantSchemePrefixPlaceholder = "${TenantSchemePrefix}";

	/**
		Check if the data of the collection is stored in the storage of each tenant (tenant-owned collection).
	*/
	virtual bool HasTenantStorage() const = 0;

	/**
		Replace the tenant storage placeholder in a query built by this delegate with the storage of the given tenant.
		\param tenantId Tenant of the collection the query is executed for (see imtbase::ITenantObjectCollection).
						An empty ID addresses the data without organization (shared storage).
		\return \c false if the query addresses tenant-owned data and the tenant storage cannot be resolved (access denied).
	*/
	virtual bool ApplyTenantStorage(QByteArray& query, const QByteArray& tenantId) const = 0;
};


} // namespace imtdb


