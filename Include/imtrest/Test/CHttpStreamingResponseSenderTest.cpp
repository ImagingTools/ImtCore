// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include "CHttpStreamingResponseSenderTest.h"


// STL includes
#include <memory>

// Qt includes
#include <QtNetwork/QTcpServer>
#include <QtNetwork/QTcpSocket>

// ImtCore includes
#include <imtrest/CConnectionTimeoutBehavior.h>
#include <imtrest/CHttpResponse.h>
#include <imtrest/CHttpStreamingResponseSender.h>
#include <imtrest/CSocketTimeoutWatcher.h>
#include <imtrest/IProtocolEngine.h>
#include <imtrest/IStreamingResponse.h>
#include <imtrest/ProtocolTypeIds.h>


namespace
{


typedef imtrest::IStreamingResponseSender Sender;
typedef imtrest::CConnectionTimeoutTracker Tracker;

const qint64 s_mebibyte = 1024 * 1024;


class CEngineStub: virtual public imtrest::IProtocolEngine
{
public:
	// reimplemented (imtrest::IProtocolEngine)
	QByteArray GetProtocolTypeId() const override
	{
		return QByteArrayLiteral("http");
	}

	const iser::IVersionInfo* GetProtocolVersion() const override
	{
		return nullptr;
	}

	bool GetProtocolStatusCode(int statusCode, int& protocolStatusCode, QByteArray& statusCodeLiteral) const override
	{
		switch (statusCode){
		case SC_OK:
			protocolStatusCode = 200;
			statusCodeLiteral = "OK";
			return true;

		case SC_NOT_MODIFIED:
			protocolStatusCode = 304;
			statusCodeLiteral = "Not Modified";
			return true;

		default:
			return false;
		}
	}

	imtrest::IRequestUniquePtr CreateRequest(const imtrest::IRequestServlet& /*requestHandler*/) const override
	{
		return nullptr;
	}

	imtrest::IRequestUniquePtr CreateRequestForSend(
				const imtrest::IRequestServlet& /*requestHandler*/,
				int /*statusCode*/,
				const QByteArray& /*data*/,
				const QByteArray& /*dataTypeId*/) const override
	{
		return nullptr;
	}

	imtrest::IResponseUniquePtr CreateResponse(
				const imtrest::IRequest& /*request*/,
				int /*statusCode*/,
				const QByteArray& /*data*/,
				const QByteArray& /*dataTypeId*/) const override
	{
		return nullptr;
	}
};


// The responses refer to the engine, so it lives as long as the test
CEngineStub s_engine;


/**
	Data of the source at the given position: every byte depends on its position, so lost, repeated and moved parts are found.
*/
QByteArray CreatePattern(qint64 offset, qint64 size)
{
	QByteArray retVal(qsizetype(size), Qt::Uninitialized);

	for (qint64 i = 0; i < size; ++i){
		retVal[qsizetype(i)] = char(((offset + i) * 31 + 7) & 0xFF);
	}

	return retVal;
}


/**
	State and the script of a test source. The source is owned by the streamer, so the test looks at the state that it shares with it.
*/
struct SSourceState
{
	// The script
	qint64 size = 0;
	qint64 partSize = 100000;
	qint64 waitAt = -1;
	qint64 errorAt = -1;
	bool isSourceAvailable = true;

	// The observed behavior
	qint64 position = 0;
	int createCount = 0;
	int abortCount = 0;
};


typedef std::shared_ptr<SSourceState> StatePtr;


StatePtr CreateState(qint64 size)
{
	StatePtr retVal = std::make_shared<SSourceState>();
	retVal->size = size;

	return retVal;
}


class CTestSource: virtual public imtrest::IResponseBodySource
{
public:
	explicit CTestSource(const StatePtr& statePtr)
		:m_statePtr(statePtr)
	{
	}

	// reimplemented (imtrest::IResponseBodySource)
	ReadResult Read(QByteArray& data, qint64 maxSize) override
	{
		SSourceState& state = *m_statePtr;

		if ((state.errorAt >= 0) && (state.position >= state.errorAt)){
			return RR_ERROR;
		}

		if ((state.waitAt >= 0) && (state.position >= state.waitAt)){
			return RR_WAIT;
		}

		if (state.position >= state.size){
			return RR_END;
		}

		qint64 count = qMin(qMin(state.partSize, state.size - state.position), maxSize);
		if (state.errorAt >= 0){
			count = qMin(count, state.errorAt - state.position);
		}

		data = CreatePattern(state.position, count);
		state.position += count;

		return RR_DATA;
	}

