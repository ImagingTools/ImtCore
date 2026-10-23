// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include "CClientControllerTest.h"


// Qt includes
#include <QtCore/QCryptographicHash>
#include <QtCore/QDir>
#include <QtCore/QElapsedTimer>
#include <QtCore/QFile>
#include <QtCore/QJsonArray>
#include <QtCore/QJsonDocument>
#include <QtCore/QJsonObject>
#include <QtNetwork/QTcpServer>

// ACF includes
#include <itest/CStandardTestExecutor.h>

// ImtCore includes
#include <imtrestdemo/CDemoProtocol.h>
#include <imtrestdemo/CWireLogModel.h>
#include <imtrestdemo/DemoTlsTools.h>


namespace
{


using imtrestdemo::CWireLogModel;
using imtrestdemo::DemoProtocol::CreatePatternData;


QByteArray CalculateSha256(const QByteArray& data)
{
	return QCryptographicHash::hash(data, QCryptographicHash::Sha256).toHex();
}


QJsonObject ParseJson(const QByteArray& data)
{
	return QJsonDocument::fromJson(data).object();
}


}


void CClientControllerTest::initTestCase()
{
	QVERIFY(m_directory.isValid());

	imtrestdemo::CDemoServerSettings settings;
	settings.filesDirectory = m_directory.path();
	settings.requestInactivityTimeoutMs = 1000;

	// The test certificates are shown by the settings of the server, the client takes them from there (the long answer of the server)
	QString samplesError;
	if (!imtrestdemo::DemoTlsTools::FindOpenssl().isEmpty() && imtrestdemo::DemoTlsTools::GenerateSamples(m_directory.path() + ".tls-presets", QString(), samplesError)){
		settings.tlsSamplesDirectory = m_directory.path() + ".tls-presets";
	}

	QVERIFY2(m_server.Start(settings), qPrintable(m_server.GetErrorString()));
}


void CClientControllerTest::cleanupTestCase()
{
	m_controllerPtr.reset();
	m_server.Stop();

	// The places of the test certificates are beside the directory of the files
	QDir(m_directory.path() + ".tls-presets").removeRecursively();
	QDir(m_directory.path() + ".tls").removeRecursively();
}


void CClientControllerTest::init()
{
	// A new client for every test: the logs are empty
	m_controllerPtr.reset(new imtrestdemo::CDemoClientController);
	m_controllerPtr->SetServerUrl(QString("http://localhost:%1").arg(m_server.GetPort()));
}


bool CClientControllerTest::WaitForExchange(int exchangeId, int timeoutMs)
{
	QElapsedTimer timer;
	timer.start();

	for (;;){
		const SDemoExchange* exchangePtr = FindExchange(exchangeId);
		if (exchangePtr != nullptr && exchangePtr->isFinished){
			return true;
		}

		if (timer.elapsed() > timeoutMs){
			return false;
		}

		QCoreApplication::processEvents(QEventLoop::WaitForMoreEvents, 20);
	}
}


const SDemoExchange* CClientControllerTest::FindExchange(int exchangeId) const
{
	return m_controllerPtr->GetExchanges()->FindExchange(exchangeId);
}


int CClientControllerTest::FindWireEntry(int kind, const QString& text, int fromRow) const
{
	const CWireLogModel* wirePtr = m_controllerPtr->GetWire();

	for (int row = fromRow; row < wirePtr->rowCount(); ++row){
		const QModelIndex index = wirePtr->index(row);
		if (wirePtr->data(index, CWireLogModel::R_KIND).toInt() == kind && wirePtr->data(index, CWireLogModel::R_TEXT).toString().contains(text, Qt::CaseInsensitive)){
			return row;
		}
	}

	return -1;
}


QString CClientControllerTest::GetWireText(int row) const
{
	const CWireLogModel* wirePtr = m_controllerPtr->GetWire();

	return wirePtr->data(wirePtr->index(row), CWireLogModel::R_TEXT).toString();
}


void CClientControllerTest::ConnectionTest()
{
	QVERIFY(!m_controllerPtr->IsBusy());

	QSignalSpy spy(m_controllerPtr.get(), &imtrestdemo::CDemoClientController::exchangeFinished);
	m_controllerPtr->testConnection();
	QVERIFY(m_controllerPtr->IsBusy());
	QVERIFY(spy.wait(10000));

	QVERIFY(!m_controllerPtr->IsBusy());
	QVERIFY2(m_controllerPtr->GetStatusText().startsWith("Connected"), qPrintable(m_controllerPtr->GetStatusText()));
	QVERIFY(m_controllerPtr->GetServerInfo().contains("threadsLimit"));
}


void CClientControllerTest::WireShowsTheRequestAndTheResponseTest()
{
	QVariantMap options;
	options.insert("method", "GET");
	options.insert("path", "/echo?x=1");
	options.insert("headers", "X-Debug: preview\nAccept: application/json");

	const int id = m_controllerPtr->sendRequest(options);
	QVERIFY(id > 0);
	QVERIFY(WaitForExchange(id));

	// The bytes that the client library has really sent: the request line, the headers of the application and of the library
	const int requestRow = FindWireEntry(CWireLogModel::K_CLIENT_TO_SERVER, "GET /echo?x=1 HTTP/1.1");
	QVERIFY(requestRow >= 0);
	QVERIFY(GetWireText(requestRow).contains("X-Debug: preview", Qt::CaseInsensitive));
	QVERIFY(GetWireText(requestRow).contains("Host: 127.0.0.1:", Qt::CaseInsensitive));

	// The head of the response and its body, the line breaks are shown
	const int responseRow = FindWireEntry(CWireLogModel::K_SERVER_TO_CLIENT, "HTTP/1.1 200 OK");
	QVERIFY(responseRow > requestRow);
	QVERIFY(GetWireText(responseRow).contains("Content-Type: application/json", Qt::CaseInsensitive));
	QVERIFY(GetWireText(responseRow).contains("\\r\\n", Qt::CaseInsensitive));
	QVERIFY(GetWireText(responseRow).contains("\"x\": \"1\""));

	// The events of the connection
	QVERIFY(FindWireEntry(CWireLogModel::K_EVENT, "is opened") >= 0);
}


