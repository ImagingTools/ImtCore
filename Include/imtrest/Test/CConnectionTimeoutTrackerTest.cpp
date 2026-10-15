// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include "CConnectionTimeoutTrackerTest.h"


// ImtCore includes
#include <imtrest/CConnectionTimeoutTracker.h>


namespace
{


typedef imtrest::CConnectionTimeoutTracker Tracker;


const qint64 s_never = -1;


imtrest::CConnectionTimeoutConfig CreateConfig(int requestInactivityMs, int keepAliveMs, int lingeringCloseMs)
{
	imtrest::CConnectionTimeoutConfig config;
	config.requestInactivityMs = requestInactivityMs;
	config.keepAliveMs = keepAliveMs;
	config.lingeringCloseMs = lingeringCloseMs;

	return config;
}


// Request inactivity 1000 ms, keep-alive 500 ms, lingering 200 ms
imtrest::CConnectionTimeoutConfig CreateTestConfig()
{
	return CreateConfig(1000, 500, 200);
}


} // namespace


// private slots

void CConnectionTimeoutTrackerTest::DefaultConfigTest()
{
	const imtrest::CConnectionTimeoutConfig config;

	QCOMPARE(config.requestInactivityMs, 30000);
	QCOMPARE(config.keepAliveMs, 15000);
	QCOMPARE(config.lingeringCloseMs, 5000);
}


void CConnectionTimeoutTrackerTest::DefaultTrackerTest()
{
	Tracker tracker;
	tracker.OnConnectionOpened(0);

	QCOMPARE(tracker.GetPhase(), Tracker::P_IDLE);
	QCOMPARE(tracker.GetDeadline(), qint64(15000));
}


void CConnectionTimeoutTrackerTest::NewConnectionIsIdleTest()
{
	Tracker tracker(CreateTestConfig());
	tracker.OnConnectionOpened(1000);

	QCOMPARE(tracker.GetPhase(), Tracker::P_IDLE);
	QCOMPARE(tracker.GetDeadline(), qint64(1500));

	QCOMPARE(tracker.GetAction(1000), Tracker::A_NONE);
	QCOMPARE(tracker.GetAction(1499), Tracker::A_NONE);
	QCOMPARE(tracker.GetAction(1500), Tracker::A_CLOSE_IDLE);
	QCOMPARE(tracker.GetAction(100000), Tracker::A_CLOSE_IDLE);
}


void CConnectionTimeoutTrackerTest::IdleTimeoutIsCountedFromOpeningTest()
{
	Tracker tracker(CreateTestConfig());
	tracker.OnConnectionOpened(5000);

	QCOMPARE(tracker.GetDeadline(), qint64(5500));
}


void CConnectionTimeoutTrackerTest::FirstDataStopsKeepAliveTimeoutTest()
{
	Tracker tracker(CreateTestConfig());
	tracker.OnConnectionOpened(0);

	tracker.OnDataReceived(400);

	QCOMPARE(tracker.GetPhase(), Tracker::P_RECEIVING);

	// The keep-alive timeout does not close a connection with a request that is being received:
	QCOMPARE(tracker.GetAction(600), Tracker::A_NONE);
	QCOMPARE(tracker.GetDeadline(), qint64(1400));
}


void CConnectionTimeoutTrackerTest::ReceivingInactivityTimeoutTest()
{
	Tracker tracker(CreateTestConfig());
	tracker.OnConnectionOpened(0);

	tracker.OnDataReceived(100);

	QCOMPARE(tracker.GetDeadline(), qint64(1100));
	QCOMPARE(tracker.GetAction(1099), Tracker::A_NONE);
	QCOMPARE(tracker.GetAction(1100), Tracker::A_REQUEST_TIMEOUT);
	QCOMPARE(tracker.GetAction(5000), Tracker::A_REQUEST_TIMEOUT);
}


void CConnectionTimeoutTrackerTest::ActiveReceivingIsNotTimedOutTest()
{
	Tracker tracker(CreateTestConfig());
	tracker.OnConnectionOpened(0);

	// A slow client is not closed while the data are coming:
	tracker.OnDataReceived(400);
	tracker.OnDataReceived(1200);
	tracker.OnDataReceived(2000);

	QCOMPARE(tracker.GetDeadline(), qint64(3000));
	QCOMPARE(tracker.GetAction(2999), Tracker::A_NONE);
	QCOMPARE(tracker.GetAction(3000), Tracker::A_REQUEST_TIMEOUT);
}


