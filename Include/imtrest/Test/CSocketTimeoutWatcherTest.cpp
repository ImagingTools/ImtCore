// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include "CSocketTimeoutWatcherTest.h"


// Qt includes
#include <QtNetwork/QTcpServer>
#include <QtNetwork/QTcpSocket>
#include <QtTest/QSignalSpy>

// ImtCore includes
#include <imtrest/CConnectionTimeoutBehavior.h>
#include <imtrest/CSocketTimeoutWatcher.h>
#include <imtrest/ProtocolTypeIds.h>


namespace
{


typedef imtrest::CConnectionTimeoutTracker Tracker;


const QByteArray s_response("HTTP/1.1 200 OK\r\nContent-Length: 0\r\n\r\n");


/**
	Connected pair of sockets: the socket of a server and the socket of a client.
*/
class CConnectedSockets
{
public:
	bool Open()
	{
		if (!m_server.listen(QHostAddress::LocalHost, 0)){
			return false;
		}

		m_client.connectToHost(QHostAddress::LocalHost, m_server.serverPort());

		if (!m_server.waitForNewConnection(5000)){
			return false;
		}

		m_serverSocketPtr = m_server.nextPendingConnection();
		if (m_serverSocketPtr == nullptr){
			return false;
		}

		return (m_client.state() == QAbstractSocket::ConnectedState) || m_client.waitForConnected(5000);
	}

	QTcpSocket& GetClient()
	{
		return m_client;
	}

	QTcpSocket& GetServerSocket()
	{
		return *m_serverSocketPtr;
	}

private:
	QTcpServer m_server;
	QTcpSocket m_client;
	QTcpSocket* m_serverSocketPtr = nullptr;
};


imtrest::CConnectionTimeoutConfig CreateConfig(int requestInactivityMs, int keepAliveMs, int lingeringCloseMs)
{
	imtrest::CConnectionTimeoutConfig config;
	config.requestInactivityMs = requestInactivityMs;
	config.keepAliveMs = keepAliveMs;
	config.lingeringCloseMs = lingeringCloseMs;

	return config;
}


imtrest::CConnectionTimeoutBehavior CreateHttpBehavior()
{
	return imtrest::CConnectionTimeoutBehavior::CreateForProtocol(imtrest::ProtocolTypeIds::s_http);
}


imtrest::CConnectionTimeoutBehavior CreateTcpBehavior()
{
	return imtrest::CConnectionTimeoutBehavior::CreateForProtocol(imtrest::ProtocolTypeIds::s_tcp);
}


int GetAction(const QSignalSpy& spy, int index)
{
	return spy.at(index).at(0).toInt();
}


} // namespace


// private slots

void CSocketTimeoutWatcherTest::IdleConnectionIsClosedTest()
{
	CConnectedSockets sockets;
	QVERIFY(sockets.Open());

	imtrest::CSocketTimeoutWatcher watcher(sockets.GetServerSocket(), CreateConfig(5000, 150, 500), CreateHttpBehavior());
	QSignalSpy timedOutSpy(&watcher, &imtrest::CSocketTimeoutWatcher::TimedOut);

	QTRY_COMPARE_WITH_TIMEOUT(sockets.GetClient().state(), QAbstractSocket::UnconnectedState, 3000);

	QCOMPARE(timedOutSpy.count(), 1);
	QCOMPARE(GetAction(timedOutSpy, 0), int(Tracker::A_CLOSE_IDLE));
	QVERIFY(watcher.IsClosing());

	// An idle connection is closed without a response:
	QVERIFY(sockets.GetClient().readAll().isEmpty());
}


void CSocketTimeoutWatcherTest::IdleConnectionIsNotClosedEarlyTest()
{
	CConnectedSockets sockets;
	QVERIFY(sockets.Open());

	imtrest::CSocketTimeoutWatcher watcher(sockets.GetServerSocket(), CreateConfig(5000, 2000, 500), CreateHttpBehavior());
	QSignalSpy timedOutSpy(&watcher, &imtrest::CSocketTimeoutWatcher::TimedOut);

	QTest::qWait(200);

	QCOMPARE(sockets.GetClient().state(), QAbstractSocket::ConnectedState);
	QCOMPARE(timedOutSpy.count(), 0);
	QVERIFY(!watcher.IsClosing());
	QCOMPARE(watcher.GetPhase(), Tracker::P_IDLE);
}


