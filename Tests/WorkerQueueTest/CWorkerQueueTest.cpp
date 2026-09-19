// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include "CWorkerQueueTest.h"

// ACF includes
#include <itest/CStandardTestExecutor.h>

// ImtCore includes
#include <imtrest/CWorkerTaskSchedule.h>


namespace
{


/**
	Records the order in which the tasks it hands out are executed, so that a test can state
	its expectation as the sequence of task ids it wants to see.
*/
class CRunLog
{
public:
	imtrest::CWorkerTaskSchedule::Task MakeTask(int taskId)
	{
		return [this, taskId](){
			m_runOrder.append(taskId);
		};
	}

	QList<int> GetRunOrder() const
	{
		return m_runOrder;
	}

private:
	QList<int> m_runOrder;
};


/**
	Take the next dispatchable task and run it at once, as a pool with a single free worker
	would. Returns false when the schedule has nothing runnable.
*/
bool RunNext(imtrest::CWorkerTaskSchedule& schedule)
{
	imtrest::CWorkerTaskSchedule::Item item;
	if (!schedule.TakeDispatchable(item)){
		return false;
	}

	item.task();

	schedule.ReleaseKey(item.orderingKey);

	return true;
}


/**
	Take the next dispatchable task and run it without releasing its key, as a pool whose
	worker is still busy with it would leave the schedule. Returns the key left in flight.
*/
bool StartNext(imtrest::CWorkerTaskSchedule& schedule, QByteArray* startedKeyPtr = nullptr)
{
	imtrest::CWorkerTaskSchedule::Item item;
	if (!schedule.TakeDispatchable(item)){
		return false;
	}

	item.task();

	if (startedKeyPtr != nullptr){
		*startedKeyPtr = item.orderingKey;
	}

	return true;
}


} // anonymous namespace


void CWorkerQueueTest::testEmptyScheduleDispatchesNothing()
{
	imtrest::CWorkerTaskSchedule schedule;

	imtrest::CWorkerTaskSchedule::Item item;
	QVERIFY(!schedule.TakeDispatchable(item));
	QCOMPARE(schedule.GetPendingCount(), 0);
}


void CWorkerQueueTest::testEmptyTaskIsRefused()
{
	imtrest::CWorkerTaskSchedule schedule;

	QVERIFY(!schedule.Append(QByteArray(), imtrest::CWorkerTaskSchedule::Task()));
	QCOMPARE(schedule.GetPendingCount(), 0);
}


void CWorkerQueueTest::testUnkeyedTasksLeaveInSubmissionOrder()
{
	imtrest::CWorkerTaskSchedule schedule;
	CRunLog runLog;

	QVERIFY(schedule.Append(QByteArray(), runLog.MakeTask(1)));
	QVERIFY(schedule.Append(QByteArray(), runLog.MakeTask(2)));
	QVERIFY(schedule.Append(QByteArray(), runLog.MakeTask(3)));
	QCOMPARE(schedule.GetPendingCount(), 3);

	while (RunNext(schedule)){
	}

	QCOMPARE(runLog.GetRunOrder(), QList<int>({1, 2, 3}));
	QCOMPARE(schedule.GetPendingCount(), 0);
}


void CWorkerQueueTest::testTasksSharingAKeyDoNotRunConcurrently()
{
	imtrest::CWorkerTaskSchedule schedule;
	CRunLog runLog;

	schedule.Append("session-1", runLog.MakeTask(1));
	schedule.Append("session-1", runLog.MakeTask(2));

	// First one starts and stays in flight, as it would while its worker is still running it.
	QVERIFY(StartNext(schedule));
	QVERIFY(schedule.IsKeyInFlight("session-1"));

	// The second must not be handed out while the first holds the key, even though a worker
	// would be free to take it.
	imtrest::CWorkerTaskSchedule::Item item;
	QVERIFY(!schedule.TakeDispatchable(item));
	QCOMPARE(schedule.GetPendingCount(), 1);
	QCOMPARE(runLog.GetRunOrder(), QList<int>({1}));
}


void CWorkerQueueTest::testTasksSharingAKeyKeepSubmissionOrder()
{
	imtrest::CWorkerTaskSchedule schedule;
	CRunLog runLog;

	for (int taskId = 1; taskId <= 5; ++taskId){
		schedule.Append("session-1", runLog.MakeTask(taskId));
	}

	while (RunNext(schedule)){
	}

	QCOMPARE(runLog.GetRunOrder(), QList<int>({1, 2, 3, 4, 5}));
}


