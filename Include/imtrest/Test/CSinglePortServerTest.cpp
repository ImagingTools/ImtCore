// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include "CSinglePortServerTest.h"


// Qt includes
#include <QtCore/QDeadlineTimer>
#include <QtNetwork/QTcpServer>
#include <QtNetwork/QTcpSocket>
#include <QtWebSockets/QWebSocket>

// ImtCore includes
#include <imtrest/IProtocolEngine.h>
#include <imtrest/CWebSocketRequest.h>
#include <imtrest/CWebSocketResponse.h>


namespace
{


const int s_timeout = 5000;


quint16 FindFreePort()
{
	QTcpServer server;
	if (!server.listen(QHostAddress::LocalHost, 0)){
		return 0;
	}

	quint16 port = server.serverPort();
	server.close();

	return port;
}


QByteArray SendHttpGet(quint16 port)
{
	QTcpSocket socket;
	socket.connectToHost(QHostAddress::LocalHost, port);

	// The servers run in this thread too, so wait by processing events instead of blocking.
	QByteArray response;
	QDeadlineTimer deadline(s_timeout);
	bool isRequestSent = false;
	while (!deadline.hasExpired() && !response.contains("\r\n")){
		if (!isRequestSent && (socket.state() == QAbstractSocket::ConnectedState)){
			socket.write(QByteArrayLiteral("GET /test HTTP/1.1\r\nHost: 127.0.0.1\r\n\r\n"));
			isRequestSent = true;
		}

		QTest::qWait(10);

		response += socket.readAll();
	}

	socket.abort();

	return response;
}


QUrl CreateWebSocketUrl(quint16 port)
{
	return QUrl(QStringLiteral("ws://127.0.0.1:%1/test/wssub").arg(port));
}


bool ContainsMessage(const QStringList& messages, const QString& text)
{
	for (const QString& message : messages){
		if (message.contains(text)){
			return true;
		}
	}

	return false;
}


} // namespace


// public methods of the class CTestHttpServletComp

// reimplemented (imtrest::IRequestServlet)

bool CTestHttpServletComp::IsCommandSupported(const QByteArray& /*commandId*/) const
{
	return true;
}


imtrest::ConstResponsePtr CTestHttpServletComp::ProcessRequest(const imtrest::IRequest& request, const QByteArray& /*subCommandId*/) const
{
	const imtrest::IProtocolEngine& engine = request.GetProtocolEngine();

	return imtrest::ConstResponsePtr(engine.CreateResponse(request, imtrest::IProtocolEngine::SC_OK, QByteArrayLiteral("OK"), QByteArrayLiteral("text/plain; charset=utf-8")).PopInterfacePtr());
}


// public methods of the class CTestWebSocketServletComp

// reimplemented (imtrest::IRequestServlet)

bool CTestWebSocketServletComp::IsCommandSupported(const QByteArray& /*commandId*/) const
{
	return true;
}


imtrest::ConstResponsePtr CTestWebSocketServletComp::ProcessRequest(const imtrest::IRequest& request, const QByteArray& /*subCommandId*/) const
{
	const imtrest::CWebSocketRequest* webSocketRequestPtr = dynamic_cast<const imtrest::CWebSocketRequest*>(&request);
	if ((webSocketRequestPtr == nullptr) || (webSocketRequestPtr->GetMethodType() != imtrest::CWebSocketRequest::MT_CONNECTION_INIT)){
		return imtrest::ConstResponsePtr();
	}

	const imtrest::IProtocolEngine& engine = request.GetProtocolEngine();

	return imtrest::ConstResponsePtr(engine.CreateResponse(request, imtrest::IProtocolEngine::SC_OK, QByteArrayLiteral(R"({"type": "connection_ack"})"), QByteArrayLiteral("application/json")).PopInterfacePtr());
}


// public methods of the class CTestServers

