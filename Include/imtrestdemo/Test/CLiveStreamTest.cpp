// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include "CLiveStreamTest.h"


// Qt includes
#include <QtCore/QBuffer>
#include <QtCore/QCryptographicHash>
#include <QtCore/QElapsedTimer>
#include <QtCore/QEventLoop>
#include <QtCore/QTimer>
#include <QtNetwork/QNetworkAccessManager>

// ACF includes
#include <itest/CStandardTestExecutor.h>

// ImtCore includes
#include <imtrestdemo/CDemoProtocol.h>


namespace
{


using imtrestdemo::DemoProtocol::CreatePatternData;


QByteArray GetToken()
{
	return QByteArray("Bearer ") + imtrestdemo::DemoProtocol::s_uploadToken;
}


}


void CLiveStreamTest::StreamTest()
{
	Response response;
	QVERIFY(Get("/stream/2000000", response));
	QCOMPARE(response.statusCode, 200);

	// The size is known: Content-Length, no chunks
	QCOMPARE(response.GetHeader("content-length"), QByteArray("2000000"));
	QVERIFY(!response.isChunked);
	QVERIFY(response.body == CreatePatternData(0, 2000000));
}


void CLiveStreamTest::StreamZeroTest()
{
	Response response;
	QVERIFY(Get("/stream/0", response));
	QCOMPARE(response.statusCode, 200);
	QCOMPARE(response.GetHeader("content-length"), QByteArray("0"));
	QVERIFY(response.body.isEmpty());
}


void CLiveStreamTest::StreamChunkedTest()
{
	Response response;
	QVERIFY(Get("/stream/1500000?chunked=1", response));
	QCOMPARE(response.statusCode, 200);

	// The size is not announced: the body is sent in the chunks (RFC 9112, section 7.1)
	QVERIFY(response.isChunked);
	QVERIFY(!response.HasHeader("content-length"));
	QVERIFY(response.body == CreatePatternData(0, 1500000));
}


void CLiveStreamTest::StreamChunkedSmallPartsTest()
{
	Response response;
	QVERIFY(Get("/stream/50000?chunked=1&part=777", response));
	QCOMPARE(response.statusCode, 200);
	QVERIFY(response.isChunked);
	QVERIFY(response.body == CreatePatternData(0, 50000));
}


void CLiveStreamTest::StreamHttp10Test()
{
	CRawHttpClient client;
	QVERIFY(Connect(client));
	QVERIFY(client.Send(CRawHttpClient::CreateRequest("GET", "/stream/100000?chunked=1", Headers(), QByteArray(), "HTTP/1.0")));

	// HTTP/1.0 has no chunks: the end of the body is the end of the connection
	Response response;
	QVERIFY(client.ReadResponse(response));
	QCOMPARE(response.statusCode, 200);
	QVERIFY(!response.isChunked);
	QVERIFY(!response.HasHeader("content-length"));
	QVERIFY(response.body == CreatePatternData(0, 100000));
	QVERIFY(client.WaitForClose());
}


void CLiveStreamTest::StreamHeadTest()
{
	CRawHttpClient client;
	QVERIFY(Connect(client));
	QVERIFY(client.Send(CRawHttpClient::CreateRequest("HEAD", "/stream/1000")));

	Response head;
	QVERIFY(client.ReadResponse(head, true));
	QCOMPARE(head.statusCode, 200);
	QCOMPARE(head.GetHeader("content-length"), QByteArray("1000"));
	QVERIFY(head.body.isEmpty());

	// The body of the stream is not generated for HEAD: the next response starts right after the head
	QVERIFY(client.Send(CRawHttpClient::CreateRequest("GET", "/stream/10")));

	Response get;
	QVERIFY(client.ReadResponse(get));
	QCOMPARE(get.statusCode, 200);
	QVERIFY(get.body == CreatePatternData(0, 10));
}


void CLiveStreamTest::StreamBadSizeTest_data()
{
	QTest::addColumn<QByteArray>("target");

	QTest::newRow("not a number") << QByteArray("/stream/abc");
	QTest::newRow("negative") << QByteArray("/stream/-5");
	QTest::newRow("without the size") << QByteArray("/stream/1/2");
}