void CSocketTimeoutWatcherTest::SlowClientIsNotTimedOutTest()
{
	CConnectedSockets sockets;
	QVERIFY(sockets.Open());

	imtrest::CSocketTimeoutWatcher watcher(sockets.GetServerSocket(), CreateConfig(400, 5000, 500), CreateHttpBehavior());
	QSignalSpy timedOutSpy(&watcher, &imtrest::CSocketTimeoutWatcher::TimedOut);

	// The request comes slowly, but the data do not stop for longer than the timeout:
	for (int i = 0; i < 6; ++i){
		sockets.GetClient().write("G");
		watcher.OnDataReceived();

		QTest::qWait(150);
	}

	QCOMPARE(watcher.GetPhase(), Tracker::P_RECEIVING);
	QCOMPARE(timedOutSpy.count(), 0);
	QCOMPARE(sockets.GetClient().state(), QAbstractSocket::ConnectedState);
}


void CSocketTimeoutWatcherTest::IncompleteRequestGetsRequestTimeoutTest()
{
	CConnectedSockets sockets;
	QVERIFY(sockets.Open());

	imtrest::CSocketTimeoutWatcher watcher(sockets.GetServerSocket(), CreateConfig(200, 5000, 5000), CreateHttpBehavior());
	QSignalSpy timedOutSpy(&watcher, &imtrest::CSocketTimeoutWatcher::TimedOut);

	sockets.GetClient().write("GET / HT");
	watcher.OnDataReceived();

	QTRY_COMPARE_WITH_TIMEOUT(timedOutSpy.count(), 1, 3000);
	QCOMPARE(GetAction(timedOutSpy, 0), int(Tracker::A_REQUEST_TIMEOUT));

	QTRY_VERIFY_WITH_TIMEOUT(sockets.GetClient().bytesAvailable() > 0, 3000);
	const QByteArray response = sockets.GetClient().readAll();

	QVERIFY(response.startsWith("HTTP/1.1 408 Request Timeout\r\n"));
	QVERIFY(response.contains("\r\nDate: "));
	QVERIFY(response.contains("\r\nConnection: close\r\n"));
	QVERIFY(response.contains("\r\nContent-Length: 0\r\n"));
	QVERIFY(response.endsWith("\r\n\r\n"));

	// The server does not close the connection at once, it waits for the client (RFC 9112, section 9.6):
	QCOMPARE(sockets.GetClient().state(), QAbstractSocket::ConnectedState);
	QVERIFY(watcher.IsClosing());
	QCOMPARE(watcher.GetPhase(), Tracker::P_CLOSING);

	sockets.GetClient().disconnectFromHost();

	QTRY_COMPARE_WITH_TIMEOUT(sockets.GetServerSocket().state(), QAbstractSocket::UnconnectedState, 3000);
	QCOMPARE(timedOutSpy.count(), 1);
}


void CSocketTimeoutWatcherTest::LingeringCloseAbortsConnectionTest()
{
	CConnectedSockets sockets;
	QVERIFY(sockets.Open());

	imtrest::CSocketTimeoutWatcher watcher(sockets.GetServerSocket(), CreateConfig(150, 5000, 300), CreateHttpBehavior());
	QSignalSpy timedOutSpy(&watcher, &imtrest::CSocketTimeoutWatcher::TimedOut);

	sockets.GetClient().write("GET / HT");
	watcher.OnDataReceived();

	// The client does not close the connection after the response:
	QTRY_COMPARE_WITH_TIMEOUT(timedOutSpy.count(), 2, 3000);

	QCOMPARE(GetAction(timedOutSpy, 0), int(Tracker::A_REQUEST_TIMEOUT));
	QCOMPARE(GetAction(timedOutSpy, 1), int(Tracker::A_ABORT_LINGERING));

	QTRY_COMPARE_WITH_TIMEOUT(sockets.GetClient().state(), QAbstractSocket::UnconnectedState, 3000);
}


void CSocketTimeoutWatcherTest::IncompleteRequestOfOtherProtocolClosesConnectionTest()
{
	CConnectedSockets sockets;
	QVERIFY(sockets.Open());

	imtrest::CSocketTimeoutWatcher watcher(sockets.GetServerSocket(), CreateConfig(150, 5000, 5000), CreateTcpBehavior());
	QSignalSpy timedOutSpy(&watcher, &imtrest::CSocketTimeoutWatcher::TimedOut);

	sockets.GetClient().write("part");
	watcher.OnDataReceived();

	QTRY_COMPARE_WITH_TIMEOUT(timedOutSpy.count(), 1, 3000);
	QCOMPARE(GetAction(timedOutSpy, 0), int(Tracker::A_REQUEST_TIMEOUT));

	// The behavior of the protocol has no message, so the connection is just closed:
	QTRY_COMPARE_WITH_TIMEOUT(sockets.GetClient().state(), QAbstractSocket::UnconnectedState, 3000);
	QVERIFY(sockets.GetClient().readAll().isEmpty());
}