void CClientControllerTest::ExchangeShowsTheApplicationViewTest()
{
	QVariantMap options;
	options.insert("method", "POST");
	options.insert("path", "/echo?a=b");
	options.insert("headers", "X-One: 1");
	options.insert("bodyType", "text");
	options.insert("bodyText", "text of the body");

	const int id = m_controllerPtr->sendRequest(options);
	QVERIFY(id > 0);
	QVERIFY(WaitForExchange(id));

	const SDemoExchange* exchangePtr = FindExchange(id);
	QVERIFY(exchangePtr != nullptr);
	QCOMPARE(exchangePtr->statusCode, 200);
	QCOMPARE(exchangePtr->reasonPhrase, QByteArray("OK"));
	QCOMPARE(exchangePtr->method, QByteArray("POST"));

	// The application sees the address of the server, not of the proxy
	QCOMPARE(exchangePtr->url, QString("http://localhost:%1/echo?a=b").arg(m_server.GetPort()));
	QCOMPARE(exchangePtr->requestBodySize, qint64(16));
	QVERIFY(exchangePtr->errorText.isEmpty());

	const QString requestText = m_controllerPtr->GetExchanges()->requestText(0);
	QVERIFY(requestText.startsWith("POST /echo?a=b HTTP/1.1"));
	QVERIFY(requestText.contains("X-One: 1"));
	QVERIFY(requestText.contains("text of the body"));

	const QString responseText = m_controllerPtr->GetExchanges()->responseText(0);
	QVERIFY(responseText.startsWith("HTTP 200 OK"));
	QVERIFY(responseText.contains("\"body\": \"text of the body\""));

	QVERIFY(m_controllerPtr->GetExchanges()->summaryText(0).contains("SHA-256"));
}


void CClientControllerTest::UploadGeneratedTest()
{
	const qint64 size = 3 * 1024 * 1024;

	QVariantMap options;
	options.insert("method", "PUT");
	options.insert("path", "/upload/controller.bin");
	options.insert("useToken", true);
	options.insert("bodyType", "generated");
	options.insert("bodySize", size);

	const int id = m_controllerPtr->sendRequest(options);
	QVERIFY(id > 0);
	QVERIFY(WaitForExchange(id));

	const SDemoExchange* exchangePtr = FindExchange(id);
	QCOMPARE(exchangePtr->statusCode, 201);
	QCOMPARE(ParseJson(exchangePtr->responsePreview).value("sha256").toString().toLatin1(), CalculateSha256(CreatePatternData(0, size)));

	// The request on the wire has the length of the body, and it is a few lines of the preview, not 3 MiB of the text
	const int requestRow = FindWireEntry(CWireLogModel::K_CLIENT_TO_SERVER, "PUT /upload/controller.bin HTTP/1.1");
	QVERIFY(requestRow >= 0);
	QVERIFY(GetWireText(requestRow).contains("Content-Length: 3145728", Qt::CaseInsensitive));
	QVERIFY(GetWireText(requestRow).contains("Authorization: Bearer demo-token", Qt::CaseInsensitive));
	QVERIFY(GetWireText(requestRow).contains("more bytes are not shown", Qt::CaseInsensitive));

	const QModelIndex index = m_controllerPtr->GetWire()->index(requestRow);
	QVERIFY(m_controllerPtr->GetWire()->data(index, CWireLogModel::R_SIZE).toLongLong() > size);
	QVERIFY(m_controllerPtr->GetWire()->data(index, CWireLogModel::R_IS_TRUNCATED).toBool());
}


void CClientControllerTest::UploadChunkedTest()
{
	const qint64 size = 500000;

	QVariantMap options;
	options.insert("method", "PUT");
	options.insert("path", "/upload/controller-chunked.bin");
	options.insert("useToken", true);
	options.insert("bodyType", "generated");
	options.insert("bodySize", size);
	options.insert("isChunked", true);

	const int id = m_controllerPtr->sendRequest(options);
	QVERIFY(id > 0);
	QVERIFY(WaitForExchange(id));
	QCOMPARE(FindExchange(id)->statusCode, 201);
	QCOMPARE(FindExchange(id)->requestSentSize, size);
	QCOMPARE(ParseJson(FindExchange(id)->responsePreview).value("sha256").toString().toLatin1(), CalculateSha256(CreatePatternData(0, size)));

	// The size is not known in advance: the framing of the chunks is seen on the wire
	const int requestRow = FindWireEntry(CWireLogModel::K_CLIENT_TO_SERVER, "PUT /upload/controller-chunked.bin");
	QVERIFY(requestRow >= 0);
	QVERIFY2(GetWireText(requestRow).contains("Transfer-Encoding: chunked", Qt::CaseInsensitive), qPrintable(GetWireText(requestRow).left(600)));
	QVERIFY(!GetWireText(requestRow).contains("Content-Length", Qt::CaseInsensitive));
}


void CClientControllerTest::UploadFileTest()
{
	const QByteArray content = CreatePatternData(11, 1024 * 1024 + 7);
	const QString path = m_directory.filePath("body-source.bin");

	QFile file(path);
	QVERIFY(file.open(QIODevice::WriteOnly));
	file.write(content);
	file.close();

	QVariantMap options;
	options.insert("method", "PUT");
	options.insert("path", "/upload/controller-file.bin");
	options.insert("useToken", true);
	options.insert("bodyType", "file");
	options.insert("bodyFile", path);

	const int id = m_controllerPtr->sendRequest(options);
	QVERIFY(id > 0);
	QVERIFY(WaitForExchange(id));
	QCOMPARE(FindExchange(id)->statusCode, 201);
	QCOMPARE(FindExchange(id)->requestBodySize, qint64(content.size()));
	QCOMPARE(ParseJson(FindExchange(id)->responsePreview).value("sha256").toString().toLatin1(), CalculateSha256(content));
}


void CClientControllerTest::ExpectContinueTest()
{
	QVariantMap options;
	options.insert("method", "PUT");
	options.insert("path", "/upload/controller-expect.bin");
	options.insert("useToken", true);
	options.insert("expectContinue", true);
	options.insert("bodyType", "generated");
	options.insert("bodySize", 20000);

	const int id = m_controllerPtr->sendRequest(options);
	QVERIFY(id > 0);
	QVERIFY(WaitForExchange(id));
	QCOMPARE(FindExchange(id)->statusCode, 201);

	// The client library sends the head together with the body, so the server does not answer with the interim response
	// (RFC 9110, section 10.1.1: the client that has sent the body does not wait). The expectation itself is on the wire.
	QVERIFY(FindWireEntry(CWireLogModel::K_CLIENT_TO_SERVER, "Expect: 100-continue") >= 0);
}


