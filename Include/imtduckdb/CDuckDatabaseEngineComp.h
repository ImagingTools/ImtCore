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
#include <imtduckdb/CDuckConnectionEngine.h>
#include <imtduckdb/IDuckConnectionProvider.h>


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
			virtual public imtdb::IDatabaseEngine,
			virtual public IDuckConnectionProvider
{
public:
	typedef CDuckDatabaseEngineAttr BaseClass;

	I_BEGIN_COMPONENT(CDuckDatabaseEngineComp);
		I_REGISTER_INTERFACE(imtdb::IDatabaseEngine)
		I_REGISTER_INTERFACE(IDuckConnectionProvider)
		I_ASSIGN(m_dbFilePathCompPtr, "DbPath", "Path to the DuckDB database file. Empty means an in-memory database", false, "");
		I_ASSIGN(m_dbNameAttrPtr, "DbName", "Logical name of the database (used for diagnostic messages only)", true, "duckdb");
		I_ASSIGN(m_migrationControllerCompPtr, "MigrationController", "Migration controller", false, "MigrationController");
		I_ASSIGN(m_memoryLimitAttrPtr, "MemoryLimit", "Maximum memory used by the database, e.g. '4GB'. Empty means DuckDB's default (~80% of system memory)", true, "");
		I_ASSIGN(m_threadCountAttrPtr, "ThreadCount", "Maximum number of CPU threads used by the database. 0 means DuckDB's default (all available)", true, 0);
		I_ASSIGN(m_checkpointThresholdAttrPtr, "CheckpointThreshold", "Checkpoint when the WAL reaches this size, e.g. '1GB'. Empty means DuckDB's default (16MB)", true, "");
		I_ASSIGN(m_readOnlyAttrPtr, "ReadOnly", "Opens the database in read-only mode. The database file must already exist", true, false);
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

	/**
		Creates an exclusively-owned connection to this component's database, able to run queries,
		DDL, bulk appends and table swaps concurrently with this component's shared connection.
		Intended for one-per-request reads and for cache builders running off-thread.
		\return nullptr if the database could not be opened.
	*/
	std::unique_ptr<IDuckConnection> CreateConnection() const override;

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

	bool EndTransaction(const char* statement, const QString& actionName) const;
	bool ReadSqlFile(const QString& filePath, QByteArray& queryString, QSqlError* sqlErrorPtr) const;

private:
	I_REF(ifile::IFileNameParam, m_dbFilePathCompPtr);
	I_ATTR(QByteArray, m_dbNameAttrPtr);
	I_REF(imtdb::IMigrationController, m_migrationControllerCompPtr);
	I_ATTR(QByteArray, m_memoryLimitAttrPtr);
	I_ATTR(int, m_threadCountAttrPtr);
	I_ATTR(QByteArray, m_checkpointThresholdAttrPtr);
	I_ATTR(bool, m_readOnlyAttrPtr);

private:
	mutable std::recursive_mutex m_connectionMutex;
	mutable std::unique_ptr<duckdb::DuckDB> m_databasePtr;
	mutable std::unique_ptr<duckdb::Connection> m_connectionPtr;
	mutable std::unique_ptr<CDuckSqlDriver> m_driverPtr;
	mutable bool m_isTransactionActive = false;
};


} // namespace imtduckdb
