// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// Qt includes
#include <QtCore/QObject>
#include <QtTest/QtTest>

// ACF includes
#include <itest/CStandardTestExecutor.h>


class CSocketTimeoutWatcherTest: public QObject
{
	Q_OBJECT
private slots:
	// Idle connection
	void IdleConnectionIsClosedTest();
	void IdleConnectionIsNotClosedEarlyTest();

	// Incomplete request
	void SlowClientIsNotTimedOutTest();
	void IncompleteRequestGetsRequestTimeoutTest();
	void LingeringCloseAbortsConnectionTest();
	void IncompleteRequestOfOtherProtocolClosesConnectionTest();
	void CustomBehaviorSendsItsOwnMessageTest();
	void EmptyMessageAbortsConnectionTest();

	// Processing and response
	void ProcessingIsNotTimedOutTest();
	void ResponseStartsKeepAliveTimeoutTest();
	void KeepAliveConnectionIsReusedTest();
	void StalledResponseIsAbortedTest();

	// Other protocols and disconnection
	void OtherProtocolIsClosedBeforeFirstMessageTest();
	void ChannelOfOtherProtocolIsNotLimitedTest();
	void NoTimeoutAfterDisconnectionTest();

	// Lifecycle of the requests
	void PhasesFollowRequestLifecycleTest();
	void PipelinedRequestsAreAnsweredOneByOneTest();
	void IncompleteSecondRequestGetsRequestTimeoutTest();
	void PartialNextRequestDuringProcessingIsTimedOutAfterResponseTest();

	// Closing, limits and behavior
	void EventsAreIgnoredWhileClosingTest();
	void ZeroLingeringTimeoutAbortsRightAfterMessageTest();
	void DisabledTimeoutsNeverFireTest();
	void MessageFactoryIsCalledOnlyOnTimeoutTest();
	void PartialMessageAfterFirstOfOtherProtocolIsNotLimitedTest();
	void DeletedSocketIsHandledTest();

	// Closing after the response
	void ConnectionIsClosedAfterResponseTest();
	void ConnectionIsNotClosedBeforeResponseTest();
	void DataAfterCloseRequestIsIgnoredTest();
	void ResponsesBeforeClosingRequestKeepConnectionTest();
	void LargeResponseIsDeliveredBeforeClosingTest();

	// Rejection of data that are not a request
	void RejectedRequestGetsBadRequestTest();
	void RejectedConnectionIsAbortedAfterLingeringTest();
	void RejectedRequestOfOtherProtocolIsAbortedTest();
	void RejectedRequestGetsMessageOfBehaviorTest();
	void RejectedConnectionIgnoresEventsTest();
	void RejectionIsSentOnceTest();
	void RejectionAfterRequestTimeoutIsIgnoredTest();

	// Processing timeout
	void ProcessingTimeoutGetsServiceUnavailableTest();
	void ProcessingTimeoutAbortedAfterLingeringTest();
	void LateResponseAfterProcessingTimeoutIsIgnoredTest();
	void ResponseInTimeCancelsProcessingTimeoutTest();
	void ProcessingTimeoutIsDisabledByDefaultTest();
	void ProcessingTimeoutWithoutMessageClosesConnectionTest();
	void ProcessingTimeoutGetsMessageOfBehaviorTest();
	void ProcessingTimeoutOfPipelinedRequestTest();
};


