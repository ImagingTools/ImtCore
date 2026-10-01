// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtdb/CTenantStorageProvisionerComp.h>

// Qt includes
#include <QtCore/QFile>
#include <QtSql/QSqlError>
#include <QtSql/QSqlQuery>

// ImtCore includes
#include <imtdb/imtdb.h>
#include <imtdb/CTenantDataMigrator.h>
#include <imtdb/CTenantStorageDbStore.h>
#include <imtdb/CTenantStorageRegistry.h>


namespace imtdb
{


// reimplemented (imtdb::ITenantStorageProvisioner)

bool CTenantStorageProvisionerComp::ProvisionTenantStorage(const QByteArray& tenantId)
{
	if (tenantId.isEmpty()){
		SendErrorMessage(0, QStringLiteral("Tenant storage provisioning rejected: empty tenant ID"), "CTenantStorageProvisionerComp");

		return false;
	}

	if (m_storageResolverCompPtr->IsTenantStorageRegistered(tenantId)){
		// idempotent: already provisioned
		return true;
	}

	TenantStorageInfo info;
	info.status = TSS_ACTIVE;

	// set only if the schema did not exist before, so a failure never drops pre-existing tenant data
	QByteArray createdSchemaName;

	if (IsPostgresDriver()){
		info.storageKind = TSK_OWN_SCHEMA;
		info.schemaName = CTenantStorageRegistry::CreateSchemaName(GetSchemaNamePrefix(), tenantId);

		bool schemaExisted = false;
		if (!SchemaExists(info.schemaName, schemaExisted) || !CreateTenantSchema(info.schemaName)){
			SendErrorMessage(
						0,
						QStringLiteral("Failed to create schema '%1' for tenant '%2'").arg(QString(info.schemaName)).arg(QString(tenantId)),
						"CTenantStorageProvisionerComp");

			return false;
		}

		if (!schemaExisted){
			createdSchemaName = info.schemaName;
		}

		if (!SetDefaultTablespace(GetDefaultTablespace())){
			SendErrorMessage(
						0,
						QStringLiteral("Failed to set the default tablespace '%1' for tenant '%2'").arg(QString(GetDefaultTablespace())).arg(QString(tenantId)),
						"CTenantStorageProvisionerComp");

			DropTenantSchema(createdSchemaName);

			return false;
		}

		bool ddlSucceeded = ExecuteDdlScripts(info.schemaName);

		if (!GetDefaultTablespace().isEmpty()){
			QSqlError sqlError;
			m_databaseEngineCompPtr->ExecSqlQuery(QByteArrayLiteral("RESET default_tablespace"), &sqlError);
		}

		if (!ddlSucceeded){
			SendErrorMessage(
						0,
						QStringLiteral("Failed to execute DDL scripts in schema '%1' for tenant '%2'").arg(QString(info.schemaName)).arg(QString(tenantId)),
						"CTenantStorageProvisionerComp");

			DropTenantSchema(createdSchemaName);

			return false;
		}
	}
	else{
		// dedicated per-tenant database files are provided by a later phase;
		// register an explicit shared-schema assignment to keep fail-closed resolution intact
		info.storageKind = TSK_SHARED_SCHEMA;
	}

	if (!PersistAssignment(tenantId, info)){
		SendErrorMessage(
					0,
					QStringLiteral("Failed to persist storage assignment for tenant '%1'").arg(QString(tenantId)),
					"CTenantStorageProvisionerComp");

		DropTenantSchema(createdSchemaName);

		return false;
	}

	if (!m_storageResolverCompPtr->RegisterTenantStorage(tenantId, info)){
		CTenantStorageDbStore store(*m_databaseEngineCompPtr, GetRegistryTableSchema());
		store.RemoveAssignment(tenantId);

		DropTenantSchema(createdSchemaName);

		return false;
	}

	SendInfoMessage(
				0,
				QStringLiteral("Provisioned storage for tenant '%1' (kind %2, schema '%3')")
					.arg(QString(tenantId))
					.arg(int(info.storageKind))
					.arg(QString(info.schemaName)),
				"CTenantStorageProvisionerComp");

	return true;
}


bool CTenantStorageProvisionerComp::DeprovisionTenantStorage(const QByteArray& tenantId)
{
	if (tenantId.isEmpty()){
		return false;
	}

	if (!m_storageResolverCompPtr->IsTenantStorageRegistered(tenantId)){
		return false;
	}

	TenantStorageInfo info;
	if (!m_storageResolverCompPtr->ResolveTenantStorage(tenantId, info)){
		return false;
	}

	if (IsDropStorageOnDeprovision() && (info.storageKind == TSK_OWN_SCHEMA) && IsPostgresDriver() && !info.schemaName.isEmpty()){
		QByteArray query = QByteArrayLiteral("DROP SCHEMA IF EXISTS \"") + info.schemaName + QByteArrayLiteral("\" CASCADE");

		QSqlError sqlError;
		m_databaseEngineCompPtr->ExecSqlQuery(query, &sqlError);
		if (sqlError.type() != QSqlError::NoError){
			SendErrorMessage(
						0,
						QStringLiteral("Failed to drop schema '%1' of tenant '%2': %3")
							.arg(QString(info.schemaName))
							.arg(QString(tenantId))
							.arg(sqlError.text()),
						"CTenantStorageProvisionerComp");

			return false;
		}

		SendInfoMessage(
					0,
					QStringLiteral("Dropped schema '%1' of tenant '%2'").arg(QString(info.schemaName)).arg(QString(tenantId)),
					"CTenantStorageProvisionerComp");
	}

	CTenantStorageDbStore store(*m_databaseEngineCompPtr, GetRegistryTableSchema());
	if (!store.RemoveAssignment(tenantId)){
		SendErrorMessage(
					0,
					QStringLiteral("Failed to remove persisted storage assignment of tenant '%1'").arg(QString(tenantId)),
					"CTenantStorageProvisionerComp");

		return false;
	}

	m_storageResolverCompPtr->UnregisterTenantStorage(tenantId);

	SendInfoMessage(
				0,
				QStringLiteral("Deprovisioned storage of tenant '%1'").arg(QString(tenantId)),
				"CTenantStorageProvisionerComp");

	return true;
}


bool CTenantStorageProvisionerComp::LoadTenantStorageAssignments()
{
	CTenantStorageDbStore store(*m_databaseEngineCompPtr, GetRegistryTableSchema());

	if (IsAutoCreateRegistryTable() && !store.EnsureRegistryTable()){
		SendErrorMessage(0, QStringLiteral("Failed to create the TenantStorage registry table"), "CTenantStorageProvisionerComp");

		return false;
	}

	CTenantStorageDbStore::Assignments assignments;
	if (!store.LoadAssignments(assignments)){
		SendErrorMessage(0, QStringLiteral("Failed to load tenant storage assignments"), "CTenantStorageProvisionerComp");

		return false;
	}

	int loadedCount = 0;
	for (const CTenantStorageDbStore::Assignment& assignment: assignments){
		if (m_storageResolverCompPtr->RegisterTenantStorage(assignment.first, assignment.second)){
			++loadedCount;
		}
		else{
			SendWarningMessage(
						0,
						QStringLiteral("Skipped invalid persisted storage assignment for tenant '%1'").arg(QString(assignment.first)),
						"CTenantStorageProvisionerComp");
		}
	}

	SendInfoMessage(
				0,
				QStringLiteral("Loaded %1 tenant storage assignment(s)").arg(loadedCount),
				"CTenantStorageProvisionerComp");

	return true;
}


// private methods

QByteArray CTenantStorageProvisionerComp::GetSchemaNamePrefix() const
{
	return m_schemaNamePrefixAttrPtr.IsValid() ? *m_schemaNamePrefixAttrPtr : QByteArrayLiteral("tenant_");
}


QByteArray CTenantStorageProvisionerComp::GetRegistryTableSchema() const
{
	return m_registryTableSchemaAttrPtr.IsValid() ? *m_registryTableSchemaAttrPtr : QByteArray();
}


QByteArray CTenantStorageProvisionerComp::GetDefaultTablespace() const
{
	return m_defaultTablespaceAttrPtr.IsValid() ? *m_defaultTablespaceAttrPtr : QByteArray();
}


bool CTenantStorageProvisionerComp::IsAutoCreateRegistryTable() const
{
	return m_autoCreateRegistryTableAttrPtr.IsValid() ? *m_autoCreateRegistryTableAttrPtr : true;
}


bool CTenantStorageProvisionerComp::IsDropStorageOnDeprovision() const
{
	return m_dropStorageOnDeprovisionAttrPtr.IsValid() ? *m_dropStorageOnDeprovisionAttrPtr : false;
}


bool CTenantStorageProvisionerComp::IsPostgresDriver() const
{
	return m_databaseEngineCompPtr->GetDatabaseDriverId().startsWith(QByteArrayLiteral("QPSQL"));
}


bool CTenantStorageProvisionerComp::SchemaExists(const QByteArray& schemaName, bool& exists) const
{
	exists = false;

	QVariantMap bindValues;
	bindValues[QStringLiteral(":schemaName")] = QString(schemaName);

	QSqlError sqlError;
	QSqlQuery query = m_databaseEngineCompPtr->ExecSqlQuery(
				QByteArrayLiteral("SELECT 1 FROM information_schema.schemata WHERE schema_name = :schemaName"),
				bindValues,
				&sqlError,
				true);
	if (sqlError.type() != QSqlError::NoError){
		return false;
	}

	exists = query.next();

	return true;
}


bool CTenantStorageProvisionerComp::CreateTenantSchema(const QByteArray& schemaName) const
{
	QByteArray query = QByteArrayLiteral("CREATE SCHEMA IF NOT EXISTS \"") + schemaName + '"';

	QSqlError sqlError;
	m_databaseEngineCompPtr->ExecSqlQuery(query, &sqlError);

	return sqlError.type() == QSqlError::NoError;
}


void CTenantStorageProvisionerComp::DropTenantSchema(const QByteArray& schemaName) const
{
	if (schemaName.isEmpty()){
		return;
	}

	QByteArray query = QByteArrayLiteral("DROP SCHEMA IF EXISTS \"") + schemaName + QByteArrayLiteral("\" CASCADE");

	QSqlError sqlError;
	m_databaseEngineCompPtr->ExecSqlQuery(query, &sqlError);
	if (sqlError.type() != QSqlError::NoError){
		SendErrorMessage(
					0,
					QStringLiteral("Rollback of the partially provisioned schema '%1' failed: %2").arg(QString(schemaName), sqlError.text()),
					"CTenantStorageProvisionerComp");
	}
}


bool CTenantStorageProvisionerComp::SetDefaultTablespace(const QByteArray& tablespaceName) const
{
	if (tablespaceName.isEmpty()){
		return true;
	}

	QByteArray quotedTablespace = CTenantDataMigrator::QuoteIdentifier(tablespaceName);
	if (quotedTablespace.isEmpty()){
		return false;
	}

	QByteArray query = QByteArrayLiteral("SET default_tablespace = ") + quotedTablespace;

	QSqlError sqlError;
	m_databaseEngineCompPtr->ExecSqlQuery(query, &sqlError);

	return sqlError.type() == QSqlError::NoError;
}


bool CTenantStorageProvisionerComp::ExecuteDdlScripts(const QByteArray& schemaName) const
{
	for (int index = 0; index < m_ddlScriptPathsAttrPtr.GetCount(); ++index){
		QString resourcePath = m_ddlScriptPathsAttrPtr[index];
		if (resourcePath.isEmpty()){
			continue;
		}

		if (!resourcePath.startsWith(QStringLiteral(":/"))){
			resourcePath = GetSqlResourcePath(*m_databaseEngineCompPtr, resourcePath);
		}

		QFile scriptFile(resourcePath);
		if (!scriptFile.open(QFile::ReadOnly)){
			SendErrorMessage(
						0,
						QStringLiteral("DDL script '%1' could not be loaded").arg(scriptFile.fileName()),
						"CTenantStorageProvisionerComp");

			return false;
		}

		QByteArray ddlQuery = scriptFile.readAll();
		scriptFile.close();

		ddlQuery.replace(QByteArrayLiteral("${TableScheme}"), schemaName);

		QSqlError sqlError;
		m_databaseEngineCompPtr->ExecSqlQuery(ddlQuery, &sqlError);
		if (sqlError.type() != QSqlError::NoError){
			SendErrorMessage(
						0,
						QStringLiteral("DDL script '%1' failed in schema '%2': %3")
							.arg(resourcePath)
							.arg(QString(schemaName))
							.arg(sqlError.text()),
						"CTenantStorageProvisionerComp");

			return false;
		}
	}

	return true;
}


bool CTenantStorageProvisionerComp::PersistAssignment(const QByteArray& tenantId, const TenantStorageInfo& info) const
{
	CTenantStorageDbStore store(*m_databaseEngineCompPtr, GetRegistryTableSchema());

	if (IsAutoCreateRegistryTable() && !store.EnsureRegistryTable()){
		return false;
	}

	return store.SaveAssignment(tenantId, info);
}


} // namespace imtdb
