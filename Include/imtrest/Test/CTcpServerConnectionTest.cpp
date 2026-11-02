// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include "CTcpServerConnectionTest.h"


// Qt includes
#include <QtCore/QElapsedTimer>
#include <QtCore/QMutex>
#include <QtCore/QMutexLocker>
#include <QtNetwork/QHostAddress>
#include <QtNetwork/QTcpServer>
#include <QtNetwork/QTcpSocket>

// ACF includes
#include <icomp/CComponentBase.h>
#include <icomp/TSimComponentWrap.h>

// ImtCore includes
#include <imtcom/CServerConnectionInterfaceParamComp.h>
#include <imtrest/CHttpProtocolEngineComp.h>
#include <imtrest/CHttpRequest.h>
#include <imtrest/CTcpServerComp.h>
#include <imtrest/IRequestServlet.h>
#include <imtrest/IResponseDispatcher.h>


namespace
{


const qint64 s_mebibyte = 1024 * 1024;


/**
	Servlet that does not answer: it keeps the requests until the test answers them, so the test decides how long a request is handled.
	It is called in the threads of the connections, so the list of the requests is guarded.
*/
class CHoldingServletComp:
			public icomp::CComponentBase,
			virtual public imtrest::IRequestServlet
{
public:
	typedef icomp::CComponentBase BaseClass;

	I_BEGIN_COMPONENT(CHoldingServletComp);
		I_REGISTER_INTERFACE(imtrest::IRequestServlet);
	I_END_COMPONENT;

	struct SHeldRequest
	{
		const imtrest::IRequest* requestPtr = nullptr;
		QByteArray requestId;
		QByteArray path;
		qint64 bodySize = 0;
	};

	~CHoldingServletComp()
	{
		// The requests that the test did not answer: the server is already stopped, nobody uses them
		for (const SHeldRequest& held : m_held){
			delete held.requestPtr;
		}
	}

	QList<SHeldRequest> GetHeld() const
	{
		QMutexLocker lock(&m_mutex);

		return m_held;
	}

	QList<SHeldRequest> TakeHeld()
	{
		QMutexLocker lock(&m_mutex);

		const QList<SHeldRequest> retVal = m_held;
		m_held.clear();

		return retVal;
	}

	// reimplemented (imtrest::IRequestServlet)
	bool IsCommandSupported(const QByteArray& /*commandId*/) const override
	{
		return true;
	}

	imtrest::ConstResponsePtr ProcessRequest(const imtrest::IRequest& request, const QByteArray& /*subCommandId*/) const override
	{
		SHeldRequest held;
		held.requestPtr = &request;
		held.requestId = request.GetRequestId();
		held.path = request.GetCommandId();

		const imtrest::CHttpRequest* httpRequestPtr = dynamic_cast<const imtrest::CHttpRequest*>(&request);
		if (httpRequestPtr != nullptr){
			held.bodySize = httpRequestPtr->GetBody().size();
		}

		QMutexLocker lock(&m_mutex);
		m_held.append(held);

		return {};
	}

private:
	mutable QMutex m_mutex;
	mutable QList<SHeldRequest> m_held;
};


typedef icomp::TSimSharedComponentPtr<CHoldingServletComp> Servlet;
typedef icomp::TSimSharedComponentPtr<imtrest::CHttpProtocolEngineComp> Engine;
typedef icomp::TSimSharedComponentPtr<imtcom::CServerConnectionInterfaceParamComp> Connection;
typedef icomp::TSimComponentWrap<imtrest::CTcpServerComp> TcpServer;


int FindFreePort()
{
	QTcpServer server;
	if (!server.listen(QHostAddress::LocalHost, 0)){
		return 0;
	}

	return server.serverPort();
}


/**
	The server of the real components, it is created as the container of the components does: the attributes and the references are set,
	then the components are created. The members are destroyed in the reverse order, so the server (and its threads) goes first.
*/
struct SServer
{
	explicit SServer(int threadsLimit)
	{
		port = FindFreePort();

		servlet->InitComponent();
		engine->InitComponent();

		connection->SetIntAttr("DefaultHttpPort", port);
		connection->InitComponent();

		server.SetIntAttr("ThreadsLimit", threadsLimit);
		server.SetRef("RequestHandler", servlet);
		server.SetRef("ProtocolEngine", engine);
		server.SetRef("ServerInterface", connection);
		server.InitComponent();
	}

	bool IsListening() const
	{
		const imtrest::IServer* serverPtr = &server;

		return (port != 0) && (serverPtr->GetServerStatus() == imtrest::IServer::SS_LISTENING);
	}

	/**
		Answer all requests that the servlet keeps with \c 200, the body of the response is the path of the request.
	*/
	int RespondToHeld()
	{
		const QList<CHoldingServletComp::SHeldRequest> heldList = servlet->TakeHeld();

		for (const CHoldingServletComp::SHeldRequest& held : heldList){
			imtrest::ConstResponsePtr responsePtr(engine->CreateResponse(*held.requestPtr, imtrest::IProtocolEngine::SC_OK, held.path, QByteArrayLiteral("text/plain")).PopInterfacePtr());

			imtrest::IResponseDispatcher* dispatcherPtr = &server;
			dispatcherPtr->SendResponse(held.requestId, responsePtr);

			// The owner of the request frees it after the response, as the worker manager does
			delete held.requestPtr;
		}

		return int(heldList.size());
	}

