// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once

// Qt includes
#include <QtCore/QObject>
#include <QtTest/QtTest>


/**
	Ordering guarantees of imtrest::IWorkerTaskQueue, exercised on the scheduling policy
	that implements them (imtrest::CWorkerTaskSchedule) rather than on a live thread pool,
	so that every case is decided deterministically and without timing.
*/
class CWorkerQueueTest: public QObject
{
	Q_OBJECT

private Q_SLOTS:
	void testEmptyScheduleDispatchesNothing();
	void testEmptyTaskIsRefused();

	// Ordering guarantees
	void testUnkeyedTasksLeaveInSubmissionOrder();
	void testTasksSharingAKeyDoNotRunConcurrently();
	void testTasksSharingAKeyKeepSubmissionOrder();
	void testTasksWithDifferentKeysRunConcurrently();
	void testBusyKeyDoesNotStallUnrelatedWork();
	void testReleasingAKeyMakesTheNextTaskDispatchable();
	void testUnkeyedTasksIgnoreKeysInFlight();

	// Shutdown
	void testShutdownDropsPendingTasksWithoutRunningThem();
	void testAppendIsRefusedAfterShutdown();
};
