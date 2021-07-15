#pragma once


// Qt includes
#include <QtSql/QSqlDatabase>

// ACF includes
#include <ilog/TLoggerCompWrap.h>

// ImtCore includes
#include <imtdb/IDataBaseEngine.h>


namespace imtdb
{


class CDataBaseEngineComp:
			public ilog::CLoggerComponentBase,
			virtual public IDataBaseEngine
{
public:
	typedef ilog::CLoggerComponentBase BaseClass;

	I_BEGIN_COMPONENT(CDataBaseEngineComp);
		I_REGISTER_INTERFACE(IDataBaseEngine)
		I_ASSIGN(m_dbType, "DbType", "List of DbType used with corresponded handlers", true, "");
		I_ASSIGN(m_dbName, "DbName", "List of DbName used with corresponded handlers", true, "");
		I_ASSIGN(m_userName, "UserName", "List of command-UserName used with corresponded handlers", true, "");
		I_ASSIGN(m_pasword, "Pasword", "List of command-Pasword used with corresponded handlers", true, "");
		I_ASSIGN(m_hostName, "HostName", "List of hostName used with corresponded handlers", true, "");
		I_ASSIGN(m_port, "Port", "List of Port used with corresponded handlers", true, 5432);
	I_END_COMPONENT;

	// reimplemented (IDatabaseEngine)
	virtual	QString GetConnectionName() const override;
	virtual QSqlDatabase GetDatabase() const override;
	bool OpenDataBase() override;
	void CloseDataBase() override;
	virtual QSqlQuery ExecSqlQuery(const QByteArray& queryString, QSqlError* sqlError = nullptr) const override;
	virtual QSqlQuery ExecSqlQuery(const QByteArray& queryString, const QVariantMap& bindValues, QSqlError* sqlError = nullptr) const override;
	virtual QSqlQuery ExecSqlQueryFromFile(const QByteArray& filePath, QSqlError* sqlError = nullptr) const override;
	virtual QSqlQuery ExecSqlQueryFromFile(const QByteArray& filePath, const QVariantMap& bindValues, QSqlError* sqlError = nullptr) const override;

	static void DrectBindValue(QByteArray* string,  const QByteArray& what,  const QByteArray& expr);
	static void DrectBindValueInsertDefault(QByteArray* string,  const QByteArray& what);
	static void DrectBindValueUpdateDefault(QByteArray* string,  const QByteArray& what);

private:
	/*!
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
	I_ATTR(int, m_port);

	QSqlDatabase m_db;
};


} // namespace imtdb


