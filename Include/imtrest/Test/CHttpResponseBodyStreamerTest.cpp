// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include "CHttpResponseBodyStreamerTest.h"


// STL includes
#include <memory>

// Qt includes
#include <QtNetwork/QTcpServer>
#include <QtNetwork/QTcpSocket>
#include <QtTest/QSignalSpy>

// ImtCore includes
#include <imtrest/CConnectionTimeoutBehavior.h>
#include <imtrest/CHttpResponseBodyStreamer.h>
#include <imtrest/CSocketTimeoutWatcher.h>
#include <imtrest/ProtocolTypeIds.h>


namespace
{


typedef imtrest::CHttpResponseBodyStreamer Streamer;
typedef imtrest::IResponseBodySource Source;
typedef imtrest::CConnectionTimeoutTracker Tracker;
typedef imtrest::CHttpSender::BodyFraming Framing;


// The head is written by the watcher before the streamer starts, its content does not matter for the streamer
const QByteArray s_head("HTTP/1.1 200 OK\r\n\r\n");

const qint64 s_mebibyte = 1024 * 1024;


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
	State and the script of a test source. The streamer owns the source, so the test looks at the state that it shares with the source.
*/
struct SSourceState
{
	// The script
	qint64 size = 0;
	qint64 partSize = 100000;
	qint64 waitAt = -1;
	qint64 errorAt = -1;
	qint64 emptyDataAt = -1;
	bool isIgnoringMaxSize = false;

	// The observed behavior
	qint64 position = 0;
	qint64 maxRequested = 0;
	int readCount = 0;
	int abortCount = 0;
	Source::DataAvailableHandler handler;

	/**
		The data are available after the wait.
	*/
	void Release()
	{
		waitAt = -1;

		if (handler){
			handler();
		}
	}
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

		++state.readCount;
		state.maxRequested = qMax(state.maxRequested, maxSize);

		if ((state.errorAt >= 0) && (state.position >= state.errorAt)){
			return RR_ERROR;
		}

		if ((state.waitAt >= 0) && (state.position >= state.waitAt)){
			return RR_WAIT;
		}

		if ((state.emptyDataAt >= 0) && (state.position >= state.emptyDataAt)){
			data.clear();

			return RR_DATA;
		}

		if (state.position >= state.size){
			return RR_END;
		}

		qint64 count = qMin(state.partSize, state.size - state.position);

		// The script stops at its marks
		if (state.waitAt >= 0){
			count = qMin(count, state.waitAt - state.position);
		}

		if (state.errorAt >= 0){
			count = qMin(count, state.errorAt - state.position);
		}

		if (state.emptyDataAt >= 0){
			count = qMin(count, state.emptyDataAt - state.position);
		}

		if (!state.isIgnoringMaxSize){
			count = qMin(count, maxSize);
		}

		data = CreatePattern(state.position, count);
		state.position += count;

		return RR_DATA;
	}

	void SetDataAvailableHandler(const DataAvailableHandler& handler) override
	{
		m_statePtr->handler = handler;
	}

	void Abort() override
	{
		++m_statePtr->abortCount;
	}

private:
	StatePtr m_statePtr;
};


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


/**
	Connection of a server with the watcher of its timeouts. The request is received and is being processed, so the response
	can be started.
*/
class CConnection
{
public:
	bool Open(int requestInactivityMs = 5000)
	{
		if (!m_sockets.Open()){
			return false;
		}

		imtrest::CConnectionTimeoutConfig config;
		config.requestInactivityMs = requestInactivityMs;
		config.keepAliveMs = 5000;
		config.lingeringCloseMs = 500;

		m_watcherPtr = std::make_unique<imtrest::CSocketTimeoutWatcher>(
					m_sockets.GetServerSocket(),
					config,
					imtrest::CConnectionTimeoutBehavior::CreateForProtocol(imtrest::ProtocolTypeIds::s_http));

		m_watcherPtr->OnDataReceived();
		m_watcherPtr->OnRequestCompleted();

		return true;
	}

	QTcpSocket& GetClient()
	{
		return m_sockets.GetClient();
	}

	QTcpSocket& GetServerSocket()
	{
		return m_sockets.GetServerSocket();
	}

	imtrest::CSocketTimeoutWatcher& GetWatcher()
	{
		return *m_watcherPtr;
	}