void CClientControllerTest::RawExpectContinueTest()
{
	// The head is sent first and the body 300 ms later: the server must answer with 100 in the meantime
	const QString head = "PUT /upload/raw-expect.bin HTTP/1.1\nHost: localhost\nAuthorization: Bearer demo-token\nContent-Length: 5\nExpect: 100-continue\nConnection: close\n\n";

	QVariantMap options;
	options.insert("text", head + "hello");
	options.insert("partSize", int(head.toUtf8().size() + head.count('\n')));
	options.insert("delayMs", 300);

	const int id = m_controllerPtr->sendRaw(options);
	QVERIFY(id > 0);
	QVERIFY(WaitForExchange(id));
	QCOMPARE(FindExchange(id)->statusCode, 201);

	const int requestRow = FindWireEntry(CWireLogModel::K_CLIENT_TO_SERVER, "Expect: 100-continue");
	QVERIFY(requestRow >= 0);
	QVERIFY(FindWireEntry(CWireLogModel::K_SERVER_TO_CLIENT, "HTTP/1.1 100 Continue", requestRow) > requestRow);
}


void CClientControllerTest::DownloadCheckPatternTest()
{
	QVariantMap options;
	options.insert("method", "GET");
	options.insert("path", "/stream/2000000?chunked=1&part=70000");
	options.insert("checkPattern", true);

	const int id = m_controllerPtr->sendRequest(options);
	QVERIFY(id > 0);
	QVERIFY(WaitForExchange(id));

	const SDemoExchange* exchangePtr = FindExchange(id);
	QCOMPARE(exchangePtr->statusCode, 200);
	QCOMPARE(exchangePtr->responseSize, qint64(2000000));
	QVERIFY2(exchangePtr->verificationText.contains("the pattern of the demo server is right"), qPrintable(exchangePtr->verificationText));
	QVERIFY(exchangePtr->verificationText.contains(QString::fromLatin1(CalculateSha256(CreatePatternData(0, 2000000)))));

	// The body of the response is not kept: only the beginning is in the preview
	QCOMPARE(exchangePtr->responsePreview.size(), qsizetype(imtrestdemo::CTrafficLogModel::s_previewLimit));

	// The chunks of the response are on the wire
	QVERIFY(FindWireEntry(CWireLogModel::K_SERVER_TO_CLIENT, "Transfer-Encoding: chunked") >= 0);
}


void CClientControllerTest::DownloadRangeTest()
{
	QVariantMap options;
	options.insert("method", "GET");
	options.insert("path", "/files/pattern.bin");
	options.insert("headers", "Range: bytes=100000-199999");
	options.insert("checkPattern", true);
	options.insert("patternOffset", 100000);

	const int id = m_controllerPtr->sendRequest(options);
	QVERIFY(id > 0);
	QVERIFY(WaitForExchange(id));

	const SDemoExchange* exchangePtr = FindExchange(id);
	QCOMPARE(exchangePtr->statusCode, 206);
	QCOMPARE(exchangePtr->responseSize, qint64(100000));
	QVERIFY2(exchangePtr->verificationText.contains("pattern of the demo server is right"), qPrintable(exchangePtr->verificationText));

	bool isRangeFound = false;
	for (const QPair<QByteArray, QByteArray>& header : exchangePtr->responseHeaders){
		if (header.first.toLower() == "content-range"){
			isRangeFound = true;

			QCOMPARE(header.second, QByteArray("bytes 100000-199999/1048589"));
		}
	}

	QVERIFY(isRangeFound);
}


void CClientControllerTest::PatternMismatchTest()
{
	QVariantMap options;
	options.insert("method", "GET");
	options.insert("path", "/files/hello.txt");
	options.insert("checkPattern", true);

	const int id = m_controllerPtr->sendRequest(options);
	QVERIFY(id > 0);
	QVERIFY(WaitForExchange(id));

	// The check must find a body that is not the pattern
	QVERIFY2(FindExchange(id)->verificationText.contains("DIFFERS at the byte"), qPrintable(FindExchange(id)->verificationText));
}


void CClientControllerTest::SaveToFileTest()
{
	const QString path = m_directory.filePath("saved.bin");

	QVariantMap options;
	options.insert("method", "GET");
	options.insert("path", "/files/pattern.bin");
	options.insert("saveTo", path);

	const int id = m_controllerPtr->sendRequest(options);
	QVERIFY(id > 0);
	QVERIFY(WaitForExchange(id));
	QCOMPARE(FindExchange(id)->statusCode, 200);

	QFile file(path);
	QVERIFY(file.open(QIODevice::ReadOnly));
	QVERIFY(file.readAll() == CreatePatternData(0, imtrestdemo::DemoProtocol::s_patternFileSize));
}


void CClientControllerTest::ErrorStatusTest()
{
	QVariantMap options;
	options.insert("method", "GET");
	options.insert("path", "/files/nothing.txt");

	const int id = m_controllerPtr->sendRequest(options);
	QVERIFY(id > 0);
	QVERIFY(WaitForExchange(id));

	// 404 is an answer of the server: it is shown as the status, not as an error of the client
	QCOMPARE(FindExchange(id)->statusCode, 404);
	QVERIFY(FindExchange(id)->errorText.isEmpty());

	const QModelIndex index = m_controllerPtr->GetExchanges()->index(0);
	QVERIFY(m_controllerPtr->GetExchanges()->data(index, imtrestdemo::CTrafficLogModel::R_HAS_ERROR).toBool());
}


void CClientControllerTest::RedirectTest()
{
	QVariantMap options;
	options.insert("method", "GET");
	options.insert("path", "/behavior/status/302");

	const int manualId = m_controllerPtr->sendRequest(options);
	QVERIFY(WaitForExchange(manualId));
	QCOMPARE(FindExchange(manualId)->statusCode, 302);

	options.insert("followRedirects", true);

	const int followId = m_controllerPtr->sendRequest(options);
	QVERIFY(WaitForExchange(followId));
	QCOMPARE(FindExchange(followId)->statusCode, 200);
}


