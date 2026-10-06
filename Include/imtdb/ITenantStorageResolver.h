// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ACF includes
#include <istd/IChangeable.h>

// Qt includes
#include <QtCore/QByteArray>
#include <QtCore/QByteArrayList>


namespace imtdb
{


/**
	Kind of the physical storage assigned to a tenant.
*/
enum TenantStorageKind
{
	/**
		Tenant data is stored in the shared schema together with other tenants (legacy mode).
	*/
	TSK_SHARED_SCHEMA = 0,

	/**
		Tenant data is stored in a dedicated schema of the shared database.
	*/
	TSK_OWN_SCHEMA = 1,

	/**
		Tenant data is stored in a dedicated database.
	*/
	TSK_OWN_DATABASE = 2,

	/**
		Tenant data is stored in a dedicated file (e.g. SQLite database file).
	*/
	TSK_OWN_FILE = 3
};


/**
	Provisioning status of the tenant storage.
*/
enum TenantStorageStatus
{
	TSS_UNKNOWN = 0,
	TSS_PROVISIONING = 1,
	TSS_ACTIVE = 2,
	TSS_MIGRATING = 3,
	TSS_ARCHIVED = 4
};


/**
	Physical storage assignment of a single tenant.
*/
struct TenantStorageInfo
{
	TenantStorageKind storageKind = TSK_SHARED_SCHEMA;
	TenantStorageStatus status = TSS_UNKNOWN;

	/**
		Name of the database schema containing the tenant data.
	*/
	QByteArray schemaName;

	/**
		Reference to the database connection (connection ID or file path).
		Empty for the default shared connection.
	*/
	QByteArray connectionRef;
};


/**
	Resolver mapping a tenant to its physical storage location.
	Resolution is fail-closed: unknown tenants are rejected unless
	the implementation is explicitly configured to fall back to the shared storage.
*/
class ITenantStorageResolver: virtual public istd::IChangeable
{
public:
	/**
		Resolve the physical storage assignment for the given tenant.
		\param tenantId ID of the tenant. Must not be empty.
		\param result Receives the storage assignment on success.
		\return \c true if the tenant could be resolved, \c false otherwise (fail-closed).
	*/
	virtual bool ResolveTenantStorage(const QByteArray& tenantId, TenantStorageInfo& result) const = 0;

	/**
		Register or update the storage assignment for a tenant.
		\return \c true if the assignment was accepted.
	*/
	virtual bool RegisterTenantStorage(const QByteArray& tenantId, const TenantStorageInfo& info) = 0;

	/**
		Remove the storage assignment of a tenant (e.g. on tenant offboarding).
		\return \c true if an assignment existed and was removed.
	*/
	virtual bool UnregisterTenantStorage(const QByteArray& tenantId) = 0;

	/**
		Check if an explicit storage assignment exists for the given tenant.
	*/
	virtual bool IsTenantStorageRegistered(const QByteArray& tenantId) const = 0;

	/**
		Get IDs of all tenants with an explicit storage assignment.
	*/
	virtual QByteArrayList GetRegisteredTenantIds() const = 0;
};


} // namespace imtdb
