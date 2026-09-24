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
		\return nullptr if the database could not be opened or the appender could not be created
				(e.g. unknown table); in that case, \a errorMessagePtr (if not null) is filled with
				the error text.
	*/
	virtual std::unique_ptr<IDuckAppender> CreateAppender(
				const QString& tableName,
				const QString& schemaName = QString(),
				QString* errorMessagePtr = nullptr) const = 0;

	/**
		Creates a new imtdb::IDatabaseEngine backed by its own duckdb::Connection against this
		component's database, so callers can run queries concurrently with each other and with
		this component's own connection (DuckDB's MVCC does not block readers against writers).
		Intended for one-per-request use; the returned engine is not thread-safe by itself and must
		not outlive this component.
		\return nullptr if the database could not be opened.
	*/
	virtual std::unique_ptr<imtdb::IDatabaseEngine> CreateReadConnection() const = 0;

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