void CClientControllerTest::RawRequestInPartsTest()
{
	QVariantMap options;
	options.insert("text", "GET /echo?raw=1 HTTP/1.1\nHost: localhost\nX-Raw: yes\nConnection: close\n\n");
	options.insert("autoCrlf", true);
	options.insert("partSize", 7);
	options.insert("delayMs", 15);

	const int id = m_controllerPtr->sendRaw(options);
	QVERIFY(id > 0);
	QVERIFY(WaitForExchange(id));

	const SDemoExchange* exchangePtr = FindExchange(id);
	QCOMPARE(exchangePtr->method, QByteArray("RAW"));
	QCOMPARE(exchangePtr->statusCode, 200);
	QVERIFY(exchangePtr->requestSentSize == exchangePtr->requestBodySize);
	QVERIFY(exchangePtr->responsePreview.contains("\"raw\": \"yes\"") || exchangePtr->responsePreview.contains("\"x-raw\": \"yes\""));

	// The server has put the parts together
	QVERIFY(FindWireEntry(CWireLogModel::K_CLIENT_TO_SERVER, "X-Raw: yes") >= 0);
	QVERIFY(FindWireEntry(CWireLogModel::K_SERVER_TO_CLIENT, "HTTP/1.1 200 OK") >= 0);
}


void CClientControllerTest::RawBadRequestTest()
{
	QVariantMap options;
	options.insert("text", "this is not a request\n\n");
	options.insert("idleTimeoutMs", 500);

	const int id = m_controllerPtr->sendRaw(options);
	QVERIFY(id > 0);
	QVERIFY(WaitForExchange(id));
	QCOMPARE(FindExchange(id)->statusCode, 400);
}


void CClientControllerTest::RawIncompleteRequestTest()
{
	// The head is not finished: the server must answer with 408 after its timeout (1 s in this test)
	QVariantMap options;
	options.insert("text", "GET /echo HTTP/1.1\nHost: localhost\nX-Unfinished: ");
	options.insert("idleTimeoutMs", 5000);

	const int id = m_controllerPtr->sendRaw(options);
	QVERIFY(id > 0);
	QVERIFY(WaitForExchange(id, 10000));
	QCOMPARE(FindExchange(id)->statusCode, 408);
}


void CClientControllerTest::CaptureDisabledTest()
{
	m_controllerPtr->SetCaptureEnabled(false);

	QVariantMap options;
	options.insert("method", "GET");
	options.insert("path", "/echo");

	const int id = m_controllerPtr->sendRequest(options);
	QVERIFY(id > 0);
	QVERIFY(WaitForExchange(id));
	QCOMPARE(FindExchange(id)->statusCode, 200);

	// Without the proxy the client talks to the server directly: the wire log is empty, the exchanges are still logged
	QCOMPARE(m_controllerPtr->GetWire()->rowCount(), 0);
}


void CClientControllerTest::ServerIsNotAvailableTest()
{
	// A port where nobody listens
	QTcpServer probe;
	QVERIFY(probe.listen(QHostAddress::LocalHost, 0));
	const int port = probe.serverPort();
	probe.close();

	m_controllerPtr->SetServerUrl(QString("http://localhost:%1").arg(port));

	QVariantMap options;
	options.insert("method", "GET");
	options.insert("path", "/echo");

	const int id = m_controllerPtr->sendRequest(options);
	QVERIFY(id > 0);
	QVERIFY(WaitForExchange(id));

	// The client has no answer: the error is shown (the proxy closes the connection, as the server is not reachable)
	QCOMPARE(FindExchange(id)->statusCode, 0);
	QVERIFY(!FindExchange(id)->errorText.isEmpty());
}


void CClientControllerTest::CurlCommandTest()
{
	QVariantMap options;
	options.insert("method", "put");
	options.insert("path", "/upload/it's.bin");
	options.insert("headers", "X-A: b");
	options.insert("useToken", true);
	options.insert("bodyType", "text");
	options.insert("bodyText", "it's text");

	const QString command = m_controllerPtr->makeCurlCommand(options);

	QVERIFY(command.startsWith("curl -v"));
	QVERIFY(command.contains("-X PUT"));
	QVERIFY(command.contains(QString("'http://localhost:%1/upload/it's.bin'").arg(m_server.GetPort())) || command.contains("it%27s.bin") || command.contains("it'\\''s.bin"));
	QVERIFY(command.contains("-H 'X-A: b'"));
	QVERIFY(command.contains("-H 'Authorization: Bearer demo-token'"));
	QVERIFY(command.contains("--data-binary 'it'\\''s text'"));
}


void CClientControllerTest::DecodeRawTextTest_data()
{
	QTest::addColumn<QString>("text");
	QTest::addColumn<bool>("isAutoCrlf");
	QTest::addColumn<QByteArray>("expected");

	QTest::newRow("line breaks are CRLF") << QString("a\nb") << true << QByteArray("a\r\nb");
	QTest::newRow("CR is dropped with the automatic CRLF") << QString("a\r\nb") << true << QByteArray("a\r\nb");
	QTest::newRow("line breaks are kept") << QString("a\nb") << false << QByteArray("a\nb");
	QTest::newRow("escapes") << QString("a\\r\\nb\\tc\\\\d") << false << QByteArray("a\r\nb\tc\\d");
	QTest::newRow("hexadecimal") << QString("\\x00\\x41\\xff") << false << QByteArray("\0A\xff", 3);
	QTest::newRow("a wrong escape is text") << QString("\\q\\xZZ") << false << QByteArray("\\q\\xZZ");
	QTest::newRow("unicode") << QString::fromUtf8("\xD0\xB8") << false << QByteArray("\xD0\xB8");
}


void CClientControllerTest::DecodeRawTextTest()
{
	QFETCH(QString, text);
	QFETCH(bool, isAutoCrlf);
	QFETCH(QByteArray, expected);

	QCOMPARE(imtrestdemo::CDemoClientController::DecodeRawText(text, isAutoCrlf), expected);
}


void CClientControllerTest::ParseHeadersTest()
{
	const SDemoExchange::Headers headers = imtrestdemo::CDemoClientController::ParseHeaders("A: 1\n  B :  two words \nno colon\n: no name\nC:\nD: x:y\n");

	QCOMPARE(headers.size(), 4);
	QCOMPARE(headers[0].first, QByteArray("A"));
	QCOMPARE(headers[0].second, QByteArray("1"));
	QCOMPARE(headers[1].first, QByteArray("B"));
	QCOMPARE(headers[1].second, QByteArray("two words"));
	QCOMPARE(headers[2].first, QByteArray("C"));
	QCOMPARE(headers[2].second, QByteArray(""));
	QCOMPARE(headers[3].second, QByteArray("x:y"));
}


void CClientControllerTest::EscapeBytesTest()
{
	QCOMPARE(CWireLogModel::EscapeBytes("GET / HTTP/1.1\r\nHost: x\r\n\r\n"), QString("GET / HTTP/1.1\\r\\n\nHost: x\\r\\n\n\\r\\n\n"));
	QCOMPARE(CWireLogModel::EscapeBytes(QByteArray("\0\x01\xff\t\\", 5)), QString("\\x00\\x01\\xff\\t\\\\"));
	QCOMPARE(CWireLogModel::EscapeBytes(QByteArray()), QString());
}


