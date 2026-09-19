// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once

// Qt includes
#include <QtCore/QObject>
#include <QtTest/QtTest>


/**
	imtrest::CWorkerPool driven with live worker threads: the guarantees CWorkerQueueTest
	settles on the schedule alone, as they come out of the pool that dispatches them, plus the
	dispatch and context rules that only a running pool can show.
*/
class CWorkerPoolTest: public QObject
{
	Q_OBJECT

private Q_SLOTS:
	void testTaskRunsOnAWorkerThread();
	void testTaskPostedFromAnotherThreadIsDispatched();
	void testTasksSharingAKeyNeverOverlap();
	void testTasksSharingAKeyRunInSubmissionOrder();
	void testTasksWithDifferentKeysRunAtTheSameTime();
	void testThreadsLimitIsNotExceeded();
	void testWorkerContextIsOfferedToTheRunningTask();
	void testWorkerContextIsCreatedOnTheOwnerThread();
	void testPostIsRefusedAfterShutdown();
	void testQueuedTasksAreDroppedByShutdown();
};