	/**
		Write the head of the response and create the streamer. It is not started.
	*/
	std::unique_ptr<Streamer> CreateStreamer(const StatePtr& statePtr, Framing framing, qint64 bodySize, qint64 writeHighWatermark)
	{
		if (!m_watcherPtr->BeginStreamingResponse(s_head)){
			return nullptr;
		}

		imtrest::IResponseBodySourcePtr sourcePtr(new CTestSource(statePtr));

		return std::make_unique<Streamer>(m_sockets.GetServerSocket(), *m_watcherPtr, std::move(sourcePtr), framing, bodySize, writeHighWatermark);
	}

private:
	CConnectedSockets m_sockets;
	std::unique_ptr<imtrest::CSocketTimeoutWatcher> m_watcherPtr;
};


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
	Client that checks the body by the pattern and does not keep it.
*/
class CPatternReader
{
public:
	CPatternReader(QTcpSocket& client, qint64 headSize)
		:m_client(client),
		m_headSize(headSize)
	{
	}

	qint64 Drain()
	{
		const QByteArray data = m_client.readAll();

		for (qsizetype i = 0; i < data.size(); ++i){
			const qint64 position = m_received + i;
			if (position < m_headSize){
				continue;
			}

			if (data.at(i) != char(((position - m_headSize) * 31 + 7) & 0xFF)){
				m_isOk = false;
			}
		}

		m_received += data.size();

		return m_received;
	}

	bool IsOk() const
	{
		return m_isOk;
	}

private:
	QTcpSocket& m_client;
	qint64 m_headSize;
	qint64 m_received = 0;
	bool m_isOk = true;
};


/**
	Take the parts of the body from the chunked encoding.
*/
bool DecodeChunked(const QByteArray& data, QByteArray& body, QList<qint64>& sizes)
{
	body.clear();
	sizes.clear();

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
			// The last chunk has no trailers: only the empty line follows
			return data.mid(position) == "\r\n";
		}

		if ((position + size + 2) > data.size()){
			return false;
		}

		body.append(data.mid(position, qsizetype(size)));

		if (data.mid(position + qsizetype(size), 2) != "\r\n"){
			return false;
		}

		sizes.append(size);
		position += qsizetype(size) + 2;
	}
}


bool IsSuccess(const QSignalSpy& spy, int index)
{
	return spy.at(index).at(0).toBool();
}


} // namespace


// private slots

void CHttpResponseBodyStreamerTest::SizedBodyTest()
{
	CConnection connection;
	QVERIFY(connection.Open());

	// The parts of the source are larger than the part that the streamer reads at a time
	const qint64 size = s_mebibyte + 3;

	StatePtr statePtr = CreateState(size);
	statePtr->partSize = 700000;

	std::unique_ptr<Streamer> streamerPtr = connection.CreateStreamer(statePtr, imtrest::CHttpSender::BF_CONTENT_LENGTH, size, s_mebibyte);
	QVERIFY(streamerPtr);

	QSignalSpy finishedSpy(streamerPtr.get(), &Streamer::Finished);
	streamerPtr->Start();

	QByteArray received;
	QTRY_VERIFY_WITH_TIMEOUT(Drain(connection.GetClient(), received) >= s_head.size() + size, 10000);

	QCOMPARE(received, s_head + CreatePattern(0, size));

	QCOMPARE(finishedSpy.count(), 1);
	QVERIFY(IsSuccess(finishedSpy, 0));
	QVERIFY(streamerPtr->IsFinished());
	QCOMPARE(streamerPtr->GetSentSize(), size);

	QCOMPARE(statePtr->position, size);
	QCOMPARE(statePtr->abortCount, 0);
	QVERIFY(statePtr->maxRequested <= Streamer::s_maxPartSize);

	// The response is complete when it is sent to the network:
	QTRY_COMPARE_WITH_TIMEOUT(connection.GetWatcher().GetPhase(), Tracker::P_IDLE, 3000);
}