void CClientControllerTest::WireModelMergesTheDirectionTest()
{
	CWireLogModel model;

	model.AddData(1, CWireLogModel::K_CLIENT_TO_SERVER, "abc");
	model.AddData(1, CWireLogModel::K_CLIENT_TO_SERVER, "def");
	model.AddData(1, CWireLogModel::K_SERVER_TO_CLIENT, "ghi");
	model.AddData(2, CWireLogModel::K_SERVER_TO_CLIENT, "jkl");
	model.AddEvent(2, "closed");
	model.AddData(1, CWireLogModel::K_SERVER_TO_CLIENT, QByteArray());

	// The same direction of the same connection is one entry; the change of the direction or of the connection is a new one
	QCOMPARE(model.rowCount(), 4);
	QCOMPARE(model.data(model.index(0), CWireLogModel::R_TEXT).toString(), QString("abcdef"));
	QCOMPARE(model.data(model.index(0), CWireLogModel::R_SIZE).toLongLong(), qint64(6));

	// A big transfer keeps the beginning only and counts the rest
	CWireLogModel bigModel;
	const QByteArray part(10000, 'x');
	for (int i = 0; i < 5; ++i){
		bigModel.AddData(1, CWireLogModel::K_SERVER_TO_CLIENT, part);
	}

	QCOMPARE(bigModel.rowCount(), 1);
	QCOMPARE(bigModel.data(bigModel.index(0), CWireLogModel::R_SIZE).toLongLong(), qint64(50000));
	QVERIFY(bigModel.data(bigModel.index(0), CWireLogModel::R_IS_TRUNCATED).toBool());
	QVERIFY(bigModel.data(bigModel.index(0), CWireLogModel::R_TEXT).toString().size() < 17000);
	QVERIFY(bigModel.data(bigModel.index(0), CWireLogModel::R_TEXT).toString().contains("more bytes are not shown"));

	bigModel.clear();
	QCOMPARE(bigModel.rowCount(), 0);
}


void CClientControllerTest::ServerSettingsAreChangedByTheClientTest()
{
	auto waitFor = [](const std::function<bool()>& isReady, int timeoutMs = 15000){
		QElapsedTimer timer;
		timer.start();

		while (!isReady()){
			if (timer.elapsed() > timeoutMs){
				return false;
			}

			QCoreApplication::processEvents(QEventLoop::WaitForMoreEvents, 20);
		}

		return true;
	};

	imtrestdemo::CDemoClientController& controller = *m_controllerPtr;
	const int basePort = m_server.GetPort();

	// The settings of the server are read with the token
	controller.loadServerSettings();
	QVERIFY(waitFor([&controller](){
		return !controller.GetServerSettings().isEmpty();
	}));
	QCOMPARE(controller.GetServerSettings().value("port").toInt(), basePort);
	QCOMPARE(controller.GetServerSettings().value("keepAliveTimeoutMs").toInt(), 15000);

	// The request and the response are in the debug preview like the others
	const SDemoExchange* exchangePtr = FindExchange(1);
	QVERIFY(exchangePtr != nullptr);
	QVERIFY(exchangePtr->url.contains("/admin/settings"));
	QCOMPARE(exchangePtr->statusCode, 200);

	// A wrong value is refused by the server, the reason is shown
	QVariantMap wrongChanges;
	wrongChanges.insert("threadsLimit", 0);
	controller.applyServerSettings(wrongChanges);
	QVERIFY(waitFor([&controller](){
		return controller.GetSettingsStatus().contains("threadsLimit");
	}));
	QCOMPARE(controller.GetServerSettings().value("restartCount").toInt(), 0);

	// The port and the token are changed: the client follows the server
	QTcpServer probe;
	QVERIFY(probe.listen(QHostAddress::Any, 0));
	const int newPort = probe.serverPort();
	probe.close();

	QVariantMap changes;
	changes.insert("keepAliveTimeoutMs", 4321);
	changes.insert("port", newPort);
	changes.insert("token", "client-token");
	controller.applyServerSettings(changes);

	QVERIFY(waitFor([&controller, newPort](){
		const QVariantMap settings = controller.GetServerSettings();

		return settings.value("port").toInt() == newPort && settings.value("keepAliveTimeoutMs").toInt() == 4321
					&& controller.GetServerUrl().endsWith(QString(":%1").arg(newPort));
	}));
	QCOMPARE(controller.GetToken(), QString("client-token"));
	QVERIFY(controller.GetSettingsStatus().contains("applied"));

	// The server that cannot start with the settings goes back to the previous ones, the client stays with them
	QTcpServer busyServer;
	QVERIFY(busyServer.listen(QHostAddress::Any, 0));
	const int busyPort = busyServer.serverPort();

	QVariantMap busyChanges;
	busyChanges.insert("port", busyPort);
	controller.applyServerSettings(busyChanges);

	QVERIFY(waitFor([&controller, busyPort](){
		return controller.GetSettingsStatus().contains(QString::number(busyPort));
	}));
	QVERIFY(controller.GetServerUrl().endsWith(QString(":%1").arg(newPort)));
	QCOMPARE(controller.GetToken(), QString("client-token"));
	QCOMPARE(controller.GetServerSettings().value("port").toInt(), newPort);
	QVERIFY(!controller.GetServerSettings().value("lastError").toString().isEmpty());

	// Back to the settings that the other tests expect
	QVariantMap backChanges;
	backChanges.insert("keepAliveTimeoutMs", 15000);
	backChanges.insert("port", basePort);
	backChanges.insert("token", "demo-token");
	controller.applyServerSettings(backChanges);

	QVERIFY(waitFor([&controller, basePort](){
		return controller.GetServerSettings().value("port").toInt() == basePort && controller.GetServerUrl().endsWith(QString(":%1").arg(basePort))
					&& controller.GetServerSettings().value("lastError").toString().isEmpty();
	}));
	QCOMPARE(controller.GetToken(), QString("demo-token"));
}


