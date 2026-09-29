// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// STL includes
#include <memory>

// Qt includes
#include <QtCore/QString>

// ACF includes
#include <istd/IPolymorphic.h>

// ImtCore includes
#include <imtdb/IDatabaseEngine.h>
#include <imtduckdb/IDuckAppender.h>
#include <imtduckdb/IDuckConnection.h>


namespace imtduckdb
{


/**
	DuckDB-specific database engine operations that have no equivalent in the generic
	imtdb::IDatabaseEngine interface (bulk appends, atomic table swaps, per-request read
	connections). Implemented by CDuckDatabaseEngineComp and registered as a separate interface so
	callers that only need these operations (e.g. a cache builder) can depend on them without
	requiring the concrete component type.
*/
class IDuckDatabaseMaintenance: virtual public istd::IPolymorphic
{
public:
	/**
		Creates a bulk row-appender for \a tableName (optionally schema-qualified via \a
		schemaName), backed by duckdb::Appender. Use this instead of ExecSqlQuery() for
		high-volume inserts, e.g. full cache rebuilds - row-at-a-time INSERT is far too slow.
		\note The returned appender drives this component's shared connection, so it must only be used
			  from the component's own thread. Off-thread writers must take their own connection via
			  CreateConnection() and call IDuckConnection::CreateAppender() on it instead.
		\return nullptr if the database could not be opened or the appender could not be created
				(e.g. unknown table); in that case, \a errorMessagePtr (if not null) is filled with
				the error text.
	*/
	virtual std::unique_ptr<IDuckAppender> CreateAppender(
				const QString& tableName,
				const QString& schemaName = QString(),
				QString* errorMessagePtr = nullptr) const = 0;

	/**
		Creates an exclusively-owned connection to this component's database, able to run queries,
		DDL, bulk appends and table swaps independently of - and concurrently with - this component's
		shared connection (DuckDB's MVCC does not block readers against writers).

		This is the only safe way to work off-thread: the appender returned by CreateAppender() below
		drives the shared connection and is therefore usable only from the component's own thread.
		\return nullptr if the database could not be opened.
	*/
	virtual std::unique_ptr<IDuckConnection> CreateConnection() const = 0;

	/**
		Atomically replaces \a liveTableName with \a shadowTableName using ALTER TABLE ... RENAME
		inside a single transaction, so readers never observe a half-built table. \a shadowTableName
		no longer exists afterwards; if \a liveTableName did not exist yet (first build), it is
		simply adopted.
		\return true on success. On failure the transaction is rolled back, so both tables remain as
				they were before the call; \a errorMessagePtr, if not null, is filled with the error
				text.
	*/
	virtual bool SwapTable(
				const QString& liveTableName,
				const QString& shadowTableName,
				QString* errorMessagePtr = nullptr) const = 0;
};


} // namespace imtduckdb