void CHttpResponseBodyStreamerTest::ChunkedBodyTest()
{
	CConnection connection;
	QVERIFY(connection.Open());

	const qint64 size = s_mebibyte + 17;

	StatePtr statePtr = CreateState(size);
	statePtr->partSize = 100000;

	// The size is not known
	std::unique_ptr<Streamer> streamerPtr = connection.CreateStreamer(statePtr, imtrest::CHttpSender::BF_CHUNKED, -1, s_mebibyte);
	QVERIFY(streamerPtr);

	QSignalSpy finishedSpy(streamerPtr.get(), &Streamer::Finished);
	streamerPtr->Start();

	QByteArray received;
	QTRY_COMPARE_WITH_TIMEOUT(finishedSpy.count(), 1, 10000);
	QVERIFY(IsSuccess(finishedSpy, 0));

	// The end of the chunked body arrives last:
	QTRY_VERIFY_WITH_TIMEOUT(DrainEndsWith(connection.GetClient(), received, "\r\n0\r\n\r\n"), 10000);

	QVERIFY(received.startsWith(s_head));

	QByteArray body;
	QList<qint64> sizes;
	QVERIFY(DecodeChunked(received.mid(s_head.size()), body, sizes));

	QCOMPARE(body, CreatePattern(0, size));

	for (qint64 chunkSize : sizes){
		QVERIFY(chunkSize <= 100000);
	}

	// The size of the body does not include the framing
	QCOMPARE(streamerPtr->GetSentSize(), size);
	QCOMPARE(statePtr->abortCount, 0);
}


void CHttpResponseBodyStreamerTest::UntilCloseBodyTest()
{
	CConnection connection;
	QVERIFY(connection.Open());

	// The end of the body is the closing of the connection: the watcher closes it when the response is complete
	connection.GetWatcher().CloseAfterResponse();

	const qint64 size = 300000;

	StatePtr statePtr = CreateState(size);

	std::unique_ptr<Streamer> streamerPtr = connection.CreateStreamer(statePtr, imtrest::CHttpSender::BF_UNTIL_CLOSE, -1, s_mebibyte);
	QVERIFY(streamerPtr);

	QSignalSpy finishedSpy(streamerPtr.get(), &Streamer::Finished);
	streamerPtr->Start();

	QByteArray received;
	QTRY_VERIFY_WITH_TIMEOUT(DrainUntilClosed(connection.GetClient(), received), 10000);

	// No framing: the body as it is
	QCOMPARE(received, s_head + CreatePattern(0, size));

	QCOMPARE(finishedSpy.count(), 1);
	QVERIFY(IsSuccess(finishedSpy, 0));
	QCOMPARE(statePtr->abortCount, 0);
}


void CHttpResponseBodyStreamerTest::NonPositiveWatermarkTest()
{
	CConnection connection;
	QVERIFY(connection.Open());

	const qint64 size = 500000;

	StatePtr statePtr = CreateState(size);

	// The default limit is used: the streamer does not wait for the room that does not exist
	std::unique_ptr<Streamer> streamerPtr = connection.CreateStreamer(statePtr, imtrest::CHttpSender::BF_CONTENT_LENGTH, size, 0);
	QVERIFY(streamerPtr);

	QSignalSpy finishedSpy(streamerPtr.get(), &Streamer::Finished);
	streamerPtr->Start();

	QByteArray received;
	QTRY_VERIFY_WITH_TIMEOUT(Drain(connection.GetClient(), received) >= s_head.size() + size, 10000);

	QCOMPARE(received, s_head + CreatePattern(0, size));
	QCOMPARE(finishedSpy.count(), 1);
	QVERIFY(IsSuccess(finishedSpy, 0));
}


void CHttpResponseBodyStreamerTest::SlowClientSlowsSourceDownTest()
{
	CConnection connection;
	QVERIFY(connection.Open());

	const qint64 size = 32 * s_mebibyte;
	const qint64 watermark = 65536;

	StatePtr statePtr = CreateState(size);
	statePtr->partSize = Streamer::s_maxPartSize;

	// The client does not read: its buffer is small, so the data stay in the network
	connection.GetClient().setReadBufferSize(65536);

	std::unique_ptr<Streamer> streamerPtr = connection.CreateStreamer(statePtr, imtrest::CHttpSender::BF_CONTENT_LENGTH, size, watermark);
	QVERIFY(streamerPtr);

	QSignalSpy finishedSpy(streamerPtr.get(), &Streamer::Finished);
	streamerPtr->Start();

	QTest::qWait(400);

	// The source is not read ahead of the network: the data are only in the buffers of the sockets, which are limited
	QVERIFY(statePtr->position > 0);
	QVERIFY2(statePtr->position < size / 2, qPrintable(QString::number(statePtr->position)));
	QCOMPARE(finishedSpy.count(), 0);

	// The memory of the server: the high watermark and one part with its framing
	QVERIFY(connection.GetServerSocket().bytesToWrite() <= watermark + Streamer::s_maxPartSize);

	// The client reads, so the rest comes
	connection.GetClient().setReadBufferSize(0);

	CPatternReader reader(connection.GetClient(), s_head.size());
	QTRY_VERIFY_WITH_TIMEOUT(reader.Drain() >= s_head.size() + size, 30000);

	QVERIFY(reader.IsOk());
	QCOMPARE(finishedSpy.count(), 1);
	QVERIFY(IsSuccess(finishedSpy, 0));
	QCOMPARE(statePtr->abortCount, 0);
}