void CLiveStreamTest::StreamBadSizeTest()
{
	QFETCH(QByteArray, target);

	Response response;
	QVERIFY(Get(target, response));
	QCOMPARE(response.statusCode, 400);
}


void CLiveStreamTest::SlowStreamTest()
{
	QElapsedTimer timer;
	timer.start();

	// Five parts and a pause after each of the four first: the source waits and tells the server when the data are ready
	Response response;
	QVERIFY(Get("/stream/5000?part=1000&delay=100", response));
	QCOMPARE(response.statusCode, 200);
	QVERIFY(response.body == CreatePatternData(0, 5000));
	QVERIFY2(timer.elapsed() >= 350, qPrintable(QString::number(timer.elapsed())));
}


void CLiveStreamTest::SlowReaderTest()
{
	const qint64 size = 20 * 1024 * 1024;

	CRawHttpClient client;
	QVERIFY(Connect(client));
	QVERIFY(client.Send(CRawHttpClient::CreateRequest("GET", "/stream/" + QByteArray::number(size))));

	// The client does not read for a while, the server must wait for it (the limit of the unsent data) and then go on
	QTest::qSleep(600);

	Response response;
	QVERIFY(client.ReadResponse(response, false, 30000));
	QCOMPARE(response.statusCode, 200);
	QCOMPARE(qint64(response.body.size()), size);
	QVERIFY(response.body == CreatePatternData(0, size));
}


void CLiveStreamTest::StreamAbortTest()
{
	// The client breaks the connection in the middle of a huge response: the server must release the connection and the thread.
	// More breaks than the server has threads would stop it if something stayed.
	for (int i = 0; i < 25; ++i){
		CRawHttpClient client;
		QVERIFY(Connect(client));
		QVERIFY(client.Send(CRawHttpClient::CreateRequest("GET", "/stream/4000000000")));

		// The head and a part of the body
		Response response;
		client.ReadResponse(response, false, 300);

		client.Close();
	}

	QVERIFY(IsServerAlive());

	Response response;
	QVERIFY(Get("/stream/1000", response));
	QCOMPARE(response.statusCode, 200);
}


void CLiveStreamTest::BigBufferedBodyTest()
{
	// A body that is in memory (not a streaming response) is sent in the same way
	Response response;
	QVERIFY(Get("/behavior/big?size=3000000", response));
	QCOMPARE(response.statusCode, 200);
	QCOMPARE(response.GetHeader("content-length"), QByteArray("3000000"));
	QVERIFY(response.body == CreatePatternData(0, 3000000));
}


void CLiveStreamTest::TooBigBufferedBodyTest()
{
	Response response;
	QVERIFY(Get("/behavior/big?size=999999999", response));
	QCOMPARE(response.statusCode, 400);
}


bool CLiveStreamTest::WaitForReply(QNetworkReply& reply, int timeoutMs)
{
	if (reply.isFinished()){
		return true;
	}

	QEventLoop loop;
	QTimer timer;
	timer.setSingleShot(true);

	QObject::connect(&reply, &QNetworkReply::finished, &loop, &QEventLoop::quit);
	QObject::connect(&timer, &QTimer::timeout, &loop, &QEventLoop::quit);

	timer.start(timeoutMs);
	loop.exec();

	return reply.isFinished();
}


QUrl CLiveStreamTest::CreateUrl(const QString& pathAndQuery) const
{
	return QUrl(QStringLiteral("http://localhost:%1%2").arg(GetPort()).arg(pathAndQuery));
}


QNetworkRequest CLiveStreamTest::CreateRequest(const QString& pathAndQuery) const
{
	QNetworkRequest request(CreateUrl(pathAndQuery));

	// The cache of the client would answer instead of the server
	request.setAttribute(QNetworkRequest::CacheLoadControlAttribute, QNetworkRequest::AlwaysNetwork);

	return request;
}


