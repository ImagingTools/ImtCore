// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// Qt includes
#include <QtCore/QDateTime>
#include <QtCore/QFutureWatcher>
#include <QtCore/QMutex>
#include <QtCore/QObject>
#include <QtCore/QTimer>

// ACF includes
#include <ilog/TLoggerCompWrap.h>

// ImtCore includes
#include <imtduckdb/IDuckConnectionProvider.h>

// IotPlatform includes
#include <imtcache/ICacheTableBuilder.h>
#include <imtcache/ICacheUpdateController.h>


namespace imtcache
{


/**
	Runs the registered cache table builders in an order satisfying the dependencies they declare.

	Knows nothing about any particular entity: adding one means registering another
	ICacheTableBuilder, not editing this class.

	Builders run sequentially, on one worker thread and one dedicated connection. That is a choice,
	not a limitation: DeriveSurrogateId() removed the need to look up already-inserted keys, so
	source mirrors no longer depend on each other and ordering only has to respect what the derived
	tables require. Running them concurrently would buy throughput at the price of DuckDB write
	contention and partially-applied failure states, and the problem that actually hurt - a rebuild
	freezing the UI - is solved by running off-thread rather than in parallel.
*/
class CCacheBuilderComp:
			public QObject,
			public ilog::CLoggerComponentBase,
			virtual public ICacheUpdateController
{
	Q_OBJECT
public:
	using BaseClass = ilog::CLoggerComponentBase;

	I_BEGIN_COMPONENT(CCacheBuilderComp)
		I_REGISTER_INTERFACE(ICacheUpdateController)
		I_ASSIGN_MULTI_0(m_tableBuildersCompPtr, "TableBuilders", "Cache table builders to run", true);
		I_ASSIGN(m_connectionProviderCompPtr, "CacheConnectionProvider", "DuckDB cache database, used to take a dedicated builder connection", true, "CacheDatabaseEngine");
		I_ASSIGN(m_updateIntervalSecAttrPtr, "UpdateIntervalSec", "Interval between automatic incremental updates in seconds. No automatic updates if disabled", false, 300);
		I_ASSIGN(m_buildOnStartupAttrPtr, "BuildOnStartup", "Run one update shortly after startup, so the cache is populated before it is queried", true, true);
	I_END_COMPONENT;

	// reimplemented (imtcache::ICacheUpdateController)
	virtual UpdateResult Update(UpdateMode mode = UM_INCREMENTAL) override;
	virtual bool RequestUpdate(UpdateMode mode = UM_INCREMENTAL) override;
	virtual bool IsUpdateRunning() const override;
	virtual void SuspendUpdates() override;
	virtual void ResumeUpdates() override;
	virtual void AttachObserver(IObserver* observerPtr) override;
	virtual void DetachObserver(IObserver* observerPtr) override;

Q_SIGNALS:
	void updateFinished(bool isOk);

protected:
	// reimplemented (icomp::CComponentBase)
	virtual void OnComponentCreated() override;
	virtual void OnComponentDestroyed() override;

private Q_SLOTS:
	void OnUpdateFinished();

private:
	/// Body executed on the worker thread; takes its own connection and touches no shared state.
	void RunUpdate(UpdateMode mode);

	/// Orders the registered builders so each one follows the tables it requires.
	QList<const ICacheTableBuilder*> GetOrderedBuilders() const;

	/// Runs one builder, falling back to a rebuild when no previous update time is available.
	ICacheTableBuilder::BuildResult RunTableBuilder(
				const ICacheTableBuilder& tableBuilder,
				imtduckdb::IDuckConnection& connection,
				UpdateMode mode) const;

	/// Time of the newest source change already mirrored into \a tableName, if that table still exists.
	QDateTime GetLastSourceUpdateTime(imtduckdb::IDuckConnection& connection, const QString& tableName) const;
	bool SetLastSourceUpdateTime(imtduckdb::IDuckConnection& connection, const QString& tableName, const QDateTime& updateTime) const;
	bool EnsureRevisionTable(imtduckdb::IDuckConnection& connection) const;

private:
	I_MULTIREF(ICacheTableBuilder, m_tableBuildersCompPtr);
	I_REF(imtduckdb::IDuckConnectionProvider, m_connectionProviderCompPtr);
	I_ATTR(int, m_updateIntervalSecAttrPtr);
	I_ATTR(bool, m_buildOnStartupAttrPtr);

	QFutureWatcher<void> m_updateWatcher;
	QTimer* m_updateTimerPtr = nullptr;

	mutable QMutex m_stateMutex;
	bool m_isUpdateRunning = false;
	int m_suspendCount = 0;
	bool m_hasPendingRequest = false;
	UpdateMode m_pendingMode = UM_INCREMENTAL;

	// Held while the observers are called, so a detached observer is never called afterwards.
	QMutex m_observerMutex;
	QList<IObserver*> m_observers;
};


} // namespace imtcache
