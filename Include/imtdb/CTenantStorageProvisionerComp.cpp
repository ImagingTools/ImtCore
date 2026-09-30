// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtdb/CTenantStorageProvisionerComp.h>

// Qt includes
#include <QtCore/QFile>
#include <QtSql/QSqlError>

// ImtCore includes
#include <imtdb/imtdb.h>
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

	if (IsPostgresDriver()){
		info.storageKind = TSK_OWN_SCHEMA;
		info.schemaName = CTenantStorageRegistry::CreateSchemaName(*m_schemaNamePrefixAttrPtr, tenantId);

		if (!CreateTenantSchema(info.schemaName)){
			SendErrorMessage(
						0,
						QStringLiteral("Failed to create schema '%1' for tenant '%2'").arg(QString(info.schemaName)).arg(QString(tenantId)),
						"CTenantStorageProvisionerComp");

			return false;
		}

		if (!ExecuteDdlScripts(info.schemaName)){
			SendErrorMessage(
						0,
						QStringLiteral("Failed to execute DDL scripts in schema '%1' for tenant '%2'").arg(QString(info.schemaName)).arg(QString(tenantId)),
						"CTenantStorageProvisionerComp");

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

		return false;
	}

	if (!m_storageResolverCompPtr->RegisterTenantStorage(tenantId, info)){
		CTenantStorageDbStore store(*m_databaseEngineCompPtr, *m_registryTableSchemaAttrPtr);
		store.RemoveAssignment(tenantId);

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

	if (*m_dropStorageOnDeprovisionAttrPtr && (info.storageKind == TSK_OWN_SCHEMA) && IsPostgresDriver() && !info.schemaName.isEmpty()){
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

	CTenantStorageDbStore store(*m_databaseEngineCompPtr, *m_registryTableSchemaAttrPtr);
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
	CTenantStorageDbStore store(*m_databaseEngineCompPtr, *m_registryTableSchemaAttrPtr);

	if (*m_autoCreateRegistryTableAttrPtr && !store.EnsureRegistryTable()){
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

bool CTenantStorageProvisionerComp::IsPostgresDriver() const
{
	return m_databaseEngineCompPtr->GetDatabaseDriverId().startsWith(QByteArrayLiteral("QPSQL"));
}


bool CTenantStorageProvisionerComp::CreateTenantSchema(const QByteArray& schemaName) const
{
	QByteArray query = QByteArrayLiteral("CREATE SCHEMA IF NOT EXISTS \"") + schemaName + '"';

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
	CTenantStorageDbStore store(*m_databaseEngineCompPtr, *m_registryTableSchemaAttrPtr);

	if (*m_autoCreateRegistryTableAttrPtr && !store.EnsureRegistryTable()){
		return false;
	}

	return store.SaveAssignment(tenantId, info);
}


} // namespace imtdb