void CLiveStreamTest::NetworkGetTest()
{
	QNetworkAccessManager manager;

	QNetworkRequest request = CreateRequest("/echo?name=%D0%B8%D0%BC%D1%8F&x=1");
	request.setRawHeader("X-From-Client", "qt");

	std::unique_ptr<QNetworkReply> replyPtr(manager.get(request));
	QVERIFY(WaitForReply(*replyPtr));
	QCOMPARE(replyPtr->error(), QNetworkReply::NoError);
	QCOMPARE(replyPtr->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt(), 200);

	const QJsonObject echo = ParseJson(replyPtr->readAll());
	QCOMPARE(echo.value("method").toString(), QString("GET"));
	QCOMPARE(echo.value("query").toObject().value("name").toString(), QString::fromUtf8("\xD0\xB8\xD0\xBC\xD1\x8F"));
	QCOMPARE(echo.value("headers").toObject().value("x-from-client").toString(), QString("qt"));
}


void CLiveStreamTest::NetworkPostTest()
{
	QNetworkAccessManager manager;

	QNetworkRequest request = CreateRequest("/echo");
	request.setHeader(QNetworkRequest::ContentTypeHeader, "text/plain");

	const QByteArray body = CreatePatternData(0, 3000);

	std::unique_ptr<QNetworkReply> replyPtr(manager.post(request, body));
	QVERIFY(WaitForReply(*replyPtr));
	QCOMPARE(replyPtr->error(), QNetworkReply::NoError);

	const QJsonObject echo = ParseJson(replyPtr->readAll());
	QCOMPARE(echo.value("method").toString(), QString("POST"));
	QCOMPARE(echo.value("bodySize").toInt(), 3000);
	QCOMPARE(echo.value("bodySha256").toString().toLatin1(), QCryptographicHash::hash(body, QCryptographicHash::Sha256).toHex());
}


void CLiveStreamTest::NetworkUploadDownloadTest()
{
	QNetworkAccessManager manager;

	// The client sends the body from a device, in parts, as a client of a big file does
	const QByteArray body = CreatePatternData(7, 5 * 1024 * 1024);
	QBuffer device;
	device.setData(body);
	QVERIFY(device.open(QIODevice::ReadOnly));

	QNetworkRequest putRequest = CreateRequest("/upload/nam.bin");
	putRequest.setRawHeader("Authorization", GetToken());

	std::unique_ptr<QNetworkReply> putPtr(manager.put(putRequest, &device));
	QVERIFY(WaitForReply(*putPtr));
	QCOMPARE(putPtr->error(), QNetworkReply::NoError);
	QCOMPARE(putPtr->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt(), 201);

	const QJsonObject result = ParseJson(putPtr->readAll());
	QCOMPARE(result.value("sha256").toString().toLatin1(), QCryptographicHash::hash(body, QCryptographicHash::Sha256).toHex());

	std::unique_ptr<QNetworkReply> getPtr(manager.get(CreateRequest("/files/uploaded/nam.bin")));
	QVERIFY(WaitForReply(*getPtr));
	QCOMPARE(getPtr->error(), QNetworkReply::NoError);
	QVERIFY(getPtr->readAll() == body);
}


void CLiveStreamTest::NetworkRangeTest()
{
	QNetworkAccessManager manager;

	QNetworkRequest request = CreateRequest("/files/pattern.bin");
	request.setRawHeader("Range", "bytes=1000-1999");

	std::unique_ptr<QNetworkReply> replyPtr(manager.get(request));
	QVERIFY(WaitForReply(*replyPtr));
	QCOMPARE(replyPtr->error(), QNetworkReply::NoError);
	QCOMPARE(replyPtr->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt(), 206);
	QCOMPARE(replyPtr->rawHeader("Content-Range"), QByteArray("bytes 1000-1999/1048589"));
	QVERIFY(replyPtr->readAll() == CreatePatternData(1000, 1000));
}


void CLiveStreamTest::NetworkStreamTest_data()
{
	QTest::addColumn<QString>("query");

	QTest::newRow("with the size") << QString();
	QTest::newRow("chunked") << QString("?chunked=1");
	QTest::newRow("slow") << QString("?part=50000&delay=20");
}


