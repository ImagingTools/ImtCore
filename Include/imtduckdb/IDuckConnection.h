// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// STL includes
#include <memory>

// Qt includes
#include <QtCore/QString>

// ImtCore includes
#include <imtdb/IDatabaseEngine.h>
#include <imtduckdb/IDuckAppender.h>


namespace imtduckdb
{


/**
	An exclusively-owned duckdb::Connection exposed as a full database engine.

	duckdb::Connection is not thread-safe, but duckdb::DuckDB is: concurrent workers each take their
	own IDuckConnection and run in parallel under DuckDB's MVCC (writers do not block readers)
	instead of serializing on the shared connection of CDuckDatabaseEngineComp. Everything a cache
	builder needs - DDL, bulk append, table swap - is available here so that an entire rebuild can
	run off-thread without ever touching the shared connection.

	Instances are created by IDuckDatabaseMaintenance::CreateConnection(). A single instance must be
	used by a single thread at a time and must not outlive the database it was created from.
*/
class IDuckConnection: virtual public imtdb::IDatabaseEngine
{
public:
	/**
		Creates a bulk row-appender for \a tableName on this connection. Use instead of row-at-a-time
		INSERT for high-volume writes.
		\return nullptr on failure, in which case \a errorMessagePtr (if not null) is filled in.
	*/
	virtual std::unique_ptr<IDuckAppender> CreateAppender(
				const QString& tableName,
				const QString& schemaName = QString(),
				QString* errorMessagePtr = nullptr) = 0;

	/**
		Atomically replaces \a liveTableName with \a shadowTableName, so readers on other connections
		never observe a half-built table. See imtduckdb::SwapTable().
	*/
	virtual bool SwapTable(
				const QString& liveTableName,
				const QString& shadowTableName,
				QString* errorMessagePtr = nullptr) = 0;
};


} // namespace imtduckdb