void CSocketTimeoutWatcherTest::CustomBehaviorSendsItsOwnMessageTest()
{
	CConnectedSockets sockets;
	QVERIFY(sockets.Open());

	// A protocol that is unknown to the transport classes describes its own message:
	imtrest::CConnectionTimeoutBehavior behavior;
	behavior.createRequestTimeoutMessage = [](){
		return QByteArray("421 Timeout\r\n");
	};
	behavior.isLimitedBeforeFirstMessageOnly = false;

	imtrest::CSocketTimeoutWatcher watcher(sockets.GetServerSocket(), CreateConfig(150, 5000, 5000), behavior);
	QSignalSpy timedOutSpy(&watcher, &imtrest::CSocketTimeoutWatcher::TimedOut);

	sockets.GetClient().write("part");
	watcher.OnDataReceived();

	QTRY_COMPARE_WITH_TIMEOUT(timedOutSpy.count(), 1, 3000);
	QCOMPARE(GetAction(timedOutSpy, 0), int(Tracker::A_REQUEST_TIMEOUT));

	QTRY_VERIFY_WITH_TIMEOUT(sockets.GetClient().bytesAvailable() >= 13, 3000);
	QCOMPARE(sockets.GetClient().readAll(), QByteArray("421 Timeout\r\n"));

	// As for HTTP, the connection waits for the closing by the client:
	QCOMPARE(sockets.GetClient().state(), QAbstractSocket::ConnectedState);
	QCOMPARE(watcher.GetPhase(), Tracker::P_CLOSING);
}


void CSocketTimeoutWatcherTest::EmptyMessageAbortsConnectionTest()
{
	CConnectedSockets sockets;
	QVERIFY(sockets.Open());

	imtrest::CConnectionTimeoutBehavior behavior;
	behavior.createRequestTimeoutMessage = [](){
		return QByteArray();
	};

	imtrest::CSocketTimeoutWatcher watcher(sockets.GetServerSocket(), CreateConfig(150, 5000, 5000), behavior);
	QSignalSpy timedOutSpy(&watcher, &imtrest::CSocketTimeoutWatcher::TimedOut);

	sockets.GetClient().write("part");
	watcher.OnDataReceived();

	QTRY_COMPARE_WITH_TIMEOUT(timedOutSpy.count(), 1, 3000);
	QCOMPARE(GetAction(timedOutSpy, 0), int(Tracker::A_REQUEST_TIMEOUT));

	QTRY_COMPARE_WITH_TIMEOUT(sockets.GetClient().state(), QAbstractSocket::UnconnectedState, 3000);
	QVERIFY(sockets.GetClient().readAll().isEmpty());
}


void CSocketTimeoutWatcherTest::ProcessingIsNotTimedOutTest()
{
	CConnectedSockets sockets;
	QVERIFY(sockets.Open());

	imtrest::CSocketTimeoutWatcher watcher(sockets.GetServerSocket(), CreateConfig(150, 150, 300), CreateHttpBehavior());
	QSignalSpy timedOutSpy(&watcher, &imtrest::CSocketTimeoutWatcher::TimedOut);

	watcher.OnDataReceived();
	watcher.OnRequestCompleted();

	// The time of the request handler is not limited by the connection:
	QTest::qWait(500);

	QCOMPARE(watcher.GetPhase(), Tracker::P_PROCESSING);
	QCOMPARE(timedOutSpy.count(), 0);
	QCOMPARE(sockets.GetClient().state(), QAbstractSocket::ConnectedState);
}


void CSocketTimeoutWatcherTest::ResponseStartsKeepAliveTimeoutTest()
{
	CConnectedSockets sockets;
	QVERIFY(sockets.Open());

	imtrest::CSocketTimeoutWatcher watcher(sockets.GetServerSocket(), CreateConfig(2000, 300, 300), CreateHttpBehavior());
	QSignalSpy timedOutSpy(&watcher, &imtrest::CSocketTimeoutWatcher::TimedOut);

	watcher.OnDataReceived();
	watcher.OnRequestCompleted();
	watcher.WriteResponse(s_response);

	QTRY_VERIFY_WITH_TIMEOUT(sockets.GetClient().bytesAvailable() >= s_response.size(), 3000);
	QCOMPARE(sockets.GetClient().readAll(), s_response);

	QCOMPARE(watcher.GetPhase(), Tracker::P_IDLE);

	// The idle connection is closed after the response:
	QTRY_COMPARE_WITH_TIMEOUT(timedOutSpy.count(), 1, 3000);
	QCOMPARE(GetAction(timedOutSpy, 0), int(Tracker::A_CLOSE_IDLE));

	QTRY_COMPARE_WITH_TIMEOUT(sockets.GetClient().state(), QAbstractSocket::UnconnectedState, 3000);
}


