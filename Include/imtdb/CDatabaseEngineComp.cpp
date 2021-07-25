#include <imtdb/CDatabaseEngineComp.h>


// Qt includes
#include <QtCore/QDataStream>
#include <QtCore/QRegularExpression>


namespace imtdb
{


QString CDatabaseEngineComp::GetConnectionName() const
{
	return m_db.connectionName();
}


QSqlDatabase CDatabaseEngineComp::GetDatabase() const
{
	return const_cast<QSqlDatabase&>(m_db);
}


bool CDatabaseEngineComp::OpenDatabase()
{
	m_db = QSqlDatabase::addDatabase(*m_dbType, *m_dbName);

	m_db.setHostName(*m_hostName);
	m_db.setUserName(*m_userName);
	m_db.setPassword(*m_pasword);
	m_db.setDatabaseName(*m_dbName);

	int port = *m_port;
	m_db.setPort(port);

	return m_db.open();
}


void CDatabaseEngineComp::CloseDatabase()
{
	m_db.close();
}


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
					<< "\n\t| Executed query" << queryString
					   ;
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
					<< "\n\t| QExecuted query" << ::qPrintable(retVal.executedQuery())
					   ;
	}

	return retVal;
}


QSqlQuery CDatabaseEngineComp::ExecSqlQueryFromFile(const QByteArray& filePath, QSqlError* sqlError) const
{
	QByteArray queryString;
	QFile sqlQuetyFile(filePath);
	sqlQuetyFile.open(QFile::ReadOnly);
	queryString = sqlQuetyFile.readAll();
	sqlQuetyFile.close();

	return this->ExecSqlQuery(queryString, sqlError);
}


QSqlQuery CDatabaseEngineComp::ExecSqlQueryFromFile(const QByteArray& filePath, const QVariantMap& bindValues, QSqlError* sqlError) const
{
	QByteArray queryString;
	QFile sqlQuetyFile(filePath);
	sqlQuetyFile.open(QFile::ReadOnly);
	queryString = sqlQuetyFile.readAll();
	sqlQuetyFile.close();

	return this->ExecSqlQuery(queryString, bindValues, sqlError);
}


void CDatabaseEngineComp::DrectBindValue(QByteArray* string, const QByteArray& what, const QByteArray& expr)
{
	QRegularExpression regExp(what);

	auto globalMatch = regExp.globalMatch(*string);
	while(globalMatch.hasNext()){
		auto regMatch = globalMatch.next();
		if(regMatch.capturedEnd() < string->length()-1) 	{
			QChar nextSym = string->at(regMatch.capturedEnd());
			if(!nextSym.isLetter()){
				string->replace(regMatch.capturedStart(), regMatch.capturedLength(), expr);
			}

		}
		else {
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


bool CDatabaseEngineComp::EnsureDatabaseConnected() const
{
	bool isOpened = m_db.isOpen();
	if(!isOpened){
		CDatabaseEngineComp* enginePtr = const_cast<CDatabaseEngineComp*>(this);

		enginePtr->m_db.close();

		enginePtr->OpenDatabase();

		isOpened = m_db.isOpen();
		if(!isOpened){
			qCritical() << __FILE__ << __LINE__
						<< "\n\t| what(): Database Error Occured Unable to open database"
						<< "\n\t| Database error" << m_db.lastError().text();
		}
	}

	return isOpened;
}


} // namespace imtdb