	void SetDataAvailableHandler(const DataAvailableHandler& /*handler*/) override
	{
	}

	void Abort() override
	{
		++m_statePtr->abortCount;
	}

private:
	StatePtr m_statePtr;
};


class CTestStreamingResponse: public imtrest::CHttpResponse, virtual public imtrest::IStreamingResponse
{
public:
	CTestStreamingResponse(int statusCode, qint64 bodySize, const StatePtr& statePtr)
		:imtrest::CHttpResponse(statusCode, QByteArray(), "application/octet-stream", s_engine),
		m_bodySize(bodySize),
		m_statePtr(statePtr)
	{
	}

	// reimplemented (imtrest::IStreamingResponse)
	qint64 GetBodySize() const override
	{
		return m_bodySize;
	}

	imtrest::IResponseBodySourcePtr CreateBodySource() const override
	{
		++m_statePtr->createCount;

		if (!m_statePtr->isSourceAvailable){
			return nullptr;
		}

		return imtrest::IResponseBodySourcePtr(new CTestSource(m_statePtr));
	}

private:
	qint64 m_bodySize;
	StatePtr m_statePtr;
};


imtrest::ConstResponsePtr CreateStreamingResponse(int statusCode, qint64 bodySize, const StatePtr& statePtr)
{
	return imtrest::ConstResponsePtr(new CTestStreamingResponse(statusCode, bodySize, statePtr));
}


/**
	Connection of a server with the watcher of its timeouts and the sender. The request is received and is being processed, so the response
	can be started.
*/
class CConnection
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

		if ((m_client.state() != QAbstractSocket::ConnectedState) && !m_client.waitForConnected(5000)){
			return false;
		}

		imtrest::CConnectionTimeoutConfig config;
		config.requestInactivityMs = 5000;
		config.keepAliveMs = 5000;
		config.lingeringCloseMs = 500;

		m_watcherPtr = std::make_unique<imtrest::CSocketTimeoutWatcher>(
					*m_serverSocketPtr,
					config,
					imtrest::CConnectionTimeoutBehavior::CreateForProtocol(imtrest::ProtocolTypeIds::s_http));

		StartRequest();

		m_senderPtr = std::make_unique<imtrest::CHttpStreamingResponseSender>(*m_serverSocketPtr, *m_watcherPtr, s_mebibyte);

		return true;
	}

	/**
		The next request of the connection is received.
	*/
	void StartRequest()
	{
		m_watcherPtr->OnDataReceived();
		m_watcherPtr->OnRequestCompleted();
	}

	QTcpSocket& GetClient()
	{
		return m_client;
	}

	QTcpSocket& GetServerSocket()
	{
		return *m_serverSocketPtr;
	}

	imtrest::CSocketTimeoutWatcher& GetWatcher()
	{
		return *m_watcherPtr;
	}

	Sender& GetSender()
	{
		return *m_senderPtr;
	}

	void DestroySender()
	{
		m_senderPtr.reset();
	}

private:
	QTcpServer m_server;
	QTcpSocket m_client;
	QTcpSocket* m_serverSocketPtr = nullptr;
	std::unique_ptr<imtrest::CSocketTimeoutWatcher> m_watcherPtr;
	std::unique_ptr<imtrest::CHttpStreamingResponseSender> m_senderPtr;
};


/**
	Counter of the calls of the handler of the end of the response.
*/
struct SFinishedCounter
{
	int count = 0;

	Sender::FinishedHandler GetHandler()
	{
		return [this](){
			++count;
		};
	}
};


Sender::Hints CreateHints(bool isChunkedAllowed, bool isConnectionClosing = false, bool isBodyOmitted = false)
{
	Sender::Hints retVal;
	retVal.isChunkedAllowed = isChunkedAllowed;
	retVal.isConnectionClosing = isConnectionClosing;
	retVal.isBodyOmitted = isBodyOmitted;

	return retVal;
}


/**
	Take the data that the client has received.
*/
qint64 Drain(QTcpSocket& client, QByteArray& received)
{
	received.append(client.readAll());

	return received.size();
}


bool DrainEndsWith(QTcpSocket& client, QByteArray& received, const QByteArray& end)
{
	received.append(client.readAll());

	return received.endsWith(end);
}


