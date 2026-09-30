// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtdb/CTenantStorageDbStore.h>

// Qt includes
#include <QtSql/QSqlError>
#include <QtSql/QSqlQuery>


namespace imtdb
{


CTenantStorageDbStore::CTenantStorageDbStore(const IDatabaseEngine& databaseEngine, const QByteArray& tableSchema)
:	m_databaseEngine(databaseEngine),
	m_tableSchema(tableSchema)
{
}


bool CTenantStorageDbStore::EnsureRegistryTable() const
{
	QByteArray query =	QByteArrayLiteral("CREATE TABLE IF NOT EXISTS ") + GetQualifiedTableName() +
						QByteArrayLiteral(
							" (\"TenantId\" TEXT PRIMARY KEY,"
							" \"StorageKind\" INTEGER NOT NULL DEFAULT 0,"
							" \"Status\" INTEGER NOT NULL DEFAULT 0,"
							" \"SchemaName\" TEXT NOT NULL DEFAULT '',"
							" \"ConnectionRef\" TEXT NOT NULL DEFAULT '')");

	QSqlError sqlError;
	m_databaseEngine.ExecSqlQuery(query, &sqlError);

	return sqlError.type() == QSqlError::NoError;
}


bool CTenantStorageDbStore::SaveAssignment(const QByteArray& tenantId, const TenantStorageInfo& info) const
{
	if (tenantId.isEmpty()){
		return false;
	}

	QVariantMap bindValues;
	bindValues[QStringLiteral(":tenantId")] = QString(tenantId);
	bindValues[QStringLiteral(":storageKind")] = int(info.storageKind);
	bindValues[QStringLiteral(":status")] = int(info.status);
	bindValues[QStringLiteral(":schemaName")] = QString(info.schemaName);
	bindValues[QStringLiteral(":connectionRef")] = QString(info.connectionRef);

	QByteArray query =	QByteArrayLiteral("INSERT INTO ") + GetQualifiedTableName() +
						QByteArrayLiteral(
							" (\"TenantId\", \"StorageKind\", \"Status\", \"SchemaName\", \"ConnectionRef\")"
							" VALUES (:tenantId, :storageKind, :status, :schemaName, :connectionRef)"
							" ON CONFLICT (\"TenantId\") DO UPDATE SET"
							" \"StorageKind\" = :storageKind,"
							" \"Status\" = :status,"
							" \"SchemaName\" = :schemaName,"
							" \"ConnectionRef\" = :connectionRef");

	QSqlError sqlError;
	m_databaseEngine.ExecSqlQuery(query, bindValues, &sqlError);

	return sqlError.type() == QSqlError::NoError;
}


bool CTenantStorageDbStore::RemoveAssignment(const QByteArray& tenantId) const
{
	if (tenantId.isEmpty()){
		return false;
	}

	QVariantMap bindValues;
	bindValues[QStringLiteral(":tenantId")] = QString(tenantId);

	QByteArray query =	QByteArrayLiteral("DELETE FROM ") + GetQualifiedTableName() +
						QByteArrayLiteral(" WHERE \"TenantId\" = :tenantId");

	QSqlError sqlError;
	m_databaseEngine.ExecSqlQuery(query, bindValues, &sqlError);

	return sqlError.type() == QSqlError::NoError;
}


bool CTenantStorageDbStore::LoadAssignments(Assignments& result) const
{
	QByteArray query =	QByteArrayLiteral("SELECT \"TenantId\", \"StorageKind\", \"Status\", \"SchemaName\", \"ConnectionRef\" FROM ") +
						GetQualifiedTableName();

	QSqlError sqlError;
	QSqlQuery sqlQuery = m_databaseEngine.ExecSqlQuery(query, &sqlError, true);
	if (sqlError.type() != QSqlError::NoError){
		return false;
	}

	while (sqlQuery.next()){
		Assignment assignment;
		assignment.first = sqlQuery.value(0).toByteArray();
		assignment.second.storageKind = TenantStorageKind(sqlQuery.value(1).toInt());
		assignment.second.status = TenantStorageStatus(sqlQuery.value(2).toInt());
		assignment.second.schemaName = sqlQuery.value(3).toByteArray();
		assignment.second.connectionRef = sqlQuery.value(4).toByteArray();

		result.append(assignment);
	}

	return true;
}


QByteArray CTenantStorageDbStore::GetQualifiedTableName() const
{
	if (m_tableSchema.isEmpty() || IsSqliteDriver()){
		return QByteArrayLiteral("\"TenantStorage\"");
	}

	return '"' + m_tableSchema + QByteArrayLiteral("\".\"TenantStorage\"");
}


bool CTenantStorageDbStore::IsSqliteDriver() const
{
	return m_databaseEngine.GetDatabaseDriverId().startsWith(QByteArrayLiteral("QSQLITE"));
}


} // namespace imtdb
