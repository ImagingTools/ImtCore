// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


#include <QtTest/QtTest>


/**
	When imtservergql::CSubscriptionCheckSchedule checks the access of subscriptions again: the control interval, the deadline,
	the changes of the access, and the grace period of a check that does not confirm it.
*/
class CSubscriptionCheckScheduleTest: public QObject
{
	Q_OBJECT

private slots:
	void NoCheckBeforeTheControlInterval();
	void DeadlineBringsTheCheckForward();
	void NearDeadlineIsNotPostponed();
	void LateConfirmationDoesNotEndTheGrace();
	void PassedDeadlineIsNotCheckedInALoop();
	void ConfirmationSchedulesTheNextControl();
	void GraceEndsAtItsBoundary_data();
	void GraceEndsAtItsBoundary();
	void GraceCountsFromTheFirstFailure();
	void ConfirmationEndsTheGrace();
	void StaleDoesNotEndTheGrace();
	void RepetitionsKeepTheirPace_data();
	void RepetitionsKeepTheirPace();
	void ChangeDoesNotRestartTheGrace();
	void ChangesCoalesce();
	void ChangeDuringCheckChecksAgain();
	void HungCheckEndsAtTheGraceBoundary();
	void RemovalDuringCheck();
	void LateAnswerOfAnEarlierAttempt();
	void NextWakeTime();
};
