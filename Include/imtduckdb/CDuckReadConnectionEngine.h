// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// STL includes
#include <memory>

// ImtCore includes
#include <imtdb/IDatabaseEngine.h>
#include <imtduckdb/CDuckSqlDriver.h>

// 3rdParty includes
#include <duckdb.hpp>


namespace imtduckdb
{


/**
	Lightweight imtdb::IDatabaseEngine backed by its own duckdb::Connection to an already-open
	duckdb::DuckDB instance.

	duckdb::Connection is not thread-safe, but duckdb::DuckDB is, so concurrent readers (e.g. one
	per GraphQL request) each need their own Connection to run in parallel under DuckDB's MVCC
	instead of serializing on CDuckDatabaseEngineComp's own connection. Instances are created via
	CDuckDatabaseEngineComp::CreateReadConnection() and are meant for one-per-request use; they are
	not thread-safe themselves and must not outlive the database they were created from.
*/
class CDuckReadConnectionEngine: public imtdb::IDatabaseEngine
{
public:
	explicit CDuckReadConnectionEngine(duckdb::DuckDB& database);

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
};


} // namespace imtduckdb
