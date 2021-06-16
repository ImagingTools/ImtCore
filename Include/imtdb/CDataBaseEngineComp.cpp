// Qt includes
#include <QDataStream>
#include <QRegularExpression>

// ACF includes

// ImtCore includes
#include <imtdb/CDataBaseEngineComp.h>


namespace imtdb
{
	
CDataBaseEngineComp::CDataBaseEngineComp()
{
}
	
QString CDataBaseEngineComp::ConnectionName() const
{
	return m_db.connectionName();
}

QSqlDatabase CDataBaseEngineComp::GetDatabase() const
{
	return const_cast<QSqlDatabase&>(m_db);
}

bool CDataBaseEngineComp::OpenDataBase()
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

void CDataBaseEngineComp::CloseDataBase()
{
	m_db.close();
}

QSqlQuery CDataBaseEngineComp::ExecSqlQuery(const QByteArray& queryString, QSqlError* sqlError) const
{
	QSqlQuery retval(m_db);
	retval.prepare(queryString);
	retval.exec();
	if(sqlError)
	{
		*sqlError = m_db.lastError().type() ? m_db.lastError() : retval.lastError();
	}
	if(m_db.lastError().type() || retval.lastError().type())
	{
		qCritical() << __FILE__ << __LINE__
					<< "\n\t| what() sqlError Occured"
					<< "\n\t| DataBase error" << m_db.lastError().text()
					<< "\n\t| Query error" << retval.lastError().text()
					<< "\n\t| Executed query" << queryString
					   ;
	}
	return retval;
}

QSqlQuery CDataBaseEngineComp::ExecSqlQuery(const QByteArray& queryString, const QVariantMap& bindValues, QSqlError* sqlError) const
{
	QSqlQuery retval(m_db);
	retval.prepare(queryString);
	retval.exec();
	if(sqlError)
	{
		*sqlError = m_db.lastError().type() ? m_db.lastError() : retval.lastError();
	}
	if(m_db.lastError().type() || retval.lastError().type())
	{
		qCritical() << __FILE__ << __LINE__
					<< "\n\t| what(): sqlError Occured"
					<< "\n\t| DataBase error" << m_db.lastError().text()
					<< "\n\t| Query error" << retval.lastError().text()
					<< "\n\t| Executed query" << queryString
					<< "\n\t| Values" << bindValues
					   ;
	}
	return retval;
}

QSqlQuery CDataBaseEngineComp::ExecSqlQueryFromFile(const QByteArray& filePath, QSqlError* sqlError) const
{
	QByteArray queryString;
	QFile sqlQuetyFile(filePath);
	sqlQuetyFile.open(QFile::ReadOnly);
	queryString = sqlQuetyFile.readAll();
	sqlQuetyFile.close();

	return this->ExecSqlQuery(queryString, sqlError);
}

QSqlQuery CDataBaseEngineComp::ExecSqlQueryFromFile(const QByteArray& filePath, const QVariantMap& bindValues, QSqlError* sqlError) const
{
	QByteArray queryString;
	QFile sqlQuetyFile(filePath);
	sqlQuetyFile.open(QFile::ReadOnly);
	queryString = sqlQuetyFile.readAll();
	sqlQuetyFile.close();

	return this->ExecSqlQuery(queryString, bindValues, sqlError);
}

void CDataBaseEngineComp::DrectBindValue(QByteArray* string, const QByteArray& what, const QByteArray& expr)
{
	QRegularExpression regExp(what);

	auto globalMatch = regExp.globalMatch(*string);
	while( globalMatch.hasNext() )
	{
		auto regMatch = globalMatch.next();
		if(regMatch.capturedEnd() < string->length()-1)
		{
			QChar nextSym = string->at(regMatch.capturedEnd());
			if(!nextSym.isLetter())
			{
				string->replace(regMatch.capturedStart(), regMatch.capturedLength(), expr);
			}
		}
		else
		{
			string->replace(regMatch.capturedStart(), string->length()-1, expr);
		}
	}
}

void CDataBaseEngineComp::DrectBindValueInsertDefault(QByteArray* string, const QByteArray& what)
{
	CDataBaseEngineComp::DrectBindValue(string, what, " DEFAULT ");
}

void CDataBaseEngineComp::DrectBindValueUpdateDefault(QByteArray* string, const QByteArray& what)
{
	QByteArray newValue = what;
	if(newValue.startsWith(':'))
	{
		newValue.remove(0,1);
		newValue.append('"');
		newValue.prepend('"');
		CDataBaseEngineComp::DrectBindValue(string, what, newValue);
	}
}


};// namespace imtdb

