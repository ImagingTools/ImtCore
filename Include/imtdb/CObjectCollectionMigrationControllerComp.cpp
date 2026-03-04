// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtdb/CObjectCollectionMigrationControllerComp.h>


// Qt includes
#include <QtSql/QSqlError>
#include <QtSql/QSqlQuery>


namespace imtdb
{


// protected methods

// reimplemented (imtdb::IMigrationController)

bool CObjectCollectionMigrationControllerComp::DoMigration(int& resultRevision, const istd::CIntRange& subRange) const
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

	QFile scriptFile(":/SQL/CreateCollectionTable.sql");
	if (!scriptFile.open(QFile::ReadOnly)){
		SendErrorMessage(0, QT_TR_NOOP(QString("Collection table creation script '%1'could not be loaded").arg(scriptFile.fileName())));

		return false;
	}

	QByteArray createTableQuery = scriptFile.readAll();
	scriptFile.close();

	for (int index = 0; index < m_sqlDatabaseObjectDelegatesCompPtr.GetCount(); index++){
		imtdb::ISqlDatabaseObjectDelegate *sqlDatabaseObjectDelegatePtr = m_sqlDatabaseObjectDelegatesCompPtr[index];
		if (sqlDatabaseObjectDelegatePtr == nullptr){
			continue;
		}

		QByteArray query = createTableQuery;
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
						<< "\n\t| Table could not be created"
						<< "\n\t| Error: " << sqlError
						<< "\n\t| Query: " << query;

			SendErrorMessage(0, QT_TR_NOOP(QString("\n\t| Table could not be created"
													"\n\t| Error: %1"
													"\n\t| Query: %2")
												.arg(sqlError.text(), qPrintable(query))));

			return false;
		}
	}

	resultRevision = endIndex;

	SendInfoMessage(0, QString("Migration '%1' succesfully migrated").arg(resultRevision), "CObjectCollectionMigrationControllerComp");

	return true;
}


} // namespace imtdb


