// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
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
	if (!EnsureDatabaseConnected()){
		return false;
	}

	return QSqlDatabase::database(GetConnectionName()).transaction();
}


bool CDatabaseEngineComp::FinishTransaction() const
{
	if (!EnsureDatabaseConnected()){
		return false;
	}

	return QSqlDatabase::database(GetConnectionName()).commit();
}


bool CDatabaseEngineComp::CancelTransaction() const
{
	if (!EnsureDatabaseConnected()){
		return false;
	}

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
	const QSqlError queryError = retVal.lastError();
	if (!success){
		if (sqlErrorPtr != nullptr){
			*sqlErrorPtr = queryError;
		}

		SendErrorMessage(0, QStringLiteral("Database query failed: '%1', SQL-statement: '%2'").arg(queryError.text()).arg(queryString), __FILE__);

		return QSqlQuery();
	}

	const QSqlError dbError = databaseConnection.lastError();
	if (sqlErrorPtr != nullptr) {
		*sqlErrorPtr = dbError.type() != QSqlError::NoError ? dbError : queryError;
	}

	if (dbError.type() != QSqlError::NoError || queryError.type() != QSqlError::NoError) {
		qCritical() << __FILE__ << __LINE__
					<< "\n\t| what() sqlError Occured"
					<< "\n\t| Database error" << dbError.text()
					<< "\n\t| Query error" << queryError.text()
					<< "\n\t| Executed query" << queryString;
	}

	return retVal;
}


