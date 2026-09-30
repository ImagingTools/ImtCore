#include <imtcache/CSourceCollectionCacheTableBuilderCompBase.h>


// Qt includes
#include <QtCore/QDebug>
#include <QtCore/QElapsedTimer>
#include <QtCore/QFile>
#include <QtCore/QSet>
#include <QtCore/QUuid>
#include <QtSql/QSqlError>
#include <QtSql/QSqlQuery>

// ACF includes
#include <iprm/CParamsSet.h>
#include <istd/TInterfacePtr.h>

// ImtCore includes
#include <imtbase/CComplexCollectionFilter.h>
#include <imtcol/CDocumentCollectionFilter.h>
#include <imtcache/CProgressMilestones.h>
#include <imtcache/imtcache.h>
#include <imtdb/imtdb.h>


namespace imtcache
{


namespace
{

const QString SHADOW_SUFFIX = QStringLiteral("_shadow");
const QString STAGING_SUFFIX = QStringLiteral("_staging");

const int PROGRESS_STEP_PERCENT = 10;
const int MIN_ROWS_FOR_PROGRESS = 5000;


/// Selects the source rows changed after \a since, or all of them when \a since is not set.
void FillChangedFilter(imtbase::CComplexCollectionFilter& filter, const QByteArray& modificationTimeField, const QDateTime& since)
{
	if (!since.isValid()){
		return;
	}

	filter.AddFieldFilter(imtbase::IComplexCollectionFilter::FieldFilter(
				modificationTimeField,
				since,
				imtbase::IComplexCollectionFilter::FO_GREATER));
}

} // anonymous namespace


// reimplemented (imtcache::ICacheTableBuilder)

QString CSourceCollectionCacheTableBuilderCompBase::GetCacheTableName() const
{
	return QString::fromUtf8(*m_tableNameAttrPtr);
}


QStringList CSourceCollectionCacheTableBuilderCompBase::GetRequiredCacheTables() const
{
	// A source mirror reads PostgreSQL only.
	return QStringList();
}


CSourceCollectionCacheTableBuilderCompBase::BuildResult CSourceCollectionCacheTableBuilderCompBase::Rebuild(imtduckdb::IDuckConnection& connection) const
{
	BuildResult retVal;
	retVal.wasFullRebuild = true;

	const QString tableName = GetCacheTableName();
	const QString shadowTableName = tableName + SHADOW_SUFFIX;

	if (!CreateTable(connection, shadowTableName, retVal.errorMessage)){
		return retVal;
	}

	// Without a State filter the source delegate returns active rows only, which is what a rebuild wants.
	if (!LoadRows(connection, shadowTableName, nullptr, retVal)){
		return retVal;
	}

	QString swapError;
	if (!connection.SwapTable(tableName, shadowTableName, &swapError)){
		retVal.errorMessage = QStringLiteral("Unable to swap in %1. Error: %2").arg(tableName, swapError);

		return retVal;
	}

	retVal.isOk = true;

	return retVal;
}


CSourceCollectionCacheTableBuilderCompBase::BuildResult CSourceCollectionCacheTableBuilderCompBase::ApplyChanges(
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

	if (!CreateTable(connection, stagingTableName, retVal.errorMessage)){
		return retVal;
	}

	imtbase::CComplexCollectionFilter changedFilter;
	FillChangedFilter(changedFilter, *m_modificationTimeFieldAttrPtr, lastSourceUpdateTime);

	iprm::CParamsSet changedParams;
	changedParams.SetEditableParameter(QByteArrayLiteral("ComplexFilter"), &changedFilter);

	if (!LoadRows(connection, stagingTableName, &changedParams, retVal)){
		return retVal;
	}

	if (!connection.BeginTransaction()){
		retVal.errorMessage = QStringLiteral("Unable to begin the %1 merge transaction").arg(tableName);

		return retVal;
	}

	QSqlError sqlError;
	if (retVal.rowsWritten > 0){
		connection.ExecSqlQuery(GetUpsertQuery(stagingTableName).toUtf8(), &sqlError);
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

const imtbase::IObjectCollection* CSourceCollectionCacheTableBuilderCompBase::GetSourceCollection() const
{
	return m_sourceCollectionCompPtr.GetPtr();
}


// private methods

bool CSourceCollectionCacheTableBuilderCompBase::CreateTable(
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


bool CSourceCollectionCacheTableBuilderCompBase::HasExpectedColumns(imtduckdb::IDuckConnection& connection) const
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


bool CSourceCollectionCacheTableBuilderCompBase::LoadRows(
			imtduckdb::IDuckConnection& connection,
			const QString& tableName,
			const iprm::IParamsSet* filterParamsPtr,
			BuildResult& result) const
{
	if (!m_sourceCollectionCompPtr.IsValid()){
		result.errorMessage = QStringLiteral("Invalid component configuration: SourceCollection reference missing");

		return false;
	}

	QString appenderError;
	std::unique_ptr<imtduckdb::IDuckAppender> appenderPtr = connection.CreateAppender(tableName, QString(), &appenderError);
	if (!appenderPtr){
		result.errorMessage = QStringLiteral("Unable to create an appender for %1. Error: %2").arg(tableName, appenderError);

		return false;
	}

	QElapsedTimer phaseTimer;
	phaseTimer.start();

	// The SQL iterator reads the whole result set here, so this is the time spent waiting on PostgreSQL.
	istd::TUniqueInterfacePtr<imtbase::IObjectCollectionIterator> iteratorPtr(
				m_sourceCollectionCompPtr->CreateObjectCollectionIterator(QByteArray(), 0, -1, filterParamsPtr));
	if (!iteratorPtr.IsValid()){
		result.errorMessage = QStringLiteral("Unable to read the source collection for %1").arg(tableName);

		return false;
	}

	const qint64 readMs = phaseTimer.elapsed();
	const int totalCount = iteratorPtr->GetElementsCount();

	// An incremental run usually touches a handful of rows; only a sizeable load is worth reporting on.
	const bool isProgressLogged = totalCount >= MIN_ROWS_FOR_PROGRESS;
	if (isProgressLogged){
		qDebug().noquote() << QStringLiteral("%1: read %2 source rows in %3").arg(tableName).arg(totalCount).arg(FormatDuration(readMs));
	}

	CProgressMilestones milestones(totalCount, PROGRESS_STEP_PERCENT);
	int processedCount = 0;
	int skippedCount = 0;

	phaseTimer.restart();

	while (iteratorPtr->Next()){
		++processedCount;

		const int milestone = milestones.Advance(processedCount);
		if (isProgressLogged && milestone >= 0){
			const qint64 elapsedMs = phaseTimer.elapsed();
			const qint64 remainingMs = elapsedMs * (totalCount - processedCount) / processedCount;

			qDebug().noquote() << QStringLiteral("%1: %2% (%3 of %4 rows), %5 elapsed, about %6 left")
						.arg(tableName)
						.arg(milestone)
						.arg(processedCount)
						.arg(totalCount)
						.arg(FormatDuration(elapsedMs), FormatDuration(remainingMs));
		}

		QVariantList rowValues;
		if (!MapRow(*iteratorPtr, rowValues)){
			++skippedCount;

			continue;
		}

		if (!appenderPtr->AppendRow(rowValues)){
			SendErrorMessage(0, QStringLiteral("Unable to append a row to %1. Error: %2").arg(tableName, appenderPtr->GetLastError()), __func__);

			continue;
		}

		const QDateTime lastModified = iteratorPtr->GetElementInfo(*m_modificationTimeFieldAttrPtr).toDateTime();
		if (lastModified.isValid() && (!result.lastSourceUpdateTime.isValid() || lastModified > result.lastSourceUpdateTime)){
			result.lastSourceUpdateTime = lastModified;
		}

		++result.rowsWritten;
	}

	const qint64 mapMs = phaseTimer.restart();

	if (!appenderPtr->Close()){
		result.errorMessage = QStringLiteral("Unable to flush %1. Error: %2").arg(tableName, appenderPtr->GetLastError());

		return false;
	}

	if (isProgressLogged){
		qDebug().noquote() << QStringLiteral("%1: loaded %2 rows (%3 skipped): read %4, mapped %5, flushed %6")
					.arg(tableName)
					.arg(result.rowsWritten)
					.arg(skippedCount)
					.arg(FormatDuration(readMs), FormatDuration(mapMs), FormatDuration(phaseTimer.elapsed()));
	}

	return true;
}


bool CSourceCollectionCacheTableBuilderCompBase::RemoveDeletedRows(
			imtduckdb::IDuckConnection& connection,
			const QDateTime& lastSourceUpdateTime,
			BuildResult& result) const
{
	if (m_detectDeletionsAttrPtr.IsValid() && !*m_detectDeletionsAttrPtr){
		return true;
	}

	if (*m_reconcileDeletionsAttrPtr){
		return RemoveRowsMissingFromSource(connection, result);
	}

	return RemoveRowsDeletedSince(connection, lastSourceUpdateTime, result);
}


bool CSourceCollectionCacheTableBuilderCompBase::RemoveRowsDeletedSince(
			imtduckdb::IDuckConnection& connection,
			const QDateTime& lastSourceUpdateTime,
			BuildResult& result) const
{
	// This relies on the source delete changing the modification time, so the cutoff that finds edits
	// finds removals too. That holds for address collections; document collections do not do it.
	imtbase::CComplexCollectionFilter changedFilter;
	FillChangedFilter(changedFilter, *m_modificationTimeFieldAttrPtr, lastSourceUpdateTime);

	imtcol::CDocumentCollectionFilter documentFilter;
	documentFilter.AddDocumentState(imtcol::IDocumentCollectionFilter::DS_DISABLED);

	iprm::CParamsSet deletedParams;
	deletedParams.SetEditableParameter(QByteArrayLiteral("ComplexFilter"), &changedFilter);
	deletedParams.SetEditableParameter(*m_stateFilterParamIdAttrPtr, &documentFilter);

	istd::TUniqueInterfacePtr<imtbase::IObjectCollectionIterator> iteratorPtr(
				m_sourceCollectionCompPtr->CreateObjectCollectionIterator(QByteArray(), 0, -1, &deletedParams));
	if (!iteratorPtr.IsValid()){
		result.errorMessage = QStringLiteral("Unable to read soft-deleted rows for %1").arg(GetCacheTableName());

		return false;
	}

	QStringList deletedIds;
	while (iteratorPtr->Next()){
		const QUuid documentUuid = QUuid(QString::fromUtf8(iteratorPtr->GetObjectId()));
		if (documentUuid.isNull()){
			continue;
		}

		deletedIds << QString::fromUtf8(documentUuid.toByteArray(QUuid::WithoutBraces));

		const QDateTime lastModified = iteratorPtr->GetElementInfo(*m_modificationTimeFieldAttrPtr).toDateTime();
		if (lastModified.isValid() && (!result.lastSourceUpdateTime.isValid() || lastModified > result.lastSourceUpdateTime)){
			result.lastSourceUpdateTime = lastModified;
		}
	}

	if (deletedIds.isEmpty()){
		return true;
	}

	return DeleteRowsById(connection, deletedIds, result);
}


bool CSourceCollectionCacheTableBuilderCompBase::RemoveRowsMissingFromSource(
			imtduckdb::IDuckConnection& connection,
			BuildResult& result) const
{
	// No filter parameters: the source returns its active rows only.
	istd::TUniqueInterfacePtr<imtbase::IObjectCollectionIterator> iteratorPtr(
				m_sourceCollectionCompPtr->CreateObjectCollectionIterator(QByteArray(), 0, -1, nullptr));
	if (!iteratorPtr.IsValid()){
		// Deleting on the strength of a failed read would empty the table.
		result.errorMessage = QStringLiteral("Unable to read the active rows of %1").arg(GetCacheTableName());

		return false;
	}

	QSet<QString> activeIds;
	while (iteratorPtr->Next()){
		const QUuid documentUuid = QUuid(QString::fromUtf8(iteratorPtr->GetObjectId()));
		if (!documentUuid.isNull()){
			activeIds.insert(QString::fromUtf8(documentUuid.toByteArray(QUuid::WithoutBraces)));
		}
	}

	QSqlError sqlError;
	QSqlQuery cachedQuery = connection.ExecSqlQuery(
				QStringLiteral(R"(SELECT CAST(%1 AS VARCHAR) FROM %2)")
							.arg(imtdb::QuoteIdentifier(QString::fromUtf8(*m_objectIdColumnAttrPtr)),
								 imtdb::QuoteIdentifier(GetCacheTableName()))
							.toUtf8(),
				&sqlError);
	if (sqlError.type() != QSqlError::NoError){
		result.errorMessage = QStringLiteral("Unable to list the cached rows of %1. Error: %2").arg(GetCacheTableName(), sqlError.text());

		return false;
	}

	QStringList removedIds;
	while (cachedQuery.next()){
		const QString cachedId = cachedQuery.value(0).toString();
		if (!activeIds.contains(cachedId)){
			removedIds << cachedId;
		}
	}

	if (removedIds.isEmpty()){
		return true;
	}

	return DeleteRowsById(connection, removedIds, result);
}


bool CSourceCollectionCacheTableBuilderCompBase::DeleteRowsById(
			imtduckdb::IDuckConnection& connection,
			const QStringList& documentIds,
			BuildResult& result) const
{
	// Ids are UUID text produced by QUuid or read back from a UUID column, so quoting them is enough.
	QStringList quotedIds;
	for (const QString& documentId : documentIds){
		quotedIds << QStringLiteral("'%1'").arg(documentId);
	}

	QSqlError sqlError;
	connection.ExecSqlQuery(QStringLiteral(R"(DELETE FROM %1 WHERE %2 IN (%3))")
								.arg(imtdb::QuoteIdentifier(GetCacheTableName()),
									 imtdb::QuoteIdentifier(QString::fromUtf8(*m_objectIdColumnAttrPtr)),
									 quotedIds.join(','))
								.toUtf8(), &sqlError);

	if (sqlError.type() != QSqlError::NoError){
		result.errorMessage = QStringLiteral("Unable to delete removed rows from %1. Error: %2").arg(GetCacheTableName(), sqlError.text());

		return false;
	}

	result.rowsDeleted = documentIds.count();

	return true;
}


QString CSourceCollectionCacheTableBuilderCompBase::GetUpsertQuery(const QString& stagingTableName) const
{
	const QString objectIdColumn = QString::fromUtf8(*m_objectIdColumnAttrPtr);

	QStringList assignments;
	for (const QString& columnName : GetColumnNames()){
		if (columnName == objectIdColumn){
			continue;
		}

		const QString columnIdentifier = imtdb::QuoteIdentifier(columnName);
		assignments << QStringLiteral("%1 = excluded.%1").arg(columnIdentifier);
	}

	return QStringLiteral("INSERT INTO %1 SELECT * FROM %2 ON CONFLICT (%3) DO UPDATE SET %4")
				.arg(imtdb::QuoteIdentifier(GetCacheTableName()),
					 imtdb::QuoteIdentifier(stagingTableName),
					 imtdb::QuoteIdentifier(objectIdColumn),
					 assignments.join(QStringLiteral(", ")));
}


} // namespace imtcache