void CWorkerQueueTest::testTasksWithDifferentKeysRunConcurrently()
{
	imtrest::CWorkerTaskSchedule schedule;
	CRunLog runLog;

	schedule.Append("session-1", runLog.MakeTask(1));
	schedule.Append("session-2", runLog.MakeTask(2));

	// Both may be in flight at the same time: different keys never block one another.
	QVERIFY(StartNext(schedule));
	QVERIFY(StartNext(schedule));

	QVERIFY(schedule.IsKeyInFlight("session-1"));
	QVERIFY(schedule.IsKeyInFlight("session-2"));
	QCOMPARE(runLog.GetRunOrder(), QList<int>({1, 2}));
}


void CWorkerQueueTest::testBusyKeyDoesNotStallUnrelatedWork()
{
	imtrest::CWorkerTaskSchedule schedule;
	CRunLog runLog;

	schedule.Append("session-1", runLog.MakeTask(1));
	schedule.Append("session-1", runLog.MakeTask(2));
	schedule.Append("session-2", runLog.MakeTask(3));

	// Task 1 holds session-1, so task 2 is blocked. Task 3 sits behind it in the queue and
	// must still be dispatched - a blocked key is skipped, not a barrier.
	QVERIFY(StartNext(schedule));
	QVERIFY(StartNext(schedule));

	QCOMPARE(runLog.GetRunOrder(), QList<int>({1, 3}));
	QCOMPARE(schedule.GetPendingCount(), 1);
}


void CWorkerQueueTest::testReleasingAKeyMakesTheNextTaskDispatchable()
{
	imtrest::CWorkerTaskSchedule schedule;
	CRunLog runLog;

	schedule.Append("session-1", runLog.MakeTask(1));
	schedule.Append("session-1", runLog.MakeTask(2));

	QByteArray startedKey;
	QVERIFY(StartNext(schedule, &startedKey));

	imtrest::CWorkerTaskSchedule::Item blockedItem;
	QVERIFY(!schedule.TakeDispatchable(blockedItem));

	schedule.ReleaseKey(startedKey);
	QVERIFY(!schedule.IsKeyInFlight("session-1"));

	QVERIFY(RunNext(schedule));
	QCOMPARE(runLog.GetRunOrder(), QList<int>({1, 2}));
}


void CWorkerQueueTest::testUnkeyedTasksIgnoreKeysInFlight()
{
	imtrest::CWorkerTaskSchedule schedule;
	CRunLog runLog;

	schedule.Append("session-1", runLog.MakeTask(1));
	schedule.Append("session-1", runLog.MakeTask(2));
	schedule.Append(QByteArray(), runLog.MakeTask(3));

	QVERIFY(StartNext(schedule));

	// An empty key means no ordering constraint, so task 3 runs although task 2 is blocked.
	QVERIFY(StartNext(schedule));

	QCOMPARE(runLog.GetRunOrder(), QList<int>({1, 3}));
}


void CWorkerQueueTest::testShutdownDropsPendingTasksWithoutRunningThem()
{
	imtrest::CWorkerTaskSchedule schedule;
	CRunLog runLog;

	schedule.Append("session-1", runLog.MakeTask(1));
	schedule.Append(QByteArray(), runLog.MakeTask(2));

	const QList<imtrest::CWorkerTaskSchedule::Item> droppedList = schedule.Shutdown();

	QCOMPARE(droppedList.count(), 2);
	QCOMPARE(schedule.GetPendingCount(), 0);
	QVERIFY(schedule.IsShuttingDown());

	// Handed back to the caller to destroy, and never executed.
	QVERIFY(runLog.GetRunOrder().isEmpty());

	imtrest::CWorkerTaskSchedule::Item item;
	QVERIFY(!schedule.TakeDispatchable(item));
}


void CWorkerQueueTest::testAppendIsRefusedAfterShutdown()
{
	imtrest::CWorkerTaskSchedule schedule;
	CRunLog runLog;

	schedule.Shutdown();

	QVERIFY(!schedule.Append(QByteArray(), runLog.MakeTask(1)));
	QVERIFY(!schedule.Append("session-1", runLog.MakeTask(2)));
	QCOMPARE(schedule.GetPendingCount(), 0);
}


I_ADD_TEST(CWorkerQueueTest);
