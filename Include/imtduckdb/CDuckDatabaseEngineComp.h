// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// STL includes
#include <memory>
#include <mutex>

// Qt includes
#include <QtSql/QSqlError>
#include <QtSql/QSqlQuery>

// 3rdParty includes
#include <duckdb.hpp>

// ACF includes
#include <ilog/TLoggerCompWrap.h>
#include <ifile/IFileNameParam.h>

// ImtCore includes
#include <imtdb/IDatabaseEngine.h>
#include <imtdb/IMigrationController.h>
#include <imtduckdb/CDuckSqlDriver.h>


namespace imtduckdb
{


class CDuckDatabaseEngineAttr: public ilog::CLoggerComponentBase
{
public:
	typedef ilog::CLoggerComponentBase BaseClass;
	I_BEGIN_COMPONENT(CDuckDatabaseEngineAttr);
	I_END_COMPONENT;
};


/**
	DuckDB-backed implementation of imtdb::IDatabaseEngine.

	Unlike imtdb::CDatabaseEngineComp (which this component does *not* derive from), queries are
	executed through the DuckDB C++ API (duckdb.hpp) instead of a QSqlDatabase driver. Results are
	still exposed as QSqlQuery/QSqlError (via CDuckSqlDriver/CDuckSqlResult), so the component is a
	drop-in replacement wherever only imtdb::IDatabaseEngine is used.
*/
class CDuckDatabaseEngineComp:
			virtual public CDuckDatabaseEngineAttr,
			virtual public imtdb::IDatabaseEngine
{
public:
	typedef CDuckDatabaseEngineAttr BaseClass;

	I_BEGIN_COMPONENT(CDuckDatabaseEngineComp);
		I_REGISTER_INTERFACE(imtdb::IDatabaseEngine)
		I_ASSIGN(m_dbFilePathCompPtr, "DbPath", "Path to the DuckDB database file. Empty means an in-memory database", false, "");
		I_ASSIGN(m_dbNameAttrPtr, "DbName", "Logical name of the database (used for diagnostic messages only)", true, "duckdb");
		I_ASSIGN(m_migrationControllerCompPtr, "MigrationController", "Migration controller", false, "MigrationController");
	I_END_COMPONENT;

	// reimplemented (imtdb::IDatabaseEngine)
	virtual bool BeginTransaction() const override;
	virtual bool FinishTransaction() const override;
	virtual bool CancelTransaction() const override;
	virtual QByteArray GetDatabaseDriverId() const override;
	virtual QSqlQuery ExecSqlQuery(const QByteArray& queryString, QSqlError* sqlError = nullptr, bool isForwardOnly = false) const override;
	virtual QSqlQuery ExecSqlQuery(const QByteArray& queryString, const QVariantMap& bindValues, QSqlError* sqlError = nullptr, bool isForwardOnly = false) const override;
	virtual QSqlQuery ExecSqlQueryFromFile(const QString& filePath, QSqlError* sqlError = nullptr, bool isForwardOnly = false) const override;
	virtual QSqlQuery ExecSqlQueryFromFile(const QString& filePath, const QVariantMap& bindValues, QSqlError* sqlError = nullptr, bool isForwardOnly = false) const override;

protected:
	// reimplemented (icomp::CComponentBase)
	virtual void OnComponentCreated() override;
	virtual void OnComponentDestroyed() override;

private:
	/**
		Lazily opens the DuckDB database/connection (on first use) and runs pending migrations.
		\return \c true if the database is open and ready to use.
	*/
	bool EnsureDatabaseOpen() const;
	/**
		Creates the "Revisions" meta-info table used to track applied migrations, if it does not
		already exist.
	*/
	bool CreateDatabaseMetaInfo() const;
	/**
		Runs pending migrations through the (optional) MigrationController.
	*/
	bool ExecuteDatabasePatches() const;
	/**
		Reads the current schema revision from the "Revisions" table.
		\return the highest applied revision, or -1 if none has been set yet.
	*/
	int GetDatabaseVersion() const;

	QString GetDatabasePath() const;

private:
	I_REF(ifile::IFileNameParam, m_dbFilePathCompPtr);
	I_ATTR(QByteArray, m_dbNameAttrPtr);
	I_REF(imtdb::IMigrationController, m_migrationControllerCompPtr);

private:
	mutable std::mutex m_connectionMutex;
	mutable std::unique_ptr<duckdb::DuckDB> m_databasePtr;
	mutable std::unique_ptr<duckdb::Connection> m_connectionPtr;
	mutable std::unique_ptr<CDuckSqlDriver> m_driverPtr;
};


} // namespace imtduckdb
