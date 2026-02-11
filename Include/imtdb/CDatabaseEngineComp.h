// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// Qt includes
#include <QtSql/QSqlDatabase>

// ACF includes
#include <imod/TModelWrap.h>
#include <ilog/TLoggerCompWrap.h>
#include <ifile/IFileNameParam.h>

// ImtCore includes
#include <imtbase/TModelUpdateBinder.h>
#include <imtdb/IDatabaseEngine.h>
#include <imtdb/IDatabaseServerConnectionChecker.h>
#include <imtdb/CDatabaseAccessSettings.h>
#include <imtdb/IMigrationController.h>


namespace imtdb
{

class CDatabaseEngineAttr: public ilog::CLoggerComponentBase
{
public:
	typedef ilog::CLoggerComponentBase BaseClass;
	I_BEGIN_COMPONENT(CDatabaseEngineAttr);
	I_END_COMPONENT;
};


class CDatabaseEngineComp:
			public QObject,
			virtual public CDatabaseEngineAttr,
			virtual public IDatabaseEngine,
			virtual public IDatabaseServerConnectionChecker
{
	Q_OBJECT
public:
	typedef CDatabaseEngineAttr BaseClass;

	I_BEGIN_COMPONENT(CDatabaseEngineComp);
		I_REGISTER_INTERFACE(IDatabaseEngine)
		I_REGISTER_INTERFACE(IDatabaseServerConnectionChecker)
		I_REGISTER_SUBELEMENT(DatabaseAccessSettings);
		I_REGISTER_SUBELEMENT_INTERFACE(DatabaseAccessSettings, imtdb::IDatabaseLoginSettings, ExtractDatabaseAccessSettings);
		I_REGISTER_SUBELEMENT_INTERFACE(DatabaseAccessSettings, istd::IChangeable, ExtractDatabaseAccessSettings);
		I_REGISTER_SUBELEMENT_INTERFACE(DatabaseAccessSettings, imod::IModel, ExtractDatabaseAccessSettings);
		I_ASSIGN(m_databaseAccessSettingsCompPtr, "DatabaseAccessSettings", "Settings for database access", false, "DatabaseAccessSettings");
		I_ASSIGN(m_dbTypeAttrPtr, "DbType", "The property holds database connections using the driver", true, "QPSQL");
		I_ASSIGN(m_dbNameAttrPtr, "DbName", "The property holds connection's database name", true, "");
		I_ASSIGN(m_dbFilePathCompPtr, "DbPath", "Database path for a file-based database", false, "");
		I_ASSIGN(m_userNameAttrPtr, "UserName", "The property holds connection's user name", true, "postgres");
		I_ASSIGN(m_paswordAttrPtr, "Pasword", "The property holds connection's password.", true, "12345");
		I_ASSIGN(m_hostNameAttrPtr, "HostName", "The property holds connection's host name.", true, "localhost");
		I_ASSIGN(m_maintenanceDatabaseNameAttrPtr, "MaintainanceDatabase", "Name of Maintenance database. It's necessary for creating database if it not exists", true, "postgres");
		I_ASSIGN(m_autoCreateDatabaseAttrPtr, "AutoCreateDatabase", "The property holds behavior to create database on startup.\n Possible values:\n0 - will not create new database;\n1 - will create database once;\n2 - will create database at each startup", true, 1);
		I_ASSIGN(m_autoCreateTablesAttrPtr, "AutoCreateTables", "The property holds behavior to create tables on startup.\n Possible values:\n0 - will not create new tables;\n1 - will create tables once;\n2 - will create tables at each startup", true, 1);
		I_ASSIGN(m_portAttrPtr, "Port", "The property holds connection's port number", true, 5432);
		I_ASSIGN(m_migrationControllerCompPtr, "MigrationController", "Migration controller", false, "MigrationController");
	I_END_COMPONENT;

	CDatabaseEngineComp();

	// reimplemented (IDatabaseEngine)
	virtual bool BeginTransaction() const override;
	virtual bool FinishTransaction() const override;
	virtual bool CancelTransaction() const override;
	virtual QSqlQuery ExecSqlQuery(const QByteArray& queryString, QSqlError* sqlError = nullptr, bool isForwardOnly = false) const override;
	virtual QSqlQuery ExecSqlQuery(const QByteArray& queryString, const QVariantMap& bindValues, QSqlError* sqlError = nullptr, bool isForwardOnly = false) const override;
	virtual QSqlQuery ExecSqlQueryFromFile(const QString& filePath, QSqlError* sqlError = nullptr, bool isForwardOnly = false) const override;
	virtual QSqlQuery ExecSqlQueryFromFile(const QString& filePath, const QVariantMap& bindValues, QSqlError* sqlError = nullptr, bool isForwardOnly = false) const override;

	// reimplemented (IDatabaseServerConnectionChecker)
	virtual bool CheckDatabaseConnection(QString& errorMessage) const override;

	static void DrectBindValue(QByteArray* string, const QByteArray& what, const QByteArray& expr);
	static void DrectBindValueInsertDefault(QByteArray* string, const QByteArray& what);
	static void DrectBindValueUpdateDefault(QByteArray* string, const QByteArray& what);

	QString GetConnectionName() const;

protected:
	enum DatabaseCreationFlags
	{
		DCF_CREATE_DATABASE = 1,
		DCF_CREATE_DATABASE_META = 2,
		DCF_EXECUTE_PATCHES = 4,
		DCF_ALL = 0xffff
	};
	virtual bool OpenDatabase() const;
	virtual bool CreateDatabase(int flags) const;
	virtual bool ExecuteDatabasePatches() const;
	virtual bool ExecuteTransaction(const QByteArray& sqlQuery) const;
	void OnDatabaseAccessChanged(const istd::IChangeable::ChangeSet& changeSet, const imtdb::IDatabaseLoginSettings* databaseAccessSettingsPtr);
	QSqlDatabase InitDatabase(const QByteArray& databaseDriverTypeId) const;

	// reimplemented (icomp::CComponentBase)
	virtual void OnComponentCreated() override;
	virtual void OnComponentDestroyed() override;

private:
	/**
		Internal method for reconnection database on disconnect or not connected.
		\value sqlError[optional] - use it to track sql errors during the connection check
		\return \c true if the database could not be connected successfully or \c false otherwise.
	 */
	bool EnsureDatabaseConnected(QSqlError* sqlError = nullptr) const;

	/**
		Ensure that the database is created and ready to use.
	*/
	bool EnsureDatabaseCreated() const;

	/**
		Ensure that the database in the consistent state.
	*/
	bool EnsureDatabaseConsistency() const;

	/**
		Create the database instance (an empty database)
	*/
	bool CreateDatabaseInstance() const;

	/**
		Create special meta-info tables for the database (Revision etc.)
	*/
	bool CreateDatabaseMetaInfo() const;

	QString GetDatabaseName() const;
	QString GetDatabasePath() const;
	bool IsSslConnectionRequired() const;
	QString GetHostName() const;
	int GetPort() const;
	QString GetUserName() const;
	QString GetPassword() const;
	int GetDatabaseVersion() const;

	QString GetConnectionOptionsString(const QByteArray& databaseDriverId) const;

	template <typename Interface>
	static Interface* ExtractDatabaseAccessSettings(CDatabaseEngineComp& component)
	{
		return &component.m_workingAccessSettings;
	}

private Q_SLOTS:
	void OnThreadFinished();

private:
	I_REF(imtdb::IDatabaseLoginSettings, m_databaseAccessSettingsCompPtr);
	I_ATTR(QByteArray, m_dbTypeAttrPtr);
	I_ATTR(QByteArray, m_dbNameAttrPtr);
	I_REF(ifile::IFileNameParam, m_dbFilePathCompPtr);
	I_ATTR(QByteArray, m_userNameAttrPtr);
	I_ATTR(QByteArray, m_paswordAttrPtr);
	I_ATTR(QByteArray, m_hostNameAttrPtr);
	I_ATTR(QByteArray, m_maintenanceDatabaseNameAttrPtr);
	I_ATTR(QByteArray, m_databaseCreationScriptPathAttrPtr);
	I_ATTR(int, m_autoCreateDatabaseAttrPtr);
	I_ATTR(int, m_autoCreateTablesAttrPtr);
	I_ATTR(int, m_portAttrPtr);
	I_REF(imtdb::IMigrationController, m_migrationControllerCompPtr);

	imtbase::TModelUpdateBinder<imtdb::IDatabaseLoginSettings, CDatabaseEngineComp> m_databaseAccessObserver;

	imod::TModelWrap<imtdb::CDatabaseAccessSettings> m_workingAccessSettings;
};


} // namespace imtdb