void CClientControllerTest::TlsIsSetUpFromTheClientTest()
{
	if (imtrestdemo::DemoTlsTools::FindOpenssl().isEmpty()){
		QSKIP("The program openssl is not found (IMTREST_OPENSSL or PATH)");
	}

	auto waitFor = [](const std::function<bool()>& isReady, int timeoutMs = 20000){
		QElapsedTimer timer;
		timer.start();

		while (!isReady()){
			if (timer.elapsed() > timeoutMs){
				return false;
			}

			QCoreApplication::processEvents(QEventLoop::WaitForMoreEvents, 20);
		}

		return true;
	};

	imtrestdemo::CDemoClientController& controller = *m_controllerPtr;
	const QString httpUrl = controller.GetServerUrl();

	controller.loadServerSettings();
	QVERIFY(waitFor([&controller](){
		return !controller.GetServerSettings().isEmpty();
	}));

	// The sets of the certificates are in the settings of the server, the answer is longer than the preview of the log
	const QJsonArray samples = QJsonArray::fromVariantList(controller.GetServerSettings().value("tlsSamples").toList());
	QCOMPARE(samples.size(), 6);
	QVERIFY(controller.GetExchanges()->FindExchange(1)->responseSize > imtrestdemo::CTrafficLogModel::s_previewLimit);

	// The trust is set by the application: the files of the set, nothing is added to the system
	for (const QString& id : {"single", "chain", "several", "bundle", "ec"}){
		QJsonObject sample;
		for (const QJsonValue& value : samples){
			if (value.toObject().value("id").toString() == id){
				sample = value.toObject();
			}
		}

		QVERIFY(!sample.isEmpty());

		QStringList trusted;
		for (const QJsonValue& file : sample.value("trustedFiles").toArray()){
			trusted << file.toString();
		}

		QVariantMap changes;
		changes.insert("tlsEnabled", true);
		changes.insert("tlsCertificateFile", sample.value("certificateFile").toString());
		changes.insert("tlsChainFiles", sample.value("chainFiles").toArray().toVariantList());
		changes.insert("tlsKeyFile", sample.value("keyFile").toString());

		// The client trusts the old and the new certificates while the server restarts, then only the new ones
		controller.applyTlsSettings(changes, trusted);

		// The client follows the server to https
		QVERIFY2(waitFor([&controller, &sample, &trusted](){
			return controller.GetServerUrl().startsWith("https://") && controller.GetServerSettings().value("tlsCertificateFile").toString() == sample.value("certificateFile").toString()
						&& controller.GetSettingsStatus().contains("applied") && controller.GetTrustedCertificates() == trusted;
		}), qPrintable(id + ": " + controller.GetSettingsStatus()));

		QVariantMap options;
		options.insert("method", "GET");
		options.insert("path", "/echo");
		const int exchangeId = controller.sendRequest(options);
		QVERIFY(WaitForExchange(exchangeId));

		const SDemoExchange* exchangePtr = FindExchange(exchangeId);
		QVERIFY(exchangePtr != nullptr);
		QVERIFY2(exchangePtr->statusCode == 200, qPrintable(id + ": " + exchangePtr->errorText));
		QVERIFY2(exchangePtr->tlsText.contains(QString("certificates from the server: %1").arg(sample.value("certificatesInChain").toInt())), qPrintable(exchangePtr->tlsText));

		bool isTlsShown = false;
		for (int row = 0; row < controller.GetExchanges()->rowCount(); ++row){
			isTlsShown = isTlsShown || controller.GetExchanges()->summaryText(row).contains("TLS: ");
		}

		QVERIFY(isTlsShown);

		// Without the trust the same request is refused, the reason is shown
		controller.SetTrustedCertificates({});
		const int refusedId = controller.sendRequest(options);
		QVERIFY(WaitForExchange(refusedId));
		QCOMPARE(FindExchange(refusedId)->statusCode, 0);
		QVERIFY(!FindExchange(refusedId)->errorText.isEmpty());
		controller.SetTrustedCertificates(trusted);
	}

	// Back to http: the client follows
	QVariantMap off;
	off.insert("tlsEnabled", false);
	controller.applyTlsSettings(off, {});
	QVERIFY(waitFor([&controller, &httpUrl](){
		return controller.GetServerUrl() == httpUrl && !controller.GetServerSettings().value("tlsEnabled").toBool() && controller.GetSettingsStatus().contains("applied");
	}));

	QVariantMap options;
	options.insert("method", "GET");
	options.insert("path", "/echo");
	const int plainId = controller.sendRequest(options);
	QVERIFY(WaitForExchange(plainId));
	QCOMPARE(FindExchange(plainId)->statusCode, 200);
	QVERIFY(FindExchange(plainId)->tlsText.isEmpty());

	// The upload in chunks and the raw bytes need a connection without TLS: the client says so, when the server is https
	QVariantMap rawOptions;
	rawOptions.insert("text", "GET /echo HTTP/1.1\nHost: localhost\nConnection: close\n\n");
	controller.SetServerUrl(QString(httpUrl).replace("http://", "https://"));
	const int exchangeCount = controller.GetExchanges()->rowCount();
	QVERIFY(controller.sendRaw(rawOptions) < 0);
	QCOMPARE(controller.GetExchanges()->rowCount(), exchangeCount + 1);
	controller.SetServerUrl(httpUrl);
}