void CConnectionTimeoutTrackerTest::ProcessingHasNoTimeoutTest()
{
	Tracker tracker(CreateTestConfig());
	tracker.OnConnectionOpened(0);

	tracker.OnDataReceived(100);
	tracker.OnRequestCompleted(100);

	// The time of the request handler is not limited by the connection:
	QCOMPARE(tracker.GetPhase(), Tracker::P_PROCESSING);
	QCOMPARE(tracker.GetDeadline(), s_never);
	QCOMPARE(tracker.GetAction(1000000000), Tracker::A_NONE);
}


void CConnectionTimeoutTrackerTest::SendingStallTimeoutTest()
{
	Tracker tracker(CreateTestConfig());
	tracker.OnConnectionOpened(0);
	tracker.OnDataReceived(100);
	tracker.OnRequestCompleted(100);

	tracker.OnResponseStarted(5000);

	QCOMPARE(tracker.GetPhase(), Tracker::P_SENDING);
	QCOMPARE(tracker.GetDeadline(), qint64(6000));
	QCOMPARE(tracker.GetAction(5999), Tracker::A_NONE);
	QCOMPARE(tracker.GetAction(6000), Tracker::A_ABORT_STALLED_RESPONSE);
}


void CConnectionTimeoutTrackerTest::SendingProgressDelaysTimeoutTest()
{
	Tracker tracker(CreateTestConfig());
	tracker.OnConnectionOpened(0);
	tracker.OnDataReceived(100);
	tracker.OnRequestCompleted(100);
	tracker.OnResponseStarted(5000);

	// A client that reads the response slowly is not aborted while the data are going:
	tracker.OnBytesWritten(5500);
	tracker.OnBytesWritten(6200);

	QCOMPARE(tracker.GetDeadline(), qint64(7200));
	QCOMPARE(tracker.GetAction(7199), Tracker::A_NONE);
	QCOMPARE(tracker.GetAction(7200), Tracker::A_ABORT_STALLED_RESPONSE);
}


void CConnectionTimeoutTrackerTest::ResponseCompletedStartsKeepAliveTest()
{
	Tracker tracker(CreateTestConfig());
	tracker.OnConnectionOpened(0);
	tracker.OnDataReceived(100);
	tracker.OnRequestCompleted(100);
	tracker.OnResponseStarted(5000);

	tracker.OnResponseCompleted(5200);

	QCOMPARE(tracker.GetPhase(), Tracker::P_IDLE);
	QCOMPARE(tracker.GetDeadline(), qint64(5700));
	QCOMPARE(tracker.GetAction(5699), Tracker::A_NONE);
	QCOMPARE(tracker.GetAction(5700), Tracker::A_CLOSE_IDLE);
}


void CConnectionTimeoutTrackerTest::KeepAliveConnectionReuseTest()
{
	Tracker tracker(CreateTestConfig());
	tracker.OnConnectionOpened(0);

	// Each request resets the timeouts, the lifetime of the connection is not limited:
	qint64 time = 0;
	for (int i = 0; i < 5; ++i){
		time += 400;

		QCOMPARE(tracker.GetAction(time), Tracker::A_NONE);

		tracker.OnDataReceived(time);
		tracker.OnRequestCompleted(time);
		tracker.OnResponseStarted(time + 10);
		tracker.OnResponseCompleted(time + 30);

		QCOMPARE(tracker.GetPhase(), Tracker::P_IDLE);
		QCOMPARE(tracker.GetDeadline(), time + 30 + 500);
	}
}


void CConnectionTimeoutTrackerTest::DisabledTimeoutsTest_data()
{
	QTest::addColumn<int>("value");

	QTest::newRow("zero") << 0;
	QTest::newRow("negative") << -5;
}


void CConnectionTimeoutTrackerTest::DisabledTimeoutsTest()
{
	QFETCH(int, value);

	Tracker tracker(CreateConfig(value, value, 200));
	tracker.OnConnectionOpened(0);

	QCOMPARE(tracker.GetDeadline(), s_never);
	QCOMPARE(tracker.GetAction(1000000000), Tracker::A_NONE);

	tracker.OnDataReceived(10);
	QCOMPARE(tracker.GetPhase(), Tracker::P_RECEIVING);
	QCOMPARE(tracker.GetDeadline(), s_never);
	QCOMPARE(tracker.GetAction(1000000000), Tracker::A_NONE);

	tracker.OnRequestCompleted(20);
	tracker.OnResponseStarted(30);
	QCOMPARE(tracker.GetPhase(), Tracker::P_SENDING);
	QCOMPARE(tracker.GetDeadline(), s_never);
	QCOMPARE(tracker.GetAction(1000000000), Tracker::A_NONE);

	tracker.OnResponseCompleted(40);
	QCOMPARE(tracker.GetPhase(), Tracker::P_IDLE);
	QCOMPARE(tracker.GetDeadline(), s_never);

	// The lingering close does not depend on the other timeouts:
	tracker.OnClosing(50);
	QCOMPARE(tracker.GetDeadline(), qint64(250));
}


