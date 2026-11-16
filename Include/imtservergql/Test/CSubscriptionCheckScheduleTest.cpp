// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include "CSubscriptionCheckScheduleTest.h"


#include <itest/CStandardTestExecutor.h>
#include <imtservergql/CSubscriptionCheckSchedule.h>


namespace
{


typedef imtservergql::CSubscriptionCheckSchedule Schedule;

const QByteArray s_key = "credentials";
const qint64 s_control = 300000;
const qint64 s_grace = 60000;


// The first check at t0 does not confirm the access
void FailFirstCheck(Schedule& schedule, qint64 t0, Schedule::CheckResult result = Schedule::CR_UNAVAILABLE)
{
	schedule.Add(s_key, 0);
	schedule.OnAccessChanged(t0);
	QCOMPARE(schedule.TakeDue(t0), QByteArrayList{s_key});
	schedule.OnChecked(s_key, result, t0 + 500);
}


} // namespace


void CSubscriptionCheckScheduleTest::NoCheckBeforeTheControlInterval()
{
	Schedule schedule;
	schedule.Add(s_key, 0);

	for (const qint64 nowMs : {qint64(1), qint64(1000), qint64(150000), s_control - 1}){
		QVERIFY(schedule.TakeDue(nowMs).isEmpty());
		QVERIFY(schedule.TakeExpired(nowMs).isEmpty());
	}
	QCOMPARE(schedule.TakeDue(s_control), QByteArrayList{s_key});
}


void CSubscriptionCheckScheduleTest::DeadlineBringsTheCheckForward()
{
	Schedule schedule;
	schedule.Add(s_key, 0, 120000);

	QVERIFY(schedule.TakeDue(119999).isEmpty());
	QCOMPARE(schedule.TakeDue(120000), QByteArrayList{s_key});

	// A deadline after the control interval does not delay the control
	schedule.OnChecked(s_key, Schedule::CR_CONFIRMED, 120000, 120000 + 2 * s_control);
	QVERIFY(schedule.TakeDue(120000 + s_control - 1).isEmpty());
	QCOMPARE(schedule.TakeDue(120000 + s_control), QByteArrayList{s_key});
}


void CSubscriptionCheckScheduleTest::PassedDeadlineIsNotCheckedInALoop()
{
	Schedule schedule;
	schedule.Add(s_key, 0, 120000);
	QCOMPARE(schedule.TakeDue(120000), QByteArrayList{s_key});

	// The session still lives after its deadline (refreshed meanwhile, another clock): checked again at the pace of a repetition
	schedule.OnChecked(s_key, Schedule::CR_CONFIRMED, 120100, 120000);
	QVERIFY(schedule.TakeDue(120100 + 9999).isEmpty());
	QCOMPARE(schedule.TakeDue(120100 + 10000), QByteArrayList{s_key});
}


void CSubscriptionCheckScheduleTest::NearDeadlineIsNotPostponed()
{
	Schedule schedule;
	schedule.Add(s_key, 0, 5000);
	QVERIFY(schedule.TakeDue(4999).isEmpty());
	QCOMPARE(schedule.TakeDue(5000), QByteArrayList{s_key});
}


void CSubscriptionCheckScheduleTest::LateConfirmationDoesNotEndTheGrace()
{
	for (const qint64 elapsedMs : {s_grace, s_grace + 1}){
		Schedule schedule;
		schedule.Add(s_key, 0);
		schedule.OnAccessChanged(1000);
		QCOMPARE(schedule.TakeDue(1000), QByteArrayList{s_key});

		// The worker can obtain the lock before the deadline thread wakes.
		schedule.OnChecked(s_key, Schedule::CR_CONFIRMED, 1000 + elapsedMs);
		QCOMPARE(schedule.TakeExpired(1000 + elapsedMs), QByteArrayList{s_key});
	}
}


void CSubscriptionCheckScheduleTest::ConfirmationSchedulesTheNextControl()
{
	Schedule schedule;
	schedule.Add(s_key, 0);
	QCOMPARE(schedule.TakeDue(s_control), QByteArrayList{s_key});
	QVERIFY(schedule.TakeDue(s_control).isEmpty());

	schedule.OnChecked(s_key, Schedule::CR_CONFIRMED, s_control + 700);
	QVERIFY(schedule.TakeDue(2 * s_control + 699).isEmpty());
	QCOMPARE(schedule.TakeDue(2 * s_control + 700), QByteArrayList{s_key});
}


void CSubscriptionCheckScheduleTest::GraceEndsAtItsBoundary_data()
{
	QTest::addColumn<qint64>("offset");
	QTest::addColumn<bool>("isExpired");

	QTest::newRow("59 999 ms") << qint64(59999) << false;
	QTest::newRow("60 000 ms") << qint64(60000) << true;
	QTest::newRow("60 001 ms") << qint64(60001) << true;
}


