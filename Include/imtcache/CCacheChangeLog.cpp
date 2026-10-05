#include <imtcache/CCacheChangeLog.h>


// Qt includes
#include <QtCore/QFile>
#include <QtSql/QSqlError>

// ImtCore includes
#include <imtcache/imtcache.h>
#include <imtdb/imtdb.h>


namespace imtcache
{


namespace
{


bool Execute(imtduckdb::IDuckConnection& connection, const QString& query, QString& errorMessage)
{
	QSqlError sqlError;
	connection.ExecSqlQuery(query.toUtf8(), &sqlError);
	if (sqlError.type() != QSqlError::NoError){
		errorMessage = sqlError.text();

		return false;
	}

	return true;
}


bool ExecuteScript(imtduckdb::IDuckConnection& connection, const QString& scriptPath, QString& errorMessage)
{
	QFile scriptFile(scriptPath);
	if (!scriptFile.open(QFile::ReadOnly)){
		errorMessage = QStringLiteral("Unable to read %1").arg(scriptPath);

		return false;
	}

	return Execute(connection, QString::fromUtf8(scriptFile.readAll()), errorMessage);
}


QString NextChangeId()
{
	return QStringLiteral("nextval('%1')").arg(CacheChangeSequence::NAME);
}


} // anonymous namespace


// public methods

bool CCacheChangeLog::EnsureTables(imtduckdb::IDuckConnection& connection, QString& errorMessage)
{
	return ExecuteScript(connection, QStringLiteral(":/SQL/DuckDb/CreateCacheChangeTable.sql"), errorMessage)
				&& ExecuteScript(connection, QStringLiteral(":/SQL/DuckDb/CreateCacheChangeCursorTable.sql"), errorMessage);
}


bool CCacheChangeLog::RecordUpserted(
			imtduckdb::IDuckConnection& connection,
			const QString& tableName,
			const QString& keyColumn,
			const QString& stagingTableName,
			QString& errorMessage)
{
	return Execute(connection,
				QStringLiteral("INSERT INTO %1 SELECT %2, '%3', CAST(%4 AS UBIGINT) FROM %5")
					.arg(imtdb::QuoteIdentifier(CacheTable::CACHE_CHANGE),
						 NextChangeId(),
						 imtdb::EscapeSql(tableName),
						 imtdb::QuoteIdentifier(keyColumn),
						 imtdb::QuoteIdentifier(stagingTableName)),
				errorMessage);
}


bool CCacheChangeLog::RecordRemoved(
			imtduckdb::IDuckConnection& connection,
			const QString& tableName,
			const QString& keyColumn,
			const QString& removedCondition,
			QString& errorMessage)
{
	return Execute(connection,
				QStringLiteral("INSERT INTO %1 SELECT %2, '%3', CAST(%4 AS UBIGINT) FROM %5 WHERE %6")
					.arg(imtdb::QuoteIdentifier(CacheTable::CACHE_CHANGE),
						 NextChangeId(),
						 imtdb::EscapeSql(tableName),
						 imtdb::QuoteIdentifier(keyColumn),
						 imtdb::QuoteIdentifier(tableName),
						 removedCondition),
				errorMessage);
}


bool CCacheChangeLog::RecordRebuilt(imtduckdb::IDuckConnection& connection, const QString& tableName, QString& errorMessage)
{
	return Execute(connection,
				QStringLiteral("INSERT INTO %1 VALUES (%2, '%3', NULL)")
					.arg(imtdb::QuoteIdentifier(CacheTable::CACHE_CHANGE),
						 NextChangeId(),
						 imtdb::EscapeSql(tableName)),
				errorMessage);
}


bool CCacheChangeLog::Purge(imtduckdb::IDuckConnection& connection, QString& errorMessage)
{
	const QString changeTable = imtdb::QuoteIdentifier(CacheTable::CACHE_CHANGE);
	const QString changeId = imtdb::QuoteIdentifier(CacheChangeColumn::CHANGE_ID);

	return Execute(connection,
				QStringLiteral("DELETE FROM %1 WHERE %2 <= COALESCE((SELECT min(%3) FROM %4), (SELECT max(%2) FROM %1))")
					.arg(changeTable,
						 changeId,
						 imtdb::QuoteIdentifier(CacheChangeCursorColumn::LAST_CHANGE_ID),
						 imtdb::QuoteIdentifier(CacheTable::CACHE_CHANGE_CURSOR)),
				errorMessage);
}


} // namespace imtcache
