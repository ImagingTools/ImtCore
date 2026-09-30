// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtdb/CTenantDataMigrator.h>


// Qt includes
#include <QtSql/QSqlError>
#include <QtSql/QSqlQuery>


namespace imtdb
{


CTenantDataMigrator::CTenantDataMigrator(const IDatabaseEngine& databaseEngine)
	:m_databaseEngine(databaseEngine),
	m_tenantIdColumn(QByteArrayLiteral("TenantId"))
{
}


void CTenantDataMigrator::SetSourceSchema(const QByteArray& sourceSchema)
{
	m_sourceSchema = sourceSchema;
}


void CTenantDataMigrator::SetTenantIdColumn(const QByteArray& tenantIdColumn)
{
	m_tenantIdColumn = tenantIdColumn;
}


bool CTenantDataMigrator::MigrateTable(
			const QByteArray& tableName,
			const QByteArray& tenantId,
			const QByteArray& targetSchema,
			int& migratedRowCount,
			QString& errorMessage) const
{
	migratedRowCount = 0;

	if (tenantId.isEmpty()){
		errorMessage = QStringLiteral("Tenant ID is empty");

		return false;
	}

	QByteArray sourceTable = CreateQualifiedTableName(m_sourceSchema, tableName);
	QByteArray targetTable = CreateQualifiedTableName(targetSchema, tableName);
	QByteArray tenantColumn = QuoteIdentifier(m_tenantIdColumn);
	if (sourceTable.isEmpty() || targetTable.isEmpty() || tenantColumn.isEmpty()){
		errorMessage = QStringLiteral("Invalid table, schema or column identifier");

		return false;
	}

	int sourceRowCount = 0;
	if (!CountTenantRows(m_sourceSchema, tableName, tenantId, sourceRowCount, errorMessage)){
		return false;
	}

	int targetRowCount = 0;
	if (!CountTenantRows(targetSchema, tableName, tenantId, targetRowCount, errorMessage)){
		return false;
	}

	if (targetRowCount == sourceRowCount){
		// Already migrated (idempotent re-run)
		migratedRowCount = targetRowCount;

		return true;
	}

	if (targetRowCount > 0){
		errorMessage = QStringLiteral("Target table '%1' contains a partial copy (%2 of %3 rows), manual cleanup required").arg(QString(targetTable)).arg(targetRowCount).arg(sourceRowCount);

		return false;
	}

	QByteArray insertQuery = QByteArrayLiteral("INSERT INTO ") + targetTable +
				QByteArrayLiteral(" SELECT * FROM ") + sourceTable +
				QByteArrayLiteral(" WHERE ") + tenantColumn + QByteArrayLiteral(" = :tenantId");

	QVariantMap bindValues;
	bindValues[QStringLiteral(":tenantId")] = QString(tenantId);

	QSqlError sqlError;
	m_databaseEngine.ExecSqlQuery(insertQuery, bindValues, &sqlError);
	if (sqlError.type() != QSqlError::NoError){
		errorMessage = QStringLiteral("Copying rows into '%1' failed: %2").arg(QString(targetTable), sqlError.text());

		return false;
	}

	if (!CountTenantRows(targetSchema, tableName, tenantId, targetRowCount, errorMessage)){
		return false;
	}

	if (targetRowCount != sourceRowCount){
		errorMessage = QStringLiteral("Verification for table '%1' failed: %2 of %3 rows copied").arg(QString(targetTable)).arg(targetRowCount).arg(sourceRowCount);

		return false;
	}

	migratedRowCount = targetRowCount;

	return true;
}


bool CTenantDataMigrator::RemoveSourceRows(const QByteArray& tableName, const QByteArray& tenantId, QString& errorMessage) const
{
	if (tenantId.isEmpty()){
		errorMessage = QStringLiteral("Tenant ID is empty");

		return false;
	}

	QByteArray sourceTable = CreateQualifiedTableName(m_sourceSchema, tableName);
	QByteArray tenantColumn = QuoteIdentifier(m_tenantIdColumn);
	if (sourceTable.isEmpty() || tenantColumn.isEmpty()){
		errorMessage = QStringLiteral("Invalid table, schema or column identifier");

		return false;
	}

	QByteArray deleteQuery = QByteArrayLiteral("DELETE FROM ") + sourceTable +
				QByteArrayLiteral(" WHERE ") + tenantColumn + QByteArrayLiteral(" = :tenantId");

	QVariantMap bindValues;
	bindValues[QStringLiteral(":tenantId")] = QString(tenantId);

	QSqlError sqlError;
	m_databaseEngine.ExecSqlQuery(deleteQuery, bindValues, &sqlError);
	if (sqlError.type() != QSqlError::NoError){
		errorMessage = QStringLiteral("Removing migrated rows from '%1' failed: %2").arg(QString(sourceTable), sqlError.text());

		return false;
	}

	return true;
}


// static methods

QByteArray CTenantDataMigrator::QuoteIdentifier(const QByteArray& identifier)
{
	if (identifier.isEmpty() || identifier.contains('"')){
		return QByteArray();
	}

	return '"' + identifier + '"';
}


// private methods

bool CTenantDataMigrator::CountTenantRows(
			const QByteArray& schemaName,
			const QByteArray& tableName,
			const QByteArray& tenantId,
			int& count,
			QString& errorMessage) const
{
	count = 0;

	QByteArray qualifiedTable = CreateQualifiedTableName(schemaName, tableName);
	QByteArray tenantColumn = QuoteIdentifier(m_tenantIdColumn);
	if (qualifiedTable.isEmpty() || tenantColumn.isEmpty()){
		errorMessage = QStringLiteral("Invalid table, schema or column identifier");

		return false;
	}

	QByteArray countQuery = QByteArrayLiteral("SELECT COUNT(*) FROM ") + qualifiedTable +
				QByteArrayLiteral(" WHERE ") + tenantColumn + QByteArrayLiteral(" = :tenantId");

	QVariantMap bindValues;
	bindValues[QStringLiteral(":tenantId")] = QString(tenantId);

	QSqlError sqlError;
	QSqlQuery query = m_databaseEngine.ExecSqlQuery(countQuery, bindValues, &sqlError, true);
	if (sqlError.type() != QSqlError::NoError || !query.next()){
		errorMessage = QStringLiteral("Counting rows in '%1' failed: %2").arg(QString(qualifiedTable), sqlError.text());

		return false;
	}

	count = query.value(0).toInt();

	return true;
}


QByteArray CTenantDataMigrator::CreateQualifiedTableName(const QByteArray& schemaName, const QByteArray& tableName) const
{
	QByteArray quotedTable = QuoteIdentifier(tableName);
	if (quotedTable.isEmpty()){
		return QByteArray();
	}

	if (schemaName.isEmpty()){
		return quotedTable;
	}

	QByteArray quotedSchema = QuoteIdentifier(schemaName);
	if (quotedSchema.isEmpty()){
		return QByteArray();
	}

	return quotedSchema + '.' + quotedTable;
}


} // namespace imtdb
