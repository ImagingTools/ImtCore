// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtdb/CTenantSchemaMigrationControllerComp.h>


// Qt includes
#include <QtSql/QSqlError>
#include <QtSql/QSqlQuery>

// ImtCore includes
#include <imtdb/CTenantDataMigrator.h>
#include <imtdb/CTenantStorageDbStore.h>


namespace imtdb
{


// reimplemented (imtdb::IMigrationController)

istd::CIntRange CTenantSchemaMigrationControllerComp::GetMigrationRange() const
{
	return m_tenantMigrationControllerCompPtr->GetMigrationRange();
}


bool CTenantSchemaMigrationControllerComp::DoMigration(int& resultRevision, const istd::CIntRange& subRange) const
{
	resultRevision = -1;

	CTenantStorageDbStore store(*m_databaseEngineCompPtr, m_registryTableSchemaAttrPtr.IsValid() ? *m_registryTableSchemaAttrPtr : QByteArray());

	CTenantStorageDbStore::Assignments assignments;
	if (!store.EnsureRegistryTable() || !store.LoadAssignments(assignments)){
		SendErrorMessage(0, QStringLiteral("Tenant schema migration failed: the tenant storage assignments could not be loaded"), "CTenantSchemaMigrationControllerComp");

		return false;
	}

	int migratedTenantCount = 0;
	for (const CTenantStorageDbStore::Assignment& assignment: assignments){
		const QByteArray& tenantId = assignment.first;
		const TenantStorageInfo& storageInfo = assignment.second;

		if ((storageInfo.storageKind != TSK_OWN_SCHEMA) || (storageInfo.status == TSS_ARCHIVED)){
			continue;
		}

		int tenantRevision = -1;
		if (!MigrateTenantSchema(storageInfo.schemaName, tenantRevision, subRange)){
			SendErrorMessage(
						0,
						QStringLiteral("Migration of schema '%1' of tenant '%2' failed, the migration of all tenants is rolled back").arg(QString(storageInfo.schemaName), QString(tenantId)),
						"CTenantSchemaMigrationControllerComp");

			return false;
		}

		resultRevision = tenantRevision;

		++migratedTenantCount;
	}

	if (migratedTenantCount == 0){
		// without tenants the revision must still advance, but only for a non-empty requested range
		istd::CIntRange availableRange = GetMigrationRange();
		int firstRevision = (subRange.GetMinValue() >= 0) ? subRange.GetMinValue() : availableRange.GetMinValue();
		int lastRevision = (subRange.GetMaxValue() >= 0) ? subRange.GetMaxValue() : availableRange.GetMaxValue();

		resultRevision = (firstRevision <= lastRevision) ? lastRevision : -1;
	}

	SendInfoMessage(0, QStringLiteral("Migrated %1 tenant schema(s) to revision %2").arg(migratedTenantCount).arg(resultRevision), "CTenantSchemaMigrationControllerComp");

	return true;
}


// private methods

bool CTenantSchemaMigrationControllerComp::IsPostgresDriver() const
{
	return m_databaseEngineCompPtr->GetDatabaseDriverId().startsWith(QByteArrayLiteral("QPSQL"));
}


bool CTenantSchemaMigrationControllerComp::MigrateTenantSchema(
			const QByteArray& schemaName,
			int& resultRevision,
			const istd::CIntRange& subRange) const
{
	// dedicated tenant schemas exist only on Postgres
	if (!IsPostgresDriver()){
		return false;
	}

	QByteArray quotedSchema = CTenantDataMigrator::QuoteIdentifier(schemaName);
	if (quotedSchema.isEmpty()){
		return false;
	}

	QSqlError sqlError;
	QSqlQuery searchPathQuery = m_databaseEngineCompPtr->ExecSqlQuery(QByteArrayLiteral("SHOW search_path"), &sqlError, true);
	if ((sqlError.type() != QSqlError::NoError) || !searchPathQuery.next()){
		return false;
	}

	static const QByteArray setSearchPathQuery = QByteArrayLiteral("SELECT set_config('search_path', :searchPath, false)");

	QString previousSearchPath = searchPathQuery.value(0).toString();

	QVariantMap bindValues;
	bindValues[QStringLiteral(":searchPath")] = QString(quotedSchema) + QStringLiteral(", ") + previousSearchPath;
	m_databaseEngineCompPtr->ExecSqlQuery(setSearchPathQuery, bindValues, &sqlError);
	if (sqlError.type() != QSqlError::NoError){
		return false;
	}

	bool retVal = m_tenantMigrationControllerCompPtr->DoMigration(resultRevision, subRange);

	bindValues[QStringLiteral(":searchPath")] = previousSearchPath;
	m_databaseEngineCompPtr->ExecSqlQuery(setSearchPathQuery, bindValues, &sqlError);

	return retVal && (sqlError.type() == QSqlError::NoError);
}


} // namespace imtdb


