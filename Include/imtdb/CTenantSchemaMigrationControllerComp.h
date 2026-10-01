// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ACF includes
#include <ilog/TLoggerCompWrap.h>

// ImtCore includes
#include <imtdb/IDatabaseEngine.h>
#include <imtdb/IMigrationController.h>
#include <imtdb/ITenantStorageResolver.h>


namespace imtdb
{


/**
	Migration controller applying the migrations of the tenant-owned tables to every dedicated tenant schema.
	The wrapped controller is executed once per tenant with a dedicated schema (\c TSK_OWN_SCHEMA),
	inside the tenant context of that tenant and, for Postgres, with the tenant schema at the head of the
	\c search_path. Tenant-aware delegates therefore resolve their table schema to the tenant schema and
	unqualified table names in SQL migration scripts address the tenant tables.
	The database engine runs all migrations in a single transaction, so a failure in any tenant schema
	rolls back the migration of all tenants and the shared revision stays consistent.
	Can be combined with the migrations of the shared catalog via \c CCompositeMigrationControllerComp.
*/
class CTenantSchemaMigrationControllerComp:
			public ilog::CLoggerComponentBase,
			virtual public imtdb::IMigrationController
{
public:
	typedef ilog::CLoggerComponentBase BaseClass;

	I_BEGIN_COMPONENT(CTenantSchemaMigrationControllerComp);
		I_REGISTER_INTERFACE(imtdb::IMigrationController);
		I_ASSIGN(m_databaseEngineCompPtr, "DatabaseEngine", "Database engine of the shared catalog", true, "DatabaseEngine");
		I_ASSIGN(m_tenantMigrationControllerCompPtr, "TenantMigrationController", "Migration controller executed for each tenant schema", true, "TenantMigrationController");
		I_ASSIGN(m_storageResolverCompPtr, "StorageResolver", "Optional tenant storage resolver updated with the persisted assignments before the migration (required if the wrapped controller uses tenant-aware delegates)", false, "TenantStorageResolver");
		I_ASSIGN(m_registryTableSchemaAttrPtr, "RegistryTableSchema", "Schema containing the TenantStorage registry table", false, "");
	I_END_COMPONENT;

	// reimplemented (imtdb::IMigrationController)
	virtual istd::CIntRange GetMigrationRange() const override;
	virtual bool DoMigration(int& resultRevision, const istd::CIntRange& subRange = istd::CIntRange()) const override;

private:
	bool IsPostgresDriver() const;
	bool MigrateTenantSchema(const QByteArray& tenantId, const QByteArray& schemaName, int& resultRevision, const istd::CIntRange& subRange) const;

	I_REF(imtdb::IDatabaseEngine, m_databaseEngineCompPtr);
	I_REF(imtdb::IMigrationController, m_tenantMigrationControllerCompPtr);
	I_REF(imtdb::ITenantStorageResolver, m_storageResolverCompPtr);
	I_ATTR(QByteArray, m_registryTableSchemaAttrPtr);
};


} // namespace imtdb