	int port = 0;
	Servlet servlet;
	Engine engine;
	Connection connection;
	TcpServer server;
};


/**
	Client on a raw socket: the test controls what and when is sent.
*/
class CClient
{
public:
	CClient()
	{
		QObject::connect(&m_socket, &QTcpSocket::readyRead, &m_socket, [this](){
			m_received.append(m_socket.readAll());
		});
	}

	bool Connect(int port)
	{
		m_socket.connectToHost(QHostAddress::LocalHost, quint16(port));

		return m_socket.waitForConnected(5000);
	}

	void SendGet(const QByteArray& path)
	{
		m_socket.write("GET " + path + " HTTP/1.1\r\nHost: test\r\n\r\n");
	}

	void Disconnect()
	{
		m_socket.disconnectFromHost();
	}

	QTcpSocket& GetSocket()
	{
		return m_socket;
	}

	int GetResponseCount() const
	{
		return int(m_received.count("HTTP/1.1 200"));
	}

	const QByteArray& GetReceived() const
	{
		return m_received;
	}

private:
	QTcpSocket m_socket;
	QByteArray m_received;
};


/**
	Sends a request with a big body in parts, as fast as the connection takes it, and counts how much has left the buffer of the client.
	The head and the body are not given to the socket at once: the memory of the test does not depend on the size of the body either.
*/
class CBigUpload
{
public:
	CBigUpload(QTcpSocket& socket, const QByteArray& path, qint64 bodySize)
		:m_socket(socket),
		m_head("PUT " + path + " HTTP/1.1\r\nHost: test\r\nContent-Length: " + QByteArray::number(bodySize) + "\r\n\r\n"),
		m_bodySize(bodySize),
		m_part(256 * 1024, 'x')
	{
	}

	/**
		Give the socket the next parts of the request while it has less than 1 MiB of unsent data.
	*/
	void Pump()
	{
		if (!m_isHeadQueued){
			m_socket.write(m_head);
			m_isHeadQueued = true;
		}

		while ((m_queuedBody < m_bodySize) && (m_socket.bytesToWrite() < s_mebibyte)){
			const qint64 size = qMin<qint64>(m_part.size(), m_bodySize - m_queuedBody);

			m_socket.write(m_part.constData(), size);
			m_queuedBody += size;
		}
	}

	/**
		Number of bytes that the socket of the client has passed to the network.
	*/
	qint64 GetSentSize() const
	{
		const qint64 queued = (m_isHeadQueued ? m_head.size() : 0) + m_queuedBody;

		return queued - m_socket.bytesToWrite();
	}

	qint64 GetTotalSize() const
	{
		return m_head.size() + m_bodySize;
	}

	bool IsAllSent() const
	{
		return GetSentSize() >= GetTotalSize();
	}

private:
	QTcpSocket& m_socket;
	QByteArray m_head;
	qint64 m_bodySize;
	QByteArray m_part;
	qint64 m_queuedBody = 0;
	bool m_isHeadQueued = false;
};


/**
	Collects the warnings of Qt about threads while it lives. The messages go on to the previous handler.
*/
class CThreadWarningCatcher
{
public:
	CThreadWarningCatcher()
	{
		s_messages.clear();
		s_previousHandler = qInstallMessageHandler(&CThreadWarningCatcher::HandleMessage);
	}

	~CThreadWarningCatcher()
	{
		qInstallMessageHandler(s_previousHandler);
	}

	QStringList GetMessages() const
	{
		QMutexLocker lock(&s_mutex);

		return s_messages;
	}

private:
	static void HandleMessage(QtMsgType type, const QMessageLogContext& context, const QString& message)
	{
		if ((type != QtDebugMsg) && (type != QtInfoMsg) && (message.contains(QStringLiteral("QThread")) || message.contains(QStringLiteral("different thread")))){
			QMutexLocker lock(&s_mutex);
			s_messages.append(message);
		}

		if (s_previousHandler != nullptr){
			s_previousHandler(type, context, message);
		}
	}

private:
	static QMutex s_mutex;
	static QStringList s_messages;
	static QtMessageHandler s_previousHandler;
};


QMutex CThreadWarningCatcher::s_mutex;
QStringList CThreadWarningCatcher::s_messages;
QtMessageHandler CThreadWarningCatcher::s_previousHandler = nullptr;


} // namespace