void CConnectionTimeoutTrackerTest::DisabledKeepAliveDoesNotAffectOthersTest()
{
	Tracker tracker(CreateConfig(1000, 0, 200));
	tracker.OnConnectionOpened(0);

	QCOMPARE(tracker.GetDeadline(), s_never);

	tracker.OnDataReceived(100);
	QCOMPARE(tracker.GetDeadline(), qint64(1100));
}


void CConnectionTimeoutTrackerTest::DataDuringProcessingDoesNotChangeDeadlinesTest()
{
	Tracker tracker(CreateTestConfig());
	tracker.OnConnectionOpened(0);
	tracker.OnDataReceived(0);
	tracker.OnRequestCompleted(0);

	// The beginning of the next request comes while the first one is being processed:
	tracker.OnDataReceived(300);

	QCOMPARE(tracker.GetPhase(), Tracker::P_PROCESSING);
	QCOMPARE(tracker.GetDeadline(), s_never);

	tracker.OnResponseStarted(1000);
	QCOMPARE(tracker.GetDeadline(), qint64(2000));

	// The data of the next request do not postpone the abort of the response that is not read:
	tracker.OnDataReceived(1500);
	QCOMPARE(tracker.GetDeadline(), qint64(2000));

	// The next request is counted from the end of the response:
	tracker.OnResponseCompleted(1600);
	QCOMPARE(tracker.GetPhase(), Tracker::P_RECEIVING);
	QCOMPARE(tracker.GetDeadline(), qint64(2600));
	QCOMPARE(tracker.GetAction(2600), Tracker::A_REQUEST_TIMEOUT);
}


void CConnectionTimeoutTrackerTest::PipelinedRequestsTest()
{
	Tracker tracker(CreateTestConfig());
	tracker.OnConnectionOpened(0);

	tracker.OnDataReceived(10);
	tracker.OnRequestCompleted(10);
	tracker.OnDataReceived(20);
	tracker.OnRequestCompleted(20);

	QCOMPARE(tracker.GetPhase(), Tracker::P_PROCESSING);

	tracker.OnResponseStarted(100);
	tracker.OnResponseCompleted(200);

	// The second request is still processed, the connection is not idle:
	QCOMPARE(tracker.GetPhase(), Tracker::P_PROCESSING);
	QCOMPARE(tracker.GetDeadline(), s_never);

	tracker.OnResponseStarted(300);
	tracker.OnResponseCompleted(400);

	QCOMPARE(tracker.GetPhase(), Tracker::P_IDLE);
	QCOMPARE(tracker.GetDeadline(), qint64(900));
}


void CConnectionTimeoutTrackerTest::RequestCompletedDuringSendingTest()
{
	Tracker tracker(CreateTestConfig());
	tracker.OnConnectionOpened(0);
	tracker.OnDataReceived(0);
	tracker.OnRequestCompleted(0);
	tracker.OnResponseStarted(100);

	tracker.OnDataReceived(150);
	tracker.OnRequestCompleted(150);

	// The phase of the response is kept:
	QCOMPARE(tracker.GetPhase(), Tracker::P_SENDING);
	QCOMPARE(tracker.GetDeadline(), qint64(1100));

	tracker.OnResponseCompleted(200);

	QCOMPARE(tracker.GetPhase(), Tracker::P_PROCESSING);
	QCOMPARE(tracker.GetDeadline(), s_never);
}


void CConnectionTimeoutTrackerTest::MessageWithoutRequestTest()
{
	Tracker tracker(CreateTestConfig());
	tracker.OnConnectionOpened(0);

	// A message of the server that is not a response to a request:
	tracker.OnResponseStarted(100);

	QCOMPARE(tracker.GetPhase(), Tracker::P_SENDING);
	QCOMPARE(tracker.GetDeadline(), qint64(1100));

	tracker.OnResponseCompleted(200);

	QCOMPARE(tracker.GetPhase(), Tracker::P_IDLE);
	QCOMPARE(tracker.GetDeadline(), qint64(700));

	// The count of the requests that are not answered does not become negative:
	tracker.OnDataReceived(300);
	tracker.OnRequestCompleted(300);
	tracker.OnResponseStarted(400);
	tracker.OnResponseCompleted(500);

	QCOMPARE(tracker.GetPhase(), Tracker::P_IDLE);
}