void CSocketTimeoutWatcherTest::KeepAliveConnectionIsReusedTest()
{
	CConnectedSockets sockets;
	QVERIFY(sockets.Open());

	imtrest::CSocketTimeoutWatcher watcher(sockets.GetServerSocket(), CreateConfig(2000, 400, 300), CreateHttpBehavior());
	QSignalSpy timedOutSpy(&watcher, &imtrest::CSocketTimeoutWatcher::TimedOut);

	// The time between the requests is shorter than the keep-alive timeout, the whole time is longer:
	for (int i = 0; i < 4; ++i){
		QTest::qWait(150);

		watcher.OnDataReceived();
		watcher.OnRequestCompleted();
		watcher.WriteResponse(s_response);

		QTRY_VERIFY_WITH_TIMEOUT(sockets.GetClient().bytesAvailable() >= s_response.size(), 3000);
		QCOMPARE(sockets.GetClient().readAll(), s_response);
	}

	QCOMPARE(timedOutSpy.count(), 0);
	QCOMPARE(sockets.GetClient().state(), QAbstractSocket::ConnectedState);
	QCOMPARE(watcher.GetPhase(), Tracker::P_IDLE);
}


void CSocketTimeoutWatcherTest::StalledResponseIsAbortedTest()
{
	CConnectedSockets sockets;

	QVERIFY(sockets.Open());

	// The client does not read the response, so it cannot be sent:
	sockets.GetClient().setReadBufferSize(1024);

	imtrest::CSocketTimeoutWatcher watcher(sockets.GetServerSocket(), CreateConfig(300, 5000, 300), CreateHttpBehavior());
	QSignalSpy timedOutSpy(&watcher, &imtrest::CSocketTimeoutWatcher::TimedOut);

	watcher.OnDataReceived();
	watcher.OnRequestCompleted();
	watcher.WriteResponse(QByteArray(16 * 1024 * 1024, 'x'));

	QCOMPARE(watcher.GetPhase(), Tracker::P_SENDING);

	QTRY_COMPARE_WITH_TIMEOUT(timedOutSpy.count(), 1, 10000);
	QCOMPARE(GetAction(timedOutSpy, 0), int(Tracker::A_ABORT_STALLED_RESPONSE));
	QVERIFY(watcher.IsClosing());
}


void CSocketTimeoutWatcherTest::OtherProtocolIsClosedBeforeFirstMessageTest()
{
	CConnectedSockets sockets;
	QVERIFY(sockets.Open());

	imtrest::CSocketTimeoutWatcher watcher(sockets.GetServerSocket(), CreateConfig(5000, 150, 500), CreateTcpBehavior());
	QSignalSpy timedOutSpy(&watcher, &imtrest::CSocketTimeoutWatcher::TimedOut);

	QTRY_COMPARE_WITH_TIMEOUT(timedOutSpy.count(), 1, 3000);
	QCOMPARE(GetAction(timedOutSpy, 0), int(Tracker::A_CLOSE_IDLE));

	QTRY_COMPARE_WITH_TIMEOUT(sockets.GetClient().state(), QAbstractSocket::UnconnectedState, 3000);
}


void CSocketTimeoutWatcherTest::ChannelOfOtherProtocolIsNotLimitedTest()
{
	CConnectedSockets sockets;
	QVERIFY(sockets.Open());

	imtrest::CSocketTimeoutWatcher watcher(sockets.GetServerSocket(), CreateConfig(150, 200, 300), CreateTcpBehavior());
	QSignalSpy timedOutSpy(&watcher, &imtrest::CSocketTimeoutWatcher::TimedOut);

	// After the first message the persistent channel is not closed, with or without responses:
	watcher.OnDataReceived();
	watcher.OnRequestCompleted();

	QTest::qWait(500);

	watcher.WriteResponse(s_response);

	QTRY_VERIFY_WITH_TIMEOUT(sockets.GetClient().bytesAvailable() >= s_response.size(), 3000);

	QTest::qWait(500);

	QCOMPARE(timedOutSpy.count(), 0);
	QCOMPARE(sockets.GetClient().state(), QAbstractSocket::ConnectedState);
	QCOMPARE(watcher.GetPhase(), Tracker::P_IDLE);
}


