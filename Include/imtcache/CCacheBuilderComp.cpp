#include <imtcache/CCacheBuilderComp.h>


// Qt includes
#include <QtConcurrent/QtConcurrent>
#include <QtCore/QDebug>
#include <QtCore/QElapsedTimer>
#include <QtCore/QFile>
#include <QtCore/QSet>
#include <QtCore/QTimeZone>
#include <QtSql/QSqlError>
#include <QtSql/QSqlQuery>

// ImtCore includes
#include <imtdb/imtdb.h>
#include <imtcache/CCacheChangeLog.h>
#include <imtcache/imtcache.h>


namespace imtcache
{


// reimplemented (imtcache::ICacheUpdateController)

CCacheBuilderComp::UpdateResult CCacheBuilderComp::Update(UpdateMode mode)
{
	UpdateResult retVal;

	if (!m_connectionProviderCompPtr.IsValid()){
		retVal.errorMessage = QStringLiteral("Invalid component configuration: CacheConnectionProvider reference missing");

		return retVal;
	}

	std::unique_ptr<imtduckdb::IDuckConnection> connectionPtr = m_connectionProviderCompPtr->CreateConnection();
	if (!connectionPtr){
		retVal.errorMessage = QStringLiteral("Unable to open a builder connection to the cache database");

		return retVal;
	}

	if (!EnsureRevisionTable(*connectionPtr)){
		retVal.errorMessage = QStringLiteral("Unable to create the cache revision table");

		return retVal;
	}

	QString changeLogError;
	if (!CCacheChangeLog::EnsureTables(*connectionPtr, changeLogError)){
		retVal.errorMessage = QStringLiteral("Unable to create the cache change log. Error: %1").arg(changeLogError);

		return retVal;
	}

	retVal.isOk = true;

	const QList<const ICacheTableBuilder*> orderedBuilders = GetOrderedBuilders();
	int tableIndex = 0;

	for (const ICacheTableBuilder* tableBuilderPtr : orderedBuilders){
		++tableIndex;

		const QString tableName = tableBuilderPtr->GetCacheTableName();

		qDebug().noquote() << QStringLiteral("Updating table %1 (%2 of %3)").arg(tableName).arg(tableIndex).arg(orderedBuilders.count());

		QElapsedTimer tableTimer;
		tableTimer.start();

		const ICacheTableBuilder::BuildResult buildResult = RunTableBuilder(*tableBuilderPtr, *connectionPtr, mode);

		retVal.rowsWritten += buildResult.rowsWritten;
		retVal.rowsDeleted += buildResult.rowsDeleted;
		retVal.wasFullRebuild = retVal.wasFullRebuild || buildResult.wasFullRebuild;

		if (buildResult.isOk){
			qDebug().noquote() << QStringLiteral("Table %1 updated (%2) in %3: %4 rows written, %5 removed")
						.arg(tableName,
							 buildResult.wasFullRebuild ? QStringLiteral("full rebuild") : QStringLiteral("incremental"),
							 FormatDuration(tableTimer.elapsed()))
						.arg(buildResult.rowsWritten)
						.arg(buildResult.rowsDeleted);

			SetLastSourceUpdateTime(*connectionPtr, tableName, buildResult.lastSourceUpdateTime);

			continue;
		}

		SendErrorMessage(0, QStringLiteral("Table %1 failed after %2. Error: %3").arg(tableName, FormatDuration(tableTimer.elapsed()), buildResult.errorMessage), __func__);

		// One failing table must not discard the tables that did rebuild, so the run continues.
		retVal.isOk = false;
		retVal.errorMessage = retVal.errorMessage.isEmpty()
					? buildResult.errorMessage
					: retVal.errorMessage + QStringLiteral("; ") + buildResult.errorMessage;
	}

	// A consumer that failed keeps its cursor, so what it has not read survives.
	if (!CCacheChangeLog::Purge(*connectionPtr, changeLogError)){
		SendErrorMessage(0, QStringLiteral("Unable to purge the cache change log. Error: %1").arg(changeLogError), __func__);
	}

	// DuckDB merges its WAL into the database file only at a size threshold or on a clean shutdown, which a killed process skips.
	QSqlError checkpointError;
	connectionPtr->ExecSqlQuery(QByteArrayLiteral("CHECKPOINT"), &checkpointError);
	if (checkpointError.type() != QSqlError::NoError){
		SendWarningMessage(0, QStringLiteral("Unable to checkpoint the cache database. Error: %1").arg(checkpointError.text()), __func__);
	}

	return retVal;
}


bool CCacheBuilderComp::RequestUpdate(UpdateMode mode)
{
	QMutexLocker locker(&m_stateMutex);

	if (m_isUpdateRunning || m_suspendCount > 0){
		// Collapse everything requested meanwhile into one run, keeping the more thorough mode.
		m_hasPendingRequest = true;
		if (mode == UM_FULL){
			m_pendingMode = UM_FULL;
		}

		return false;
	}

	m_isUpdateRunning = true;

	locker.unlock();

	m_updateWatcher.setFuture(QtConcurrent::run(&CCacheBuilderComp::RunUpdate, this, mode));

	return true;
}


bool CCacheBuilderComp::IsUpdateRunning() const
{
	QMutexLocker locker(&m_stateMutex);

	return m_isUpdateRunning;
}


void CCacheBuilderComp::SuspendUpdates()
{
	QMutexLocker locker(&m_stateMutex);

	++m_suspendCount;
}


void CCacheBuilderComp::ResumeUpdates()
{
	QMutexLocker locker(&m_stateMutex);

	if (m_suspendCount <= 0){
		return;
	}

	if (--m_suspendCount > 0 || !m_hasPendingRequest){
		return;
	}

	const UpdateMode pendingMode = m_pendingMode;
	m_hasPendingRequest = false;
	m_pendingMode = UM_INCREMENTAL;

	locker.unlock();

	RequestUpdate(pendingMode);
}


void CCacheBuilderComp::AttachObserver(IObserver* observerPtr)
{
	QMutexLocker locker(&m_observerMutex);

	if (observerPtr != nullptr && !m_observers.contains(observerPtr)){
		m_observers.append(observerPtr);
	}
}


void CCacheBuilderComp::DetachObserver(IObserver* observerPtr)
{
	QMutexLocker locker(&m_observerMutex);

	m_observers.removeAll(observerPtr);
}


// protected methods

// reimplemented (icomp::CComponentBase)

void CCacheBuilderComp::OnComponentCreated()
{
	BaseClass::OnComponentCreated();

	connect(&m_updateWatcher, &QFutureWatcher<void>::finished, this, &CCacheBuilderComp::OnUpdateFinished);

	if (*m_buildOnStartupAttrPtr){
		// Deferred rather than called directly: the component graph is still being wired here, and
		// blocking would stall startup for the whole rebuild.
		QTimer::singleShot(0, this, [this](){ RequestUpdate(UM_INCREMENTAL); });
	}

	if (!m_updateIntervalSecAttrPtr.IsValid() || *m_updateIntervalSecAttrPtr <= 0){
		return;
	}

	m_updateTimerPtr = new QTimer(this);
	m_updateTimerPtr->setInterval(std::chrono::seconds{*m_updateIntervalSecAttrPtr});
	connect(m_updateTimerPtr, &QTimer::timeout, this, [this](){ RequestUpdate(UM_INCREMENTAL); });
	m_updateTimerPtr->start();
}


void CCacheBuilderComp::OnComponentDestroyed()
{
	if (m_updateTimerPtr != nullptr){
		m_updateTimerPtr->stop();
	}

	m_updateWatcher.disconnect(this);

	if (m_updateWatcher.isStarted()){
		m_updateWatcher.waitForFinished();
	}

	BaseClass::OnComponentDestroyed();
}


// private slots

void CCacheBuilderComp::OnUpdateFinished()
{
	QMutexLocker locker(&m_stateMutex);

	m_isUpdateRunning = false;

	const bool runPending = m_hasPendingRequest && m_suspendCount == 0;
	const UpdateMode pendingMode = m_pendingMode;
	if (runPending){
		m_hasPendingRequest = false;
		m_pendingMode = UM_INCREMENTAL;
	}

	locker.unlock();

	if (runPending){
		RequestUpdate(pendingMode);
	}
}


// private methods

void CCacheBuilderComp::RunUpdate(UpdateMode mode)
{
	const qint64 startedAtMs = QDateTime::currentMSecsSinceEpoch();

	QElapsedTimer timer;
	timer.start();

	SendInfoMessage(0, QStringLiteral("Cache update started (requested %1)").arg(mode == UM_FULL ? QStringLiteral("full rebuild") : QStringLiteral("incremental")), __func__);

	const UpdateResult result = Update(mode);

	const QString duration = FormatDuration(timer.elapsed());

	if (result.isOk){
		SendInfoMessage(0, QStringLiteral("Cache updated (%1) in %2: %3 rows written, %4 removed")
							 .arg(result.wasFullRebuild ? QStringLiteral("full rebuild") : QStringLiteral("incremental"), duration)
							 .arg(result.rowsWritten)
							 .arg(result.rowsDeleted), __func__);
	}
	else{
		SendErrorMessage(0, QStringLiteral("Cache update failed after %1. Error: %2").arg(duration, result.errorMessage), __func__);
	}

	emit updateFinished(result.isOk);

	QMutexLocker observerLocker(&m_observerMutex);

	for (IObserver* observerPtr : std::as_const(m_observers)){
		observerPtr->OnCacheUpdated(result.isOk, startedAtMs);
	}
}


QList<const ICacheTableBuilder*> CCacheBuilderComp::GetOrderedBuilders() const
{
	QList<const ICacheTableBuilder*> pendingBuilders;
	for (int i = 0; i < m_tableBuildersCompPtr.GetCount(); ++ i){
		const ICacheTableBuilder* tableBuilderPtr = m_tableBuildersCompPtr[i];
		if (tableBuilderPtr != nullptr){
			pendingBuilders.append(tableBuilderPtr);
		}
	}

	QList<const ICacheTableBuilder*> retVal;
	QSet<QString> builtTables;

	while (!pendingBuilders.isEmpty()){
		bool isAnyBuilderReady = false;

		for (int i = 0; i < pendingBuilders.count(); ){
			const ICacheTableBuilder* tableBuilderPtr = pendingBuilders.at(i);

			bool areDependenciesReady = true;
			for (const QString& requiredTable : tableBuilderPtr->GetRequiredCacheTables()){
				if (!builtTables.contains(requiredTable)){
					areDependenciesReady = false;

					break;
				}
			}

			if (!areDependenciesReady){
				++ i;

				continue;
			}

			retVal.append(tableBuilderPtr);
			builtTables.insert(tableBuilderPtr->GetCacheTableName());
			pendingBuilders.removeAt(i);
			isAnyBuilderReady = true;
		}

		if (!isAnyBuilderReady){
			// A cycle, or a dependency on a table nobody builds: run the rest in declaration order
			// rather than silently dropping them.
			QStringList unresolvedTables;
			for (const ICacheTableBuilder* tableBuilderPtr : pendingBuilders){
				unresolvedTables << tableBuilderPtr->GetCacheTableName();
			}

			SendWarningMessage(0, QStringLiteral("Unresolved cache table dependencies for: %1").arg(unresolvedTables.join(QStringLiteral(", "))), __func__);

			retVal.append(pendingBuilders);

			break;
		}
	}

	return retVal;
}


ICacheTableBuilder::BuildResult CCacheBuilderComp::RunTableBuilder(
			const ICacheTableBuilder& tableBuilder,
			imtduckdb::IDuckConnection& connection,
			UpdateMode mode) const
{
	const QString tableName = tableBuilder.GetCacheTableName();

	const QDateTime lastSourceUpdateTime = mode == UM_INCREMENTAL
				? GetLastSourceUpdateTime(connection, tableName)
				: QDateTime();

	// Without a previous update time there is nothing to apply changes on top of.
	if (!lastSourceUpdateTime.isValid()){
		return tableBuilder.Rebuild(connection);
	}

	return tableBuilder.ApplyChanges(connection, lastSourceUpdateTime);
}


bool CCacheBuilderComp::EnsureRevisionTable(imtduckdb::IDuckConnection& connection) const
{
	QFile scriptFile(QStringLiteral(":/SQL/DuckDb/CreateCacheRevisionTable.sql"));
	if (!scriptFile.open(QFile::ReadOnly)){
		return false;
	}

	const QByteArray createTableQuery = scriptFile.readAll();
	scriptFile.close();

	QSqlError sqlError;
	connection.ExecSqlQuery(createTableQuery, &sqlError);

	return sqlError.type() == QSqlError::NoError;
}


QDateTime CCacheBuilderComp::GetLastSourceUpdateTime(imtduckdb::IDuckConnection& connection, const QString& tableName) const
{
	// A stored time alone does not prove the cached data survived, so require the table as well.
	QSqlError existsError;
	QSqlQuery existsQuery = connection.ExecSqlQuery(
				QStringLiteral("SELECT EXISTS (SELECT 1 FROM information_schema.tables WHERE table_name = '%1' AND table_schema = current_schema())")
					.arg(imtdb::EscapeSql(tableName)).toUtf8(),
				&existsError);

	if (existsError.type() != QSqlError::NoError || !existsQuery.next() || !existsQuery.value(0).toBool()){
		return QDateTime();
	}

	QSqlError sqlError;
	QSqlQuery query = connection.ExecSqlQuery(
				QStringLiteral(R"(SELECT "%1" FROM "%2" WHERE "%3" = '%4')")
					.arg(CacheRevisionColumn::LAST_REVISION,
						 CacheTable::CACHE_REVISION,
						 CacheRevisionColumn::TABLE_NAME,
						 imtdb::EscapeSql(tableName)).toUtf8(),
				&sqlError);

	if (sqlError.type() != QSqlError::NoError || !query.next()){
		return QDateTime();
	}

	// The value is stored as UTC, yet comes back as zone-less text that Qt reads as local time. Left so, the
	// toUTC() on the next store would shift it back by the local offset on every update.
	QDateTime lastUpdateTime = query.value(0).toDateTime();
	lastUpdateTime.setTimeZone(QTimeZone::utc());

	return lastUpdateTime;
}


bool CCacheBuilderComp::SetLastSourceUpdateTime(imtduckdb::IDuckConnection& connection, const QString& tableName, const QDateTime& updateTime) const
{
	if (!updateTime.isValid()){
		return true;
	}

	QSqlError sqlError;
	connection.ExecSqlQuery(
				QStringLiteral(R"(INSERT INTO "%1" ("%2", "%3") VALUES ('%4', '%5') ON CONFLICT ("%2") DO UPDATE SET "%3" = excluded."%3")")
					.arg(CacheTable::CACHE_REVISION,
						 CacheRevisionColumn::TABLE_NAME,
						 CacheRevisionColumn::LAST_REVISION,
						 imtdb::EscapeSql(tableName),
						 updateTime.toUTC().toString(Qt::ISODateWithMs)).toUtf8(),
				&sqlError);

	if (sqlError.type() != QSqlError::NoError){
		SendErrorMessage(0, QStringLiteral("Unable to store the last update time for %1. Error: %2").arg(tableName, sqlError.text()), __func__);

		return false;
	}

	return true;
}


} // namespace imtcache