void CHttpResponseBodyStreamerTest::WaitingSourceTest()
{
	CConnection connection;
	QVERIFY(connection.Open());

	StatePtr statePtr = CreateState(1000);
	statePtr->waitAt = 500;

	std::unique_ptr<Streamer> streamerPtr = connection.CreateStreamer(statePtr, imtrest::CHttpSender::BF_CONTENT_LENGTH, 1000, s_mebibyte);
	QVERIFY(streamerPtr);

	QSignalSpy finishedSpy(streamerPtr.get(), &Streamer::Finished);
	streamerPtr->Start();

	// The data of the source that are available are sent:
	QByteArray received;
	QTRY_VERIFY_WITH_TIMEOUT(Drain(connection.GetClient(), received) >= s_head.size() + 500, 5000);

	QTest::qWait(150);
	Drain(connection.GetClient(), received);

	// Nothing more is sent, the response is not complete, though all written data are sent to the network:
	QCOMPARE(received, s_head + CreatePattern(0, 500));
	QCOMPARE(finishedSpy.count(), 0);
	QCOMPARE(connection.GetWatcher().GetPhase(), Tracker::P_SENDING);
	QVERIFY(!streamerPtr->IsFinished());
	QVERIFY(statePtr->handler);

	// The source has the data
	statePtr->Release();

	QTRY_VERIFY_WITH_TIMEOUT(Drain(connection.GetClient(), received) >= s_head.size() + 1000, 5000);

	QCOMPARE(received, s_head + CreatePattern(0, 1000));
	QCOMPARE(finishedSpy.count(), 1);
	QVERIFY(IsSuccess(finishedSpy, 0));
	QCOMPARE(statePtr->abortCount, 0);

	QTRY_COMPARE_WITH_TIMEOUT(connection.GetWatcher().GetPhase(), Tracker::P_IDLE, 3000);
}


void CHttpResponseBodyStreamerTest::WaitingSourceIsAbortedWithStreamerTest()
{
	CConnection connection;
	QVERIFY(connection.Open());

	StatePtr statePtr = CreateState(1000);
	statePtr->waitAt = 0;

	std::unique_ptr<Streamer> streamerPtr = connection.CreateStreamer(statePtr, imtrest::CHttpSender::BF_CONTENT_LENGTH, 1000, s_mebibyte);
	QVERIFY(streamerPtr);

	streamerPtr->Start();
	QVERIFY(!streamerPtr->IsFinished());

	// The body is not finished: the source is told that it is not needed
	streamerPtr.reset();

	QCOMPARE(statePtr->abortCount, 1);

	// A late notification of the source does not reach the destroyed streamer
	statePtr->Release();
	QTest::qWait(50);
}


void CHttpResponseBodyStreamerTest::DeclaredSizeIsNotExceededTest()
{
	CConnection connection;
	QVERIFY(connection.Open());

	// The source has more than the declared size: the header Content-Length says 10, so the client takes 10 bytes
	StatePtr statePtr = CreateState(100);
	statePtr->partSize = 1000;

	std::unique_ptr<Streamer> streamerPtr = connection.CreateStreamer(statePtr, imtrest::CHttpSender::BF_CONTENT_LENGTH, 10, s_mebibyte);
	QVERIFY(streamerPtr);

	QSignalSpy finishedSpy(streamerPtr.get(), &Streamer::Finished);
	streamerPtr->Start();

	QByteArray received;
	QTRY_VERIFY_WITH_TIMEOUT(Drain(connection.GetClient(), received) >= s_head.size() + 10, 5000);

	QTest::qWait(100);
	Drain(connection.GetClient(), received);

	QCOMPARE(received, s_head + CreatePattern(0, 10));
	QCOMPARE(finishedSpy.count(), 1);
	QVERIFY(IsSuccess(finishedSpy, 0));

	// The source is never asked for more than the rest of the declared size
	QCOMPARE(statePtr->maxRequested, qint64(10));
	QCOMPARE(statePtr->position, qint64(10));
	QCOMPARE(statePtr->abortCount, 0);
}


