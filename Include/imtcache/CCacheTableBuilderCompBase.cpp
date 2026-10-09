#include <imtcache/CCacheTableBuilderCompBase.h>


// Qt includes
#include <QtCore/QFile>
#include <QtSql/QSqlError>
#include <QtSql/QSqlQuery>

// ImtCore includes
#include <imtcache/CCacheChangeLog.h>
#include <imtdb/imtdb.h>


namespace imtcache
{


namespace
{

const QString SHADOW_SUFFIX = QStringLiteral("_shadow");
const QString STAGING_SUFFIX = QStringLiteral("_staging");

} // anonymous namespace


// reimplemented (imtcache::ICacheTableBuilder)

QString CCacheTableBuilderCompBase::GetCacheTableName() const
{
	return QString::fromUtf8(*m_tableNameAttrPtr);
}


QStringList CCacheTableBuilderCompBase::GetRequiredCacheTables() const
{
	// A table loaded from an outside source reads nothing from the cache.
	return QStringList();
}


CCacheTableBuilderCompBase::BuildResult CCacheTableBuilderCompBase::Rebuild(imtduckdb::IDuckConnection& connection) const
{
	BuildResult retVal;
	retVal.wasFullRebuild = true;

	const QString tableName = GetCacheTableName();
	const QString shadowTableName = tableName + SHADOW_SUFFIX;

	if (!CCacheChangeLog::EnsureTables(connection, retVal.errorMessage)){
		return retVal;
	}

	if (!CreateTable(connection, shadowTableName, retVal.errorMessage)){
		return retVal;
	}

	if (!LoadRows(connection, shadowTableName, QDateTime(), retVal)){
		return retVal;
	}

	QString swapError;
	if (!connection.SwapTable(tableName, shadowTableName, &swapError)){
		retVal.errorMessage = QStringLiteral("Unable to swap in %1. Error: %2").arg(tableName, swapError);

		return retVal;
	}

	QString logError;
	if (!CCacheChangeLog::RecordRebuilt(connection, tableName, logError)){
		retVal.errorMessage = QStringLiteral("Unable to log the rebuild of %1. Error: %2").arg(tableName, logError);

		return retVal;
	}

	retVal.isOk = true;

	return retVal;
}


CCacheTableBuilderCompBase::BuildResult CCacheTableBuilderCompBase::ApplyChanges(
			imtduckdb::IDuckConnection& connection,
			const QDateTime& lastSourceUpdateTime) const
{
	BuildResult retVal;
	retVal.lastSourceUpdateTime = lastSourceUpdateTime;

	const QString tableName = GetCacheTableName();
	const QString stagingTableName = tableName + STAGING_SUFFIX;
	const QString stagingTableIdentifier = imtdb::QuoteIdentifier(stagingTableName);

	// The merge is positional (INSERT ... SELECT *), so a live table from before a schema change cannot
	// be patched - it would fail on every run. Rebuilding is the only way to bring it forward.
	if (!HasExpectedColumns(connection)){
		return Rebuild(connection);
	}

	if (!CCacheChangeLog::EnsureTables(connection, retVal.errorMessage)){
		return retVal;
	}

	if (!CreateTable(connection, stagingTableName, retVal.errorMessage)){
		return retVal;
	}

	if (!LoadRows(connection, stagingTableName, lastSourceUpdateTime, retVal)){
		return retVal;
	}

	if (!connection.BeginTransaction()){
		retVal.errorMessage = QStringLiteral("Unable to begin the %1 merge transaction").arg(tableName);

		return retVal;
	}

	QSqlError sqlError;
	if (retVal.rowsWritten > 0){
		// Before the merge, which overwrites what the staged rows are compared with.
		QString logError;
		int changedCount = 0;
		if (!CCacheChangeLog::RecordUpserted(connection, tableName, GetKeyColumn(), stagingTableName, GetReplaceCondition(), changedCount, logError)){
			connection.CancelTransaction();
			retVal.errorMessage = QStringLiteral("Unable to log the %1 changes. Error: %2").arg(tableName, logError);

			return retVal;
		}

		connection.ExecSqlQuery(GetUpsertQuery(stagingTableName).toUtf8(), &sqlError);

		// Rows read again unchanged are not writes.
		retVal.rowsWritten = changedCount;
	}

	if (sqlError.type() == QSqlError::NoError && !RemoveDeletedRows(connection, lastSourceUpdateTime, retVal)){
		connection.CancelTransaction();

		return retVal;
	}

	if (sqlError.type() != QSqlError::NoError){
		connection.CancelTransaction();
		retVal.errorMessage = QStringLiteral("Unable to merge %1 changes. Error: %2").arg(tableName, sqlError.text());

		return retVal;
	}

	if (!connection.FinishTransaction()){
		retVal.errorMessage = QStringLiteral("Unable to commit the %1 merge transaction").arg(tableName);

		return retVal;
	}

	connection.ExecSqlQuery(QStringLiteral("DROP TABLE IF EXISTS %1").arg(stagingTableIdentifier).toUtf8());

	retVal.isOk = true;

	return retVal;
}


// protected methods

bool CCacheTableBuilderCompBase::RemoveDeletedRows(
			imtduckdb::IDuckConnection& /*connection*/,
			const QDateTime& /*lastSourceUpdateTime*/,
			BuildResult& /*result*/) const
{
	return true;
}


QString CCacheTableBuilderCompBase::GetReplaceCondition() const
{
	return QString();
}


QString CCacheTableBuilderCompBase::GetObjectIdColumn() const
{
	return QString::fromUtf8(*m_objectIdColumnAttrPtr);
}


QString CCacheTableBuilderCompBase::GetKeyColumn() const
{
	return GetColumnNames().value(0);
}


// private methods

bool CCacheTableBuilderCompBase::CreateTable(
			imtduckdb::IDuckConnection& connection,
			const QString& tableName,
			QString& errorMessage) const
{
	QFile scriptFile(QString::fromUtf8(*m_createTableScriptPathAttrPtr));
	if (!scriptFile.open(QFile::ReadOnly)){
		errorMessage = QStringLiteral("Unable to read the creation script for %1").arg(tableName);

		return false;
	}

	QByteArray createTableQuery = scriptFile.readAll();
	scriptFile.close();
	createTableQuery.replace(QByteArrayLiteral("${TableName}"), tableName.toUtf8());

	QSqlError sqlError;
	connection.ExecSqlQuery(QStringLiteral("DROP TABLE IF EXISTS %1").arg(imtdb::QuoteIdentifier(tableName)).toUtf8(), &sqlError);
	if (sqlError.type() == QSqlError::NoError){
		connection.ExecSqlQuery(createTableQuery, &sqlError);
	}

	if (sqlError.type() != QSqlError::NoError){
		errorMessage = QStringLiteral("Unable to create %1. Error: %2").arg(tableName, sqlError.text());

		return false;
	}

	return true;
}


bool CCacheTableBuilderCompBase::HasExpectedColumns(imtduckdb::IDuckConnection& connection) const
{
	QSqlError sqlError;
	QSqlQuery query = connection.ExecSqlQuery(
				QStringLiteral("SELECT column_name FROM information_schema.columns WHERE table_name = '%1' AND table_schema = current_schema() ORDER BY ordinal_position")
					.arg(imtdb::EscapeSql(GetCacheTableName())).toUtf8(),
				&sqlError);

	if (sqlError.type() != QSqlError::NoError){
		return false;
	}

	QStringList actualColumns;
	while (query.next()){
		actualColumns << query.value(0).toString();
	}

	const QStringList expectedColumns = GetColumnNames();
	if (actualColumns.count() != expectedColumns.count()){
		return false;
	}

	for (int i = 0; i < expectedColumns.count(); ++ i){
		// DuckDB identifiers are case-insensitive, so the script's spelling need not match the constants'.
		if (actualColumns[i].compare(expectedColumns[i], Qt::CaseInsensitive) != 0){
			return false;
		}
	}

	return true;
}


QString CCacheTableBuilderCompBase::GetUpsertQuery(const QString& stagingTableName) const
{
	const QString objectIdColumn = GetObjectIdColumn();

	QStringList assignments;
	for (const QString& columnName : GetColumnNames()){
		if (columnName == objectIdColumn){
			continue;
		}

		const QString columnIdentifier = imtdb::QuoteIdentifier(columnName);
		assignments << QStringLiteral("%1 = excluded.%1").arg(columnIdentifier);
	}

	QString retVal = QStringLiteral("INSERT INTO %1 SELECT * FROM %2 ON CONFLICT (%3) DO UPDATE SET %4")
				.arg(imtdb::QuoteIdentifier(GetCacheTableName()),
					 imtdb::QuoteIdentifier(stagingTableName),
					 imtdb::QuoteIdentifier(objectIdColumn),
					 assignments.join(QStringLiteral(", ")));

	const QString replaceCondition = GetReplaceCondition();
	if (!replaceCondition.isEmpty()){
		retVal += QStringLiteral(" WHERE ") + replaceCondition;
	}

	return retVal;
}


} // namespace imtcache
