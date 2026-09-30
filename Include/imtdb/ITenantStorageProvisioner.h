// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ACF includes
#include <istd/IChangeable.h>

// Qt includes
#include <QtCore/QByteArray>


namespace imtdb
{


/**
	Lifecycle manager for the physical tenant storage.
	Provisions the storage of a tenant (dedicated schema/database and its tables),
	persists the assignment in the storage registry and removes the storage
	again on tenant offboarding.
*/
class ITenantStorageProvisioner: virtual public istd::IChangeable
{
public:
	/**
		Provision the physical storage for the given tenant.
		Creates the storage location (e.g. a dedicated database schema), executes
		the configured DDL scripts and registers the assignment in the storage registry.
		Provisioning is idempotent — an already provisioned tenant reports success.
		\return \c true if the storage is available afterwards.
	*/
	virtual bool ProvisionTenantStorage(const QByteArray& tenantId) = 0;

	/**
		Remove the physical storage assignment of the given tenant (offboarding).
		Depending on the configuration, the storage itself (schema and data) is dropped.
		\return \c true if the assignment was removed.
	*/
	virtual bool DeprovisionTenantStorage(const QByteArray& tenantId) = 0;

	/**
		Load all persisted storage assignments from the registry table
		into the storage resolver.
		\return \c true if the assignments could be loaded.
	*/
	virtual bool LoadTenantStorageAssignments() = 0;
};


} // namespace imtdb
