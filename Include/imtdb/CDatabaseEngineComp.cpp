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

	bool success = retVal.prepare(queryString);
	if (!success){
		if (sqlErrorPtr != nullptr){
			*sqlErrorPtr = retVal.lastError();
		}
	
		return QSqlQuery();
	}
	
	success = retVal.exec();

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


QSqlQuery CDatabaseEngineComp::ExecSqlQueryFromFile(const QByteArray& filePath, QSqlError* sqlError, bool isForwardOnly) const
{
	QFile sqlQuetyFile(filePath);

	sqlQuetyFile.open(QFile::ReadOnly);

	QByteArray queryString = sqlQuetyFile.readAll();

	sqlQuetyFile.close();

	return ExecSqlQuery(queryString, sqlError, isForwardOnly);
}


QSqlQuery CDatabaseEngineComp::ExecSqlQueryFromFile(const QByteArray& filePath, const QVariantMap& bindValues, QSqlError* sqlError, bool isForwardOnly) const
{
	QFile sqlQuetyFile(filePath);

	sqlQuetyFile.open(QFile::ReadOnly);

	QByteArray queryString = sqlQuetyFile.readAll();

	sqlQuetyFile.close();

	return ExecSqlQuery(queryString, bindValues, sqlError, isForwardOnly);
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
	if (!retVal){
		SendErrorMessage(0, "Database could not be connected", "Database engine");
	}

	return retVal;
}


bool CDatabaseEngineComp::CreateDatabase() const
{
	bool retVal = false;

	QSqlDatabase maintainanceDb = QSqlDatabase::addDatabase(*m_dbTypeAttrPtr, *m_maintenanceDatabaseNameAttrPtr);
	maintainanceDb.setHostName(GetHostName());
	maintainanceDb.setUserName(GetUserName());
	maintainanceDb.setPassword(GetPassword());
	maintainanceDb.setDatabaseName(*m_maintenanceDatabaseNameAttrPtr);
	maintainanceDb.setPort(GetPort());

	retVal = maintainanceDb.open();
	if (retVal){
		QString queryString;

		if (m_databaseCreationScriptPathAttrPtr.IsValid()){
			QFile scriptFile(*m_databaseCreationScriptPathAttrPtr);

			scriptFile.open(QFile::ReadOnly);

			queryString = scriptFile.readAll();

			scriptFile.close();
		}
		else{
			queryString = "CREATE DATABASE ";
			queryString.append('"');
			queryString.append(GetDatabaseName());
			queryString.append('"');
			queryString.append("WITH OWNER ");
			queryString.append('"');
			queryString.append(GetUserName());
			queryString.append('"');
		}

		maintainanceDb.exec(queryString);

		QSqlError sqlError;
		sqlError = maintainanceDb.lastError();

		maintainanceDb.close();

		retVal = bool(sqlError.type() == QSqlError::ErrorType::NoError);
		if (!retVal){
			qCritical() << __FILE__ << __LINE__
						<< "\n\t| Maintainance SQL error occured"
						<< "\n\t| Error: " << sqlError
						<< "\n\t| Query: " << queryString;
		}
		else{
			QSqlDatabase::removeDatabase(*m_maintenanceDatabaseNameAttrPtr);

			QSqlDatabase databaseConnection = QSqlDatabase::addDatabase(*m_dbTypeAttrPtr, GetConnectionName());

			databaseConnection.setHostName(GetHostName());
			databaseConnection.setUserName(GetUserName());
			databaseConnection.setPassword(GetPassword());
			databaseConnection.setDatabaseName(GetDatabaseName());
			databaseConnection.setPort(GetPort());

			retVal = databaseConnection.open();
		}
	}
	else{
		SendErrorMessage(0, QString("Maintanance database could not be opened. Error message: '%1'").arg(maintainanceDb.lastError().text()));
	}

	if (retVal){
		if (*m_autoCreateTablesAttrPtr > 0 && m_tablesCreationScriptPathAttrPtr.IsValid()){
			retVal = CreateTables();
		}
	}

	return retVal;
}


bool CDatabaseEngineComp::CreateTables() const
{
	QSqlError sqlError;
	QString queryString;
	QFile scriptFile(*m_tablesCreationScriptPathAttrPtr);

	if (!scriptFile.open(QFile::ReadOnly)){
		qCritical() << __FILE__ << __LINE__
					<< "\n\t| Unable to open file"
					<< "\n\t| File: " << scriptFile.fileName();

		return false;
	}

	queryString = scriptFile.readAll();
	scriptFile.close();

	QSqlDatabase databaseConnection = QSqlDatabase::database(GetConnectionName());

	databaseConnection.exec(queryString);

	sqlError = databaseConnection.lastError();
	if (sqlError.type() != QSqlError::ErrorType::NoError){
		qCritical() << __FILE__ << __LINE__
			<< "\n\t| Maintainance SQL error occured"
			<< "\n\t| Error: " << sqlError
			<< "\n\t| Query: " << queryString;
	}

	return sqlError.type() == QSqlError::ErrorType::NoError;
}


void CDatabaseEngineComp::OnDatabaseAccessChanged(const istd::IChangeable::ChangeSet& /*changeSet*/, const imtdb::IDatabaseLoginSettings* databaseAccessSettingsPtr)
{
	bool isOpened = OpenDatabase();

	if (!isOpened || *m_autoCreateDatabaseAttrPtr == 2){
		CreateDatabase();
	}

	Q_ASSERT(databaseAccessSettingsPtr != nullptr);
	if (databaseAccessSettingsPtr != nullptr){
		m_workingAccessSettings.CopyFrom(*databaseAccessSettingsPtr);
	}
}


// reimplemented (icomp::CComponentBase)

void CDatabaseEngineComp::OnComponentCreated()
{
	BaseClass::OnComponentCreated();

	m_databaseAccessObserver.RegisterObject(m_databaseAccessSettingsCompPtr.GetPtr(), &CDatabaseEngineComp::OnDatabaseAccessChanged);
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
		}
	}

	if(sqlError != nullptr){
		*sqlError = databaseConnection.lastError();
	}

	return isOpened;
}


QString CDatabaseEngineComp::GetConnectionName() const
{	
	qptrdiff threadId = (qptrdiff)QThread::currentThreadId();
	QString suffix;
	if (m_suffixAttrPtr.IsValid()){
		//suffix = *m_suffixAttrPtr;
	}

	return GetDatabaseName() + QString(" - %1 - %2").arg(threadId).arg(suffix);
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


} // namespace imtdb


