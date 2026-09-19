// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include "CWorkerPoolTest.h"

// STL includes
#include <atomic>

// Qt includes
#include <QtCore/QElapsedTimer>
#include <QtCore/QMutex>
#include <QtCore/QSemaphore>
#include <QtCore/QThread>

// ACF includes
#include <itest/CStandardTestExecutor.h>

// ImtCore includes
#include <imtrest/CWorkerPool.h>


namespace
{


const int c_timeoutMs = 5000;


/**
	Pump the owner's event loop until \a predicate holds. The pool dispatches and reports
	completion through that loop, so nothing advances while the test merely sleeps.
*/
template <typename Predicate>
bool WaitFor(Predicate predicate)
{
	QElapsedTimer timer;
	timer.start();

	while (!predicate()){
		if (timer.elapsed() > c_timeoutMs){
			return false;
		}

		QCoreApplication::processEvents(QEventLoop::AllEvents, 5);
		QThread::msleep(1);
	}

	return true;
}


/**
	What the tasks report back, guarded because they run on worker threads.
*/
class CTaskLog
{
public:
	void OnTaskStarted(int taskId)
	{
		QMutexLocker locker(&m_mutex);

		m_startOrder.append(taskId);

		++m_runningCount;
		m_maxRunningCount = qMax(m_maxRunningCount, m_runningCount);

		m_threadSet.insert(QThread::currentThread());
	}

	void OnTaskFinished()
	{
		QMutexLocker locker(&m_mutex);

		--m_runningCount;
		++m_finishedCount;
	}

	QList<int> GetStartOrder() const
	{
		QMutexLocker locker(&m_mutex);

		return m_startOrder;
	}

	int GetFinishedCount() const
	{
		QMutexLocker locker(&m_mutex);

		return m_finishedCount;
	}

	int GetMaxRunningCount() const
	{
		QMutexLocker locker(&m_mutex);

		return m_maxRunningCount;
	}

	int GetThreadCount() const
	{
		QMutexLocker locker(&m_mutex);

		return m_threadSet.count();
	}

	bool ContainsThread(QThread* threadPtr) const
	{
		QMutexLocker locker(&m_mutex);

		return m_threadSet.contains(threadPtr);
	}

private:
	mutable QMutex m_mutex;
	QList<int> m_startOrder;
	QSet<QThread*> m_threadSet;
	int m_runningCount = 0;
	int m_maxRunningCount = 0;
	int m_finishedCount = 0;
};


/**
	Worker context recording the thread it was constructed on, so that a test can state where
	the pool is required to create it.
*/
class CProbeContext: virtual public istd::IPolymorphic
{
public:
	CProbeContext()
		:m_creationThreadPtr(QThread::currentThread())
	{
	}

	QThread* GetCreationThread() const
	{
		return m_creationThreadPtr;
	}

private:
	QThread* m_creationThreadPtr;
};


imtrest::CWorkerPool::WorkerContextFactory MakeProbeContextFactory()
{
	return [](){
		return imtrest::CWorkerPool::ContextPtr(std::make_unique<CProbeContext>());
	};
}


} // anonymous namespace


void CWorkerPoolTest::testTaskRunsOnAWorkerThread()
{
	QObject owner;
	imtrest::CWorkerPool pool(owner);
	CTaskLog taskLog;

	QVERIFY(pool.PostTask(QByteArray(), [&taskLog](){
		taskLog.OnTaskStarted(1);
		taskLog.OnTaskFinished();
	}));

	QVERIFY(WaitFor([&taskLog](){ return taskLog.GetFinishedCount() == 1; }));
	QVERIFY(!taskLog.ContainsThread(QThread::currentThread()));
}