void CConnectionTimeoutTrackerTest::ResponseWhileReceivingTest()
{
	Tracker tracker(CreateTestConfig());
	tracker.OnConnectionOpened(0);

	// The server answers before the request is completely received, for example, it rejects the request:
	tracker.OnDataReceived(0);
	tracker.OnResponseStarted(100);

	QCOMPARE(tracker.GetPhase(), Tracker::P_SENDING);

	tracker.OnResponseCompleted(200);

	// The rest of the request is still expected:
	QCOMPARE(tracker.GetPhase(), Tracker::P_RECEIVING);
	QCOMPARE(tracker.GetDeadline(), qint64(1200));
}


void CConnectionTimeoutTrackerTest::ClosingTest()
{
	Tracker tracker(CreateTestConfig());
	tracker.OnConnectionOpened(0);
	tracker.OnDataReceived(0);

	tracker.OnClosing(1000);

	QCOMPARE(tracker.GetPhase(), Tracker::P_CLOSING);
	QCOMPARE(tracker.GetDeadline(), qint64(1200));
	QCOMPARE(tracker.GetAction(1199), Tracker::A_NONE);
	QCOMPARE(tracker.GetAction(1200), Tracker::A_ABORT_LINGERING);
}


void CConnectionTimeoutTrackerTest::ClosingIgnoresEventsTest()
{
	Tracker tracker(CreateTestConfig());
	tracker.OnConnectionOpened(0);
	tracker.OnDataReceived(0);
	tracker.OnClosing(1000);

	// The data that the client sends to the closing connection do not postpone the closing:
	tracker.OnDataReceived(1100);
	tracker.OnRequestCompleted(1100);
	tracker.OnResponseStarted(1100);
	tracker.OnBytesWritten(1100);
	tracker.OnResponseCompleted(1100);

	QCOMPARE(tracker.GetPhase(), Tracker::P_CLOSING);
	QCOMPARE(tracker.GetDeadline(), qint64(1200));
}


void CConnectionTimeoutTrackerTest::ImmediateClosingTest_data()
{
	QTest::addColumn<int>("lingeringCloseMs");

	QTest::newRow("zero") << 0;
	QTest::newRow("negative") << -1;
}


void CConnectionTimeoutTrackerTest::ImmediateClosingTest()
{
	QFETCH(int, lingeringCloseMs);

	Tracker tracker(CreateConfig(1000, 500, lingeringCloseMs));
	tracker.OnConnectionOpened(0);
	tracker.OnDataReceived(0);
	tracker.OnClosing(1000);

	QCOMPARE(tracker.GetDeadline(), qint64(1000));
	QCOMPARE(tracker.GetAction(1000), Tracker::A_ABORT_LINGERING);
}


void CConnectionTimeoutTrackerTest::UnexpectedEventsAreIgnoredTest()
{
	Tracker tracker(CreateTestConfig());
	tracker.OnConnectionOpened(0);

	// There is no response in progress:
	tracker.OnResponseCompleted(100);
	tracker.OnBytesWritten(100);

	QCOMPARE(tracker.GetPhase(), Tracker::P_IDLE);
	QCOMPARE(tracker.GetDeadline(), qint64(500));

	// A request can be completed without the report of its data:
	tracker.OnRequestCompleted(200);

	QCOMPARE(tracker.GetPhase(), Tracker::P_PROCESSING);
	QCOMPARE(tracker.GetDeadline(), s_never);
}


void CConnectionTimeoutTrackerTest::ReopeningResetsTest()
{
	Tracker tracker(CreateTestConfig());
	tracker.OnConnectionOpened(0);
	tracker.OnDataReceived(0);
	tracker.OnRequestCompleted(0);
	tracker.OnDataReceived(10);

	tracker.OnConnectionOpened(7000);

	QCOMPARE(tracker.GetPhase(), Tracker::P_IDLE);
	QCOMPARE(tracker.GetDeadline(), qint64(7500));

	// Nothing is left from the previous connection:
	tracker.OnDataReceived(7100);
	tracker.OnRequestCompleted(7100);
	tracker.OnResponseStarted(7200);
	tracker.OnResponseCompleted(7300);

	QCOMPARE(tracker.GetPhase(), Tracker::P_IDLE);
	QCOMPARE(tracker.GetDeadline(), qint64(7800));
}


void CConnectionTimeoutTrackerTest::LargeTimeTest()
{
	const qint64 start = 1000000000000;

	Tracker tracker(CreateTestConfig());
	tracker.OnConnectionOpened(start);

	QCOMPARE(tracker.GetDeadline(), start + 500);
	QCOMPARE(tracker.GetAction(start + 499), Tracker::A_NONE);
	QCOMPARE(tracker.GetAction(start + 500), Tracker::A_CLOSE_IDLE);
}


I_ADD_TEST(CConnectionTimeoutTrackerTest);