void CSubscriptionCheckScheduleTest::GraceEndsAtItsBoundary()
{
	QFETCH(qint64, offset);
	QFETCH(bool, isExpired);

	const qint64 t0 = 1000;
	Schedule schedule;
	FailFirstCheck(schedule, t0);

	QCOMPARE(schedule.TakeExpired(t0 + offset), isExpired ? QByteArrayList{s_key} : QByteArrayList());
	QCOMPARE(schedule.Contains(s_key), !isExpired);
}


void CSubscriptionCheckScheduleTest::GraceCountsFromTheFirstFailure()
{
	const qint64 t0 = 1000;
	Schedule schedule;
	FailFirstCheck(schedule, t0);

	// Further failed attempts do not move it
	for (qint64 attemptMs = t0 + 10000; attemptMs < t0 + s_grace; attemptMs += 10000){
		QVERIFY(schedule.TakeExpired(attemptMs).isEmpty());
		QCOMPARE(schedule.TakeDue(attemptMs), QByteArrayList{s_key});
		schedule.OnChecked(s_key, Schedule::CR_UNAVAILABLE, attemptMs + 300);
	}
	QVERIFY(schedule.TakeExpired(t0 + s_grace - 1).isEmpty());
	QCOMPARE(schedule.TakeExpired(t0 + s_grace), QByteArrayList{s_key});
}


void CSubscriptionCheckScheduleTest::ConfirmationEndsTheGrace()
{
	const qint64 t0 = 1000;
	Schedule schedule;
	FailFirstCheck(schedule, t0);

	QCOMPARE(schedule.TakeDue(t0 + 10000), QByteArrayList{s_key});
	schedule.OnChecked(s_key, Schedule::CR_CONFIRMED, t0 + 10200);
	QVERIFY(schedule.TakeExpired(t0 + s_grace).isEmpty());
	QVERIFY(schedule.TakeExpired(t0 + 10200 + s_control - 1).isEmpty());

	// A new failure starts a new grace period
	QCOMPARE(schedule.TakeDue(t0 + 10200 + s_control), QByteArrayList{s_key});
	schedule.OnChecked(s_key, Schedule::CR_UNAVAILABLE, t0 + 10300 + s_control);
	QVERIFY(schedule.TakeExpired(t0 + 10200 + s_control + s_grace - 1).isEmpty());
	QCOMPARE(schedule.TakeExpired(t0 + 10200 + s_control + s_grace), QByteArrayList{s_key});
}


void CSubscriptionCheckScheduleTest::StaleDoesNotEndTheGrace()
{
	const qint64 t0 = 1000;
	Schedule schedule;
	FailFirstCheck(schedule, t0);

	// The access keeps changing: the wait for a confirmed access ends like any other
	qint64 attemptMs = t0 + 10000;
	while (attemptMs < t0 + s_grace){
		QCOMPARE(schedule.TakeDue(attemptMs), QByteArrayList{s_key});
		schedule.OnChecked(s_key, Schedule::CR_STALE, attemptMs + 10);
		attemptMs += 1000;
	}
	QCOMPARE(schedule.TakeExpired(t0 + s_grace), QByteArrayList{s_key});
}


void CSubscriptionCheckScheduleTest::RepetitionsKeepTheirPace_data()
{
	QTest::addColumn<int>("result");
	QTest::addColumn<qint64>("pace");

	QTest::newRow("unavailable") << int(Schedule::CR_UNAVAILABLE) << qint64(10000);
	QTest::newRow("stale") << int(Schedule::CR_STALE) << qint64(1000);
}


void CSubscriptionCheckScheduleTest::RepetitionsKeepTheirPace()
{
	QFETCH(int, result);
	QFETCH(qint64, pace);

	const qint64 t0 = 1000;
	Schedule schedule;
	FailFirstCheck(schedule, t0, Schedule::CheckResult(result));

	// A change does not make a failing source asked more often
	schedule.OnAccessChanged(t0 + 600);
	QVERIFY(schedule.TakeDue(t0 + 600).isEmpty());
	QVERIFY(schedule.TakeDue(t0 + pace - 1).isEmpty());
	QCOMPARE(schedule.TakeDue(t0 + pace), QByteArrayList{s_key});
}


void CSubscriptionCheckScheduleTest::ChangeDoesNotRestartTheGrace()
{
	const qint64 t0 = 1000;
	Schedule schedule;
	FailFirstCheck(schedule, t0);

	for (qint64 changeMs = t0 + 1000; changeMs < t0 + s_grace; changeMs += 5000){
		schedule.OnAccessChanged(changeMs);
	}
	QCOMPARE(schedule.TakeExpired(t0 + s_grace), QByteArrayList{s_key});
}