void CWorkerPoolTest::testTaskPostedFromAnotherThreadIsDispatched()
{
	QObject owner;
	imtrest::CWorkerPool pool(owner);
	CTaskLog taskLog;

	// Dispatch happens on the owner's thread whoever posted the task; a transport thread
	// posting into the pool must not have to be that thread.
	QThread posterThread;
	posterThread.start();

	QSemaphore posted;
	QMetaObject::invokeMethod(
				&posterThread,
				[&pool, &taskLog, &posted](){
					pool.PostTask(QByteArray(), [&taskLog](){
						taskLog.OnTaskStarted(1);
						taskLog.OnTaskFinished();
					});

					posted.release();
				},
				Qt::QueuedConnection);

	QVERIFY(WaitFor([&taskLog](){ return taskLog.GetFinishedCount() == 1; }));
	QVERIFY(posted.tryAcquire(1, c_timeoutMs));

	posterThread.quit();
	QVERIFY(posterThread.wait(c_timeoutMs));
}


void CWorkerPoolTest::testTasksSharingAKeyNeverOverlap()
{
	QObject owner;
	imtrest::CWorkerPool pool(owner);
	pool.SetThreadsLimit(4);

	CTaskLog taskLog;

	for (int taskId = 1; taskId <= 6; ++taskId){
		pool.PostTask("session-1", [&taskLog, taskId](){
			taskLog.OnTaskStarted(taskId);
			QThread::msleep(5);
			taskLog.OnTaskFinished();
		});
	}

	QVERIFY(WaitFor([&taskLog](){ return taskLog.GetFinishedCount() == 6; }));
	QCOMPARE(taskLog.GetMaxRunningCount(), 1);
}


void CWorkerPoolTest::testTasksSharingAKeyRunInSubmissionOrder()
{
	QObject owner;
	imtrest::CWorkerPool pool(owner);
	pool.SetThreadsLimit(4);

	CTaskLog taskLog;

	for (int taskId = 1; taskId <= 6; ++taskId){
		pool.PostTask("session-1", [&taskLog, taskId](){
			taskLog.OnTaskStarted(taskId);
			taskLog.OnTaskFinished();
		});
	}

	QVERIFY(WaitFor([&taskLog](){ return taskLog.GetFinishedCount() == 6; }));
	QCOMPARE(taskLog.GetStartOrder(), QList<int>({1, 2, 3, 4, 5, 6}));
}


void CWorkerPoolTest::testTasksWithDifferentKeysRunAtTheSameTime()
{
	QObject owner;
	imtrest::CWorkerPool pool(owner);
	pool.SetThreadsLimit(2);

	CTaskLog taskLog;
	QSemaphore firstReached;
	QSemaphore secondReached;

	// Each task waits for the other to arrive, so both complete only if they really run at the
	// same time. The wait is bounded, so a pool that serialises them fails rather than hangs.
	pool.PostTask("session-1", [&](){
		taskLog.OnTaskStarted(1);
		firstReached.release();
		secondReached.tryAcquire(1, c_timeoutMs);
		taskLog.OnTaskFinished();
	});

	pool.PostTask("session-2", [&](){
		taskLog.OnTaskStarted(2);
		secondReached.release();
		firstReached.tryAcquire(1, c_timeoutMs);
		taskLog.OnTaskFinished();
	});

	QVERIFY(WaitFor([&taskLog](){ return taskLog.GetFinishedCount() == 2; }));
	QCOMPARE(taskLog.GetMaxRunningCount(), 2);
}


void CWorkerPoolTest::testThreadsLimitIsNotExceeded()
{
	QObject owner;
	imtrest::CWorkerPool pool(owner);
	pool.SetThreadsLimit(2);

	CTaskLog taskLog;

	for (int taskId = 1; taskId <= 8; ++taskId){
		pool.PostTask(QByteArray::number(taskId), [&taskLog, taskId](){
			taskLog.OnTaskStarted(taskId);
			QThread::msleep(5);
			taskLog.OnTaskFinished();
		});
	}

	QVERIFY(WaitFor([&taskLog](){ return taskLog.GetFinishedCount() == 8; }));
	QVERIFY(taskLog.GetThreadCount() <= 2);
	QVERIFY(taskLog.GetMaxRunningCount() <= 2);
}


