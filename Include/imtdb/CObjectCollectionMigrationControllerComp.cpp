#include <imtdb/CObjectCollectionMigrationControllerComp.h>


// Qt includes
#include <QtSql/QSqlError>
#include <QtSql/QSqlQuery>
// #include <QtCore/QDir>
// #include <QtCore/QRegularExpression>


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

	// int min = m_range.GetMinValue();
	int max = m_range.GetMaxValue();

	if (subRange.IsValid() && !m_range.Contains(subRange)){
		return false;
	}

	// int startIndex = min;
	int endIndex = max;

	if (subRange.IsValid()){
		// startIndex = subRange.GetMinValue();
		endIndex = subRange.GetMaxValue();
	}

	if (endIndex == 0){
		QByteArray createEnumSql = "CREATE TYPE \"DocumentState\" AS ENUM ('Active', 'InActive', 'Deleted');";
		m_databaseEngineCompPtr->ExecSqlQuery(createEnumSql);
	}

	for (int index = 0; index < m_sqlDatabaseObjectDelegatesCompPtr.GetCount(); index++){
		imtdb::ISqlDatabaseObjectDelegate *sqlDatabaseObjectDelegatePtr = m_sqlDatabaseObjectDelegatesCompPtr[index];
		if (sqlDatabaseObjectDelegatePtr == nullptr){
			continue;
		}

		QFile scriptFile(":/SQL/CreateCollectionTable.sql");
		if (!scriptFile.open(QFile::ReadOnly)){
			SendErrorMessage(0, QT_TR_NOOP(QString("Collection table creation script '%1'could not be loaded").arg(scriptFile.fileName())));

			return false;
		}

		QByteArray createTableQuery = scriptFile.readAll();
		createTableQuery.replace("${TableName}", sqlDatabaseObjectDelegatePtr->GetTableName());

		QSqlError sqlError;
		m_databaseEngineCompPtr->ExecSqlQuery(createTableQuery, &sqlError);

		if (!(sqlError.type() == QSqlError::ErrorType::NoError)){
			qCritical() << __FILE__ << __LINE__
						<< "\n\t| Table could not be created"
						<< "\n\t| Error: " << sqlError
						<< "\n\t| Query: " << createTableQuery;

			SendErrorMessage(0, QT_TR_NOOP(QString("\n\t| Table could not be created"
												   "\n\t| Error: %1"
												   "\n\t| Query: %2")
											   .arg(sqlError.text())
											   .arg(createTableQuery)));

		}

	}



	resultRevision = endIndex;

	SendInfoMessage(0, QString("Migration '%1' succesfully migrated").arg(resultRevision), "CObjectCollectionMigrationControllerComp");


	return true;
}



} // namespace imtdb