void CLiveStreamTest::NetworkStreamTest()
{
	QFETCH(QString, query);

	QNetworkAccessManager manager;

	std::unique_ptr<QNetworkReply> replyPtr(manager.get(CreateRequest("/stream/600000" + query)));
	QVERIFY(WaitForReply(*replyPtr));
	QCOMPARE(replyPtr->error(), QNetworkReply::NoError);
	QVERIFY(replyPtr->readAll() == CreatePatternData(0, 600000));
}


void CLiveStreamTest::NetworkStatusTest_data()
{
	QTest::addColumn<int>("status");
	QTest::addColumn<QNetworkReply::NetworkError>("error");

	QTest::newRow("200") << 200 << QNetworkReply::NoError;
	QTest::newRow("201") << 201 << QNetworkReply::NoError;
	QTest::newRow("204") << 204 << QNetworkReply::NoError;
	QTest::newRow("400") << 400 << QNetworkReply::ProtocolInvalidOperationError;
	QTest::newRow("401") << 401 << QNetworkReply::AuthenticationRequiredError;
	QTest::newRow("403") << 403 << QNetworkReply::ContentAccessDenied;
	QTest::newRow("404") << 404 << QNetworkReply::ContentNotFoundError;
	QTest::newRow("500") << 500 << QNetworkReply::InternalServerError;
	QTest::newRow("503") << 503 << QNetworkReply::ServiceUnavailableError;
}


void CLiveStreamTest::NetworkStatusTest()
{
	QFETCH(int, status);
	QFETCH(QNetworkReply::NetworkError, error);

	QNetworkAccessManager manager;

	std::unique_ptr<QNetworkReply> replyPtr(manager.get(CreateRequest("/behavior/status/" + QString::number(status))));
	QVERIFY(WaitForReply(*replyPtr));
	QCOMPARE(replyPtr->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt(), status);
	QCOMPARE(replyPtr->error(), error);
}


void CLiveStreamTest::NetworkConditionTest()
{
	QNetworkAccessManager manager;

	std::unique_ptr<QNetworkReply> firstPtr(manager.get(CreateRequest("/files/hello.txt")));
	QVERIFY(WaitForReply(*firstPtr));
	const QByteArray eTag = firstPtr->rawHeader("ETag");
	QVERIFY(!eTag.isEmpty());

	QNetworkRequest request = CreateRequest("/files/hello.txt");
	request.setRawHeader("If-None-Match", eTag);

	std::unique_ptr<QNetworkReply> secondPtr(manager.get(request));
	QVERIFY(WaitForReply(*secondPtr));
	QCOMPARE(secondPtr->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt(), 304);
	QVERIFY(secondPtr->readAll().isEmpty());
}


void CLiveStreamTest::NetworkRedirectTest()
{
	QNetworkAccessManager manager;
	manager.setRedirectPolicy(QNetworkRequest::NoLessSafeRedirectPolicy);

	// The client follows the redirect of the servlet to /echo
	std::unique_ptr<QNetworkReply> replyPtr(manager.get(CreateRequest("/behavior/status/302")));
	QVERIFY(WaitForReply(*replyPtr));
	QCOMPARE(replyPtr->error(), QNetworkReply::NoError);
	QCOMPARE(ParseJson(replyPtr->readAll()).value("path").toString(), QString("/echo"));
}


void CLiveStreamTest::NetworkKeepAliveTest()
{
	QNetworkAccessManager manager;

	// Many requests one after another, the client reuses its connection
	for (int i = 0; i < 20; ++i){
		std::unique_ptr<QNetworkReply> replyPtr(manager.get(CreateRequest(QString("/echo?i=%1").arg(i))));
		QVERIFY(WaitForReply(*replyPtr));
		QCOMPARE(replyPtr->error(), QNetworkReply::NoError);
		QCOMPARE(ParseJson(replyPtr->readAll()).value("query").toObject().value("i").toString(), QString::number(i));
	}
}


I_ADD_TEST(CLiveStreamTest);