void CSocketTimeoutWatcherTest::NoTimeoutAfterDisconnectionTest()
{
	CConnectedSockets sockets;
	QVERIFY(sockets.Open());

	imtrest::CSocketTimeoutWatcher watcher(sockets.GetServerSocket(), CreateConfig(5000, 200, 300), CreateHttpBehavior());
	QSignalSpy timedOutSpy(&watcher, &imtrest::CSocketTimeoutWatcher::TimedOut);

	sockets.GetClient().disconnectFromHost();

	QTRY_COMPARE_WITH_TIMEOUT(sockets.GetServerSocket().state(), QAbstractSocket::UnconnectedState, 3000);

	QTest::qWait(500);

	// The connection is closed by the client, the timer does not act on it:
	QCOMPARE(timedOutSpy.count(), 0);
	QVERIFY(watcher.IsClosing());
}


void CSocketTimeoutWatcherTest::PhasesFollowRequestLifecycleTest()
{
	CConnectedSockets sockets;
	QVERIFY(sockets.Open());

	imtrest::CSocketTimeoutWatcher watcher(sockets.GetServerSocket(), CreateConfig(5000, 5000, 5000), CreateHttpBehavior());

	QCOMPARE(watcher.GetPhase(), Tracker::P_IDLE);

	watcher.OnDataReceived();
	QCOMPARE(watcher.GetPhase(), Tracker::P_RECEIVING);

	watcher.OnRequestCompleted();
	QCOMPARE(watcher.GetPhase(), Tracker::P_PROCESSING);

	watcher.WriteResponse(s_response);

	// The phase is SENDING until the data are written to the network, then the connection is idle again:
	QTRY_COMPARE_WITH_TIMEOUT(watcher.GetPhase(), Tracker::P_IDLE, 3000);
	QTRY_VERIFY_WITH_TIMEOUT(sockets.GetClient().bytesAvailable() >= s_response.size(), 3000);
	QVERIFY(!watcher.IsClosing());
}


void CSocketTimeoutWatcherTest::PipelinedRequestsAreAnsweredOneByOneTest()
{
	CConnectedSockets sockets;
	QVERIFY(sockets.Open());

	imtrest::CSocketTimeoutWatcher watcher(sockets.GetServerSocket(), CreateConfig(5000, 150, 300), CreateHttpBehavior());
	QSignalSpy timedOutSpy(&watcher, &imtrest::CSocketTimeoutWatcher::TimedOut);

	// Two requests are received before the first response:
	watcher.OnDataReceived();
	watcher.OnRequestCompleted();
	watcher.OnDataReceived();
	watcher.OnRequestCompleted();

	watcher.WriteResponse(s_response);

	// The second request is still being processed, so the keep-alive timeout does not act:
	QTRY_COMPARE_WITH_TIMEOUT(watcher.GetPhase(), Tracker::P_PROCESSING, 3000);
	QTest::qWait(400);
	QCOMPARE(watcher.GetPhase(), Tracker::P_PROCESSING);
	QCOMPARE(timedOutSpy.count(), 0);

	watcher.WriteResponse(s_response);

	// After the last response the connection is idle and is closed by the keep-alive timeout:
	QTRY_COMPARE_WITH_TIMEOUT(timedOutSpy.count(), 1, 3000);
	QCOMPARE(GetAction(timedOutSpy, 0), int(Tracker::A_CLOSE_IDLE));

	QTRY_COMPARE_WITH_TIMEOUT(sockets.GetClient().state(), QAbstractSocket::UnconnectedState, 3000);
	QCOMPARE(sockets.GetClient().readAll(), s_response + s_response);
}


void CSocketTimeoutWatcherTest::IncompleteSecondRequestGetsRequestTimeoutTest()
{
	CConnectedSockets sockets;
	QVERIFY(sockets.Open());

	imtrest::CSocketTimeoutWatcher watcher(sockets.GetServerSocket(), CreateConfig(200, 5000, 5000), CreateHttpBehavior());
	QSignalSpy timedOutSpy(&watcher, &imtrest::CSocketTimeoutWatcher::TimedOut);

	// The first request on the connection is complete and answered:
	watcher.OnDataReceived();
	watcher.OnRequestCompleted();
	watcher.WriteResponse(s_response);

	QTRY_VERIFY_WITH_TIMEOUT(sockets.GetClient().bytesAvailable() >= s_response.size(), 3000);
	QCOMPARE(sockets.GetClient().readAll(), s_response);
	QTRY_COMPARE_WITH_TIMEOUT(watcher.GetPhase(), Tracker::P_IDLE, 3000);

	// The second request on the same connection is not completed:
	sockets.GetClient().write("GET / HT");
	watcher.OnDataReceived();

	QTRY_COMPARE_WITH_TIMEOUT(timedOutSpy.count(), 1, 3000);
	QCOMPARE(GetAction(timedOutSpy, 0), int(Tracker::A_REQUEST_TIMEOUT));

	QTRY_VERIFY_WITH_TIMEOUT(sockets.GetClient().bytesAvailable() > 0, 3000);
	QVERIFY(sockets.GetClient().readAll().startsWith("HTTP/1.1 408 Request Timeout\r\n"));
}