void CClientControllerTest::TlsPresetAndFilesFromTheClientTest()
{
	if (imtrestdemo::DemoTlsTools::FindOpenssl().isEmpty()){
		QSKIP("The program openssl is not found (IMTREST_OPENSSL or PATH)");
	}

	auto waitFor = [](const std::function<bool()>& isReady, int timeoutMs = 20000){
		QElapsedTimer timer;
		timer.start();

		while (!isReady()){
			if (timer.elapsed() > timeoutMs){
				return false;
			}

			QCoreApplication::processEvents(QEventLoop::WaitForMoreEvents, 20);
		}

		return true;
	};

	imtrestdemo::CDemoClientController& controller = *m_controllerPtr;
	const QString httpUrl = controller.GetServerUrl();

	controller.loadServerSettings();
	QVERIFY(waitFor([&controller](){
		return !controller.GetServerSettings().isEmpty();
	}));

	QJsonArray presets = QJsonArray::fromVariantList(controller.GetServerSettings().value("tlsSamples").toList());
	QCOMPARE(presets.size(), 6);

	auto find = [&presets](const QString& id){
		for (const QJsonValue& value : presets){
			if (value.toObject().value("id").toString() == id){
				return value.toObject();
			}
		}

		return QJsonObject();
	};

	auto echo = [this, &controller](int& certificates){
		QVariantMap options;
		options.insert("method", "GET");
		options.insert("path", "/echo");
		const int exchangeId = controller.sendRequest(options);
		if (!WaitForExchange(exchangeId)){
			return 0;
		}

		certificates = FindExchange(exchangeId)->tlsText.section("certificates from the server: ", 1).toInt();

		return FindExchange(exchangeId)->statusCode;
	};

	// A preset with one click: the client takes the certificates to trust from the text of the settings
	for (const QString& id : {"bundle", "single", "ec"}){
		const QJsonObject preset = find(id);
		QStringList trusted;
		for (const QJsonValue& pem : preset.value("trustedPem").toArray()){
			trusted << pem.toString();
		}

		controller.applyTlsPreset(id, trusted);
		QVERIFY2(waitFor([&controller, &preset](){
			return controller.GetServerUrl().startsWith("https://") && controller.GetServerSettings().value("tlsCertificateFile").toString() == preset.value("certificateFile").toString()
						&& controller.GetSettingsStatus().contains("applied") && !controller.GetTrustedCertificates().isEmpty();
		}), qPrintable(id + ": " + controller.GetSettingsStatus()));

		int certificates = 0;
		QCOMPARE(echo(certificates), 200);
		QCOMPARE(certificates, preset.value("certificatesInChain").toInt());
	}

	// The files of this computer: the client reads them and sends their content, the server keeps them in its own directory
	for (const QString& id : {"chain", "bundle", "several", "ec"}){
		const QJsonObject preset = find(id);
		QStringList chainFiles;
		for (const QJsonValue& chainFile : preset.value("chainFiles").toArray()){
			chainFiles << chainFile.toString();
		}

		QStringList trusted;
		for (const QJsonValue& file : preset.value("trustedFiles").toArray()){
			trusted << file.toString();
		}

		controller.applyTlsFiles(preset.value("certificateFile").toString(), chainFiles, preset.value("keyFile").toString(), trusted);
		QVERIFY2(waitFor([&controller](){
			return controller.GetServerUrl().startsWith("https://") && controller.GetServerSettings().value("tlsCertificateFile").toString().contains(".tls/")
						&& controller.GetSettingsStatus().contains("applied");
		}), qPrintable(id + ": " + controller.GetSettingsStatus()));

		int certificates = 0;
		QCOMPARE(echo(certificates), 200);
		QCOMPARE(certificates, preset.value("certificatesInChain").toInt());

		// The key is not shown in the log of the exchanges
		for (int row = 0; row < controller.GetExchanges()->rowCount(); ++row){
			QVERIFY(!controller.GetExchanges()->requestText(row).contains("PRIVATE KEY"));
		}
	}

	// The files that cannot be read: the settings of the server are not touched
	const int restartCount = controller.GetServerSettings().value("restartCount").toInt();
	controller.applyTlsFiles(m_directory.path() + "/no-such.pem", {}, m_directory.path() + "/no-such.key", {});
	QVERIFY(controller.GetSettingsStatus().contains("no-such.pem"));
	QCOMPARE(controller.GetServerSettings().value("restartCount").toInt(), restartCount);

	// Back to http
	QVariantMap off;
	off.insert("tlsEnabled", false);
	controller.applyTlsSettings(off, {});
	QVERIFY(waitFor([&controller, &httpUrl](){
		return controller.GetServerUrl() == httpUrl && !controller.GetServerSettings().value("tlsEnabled").toBool() && controller.GetSettingsStatus().contains("applied");
	}));
}


void CClientControllerTest::SpeedIsMeasuredDuringTransferTest()
{
	// 40000 bytes in 10 parts with a pause of 200 ms: the response lasts about 2 seconds, so it is measured while it goes
	QVariantMap options;
	options.insert("method", "GET");
	options.insert("path", "/stream/40000?part=4000&delay=200");

	const int id = m_controllerPtr->sendRequest(options);
	QVERIFY(id > 0);

	double maxSpeed = 0;
	QString speedTextWhileWorking;
	QElapsedTimer timer;
	timer.start();

	for (;;){
		const SDemoExchange* exchangePtr = FindExchange(id);
		QVERIFY(exchangePtr != nullptr);

		if (exchangePtr->isFinished){
			break;
		}

		QVERIFY2(timer.elapsed() < 20000, "The response is not finished");

		maxSpeed = qMax(maxSpeed, exchangePtr->responseSpeed);
		if (exchangePtr->responseSpeed > 0 && speedTextWhileWorking.isEmpty()){
			speedTextWhileWorking = m_controllerPtr->GetExchanges()->data(m_controllerPtr->GetExchanges()->index(0), imtrestdemo::CTrafficLogModel::R_SPEED_TEXT).toString();
		}

		QCoreApplication::processEvents(QEventLoop::WaitForMoreEvents, 20);
	}

	QVERIFY2(maxSpeed > 0, "The speed was not measured while the response was received");
	QVERIFY2(speedTextWhileWorking.contains("B/s"), qPrintable(speedTextWhileWorking));

	// Speed after the end: the average of the whole exchange, the current one is zero
	const SDemoExchange* exchangePtr = FindExchange(id);
	QCOMPARE(exchangePtr->responseSize, qint64(40000));
	QCOMPARE(exchangePtr->responseSpeed, 0.0);
	QVERIFY(exchangePtr->responseAverageSpeed > 0);
	QVERIFY(exchangePtr->responseAverageSpeed < 40000);
	QVERIFY(m_controllerPtr->GetExchanges()->summaryText(0).contains("Average speed"));
}


void CClientControllerTest::ResumeUploadAfterTheBreakTest_data()
{
	QTest::addColumn<QString>("bodyType");

	QTest::newRow("generated") << QString("generated");
	QTest::newRow("file") << QString("file");
}