QSqlQuery CDatabaseEngineComp::ExecSqlQuery(
			const QByteArray& queryString,
			const QVariantMap& bindValues,
			QSqlError* sqlError,
			bool isForwardOnly) const
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

	const QSqlError queryError = retVal.lastError();
	const QSqlError dbError = databaseConnection.lastError();
	if (sqlError != nullptr){
		*sqlError = dbError.type() != QSqlError::NoError ? dbError : queryError;
	}

	if ((dbError.type() != QSqlError::NoError) || (retVal.lastError().type() != QSqlError::NoError)){
		qCritical() << __FILE__ << __LINE__
					<< "\n\t| what(): sqlError Occured"
					<< "\n\t| Database error" << dbError.text()
					<< "\n\t| Query error" << queryError.text()
					<< "\n\t| Executed query" << queryString
					<< "\n\t| Bind Values" << bindValues
					<< "\n\t| Bound Values" << retVal.boundValues()
					<< "\n\t| Executed query" << retVal.executedQuery();
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


QSqlQuery CDatabaseEngineComp::ExecSqlQueryFromFile(
			const QString& filePath,
			const QVariantMap& bindValues,
			QSqlError* sqlError,
			bool isForwardOnly) const
{
	QFile sqlQuetyFile(filePath);

	sqlQuetyFile.open(QFile::ReadOnly);

	QByteArray queryString = sqlQuetyFile.readAll();

	sqlQuetyFile.close();

	return ExecSqlQuery(queryString, bindValues, sqlError, isForwardOnly);
}


// reimplemented (IDatabaseServerConnectionChecker)

bool CDatabaseEngineComp::CheckDatabaseConnection(QString& errorMessage) const
{
	QSqlDatabase::removeDatabase(*m_maintenanceDatabaseNameAttrPtr);

	QSqlDatabase maintainanceDb = QSqlDatabase::addDatabase(*m_dbTypeAttrPtr, *m_maintenanceDatabaseNameAttrPtr);

	maintainanceDb.setConnectOptions(GetConnectionOptionsString(*m_dbTypeAttrPtr));

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
	CDatabaseEngineComp::DrectBindValue(string, what, QByteArrayLiteral(" DEFAULT "));
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

	QByteArray databaseDriverTypeId = *m_dbTypeAttrPtr;

	databaseConnection = InitDatabase(databaseDriverTypeId);

	retVal = databaseConnection.open();

	return retVal;
}


bool CDatabaseEngineComp::CreateDatabase(int flags) const
{
	if ((*m_maintenanceDatabaseNameAttrPtr).isEmpty()){
		SendCriticalMessage(0, QT_TR_NOOP("Maintenance database name was not set"));

		return false;
	}

	bool retVal = true;
	QString rollbackQuery;

	if (flags & DCF_CREATE_DATABASE){
		retVal = CreateDatabaseInstance();

		rollbackQuery = QStringLiteral("DROP DATABASE %1").arg(GetDatabaseName());
	}

	if (retVal){
		QByteArray databaseDriverTypeId = *m_dbTypeAttrPtr;
		QSqlDatabase databaseConnection = InitDatabase(databaseDriverTypeId);

		// Open the database
		retVal = databaseConnection.open();
		if (!retVal){
			SendErrorMessage(0, QStringLiteral("\n\t| Database could not be connected""\n\t| Error: %1").arg(databaseConnection.lastError().text()));

			return retVal;
		}

		if (flags & DCF_CREATE_DATABASE_META){
			retVal = CreateDatabaseMetaInfo();
			if (!retVal && !rollbackQuery.isEmpty()){
				// If the creation of the revision tables was failed, remove newly created database:
				ExecSqlQuery(rollbackQuery.toUtf8());
			}
		}

		if (flags & DCF_EXECUTE_PATCHES){
			if (retVal && (*m_autoCreateTablesAttrPtr > 0)){
				retVal = ExecuteDatabasePatches();
				if (!retVal && !rollbackQuery.isEmpty()){
					// If the execution of patches was failed, remove newly created database:
					ExecSqlQuery(rollbackQuery.toLocal8Bit());
				}
			}
		}
	}

	return retVal;
}


bool CDatabaseEngineComp::ExecuteDatabasePatches() const
{
	if (!m_migrationControllerCompPtr.IsValid()){
		return false;
	}

	int newRevision;
	int databaseVersion = GetDatabaseVersion();

	if(!BeginTransaction()){
		qCritical() << __FILE__ << __LINE__
					<< "\n\t| what(): Transaction could not be started"
					<< "\n\t| Error: " << QSqlDatabase::database(GetConnectionName()).lastError().text();

		SendErrorMessage(0, QStringLiteral("Transaction could not be started: %1").arg(QSqlDatabase::database(GetConnectionName()).lastError().text()), __FILE__);

		return false;
	}

	bool retVal = m_migrationControllerCompPtr->DoMigration(newRevision, istd::CIntRange(databaseVersion + 1, -1));
	if (!retVal){
		CancelTransaction();

		return false;
	}
	if (newRevision >= 0){
		// Set max revision to database
		QSqlError sqlError;
		QVariantMap valuesRevision({ {QStringLiteral(":Revision"), newRevision} });
		ExecSqlQueryFromFile(QStringLiteral(":/SQL/SetRevision.sql"), valuesRevision, &sqlError);

		if (sqlError.type() != QSqlError::NoError){
			SendErrorMessage(0, QStringLiteral("Execution of SetRevision.sql failed: '%1'").arg(sqlError.text()), __FILE__);

			CancelTransaction();

			return false;
		}
	}

	if(!FinishTransaction()){
		qCritical() << __FILE__ << __LINE__
					<< "\n\t| what(): Transaction could not be finished"
					<< "\n\t| Error: " << QSqlDatabase::database(GetConnectionName()).lastError().text();

		SendErrorMessage(0, QStringLiteral("Transaction could not be finished: %1").arg(QSqlDatabase::database(GetConnectionName()).lastError().text()), __FILE__);

		return false;
	}

	return retVal;
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


QSqlDatabase CDatabaseEngineComp::InitDatabase(const QByteArray& databaseDriverTypeId) const
{
	QSqlDatabase databaseConnection = QSqlDatabase::addDatabase(databaseDriverTypeId, GetConnectionName());

	databaseConnection.setConnectOptions(GetConnectionOptionsString(databaseDriverTypeId));

	QString databaseName = GetDatabaseName();
	if (databaseDriverTypeId.compare(QByteArrayLiteral("QODBC"), Qt::CaseInsensitive) == 0){
	}
	else if (databaseDriverTypeId.compare(QByteArrayLiteral("QSQLITE"), Qt::CaseInsensitive) == 0){
		databaseName = GetDatabasePath();
	}
	else{
		databaseConnection.setHostName(GetHostName());
		databaseConnection.setUserName(GetUserName());
		databaseConnection.setPassword(GetPassword());
		databaseConnection.setPort(GetPort());
	}

	databaseConnection.setDatabaseName(databaseName);

	return databaseConnection;
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
	QString databaseConnectionName = GetConnectionName();
	QSqlDatabase databaseConnection = QSqlDatabase::database(databaseConnectionName);

	QThread* threadPtr = QThread::currentThread();
	connect(threadPtr, &QThread::finished, this, &CDatabaseEngineComp::OnThreadFinished, Qt::DirectConnection);

	bool isOpened = databaseConnection.isOpen();
	if (!isOpened){
		databaseConnection.close();

		isOpened = OpenDatabase();
		if (!isOpened){
			qCritical() << __FILE__ << __LINE__
						<< "\n\t| Unable to open database"
						<< "\n\t| Error: " << databaseConnection.lastError().text();

			SendErrorMessage(0, QStringLiteral("Database '%1' could not be connected").arg(GetDatabaseName()), __FILE__);
		}
	}

	if(sqlError != nullptr){
		*sqlError = databaseConnection.lastError();
	}

	return isOpened;
}


bool CDatabaseEngineComp::EnsureDatabaseCreated() const
{
	bool retVal = OpenDatabase();
	if (!retVal){
		QString errorMessage;
		bool isServerConnected = CheckDatabaseConnection(errorMessage);
		if (isServerConnected){
			if (*m_autoCreateDatabaseAttrPtr >= 1){
				return CreateDatabase(DCF_ALL);
			}
		}
		else{
			SendErrorMessage(0, QStringLiteral("Database server could not be connected: %1").arg(errorMessage), __FILE__);

			return false;
		}
	}
	else {
		if (*m_autoCreateDatabaseAttrPtr == 2){
			retVal = retVal && CreateDatabase(DCF_ALL);
		}

		retVal = retVal && EnsureDatabaseConsistency();

		retVal = retVal && ExecuteDatabasePatches();
	}

	return retVal;
}


bool CDatabaseEngineComp::EnsureDatabaseConsistency() const
{
	int databaseVersion = GetDatabaseVersion();
	// Database tables were not created
	if (databaseVersion < 0){
		return CreateDatabase(DCF_CREATE_DATABASE_META | DCF_EXECUTE_PATCHES);
	}

	return true;
}


bool CDatabaseEngineComp::CreateDatabaseInstance() const
{
	if ((*m_maintenanceDatabaseNameAttrPtr).isEmpty()){
		SendCriticalMessage(0, QT_TR_NOOP("Maintenance database name was not set"));

		return false;
	}

	qDebug() << GetHostName() << GetUserName() << GetPassword() << GetDatabaseName();

	bool retVal = false;
	QSqlDatabase maintainanceDb = QSqlDatabase::addDatabase(*m_dbTypeAttrPtr, *m_maintenanceDatabaseNameAttrPtr);

	maintainanceDb.setConnectOptions(GetConnectionOptionsString(*m_dbTypeAttrPtr));

	maintainanceDb.setHostName(GetHostName());
	maintainanceDb.setUserName(GetUserName());
	maintainanceDb.setPassword(GetPassword());
	maintainanceDb.setDatabaseName(*m_maintenanceDatabaseNameAttrPtr);
	maintainanceDb.setPort(GetPort());

	retVal = maintainanceDb.open();
	if (!retVal){
		SendErrorMessage(0, QStringLiteral("Maintanance database could not be opened. Error message: '%1'").arg(maintainanceDb.lastError().text()));

		return false;
	}

	QFile scriptFile(QStringLiteral(":/SQL/CreateDatabase.sql"));
	if (!scriptFile.open(QFile::ReadOnly)){
		SendErrorMessage(0, QStringLiteral("Database creation script '%1'could not be loaded").arg(scriptFile.fileName()));

		return false;
	}

	QString createDatabaseQuery = scriptFile.readAll();
	createDatabaseQuery.replace(QStringLiteral(":DatabaseName"), GetDatabaseName());

	QSqlQuery createDbQuery(maintainanceDb);
	createDbQuery.exec(createDatabaseQuery);

	QSqlError sqlError = createDbQuery.lastError();
	retVal = bool(sqlError.type() == QSqlError::ErrorType::NoError);
	if (!retVal){
		qCritical() << __FILE__ << __LINE__
					<< "\n\t| Database could not be created"
					<< "\n\t| Error: " << sqlError
					<< "\n\t| Query: " << createDatabaseQuery;

		SendErrorMessage(0,  QStringLiteral("\n\t| Database could not be created"
											"\n\t| Error: %1"
											"\n\t| Query: %2")
								.arg(sqlError.text())
								.arg(createDatabaseQuery));

	}

	maintainanceDb.close();

	// Close connection to the service database:
	QSqlDatabase::removeDatabase(*m_maintenanceDatabaseNameAttrPtr);

	return retVal;
}


bool CDatabaseEngineComp::CreateDatabaseMetaInfo() const
{
	QSqlError sqlError;

	// Create revision table in the database:
	ExecSqlQueryFromFile(QStringLiteral(":/SQL/CreateRevision.sql"), &sqlError);
	if (sqlError.type() != QSqlError::NoError){
		SendErrorMessage(0, QStringLiteral("\n\t| Revision table could not be created""\n\t| Error: %1").arg(sqlError.text()));

		return false;
	}

	return true;
}


QString CDatabaseEngineComp::GetConnectionName() const
{
	quintptr threadId = reinterpret_cast<quintptr>(QThread::currentThreadId());

	return GetDatabaseName() + QStringLiteral(" - %1").arg(threadId);
}


QString CDatabaseEngineComp::GetDatabaseName() const
{
	if (m_databaseAccessSettingsCompPtr.IsValid()){
		return m_databaseAccessSettingsCompPtr->GetDatabaseName();
	}

	QString databaseName = *m_dbNameAttrPtr;

	if (databaseName.isEmpty()){
		QFileInfo fileInfo(GetDatabasePath());
		databaseName = fileInfo.fileName();
	}

	return databaseName;
}


QString CDatabaseEngineComp::GetDatabasePath() const
{
	if (m_databaseAccessSettingsCompPtr.IsValid()){
		return m_databaseAccessSettingsCompPtr->GetDatabasePath();
	}

	if (!m_dbFilePathCompPtr.IsValid()){
		SendErrorMessage(0, QT_TR_NOOP("Database file path incorrect"));

		return QString();
	}

	return m_dbFilePathCompPtr->GetPath();
}


bool CDatabaseEngineComp::IsSslConnectionRequired() const
{
	if (!m_databaseAccessSettingsCompPtr.IsValid()){
		return false;
	}

	return m_databaseAccessSettingsCompPtr->GetConnectionFlags() & IDatabaseLoginSettings::COF_SSL;
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


int CDatabaseEngineComp::GetDatabaseVersion() const
{
	QSqlError sqlError;
	QSqlQuery queryGetRevision = ExecSqlQueryFromFile(QStringLiteral(":/SQL/GetRevision.sql"), &sqlError);
	if (sqlError.type() != QSqlError::NoError){
		return -1;
	}

	if (queryGetRevision.next()){
		return queryGetRevision.value(0).toInt();
	}

	return -1;
}


QString CDatabaseEngineComp::GetConnectionOptionsString(const QByteArray& databaseDriverId) const
{
	if (!m_databaseAccessSettingsCompPtr.IsValid()){
		return QString();
	}

	QString retVal;
	int flags = m_databaseAccessSettingsCompPtr->GetConnectionFlags();
	if (flags > 0){
		if (databaseDriverId == QStringLiteral("PSQL") && (flags & imtdb::IDatabaseLoginSettings::COF_SSL)){
			retVal += QStringLiteral("requiressl=1");
		}
	}

	return retVal;
}


bool CDatabaseEngineComp::ExecuteTransaction(const QByteArray& sqlQuery) const
{
	if(!BeginTransaction()){
		qCritical() << __FILE__ << __LINE__
					<< "\n\t| what(): Transaction could not be started"
					<< "\n\t| Error: " << QSqlDatabase::database(GetConnectionName()).lastError().text();

		SendErrorMessage(0, QStringLiteral("Transaction could not be started: %1").arg(QSqlDatabase::database(GetConnectionName()).lastError().text()), __FILE__);

		return false;
	}

	QByteArray singleQuery;

	for (const char* ch = sqlQuery.begin(); ch != sqlQuery.end(); ch++){
		if (*ch != ';'){
			singleQuery.append(*ch);

			continue;
		}

		if (!singleQuery.isEmpty()){
			QSqlError error;
			ExecSqlQuery(singleQuery, &error);
			if (error.type() != QSqlError::NoError){
				SendErrorMessage(0, error.text(), __FILE__);

				CancelTransaction();

				return false;
			}

			singleQuery.clear();
		}
	}

	if (!singleQuery.isEmpty()){
		QSqlError error;
		ExecSqlQuery(singleQuery, &error);
		if (error.type() != QSqlError::NoError){
			SendErrorMessage(0, error.text(), __FILE__);

			CancelTransaction();

			return false;
		}
	}

	if(!FinishTransaction()){
		qCritical() << __FILE__ << __LINE__
					<< "\n\t| what(): Transaction could not be finished"
					<< "\n\t| Error: " << QSqlDatabase::database(GetConnectionName()).lastError().text();

		SendErrorMessage(0, QStringLiteral("Transaction could not be finished: %1").arg(QSqlDatabase::database(GetConnectionName()).lastError().text()), __FILE__);

		return false;
	}
	return true;
}


// private slots

void CDatabaseEngineComp::OnThreadFinished()
{
	QString connectionName = GetConnectionName();

	QSqlDatabase::removeDatabase(connectionName);
}


} // namespace imtdb