void CSocketTimeoutWatcherTest::PartialNextRequestDuringProcessingIsTimedOutAfterResponseTest()
{
	CConnectedSockets sockets;
	QVERIFY(sockets.Open());

	imtrest::CSocketTimeoutWatcher watcher(sockets.GetServerSocket(), CreateConfig(200, 5000, 5000), CreateHttpBehavior());
	QSignalSpy timedOutSpy(&watcher, &imtrest::CSocketTimeoutWatcher::TimedOut);

	watcher.OnDataReceived();
	watcher.OnRequestCompleted();

	// The client starts the next request while the handler works:
	watcher.OnDataReceived();

	QTest::qWait(500);
	QCOMPARE(watcher.GetPhase(), Tracker::P_PROCESSING);
	QCOMPARE(timedOutSpy.count(), 0);

	watcher.WriteResponse(s_response);

	// The next request is still incomplete after the response, its timeout is counted from the response:
	QTRY_COMPARE_WITH_TIMEOUT(timedOutSpy.count(), 1, 3000);
	QCOMPARE(GetAction(timedOutSpy, 0), int(Tracker::A_REQUEST_TIMEOUT));
}


void CSocketTimeoutWatcherTest::EventsAreIgnoredWhileClosingTest()
{
	CConnectedSockets sockets;
	QVERIFY(sockets.Open());

	imtrest::CSocketTimeoutWatcher watcher(sockets.GetServerSocket(), CreateConfig(150, 5000, 5000), CreateHttpBehavior());
	QSignalSpy timedOutSpy(&watcher, &imtrest::CSocketTimeoutWatcher::TimedOut);

	sockets.GetClient().write("GET / HT");
	watcher.OnDataReceived();

	QTRY_COMPARE_WITH_TIMEOUT(watcher.GetPhase(), Tracker::P_CLOSING, 3000);

	QTRY_VERIFY_WITH_TIMEOUT(sockets.GetClient().bytesAvailable() > 0, 3000);
	QVERIFY(sockets.GetClient().readAll().startsWith("HTTP/1.1 408 Request Timeout\r\n"));

	// The closing connection does not accept new events and does not send anything else:
	watcher.OnDataReceived();
	watcher.OnRequestCompleted();
	watcher.WriteResponse(s_response);

	QTest::qWait(200);

	QCOMPARE(watcher.GetPhase(), Tracker::P_CLOSING);
	QVERIFY(watcher.IsClosing());
	QCOMPARE(sockets.GetClient().bytesAvailable(), qint64(0));
	QCOMPARE(timedOutSpy.count(), 1);
}


void CSocketTimeoutWatcherTest::ZeroLingeringTimeoutAbortsRightAfterMessageTest()
{
	CConnectedSockets sockets;
	QVERIFY(sockets.Open());

	imtrest::CSocketTimeoutWatcher watcher(sockets.GetServerSocket(), CreateConfig(150, 5000, 0), CreateHttpBehavior());
	QSignalSpy timedOutSpy(&watcher, &imtrest::CSocketTimeoutWatcher::TimedOut);

	sockets.GetClient().write("GET / HT");
	watcher.OnDataReceived();

	QTRY_COMPARE_WITH_TIMEOUT(timedOutSpy.count(), 2, 3000);

	QCOMPARE(GetAction(timedOutSpy, 0), int(Tracker::A_REQUEST_TIMEOUT));
	QCOMPARE(GetAction(timedOutSpy, 1), int(Tracker::A_ABORT_LINGERING));

	QTRY_COMPARE_WITH_TIMEOUT(sockets.GetClient().state(), QAbstractSocket::UnconnectedState, 3000);
}


