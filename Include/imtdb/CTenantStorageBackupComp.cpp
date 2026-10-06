// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtdb/CTenantStorageBackupComp.h>


// Qt includes
#include <QtCore/QProcess>
#include <QtCore/QRegularExpression>
#include <QtCore/QUuid>
#include <QtSql/QSqlError>

// ImtCore includes
#include <imtdb/CTenantDataMigrator.h>
#include <imtdb/CTenantStorageDbStore.h>


namespace imtdb
{


// reimplemented (imtdb::ITenantStorageBackup)

bool CTenantStorageBackupComp::BackupTenantStorage(const QByteArray& tenantId, const QString& filePath)
{
	TenantStorageInfo storageInfo;
	if (!ResolveDedicatedSchema(tenantId, storageInfo)){
		return false;
	}

	QStringList arguments = GetConnectionArguments();
	arguments << QStringLiteral("--format=custom");
	arguments << QStringLiteral("--schema=") + QString(storageInfo.schemaName);
	arguments << QStringLiteral("--file=") + filePath;
	arguments << m_databaseLoginSettingsCompPtr->GetDatabaseName();

	if (!RunTool(m_pgDumpPathAttrPtr.IsValid() ? *m_pgDumpPathAttrPtr : QStringLiteral("pg_dump"), arguments)){
		SendErrorMessage(0, QStringLiteral("Backup of the storage of tenant '%1' failed").arg(QString(tenantId)), "CTenantStorageBackupComp");

		return false;
	}

	SendInfoMessage(0, QStringLiteral("Backup of the storage of tenant '%1' (schema '%2') written to '%3'").arg(QString(tenantId), QString(storageInfo.schemaName), filePath), "CTenantStorageBackupComp");

	return true;
}


bool CTenantStorageBackupComp::RestoreTenantStorage(const QByteArray& tenantId, const QString& filePath)
{
	TenantStorageInfo storageInfo;
	if (!ResolveDedicatedSchema(tenantId, storageInfo)){
		return false;
	}

	// checked before anything is changed: a foreign archive would restore nothing after the schema was replaced
	if (!ArchiveContainsSchema(filePath, storageInfo.schemaName)){
		SendErrorMessage(
					0,
					QStringLiteral("Restore of tenant '%1' rejected: the archive '%2' does not contain the schema '%3'").arg(QString(tenantId), filePath, QString(storageInfo.schemaName)),
					"CTenantStorageBackupComp");

		return false;
	}

	if (!SetTenantStatus(tenantId, storageInfo, TSS_PROVISIONING)){
		return false;
	}

	QByteArray quotedSchema = CTenantDataMigrator::QuoteIdentifier(storageInfo.schemaName);
	QByteArray parkedSchemaName = QByteArrayLiteral("imt_restore_") + QUuid::createUuid().toByteArray(QUuid::Id128);
	QByteArray quotedParkedSchema = CTenantDataMigrator::QuoteIdentifier(parkedSchemaName);

	if (!ExecuteStatement(QByteArrayLiteral("ALTER SCHEMA ") + quotedSchema + QByteArrayLiteral(" RENAME TO ") + quotedParkedSchema)){
		SetTenantStatus(tenantId, storageInfo, storageInfo.status);

		return false;
	}

	QStringList arguments = GetConnectionArguments();
	arguments << QStringLiteral("--dbname=") + m_databaseLoginSettingsCompPtr->GetDatabaseName();
	arguments << QStringLiteral("--schema=") + QString(storageInfo.schemaName);
	arguments << QStringLiteral("--no-owner");
	arguments << QStringLiteral("--exit-on-error");
	arguments << QStringLiteral("--single-transaction");
	arguments << filePath;

	// pg_restore --schema does not create the schema itself
	bool isRestored =
				ExecuteStatement(QByteArrayLiteral("CREATE SCHEMA ") + quotedSchema) &&
				RunTool(m_pgRestorePathAttrPtr.IsValid() ? *m_pgRestorePathAttrPtr : QStringLiteral("pg_restore"), arguments);

	if (isRestored){
		ExecuteStatement(QByteArrayLiteral("DROP SCHEMA ") + quotedParkedSchema + QByteArrayLiteral(" CASCADE"));
	}
	else{
		bool isRolledBack =
					ExecuteStatement(QByteArrayLiteral("DROP SCHEMA IF EXISTS ") + quotedSchema + QByteArrayLiteral(" CASCADE")) &&
					ExecuteStatement(QByteArrayLiteral("ALTER SCHEMA ") + quotedParkedSchema + QByteArrayLiteral(" RENAME TO ") + quotedSchema);
		if (!isRolledBack){
			// the tenant stays blocked: its previous data is kept in the parked schema for manual recovery
			SendCriticalMessage(
						0,
						QStringLiteral("Restore of tenant '%1' failed and the previous schema could not be put back, it is kept as '%2'").arg(QString(tenantId), QString(parkedSchemaName)),
						"CTenantStorageBackupComp");

			return false;
		}
	}

	SetTenantStatus(tenantId, storageInfo, storageInfo.status);

	if (!isRestored){
		SendErrorMessage(0, QStringLiteral("Restore of tenant '%1' from '%2' failed, the previous storage content was kept").arg(QString(tenantId), filePath), "CTenantStorageBackupComp");

		return false;
	}

	SendInfoMessage(0, QStringLiteral("Storage of tenant '%1' (schema '%2') restored from '%3'").arg(QString(tenantId), QString(storageInfo.schemaName), filePath), "CTenantStorageBackupComp");

	return true;
}


// private methods

bool CTenantStorageBackupComp::ResolveDedicatedSchema(const QByteArray& tenantId, TenantStorageInfo& storageInfo) const
{
	if (!m_databaseEngineCompPtr->GetDatabaseDriverId().startsWith(QByteArrayLiteral("QPSQL"))){
		SendErrorMessage(0, QStringLiteral("Tenant storage backup is only supported for Postgres databases"), "CTenantStorageBackupComp");

		return false;
	}

	if (tenantId.isEmpty() || !m_storageResolverCompPtr->ResolveTenantStorage(tenantId, storageInfo)){
		SendErrorMessage(0, QStringLiteral("Tenant storage backup failed: storage of tenant '%1' could not be resolved").arg(QString(tenantId)), "CTenantStorageBackupComp");

		return false;
	}

	if ((storageInfo.storageKind != TSK_OWN_SCHEMA) || CTenantDataMigrator::QuoteIdentifier(storageInfo.schemaName).isEmpty()){
		SendErrorMessage(0, QStringLiteral("Tenant storage backup failed: tenant '%1' has no dedicated schema").arg(QString(tenantId)), "CTenantStorageBackupComp");

		return false;
	}

	return true;
}


bool CTenantStorageBackupComp::ArchiveContainsSchema(const QString& filePath, const QByteArray& schemaName) const
{
	QByteArray tableOfContents;
	if (!RunTool(m_pgRestorePathAttrPtr.IsValid() ? *m_pgRestorePathAttrPtr : QStringLiteral("pg_restore"), {QStringLiteral("--list"), filePath}, &tableOfContents)){
		return false;
	}

	// a table of contents entry looks like "6; 2615 1350204 SCHEMA - tenant_x owner"
	QRegularExpression schemaEntry(QStringLiteral("^\\d+; \\d+ \\d+ SCHEMA - %1 ").arg(QRegularExpression::escape(QString(schemaName))), QRegularExpression::MultilineOption);

	return schemaEntry.match(QString::fromUtf8(tableOfContents)).hasMatch();
}


bool CTenantStorageBackupComp::SetTenantStatus(const QByteArray& tenantId, TenantStorageInfo storageInfo, TenantStorageStatus status) const
{
	storageInfo.status = status;

	CTenantStorageDbStore store(*m_databaseEngineCompPtr, m_registryTableSchemaAttrPtr.IsValid() ? *m_registryTableSchemaAttrPtr : QByteArray());
	if (!store.SaveAssignment(tenantId, storageInfo) || !m_storageResolverCompPtr->RegisterTenantStorage(tenantId, storageInfo)){
		SendErrorMessage(0, QStringLiteral("Status of the storage of tenant '%1' could not be updated").arg(QString(tenantId)), "CTenantStorageBackupComp");

		return false;
	}

	return true;
}


bool CTenantStorageBackupComp::ExecuteStatement(const QByteArray& statement) const
{
	QSqlError sqlError;
	m_databaseEngineCompPtr->ExecSqlQuery(statement, &sqlError);

	return sqlError.type() == QSqlError::NoError;
}


bool CTenantStorageBackupComp::RunTool(const QString& program, const QStringList& arguments, QByteArray* standardOutputPtr) const
{
	QProcessEnvironment environment = QProcessEnvironment::systemEnvironment();
	environment.insert(QStringLiteral("PGPASSWORD"), m_databaseLoginSettingsCompPtr->GetPassword());
	if (m_databaseLoginSettingsCompPtr->GetConnectionFlags() & IDatabaseLoginSettings::COF_SSL){
		environment.insert(QStringLiteral("PGSSLMODE"), QStringLiteral("require"));
	}

	QProcess process;
	process.setProcessEnvironment(environment);
	process.start(program, arguments);

	if (!process.waitForFinished(-1)){
		SendErrorMessage(0, QStringLiteral("'%1' could not be executed: %2").arg(program, process.errorString()), "CTenantStorageBackupComp");

		return false;
	}

	if ((process.exitStatus() != QProcess::NormalExit) || (process.exitCode() != 0)){
		SendErrorMessage(0, QStringLiteral("'%1' failed: %2").arg(program, QString::fromUtf8(process.readAllStandardError())), "CTenantStorageBackupComp");

		return false;
	}

	if (standardOutputPtr != nullptr){
		*standardOutputPtr = process.readAllStandardOutput();
	}

	return true;
}


QStringList CTenantStorageBackupComp::GetConnectionArguments() const
{
	return {
				QStringLiteral("--host=") + m_databaseLoginSettingsCompPtr->GetHost(),
				QStringLiteral("--port=") + QString::number(m_databaseLoginSettingsCompPtr->GetPort()),
				QStringLiteral("--username=") + m_databaseLoginSettingsCompPtr->GetUserName(),
				QStringLiteral("--no-password")};
}


} // namespace imtdb


