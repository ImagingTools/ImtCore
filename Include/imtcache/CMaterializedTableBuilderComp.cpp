#include <imtcache/CMaterializedTableBuilderComp.h>


// Qt includes
#include <QtCore/QFile>
#include <QtSql/QSqlError>
#include <QtSql/QSqlQuery>
#include <QtSql/QSqlRecord>

// ImtCore includes
#include <imtcache/CCacheChangeLog.h>
#include <imtdb/imtdb.h>


namespace imtcache
{


namespace
{

const QString SHADOW_SUFFIX = QStringLiteral("_shadow");
const QString AFFECTED_SUFFIX = QStringLiteral("_affected");

/// Column of the table of affected keys.
const QString AFFECTED_KEY_COLUMN = QStringLiteral("KeyId");


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


/// The first column of the first row of the result.
bool ReadValue(imtduckdb::IDuckConnection& connection, const QString& query, QVariant& value, QString& errorMessage)
{
	QSqlError sqlError;
	QSqlQuery result = connection.ExecSqlQuery(query.toUtf8(), &sqlError);
	if (sqlError.type() != QSqlError::NoError){
		errorMessage = sqlError.text();

		return false;
	}

	value = result.next() ? result.value(0) : QVariant();

	return true;
}

} // anonymous namespace


// reimplemented (imtcache::ICacheTableBuilder)

QString CMaterializedTableBuilderComp::GetCacheTableName() const
{
	return QString::fromUtf8(*m_tableNameAttrPtr);
}


QStringList CMaterializedTableBuilderComp::GetRequiredCacheTables() const
{
	QStringList retVal;
	for (int i = 0; i < m_requiredTablesAttrPtr.GetCount(); ++ i){
		retVal << QString::fromUtf8(m_requiredTablesAttrPtr[i]);
	}

	return retVal;
}


CMaterializedTableBuilderComp::BuildResult CMaterializedTableBuilderComp::Rebuild(imtduckdb::IDuckConnection& connection) const
{
	BuildResult retVal;
	retVal.wasFullRebuild = true;

	const QString tableName = GetCacheTableName();
	const QString shadowTableName = tableName + SHADOW_SUFFIX;

	QString error;
	if (!CCacheChangeLog::EnsureTables(connection, error)){
		retVal.errorMessage = QStringLiteral("Unable to create the cache change log. Error: %1").arg(error);

		return retVal;
	}

	// The tables are read after this, so everything logged up to now is in the result.
	bool hasCursor = false;
	qint64 lastChangeId = 0;
	qint64 latestChangeId = 0;
	if (!CCacheChangeLog::GetCursor(connection, tableName, hasCursor, lastChangeId, error)
				|| !CCacheChangeLog::GetLatestChangeId(connection, lastChangeId, latestChangeId, error)){
		retVal.errorMessage = QStringLiteral("Unable to read the cache change log. Error: %1").arg(error);

		return retVal;
	}

	QString selectQuery;
	if (!CreateSelectQuery(QStringLiteral("TRUE"), selectQuery, error)){
		retVal.errorMessage = error;

		return retVal;
	}

	const QString shadowIdentifier = imtdb::QuoteIdentifier(shadowTableName);
	if (!Execute(connection, QStringLiteral("DROP TABLE IF EXISTS %1").arg(shadowIdentifier), error)
				|| !Execute(connection, QStringLiteral("CREATE TABLE %1 AS\n%2").arg(shadowIdentifier, selectQuery), error)){
		retVal.errorMessage = QStringLiteral("Unable to build %1. Error: %2").arg(tableName, error);

		return retVal;
	}

	if (!connection.SwapTable(tableName, shadowTableName, &error)){
		retVal.errorMessage = QStringLiteral("Unable to swap in %1. Error: %2").arg(tableName, error);

		return retVal;
	}

	if (!CCacheChangeLog::SetCursor(connection, tableName, latestChangeId, error)){
		retVal.errorMessage = QStringLiteral("Unable to move the cursor of %1. Error: %2").arg(tableName, error);

		return retVal;
	}

	QVariant rowCount;
	if (ReadValue(connection, QStringLiteral("SELECT count(*) FROM %1").arg(imtdb::QuoteIdentifier(tableName)), rowCount, error)){
		retVal.rowsWritten = rowCount.toInt();
	}

	// The orchestrator needs a valid time to run the next update as incremental.
	retVal.lastSourceUpdateTime = QDateTime::currentDateTimeUtc();
	retVal.isOk = true;

	return retVal;
}


CMaterializedTableBuilderComp::BuildResult CMaterializedTableBuilderComp::ApplyChanges(
			imtduckdb::IDuckConnection& connection,
			const QDateTime& /*lastSourceUpdateTime*/) const
{
	BuildResult retVal;
	retVal.lastSourceUpdateTime = QDateTime::currentDateTimeUtc();

	const QString tableName = GetCacheTableName();
	const QString tableIdentifier = imtdb::QuoteIdentifier(tableName);
	const QString keyIdentifier = imtdb::QuoteIdentifier(QString::fromUtf8(*m_keyColumnAttrPtr));
	const QString affectedIdentifier = imtdb::QuoteIdentifier(GetAffectedTableName());
	const QString affectedKeyIdentifier = imtdb::QuoteIdentifier(AFFECTED_KEY_COLUMN);

	QString error;
	if (!CCacheChangeLog::EnsureTables(connection, error)){
		retVal.errorMessage = QStringLiteral("Unable to create the cache change log. Error: %1").arg(error);

		return retVal;
	}

	bool hasCursor = false;
	qint64 lastChangeId = 0;
	qint64 latestChangeId = 0;
	bool hasRebuilt = false;
	if (!CCacheChangeLog::GetCursor(connection, tableName, hasCursor, lastChangeId, error)
				|| !CCacheChangeLog::GetLatestChangeId(connection, lastChangeId, latestChangeId, error)
				|| !CCacheChangeLog::HasRebuilt(connection, GetRequiredCacheTables(), lastChangeId, latestChangeId, hasRebuilt, error)){
		retVal.errorMessage = QStringLiteral("Unable to read the cache change log. Error: %1").arg(error);

		return retVal;
	}

	// Without a cursor the table was never built from the log, and a rebuilt source or a changed script leaves no rows to patch.
	if (!hasCursor || hasRebuilt || !HasExpectedColumns(connection)){
		return Rebuild(connection);
	}

	if (latestChangeId == lastChangeId){
		retVal.isOk = true;

		return retVal;
	}

	QString affectedKeysQuery;
	if (!ReadScript(*m_selectAffectedKeysScriptPathAttrPtr, affectedKeysQuery, error)){
		retVal.errorMessage = error;

		return retVal;
	}

	affectedKeysQuery.replace(QStringLiteral("${LastChangeId}"), QString::number(lastChangeId));
	affectedKeysQuery.replace(QStringLiteral("${LatestChangeId}"), QString::number(latestChangeId));

	QString selectQuery;
	if (!CreateSelectQuery(
				QStringLiteral("%1 IN (SELECT %2 FROM %3)").arg(QString::fromUtf8(*m_keyExpressionAttrPtr), affectedKeyIdentifier, affectedIdentifier),
				selectQuery,
				error)){
		retVal.errorMessage = error;

		return retVal;
	}

	if (!Execute(connection,
				QStringLiteral("CREATE OR REPLACE TEMP TABLE %1 AS SELECT DISTINCT CAST(k AS UBIGINT) AS %2 FROM (\n%3\n) AS keys(k)")
					.arg(affectedIdentifier, affectedKeyIdentifier, affectedKeysQuery),
				error)){
		retVal.errorMessage = QStringLiteral("Unable to select the rows of %1 to recompute. Error: %2").arg(tableName, error);

		return retVal;
	}

	QVariant affectedCount;
	if (!ReadValue(connection, QStringLiteral("SELECT count(*) FROM %1").arg(affectedIdentifier), affectedCount, error)){
		retVal.errorMessage = error;

		return retVal;
	}

	if (affectedCount.toLongLong() == 0){
		if (!CCacheChangeLog::SetCursor(connection, tableName, latestChangeId, error)){
			retVal.errorMessage = QStringLiteral("Unable to move the cursor of %1. Error: %2").arg(tableName, error);

			return retVal;
		}

		retVal.isOk = true;

		return retVal;
	}

	if (!connection.BeginTransaction()){
		retVal.errorMessage = QStringLiteral("Unable to begin the %1 transaction").arg(tableName);

		return retVal;
	}

	if (!Execute(connection, QStringLiteral("DELETE FROM %1 WHERE %2 IN (SELECT %3 FROM %4)").arg(tableIdentifier, keyIdentifier, affectedKeyIdentifier, affectedIdentifier), error)
				|| !Execute(connection, QStringLiteral("INSERT INTO %1\n%2").arg(tableIdentifier, selectQuery), error)
				|| !CCacheChangeLog::SetCursor(connection, tableName, latestChangeId, error)){
		connection.CancelTransaction();
		retVal.errorMessage = QStringLiteral("Unable to recompute rows of %1. Error: %2").arg(tableName, error);

		return retVal;
	}

	if (!connection.FinishTransaction()){
		retVal.errorMessage = QStringLiteral("Unable to commit the %1 transaction").arg(tableName);

		return retVal;
	}

	QVariant writtenCount;
	if (ReadValue(connection, QStringLiteral("SELECT count(*) FROM %1 WHERE %2 IN (SELECT %3 FROM %4)").arg(tableIdentifier, keyIdentifier, affectedKeyIdentifier, affectedIdentifier), writtenCount, error)){
		retVal.rowsWritten = writtenCount.toInt();
	}

	// An affected key with no row left is a row whose source is gone.
	QVariant removedCount;
	if (ReadValue(connection, QStringLiteral("SELECT count(*) FROM %1 WHERE %2 NOT IN (SELECT %3 FROM %4)").arg(affectedIdentifier, affectedKeyIdentifier, keyIdentifier, tableIdentifier), removedCount, error)){
		retVal.rowsDeleted = removedCount.toInt();
	}

	Execute(connection, QStringLiteral("DROP TABLE IF EXISTS %1").arg(affectedIdentifier), error);

	retVal.isOk = true;

	return retVal;
}


// private methods

bool CMaterializedTableBuilderComp::ReadScript(const QByteArray& path, QString& script, QString& errorMessage) const
{
	QFile scriptFile(QString::fromUtf8(path));
	if (!scriptFile.open(QFile::ReadOnly)){
		errorMessage = QStringLiteral("Unable to read the script %1 of %2").arg(QString::fromUtf8(path), GetCacheTableName());

		return false;
	}

	script = QString::fromUtf8(scriptFile.readAll()).trimmed();
	scriptFile.close();

	// The script is put inside other statements, which a closing semicolon would break.
	while (script.endsWith(QLatin1Char(';'))){
		script.chop(1);
	}

	return true;
}


bool CMaterializedTableBuilderComp::CreateSelectQuery(const QString& keyFilter, QString& query, QString& errorMessage) const
{
	if (!ReadScript(*m_selectRowsScriptPathAttrPtr, query, errorMessage)){
		return false;
	}

	query.replace(QStringLiteral("${KeyFilter}"), keyFilter);

	return true;
}


bool CMaterializedTableBuilderComp::HasExpectedColumns(imtduckdb::IDuckConnection& connection) const
{
	QString error;
	QString selectQuery;
	if (!CreateSelectQuery(QStringLiteral("FALSE"), selectQuery, error)){
		return false;
	}

	QSqlError sqlError;
	QSqlQuery probeQuery = connection.ExecSqlQuery(QStringLiteral("SELECT * FROM (\n%1\n) AS probe LIMIT 0").arg(selectQuery).toUtf8(), &sqlError);
	if (sqlError.type() != QSqlError::NoError){
		return false;
	}

	const QSqlRecord expectedRecord = probeQuery.record();

	QSqlQuery columnsQuery = connection.ExecSqlQuery(
				QStringLiteral("SELECT column_name FROM information_schema.columns WHERE table_name = '%1' AND table_schema = current_schema() ORDER BY ordinal_position")
					.arg(imtdb::EscapeSql(GetCacheTableName())).toUtf8(),
				&sqlError);
	if (sqlError.type() != QSqlError::NoError){
		return false;
	}

	QStringList actualColumns;
	while (columnsQuery.next()){
		actualColumns << columnsQuery.value(0).toString();
	}

	if (actualColumns.count() != expectedRecord.count()){
		return false;
	}

	for (int i = 0; i < actualColumns.count(); ++ i){
		// DuckDB identifiers are case-insensitive.
		if (actualColumns[i].compare(expectedRecord.fieldName(i), Qt::CaseInsensitive) != 0){
			return false;
		}
	}

	return true;
}


QString CMaterializedTableBuilderComp::GetAffectedTableName() const
{
	return GetCacheTableName() + AFFECTED_SUFFIX;
}


} // namespace imtcache
