// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// Qt includes
#include <QtCore/QString>
#include <QtCore/QVariantList>


namespace imtduckdb
{


/**
	Bulk row-appender for a DuckDB table, backed by duckdb::Appender.

	Unlike imtdb::IDatabaseEngine::ExecSqlQuery() (row-at-a-time INSERT through the SQL parser),
	this interface is meant for high-throughput cache rebuilds: AppendRow() buffers rows in a
	columnar chunk and Flush()/Close() commit them via DuckDB's native appender, which is orders of
	magnitude faster than row-wise INSERT for bulk loads.

	Not thread-safe; instances are obtained from CDuckDatabaseEngineComp::CreateAppender() and are
	meant to be used by a single writer (e.g. the cache builder) for the duration of one table load.
*/
class IDuckAppender
{
public:
	virtual ~IDuckAppender() = default;

	/**
		Appends a single row. \a rowValues must have exactly one entry per active column of the
		target table, in column order.
		\return true on success, false if the row could not be appended (see GetLastError()).
	*/
	virtual bool AppendRow(const QVariantList& rowValues) = 0;

	//! Commits all buffered rows to the table.
	virtual bool Flush() = 0;

	//! Flushes and closes the appender. AppendRow() must not be called afterwards.
	virtual bool Close() = 0;

	//! Returns the last error message, if any of the calls above returned false.
	virtual QString GetLastError() const = 0;
};


} // namespace imtduckdb
