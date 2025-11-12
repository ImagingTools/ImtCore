#include <imtauthdb/CUserActionDatabaseDelegateComp.h>


// ACF includes
#include <iprm/TParamsPtr.h>

// ImtCore includes
#include <imtauth/IUserGroupFilter.h>


namespace imtauthdb
{


// protected methods

// reimplemented (imtdb::CSqlDatabaseDocumentDelegateComp)

QString CUserActionDatabaseDelegateComp::CreateAdditionalFiltersQuery(const iprm::IParamsSet& /*filterParams*/) const
{
	return QString();
}


// reimplemented (icomp::CComponentBase)

void CUserActionDatabaseDelegateComp::OnComponentCreated()
{
	BaseClass::OnComponentCreated();

	if (m_databaseEngineCompPtr.IsValid()){
		QString tableName = GetTableName();

		if (!TableExists(tableName)){
			QFile scriptFile(":/SQL/CreateCollectionTable.sql");
			if (!scriptFile.open(QFile::ReadOnly)){
				SendErrorMessage(0, QT_TR_NOOP(QString("Collection table creation script '%1'could not be loaded").arg(scriptFile.fileName())));
				return;
			}
	
			QByteArray createTableQuery = scriptFile.readAll();
			scriptFile.close();
	
			createTableQuery.replace("${TableName}", tableName.toUtf8());
			QSqlError sqlError;
			m_databaseEngineCompPtr->ExecSqlQuery(createTableQuery, &sqlError);
	
			if (sqlError.type() != QSqlError::NoError){
				qCritical() << __FILE__ << __LINE__
							<< "\n\t| Table could not be created"
							<< "\n\t| Error: " << sqlError
							<< "\n\t| Query: " << createTableQuery;
	
				SendErrorMessage(0, QT_TR_NOOP(QString("\n\t| Table could not be created"
													   "\n\t| Error: %1"
													   "\n\t| Query: %2")
												   .arg(sqlError.text(), qPrintable(createTableQuery))));
			}
		}
	}
}


// private methods

bool CUserActionDatabaseDelegateComp::TableExists(const QString& tableName) const
{
	if (!m_databaseEngineCompPtr.IsValid()){
		return false;
	}

	QString tableExistsQuery = QString(R"(SELECT EXISTS (SELECT FROM information_schema.tables WHERE table_name = '%1');)").arg(tableName);

	QSqlError sqlError;
	QSqlQuery sqlQuery = m_databaseEngineCompPtr->ExecSqlQuery(tableExistsQuery.toUtf8(), &sqlError);

	if (sqlError.type() != QSqlError::NoError){
		return false;
	}

	if (!sqlQuery.next()){
		return false;
	}

	QSqlRecord record = sqlQuery.record();
	if (record.contains("exists")){
		return record.value("exists").toBool();
	}

	return false;
}


} // namespace imtauthdb


