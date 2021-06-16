#pragma once


// Qt includes
//#include <QSqlDatabase>

// ACF includes
#include <ilog/TLoggerCompWrap.h>

// ImtCore includes
#include <imtdb/IDataBaseEngine.h>


namespace imtdb
{


class CDataBaseEngineComp :
			public ilog::CLoggerComponentBase,
			virtual public IDatabaseEngine
{
public:
	typedef ilog::CLoggerComponentBase BaseClass;

	CDataBaseEngineComp();

	I_BEGIN_COMPONENT(CDataBaseEngineComp);
		I_REGISTER_INTERFACE(IDatabaseEngine)
		I_ASSIGN(m_dbType, "DbType", "List of DbType used with corresponded handlers", true, "");
		I_ASSIGN(m_dbName, "DbName", "List of DbName used with corresponded handlers", true, "");
		I_ASSIGN(m_userName, "UserName", "List of command-UserName used with corresponded handlers", true, "");
		I_ASSIGN(m_pasword, "Pasword", "List of command-Pasword used with corresponded handlers", true, "");
		I_ASSIGN(m_hostName, "HostName", "List of hostName used with corresponded handlers", true, "");
		I_ASSIGN(m_port, "Port", "List of Port used with corresponded handlers", true, 5432);
	I_END_COMPONENT;

	// reimplemented (IDatabaseEngine)
	virtual	QString ConnectionName() const override;
	virtual QSqlDatabase GetDatabase() const override;
	bool OpenDataBase() override;
	void CloseDataBase() override;
	Q_REQUIRED_RESULT virtual QSqlQuery ExecSqlQuery(const QByteArray& queryString, QSqlError* sqlError = nullptr) const override;
	Q_REQUIRED_RESULT virtual QSqlQuery ExecSqlQuery(const QByteArray& queryString, const QVariantMap& bindValues, QSqlError* sqlError = nullptr) const override;
	Q_REQUIRED_RESULT virtual QSqlQuery ExecSqlQueryFromFile(const QByteArray& filePath, QSqlError* sqlError = nullptr) const override;
	Q_REQUIRED_RESULT virtual QSqlQuery ExecSqlQueryFromFile(const QByteArray& filePath, const QVariantMap& bindValues, QSqlError* sqlError = nullptr) const override;

	static void DrectBindValue(_Inout_ QByteArray* string, _In_ const QByteArray& what, _In_ const QByteArray& expr);
	static void DrectBindValueInsertDefault(_Inout_ QByteArray* string, _In_ const QByteArray& what);
	static void DrectBindValueUpdateDefault(_Inout_ QByteArray* string, _In_ const QByteArray& what);


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

