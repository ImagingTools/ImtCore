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
const QByteArray s_interimMessage("HTTP/1.1 100 Continue\r\n\r\n");
const QByteArray s_refusalMessage("HTTP/1.1 413 Payload Too Large\r\nConnection: close\r\nContent-Length: 0\r\n\r\n");


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


imtrest::CConnectionTimeoutConfig CreateProcessingConfig(int requestProcessingMs, int lingeringCloseMs = 5000)
{
	imtrest::CConnectionTimeoutConfig config = CreateConfig(5000, 5000, lingeringCloseMs);
	config.requestProcessingMs = requestProcessingMs;

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


void CSocketTimeoutWatcherTest::RejectedRequestGetsBadRequestTest()
{
	CConnectedSockets sockets;
	QVERIFY(sockets.Open());

	imtrest::CSocketTimeoutWatcher watcher(sockets.GetServerSocket(), CreateConfig(5000, 5000, 5000), CreateHttpBehavior());
	QSignalSpy timedOutSpy(&watcher, &imtrest::CSocketTimeoutWatcher::TimedOut);

	// The data of the client are not a request (RFC 9112, section 2.2):
	watcher.OnDataReceived();
	watcher.RejectRequest();

	QVERIFY(watcher.IsClosing());
	QCOMPARE(watcher.GetPhase(), Tracker::P_CLOSING);

	QTRY_VERIFY_WITH_TIMEOUT(sockets.GetClient().bytesAvailable() > 0, 3000);
	const QByteArray response = sockets.GetClient().readAll();

	QVERIFY(response.startsWith("HTTP/1.1 400 Bad Request\r\n"));
	QVERIFY(response.contains("\r\nDate: "));
	QVERIFY(response.contains("\r\nConnection: close\r\n"));
	QVERIFY(response.contains("\r\nContent-Length: 0\r\n"));
	QVERIFY(response.endsWith("\r\n\r\n"));

	// The server waits for the closing by the client, as after 408 (RFC 9112, section 9.6):
	QTest::qWait(100);
	QCOMPARE(sockets.GetClient().state(), QAbstractSocket::ConnectedState);

	sockets.GetClient().disconnectFromHost();

	QTRY_COMPARE_WITH_TIMEOUT(sockets.GetServerSocket().state(), QAbstractSocket::UnconnectedState, 3000);
	QCOMPARE(timedOutSpy.count(), 0);
}


void CSocketTimeoutWatcherTest::RejectedConnectionIsAbortedAfterLingeringTest()
{
	CConnectedSockets sockets;
	QVERIFY(sockets.Open());

	imtrest::CSocketTimeoutWatcher watcher(sockets.GetServerSocket(), CreateConfig(5000, 5000, 300), CreateHttpBehavior());
	QSignalSpy timedOutSpy(&watcher, &imtrest::CSocketTimeoutWatcher::TimedOut);

	watcher.OnDataReceived();
	watcher.RejectRequest();

	// The client does not close the connection after the response:
	QTRY_COMPARE_WITH_TIMEOUT(timedOutSpy.count(), 1, 3000);
	QCOMPARE(GetAction(timedOutSpy, 0), int(Tracker::A_ABORT_LINGERING));

	QTRY_COMPARE_WITH_TIMEOUT(sockets.GetClient().state(), QAbstractSocket::UnconnectedState, 3000);
}


void CSocketTimeoutWatcherTest::RejectedRequestOfOtherProtocolIsAbortedTest()
{
	CConnectedSockets sockets;
	QVERIFY(sockets.Open());

	imtrest::CSocketTimeoutWatcher watcher(sockets.GetServerSocket(), CreateConfig(5000, 5000, 5000), CreateTcpBehavior());

	watcher.OnDataReceived();
	watcher.RejectRequest();

	// The behavior of the protocol has no message, so the connection is just closed:
	QVERIFY(watcher.IsClosing());
	QTRY_COMPARE_WITH_TIMEOUT(sockets.GetClient().state(), QAbstractSocket::UnconnectedState, 3000);
	QVERIFY(sockets.GetClient().readAll().isEmpty());
}


void CSocketTimeoutWatcherTest::RejectedRequestGetsMessageOfBehaviorTest()
{
	CConnectedSockets sockets;
	QVERIFY(sockets.Open());

	// A protocol that is unknown to the transport classes describes its own message:
	imtrest::CConnectionTimeoutBehavior behavior;
	behavior.createBadRequestMessage = [](){
		return QByteArray("500 Syntax error\r\n");
	};

	imtrest::CSocketTimeoutWatcher watcher(sockets.GetServerSocket(), CreateConfig(5000, 5000, 5000), behavior);

	watcher.OnDataReceived();
	watcher.RejectRequest();

	QTRY_VERIFY_WITH_TIMEOUT(sockets.GetClient().bytesAvailable() >= 18, 3000);
	QCOMPARE(sockets.GetClient().readAll(), QByteArray("500 Syntax error\r\n"));
	QCOMPARE(watcher.GetPhase(), Tracker::P_CLOSING);
}


void CSocketTimeoutWatcherTest::RejectedConnectionIgnoresEventsTest()
{
	CConnectedSockets sockets;
	QVERIFY(sockets.Open());

	imtrest::CSocketTimeoutWatcher watcher(sockets.GetServerSocket(), CreateConfig(5000, 5000, 5000), CreateHttpBehavior());
	QSignalSpy timedOutSpy(&watcher, &imtrest::CSocketTimeoutWatcher::TimedOut);

	watcher.OnDataReceived();
	watcher.RejectRequest();

	QTRY_VERIFY_WITH_TIMEOUT(sockets.GetClient().bytesAvailable() > 0, 3000);
	QVERIFY(sockets.GetClient().readAll().startsWith("HTTP/1.1 400 "));

	// The rejected connection does not accept new events and does not send anything else:
	watcher.OnDataReceived();
	watcher.OnRequestCompleted();
	watcher.WriteResponse(s_response);

	QTest::qWait(200);

	QCOMPARE(watcher.GetPhase(), Tracker::P_CLOSING);
	QCOMPARE(sockets.GetClient().bytesAvailable(), qint64(0));
	QCOMPARE(timedOutSpy.count(), 0);
}


void CSocketTimeoutWatcherTest::RejectionIsSentOnceTest()
{
	CConnectedSockets sockets;
	QVERIFY(sockets.Open());

	imtrest::CSocketTimeoutWatcher watcher(sockets.GetServerSocket(), CreateConfig(5000, 5000, 5000), CreateHttpBehavior());

	watcher.OnDataReceived();
	watcher.RejectRequest();
	watcher.RejectRequest();

	QTRY_VERIFY_WITH_TIMEOUT(sockets.GetClient().bytesAvailable() > 0, 3000);
	QTest::qWait(200);

	// A second rejection does not send a second response:
	const QByteArray response = sockets.GetClient().readAll();

	QCOMPARE(response.count("HTTP/1.1 400 "), 1);
}


void CSocketTimeoutWatcherTest::RejectionAfterRequestTimeoutIsIgnoredTest()
{
	CConnectedSockets sockets;
	QVERIFY(sockets.Open());

	imtrest::CSocketTimeoutWatcher watcher(sockets.GetServerSocket(), CreateConfig(150, 5000, 5000), CreateHttpBehavior());
	QSignalSpy timedOutSpy(&watcher, &imtrest::CSocketTimeoutWatcher::TimedOut);

	sockets.GetClient().write("GET / HT");
	watcher.OnDataReceived();

	// The connection is already being closed with 408, the client gets no other response:
	QTRY_COMPARE_WITH_TIMEOUT(timedOutSpy.count(), 1, 3000);
	watcher.RejectRequest();

	QTRY_VERIFY_WITH_TIMEOUT(sockets.GetClient().bytesAvailable() > 0, 3000);
	QTest::qWait(200);

	const QByteArray response = sockets.GetClient().readAll();

	QVERIFY(response.startsWith("HTTP/1.1 408 "));
	QVERIFY(!response.contains("HTTP/1.1 400"));
}


void CSocketTimeoutWatcherTest::ProcessingTimeoutGetsServiceUnavailableTest()
{
	CConnectedSockets sockets;
	QVERIFY(sockets.Open());

	imtrest::CSocketTimeoutWatcher watcher(sockets.GetServerSocket(), CreateProcessingConfig(200), CreateHttpBehavior());
	QSignalSpy timedOutSpy(&watcher, &imtrest::CSocketTimeoutWatcher::TimedOut);

	watcher.OnDataReceived();
	watcher.OnRequestCompleted();

	// The handler does not answer in time:
	QTRY_COMPARE_WITH_TIMEOUT(timedOutSpy.count(), 1, 3000);
	QCOMPARE(GetAction(timedOutSpy, 0), int(Tracker::A_PROCESSING_TIMEOUT));

	QTRY_VERIFY_WITH_TIMEOUT(sockets.GetClient().bytesAvailable() > 0, 3000);
	const QByteArray response = sockets.GetClient().readAll();

	QVERIFY(response.startsWith("HTTP/1.1 503 Service Unavailable\r\n"));
	QVERIFY(response.contains("\r\nDate: "));
	QVERIFY(response.contains("\r\nConnection: close\r\n"));
	QVERIFY(response.contains("\r\nContent-Length: 0\r\n"));
	QVERIFY(response.endsWith("\r\n\r\n"));

	// As after 408, the server waits for the closing by the client (RFC 9112, section 9.6):
	QVERIFY(watcher.IsClosing());
	QCOMPARE(watcher.GetPhase(), Tracker::P_CLOSING);
	QCOMPARE(sockets.GetClient().state(), QAbstractSocket::ConnectedState);

	sockets.GetClient().disconnectFromHost();

	QTRY_COMPARE_WITH_TIMEOUT(sockets.GetServerSocket().state(), QAbstractSocket::UnconnectedState, 3000);
	QCOMPARE(timedOutSpy.count(), 1);
}


void CSocketTimeoutWatcherTest::ProcessingTimeoutAbortedAfterLingeringTest()
{
	CConnectedSockets sockets;
	QVERIFY(sockets.Open());

	imtrest::CSocketTimeoutWatcher watcher(sockets.GetServerSocket(), CreateProcessingConfig(150, 300), CreateHttpBehavior());
	QSignalSpy timedOutSpy(&watcher, &imtrest::CSocketTimeoutWatcher::TimedOut);

	watcher.OnDataReceived();
	watcher.OnRequestCompleted();

	// The client does not close the connection after the response:
	QTRY_COMPARE_WITH_TIMEOUT(timedOutSpy.count(), 2, 3000);

	QCOMPARE(GetAction(timedOutSpy, 0), int(Tracker::A_PROCESSING_TIMEOUT));
	QCOMPARE(GetAction(timedOutSpy, 1), int(Tracker::A_ABORT_LINGERING));

	QTRY_COMPARE_WITH_TIMEOUT(sockets.GetClient().state(), QAbstractSocket::UnconnectedState, 3000);
}


void CSocketTimeoutWatcherTest::LateResponseAfterProcessingTimeoutIsIgnoredTest()
{
	CConnectedSockets sockets;
	QVERIFY(sockets.Open());

	imtrest::CSocketTimeoutWatcher watcher(sockets.GetServerSocket(), CreateProcessingConfig(150), CreateHttpBehavior());
	QSignalSpy timedOutSpy(&watcher, &imtrest::CSocketTimeoutWatcher::TimedOut);

	watcher.OnDataReceived();
	watcher.OnRequestCompleted();

	QTRY_COMPARE_WITH_TIMEOUT(timedOutSpy.count(), 1, 3000);
	QTRY_VERIFY_WITH_TIMEOUT(sockets.GetClient().bytesAvailable() > 0, 3000);
	QVERIFY(sockets.GetClient().readAll().startsWith("HTTP/1.1 503 "));

	// The handler finishes after the timeout, the client already has the response to this request:
	watcher.WriteResponse(s_response);

	QTest::qWait(200);

	QCOMPARE(sockets.GetClient().bytesAvailable(), qint64(0));
	QCOMPARE(watcher.GetPhase(), Tracker::P_CLOSING);
}


void CSocketTimeoutWatcherTest::ResponseInTimeCancelsProcessingTimeoutTest()
{
	CConnectedSockets sockets;
	QVERIFY(sockets.Open());

	imtrest::CSocketTimeoutWatcher watcher(sockets.GetServerSocket(), CreateProcessingConfig(500), CreateHttpBehavior());
	QSignalSpy timedOutSpy(&watcher, &imtrest::CSocketTimeoutWatcher::TimedOut);

	watcher.OnDataReceived();
	watcher.OnRequestCompleted();

	QTest::qWait(100);

	watcher.WriteResponse(s_response);

	QTRY_VERIFY_WITH_TIMEOUT(sockets.GetClient().bytesAvailable() >= s_response.size(), 3000);
	QCOMPARE(sockets.GetClient().readAll(), s_response);

	// The time of the handler is over, the connection is waiting for the next request:
	QTest::qWait(600);

	QCOMPARE(timedOutSpy.count(), 0);
	QCOMPARE(watcher.GetPhase(), Tracker::P_IDLE);
	QCOMPARE(sockets.GetClient().state(), QAbstractSocket::ConnectedState);
}


void CSocketTimeoutWatcherTest::ProcessingTimeoutIsDisabledByDefaultTest()
{
	CConnectedSockets sockets;
	QVERIFY(sockets.Open());

	// The default configuration does not limit the handler, for example, for long calculations:
	imtrest::CConnectionTimeoutConfig config = CreateConfig(100, 100, 100);
	QCOMPARE(imtrest::CConnectionTimeoutConfig().requestProcessingMs, 0);

	imtrest::CSocketTimeoutWatcher watcher(sockets.GetServerSocket(), config, CreateHttpBehavior());
	QSignalSpy timedOutSpy(&watcher, &imtrest::CSocketTimeoutWatcher::TimedOut);

	watcher.OnDataReceived();
	watcher.OnRequestCompleted();

	QTest::qWait(500);

	QCOMPARE(timedOutSpy.count(), 0);
	QCOMPARE(watcher.GetPhase(), Tracker::P_PROCESSING);

	// A response after a long handling is delivered:
	watcher.WriteResponse(s_response);

	QTRY_VERIFY_WITH_TIMEOUT(sockets.GetClient().bytesAvailable() >= s_response.size(), 3000);
	QCOMPARE(sockets.GetClient().readAll(), s_response);
}


void CSocketTimeoutWatcherTest::ProcessingTimeoutWithoutMessageClosesConnectionTest()
{
	CConnectedSockets sockets;
	QVERIFY(sockets.Open());

	// A protocol that is unknown to the transport classes does not describe the message of the timeout:
	imtrest::CConnectionTimeoutBehavior behavior;
	behavior.isLimitedBeforeFirstMessageOnly = false;

	imtrest::CSocketTimeoutWatcher watcher(sockets.GetServerSocket(), CreateProcessingConfig(150), behavior);
	QSignalSpy timedOutSpy(&watcher, &imtrest::CSocketTimeoutWatcher::TimedOut);

	watcher.OnDataReceived();
	watcher.OnRequestCompleted();

	QTRY_COMPARE_WITH_TIMEOUT(timedOutSpy.count(), 1, 3000);
	QCOMPARE(GetAction(timedOutSpy, 0), int(Tracker::A_PROCESSING_TIMEOUT));

	QTRY_COMPARE_WITH_TIMEOUT(sockets.GetClient().state(), QAbstractSocket::UnconnectedState, 3000);
	QVERIFY(sockets.GetClient().readAll().isEmpty());
}


void CSocketTimeoutWatcherTest::ProcessingTimeoutGetsMessageOfBehaviorTest()
{
	CConnectedSockets sockets;
	QVERIFY(sockets.Open());

	imtrest::CConnectionTimeoutBehavior behavior;
	behavior.isLimitedBeforeFirstMessageOnly = false;
	behavior.createProcessingTimeoutMessage = [](){
		return QByteArray("503 Busy\r\n");
	};

	imtrest::CSocketTimeoutWatcher watcher(sockets.GetServerSocket(), CreateProcessingConfig(150), behavior);

	watcher.OnDataReceived();
	watcher.OnRequestCompleted();

	QTRY_VERIFY_WITH_TIMEOUT(sockets.GetClient().bytesAvailable() >= 10, 3000);
	QCOMPARE(sockets.GetClient().readAll(), QByteArray("503 Busy\r\n"));
	QCOMPARE(watcher.GetPhase(), Tracker::P_CLOSING);
}


void CSocketTimeoutWatcherTest::ProcessingTimeoutOfPipelinedRequestTest()
{
	CConnectedSockets sockets;
	QVERIFY(sockets.Open());

	imtrest::CSocketTimeoutWatcher watcher(sockets.GetServerSocket(), CreateProcessingConfig(300), CreateHttpBehavior());
	QSignalSpy timedOutSpy(&watcher, &imtrest::CSocketTimeoutWatcher::TimedOut);

	watcher.OnDataReceived();
	watcher.OnRequestCompleted();
	watcher.OnDataReceived();
	watcher.OnRequestCompleted();

	QTest::qWait(200);

	watcher.WriteResponse(s_response);

	QTRY_VERIFY_WITH_TIMEOUT(sockets.GetClient().bytesAvailable() >= s_response.size(), 3000);
	QCOMPARE(sockets.GetClient().readAll(), s_response);

	// The first response was in time, the time of the second request is counted from its end:
	QCOMPARE(watcher.GetPhase(), Tracker::P_PROCESSING);
	QCOMPARE(timedOutSpy.count(), 0);

	QTest::qWait(150);
	QCOMPARE(timedOutSpy.count(), 0);

	QTRY_COMPARE_WITH_TIMEOUT(timedOutSpy.count(), 1, 3000);
	QCOMPARE(GetAction(timedOutSpy, 0), int(Tracker::A_PROCESSING_TIMEOUT));

	QTRY_VERIFY_WITH_TIMEOUT(sockets.GetClient().bytesAvailable() > 0, 3000);
	QVERIFY(sockets.GetClient().readAll().startsWith("HTTP/1.1 503 "));
}


void CSocketTimeoutWatcherTest::InterimMessageIsWrittenTest()
{
	CConnectedSockets sockets;
	QVERIFY(sockets.Open());

	imtrest::CSocketTimeoutWatcher watcher(sockets.GetServerSocket(), CreateConfig(5000, 5000, 5000), CreateHttpBehavior());

	watcher.OnDataReceived();
	watcher.WriteInterim(s_interimMessage);

	QTRY_VERIFY_WITH_TIMEOUT(sockets.GetClient().bytesAvailable() >= s_interimMessage.size(), 3000);
	QCOMPARE(sockets.GetClient().readAll(), s_interimMessage);
}


void CSocketTimeoutWatcherTest::InterimMessageKeepsPhaseTest()
{
	CConnectedSockets sockets;
	QVERIFY(sockets.Open());

	imtrest::CSocketTimeoutWatcher watcher(sockets.GetServerSocket(), CreateConfig(5000, 5000, 5000), CreateHttpBehavior());

	watcher.OnDataReceived();
	watcher.WriteInterim(s_interimMessage);

	QTRY_VERIFY_WITH_TIMEOUT(sockets.GetClient().bytesAvailable() >= s_interimMessage.size(), 3000);
	QTest::qWait(100);

	// The request is still received, 100 is not a response to it
	QCOMPARE(watcher.GetPhase(), Tracker::P_RECEIVING);
	QVERIFY(!watcher.IsClosing());
}


void CSocketTimeoutWatcherTest::InterimMessageDoesNotStartKeepAliveTest()
{
	CConnectedSockets sockets;
	QVERIFY(sockets.Open());

	imtrest::CSocketTimeoutWatcher watcher(sockets.GetServerSocket(), CreateConfig(5000, 150, 5000), CreateHttpBehavior());
	QSignalSpy timedOutSpy(&watcher, &imtrest::CSocketTimeoutWatcher::TimedOut);

	watcher.OnDataReceived();
	watcher.WriteInterim(s_interimMessage);

	// If 100 were taken for a response, the connection would be idle and closed by the keep-alive timeout
	QTest::qWait(500);

	QCOMPARE(timedOutSpy.count(), 0);
	QCOMPARE(watcher.GetPhase(), Tracker::P_RECEIVING);
	QCOMPARE(sockets.GetClient().state(), QAbstractSocket::ConnectedState);
}


void CSocketTimeoutWatcherTest::InterimMessageIsIgnoredWhileClosingTest()
{
	CConnectedSockets sockets;
	QVERIFY(sockets.Open());

	imtrest::CSocketTimeoutWatcher watcher(sockets.GetServerSocket(), CreateConfig(5000, 5000, 5000), CreateHttpBehavior());

	watcher.OnDataReceived();
	watcher.RejectRequest();

	QTRY_VERIFY_WITH_TIMEOUT(sockets.GetClient().bytesAvailable() > 0, 3000);

	watcher.WriteInterim(s_interimMessage);

	QTest::qWait(200);

	const QByteArray response = sockets.GetClient().readAll();

	QVERIFY(response.startsWith("HTTP/1.1 400 "));
	QVERIFY(!response.contains("100 Continue"));
}


void CSocketTimeoutWatcherTest::InterimMessageBeforeResponseKeepsOrderTest()
{
	CConnectedSockets sockets;
	QVERIFY(sockets.Open());

	imtrest::CSocketTimeoutWatcher watcher(sockets.GetServerSocket(), CreateConfig(5000, 5000, 5000), CreateHttpBehavior());

	watcher.OnDataReceived();
	watcher.WriteInterim(s_interimMessage);
	watcher.OnRequestCompleted();
	watcher.WriteResponse(s_response);

	const QByteArray expected = s_interimMessage + s_response;

	QTRY_VERIFY_WITH_TIMEOUT(sockets.GetClient().bytesAvailable() >= expected.size(), 3000);
	QCOMPARE(sockets.GetClient().readAll(), expected);
	QCOMPARE(watcher.GetPhase(), Tracker::P_IDLE);
}


void CSocketTimeoutWatcherTest::RefusalWithMessageTest()
{
	CConnectedSockets sockets;
	QVERIFY(sockets.Open());

	imtrest::CSocketTimeoutWatcher watcher(sockets.GetServerSocket(), CreateConfig(5000, 5000, 5000), CreateHttpBehavior());
	QSignalSpy timedOutSpy(&watcher, &imtrest::CSocketTimeoutWatcher::TimedOut);

	watcher.OnDataReceived();
	watcher.RejectRequest(s_refusalMessage);

	QVERIFY(watcher.IsClosing());
	QCOMPARE(watcher.GetPhase(), Tracker::P_CLOSING);

	// The message of the caller is sent instead of 400
	QTRY_VERIFY_WITH_TIMEOUT(sockets.GetClient().bytesAvailable() >= s_refusalMessage.size(), 3000);
	QCOMPARE(sockets.GetClient().readAll(), s_refusalMessage);

	// The server waits for the closing by the client, as after 400 (RFC 9112, section 9.6)
	QTest::qWait(100);
	QCOMPARE(sockets.GetClient().state(), QAbstractSocket::ConnectedState);

	sockets.GetClient().disconnectFromHost();

	QTRY_COMPARE_WITH_TIMEOUT(sockets.GetServerSocket().state(), QAbstractSocket::UnconnectedState, 3000);
	QCOMPARE(timedOutSpy.count(), 0);
}


void CSocketTimeoutWatcherTest::RefusalWithMessageIsAbortedAfterLingeringTest()
{
	CConnectedSockets sockets;
	QVERIFY(sockets.Open());

	imtrest::CSocketTimeoutWatcher watcher(sockets.GetServerSocket(), CreateConfig(5000, 5000, 300), CreateHttpBehavior());
	QSignalSpy timedOutSpy(&watcher, &imtrest::CSocketTimeoutWatcher::TimedOut);

	watcher.OnDataReceived();
	watcher.RejectRequest(s_refusalMessage);

	// The client does not close the connection and goes on sending the body
	QTRY_COMPARE_WITH_TIMEOUT(timedOutSpy.count(), 1, 3000);
	QCOMPARE(GetAction(timedOutSpy, 0), int(Tracker::A_ABORT_LINGERING));

	QTRY_COMPARE_WITH_TIMEOUT(sockets.GetClient().state(), QAbstractSocket::UnconnectedState, 3000);
}


void CSocketTimeoutWatcherTest::RefusalWithEmptyMessageAbortsConnectionTest()
{
	CConnectedSockets sockets;
	QVERIFY(sockets.Open());

	imtrest::CSocketTimeoutWatcher watcher(sockets.GetServerSocket(), CreateConfig(5000, 5000, 5000), CreateHttpBehavior());

	watcher.OnDataReceived();
	watcher.RejectRequest(QByteArray());

	QVERIFY(watcher.IsClosing());
	QTRY_COMPARE_WITH_TIMEOUT(sockets.GetClient().state(), QAbstractSocket::UnconnectedState, 3000);
	QVERIFY(sockets.GetClient().readAll().isEmpty());
}


void CSocketTimeoutWatcherTest::RefusalWithMessageIsSentOnceTest()
{
	CConnectedSockets sockets;
	QVERIFY(sockets.Open());

	imtrest::CSocketTimeoutWatcher watcher(sockets.GetServerSocket(), CreateConfig(5000, 5000, 5000), CreateHttpBehavior());

	watcher.OnDataReceived();
	watcher.RejectRequest(s_refusalMessage);
	watcher.RejectRequest(s_refusalMessage);
	watcher.RejectRequest();

	QTRY_VERIFY_WITH_TIMEOUT(sockets.GetClient().bytesAvailable() >= s_refusalMessage.size(), 3000);
	QTest::qWait(200);

	QCOMPARE(sockets.GetClient().readAll(), s_refusalMessage);
}


I_ADD_TEST(CSocketTimeoutWatcherTest);