void CSocketTimeoutWatcherTest::DisabledTimeoutsNeverFireTest()
{
	CConnectedSockets sockets;
	QVERIFY(sockets.Open());

	// 0 disables the limits of the idle connection and of the receiving of a request:
	imtrest::CSocketTimeoutWatcher watcher(sockets.GetServerSocket(), CreateConfig(0, 0, 5000), CreateHttpBehavior());
	QSignalSpy timedOutSpy(&watcher, &imtrest::CSocketTimeoutWatcher::TimedOut);

	QTest::qWait(300);

	QCOMPARE(watcher.GetPhase(), Tracker::P_IDLE);
	QCOMPARE(timedOutSpy.count(), 0);

	sockets.GetClient().write("GET / HT");
	watcher.OnDataReceived();

	QTest::qWait(300);

	QCOMPARE(watcher.GetPhase(), Tracker::P_RECEIVING);
	QCOMPARE(timedOutSpy.count(), 0);
	QCOMPARE(sockets.GetClient().state(), QAbstractSocket::ConnectedState);
	QCOMPARE(sockets.GetClient().bytesAvailable(), qint64(0));
}


void CSocketTimeoutWatcherTest::MessageFactoryIsCalledOnlyOnTimeoutTest()
{
	CConnectedSockets sockets;
	QVERIFY(sockets.Open());

	int callCount = 0;

	imtrest::CConnectionTimeoutBehavior behavior;
	behavior.createRequestTimeoutMessage = [&callCount](){
		++callCount;

		return QByteArray("timeout");
	};

	imtrest::CSocketTimeoutWatcher watcher(sockets.GetServerSocket(), CreateConfig(400, 5000, 5000), behavior);
	QSignalSpy timedOutSpy(&watcher, &imtrest::CSocketTimeoutWatcher::TimedOut);

	sockets.GetClient().write("part");
	watcher.OnDataReceived();

	QTest::qWait(100);
	QCOMPARE(callCount, 0);

	QTRY_COMPARE_WITH_TIMEOUT(timedOutSpy.count(), 1, 3000);
	QCOMPARE(callCount, 1);

	// The message is created once per timeout:
	QTest::qWait(200);
	QCOMPARE(callCount, 1);
}


void CSocketTimeoutWatcherTest::PartialMessageAfterFirstOfOtherProtocolIsNotLimitedTest()
{
	CConnectedSockets sockets;
	QVERIFY(sockets.Open());

	imtrest::CSocketTimeoutWatcher watcher(sockets.GetServerSocket(), CreateConfig(150, 150, 300), CreateTcpBehavior());
	QSignalSpy timedOutSpy(&watcher, &imtrest::CSocketTimeoutWatcher::TimedOut);

	watcher.OnDataReceived();
	watcher.OnRequestCompleted();

	// A persistent channel can be silent in the middle of a message after its first message:
	watcher.OnDataReceived();

	QTest::qWait(500);

	QCOMPARE(timedOutSpy.count(), 0);
	QCOMPARE(sockets.GetClient().state(), QAbstractSocket::ConnectedState);
}


void CSocketTimeoutWatcherTest::DeletedSocketIsHandledTest()
{
	CConnectedSockets sockets;
	QVERIFY(sockets.Open());

	imtrest::CSocketTimeoutWatcher watcher(sockets.GetServerSocket(), CreateConfig(5000, 150, 300), CreateHttpBehavior());

	// The socket is destroyed by its owner, the watcher should not use it:
	delete &sockets.GetServerSocket();

	watcher.WriteResponse(s_response);

	// The keep-alive timeout fires for the idle connection or the disconnection is noticed, in both cases the watcher stops:
	QTRY_VERIFY_WITH_TIMEOUT(watcher.IsClosing(), 3000);
}


void CSocketTimeoutWatcherTest::ConnectionIsClosedAfterResponseTest()
{
	CConnectedSockets sockets;
	QVERIFY(sockets.Open());

	imtrest::CSocketTimeoutWatcher watcher(sockets.GetServerSocket(), CreateConfig(5000, 5000, 5000), CreateHttpBehavior());
	QSignalSpy timedOutSpy(&watcher, &imtrest::CSocketTimeoutWatcher::TimedOut);

	// The request has "Connection: close" (RFC 9112, section 9.6):
	watcher.OnDataReceived();
	watcher.OnRequestCompleted();
	watcher.CloseAfterResponse();

	QVERIFY(watcher.IsCloseRequested());
	QVERIFY(!watcher.IsClosing());

	watcher.WriteResponse(s_response);

	QTRY_COMPARE_WITH_TIMEOUT(sockets.GetClient().state(), QAbstractSocket::UnconnectedState, 3000);

	// The whole response is delivered before the closing and it is not a timeout:
	QCOMPARE(sockets.GetClient().readAll(), s_response);
	QVERIFY(watcher.IsClosing());
	QCOMPARE(timedOutSpy.count(), 0);
}


