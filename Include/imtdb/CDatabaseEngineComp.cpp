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
	EnsureDatabaseConnected();

	QSqlQuery retVal(m_db);

	retVal.prepare(queryString);

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

	if(sqlError){
		*sqlError = m_db.lastError().type() ? m_db.lastError() : retVal.lastError();
	}

	if(m_db.lastError().type() || retVal.lastError().type()){
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

	return this->ExecSqlQuery(queryString, sqlError);
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
		if(regMatch.capturedEnd() < string->length()-1){
			QChar nextSym = string->at(regMatch.capturedEnd());
			if(!nextSym.isLetter()){
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
	if(newValue.startsWith(':')){
		newValue.remove(0,1);
		newValue.append('"');
		newValue.prepend('"');

		CDatabaseEngineComp::DrectBindValue(string, what, newValue);
	}
}


// protected methods

bool CDatabaseEngineComp::OpenDatabase() const
{
	bool retval = false;

	if (m_db.isOpen()){
		m_db.close();
	}

	m_db = QSqlDatabase::addDatabase(*m_dbType, *m_dbName);
	m_db.setHostName(*m_hostName);
	m_db.setUserName(*m_userName);
	m_db.setPassword(*m_pasword);
	m_db.setDatabaseName(*m_dbName);
	m_db.setPort(*m_port);
	retval = m_db.open();

	if (*m_autoCreateDatabase > 0){
		this->CreateDatabase();
	}

	if (*m_autoCreateTables > 0){
		this->CreateTables();
	}

	return retval;
}


bool CDatabaseEngineComp::CreateDatabase() const
{
	bool retval = false;

	QSqlDatabase maintainanceDb = QSqlDatabase::addDatabase(*m_dbType, *m_maintenanceDatabaseName);
	maintainanceDb.setHostName(*m_hostName);
	maintainanceDb.setUserName(*m_userName);
	maintainanceDb.setPassword(*m_pasword);
	maintainanceDb.setDatabaseName(*m_maintenanceDatabaseName);
	maintainanceDb.setPort(*m_port);
	retval = maintainanceDb.open();

	if(retval){

		QString queryString;

		if (m_databaseCreationScriptPath.IsValid() && QFile(*m_databaseCreationScriptPath).isReadable()){

			QFile scriptFile(*m_databaseCreationScriptPath);
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

		retval = bool(sqlError.type() == QSqlError::ErrorType::NoError);

		maintainanceDb.close();

		if (!retval){

			qCritical() << __FILE__ << __LINE__
						<< "\n\t| what(): Maintainance SQL error occured"
						<< "\n\t| error():" << sqlError
						<< "\n\t| query():" << queryString
						;

		}

		else{

			QSqlDatabase::removeDatabase(*m_maintenanceDatabaseName);

			m_db = QSqlDatabase::addDatabase(*m_dbType, *m_dbName);
			m_db.setHostName(*m_hostName);
			m_db.setUserName(*m_userName);
			m_db.setPassword(*m_pasword);
			m_db.setDatabaseName(*m_dbName);
			m_db.setPort(*m_port);
			retval = m_db.open();

		}

	}

	return retval;
}


bool CDatabaseEngineComp::CreateTables() const
{
	QSqlError sqlError;
	QString queryString;
	QFile scriptFile(*m_tablesCreationScriptPath);

	if(!scriptFile.open(QFile::ReadOnly)) {

		qCritical() << __FILE__ << __LINE__
					<< "\n\t| what(): Unable to open file"
					<< "\n\t| fileName():" << scriptFile.fileName();
		return false;

	}

	queryString = scriptFile.readAll();
	scriptFile.close();

	m_db.exec(queryString);

	sqlError = m_db.lastError();

	return bool(sqlError.type() == QSqlError::ErrorType::NoError);
}


// private methods

bool CDatabaseEngineComp::EnsureDatabaseConnected() const
{
	bool isOpened = m_db.isOpen();
	if(!isOpened){
		m_db.close();

		OpenDatabase();

		isOpened = m_db.isOpen();

		if (!isOpened && *m_autoCreateDatabase == 2){
			this->CreateDatabase();
			isOpened = m_db.isOpen();
		}

		if (!isOpened){
			qCritical() << __FILE__ << __LINE__
						<< "\n\t| what(): Database Error Occured Unable to open database"
						<< "\n\t| Database error" << m_db.lastError().text();
		}

	}

	return isOpened;
}


} // namespace imtdb


