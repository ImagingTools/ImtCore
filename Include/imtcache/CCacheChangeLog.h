// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// Qt includes
#include <QtCore/QString>

// ImtCore includes
#include <imtduckdb/IDuckConnection.h>


namespace imtcache
{


/**
	Remembers which rows of the mirror tables changed, so a table derived from them can recompute only
	the rows that depend on a change instead of all of them.

	The builders of the mirrors write here in the transaction that changes the row, so the log never
	disagrees with the table. A consumer reads the entries after its cursor and moves the cursor on;
	entries every consumer has read are purged at the end of an update.
*/
class CCacheChangeLog
{
public:
	/// Creates the log tables and their sequence when they are not there.
	static bool EnsureTables(imtduckdb::IDuckConnection& connection, QString& errorMessage);

	/// Logs the key of every row of \a stagingTableName, which is about to be merged into \a tableName.
	static bool RecordUpserted(
				imtduckdb::IDuckConnection& connection,
				const QString& tableName,
				const QString& keyColumn,
				const QString& stagingTableName,
				QString& errorMessage);

	/// Logs the key of every row of \a tableName matching the SQL condition \a removedCondition. Call it before the rows are deleted.
	static bool RecordRemoved(
				imtduckdb::IDuckConnection& connection,
				const QString& tableName,
				const QString& keyColumn,
				const QString& removedCondition,
				QString& errorMessage);

	/// Logs that \a tableName was rebuilt, which no key can describe.
	static bool RecordRebuilt(imtduckdb::IDuckConnection& connection, const QString& tableName, QString& errorMessage);

	/// Drops the entries every consumer has read, or all of them when there is no consumer.
	static bool Purge(imtduckdb::IDuckConnection& connection, QString& errorMessage);
};


} // namespace imtcache
