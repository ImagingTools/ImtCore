// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// Qt includes
#include <QtCore/QStringList>

// ACF includes
#include <ilog/TLoggerCompWrap.h>

// ImtCore includes
#include <imtdb/IDatabaseEngine.h>
#include <imtdb/IDatabaseLoginSettings.h>
#include <imtdb/ITenantStorageBackup.h>
#include <imtdb/ITenantStorageResolver.h>


namespace imtdb
{


/**
	Backup and restore of dedicated Postgres tenant schemas using \c pg_dump and \c pg_restore.
	A restore verifies that the archive contains the schema of the requested tenant before anything
	is changed, blocks the tenant (status \c TSS_PROVISIONING) during the restore and keeps the previous
	schema under a temporary name until the restore succeeded; on failure the previous schema is put back.
	All operations are audit-logged (EU CRA, Annex I Part I 2(l)).
*/
class CTenantStorageBackupComp:
			public ilog::CLoggerComponentBase,
			virtual public imtdb::ITenantStorageBackup
{
public:
	typedef ilog::CLoggerComponentBase BaseClass;

	I_BEGIN_COMPONENT(CTenantStorageBackupComp);
		I_REGISTER_INTERFACE(imtdb::ITenantStorageBackup);
		I_ASSIGN(m_databaseEngineCompPtr, "DatabaseEngine", "Database engine of the database containing the tenant schemas", true, "DatabaseEngine");
		I_ASSIGN(m_databaseLoginSettingsCompPtr, "DatabaseLoginSettings", "Login settings used by pg_dump and pg_restore", true, "DatabaseLoginSettings");
		I_ASSIGN(m_storageResolverCompPtr, "StorageResolver", "Tenant storage resolver", true, "TenantStorageResolver");
		I_ASSIGN(m_registryTableSchemaAttrPtr, "RegistryTableSchema", "Schema containing the TenantStorage registry table", false, "");
		I_ASSIGN(m_pgDumpPathAttrPtr, "PgDumpPath", "Path of the pg_dump executable", false, "pg_dump");
		I_ASSIGN(m_pgRestorePathAttrPtr, "PgRestorePath", "Path of the pg_restore executable", false, "pg_restore");
	I_END_COMPONENT;

	// reimplemented (imtdb::ITenantStorageBackup)
	virtual bool BackupTenantStorage(const QByteArray& tenantId, const QString& filePath) override;
	virtual bool RestoreTenantStorage(const QByteArray& tenantId, const QString& filePath) override;

private:
	bool ResolveDedicatedSchema(const QByteArray& tenantId, TenantStorageInfo& storageInfo) const;
	bool ArchiveContainsSchema(const QString& filePath, const QByteArray& schemaName) const;
	bool SetTenantStatus(const QByteArray& tenantId, TenantStorageInfo storageInfo, TenantStorageStatus status) const;
	bool ExecuteStatement(const QByteArray& statement) const;
	bool RunTool(const QString& program, const QStringList& arguments, QByteArray* standardOutputPtr = nullptr) const;
	QStringList GetConnectionArguments() const;

	I_REF(imtdb::IDatabaseEngine, m_databaseEngineCompPtr);
	I_REF(imtdb::IDatabaseLoginSettings, m_databaseLoginSettingsCompPtr);
	I_REF(imtdb::ITenantStorageResolver, m_storageResolverCompPtr);
	I_ATTR(QByteArray, m_registryTableSchemaAttrPtr);
	I_ATTR(QString, m_pgDumpPathAttrPtr);
	I_ATTR(QString, m_pgRestorePathAttrPtr);
};


} // namespace imtdb


