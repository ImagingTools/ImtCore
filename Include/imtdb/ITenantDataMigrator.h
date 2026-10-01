// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// Qt includes
#include <QtCore/QByteArray>

// ACF includes
#include <istd/IChangeable.h>


namespace imtdb
{


/**
	Migrator copying existing tenant data from the shared storage
	into the dedicated physical storage of the tenant.
*/
class ITenantDataMigrator: virtual public istd::IChangeable
{
public:
	/**
		Migrate the data of a single tenant from the shared storage
		into its dedicated storage.
		The operation is idempotent: already migrated tables are skipped.
		\param tenantId ID of the tenant. Must not be empty.
		\return \c true if all configured tables were migrated and verified.
	*/
	virtual bool MigrateTenantData(const QByteArray& tenantId) = 0;

	/**
		Migrate the data of all tenants with a registered storage assignment.
		\return \c true if the migration succeeded for every tenant.
	*/
	virtual bool MigrateAllTenants() = 0;
};


} // namespace imtdb
