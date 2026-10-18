// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// Qt includes
#include <QtCore/QObject>
#include <QtTest/QtTest>

// ACF includes
#include <itest/CStandardTestExecutor.h>


class CConnectionTimeoutTrackerTest: public QObject
{
	Q_OBJECT
private slots:
	void DefaultConfigTest();
	void DefaultTrackerTest();

	// Idle connection
	void NewConnectionIsIdleTest();
	void IdleTimeoutIsCountedFromOpeningTest();
	void FirstDataStopsKeepAliveTimeoutTest();

	// Receiving of a request
	void ReceivingInactivityTimeoutTest();
	void ActiveReceivingIsNotTimedOutTest();

	// Processing and sending of a response
	void ProcessingHasNoTimeoutTest();
	void DefaultProcessingTimeoutIsDisabledTest();
	void ProcessingTimeoutTest();
	void ProcessingTimeoutIsCountedFromRequestEndTest();
	void ProcessingTimeoutIgnoresDataTest();
	void ProcessingTimeoutEndsWithResponseTest();
	void ProcessingTimeoutOfPipelinedRequestTest();
	void ProcessingTimeoutClampedTest();
	void ProcessingTimeoutClosingTest();
	void SendingStallTimeoutTest();
	void SendingProgressDelaysTimeoutTest();
	void ResponseCompletedStartsKeepAliveTest();
	void KeepAliveConnectionReuseTest();

	// Disabled timeouts
	void DisabledTimeoutsTest_data();
	void DisabledTimeoutsTest();
	void DisabledKeepAliveDoesNotAffectOthersTest();

	// Several requests at the same time
	void DataDuringProcessingDoesNotChangeDeadlinesTest();
	void PipelinedRequestsTest();
	void RequestCompletedDuringSendingTest();
	void MessageWithoutRequestTest();
	void ResponseWhileReceivingTest();

	// Closing
	void ClosingTest();
	void ClosingIgnoresEventsTest();
	void ImmediateClosingTest_data();
	void ImmediateClosingTest();

	// Misc
	void UnexpectedEventsAreIgnoredTest();
	void ReopeningResetsTest();
	void LargeTimeTest();
};