/**
	Take the data that the client has received. Returns \c true when the connection is closed: the data that are received before are
	still in the buffer of the socket.
*/
bool DrainUntilClosed(QTcpSocket& client, QByteArray& received)
{
	received.append(client.readAll());

	return client.state() == QAbstractSocket::UnconnectedState;
}


/**
	Split the data of the client to the head and the body, the head has the end of the headers.
*/
bool SplitMessage(const QByteArray& data, QByteArray& head, QByteArray& body)
{
	const qsizetype headEnd = data.indexOf("\r\n\r\n");
	if (headEnd < 0){
		return false;
	}

	head = data.left(headEnd + 4);
	body = data.mid(headEnd + 4);

	return true;
}


bool HasHeader(const QByteArray& head, const QByteArray& header)
{
	return head.toLower().contains("\r\n" + header.toLower() + "\r\n");
}


/**
	Take the data that the client has received. Returns the size of the body that is received, -1 if the head is not received yet.
*/
qint64 DrainBodySize(QTcpSocket& client, QByteArray& received)
{
	received.append(client.readAll());

	QByteArray head;
	QByteArray body;
	if (!SplitMessage(received, head, body)){
		return -1;
	}

	return body.size();
}


/**
	Take the parts of the body from the chunked encoding.
*/
bool DecodeChunked(const QByteArray& data, QByteArray& body)
{
	body.clear();

	qsizetype position = 0;

	while (true){
		const qsizetype lineEnd = data.indexOf("\r\n", position);
		if (lineEnd < 0){
			return false;
		}

		bool isOk = false;
		const qint64 size = data.mid(position, lineEnd - position).toLongLong(&isOk, 16);
		if (!isOk){
			return false;
		}

		position = lineEnd + 2;

		if (size == 0){
			return data.mid(position) == "\r\n";
		}

		if (((position + size + 2) > data.size()) || (data.mid(position + qsizetype(size), 2) != "\r\n")){
			return false;
		}

		body.append(data.mid(position, qsizetype(size)));
		position += qsizetype(size) + 2;
	}
}


} // namespace


// private slots

void CHttpStreamingResponseSenderTest::KnownSizeTest()
{
	CConnection connection;
	QVERIFY(connection.Open());

	const qint64 size = 300000;
	StatePtr statePtr = CreateState(size);
	SFinishedCounter counter;

	QCOMPARE(connection.GetSender().Start(CreateStreamingResponse(imtrest::IProtocolEngine::SC_OK, size, statePtr), CreateHints(true), counter.GetHandler()), Sender::SR_STARTED);

	QByteArray received;
	QTRY_VERIFY_WITH_TIMEOUT(DrainBodySize(connection.GetClient(), received) >= size, 10000);

	QByteArray head;
	QByteArray body;
	QVERIFY(SplitMessage(received, head, body));

	QVERIFY(head.startsWith("HTTP/1.1 200 OK\r\n"));
	QVERIFY(HasHeader(head, "Content-Length: 300000"));
	QVERIFY(!head.toLower().contains("transfer-encoding"));
	QCOMPARE(body, CreatePattern(0, size));

	QTRY_COMPARE_WITH_TIMEOUT(counter.count, 1, 5000);
	QCOMPARE(statePtr->createCount, 1);
	QCOMPARE(statePtr->abortCount, 0);
	QTRY_COMPARE_WITH_TIMEOUT(connection.GetWatcher().GetPhase(), Tracker::P_IDLE, 3000);
}


void CHttpStreamingResponseSenderTest::ChunkedTest()
{
	CConnection connection;
	QVERIFY(connection.Open());

	const qint64 size = 300000;
	StatePtr statePtr = CreateState(size);
	SFinishedCounter counter;

	// The size is not known and the client understands the chunked body
	QCOMPARE(connection.GetSender().Start(CreateStreamingResponse(imtrest::IProtocolEngine::SC_OK, -1, statePtr), CreateHints(true), counter.GetHandler()), Sender::SR_STARTED);

	QByteArray received;
	QTRY_VERIFY_WITH_TIMEOUT(DrainEndsWith(connection.GetClient(), received, "\r\n0\r\n\r\n"), 10000);

	QByteArray head;
	QByteArray rawBody;
	QVERIFY(SplitMessage(received, head, rawBody));

	QVERIFY(HasHeader(head, "Transfer-Encoding: chunked"));
	QVERIFY(!head.toLower().contains("content-length"));

	QByteArray body;
	QVERIFY(DecodeChunked(rawBody, body));
	QCOMPARE(body, CreatePattern(0, size));

	QTRY_COMPARE_WITH_TIMEOUT(counter.count, 1, 5000);
	QCOMPARE(statePtr->abortCount, 0);
}


