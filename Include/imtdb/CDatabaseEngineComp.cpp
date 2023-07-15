#include <imtdb/CDatabaseEngineComp.h>


// Qt includes
#include <QtCore/QDataStream>
#include <QtCore/QRegularExpression>


namespace imtdb
{


// public methods

CDatabaseEngineComp::CDatabaseEngineComp()
	:m_databaseAccessObserver(*this)
{
}


// reimplemented (IDatabaseEngine)

bool CDatabaseEngineComp::BeginTransaction() const
{
	return QSqlDatabase::database(GetConnectionName()).transaction();
}


bool CDatabaseEngineComp::FinishTransaction() const
{
	return QSqlDatabase::database(GetConnectionName()).commit();
}


bool CDatabaseEngineComp::CancelTransaction() const
{
	return QSqlDatabase::database(GetConnectionName()).rollback();
}


QSqlQuery CDatabaseEngineComp::ExecSqlQuery(const QByteArray& queryString, QSqlError* sqlErrorPtr, bool isForwardOnly) const
{
	if (!EnsureDatabaseConnected(sqlErrorPtr)){
		return QSqlQuery();
	}

	QSqlDatabase databaseConnection = QSqlDatabase::database(GetConnectionName());

	QSqlQuery retVal(databaseConnection);

	retVal.setForwardOnly(isForwardOnly);

	bool success = retVal.exec(queryString);
	if (!success){
		if (sqlErrorPtr != nullptr){
			*sqlErrorPtr = retVal.lastError();
		}
	
		return QSqlQuery();
	}
	
	if (sqlErrorPtr != nullptr){
		*sqlErrorPtr = databaseConnection.lastError().type() != QSqlError::NoError ? databaseConnection.lastError() : retVal.lastError();
	}

	if ((databaseConnection.lastError().type() != QSqlError::NoError) || (retVal.lastError().type() != QSqlError::NoError)){
		qCritical() << __FILE__ << __LINE__
					<< "\n\t| what() sqlError Occured"
					<< "\n\t| Database error" << databaseConnection.lastError().text()
					<< "\n\t| Query error" << retVal.lastError().text()
					<< "\n\t| Executed query" << queryString;
	}

	return retVal;
}


QSqlQuery CDatabaseEngineComp::ExecSqlQuery(const QByteArray& queryString, const QVariantMap& bindValues, QSqlError* sqlError, bool isForwardOnly) const
{
	if (!EnsureDatabaseConnected(sqlError)){
		return QSqlQuery();
	}

	QSqlDatabase databaseConnection = QSqlDatabase::database(GetConnectionName());

	QSqlQuery retVal(databaseConnection);

	retVal.setForwardOnly(isForwardOnly);

	retVal.prepare(queryString);

	for(auto value = bindValues.cbegin(); value != bindValues.cend(); ++ value){
		retVal.bindValue(value.key(), *value);
	}

	retVal.exec();

	if (sqlError){
		*sqlError = databaseConnection.lastError().type() ? databaseConnection.lastError() : retVal.lastError();
    }

	if ((databaseConnection.lastError().type() != QSqlError::NoError) || (retVal.lastError().type() != QSqlError::NoError)){
		qCritical() << __FILE__ << __LINE__
					<< "\n\t| what(): sqlError Occured"
					<< "\n\t| Database error" << databaseConnection.lastError().text()
					<< "\n\t| Query error" << retVal.lastError().text()
					<< "\n\t| Executed query" << queryString
					<< "\n\t| Bind Values" << bindValues
					<< "\n\t| Bound Values" << retVal.boundValues()
					<< "\n\t| Executed query" << ::qPrintable(retVal.executedQuery());
	}

	return retVal;
}


QSqlQuery CDatabaseEngineComp::ExecSqlQueryFromFile(const QString& filePath, QSqlError* sqlError, bool isForwardOnly) const
{
	QFile sqlQuetyFile(filePath);

	sqlQuetyFile.open(QFile::ReadOnly);

	QByteArray queryString = sqlQuetyFile.readAll();

	sqlQuetyFile.close();

	return ExecSqlQuery(queryString, sqlError, isForwardOnly);
}


QSqlQuery CDatabaseEngineComp::ExecSqlQueryFromFile(const QString& filePath, const QVariantMap& bindValues, QSqlError* sqlError, bool isForwardOnly) const
{
	QFile sqlQuetyFile(filePath);

	sqlQuetyFile.open(QFile::ReadOnly);

	QByteArray queryString = sqlQuetyFile.readAll();

	sqlQuetyFile.close();

	return ExecSqlQuery(queryString, bindValues, sqlError, isForwardOnly);
}


// reimplemented (IDatabaseServerConnectionChecker)

bool CDatabaseEngineComp::IsDatabaseServerConnected(QString& errorMessage) const
{
	QSqlDatabase maintainanceDb = QSqlDatabase::addDatabase(*m_dbTypeAttrPtr, *m_maintenanceDatabaseNameAttrPtr);
	maintainanceDb.setHostName(GetHostName());
	maintainanceDb.setUserName(GetUserName());
	maintainanceDb.setPassword(GetPassword());
	maintainanceDb.setDatabaseName(*m_maintenanceDatabaseNameAttrPtr);
	maintainanceDb.setPort(GetPort());

	bool isConnected = maintainanceDb.open();

	maintainanceDb.close();

	if (!isConnected){
		QSqlError sqlError = maintainanceDb.lastError();
		errorMessage = sqlError.text();
	}

	return isConnected;
}


void CDatabaseEngineComp::DrectBindValue(QByteArray* string, const QByteArray& what, const QByteArray& expr)
{
	QRegularExpression regExp(what);

	QRegularExpressionMatchIterator globalMatch = regExp.globalMatch(*string);
	while(globalMatch.hasNext()){
		QRegularExpressionMatch regMatch = globalMatch.next();
		if (regMatch.capturedEnd() < string->length()-1){
			QChar nextSym = string->at(regMatch.capturedEnd());
			if (!nextSym.isLetter()){
				string->replace(regMatch.capturedStart(), regMatch.capturedLength(), expr);
			}
		}
		else{
			string->replace(regMatch.capturedStart(), string->length() - 1, expr);
		}
	}
}


void CDatabaseEngineComp::DrectBindValueInsertDefault(QByteArray* string, const QByteArray& what)
{
	CDatabaseEngineComp::DrectBindValue(string, what, " DEFAULT ");
}


void CDatabaseEngineComp::DrectBindValueUpdateDefault(QByteArray* string, const QByteArray& what)
{
	QByteArray newValue = what;
	if (newValue.startsWith(':')){
		newValue.remove(0, 1);
		newValue.append('"');
		newValue.prepend('"');

		CDatabaseEngineComp::DrectBindValue(string, what, newValue);
	}
}


// protected methods

bool CDatabaseEngineComp::OpenDatabase() const
{
	bool retVal = false;

	QSqlDatabase databaseConnection = QSqlDatabase::database(GetConnectionName());
	if (databaseConnection.isOpen()){
		databaseConnection.close();
	}

	databaseConnection = QSqlDatabase::addDatabase(*m_dbTypeAttrPtr, GetConnectionName());
	QString databaseName = GetDatabaseName();
	if (m_dbTypeAttrPtr->GetValue().compare(QByteArray("QODBC"), Qt::CaseInsensitive) == 0){

	}
	else {
		databaseConnection.setHostName(GetHostName());
		databaseConnection.setUserName(GetUserName());
		databaseConnection.setPassword(GetPassword());
		databaseConnection.setPort(GetPort());
	}
	databaseConnection.setDatabaseName(databaseName);
	retVal = databaseConnection.open();

	return retVal;
}


bool CDatabaseEngineComp::CreateDatabase() const
{
	if (!m_migrationFolderPathCompPtr.IsValid()){
		SendCriticalMessage(0, QString("Wrong component configuration: 'MigrationFolderPath' component was not set"));

		return false;
	}

	QDir migrationsFolder(m_migrationFolderPathCompPtr->GetPath());
	if (!migrationsFolder.exists()){
		SendErrorMessage(0, QString("Folder containing SQL scripts doesn't exist: %1").arg(m_migrationFolderPathCompPtr->GetPath()));
		
		return false;
	}

	if ((*m_maintenanceDatabaseNameAttrPtr).isEmpty()){
		SendCriticalMessage(0, QString("Maintenance database name not was not set"));

		return false;
	}

	qDebug() << GetHostName() << GetUserName() << GetPassword() << GetDatabaseName();

	bool retVal = false;
	QSqlDatabase maintainanceDb = QSqlDatabase::addDatabase(*m_dbTypeAttrPtr, *m_maintenanceDatabaseNameAttrPtr);
	maintainanceDb.setHostName(GetHostName());
	maintainanceDb.setUserName(GetUserName());
	maintainanceDb.setPassword(GetPassword());
	maintainanceDb.setDatabaseName(*m_maintenanceDatabaseNameAttrPtr);
	maintainanceDb.setPort(GetPort());

	retVal = maintainanceDb.open();
	if (retVal){
		QString createDatabaseQuery;

		if (QFile(migrationsFolder.filePath("CreateDatabase.sql")).exists()){
			QFile scriptFile(migrationsFolder.filePath("CreateDatabase.sql"));

			scriptFile.open(QFile::ReadOnly);

			createDatabaseQuery = scriptFile.readAll();

			createDatabaseQuery.replace(":DatabaseName", GetDatabaseName());

			scriptFile.close();
		}

		maintainanceDb.exec(createDatabaseQuery);
		QSqlError sqlError = maintainanceDb.lastError();

		QString rollbackQuery = QString("DROP DATABASE %1").arg(GetDatabaseName());

		maintainanceDb.close();

		retVal = bool(sqlError.type() == QSqlError::ErrorType::NoError);
		if (retVal){
			// Close connection to the service database:
			QSqlDatabase::removeDatabase(*m_maintenanceDatabaseNameAttrPtr);

			// Initialize a new connection to the "real" database:
			QSqlDatabase databaseConnection = QSqlDatabase::addDatabase(*m_dbTypeAttrPtr, GetConnectionName());

			databaseConnection.setHostName(GetHostName());
			databaseConnection.setUserName(GetUserName());
			databaseConnection.setPassword(GetPassword());
			databaseConnection.setDatabaseName(GetDatabaseName());
			databaseConnection.setPort(GetPort());

			// Open the database
			retVal = databaseConnection.open();
			if (retVal){
				// Create revision table in the database:
				ExecSqlQueryFromFile(migrationsFolder.filePath("CreateRevision.sql"), &sqlError);
				if (sqlError.type() != QSqlError::NoError){
					// If the creation of the revision tables was failed, remove newly created database:
					ExecSqlQuery(rollbackQuery.toLocal8Bit());

					SendErrorMessage(0, QString("\n\t| Revision table could not be created""\n\t| Error: %1").arg(sqlError.text()));

					retVal = false;
				}

				if (retVal && (*m_autoCreateTablesAttrPtr > 0)){
					retVal = ExecuteDatabasePatches();
					if (!retVal){
						// If the execution of patches was failed, remove newly created database:
						ExecSqlQuery(rollbackQuery.toLocal8Bit());
					}
				}
			}
			else{
				SendErrorMessage(0, QString("\n\t| Database could not be connected""\n\t| Error: %1").arg(databaseConnection.lastError().text()));
			}
		}
		else{
			qCritical() << __FILE__ << __LINE__
						<< "\n\t| Database could not be created"
						<< "\n\t| Error: " << sqlError
						<< "\n\t| Query: " << createDatabaseQuery;
			SendErrorMessage(0, QString("\n\t| Database could not be created"
						"\n\t| Error: %1"
						"\n\t| Query: %2")
						.arg(sqlError.text())
						.arg(createDatabaseQuery));
		}
	}
	else{
		SendErrorMessage(0, QString("Maintanance database could not be opened. Error message: '%1'").arg(maintainanceDb.lastError().text()));

		return false;
	}

	return retVal;
}


bool CDatabaseEngineComp::ExecuteDatabasePatches() const
{
	if (m_migrationFolderPathCompPtr.IsValid()){
		int lastMigration = GetLastMigration();
		int databaseVersion =  GetDatabaseVersion();
		QDir folder(m_migrationFolderPathCompPtr->GetPath());
		QStringList nameFilter = {"migration_*.sql"};
		folder.setNameFilters(nameFilter);
		QStringList files = folder.entryList();
		for (int index = databaseVersion + 1; index <= lastMigration; index++){
			QSqlError sqlError;
			if (index == 0){
				ExecSqlQueryFromFile(folder.filePath("GetRevision.sql"), &sqlError);
				if (sqlError.type() != QSqlError::NoError){
					qCritical() << __FILE__ << __LINE__
								<< "\n\t| Unable to migration database"
								<< "\n\t| Error: " << sqlError.text();

					SendErrorMessage(0, QString("Migration is failed: %1").arg(sqlError.text()), "Database engine");
					return false;
				}
				ExecSqlQueryFromFile(folder.filePath("migration_"+QString::number(index)+".sql"), &sqlError);
				if (sqlError.type() != QSqlError::NoError){
					qCritical() << __FILE__ << __LINE__
								<< "\n\t| Unable to migration database"
								<< "\n\t| Error: " << sqlError.text();

					SendErrorMessage(0, QString("Migration is failed: %1").arg(sqlError.text()), "Database engine");

					return false;
				}
			}
			else{
				QString migrationFilePath = folder.filePath("migration_"+QString::number(index) + ".sql");

				ExecSqlQueryFromFile(migrationFilePath.toLocal8Bit(), &sqlError);

				if (sqlError.type() != QSqlError::NoError){
					SendErrorMessage(0, QString("Execution of %1 is failed: %2").arg(migrationFilePath).arg(sqlError.text()), "Database engine");

					return false;
				}
			}
			QVariantMap valuesRevision;
			valuesRevision.insert(":Revision",index);
			ExecSqlQueryFromFile(folder.filePath("SetRevision.sql"), valuesRevision, &sqlError);
			if (sqlError.type() != QSqlError::NoError){
				SendErrorMessage(0, QString("Execution of SetRevision.sql is failed: %1").arg(sqlError.text()), "Database engine");
			
				return false;
			}
		}

		SendInfoMessage(0, QString("Database was successfully  migrated"), "Database engine");

		return true;
	}

	return false;
}


void CDatabaseEngineComp::OnDatabaseAccessChanged(
			const istd::IChangeable::ChangeSet& /*changeSet*/,
			const imtdb::IDatabaseLoginSettings* databaseAccessSettingsPtr)
{
	EnsureDatabaseCreated();

	Q_ASSERT(databaseAccessSettingsPtr != nullptr);
	if (databaseAccessSettingsPtr != nullptr){
		m_workingAccessSettings.CopyFrom(*databaseAccessSettingsPtr);
	}
}


// reimplemented (icomp::CComponentBase)

void CDatabaseEngineComp::OnComponentCreated()
{
	BaseClass::OnComponentCreated();

	if (m_databaseAccessSettingsCompPtr.IsValid()){
		m_databaseAccessObserver.RegisterObject(m_databaseAccessSettingsCompPtr.GetPtr(), &CDatabaseEngineComp::OnDatabaseAccessChanged);
	}
	else{
		EnsureDatabaseCreated();
	}
}


void CDatabaseEngineComp::OnComponentDestroyed()
{
	m_databaseAccessObserver.UnregisterAllObjects();

	BaseClass::OnComponentDestroyed();
}


// private methods

bool CDatabaseEngineComp::EnsureDatabaseConnected(QSqlError* sqlError) const
{
	QSqlDatabase databaseConnection = QSqlDatabase::database(GetConnectionName());

	bool isOpened = databaseConnection.isOpen();
	if (!isOpened){
		databaseConnection.close();

		isOpened = OpenDatabase();
		if (!isOpened){
			qCritical() << __FILE__ << __LINE__
						<< "\n\t| Unable to open database"
						<< "\n\t| Error: " << databaseConnection.lastError().text();

			SendErrorMessage(0, QString("Database '%1' could not be connected").arg(GetDatabaseName()), "Database engine");
		}
	}

	if(sqlError != nullptr){
		*sqlError = databaseConnection.lastError();
	}

	return isOpened;
}


bool CDatabaseEngineComp::EnsureDatabaseCreated() const
{
	bool isOpened = OpenDatabase();
	if (!isOpened){
		QString errorMessage;
		bool isServerConnected = IsDatabaseServerConnected(errorMessage);
		if (isServerConnected){
			if (*m_autoCreateDatabaseAttrPtr >= 1){
				return CreateDatabase();
			}
		}
		else{
			SendErrorMessage(0, "Database server could not be connected", "Database Engine");

			return false;
		}
	}
	else {
		if (*m_autoCreateDatabaseAttrPtr == 2){
			return CreateDatabase();
		}
		ExecuteDatabasePatches();
	}

	return true;
}


QString CDatabaseEngineComp::GetConnectionName() const
{
	qptrdiff threadId = (qptrdiff)QThread::currentThreadId();

	return GetDatabaseName() + QString(" - %1").arg(threadId);
}


QString CDatabaseEngineComp::GetDatabaseName() const
{
	if (m_databaseAccessSettingsCompPtr.IsValid()){
		return m_databaseAccessSettingsCompPtr->GetDatabaseName();
	}

	return *m_dbNameAttrPtr;
}


QString CDatabaseEngineComp::GetHostName() const
{
	if (m_databaseAccessSettingsCompPtr.IsValid()){
		return m_databaseAccessSettingsCompPtr->GetHost();
	}

	return *m_hostNameAttrPtr;
}


int CDatabaseEngineComp::GetPort() const
{
	if (m_databaseAccessSettingsCompPtr.IsValid()){
		return m_databaseAccessSettingsCompPtr->GetPort();
	}

	return *m_portAttrPtr;
}


QString CDatabaseEngineComp::GetUserName() const
{
	if (m_databaseAccessSettingsCompPtr.IsValid()){
		return m_databaseAccessSettingsCompPtr->GetUserName();
	}

	return *m_userNameAttrPtr;
}


QString CDatabaseEngineComp::GetPassword() const
{
	if (m_databaseAccessSettingsCompPtr.IsValid()){
		return m_databaseAccessSettingsCompPtr->GetPassword();
	}

	return *m_paswordAttrPtr;
}


int CDatabaseEngineComp::GetLastMigration() const
{
	if (m_migrationFolderPathCompPtr.IsValid()){
		QString migrationFolder = m_migrationFolderPathCompPtr->GetPath();

		QDir folder(migrationFolder);
		if (folder.exists()){
			QStringList nameFilter = {"migration_*.sql"};
			folder.setNameFilters(nameFilter);
			int avaliableMigration = -1;
			if (!folder.entryList().isEmpty()){
				QStringList files = folder.entryList();
				for (int index = 0; index < files.size(); index++){
					QString nameFile = files[index];
					nameFile.remove("migration_").remove(".sql");
					QRegularExpression re("\\d*");
					if (re.match(nameFile).hasMatch()){
						if (avaliableMigration < nameFile.toInt()){
							avaliableMigration = nameFile.toInt();
						}
					}
				}
			}
			return avaliableMigration;
		}
		else{
			SendErrorMessage(0, QString("Directory containing SQL-migration files doesn't exist: '%1'").arg(migrationFolder), "Database Migration");
			return -1;
		}
	}
	return -1;
}


int CDatabaseEngineComp::GetDatabaseVersion() const
{
	if (m_migrationFolderPathCompPtr.IsValid()){
		QSqlError sqlError;
		QDir folder(m_migrationFolderPathCompPtr->GetPath());
		QSqlQuery queryGetRevision = ExecSqlQueryFromFile(folder.filePath("GetRevision.sql"), &sqlError);
		if (sqlError.type() != QSqlError::NoError){
			return -1;
		}
		else{
			if (queryGetRevision.next()){
				return queryGetRevision.value(0).toInt();
			}
			return -1;
		}
	}
	return -1;
}


bool CDatabaseEngineComp::ExecuteTransaction(const QByteArray &sqlQuery) const
{
	BeginTransaction();

	QByteArray singleQuery;

	for (const char* ch = sqlQuery.begin(); ch != sqlQuery.end(); ch++){
		if (*ch != ';'){
			singleQuery.append(*ch);
		}
		else {
			if (!singleQuery.isEmpty()){
				QSqlError error;
				ExecSqlQuery(singleQuery, &error);
				if (error.type() != QSqlError::NoError){
					SendErrorMessage(0, error.text(), "Database collection");

					CancelTransaction();

					return false;
				}

				singleQuery.clear();
			}
		}
	}

	if (!singleQuery.isEmpty()){
		QSqlError error;
		ExecSqlQuery(singleQuery, &error);
		if (error.type() != QSqlError::NoError){
			SendErrorMessage(0, error.text(), "Database collection");

			CancelTransaction();

			return false;
		}
	}

	FinishTransaction();

	return true;
}


} // namespace imtdb


