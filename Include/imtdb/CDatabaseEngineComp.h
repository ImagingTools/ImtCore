#pragma once


// Qt includes
#include <QtSql/QSqlDatabase>

// ACF includes
#include <ilog/TLoggerCompWrap.h>

// ImtCore includes
#include <imtdb/IDatabaseEngine.h>


namespace imtdb
{


class CDatabaseEngineComp:
			public ilog::CLoggerComponentBase,
			virtual public IDatabaseEngine
{
public:
	typedef ilog::CLoggerComponentBase BaseClass;

	I_BEGIN_COMPONENT(CDatabaseEngineComp);
		I_REGISTER_INTERFACE(IDatabaseEngine)
		I_ASSIGN(m_dbType, "DbType", "The property holds database connections using the driver", true, "QPSQL");
		I_ASSIGN(m_dbName, "DbName", "The property holds connection's database name", true, "postgres");
		I_ASSIGN(m_userName, "UserName", "The property holds connection's user name", true, "postgres");
		I_ASSIGN(m_pasword, "Pasword", "The property holds connection's password.", true, "12345");
		I_ASSIGN(m_hostName, "HostName", "The property holds connection's host name.", true, "localhost");
		I_ASSIGN(m_maintenanceDatabaseName, "MaintainanceDatabase", "Name of Maintenance database. It's necessary for creating database if it not exists", true, "postgres");
		I_ASSIGN(m_autoCreateDatabase, "AutoCreateDatabase", "The property holds behavior to create database on startup.\n Possible values:\n0 - will not create new database;\n1 - will create database once;\n2 - will create database at each startup", true, 1);
		I_ASSIGN(m_autoCreateTables, "AutoCreateTables", "The property holds behavior to create tables on startup.\n Possible values:\n0 - will not create new tables;\n1 - will create tables once;\n2 - will create tables at each startup", true, 1);
		I_ASSIGN(m_port, "Port", "The property holds connection's port number", true, 5432);
	I_END_COMPONENT;

	// reimplemented (IDatabaseEngine)
	virtual QSqlQuery ExecSqlQuery(const QByteArray& queryString, QSqlError* sqlError = nullptr) const override;
	virtual QSqlQuery ExecSqlQuery(const QByteArray& queryString, const QVariantMap& bindValues, QSqlError* sqlError = nullptr) const override;
	virtual QSqlQuery ExecSqlQueryFromFile(const QByteArray& filePath, QSqlError* sqlError = nullptr) const override;
	virtual QSqlQuery ExecSqlQueryFromFile(const QByteArray& filePath, const QVariantMap& bindValues, QSqlError* sqlError = nullptr) const override;

	static void DrectBindValue(QByteArray* string,  const QByteArray& what,  const QByteArray& expr);
	static void DrectBindValueInsertDefault(QByteArray* string,  const QByteArray& what);
	static void DrectBindValueUpdateDefault(QByteArray* string,  const QByteArray& what);

protected:
	bool OpenDatabase() const;
	virtual bool CreateDatabase() const;
	virtual bool CreateTables() const;

private:
	/**
		Internal method for reconnection database on disconnect or not connected.
		\return \c true if the database could not be connected successfully or \c false otherwise.
	 */
	bool EnsureDatabaseConnected() const;

private:
	I_ATTR(QByteArray, m_dbType);
	I_ATTR(QByteArray, m_dbName);
	I_ATTR(QByteArray, m_userName);
	I_ATTR(QByteArray, m_pasword);
	I_ATTR(QByteArray, m_hostName);
	I_ATTR(QByteArray, m_maintenanceDatabaseName);
	I_ATTR(int, m_autoCreateDatabase);
	I_ATTR(int, m_autoCreateTables);
	I_ATTR(int, m_port);

	mutable QSqlDatabase m_db;
};


} // namespace imtdb


