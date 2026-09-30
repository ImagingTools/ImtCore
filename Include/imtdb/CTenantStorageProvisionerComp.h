// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ACF includes
#include <ilog/TLoggerCompWrap.h>

// ImtCore includes
#include <imtdb/IDatabaseEngine.h>
#include <imtdb/ITenantStorageProvisioner.h>
#include <imtdb/ITenantStorageResolver.h>


namespace imtdb
{


/**
	Lifecycle manager provisioning the physical storage of a tenant.
	For PostgreSQL a dedicated schema is created per tenant and the configured
	DDL scripts are executed inside it (using the \c ${TableScheme} placeholder).
	For other backends an explicit shared-schema assignment is registered
	(dedicated per-tenant database files are provided by a later phase).
	All lifecycle events are audit-logged (EU CRA, Annex I Part I 2(l)).
*/
class CTenantStorageProvisionerComp:
			public ilog::CLoggerComponentBase,
			virtual public imtdb::ITenantStorageProvisioner
{
public:
	typedef ilog::CLoggerComponentBase BaseClass;

	I_BEGIN_COMPONENT(CTenantStorageProvisionerComp);
		I_REGISTER_INTERFACE(imtdb::ITenantStorageProvisioner);
		I_ASSIGN(m_databaseEngineCompPtr, "DatabaseEngine", "Database engine for SQL queries", true, "DatabaseEngine");
		I_ASSIGN(m_storageResolverCompPtr, "StorageResolver", "Tenant storage resolver receiving the provisioned assignments", true, "TenantStorageResolver");
		I_ASSIGN_MULTI_0(m_ddlScriptPathsAttrPtr, "DdlScriptPaths", "QRC paths or file names of SQL scripts executed inside a newly provisioned tenant schema", false);
		I_ASSIGN(m_schemaNamePrefixAttrPtr, "SchemaNamePrefix", "Prefix used to derive dedicated tenant schema names", false, "tenant_");
		I_ASSIGN(m_registryTableSchemaAttrPtr, "RegistryTableSchema", "Schema containing the TenantStorage registry table", false, "");
		I_ASSIGN(m_autoCreateRegistryTableAttrPtr, "AutoCreateRegistryTable", "Create the TenantStorage registry table if it does not exist", false, true);
		I_ASSIGN(m_dropStorageOnDeprovisionAttrPtr, "DropStorageOnDeprovision", "Drop the dedicated tenant schema including all data on deprovisioning", false, false);
	I_END_COMPONENT;

	// reimplemented (imtdb::ITenantStorageProvisioner)
	virtual bool ProvisionTenantStorage(const QByteArray& tenantId) override;
	virtual bool DeprovisionTenantStorage(const QByteArray& tenantId) override;
	virtual bool LoadTenantStorageAssignments() override;

private:
	bool IsPostgresDriver() const;
	bool CreateTenantSchema(const QByteArray& schemaName) const;
	bool ExecuteDdlScripts(const QByteArray& schemaName) const;
	bool PersistAssignment(const QByteArray& tenantId, const TenantStorageInfo& info) const;

	I_REF(imtdb::IDatabaseEngine, m_databaseEngineCompPtr);
	I_REF(imtdb::ITenantStorageResolver, m_storageResolverCompPtr);
	I_MULTIATTR(QString, m_ddlScriptPathsAttrPtr);
	I_ATTR(QByteArray, m_schemaNamePrefixAttrPtr);
	I_ATTR(QByteArray, m_registryTableSchemaAttrPtr);
	I_ATTR(bool, m_autoCreateRegistryTableAttrPtr);
	I_ATTR(bool, m_dropStorageOnDeprovisionAttrPtr);
};


} // namespace imtdb