void CSubscriptionCheckScheduleTest::ChangesCoalesce()
{
	Schedule schedule;
	schedule.Add(s_key, 0);
	schedule.Add("other", 0);

	for (qint64 changeMs = 10; changeMs < 20; ++changeMs){
		schedule.OnAccessChanged(changeMs);
	}
	QByteArrayList due = schedule.TakeDue(20);
	std::sort(due.begin(), due.end());
	QCOMPARE(due, (QByteArrayList{s_key, "other"}));
	QVERIFY(schedule.TakeDue(20).isEmpty());
}


void CSubscriptionCheckScheduleTest::ChangeDuringCheckChecksAgain()
{
	Schedule schedule;
	schedule.Add(s_key, 0);
	schedule.OnAccessChanged(10);
	QCOMPARE(schedule.TakeDue(10), QByteArrayList{s_key});

	// The running check may have read before the change
	schedule.OnAccessChanged(20);
	QVERIFY(schedule.TakeDue(20).isEmpty());
	schedule.OnChecked(s_key, Schedule::CR_CONFIRMED, 30);
	QCOMPARE(schedule.TakeDue(30), QByteArrayList{s_key});

	// Without a change during it, the next check is the control
	schedule.OnChecked(s_key, Schedule::CR_CONFIRMED, 40);
	QVERIFY(schedule.TakeDue(40 + s_control - 1).isEmpty());
}


void CSubscriptionCheckScheduleTest::HungCheckEndsAtTheGraceBoundary()
{
	const qint64 t0 = 5000;
	Schedule schedule;
	schedule.Add(s_key, 0);
	schedule.OnAccessChanged(t0);
	QCOMPARE(schedule.TakeDue(t0), QByteArrayList{s_key});

	// The check never answers
	QVERIFY(schedule.TakeExpired(t0 + s_grace - 1).isEmpty());
	QCOMPARE(schedule.TakeExpired(t0 + s_grace), QByteArrayList{s_key});

	// Its late answer does not bring the set back
	schedule.OnChecked(s_key, Schedule::CR_CONFIRMED, t0 + s_grace + 100);
	QVERIFY(!schedule.Contains(s_key));
}


void CSubscriptionCheckScheduleTest::RemovalDuringCheck()
{
	Schedule schedule;
	schedule.Add(s_key, 0);
	schedule.OnAccessChanged(10);
	QCOMPARE(schedule.TakeDue(10), QByteArrayList{s_key});

	schedule.Remove(s_key);
	schedule.OnChecked(s_key, Schedule::CR_UNAVAILABLE, 20);
	QVERIFY(!schedule.Contains(s_key));
	QVERIFY(schedule.TakeExpired(10 + s_grace).isEmpty());
	QCOMPARE(schedule.GetNextWakeTime(), qint64(-1));
}


void CSubscriptionCheckScheduleTest::LateAnswerOfAnEarlierAttempt()
{
	Schedule schedule;
	schedule.Add(s_key, 0);
	schedule.OnAccessChanged(10);
	QCOMPARE(schedule.TakeDue(10), QByteArrayList{s_key});
	const qint64 earlierAttemptId = schedule.GetAttemptId(s_key);
	QVERIFY(schedule.IsCurrentAttempt(s_key, earlierAttemptId));

	// Removed and added again with the same credentials: the answer of the earlier check is not the one of the new set
	schedule.Remove(s_key);
	schedule.Add(s_key, 20);
	QVERIFY(!schedule.IsCurrentAttempt(s_key, earlierAttemptId));
	schedule.OnAccessChanged(30);
	QCOMPARE(schedule.TakeDue(30), QByteArrayList{s_key});
	QVERIFY(!schedule.IsCurrentAttempt(s_key, earlierAttemptId));
	QVERIFY(schedule.IsCurrentAttempt(s_key, schedule.GetAttemptId(s_key)));

	// An answered attempt is current no more
	const qint64 attemptId = schedule.GetAttemptId(s_key);
	schedule.OnChecked(s_key, Schedule::CR_CONFIRMED, 40);
	QVERIFY(!schedule.IsCurrentAttempt(s_key, attemptId));
}


void CSubscriptionCheckScheduleTest::NextWakeTime()
{
	Schedule schedule;
	QCOMPARE(schedule.GetNextWakeTime(), qint64(-1));

	schedule.Add(s_key, 0);
	QCOMPARE(schedule.GetNextWakeTime(), s_control);

	// A running check wakes at the end of its grace period
	schedule.OnAccessChanged(100);
	QCOMPARE(schedule.GetNextWakeTime(), qint64(100));
	QCOMPARE(schedule.TakeDue(100), QByteArrayList{s_key});
	QCOMPARE(schedule.GetNextWakeTime(), 100 + s_grace);

	// A failed one at its repetition, before the end of its grace period
	schedule.OnChecked(s_key, Schedule::CR_UNAVAILABLE, 200);
	QCOMPARE(schedule.GetNextWakeTime(), qint64(100 + 10000));
}


I_ADD_TEST(CSubscriptionCheckScheduleTest);