void CClientControllerTest::ResumeUploadAfterTheBreakTest()
{
	QFETCH(QString, bodyType);

	// The file is read faster than the pattern is made, so it must be big to be stopped before the end
	const qint64 size = (bodyType == "file" ? 192 : 48) * qint64(1024 * 1024);

	QVariantMap options;
	options.insert("method", "PUT");
	options.insert("useToken", true);
	options.insert("bodyType", bodyType);

	if (bodyType == "file"){
		const QString path = m_directory.filePath("resume-source.bin");

		QFile file(path);
		QVERIFY(file.open(QIODevice::WriteOnly));
		for (qint64 position = 0; position < size; position += 8 * 1024 * 1024){
			file.write(CreatePatternData(position, qMin<qint64>(8 * 1024 * 1024, size - position)));
		}
		file.close();

		options.insert("bodyFile", path);
	}
	else{
		options.insert("bodySize", size);
	}

	QCryptographicHash hash(QCryptographicHash::Sha256);
	for (qint64 position = 0; position < size; position += 8 * 1024 * 1024){
		hash.addData(CreatePatternData(position, qMin<qint64>(8 * 1024 * 1024, size - position)));
	}
	const QByteArray expectedHash = hash.result().toHex();

	// The client breaks the connection after a part of the body. The upload with the range is resumable: the server keeps what it has received.
	// On the local network the body can be sent before the break works, such an attempt is repeated with another name.
	int lastId = 0;
	qint64 received = 0;
	const SDemoExchange* resultPtr = nullptr;
	for (int attempt = 0; attempt < 5 && resultPtr == nullptr; ++attempt){
		options.insert("path", "/upload/" + QString("resume-client-%1-%2.bin").arg(bodyType).arg(attempt));

		QVariantMap breaking = options;
		breaking.insert("contentRange", true);
		breaking.insert("rangeStart", 0);
		breaking.insert("rangeTotal", size);
		breaking.insert("abortAfter", qint64(1024 * 1024));

		const int breakId = m_controllerPtr->sendRequest(breaking);
		QVERIFY(breakId > 0);
		QVERIFY(WaitForExchange(breakId));
		if (FindExchange(breakId)->statusCode == 0){
			QVERIFY2(FindExchange(breakId)->errorText.contains("stopped by the client"), qPrintable(FindExchange(breakId)->errorText));
		}

		// The new upload asks the state and sends only the rest. The server may not have noticed the break yet, then the client asks again.
		const int stateId = m_controllerPtr->resumeUpload(options);
		QVERIFY(stateId > 0);
		QVERIFY(WaitForExchange(stateId));
		QCOMPARE(FindExchange(stateId)->method, QByteArray("GET"));
		QCOMPARE(FindExchange(stateId)->statusCode, 200);

		if (ParseJson(FindExchange(stateId)->responsePreview).value("complete").toBool()){
			continue;
		}

		received = qint64(ParseJson(FindExchange(stateId)->responsePreview).value("received").toDouble());
		QVERIFY(received > 0);
		QVERIFY(received < size);

		lastId = stateId;
		bool isFinishedEarly = false;
		QElapsedTimer timer;
		timer.start();
		for (;;){
			QVERIFY2(timer.elapsed() < 30000, "The upload was not completed");
			QCoreApplication::processEvents(QEventLoop::WaitForMoreEvents, 20);

			if (FindExchange(lastId + 1) == nullptr){
				continue;
			}

			QVERIFY(WaitForExchange(lastId + 1));
			++lastId;

			// The exchanges are added while the test waits, so the pointer is taken after the wait
			const SDemoExchange* nextPtr = FindExchange(lastId);

			// The client asks the state again while the server still takes the bytes of the broken upload, and when the server has more than it said
			if (nextPtr->method == "GET"){
				const QJsonObject state = ParseJson(nextPtr->responsePreview);
				if (state.value("complete").toBool()){
					isFinishedEarly = true;

					break;
				}

				received = qint64(state.value("received").toDouble());
			}
			else if (nextPtr->statusCode != 409){
				break;
			}
		}

		if (!isFinishedEarly){
			resultPtr = FindExchange(lastId);
		}
	}

	QVERIFY2(resultPtr != nullptr, "The body was sent before the break five times");
	QCOMPARE(resultPtr->statusCode, 201);
	QCOMPARE(resultPtr->method, QByteArray("PUT"));
	QCOMPARE(ParseJson(resultPtr->responsePreview).value("sha256").toString().toLatin1(), expectedHash);
	QCOMPARE(resultPtr->requestBodySize, size - received);

	// The request of the rest has the range
	bool hasRange = false;
	for (const QPair<QByteArray, QByteArray>& header : resultPtr->requestHeaders){
		hasRange = hasRange || (header.first == "Content-Range" && header.second.endsWith("/" + QByteArray::number(size)));
	}
	QVERIFY(hasRange);

	// The file is complete: nothing more is sent
	const int againId = m_controllerPtr->resumeUpload(options);
	QVERIFY(WaitForExchange(againId));
	QCOMPARE(FindExchange(againId)->statusCode, 200);
	QVERIFY(ParseJson(FindExchange(againId)->responsePreview).value("complete").toBool());

	QElapsedTimer waitTimer;
	waitTimer.start();
	while (waitTimer.elapsed() < 300){
		QCoreApplication::processEvents(QEventLoop::WaitForMoreEvents, 20);
	}
	QVERIFY(FindExchange(againId + 1) == nullptr);
}


void CClientControllerTest::UploadSpeedTest_data()
{
	QTest::addColumn<bool>("isCaptureEnabled");
	QTest::addColumn<QString>("bodyType");

	QTest::newRow("generated, without the proxy") << false << QString("generated");
	QTest::newRow("generated, through the proxy") << true << QString("generated");
	QTest::newRow("file, without the proxy") << false << QString("file");
	QTest::newRow("file, through the proxy") << true << QString("file");
}


void CClientControllerTest::UploadSpeedTest()
{
	QFETCH(bool, isCaptureEnabled);
	QFETCH(QString, bodyType);

	const qint64 size = 64 * 1024 * 1024;

	m_controllerPtr->SetCaptureEnabled(isCaptureEnabled);

	QVariantMap options;
	options.insert("method", "PUT");
	options.insert("path", "/upload/speed.bin");
	options.insert("useToken", true);
	options.insert("bodyType", bodyType);

	if (bodyType == "file"){
		const QString path = m_directory.filePath("speed-source.bin");

		QFile file(path);
		QVERIFY(file.open(QIODevice::WriteOnly));
		file.write(CreatePatternData(0, size));
		file.close();

		options.insert("bodyFile", path);
	}
	else{
		options.insert("bodySize", size);
	}

	QElapsedTimer timer;
	timer.start();

	const int id = m_controllerPtr->sendRequest(options);
	QVERIFY(id > 0);
	QVERIFY(WaitForExchange(id, 120000));

	const qint64 elapsedMs = qMax<qint64>(1, timer.elapsed());
	QCOMPARE(FindExchange(id)->statusCode, 201);

	// The speed is only reported: it depends on the machine. A user sees the same difference between the lines of the data.
	qInfo().noquote() << QString("SPEED %1: %2 MiB in %3 ms = %4 MiB/s")
				.arg(QTest::currentDataTag()).arg(size / (1024 * 1024)).arg(elapsedMs).arg(double(size) / (1024 * 1024) / (double(elapsedMs) / 1000), 0, 'f', 1);
}


I_ADD_TEST(CClientControllerTest);