void CWorkerPoolTest::testWorkerContextIsOfferedToTheRunningTask()
{
	QObject owner;
	imtrest::CWorkerPool pool(owner);
	pool.SetThreadsLimit(1);
	pool.SetWorkerContextFactory(MakeProbeContextFactory());

	std::atomic<istd::IPolymorphic*> firstContextPtr{nullptr};
	std::atomic<istd::IPolymorphic*> secondContextPtr{nullptr};
	std::atomic<int> finishedCount{0};

	pool.PostTask("session-1", [&](){
		firstContextPtr = pool.GetWorkerContext();
		++finishedCount;
	});

	pool.PostTask("session-1", [&](){
		secondContextPtr = pool.GetWorkerContext();
		++finishedCount;
	});

	QVERIFY(WaitFor([&finishedCount](){ return finishedCount.load() == 2; }));

	QVERIFY(firstContextPtr.load() != nullptr);

	// One context per worker, not per task: both tasks ran on the single allowed worker.
	QCOMPARE(secondContextPtr.load(), firstContextPtr.load());

	// Outside a task there is no context to offer.
	QCOMPARE(pool.GetWorkerContext(), nullptr);
}


void CWorkerPoolTest::testWorkerContextIsCreatedOnTheOwnerThread()
{
	QObject owner;
	imtrest::CWorkerPool pool(owner);
	pool.SetWorkerContextFactory(MakeProbeContextFactory());

	std::atomic<QThread*> creationThreadPtr{nullptr};
	std::atomic<QThread*> taskThreadPtr{nullptr};
	std::atomic<bool> isFinished{false};

	pool.PostTask(QByteArray(), [&](){
		const auto* contextPtr = dynamic_cast<const CProbeContext*>(pool.GetWorkerContext());
		if (contextPtr != nullptr){
			creationThreadPtr = contextPtr->GetCreationThread();
		}

		taskThreadPtr = QThread::currentThread();
		isFinished = true;
	});

	QVERIFY(WaitFor([&isFinished](){ return isFinished.load(); }));

	// Created where the pool lives, not on the worker that ends up using it: a factory may
	// build QObjects parented to the application.
	QCOMPARE(creationThreadPtr.load(), owner.thread());
	QVERIFY(taskThreadPtr.load() != owner.thread());
}


void CWorkerPoolTest::testPostIsRefusedAfterShutdown()
{
	QObject owner;
	imtrest::CWorkerPool pool(owner);

	pool.Shutdown();

	std::atomic<bool> hasRun{false};
	QVERIFY(!pool.PostTask(QByteArray(), [&hasRun](){ hasRun = true; }));

	QCoreApplication::processEvents(QEventLoop::AllEvents, 20);
	QVERIFY(!hasRun.load());
}


void CWorkerPoolTest::testQueuedTasksAreDroppedByShutdown()
{
	QObject owner;
	imtrest::CWorkerPool pool(owner);
	pool.SetThreadsLimit(1);

	std::atomic<int> runCount{0};
	QSemaphore blockFirstTask;
	QSemaphore firstTaskReached;

	// Occupy the only worker, so that everything posted behind it is still queued.
	pool.PostTask(QByteArray(), [&](){
		firstTaskReached.release();
		blockFirstTask.tryAcquire(1, c_timeoutMs);
		++runCount;
	});

	QVERIFY(WaitFor([&firstTaskReached](){ return firstTaskReached.available() > 0; }));

	for (int taskId = 0; taskId < 4; ++taskId){
		QVERIFY(pool.PostTask(QByteArray(), [&runCount](){ ++runCount; }));
	}

	blockFirstTask.release();
	pool.Shutdown();

	QCoreApplication::processEvents(QEventLoop::AllEvents, 50);

	// Only the task already handed to a worker ran; the queued ones were dropped.
	QCOMPARE(runCount.load(), 1);
}


I_ADD_TEST(CWorkerPoolTest);