// While the first request is handled, the client sends 32 MiB of the next one. Qt does not limit the read buffer by default,
// so before the limit the server took all the data into its memory at once, even though nobody read them.
void CTcpServerConnectionTest::ReadBufferIsLimitedWhileRequestIsHandledTest()
{
	SServer server(5);
	QVERIFY(server.IsListening());

	CClient client;
	QVERIFY(client.Connect(server.port));

	client.SendGet("/first");
	QTRY_COMPARE_WITH_TIMEOUT(server.servlet->GetHeld().size(), 1, 5000);

	CBigUpload upload(client.GetSocket(), "/second", 32 * s_mebibyte);

	// The server does not read the data of the next request, so the client cannot pass them all to the network
	QElapsedTimer timer;
	timer.start();
	while ((timer.elapsed() < 2000) && !upload.IsAllSent()){
		upload.Pump();
		QTest::qWait(5);
	}

	const qint64 sentSize = upload.GetSentSize();
	QVERIFY2(sentSize < 24 * s_mebibyte, qPrintable(QStringLiteral("The client has sent %1 of %2 bytes while the request is handled").arg(sentSize).arg(upload.GetTotalSize())));

	// The first request is not finished, the next one is not received
	QCOMPARE(server.servlet->GetHeld().size(), 1);
	QCOMPARE(server.servlet->GetHeld().first().path, QByteArray("/first"));
}


// The limit is removed with the response: the held data of the next request are received completely and without damage
void CTcpServerConnectionTest::NextRequestIsReceivedAfterResponseTest()
{
	SServer server(5);
	QVERIFY(server.IsListening());

	CClient client;
	QVERIFY(client.Connect(server.port));

	client.SendGet("/first");
	QTRY_COMPARE_WITH_TIMEOUT(server.servlet->GetHeld().size(), 1, 5000);

	const qint64 bodySize = 32 * s_mebibyte;
	CBigUpload upload(client.GetSocket(), "/second", bodySize);

	QElapsedTimer timer;
	timer.start();
	while ((timer.elapsed() < 500) && !upload.IsAllSent()){
		upload.Pump();
		QTest::qWait(5);
	}

	QCOMPARE(server.RespondToHeld(), 1);

	// The client goes on sending, the server receives the whole body
	timer.restart();
	while ((timer.elapsed() < 30000) && (server.servlet->GetHeld().isEmpty())){
		upload.Pump();
		QTest::qWait(5);
	}

	const QList<CHoldingServletComp::SHeldRequest> heldList = server.servlet->GetHeld();
	QCOMPARE(heldList.size(), 1);
	QCOMPARE(heldList.first().path, QByteArray("/second"));
	QCOMPARE(heldList.first().bodySize, bodySize);

	QCOMPARE(server.RespondToHeld(), 1);
	QTRY_COMPARE_WITH_TIMEOUT(client.GetResponseCount(), 2, 5000);
}


// One thread serves one connection at a time, the other connections wait for it. When a connection is closed, the thread is
// recreated for the next one. The disconnection of such a connection must not break the threads (the warnings of Qt are checked).
void CTcpServerConnectionTest::QueuedConnectionsAreServedTest()
{
	CThreadWarningCatcher warnings;

	SServer server(1);
	QVERIFY(server.IsListening());

	CClient client1;
	CClient client2;
	CClient client3;
	QVERIFY(client1.Connect(server.port));
	client1.SendGet("/c1");
	QTRY_COMPARE_WITH_TIMEOUT(server.servlet->GetHeld().size(), 1, 5000);

	QVERIFY(client2.Connect(server.port));
	client2.SendGet("/c2");
	QVERIFY(client3.Connect(server.port));
	client3.SendGet("/c3");

	// The only thread is busy: the queued connections are not served
	QTest::qWait(300);
	QCOMPARE(server.servlet->GetHeld().size(), 1);
	QCOMPARE(server.servlet->GetHeld().first().path, QByteArray("/c1"));

	QCOMPARE(server.RespondToHeld(), 1);
	QTRY_COMPARE_WITH_TIMEOUT(client1.GetResponseCount(), 1, 5000);
	client1.Disconnect();

	// The connection of the first client is closed: its thread is replaced by the thread of the second one
	QTRY_COMPARE_WITH_TIMEOUT(server.servlet->GetHeld().size(), 1, 5000);
	QCOMPARE(server.servlet->GetHeld().first().path, QByteArray("/c2"));

	QCOMPARE(server.RespondToHeld(), 1);
	QTRY_COMPARE_WITH_TIMEOUT(client2.GetResponseCount(), 1, 5000);
	client2.Disconnect();

	// The thread of the second connection ended, it was created for the queued connection: now the third one is served
	QTRY_COMPARE_WITH_TIMEOUT(server.servlet->GetHeld().size(), 1, 5000);
	QCOMPARE(server.servlet->GetHeld().first().path, QByteArray("/c3"));

	QCOMPARE(server.RespondToHeld(), 1);
	QTRY_COMPARE_WITH_TIMEOUT(client3.GetResponseCount(), 1, 5000);
	client3.Disconnect();

	QTest::qWait(200);

	const QStringList messages = warnings.GetMessages();
	QVERIFY2(messages.isEmpty(), qPrintable(messages.join(QStringLiteral("; "))));
}


I_ADD_TEST(CTcpServerConnectionTest);