void CHttpResponseBodyStreamerTest::SourceEndsBeforeDeclaredSizeTest()
{
	CConnection connection;
	QVERIFY(connection.Open());

	// The head says 1000 bytes, the source has 400: the client cannot be told, so the connection is broken
	StatePtr statePtr = CreateState(400);

	std::unique_ptr<Streamer> streamerPtr = connection.CreateStreamer(statePtr, imtrest::CHttpSender::BF_CONTENT_LENGTH, 1000, s_mebibyte);
	QVERIFY(streamerPtr);

	QSignalSpy finishedSpy(streamerPtr.get(), &Streamer::Finished);
	streamerPtr->Start();

	QTRY_COMPARE_WITH_TIMEOUT(finishedSpy.count(), 1, 5000);
	QVERIFY(!IsSuccess(finishedSpy, 0));

	QCOMPARE(statePtr->abortCount, 1);
	QVERIFY(streamerPtr->IsFinished());

	QTRY_COMPARE_WITH_TIMEOUT(connection.GetClient().state(), QAbstractSocket::UnconnectedState, 5000);

	// The client got less than the declared size
	QByteArray received = connection.GetClient().readAll();
	QVERIFY(received.size() < s_head.size() + 1000);
}


void CHttpResponseBodyStreamerTest::SourceGivesMoreThanRequestedTest()
{
	CConnection connection;
	QVERIFY(connection.Open());

	StatePtr statePtr = CreateState(1000);
	statePtr->partSize = 1000;
	statePtr->isIgnoringMaxSize = true;

	// The rest of the declared size is 100, the source gives 1000: the body would not match its length
	std::unique_ptr<Streamer> streamerPtr = connection.CreateStreamer(statePtr, imtrest::CHttpSender::BF_CONTENT_LENGTH, 100, s_mebibyte);
	QVERIFY(streamerPtr);

	QSignalSpy finishedSpy(streamerPtr.get(), &Streamer::Finished);
	streamerPtr->Start();

	QTRY_COMPARE_WITH_TIMEOUT(finishedSpy.count(), 1, 5000);
	QVERIFY(!IsSuccess(finishedSpy, 0));

	QCOMPARE(statePtr->abortCount, 1);
	QCOMPARE(streamerPtr->GetSentSize(), qint64(0));

	QTRY_COMPARE_WITH_TIMEOUT(connection.GetClient().state(), QAbstractSocket::UnconnectedState, 5000);
}


void CHttpResponseBodyStreamerTest::SourceErrorTest()
{
	CConnection connection;
	QVERIFY(connection.Open());

	StatePtr statePtr = CreateState(1000);
	statePtr->errorAt = 400;

	std::unique_ptr<Streamer> streamerPtr = connection.CreateStreamer(statePtr, imtrest::CHttpSender::BF_CHUNKED, -1, s_mebibyte);
	QVERIFY(streamerPtr);

	QSignalSpy finishedSpy(streamerPtr.get(), &Streamer::Finished);
	streamerPtr->Start();

	QTRY_COMPARE_WITH_TIMEOUT(finishedSpy.count(), 1, 5000);
	QVERIFY(!IsSuccess(finishedSpy, 0));

	// The response is started, so the only way to tell about the error is the broken connection. The end of the chunked body
	// is not sent: the client sees that the body is not complete.
	QCOMPARE(statePtr->abortCount, 1);
	QCOMPARE(streamerPtr->GetSentSize(), qint64(400));

	QTRY_COMPARE_WITH_TIMEOUT(connection.GetClient().state(), QAbstractSocket::UnconnectedState, 5000);

	const QByteArray received = connection.GetClient().readAll();
	QVERIFY(!received.endsWith("0\r\n\r\n"));
}


void CHttpResponseBodyStreamerTest::SourceGivesEmptyDataTest()
{
	CConnection connection;
	QVERIFY(connection.Open());

	StatePtr statePtr = CreateState(1000);
	statePtr->emptyDataAt = 300;

	// An empty chunk would finish the chunked body: the data of the source must not be empty
	std::unique_ptr<Streamer> streamerPtr = connection.CreateStreamer(statePtr, imtrest::CHttpSender::BF_CHUNKED, -1, s_mebibyte);
	QVERIFY(streamerPtr);

	QSignalSpy finishedSpy(streamerPtr.get(), &Streamer::Finished);
	streamerPtr->Start();

	QTRY_COMPARE_WITH_TIMEOUT(finishedSpy.count(), 1, 5000);
	QVERIFY(!IsSuccess(finishedSpy, 0));

	QCOMPARE(statePtr->abortCount, 1);
	QCOMPARE(streamerPtr->GetSentSize(), qint64(300));

	QTRY_COMPARE_WITH_TIMEOUT(connection.GetClient().state(), QAbstractSocket::UnconnectedState, 5000);
}