void CHttpStreamingResponseSenderTest::UntilCloseTest()
{
	CConnection connection;
	QVERIFY(connection.Open());

	const qint64 size = 200000;
	StatePtr statePtr = CreateState(size);
	SFinishedCounter counter;

	// The size is not known and the client does not understand the chunked body (HTTP/1.0): the end of the body is the closing of the connection
	QCOMPARE(connection.GetSender().Start(CreateStreamingResponse(imtrest::IProtocolEngine::SC_OK, -1, statePtr), CreateHints(false), counter.GetHandler()), Sender::SR_STARTED);

	QByteArray received;
	QTRY_VERIFY_WITH_TIMEOUT(DrainUntilClosed(connection.GetClient(), received), 10000);

	QByteArray head;
	QByteArray body;
	QVERIFY(SplitMessage(received, head, body));

	QVERIFY(HasHeader(head, "Connection: close"));
	QVERIFY(!head.toLower().contains("transfer-encoding"));
	QVERIFY(!head.toLower().contains("content-length"));
	QCOMPARE(body, CreatePattern(0, size));

	QTRY_COMPARE_WITH_TIMEOUT(counter.count, 1, 5000);
	QCOMPARE(statePtr->abortCount, 0);
}


void CHttpStreamingResponseSenderTest::ConnectionClosingHintTest()
{
	CConnection connection;
	QVERIFY(connection.Open());

	StatePtr statePtr = CreateState(1000);
	SFinishedCounter counter;

	QCOMPARE(connection.GetSender().Start(CreateStreamingResponse(imtrest::IProtocolEngine::SC_OK, 1000, statePtr), CreateHints(true, true), counter.GetHandler()), Sender::SR_STARTED);

	QByteArray received;
	QTRY_VERIFY_WITH_TIMEOUT(DrainBodySize(connection.GetClient(), received) >= 1000, 5000);

	QByteArray head;
	QByteArray body;
	QVERIFY(SplitMessage(received, head, body));

	QVERIFY(HasHeader(head, "Connection: close"));
	QVERIFY(HasHeader(head, "Content-Length: 1000"));
	QCOMPARE(body, CreatePattern(0, 1000));
}


void CHttpStreamingResponseSenderTest::NoBodyTest_data()
{
	QTest::addColumn<int>("statusCode");
	QTest::addColumn<qint64>("bodySize");
	QTest::addColumn<bool>("isBodyOmitted");
	QTest::addColumn<QByteArray>("contentLength");

	// RFC 9110, section 9.3.2, 8.6, 15.4.5: the head only, the length is the length of the representation
	QTest::newRow("HEAD") << int(imtrest::IProtocolEngine::SC_OK) << qint64(1000) << true << QByteArray("1000");
	QTest::newRow("empty body") << int(imtrest::IProtocolEngine::SC_OK) << qint64(0) << false << QByteArray("0");
	QTest::newRow("304") << int(imtrest::IProtocolEngine::SC_NOT_MODIFIED) << qint64(1000) << false << QByteArray();
}


void CHttpStreamingResponseSenderTest::NoBodyTest()
{
	QFETCH(int, statusCode);
	QFETCH(qint64, bodySize);
	QFETCH(bool, isBodyOmitted);
	QFETCH(QByteArray, contentLength);

	CConnection connection;
	QVERIFY(connection.Open());

	StatePtr statePtr = CreateState(bodySize);
	SFinishedCounter counter;

	// The whole response is written, so nothing is left to finish
	QCOMPARE(connection.GetSender().Start(CreateStreamingResponse(statusCode, bodySize, statePtr), CreateHints(true, false, isBodyOmitted), counter.GetHandler()), Sender::SR_COMPLETED);

	QByteArray received;
	QTRY_VERIFY_WITH_TIMEOUT(DrainEndsWith(connection.GetClient(), received, "\r\n\r\n"), 5000);

	QTest::qWait(100);
	Drain(connection.GetClient(), received);

	QByteArray head;
	QByteArray body;
	QVERIFY(SplitMessage(received, head, body));
	QVERIFY(body.isEmpty());

	if (contentLength.isEmpty()){
		QVERIFY(!head.toLower().contains("content-length"));
	}
	else{
		QVERIFY(HasHeader(head, "Content-Length: " + contentLength));
	}

	QVERIFY(!head.toLower().contains("transfer-encoding"));

	// The source is not needed
	QCOMPARE(statePtr->createCount, 0);
	QCOMPARE(counter.count, 0);
	QTRY_COMPARE_WITH_TIMEOUT(connection.GetWatcher().GetPhase(), Tracker::P_IDLE, 3000);
}


