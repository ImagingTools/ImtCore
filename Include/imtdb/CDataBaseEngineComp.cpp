// Qt includes
#include <QDataStream>
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
	QSqlQuery retval(queryString, m_db);
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
	QSqlQuery retval(queryString, m_db);
	for(auto value = bindValues.cbegin(); value != bindValues.cend(); ++value)
	{
		retval.bindValue(value.key(), *value);
	}
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




};// namespace imtdb

