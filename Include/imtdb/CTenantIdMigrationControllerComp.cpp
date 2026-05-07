// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtdb/CTenantIdMigrationControllerComp.h>


// Qt includes
#include <QtSql/QSqlError>
#include <QtSql/QSqlQuery>


namespace imtdb
{

namespace
{

QString GetSqlResourcePath(const imtdb::IDatabaseEngine& databaseEngine, const QString& fileName)
{
	const QByteArray databaseDriverId = databaseEngine.GetDatabaseDriverId();
	const bool isSqlite = databaseDriverId.compare(QByteArrayLiteral("QSQLITE"), Qt::CaseInsensitive) == 0;
	const QString prefix = isSqlite ? QStringLiteral(":/SQL/SQLite/") : QStringLiteral(":/SQL/Postgres/");
	return prefix + fileName;
}

} // namespace


// protected methods

// reimplemented (imtdb::IMigrationController)

bool CTenantIdMigrationControllerComp::DoMigration(int& resultRevision, const istd::CIntRange& subRange) const
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

	QFile scriptFile(GetSqlResourcePath(*m_databaseEngineCompPtr, QStringLiteral("AddTenantIdColumn.sql")));
	if (!scriptFile.open(QFile::ReadOnly)){
		SendErrorMessage(0, QObject::tr("TenantId migration script '%1' could not be loaded").arg(scriptFile.fileName()));

		return false;
	}

	QByteArray addColumnQuery = scriptFile.readAll();
	scriptFile.close();

	for (int index = 0; index < m_sqlDatabaseObjectDelegatesCompPtr.GetCount(); index++){
		imtdb::ISqlDatabaseObjectDelegate *sqlDatabaseObjectDelegatePtr = m_sqlDatabaseObjectDelegatesCompPtr[index];
		if (sqlDatabaseObjectDelegatePtr == nullptr){
			continue;
		}

		QByteArray query = addColumnQuery;
		QByteArray tableScheme = sqlDatabaseObjectDelegatePtr->GetTableScheme();

		if (!tableScheme.isEmpty()){
			query.replace("${TableScheme}", tableScheme);
		}
		else{
			query.replace("${TableScheme}", "public");
		}

		query.replace("${TableName}", sqlDatabaseObjectDelegatePtr->GetTableName());

		QSqlError sqlError;
		m_databaseEngineCompPtr->ExecSqlQuery(query, &sqlError);

		if (sqlError.type() != QSqlError::NoError){
			qCritical() << __FILE__ << __LINE__
						<< "\n\t| TenantId column could not be added"
						<< "\n\t| Error: " << sqlError
						<< "\n\t| Query: " << query;

			SendErrorMessage(0, QObject::tr("\n\t| TenantId column could not be added"
													"\n\t| Error: %1"
													"\n\t| Query: %2")
												.arg(sqlError.text(), qPrintable(query)));

			return false;
		}
	}

	resultRevision = endIndex;

	SendInfoMessage(0, QString("Migration '%1' succesfully migrated").arg(resultRevision), "CTenantIdMigrationControllerComp");

	return true;
}


} // namespace imtdb