void CHttpStreamingResponseSenderTest::NotStreamingResponseTest()
{
	CConnection connection;
	QVERIFY(connection.Open());

	imtrest::ConstResponsePtr responsePtr(new imtrest::CHttpResponse(imtrest::IProtocolEngine::SC_OK, "hello", "text/plain", s_engine));
	SFinishedCounter counter;

	QCOMPARE(connection.GetSender().Start(responsePtr, CreateHints(true), counter.GetHandler()), Sender::SR_FAILED);

	QTest::qWait(100);

	// Nothing is written: the caller sends the error
	QVERIFY(connection.GetClient().readAll().isEmpty());
	QCOMPARE(counter.count, 0);
}


void CHttpStreamingResponseSenderTest::UnknownStatusTest()
{
	CConnection connection;
	QVERIFY(connection.Open());

	StatePtr statePtr = CreateState(100);
	SFinishedCounter counter;

	// The head cannot be made without the status line
	QCOMPARE(connection.GetSender().Start(CreateStreamingResponse(imtrest::IProtocolEngine::SC_NONE, 100, statePtr), CreateHints(true), counter.GetHandler()), Sender::SR_FAILED);

	QTest::qWait(100);

	QVERIFY(connection.GetClient().readAll().isEmpty());
	QCOMPARE(statePtr->createCount, 0);
	QCOMPARE(counter.count, 0);
}


void CHttpStreamingResponseSenderTest::NoSourceTest()
{
	CConnection connection;
	QVERIFY(connection.Open());

	StatePtr statePtr = CreateState(100);
	statePtr->isSourceAvailable = false;
	SFinishedCounter counter;

	// For example, the file cannot be opened: nothing is written, so the client can get the error instead of a broken response
	QCOMPARE(connection.GetSender().Start(CreateStreamingResponse(imtrest::IProtocolEngine::SC_OK, 100, statePtr), CreateHints(true), counter.GetHandler()), Sender::SR_FAILED);

	QTest::qWait(100);

	QVERIFY(connection.GetClient().readAll().isEmpty());
	QCOMPARE(statePtr->createCount, 1);
	QCOMPARE(counter.count, 0);
}


void CHttpStreamingResponseSenderTest::ClosedSocketTest()
{
	CConnection connection;
	QVERIFY(connection.Open());

	connection.GetServerSocket().abort();

	StatePtr statePtr = CreateState(100);
	SFinishedCounter counter;

	// The client has gone: the response is not needed and the source is not created
	QCOMPARE(connection.GetSender().Start(CreateStreamingResponse(imtrest::IProtocolEngine::SC_OK, 100, statePtr), CreateHints(true), counter.GetHandler()), Sender::SR_DROPPED);

	QCOMPARE(statePtr->createCount, 0);
	QCOMPARE(counter.count, 0);
}


void CHttpStreamingResponseSenderTest::ClosingConnectionTest()
{
	CConnection connection;
	QVERIFY(connection.Open());

	// For example, the request was rejected while the handler worked
	connection.GetWatcher().RejectRequest();
	QVERIFY(connection.GetWatcher().IsClosing());

	StatePtr statePtr = CreateState(100);
	SFinishedCounter counter;

	QCOMPARE(connection.GetSender().Start(CreateStreamingResponse(imtrest::IProtocolEngine::SC_OK, 100, statePtr), CreateHints(true), counter.GetHandler()), Sender::SR_DROPPED);

	QCOMPARE(statePtr->createCount, 0);
	QCOMPARE(counter.count, 0);
}


