#include <imtdb/CDatabaseEngineComp.h>


// Qt includes
#include <QtCore/QDataStream>
#include <QtCore/QRegularExpression>


namespace imtdb
{


// public methods

// reimplemented (IDatabaseEngine)

QSqlQuery CDatabaseEngineComp::ExecSqlQuery(const QByteArray& queryString, QSqlError* sqlErrorPtr) const
{
	if (!EnsureDatabaseConnected()){
		return QSqlQuery();
	}

	QSqlQuery retVal(m_db);

	bool success = retVal.prepare(queryString);
	if (!success){
		return retVal;
	}
	
	retVal.exec();

	if (sqlErrorPtr != nullptr){
		*sqlErrorPtr = m_db.lastError().type() ? m_db.lastError() : retVal.lastError();
	}

	if (m_db.lastError().type() || retVal.lastError().type()){
		qCritical() << __FILE__ << __LINE__
					<< "\n\t| what() sqlError Occured"
					<< "\n\t| Database error" << m_db.lastError().text()
					<< "\n\t| Query error" << retVal.lastError().text()
					<< "\n\t| Executed query" << queryString;
	}

	return retVal;
}


QSqlQuery CDatabaseEngineComp::ExecSqlQuery(const QByteArray& queryString, const QVariantMap& bindValues, QSqlError* sqlError) const
{
	if (!EnsureDatabaseConnected()){
		return QSqlQuery();
	}

	QSqlQuery retVal(m_db);
	retVal.prepare(queryString);

	for(auto value = bindValues.cbegin(); value != bindValues.cend(); ++ value){
		retVal.bindValue(value.key(), *value);
	}

	retVal.exec();

	if (sqlError){
		*sqlError = m_db.lastError().type() ? m_db.lastError() : retVal.lastError();
	}

	if (m_db.lastError().type() || retVal.lastError().type()){
		qCritical() << __FILE__ << __LINE__
					<< "\n\t| what(): sqlError Occured"
					<< "\n\t| Database error" << m_db.lastError().text()
					<< "\n\t| Query error" << retVal.lastError().text()
					<< "\n\t| Executed query" << queryString
					<< "\n\t| Bind Values" << bindValues
					<< "\n\t| Bound Values" << retVal.boundValues()
					<< "\n\t| QExecuted query" << ::qPrintable(retVal.executedQuery());
	}

	return retVal;
}


QSqlQuery CDatabaseEngineComp::ExecSqlQueryFromFile(const QByteArray& filePath, QSqlError* sqlError) const
{
	QFile sqlQuetyFile(filePath);

	sqlQuetyFile.open(QFile::ReadOnly);

	QByteArray queryString = sqlQuetyFile.readAll();

	sqlQuetyFile.close();

	return ExecSqlQuery(queryString, sqlError);
}


QSqlQuery CDatabaseEngineComp::ExecSqlQueryFromFile(const QByteArray& filePath, const QVariantMap& bindValues, QSqlError* sqlError) const
{
	QFile sqlQuetyFile(filePath);

	sqlQuetyFile.open(QFile::ReadOnly);

	QByteArray queryString = sqlQuetyFile.readAll();

	sqlQuetyFile.close();

	return this->ExecSqlQuery(queryString, bindValues, sqlError);
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
			string->replace(regMatch.capturedStart(), string->length()-1, expr);
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
		newValue.remove(0,1);
		newValue.append('"');
		newValue.prepend('"');

		CDatabaseEngineComp::DrectBindValue(string, what, newValue);
	}
}


// protected methods

bool CDatabaseEngineComp::OpenDatabase() const
{
	bool retVal = false;

	if (m_db.isOpen()){
		m_db.close();
	}

	m_db = QSqlDatabase::addDatabase(*m_dbType, *m_dbName);
	m_db.setHostName(*m_hostName);
	m_db.setUserName(*m_userName);
	m_db.setPassword(*m_pasword);
	m_db.setDatabaseName(*m_dbName);
	m_db.setPort(*m_port);
	
	retVal = m_db.open();
	if (!retVal){
		SendErrorMessage(0, "Database could not be connected", "Database engine");
	}

	return retVal;
}


bool CDatabaseEngineComp::CreateDatabase() const
{
	bool retVal = false;

	QSqlDatabase maintainanceDb = QSqlDatabase::addDatabase(*m_dbType, *m_maintenanceDatabaseNameAttrPtr);
	maintainanceDb.setHostName(*m_hostName);
	maintainanceDb.setUserName(*m_userName);
	maintainanceDb.setPassword(*m_pasword);
	maintainanceDb.setDatabaseName(*m_maintenanceDatabaseNameAttrPtr);
	maintainanceDb.setPort(*m_port);

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
			queryString.append(*m_dbName);
			queryString.append('"');
			queryString.append("WITH OWNER ");
			queryString.append('"');
			queryString.append(*m_userName);
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

			m_db = QSqlDatabase::addDatabase(*m_dbType, *m_dbName);

			m_db.setHostName(*m_hostName);
			m_db.setUserName(*m_userName);
			m_db.setPassword(*m_pasword);
			m_db.setDatabaseName(*m_dbName);
			m_db.setPort(*m_port);

			retVal = m_db.open();
		}
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

	m_db.exec(queryString);

	sqlError = m_db.lastError();
	if (sqlError.type() != QSqlError::ErrorType::NoError){
		qCritical() << __FILE__ << __LINE__
			<< "\n\t| Maintainance SQL error occured"
			<< "\n\t| Error: " << sqlError
			<< "\n\t| Query: " << queryString;
	}

	return sqlError.type() == QSqlError::ErrorType::NoError;
}


// reimplemented (icomp::CComponentBase)

void CDatabaseEngineComp::OnComponentCreated()
{
	BaseClass::OnComponentCreated();

	bool isOpened = OpenDatabase();

	if (!isOpened || *m_autoCreateDatabaseAttrPtr == 2){
		CreateDatabase();
	}
}


// private methods

bool CDatabaseEngineComp::EnsureDatabaseConnected() const
{
	bool isOpened = m_db.isOpen();
	if (!isOpened){
		m_db.close();

		isOpened = OpenDatabase();
		if (!isOpened){
			qCritical() << __FILE__ << __LINE__
						<< "\n\t| Unable to open database"
						<< "\n\t| Error: " << m_db.lastError().text();
		}
	}

	return isOpened;
}


} // namespace imtdb


