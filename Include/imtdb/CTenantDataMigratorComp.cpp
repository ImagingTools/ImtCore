// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtdb/CTenantDataMigratorComp.h>


// ImtCore includes
#include <imtdb/CTenantDataMigrator.h>
#include <imtdb/CTenantStorageDbStore.h>


namespace imtdb
{


// reimplemented (imtdb::ITenantDataMigrator)

bool CTenantDataMigratorComp::MigrateTenantData(const QByteArray& tenantId)
{
	if (tenantId.isEmpty()){
		SendErrorMessage(0, QStringLiteral("Tenant data migration rejected: empty tenant ID"), "CTenantDataMigratorComp");

		return false;
	}

	TenantStorageInfo storageInfo;
	if (!m_storageResolverCompPtr->ResolveTenantStorage(tenantId, storageInfo)){
		SendErrorMessage(0, QStringLiteral("Tenant data migration failed: storage of tenant '%1' could not be resolved").arg(QString(tenantId)), "CTenantDataMigratorComp");

		return false;
	}

	if (storageInfo.storageKind != TSK_OWN_SCHEMA || storageInfo.schemaName.isEmpty()){
		SendErrorMessage(0, QStringLiteral("Tenant data migration failed: tenant '%1' has no dedicated schema").arg(QString(tenantId)), "CTenantDataMigratorComp");

		return false;
	}

	if (storageInfo.status != TSS_ACTIVE && storageInfo.status != TSS_MIGRATING){
		SendErrorMessage(0, QStringLiteral("Tenant data migration failed: storage of tenant '%1' is not active").arg(QString(tenantId)), "CTenantDataMigratorComp");

		return false;
	}

	TenantStorageStatus previousStatus = storageInfo.status;

	storageInfo.status = TSS_MIGRATING;
	if (!PersistStatus(tenantId, storageInfo) || !m_storageResolverCompPtr->RegisterTenantStorage(tenantId, storageInfo)){
		SendErrorMessage(0, QStringLiteral("Tenant data migration failed: status of tenant '%1' could not be updated").arg(QString(tenantId)), "CTenantDataMigratorComp");

		return false;
	}

	CTenantDataMigrator migrator(*m_databaseEngineCompPtr);
	migrator.SetSourceSchema(m_sourceSchemaAttrPtr.IsValid() ? *m_sourceSchemaAttrPtr : QByteArrayLiteral("public"));
	migrator.SetTenantIdColumn(m_tenantIdColumnAttrPtr.IsValid() ? *m_tenantIdColumnAttrPtr : QByteArrayLiteral("TenantId"));
	migrator.SetChecksumVerificationEnabled(m_verifyChecksumsAttrPtr.IsValid() && *m_verifyChecksumsAttrPtr);

	for (int tableIndex = 0; tableIndex < m_tableNamesAttrPtr.GetCount(); ++tableIndex){
		QByteArray tableName = m_tableNamesAttrPtr[tableIndex];

		int migratedRowCount = 0;
		QString errorMessage;
		if (!migrator.MigrateTable(tableName, tenantId, storageInfo.schemaName, migratedRowCount, errorMessage)){
			SendErrorMessage(0, QStringLiteral("Tenant data migration for tenant '%1' failed: %2").arg(QString(tenantId), errorMessage), "CTenantDataMigratorComp");

			storageInfo.status = previousStatus;
			PersistStatus(tenantId, storageInfo);
			m_storageResolverCompPtr->RegisterTenantStorage(tenantId, storageInfo);

			return false;
		}

		SendInfoMessage(0, QStringLiteral("Migrated %1 rows of table '%2' for tenant '%3' into schema '%4'").arg(migratedRowCount).arg(QString(tableName), QString(tenantId), QString(storageInfo.schemaName)), "CTenantDataMigratorComp");
	}

	if (m_purgeSourceRowsAttrPtr.IsValid() && *m_purgeSourceRowsAttrPtr){
		for (int tableIndex = 0; tableIndex < m_tableNamesAttrPtr.GetCount(); ++tableIndex){
			QByteArray tableName = m_tableNamesAttrPtr[tableIndex];

			QString errorMessage;
			if (!migrator.RemoveSourceRows(tableName, tenantId, errorMessage)){
				SendWarningMessage(0, QStringLiteral("Cleanup of migrated rows for tenant '%1' failed: %2").arg(QString(tenantId), errorMessage), "CTenantDataMigratorComp");
			}
		}
	}

	storageInfo.status = TSS_ACTIVE;
	if (!PersistStatus(tenantId, storageInfo) || !m_storageResolverCompPtr->RegisterTenantStorage(tenantId, storageInfo)){
		SendErrorMessage(0, QStringLiteral("Tenant data migration for tenant '%1' completed, but the status could not be set to active").arg(QString(tenantId)), "CTenantDataMigratorComp");

		return false;
	}

	SendInfoMessage(0, QStringLiteral("Tenant data migration for tenant '%1' completed").arg(QString(tenantId)), "CTenantDataMigratorComp");

	return true;
}


bool CTenantDataMigratorComp::MigrateAllTenants()
{
	bool retVal = true;

	QByteArrayList tenantIds = m_storageResolverCompPtr->GetRegisteredTenantIds();
	for (const QByteArray& tenantId: tenantIds){
		TenantStorageInfo storageInfo;
		if (!m_storageResolverCompPtr->ResolveTenantStorage(tenantId, storageInfo)){
			continue;
		}

		if (storageInfo.storageKind != TSK_OWN_SCHEMA){
			// shared-storage tenants have nothing to migrate
			continue;
		}

		if (!MigrateTenantData(tenantId)){
			retVal = false;
		}
	}

	return retVal;
}


// private methods

bool CTenantDataMigratorComp::PersistStatus(const QByteArray& tenantId, const TenantStorageInfo& info) const
{
	CTenantStorageDbStore dbStore(*m_databaseEngineCompPtr, m_registryTableSchemaAttrPtr.IsValid() ? *m_registryTableSchemaAttrPtr : QByteArray());

	return dbStore.SaveAssignment(tenantId, info);
}


} // namespace imtdb