CTestServers::CTestServers(bool isSinglePortMode, bool listenWebSocketPort)
	:m_httpPort(FindFreePort()),
	m_webSocketPort(FindFreePort())
{
	m_serverInterfaceCompPtr->SetIntAttr("DefaultHttpPort", m_httpPort);
	m_serverInterfaceCompPtr->SetIntAttr("DefaultWebSocketPort", m_webSocketPort);
	m_serverInterfaceCompPtr->InitComponent();

	m_httpProtocolEngineCompPtr->InitComponent();
	m_webSocketProtocolEngineCompPtr->InitComponent();
	m_webSocketServletCompPtr->InitComponent();

	m_workerManagerCompPtr->SetFactory("RequestHandler", &m_httpServletFactory);
	m_workerManagerCompPtr->SetRef("RequestManager", m_tcpServerCompPtr);
	m_workerManagerCompPtr->SetIntAttr("ThreadsLimit", 2);
	m_workerManagerCompPtr->InitComponent();

	m_webSocketServerCompPtr->SetRef("ProtocolEngine", m_webSocketProtocolEngineCompPtr);
	m_webSocketServerCompPtr->SetRef("RequestServerHandler", m_webSocketServletCompPtr);
	m_webSocketServerCompPtr->SetRef("WebServerConnectionInterface", m_serverInterfaceCompPtr);
	m_webSocketServerCompPtr->SetBoolAttr("ListenWebSocketPort", listenWebSocketPort);
	m_webSocketServerCompPtr->InitComponent();

	m_tcpServerCompPtr->SetRef("RequestHandler", m_workerManagerCompPtr);
	m_tcpServerCompPtr->SetRef("ProtocolEngine", m_httpProtocolEngineCompPtr);
	m_tcpServerCompPtr->SetRef("ServerInterface", m_serverInterfaceCompPtr);
	m_tcpServerCompPtr->SetIntAttr("ThreadsLimit", 5);
	if (isSinglePortMode){
		m_tcpServerCompPtr->SetRef("WebSocketUpgradeHandler", m_webSocketServerCompPtr);
	}
	m_tcpServerCompPtr->InitComponent();
}


CTestServers::~CTestServers()
{
	// The servers reference each other, stop listening explicitly.
	imtrest::IServer& tcpServer = m_tcpServerCompPtr.GetImpl();
	tcpServer.StopServer();

	imtrest::IServer& webSocketServer = m_webSocketServerCompPtr.GetImpl();
	webSocketServer.StopServer();
}


quint16 CTestServers::GetHttpPort() const
{
	return m_httpPort;
}


quint16 CTestServers::GetWebSocketPort() const
{
	return m_webSocketPort;
}


imtrest::CWebSocketServerComp& CTestServers::GetWebSocketServer()
{
	return m_webSocketServerCompPtr.GetImpl();
}


const imtrest::IProtocolEngine& CTestServers::GetWebSocketProtocolEngine()
{
	return m_webSocketProtocolEngineCompPtr.GetImpl();
}


// private slots of the class CSinglePortServerTest

void CSinglePortServerTest::HttpRequestOnSinglePortTest()
{
	CTestServers servers(true);

	QByteArray response = SendHttpGet(servers.GetHttpPort());
	QVERIFY2(response.startsWith("HTTP/1.1 200"), response.constData());
}


void CSinglePortServerTest::WebSocketSubscriptionOnSinglePortTest()
{
	CTestServers servers(true);

	QWebSocket client;
	QStringList messages;
	connect(&client, &QWebSocket::textMessageReceived, this, [&messages](const QString& message){
		messages.append(message);
	});

	QSignalSpy connectedSpy(&client, &QWebSocket::connected);
	client.open(CreateWebSocketUrl(servers.GetHttpPort()));
	QTRY_COMPARE_WITH_TIMEOUT(connectedSpy.count(), 1, s_timeout);

	client.sendTextMessage(QStringLiteral(R"({"type": "connection_init", "payload": {}})"));
	QTRY_VERIFY_WITH_TIMEOUT(ContainsMessage(messages, QStringLiteral("connection_ack")), s_timeout);

	client.sendTextMessage(QStringLiteral(R"({"type": "start", "id": "testSubscription", "payload": {"query": "subscription { OnTest }"}})"));

	// Published like CGqlPublisherCompBase does: through IResponseDispatcher of the WebSocket server.
	const QByteArray data = QByteArrayLiteral(R"({"type": "data", "id": "testSubscription", "payload": {"data": {"OnTest": "published"}}})");
	imtrest::ConstResponsePtr responsePtr(new imtrest::CWebSocketResponse(imtrest::IProtocolEngine::SC_OK, data, QByteArrayLiteral("application/json"), servers.GetWebSocketProtocolEngine()));
	imtrest::CWebSocketServerComp& webSocketServer = servers.GetWebSocketServer();
	QTRY_VERIFY_WITH_TIMEOUT(webSocketServer.SendResponse(QByteArrayLiteral("testSubscription"), responsePtr), s_timeout);
	QTRY_VERIFY_WITH_TIMEOUT(ContainsMessage(messages, QStringLiteral("published")), s_timeout);

	// The separate WebSocket port is still served.
	QWebSocket separatePortClient;
	QSignalSpy separatePortConnectedSpy(&separatePortClient, &QWebSocket::connected);
	separatePortClient.open(CreateWebSocketUrl(servers.GetWebSocketPort()));
	QTRY_COMPARE_WITH_TIMEOUT(separatePortConnectedSpy.count(), 1, s_timeout);
}


