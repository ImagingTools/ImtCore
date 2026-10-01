// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ACF includes
#include <ifile/IFileNameParam.h>
#include <ilog/TLoggerCompWrap.h>

// ImtCore includes
#include <imtdb/ITenantDataMigrator.h>
#include <imtdb/ITenantStorageResolver.h>
#include <imtdb/IDatabaseEngine.h>


namespace imtdb
{


/**
	Component migrating existing tenant data from the shared schema
	into the dedicated per-tenant schemas resolved via \c ITenantStorageResolver.
*/
class CTenantDataMigratorComp:
			public ilog::CLoggerComponentBase,
			virtual public imtdb::ITenantDataMigrator
{
public:
	typedef ilog::CLoggerComponentBase BaseClass;

	I_BEGIN_COMPONENT(CTenantDataMigratorComp)
		I_REGISTER_INTERFACE(imtdb::ITenantDataMigrator);
		I_ASSIGN(m_databaseEngineCompPtr, "DatabaseEngine", "Database engine for SQL queries", true, "DatabaseEngine");
		I_ASSIGN(m_storageResolverCompPtr, "StorageResolver", "Tenant storage resolver providing the migration targets", true, "TenantStorageResolver");
		I_ASSIGN_MULTI_0(m_tableNamesAttrPtr, "TableNames", "Names of the shared tables whose tenant rows are migrated into the tenant schema", true);
		I_ASSIGN(m_sourceSchemaAttrPtr, "SourceSchema", "Schema containing the shared source tables", false, "public");
		I_ASSIGN(m_tenantIdColumnAttrPtr, "TenantIdColumn", "Name of the column containing the tenant ID in the shared tables", false, "TenantId");
		I_ASSIGN(m_registryTableSchemaAttrPtr, "RegistryTableSchema", "Schema containing the TenantStorage registry table", false, "");
		I_ASSIGN(m_purgeSourceRowsAttrPtr, "PurgeSourceRowsAfterMigration", "Remove the migrated rows from the shared tables after successful verification", false, false);
		I_ASSIGN(m_verifyChecksumsAttrPtr, "VerifyChecksums", "Verify the migrated rows by a content checksum in addition to the row count (Postgres only)", false, false);
		I_ASSIGN_MULTI_0(m_fileDocumentTableNamesAttrPtr, "FileDocumentTableNames", "Tables of TableNames written by a file document delegate; the content files referenced by the migrated rows are copied into the tenant store", false);
		I_ASSIGN(m_fileStorageRootCompPtr, "FileStorageRoot", "Root folder of the shared file document store (required with FileDocumentTableNames)", false, "FileStorageRoot");
	I_END_COMPONENT;

	// reimplemented (imtdb::ITenantDataMigrator)
	virtual bool MigrateTenantData(const QByteArray& tenantId) override;
	virtual bool MigrateAllTenants() override;

private:
	bool PersistStatus(const QByteArray& tenantId, const TenantStorageInfo& info) const;
	bool CopyReferencedFiles(const QByteArray& tableName, const QByteArray& schemaName, QString& errorMessage) const;

	I_REF(imtdb::IDatabaseEngine, m_databaseEngineCompPtr);
	I_REF(imtdb::ITenantStorageResolver, m_storageResolverCompPtr);
	I_MULTIATTR(QByteArray, m_tableNamesAttrPtr);
	I_ATTR(QByteArray, m_sourceSchemaAttrPtr);
	I_ATTR(QByteArray, m_tenantIdColumnAttrPtr);
	I_ATTR(QByteArray, m_registryTableSchemaAttrPtr);
	I_ATTR(bool, m_purgeSourceRowsAttrPtr);
	I_ATTR(bool, m_verifyChecksumsAttrPtr);
	I_MULTIATTR(QByteArray, m_fileDocumentTableNamesAttrPtr);
	I_REF(ifile::IFileNameParam, m_fileStorageRootCompPtr);
};


} // namespace imtdb
