// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtdb/CTenantBindingMigrationControllerComp.h>


// Qt includes
#include <QtCore/QFile>
#include <QtCore/QObject>
#include <QtCore/QSet>
#include <QtSql/QSqlError>
#include <QtSql/QSqlQuery>


namespace imtdb
{

namespace
{

QString SqlEncode(const QString& value)
{
	QString retVal = value;
	return retVal.replace("'", "''").replace(";", "\b");
}


QString GetSqlResourcePath(const imtdb::IDatabaseEngine& databaseEngine, const QString& fileName)
{
	const QByteArray databaseDriverId = databaseEngine.GetDatabaseDriverId();
	const bool isSqlite = databaseDriverId.compare(QByteArrayLiteral("QSQLITE"), Qt::CaseInsensitive) == 0;
	const QString prefix = isSqlite ? QStringLiteral(":/SQL/SQLite/") : QStringLiteral(":/SQL/Postgres/");
	return prefix + fileName;
}


bool IsSqlite(const imtdb::IDatabaseEngine& databaseEngine)
{
	return databaseEngine.GetDatabaseDriverId().compare(QByteArrayLiteral("QSQLITE"), Qt::CaseInsensitive) == 0;
}


bool ColumnExists(
			const imtdb::IDatabaseEngine& databaseEngine,
			const QByteArray& tableScheme,
			const QByteArray& tableName,
			const QByteArray& columnName)
{
	QString queryText;
	if (IsSqlite(databaseEngine)){
		queryText = QString("PRAGMA table_info(\"%1\")").arg(SqlEncode(QString::fromUtf8(tableName)));
	}
	else{
		queryText = QString(
					"SELECT 1 FROM information_schema.columns "
					"WHERE table_schema = '%1' AND table_name = '%2' AND column_name = '%3'")
				.arg(
						SqlEncode(QString::fromUtf8(tableScheme)),
						SqlEncode(QString::fromUtf8(tableName)),
						SqlEncode(QString::fromUtf8(columnName)));
	}

	QSqlError sqlError;
	QSqlQuery query = databaseEngine.ExecSqlQuery(queryText.toUtf8(), &sqlError);
	if (sqlError.type() != QSqlError::NoError){
		return false;
	}

	if (IsSqlite(databaseEngine)){
		while (query.next()){
			if (query.value(1).toByteArray() == columnName){
				return true;
			}
		}

		return false;
	}

	return query.next();
}


bool ExecuteTenantBindingScript(
			const imtdb::IDatabaseEngine& databaseEngine,
			const QByteArray& scriptTemplate,
			const QByteArray& tableScheme,
			const QByteArray& tableName,
			QSqlError* sqlErrorPtr)
{
	QByteArray query = scriptTemplate;
	query.replace("${TableScheme}", tableScheme);
	query.replace("${TableName}", tableName);

	databaseEngine.ExecSqlQuery(query, sqlErrorPtr);
	return sqlErrorPtr == nullptr || sqlErrorPtr->type() == QSqlError::NoError;
}

} // namespace


// protected methods

// reimplemented (imtdb::IMigrationController)

bool CTenantBindingMigrationControllerComp::DoMigration(int& resultRevision, const istd::CIntRange& subRange) const
{
	resultRevision = -1;

	if (!m_databaseEngineCompPtr.IsValid()){
		return false;
	}

	int max = m_range.GetMaxValue();

	if (!subRange.IsValid()){
		return false;
	}

	if (!m_range.Contains(subRange)){
		return false;
	}

	int endIndex = max;

	if (subRange.IsValid()){
		endIndex = subRange.GetMaxValue();
	}

	QFile scriptFile(GetSqlResourcePath(*m_databaseEngineCompPtr, QStringLiteral("CreateTenantEntityBindingsTable.sql")));
	if (!scriptFile.open(QFile::ReadOnly)){
		SendErrorMessage(0, QObject::tr("Tenant entity bindings migration script '%1' could not be loaded").arg(scriptFile.fileName()));

		return false;
	}

	QByteArray createBindingsQuery = scriptFile.readAll();
	scriptFile.close();

	QFile migrateScriptFile(GetSqlResourcePath(*m_databaseEngineCompPtr, QStringLiteral("MigrateTenantIdToEntityBindings.sql")));
	if (!migrateScriptFile.open(QFile::ReadOnly)){
		SendErrorMessage(0, QObject::tr("TenantId to bindings migration script '%1' could not be loaded").arg(migrateScriptFile.fileName()));

		return false;
	}

	QByteArray migrateBindingsQuery = migrateScriptFile.readAll();
	migrateScriptFile.close();

	QSet<QByteArray> migratedSchemes;

	for (int index = 0; index < m_sqlDatabaseObjectDelegatesCompPtr.GetCount(); index++){
		imtdb::ISqlDatabaseObjectDelegate *sqlDatabaseObjectDelegatePtr = m_sqlDatabaseObjectDelegatesCompPtr[index];
		if (sqlDatabaseObjectDelegatePtr == nullptr){
			continue;
		}

		QByteArray tableScheme = sqlDatabaseObjectDelegatePtr->GetTableScheme();
		if (tableScheme.isEmpty()){
			tableScheme = "public";
		}

		if (!migratedSchemes.contains(tableScheme)){
			QSqlError sqlError;
			if (!ExecuteTenantBindingScript(*m_databaseEngineCompPtr, createBindingsQuery, tableScheme, QByteArray(), &sqlError)){
				qCritical() << __FILE__ << __LINE__
							<< "\n\t| Tenant entity bindings table could not be created"
							<< "\n\t| Error: " << sqlError;

				SendErrorMessage(0, QObject::tr("\n\t| Tenant entity bindings table could not be created"
														"\n\t| Error: %1")
													.arg(sqlError.text()));

				return false;
			}

			migratedSchemes.insert(tableScheme);
		}

		QByteArray tableName = sqlDatabaseObjectDelegatePtr->GetTableName();
		if (ColumnExists(*m_databaseEngineCompPtr, tableScheme, tableName, QByteArrayLiteral("TenantId"))){
			QSqlError sqlError;
			if (!ExecuteTenantBindingScript(*m_databaseEngineCompPtr, migrateBindingsQuery, tableScheme, tableName, &sqlError)){
				qCritical() << __FILE__ << __LINE__
							<< "\n\t| TenantId values could not be migrated to tenant entity bindings"
							<< "\n\t| Error: " << sqlError;

				SendErrorMessage(0, QObject::tr("\n\t| TenantId values could not be migrated to tenant entity bindings"
														"\n\t| Error: %1")
													.arg(sqlError.text()));

				return false;
			}
		}
	}

	resultRevision = endIndex;

	SendInfoMessage(0, QString("Migration '%1' succesfully migrated").arg(resultRevision), "CTenantBindingMigrationControllerComp");

	return true;
}


} // namespace imtdb