void CSinglePortServerTest::WebSocketAbortOnSinglePortTest()
{
	CTestServers servers(true);
	imtrest::CWebSocketServerComp& webSocketServer = servers.GetWebSocketServer();

	{
		QWebSocket client;
		QStringList messages;
		connect(&client, &QWebSocket::textMessageReceived, this, [&messages](const QString& message){
			messages.append(message);
		});

		QSignalSpy connectedSpy(&client, &QWebSocket::connected);
		client.open(CreateWebSocketUrl(servers.GetHttpPort()));
		QTRY_COMPARE_WITH_TIMEOUT(connectedSpy.count(), 1, s_timeout);

		client.sendTextMessage(QStringLiteral(R"({"type": "connection_init", "payload": {}})"));
		QTRY_VERIFY_WITH_TIMEOUT(ContainsMessage(messages, QStringLiteral("connection_ack")), s_timeout);

		client.sendTextMessage(QStringLiteral(R"({"type": "start", "id": "abortedSubscription", "payload": {"query": "subscription { OnTest }"}})"));

		const QByteArray data = QByteArrayLiteral(R"({"type": "data", "id": "abortedSubscription", "payload": {}})");
		imtrest::ConstResponsePtr responsePtr(new imtrest::CWebSocketResponse(imtrest::IProtocolEngine::SC_OK, data, QByteArrayLiteral("application/json"), servers.GetWebSocketProtocolEngine()));
		QTRY_VERIFY_WITH_TIMEOUT(webSocketServer.SendResponse(QByteArrayLiteral("abortedSubscription"), responsePtr), s_timeout);

		// Drop the connection without a close frame.
		client.abort();

		// The server removes the sender of the dropped connection.
		QTRY_VERIFY_WITH_TIMEOUT(!webSocketServer.SendResponse(QByteArrayLiteral("abortedSubscription"), responsePtr), s_timeout);
	}

	QWebSocket client;
	QStringList messages;
	connect(&client, &QWebSocket::textMessageReceived, this, [&messages](const QString& message){
		messages.append(message);
	});

	QSignalSpy connectedSpy(&client, &QWebSocket::connected);
	client.open(CreateWebSocketUrl(servers.GetHttpPort()));
	QTRY_COMPARE_WITH_TIMEOUT(connectedSpy.count(), 1, s_timeout);

	client.sendTextMessage(QStringLiteral(R"({"type": "connection_init", "payload": {}})"));
	QTRY_VERIFY_WITH_TIMEOUT(ContainsMessage(messages, QStringLiteral("connection_ack")), s_timeout);

	QByteArray response = SendHttpGet(servers.GetHttpPort());
	QVERIFY2(response.startsWith("HTTP/1.1 200"), response.constData());
}


void CSinglePortServerTest::SeparateWebSocketPortDisabledTest()
{
	CTestServers servers(true, false);

	imtrest::IServer& webSocketServer = servers.GetWebSocketServer();
	QCOMPARE(webSocketServer.GetServerStatus(), imtrest::IServer::SS_NOT_STARTED);

	QWebSocket client;
	QStringList messages;
	connect(&client, &QWebSocket::textMessageReceived, this, [&messages](const QString& message){
		messages.append(message);
	});

	QSignalSpy connectedSpy(&client, &QWebSocket::connected);
	client.open(CreateWebSocketUrl(servers.GetHttpPort()));
	QTRY_COMPARE_WITH_TIMEOUT(connectedSpy.count(), 1, s_timeout);

	client.sendTextMessage(QStringLiteral(R"({"type": "connection_init", "payload": {}})"));
	QTRY_VERIFY_WITH_TIMEOUT(ContainsMessage(messages, QStringLiteral("connection_ack")), s_timeout);
}


void CSinglePortServerTest::TwoPortModeTest()
{
	CTestServers servers(false);

	QByteArray response = SendHttpGet(servers.GetHttpPort());
	QVERIFY2(response.startsWith("HTTP/1.1 200"), response.constData());

	QWebSocket client;
	QStringList messages;
	connect(&client, &QWebSocket::textMessageReceived, this, [&messages](const QString& message){
		messages.append(message);
	});

	QSignalSpy connectedSpy(&client, &QWebSocket::connected);
	client.open(CreateWebSocketUrl(servers.GetWebSocketPort()));
	QTRY_COMPARE_WITH_TIMEOUT(connectedSpy.count(), 1, s_timeout);

	client.sendTextMessage(QStringLiteral(R"({"type": "connection_init", "payload": {}})"));
	QTRY_VERIFY_WITH_TIMEOUT(ContainsMessage(messages, QStringLiteral("connection_ack")), s_timeout);

	// Without the upgrade handler the HTTP port does not accept WebSocket connections.
	QWebSocket httpPortClient;
	QSignalSpy httpPortConnectedSpy(&httpPortClient, &QWebSocket::connected);
	httpPortClient.open(CreateWebSocketUrl(servers.GetHttpPort()));
	QTest::qWait(1000);
	QCOMPARE(httpPortConnectedSpy.count(), 0);
}


I_ADD_TEST(CSinglePortServerTest);


