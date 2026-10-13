// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// STL includes
#include <memory>

// ImtCore includes
#include <imtduckdb/CDuckSqlDriver.h>
#include <imtduckdb/IDuckConnection.h>

// 3rdParty includes
#include <duckdb.hpp>


namespace imtduckdb
{


/**
	IDuckConnection implementation owning its own duckdb::Connection to an already-open
	duckdb::DuckDB instance.

	duckdb::Connection is not thread-safe, but duckdb::DuckDB is, so concurrent workers (one per
	GraphQL request, or a cache builder running off-thread) each take their own Connection and run
	in parallel under DuckDB's MVCC instead of serializing on CDuckDatabaseEngineComp's shared
	connection. Created via CDuckDatabaseEngineComp::CreateConnection(); carries no mutex of its own
	- exclusive use by one thread at a time is the contract - and must not outlive the database it
	was created from.
*/
class CDuckConnectionEngine: public IDuckConnection
{
public:
	explicit CDuckConnectionEngine(duckdb::DuckDB& database);

	// reimplemented (imtduckdb::IDuckConnection)
	virtual std::unique_ptr<IDuckAppender> CreateAppender(
				const QString& tableName,
				const QString& schemaName = QString(),
				QString* errorMessagePtr = nullptr) override;
	virtual bool SwapTable(
				const QString& liveTableName,
				const QString& shadowTableName,
				QString* errorMessagePtr = nullptr) override;

	// reimplemented (imtdb::IDatabaseEngine)
	virtual bool BeginTransaction() const override;
	virtual bool FinishTransaction() const override;
	virtual bool CancelTransaction() const override;
	virtual QByteArray GetDatabaseDriverId() const override;
	virtual QSqlQuery ExecSqlQuery(const QByteArray& queryString, QSqlError* sqlError = nullptr, bool isForwardOnly = false) const override;
	virtual QSqlQuery ExecSqlQuery(const QByteArray& queryString, const QVariantMap& bindValues, QSqlError* sqlError = nullptr, bool isForwardOnly = false) const override;
	virtual QSqlQuery ExecSqlQueryFromFile(const QString& filePath, QSqlError* sqlError = nullptr, bool isForwardOnly = false) const override;
	virtual QSqlQuery ExecSqlQueryFromFile(const QString& filePath, const QVariantMap& bindValues, QSqlError* sqlError = nullptr, bool isForwardOnly = false) const override;

private:
	std::unique_ptr<duckdb::Connection> m_connectionPtr;
	std::unique_ptr<CDuckSqlDriver> m_driverPtr;
	mutable bool m_isTransactionActive = false;
};


} // namespace imtduckdb