void CHttpResponseBodyStreamerTest::ClientDisconnectAbortsSourceTest()
{
	CConnection connection;
	QVERIFY(connection.Open());

	const qint64 size = 32 * s_mebibyte;

	StatePtr statePtr = CreateState(size);
	statePtr->partSize = Streamer::s_maxPartSize;

	connection.GetClient().setReadBufferSize(65536);

	std::unique_ptr<Streamer> streamerPtr = connection.CreateStreamer(statePtr, imtrest::CHttpSender::BF_CONTENT_LENGTH, size, 65536);
	QVERIFY(streamerPtr);

	QSignalSpy finishedSpy(streamerPtr.get(), &Streamer::Finished);
	streamerPtr->Start();

	QTest::qWait(200);
	QCOMPARE(finishedSpy.count(), 0);
	QCOMPARE(statePtr->abortCount, 0);

	// The client has gone in the middle of the response: the file or the request to another server is released at once
	connection.GetClient().abort();

	QTRY_COMPARE_WITH_TIMEOUT(finishedSpy.count(), 1, 5000);
	QVERIFY(!IsSuccess(finishedSpy, 0));

	QCOMPARE(statePtr->abortCount, 1);
	QVERIFY(statePtr->position < size);
	QVERIFY(streamerPtr->IsFinished());
}


void CHttpResponseBodyStreamerTest::ClosedSocketAtStartTest()
{
	CConnection connection;
	QVERIFY(connection.Open());

	StatePtr statePtr = CreateState(1000);

	imtrest::IResponseBodySourcePtr sourcePtr(new CTestSource(statePtr));

	connection.GetServerSocket().abort();

	// The disconnection is already past: no signal will tell about it
	Streamer streamer(connection.GetServerSocket(), connection.GetWatcher(), std::move(sourcePtr), imtrest::CHttpSender::BF_CONTENT_LENGTH, 1000, s_mebibyte);

	QSignalSpy finishedSpy(&streamer, &Streamer::Finished);
	streamer.Start();

	QCOMPARE(finishedSpy.count(), 1);
	QVERIFY(!IsSuccess(finishedSpy, 0));
	QCOMPARE(statePtr->abortCount, 1);
	QCOMPARE(statePtr->readCount, 0);
	QVERIFY(streamer.IsFinished());
}


void CHttpResponseBodyStreamerTest::StalledClientTest()
{
	// The client does not read the response for longer than the timeout: the watcher aborts the connection and the streamer
	// releases the source
	CConnection connection;
	QVERIFY(connection.Open(300));

	const qint64 size = 32 * s_mebibyte;

	StatePtr statePtr = CreateState(size);
	statePtr->partSize = Streamer::s_maxPartSize;

	connection.GetClient().setReadBufferSize(65536);

	std::unique_ptr<Streamer> streamerPtr = connection.CreateStreamer(statePtr, imtrest::CHttpSender::BF_CONTENT_LENGTH, size, 65536);
	QVERIFY(streamerPtr);

	QSignalSpy finishedSpy(streamerPtr.get(), &Streamer::Finished);
	streamerPtr->Start();

	QTRY_COMPARE_WITH_TIMEOUT(finishedSpy.count(), 1, 10000);
	QVERIFY(!IsSuccess(finishedSpy, 0));

	QCOMPARE(statePtr->abortCount, 1);
	QVERIFY(statePtr->position < size);
}


void CHttpResponseBodyStreamerTest::FinishedStreamerDoesNotAbortSourceTest()
{
	CConnection connection;
	QVERIFY(connection.Open());

	StatePtr statePtr = CreateState(100);

	std::unique_ptr<Streamer> streamerPtr = connection.CreateStreamer(statePtr, imtrest::CHttpSender::BF_CONTENT_LENGTH, 100, s_mebibyte);
	QVERIFY(streamerPtr);

	QSignalSpy finishedSpy(streamerPtr.get(), &Streamer::Finished);
	streamerPtr->Start();

	QCOMPARE(finishedSpy.count(), 1);
	QVERIFY(IsSuccess(finishedSpy, 0));

	// The body is completely read: destroying the streamer or closing the connection later is not an abort of the source
	streamerPtr.reset();
	connection.GetClient().abort();
	QTest::qWait(50);

	QCOMPARE(statePtr->abortCount, 0);
}


I_ADD_TEST(CHttpResponseBodyStreamerTest);