void CHttpStreamingResponseSenderTest::BusySenderTest()
{
	CConnection connection;
	QVERIFY(connection.Open());

	// The source waits for the data that do not come: the first response is not finished
	StatePtr firstStatePtr = CreateState(1000);
	firstStatePtr->waitAt = 0;
	SFinishedCounter firstCounter;

	QCOMPARE(connection.GetSender().Start(CreateStreamingResponse(imtrest::IProtocolEngine::SC_OK, 1000, firstStatePtr), CreateHints(true), firstCounter.GetHandler()), Sender::SR_STARTED);

	// One response at a time
	StatePtr secondStatePtr = CreateState(1000);
	SFinishedCounter secondCounter;

	QCOMPARE(connection.GetSender().Start(CreateStreamingResponse(imtrest::IProtocolEngine::SC_OK, 1000, secondStatePtr), CreateHints(true), secondCounter.GetHandler()), Sender::SR_FAILED);

	QCOMPARE(secondStatePtr->createCount, 0);
	QCOMPARE(secondCounter.count, 0);
	QCOMPARE(firstCounter.count, 0);
	QCOMPARE(firstStatePtr->abortCount, 0);

	// The unfinished response is dropped with the sender: the source is released
	connection.DestroySender();

	QCOMPARE(firstStatePtr->abortCount, 1);
}


void CHttpStreamingResponseSenderTest::HandlerIsCalledOnceTest()
{
	CConnection connection;
	QVERIFY(connection.Open());

	// The body is larger than the limit of the unsent data, so it is sent in several steps
	const qint64 size = 3 * s_mebibyte;
	StatePtr statePtr = CreateState(size);
	SFinishedCounter counter;

	QCOMPARE(connection.GetSender().Start(CreateStreamingResponse(imtrest::IProtocolEngine::SC_OK, size, statePtr), CreateHints(true), counter.GetHandler()), Sender::SR_STARTED);

	QByteArray received;
	QTRY_VERIFY_WITH_TIMEOUT(DrainBodySize(connection.GetClient(), received) >= size, 20000);

	QTRY_COMPARE_WITH_TIMEOUT(counter.count, 1, 5000);

	QTest::qWait(100);
	QCOMPARE(counter.count, 1);
	QCOMPARE(statePtr->abortCount, 0);
}


void CHttpStreamingResponseSenderTest::HandlerIsCalledOnSourceErrorTest()
{
	CConnection connection;
	QVERIFY(connection.Open());

	StatePtr statePtr = CreateState(1000);
	statePtr->errorAt = 400;
	SFinishedCounter counter;

	// The response is started, so the failure is the end of it: the client sees the broken connection
	QCOMPARE(connection.GetSender().Start(CreateStreamingResponse(imtrest::IProtocolEngine::SC_OK, 1000, statePtr), CreateHints(true), counter.GetHandler()), Sender::SR_STARTED);

	QTRY_COMPARE_WITH_TIMEOUT(counter.count, 1, 5000);
	QCOMPARE(statePtr->abortCount, 1);

	QTRY_COMPARE_WITH_TIMEOUT(connection.GetClient().state(), QAbstractSocket::UnconnectedState, 5000);

	QTest::qWait(100);
	QCOMPARE(counter.count, 1);
}


void CHttpStreamingResponseSenderTest::NextResponseAfterFinishTest()
{
	CConnection connection;
	QVERIFY(connection.Open());

	SFinishedCounter counter;
	QByteArray received;

	for (int i = 0; i < 2; ++i){
		StatePtr statePtr = CreateState(1000);

		QCOMPARE(connection.GetSender().Start(CreateStreamingResponse(imtrest::IProtocolEngine::SC_OK, 1000, statePtr), CreateHints(true), counter.GetHandler()), Sender::SR_STARTED);

		QTRY_COMPARE_WITH_TIMEOUT(counter.count, i + 1, 5000);

		received.clear();
		QTRY_VERIFY_WITH_TIMEOUT(DrainBodySize(connection.GetClient(), received) >= 1000, 5000);

		QByteArray head;
		QByteArray body;
		QVERIFY(SplitMessage(received, head, body));
		QCOMPARE(body, CreatePattern(0, 1000));

		// The next request of the persistent connection
		QTRY_COMPARE_WITH_TIMEOUT(connection.GetWatcher().GetPhase(), Tracker::P_IDLE, 3000);
		connection.StartRequest();
	}
}


I_ADD_TEST(CHttpStreamingResponseSenderTest);