void CSocketTimeoutWatcherTest::ConnectionIsNotClosedBeforeResponseTest()
{
	CConnectedSockets sockets;
	QVERIFY(sockets.Open());

	imtrest::CSocketTimeoutWatcher watcher(sockets.GetServerSocket(), CreateConfig(5000, 150, 5000), CreateHttpBehavior());
	QSignalSpy timedOutSpy(&watcher, &imtrest::CSocketTimeoutWatcher::TimedOut);

	watcher.OnDataReceived();
	watcher.OnRequestCompleted();
	watcher.CloseAfterResponse();

	// The handler works, the connection waits for its response:
	QTest::qWait(400);

	QCOMPARE(sockets.GetClient().state(), QAbstractSocket::ConnectedState);
	QCOMPARE(watcher.GetPhase(), Tracker::P_PROCESSING);
	QCOMPARE(timedOutSpy.count(), 0);

	watcher.WriteResponse(s_response);

	QTRY_COMPARE_WITH_TIMEOUT(sockets.GetClient().state(), QAbstractSocket::UnconnectedState, 3000);
	QCOMPARE(sockets.GetClient().readAll(), s_response);
}


void CSocketTimeoutWatcherTest::DataAfterCloseRequestIsIgnoredTest()
{
	CConnectedSockets sockets;
	QVERIFY(sockets.Open());

	imtrest::CSocketTimeoutWatcher watcher(sockets.GetServerSocket(), CreateConfig(150, 5000, 5000), CreateHttpBehavior());
	QSignalSpy timedOutSpy(&watcher, &imtrest::CSocketTimeoutWatcher::TimedOut);

	watcher.OnDataReceived();
	watcher.OnRequestCompleted();
	watcher.CloseAfterResponse();

	// The client should not send anything after such a request, the data are not a new request:
	watcher.OnDataReceived();
	watcher.OnRequestCompleted();

	QTest::qWait(400);

	QCOMPARE(watcher.GetPhase(), Tracker::P_PROCESSING);
	QCOMPARE(timedOutSpy.count(), 0);

	// The response is still written and the connection is closed after it:
	watcher.WriteResponse(s_response);

	QTRY_COMPARE_WITH_TIMEOUT(sockets.GetClient().state(), QAbstractSocket::UnconnectedState, 3000);
	QCOMPARE(sockets.GetClient().readAll(), s_response);
}


void CSocketTimeoutWatcherTest::ResponsesBeforeClosingRequestKeepConnectionTest()
{
	CConnectedSockets sockets;
	QVERIFY(sockets.Open());

	imtrest::CSocketTimeoutWatcher watcher(sockets.GetServerSocket(), CreateConfig(5000, 5000, 5000), CreateHttpBehavior());

	// Two requests are pipelined, the second one has "Connection: close":
	watcher.OnDataReceived();
	watcher.OnRequestCompleted();
	watcher.OnDataReceived();
	watcher.OnRequestCompleted();
	watcher.CloseAfterResponse();

	watcher.WriteResponse(s_response);

	// The response to the first request does not close the connection, the second request is not answered yet:
	QTRY_COMPARE_WITH_TIMEOUT(watcher.GetPhase(), Tracker::P_PROCESSING, 3000);
	QTest::qWait(200);
	QCOMPARE(sockets.GetClient().state(), QAbstractSocket::ConnectedState);

	watcher.WriteResponse(s_response);

	QTRY_COMPARE_WITH_TIMEOUT(sockets.GetClient().state(), QAbstractSocket::UnconnectedState, 3000);
	QCOMPARE(sockets.GetClient().readAll(), s_response + s_response);
}


void CSocketTimeoutWatcherTest::LargeResponseIsDeliveredBeforeClosingTest()
{
	CConnectedSockets sockets;
	QVERIFY(sockets.Open());

	imtrest::CSocketTimeoutWatcher watcher(sockets.GetServerSocket(), CreateConfig(5000, 5000, 5000), CreateHttpBehavior());

	const QByteArray response(4 * 1024 * 1024, 'x');

	watcher.OnDataReceived();
	watcher.OnRequestCompleted();
	watcher.CloseAfterResponse();
	watcher.WriteResponse(response);

	// A big response is sent in parts, the connection is closed after the last of them and not at the first one:
	QByteArray received;
	QTRY_VERIFY_WITH_TIMEOUT((received += sockets.GetClient().readAll()).size() >= response.size(), 10000);

	QCOMPARE(received.size(), response.size());

	QTRY_COMPARE_WITH_TIMEOUT(sockets.GetClient().state(), QAbstractSocket::UnconnectedState, 3000);
}


I_ADD_TEST(CSocketTimeoutWatcherTest);


