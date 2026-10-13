// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// Qt includes
#include <QtCore/QString>
#include <QtCore/QStringList>

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

	/**
		Logs the key of every row of \a stagingTableName that the merge into \a tableName will change: a row
		that is new, or differs from the cached one and passes \a replaceCondition (written against excluded
		and the table, as for the upsert). A row read again unchanged is not logged. Call it before the merge.
	\a changedCount receives the number of logged rows.
	*/
	static bool RecordUpserted(
				imtduckdb::IDuckConnection& connection,
				const QString& tableName,
				const QString& keyColumn,
				const QString& stagingTableName,
				const QString& replaceCondition,
				int& changedCount,
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

	/// The newest entry, or \a fallback when there is none because they were all purged.
	static bool GetLatestChangeId(imtduckdb::IDuckConnection& connection, qint64 fallback, qint64& changeId, QString& errorMessage);

	/// How far \a consumer has read. \a hasCursor is false before its first read.
	static bool GetCursor(imtduckdb::IDuckConnection& connection, const QString& consumer, bool& hasCursor, qint64& changeId, QString& errorMessage);

	static bool SetCursor(imtduckdb::IDuckConnection& connection, const QString& consumer, qint64 changeId, QString& errorMessage);

	/// Whether any of \a tableNames was rebuilt by an entry in (\a afterChangeId, \a upToChangeId].
	static bool HasRebuilt(
				imtduckdb::IDuckConnection& connection,
				const QStringList& tableNames,
				qint64 afterChangeId,
				qint64 upToChangeId,
				bool& hasRebuilt,
				QString& errorMessage);

	/// Drops the entries every consumer has read, or all of them when there is no consumer.
	static bool Purge(imtduckdb::IDuckConnection& connection, QString& errorMessage);
};


} // namespace imtcache
