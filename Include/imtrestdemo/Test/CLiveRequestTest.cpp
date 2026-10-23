// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include "CLiveRequestTest.h"


// Qt includes
#include <QtCore/QElapsedTimer>
#include <QtCore/QRegularExpression>

// ACF includes
#include <itest/CStandardTestExecutor.h>


void CLiveRequestTest::RequestInPartsTest()
{
	CRawHttpClient client;
	QVERIFY(Connect(client));

	// The boundaries are inside of the path, of the name of a header and of its value
	const QByteArrayList parts = {
		"GET /echo?name=va",
		"lue&x=1 HTTP/1.1\r\nHo",
		"st: localhost\r\nX-Pa",
		"rt: first",
		"-second\r\nConnection: close\r\n\r\n"};

	for (const QByteArray& part : parts){
		QVERIFY(client.Send(part));
		QTest::qSleep(80);
	}

	Response response;
	QVERIFY(client.ReadResponse(response));
	QCOMPARE(response.statusCode, 200);

	const QJsonObject echo = ParseJson(response.body);
	QCOMPARE(echo.value("path").toString(), QString("/echo"));
	QCOMPARE(echo.value("query").toObject().value("name").toString(), QString("value"));
	QCOMPARE(echo.value("query").toObject().value("x").toString(), QString("1"));
	QCOMPARE(echo.value("headers").toObject().value("x-part").toString(), QString("first-second"));
}


void CLiveRequestTest::RepeatedHeadersTest()
{
	Response response;
	QVERIFY(Get("/echo", response, {{"X-Rep", "1"}, {"x-rep", "2"}, {"Cookie", "a=1"}, {"cookie", "b=2"}}));
	QCOMPARE(response.statusCode, 200);

	const QJsonObject headers = ParseJson(response.body).value("headers").toObject();

	// The repeated header is one header with the values in the order of the request (RFC 9110, section 5.3), the cookies are joined with ';'
	QCOMPARE(headers.value("x-rep").toString(), QString("1, 2"));
	QCOMPARE(headers.value("cookie").toString(), QString("a=1; b=2"));
}


void CLiveRequestTest::HeaderNamesAreCaseInsensitiveTest()
{
	Response response;
	QVERIFY(Get("/echo", response, {{"X-MiXeD-CaSe", "value"}}));
	QCOMPARE(response.statusCode, 200);

	const QJsonObject headers = ParseJson(response.body).value("headers").toObject();
	QCOMPARE(headers.value("x-mixed-case").toString(), QString("value"));
}


void CLiveRequestTest::MethodsTest_data()
{
	QTest::addColumn<QByteArray>("method");

	QTest::newRow("GET") << QByteArray("GET");
	QTest::newRow("POST") << QByteArray("POST");
	QTest::newRow("PUT") << QByteArray("PUT");
	QTest::newRow("DELETE") << QByteArray("DELETE");
	QTest::newRow("PATCH") << QByteArray("PATCH");
	QTest::newRow("OPTIONS") << QByteArray("OPTIONS");
}


void CLiveRequestTest::MethodsTest()
{
	QFETCH(QByteArray, method);

	const bool hasBody = method == "POST" || method == "PUT" || method == "PATCH";

	Response response;
	QVERIFY(Request(method, "/echo", response, Headers(), hasBody ? QByteArray("body of the request") : QByteArray()));
	QCOMPARE(response.statusCode, 200);

	const QJsonObject echo = ParseJson(response.body);
	QCOMPARE(echo.value("method").toString(), QString::fromLatin1(method));

	if (hasBody){
		QCOMPARE(echo.value("body").toString(), QString("body of the request"));
		QCOMPARE(echo.value("bodySize").toInt(), 19);
	}
}


void CLiveRequestTest::SlowRequestTimeoutTest()
{
	CRawHttpClient client;
	QVERIFY(Connect(client));

	// The head of the request is not finished: the server must not wait for it forever
	QVERIFY(client.Send("GET /echo HTTP/1.1\r\nHost: localhost\r\nX-Slow: "));

	QElapsedTimer timer;
	timer.start();

	Response response;
	QVERIFY(client.ReadResponse(response, false, s_requestInactivityTimeoutMs + 4000));
	QCOMPARE(response.statusCode, 408);
	QCOMPARE(response.GetHeader("connection").toLower(), QByteArray("close"));
	QVERIFY(timer.elapsed() >= s_requestInactivityTimeoutMs / 2);

	// The server waits for the client to close first (lingering close), then aborts the connection after 5 seconds
	QVERIFY(client.WaitForClose(8000));
	QVERIFY(IsServerAlive());
}


void CLiveRequestTest::IdleConnectionTest()
{
	CRawHttpClient client;
	QVERIFY(Connect(client));

	QElapsedTimer timer;
	timer.start();

	// A connection without any request is closed after the keep-alive timeout
	QVERIFY(client.WaitForClose(s_keepAliveTimeoutMs + 4000));
	QVERIFY(timer.elapsed() >= s_keepAliveTimeoutMs / 2);
	QVERIFY(IsServerAlive());
}


void CLiveRequestTest::IdleKeepAliveConnectionTest()
{
	CRawHttpClient client;
	QVERIFY(Connect(client));
	QVERIFY(client.Send(CRawHttpClient::CreateRequest("GET", "/echo")));

	Response response;
	QVERIFY(client.ReadResponse(response));
	QCOMPARE(response.statusCode, 200);

	QElapsedTimer timer;
	timer.start();

	// The connection stays open for the next request and is closed when it is idle too long
	QVERIFY(client.IsOpen());
	QVERIFY(client.WaitForClose(s_keepAliveTimeoutMs + 4000));
	QVERIFY(timer.elapsed() >= s_keepAliveTimeoutMs / 2);
}


void CLiveRequestTest::ResponseHeadersTest()
{
	Response response;
	QVERIFY(Get("/echo", response));

	QCOMPARE(response.version, QByteArray("HTTP/1.1"));
	QCOMPARE(response.statusCode, 200);

	// RFC 9110, section 5.6.7
	const QRegularExpression dateFormat("^(Mon|Tue|Wed|Thu|Fri|Sat|Sun), \\d{2} (Jan|Feb|Mar|Apr|May|Jun|Jul|Aug|Sep|Oct|Nov|Dec) \\d{4} \\d{2}:\\d{2}:\\d{2} GMT$");
	QVERIFY2(dateFormat.match(QString::fromLatin1(response.GetHeader("date"))).hasMatch(), response.GetHeader("date").constData());

	QCOMPARE(response.GetHeader("content-length").toInt(), int(response.body.size()));
	QCOMPARE(response.GetHeader("content-type"), QByteArray("application/json"));
}


void CLiveRequestTest::HeadResponseTest()
{
	CRawHttpClient client;
	QVERIFY(Connect(client));

	QVERIFY(client.Send(CRawHttpClient::CreateRequest("HEAD", "/files/hello.txt")));

	Response head;
	QVERIFY(client.ReadResponse(head, true));
	QCOMPARE(head.statusCode, 200);

	// The headers are the same as of GET, the body is not sent
	QCOMPARE(head.GetHeader("content-length"), QByteArray("16"));
	QVERIFY(head.body.isEmpty());

	// The bytes that are not sent would break the next response on the connection
	QVERIFY(client.Send(CRawHttpClient::CreateRequest("GET", "/files/hello.txt")));

	Response get;
	QVERIFY(client.ReadResponse(get));
	QCOMPARE(get.statusCode, 200);
	QCOMPARE(get.body, QByteArray("Hello, imtrest!\n"));
	QCOMPARE(get.GetHeader("etag"), head.GetHeader("etag"));
}


void CLiveRequestTest::ResponseWithoutBodyTest_data()
{
	QTest::addColumn<QByteArray>("target");
	QTest::addColumn<int>("status");

	QTest::newRow("204") << QByteArray("/behavior/status/204") << 204;
	QTest::newRow("205") << QByteArray("/behavior/status/205") << 205;
	QTest::newRow("304") << QByteArray("/behavior/status/304") << 304;
}


void CLiveRequestTest::ResponseWithoutBodyTest()
{
	QFETCH(QByteArray, target);
	QFETCH(int, status);

	CRawHttpClient client;
	QVERIFY(Connect(client));
	QVERIFY(client.Send(CRawHttpClient::CreateRequest("GET", target)));

	Response response;
	QVERIFY(client.ReadResponse(response));
	QCOMPARE(response.statusCode, status);
	QVERIFY(response.body.isEmpty());
	QVERIFY(!response.isChunked);

	// RFC 9110, section 8.6: 204 has no Content-Length, 205 has the length 0
	if (status == 204){
		QVERIFY(!response.HasHeader("content-length"));
	}

	if (status == 205){
		QCOMPARE(response.GetHeader("content-length"), QByteArray("0"));
	}

	// Nothing is left on the connection: the next response is read from its beginning
	QVERIFY(client.Send(CRawHttpClient::CreateRequest("GET", "/echo?next=1")));

	Response next;
	QVERIFY(client.ReadResponse(next));
	QCOMPARE(next.statusCode, 200);
	QCOMPARE(ParseJson(next.body).value("query").toObject().value("next").toString(), QString("1"));
}


void CLiveRequestTest::ConnectionCloseTest()
{
	CRawHttpClient client;
	QVERIFY(Connect(client));
	QVERIFY(client.Send(CRawHttpClient::CreateRequest("GET", "/echo", {{"Connection", "close"}})));

	Response response;
	QVERIFY(client.ReadResponse(response));
	QCOMPARE(response.statusCode, 200);
	QCOMPARE(response.GetHeader("connection").toLower(), QByteArray("close"));
	QVERIFY(client.WaitForClose());
}


void CLiveRequestTest::Http10Test()
{
	CRawHttpClient client;
	QVERIFY(Connect(client));
	QVERIFY(client.Send(CRawHttpClient::CreateRequest("GET", "/echo", Headers(), QByteArray(), "HTTP/1.0")));

	Response response;
	QVERIFY(client.ReadResponse(response));
	QCOMPARE(response.statusCode, 200);

	// The client of HTTP/1.0 does not keep the connection without Connection: keep-alive
	QVERIFY(client.WaitForClose());
}


void CLiveRequestTest::ServerControlsHeadersTest()
{
	Response response;
	QVERIFY(Get("/behavior/headers", response));
	QCOMPARE(response.statusCode, 200);

	// The servlet has set Content-Length: 99999 and Connection: keep-alive, the server sends the real length
	QCOMPARE(response.body, QByteArray("headers"));
	QCOMPARE(response.GetHeader("content-length"), QByteArray("7"));
	QCOMPARE(response.GetHeader("x-demo-header"), QByteArray("demo value"));
	QVERIFY(response.GetHeader("connection").toLower() != "keep-alive, keep-alive");

	// The value with a line break would add a header: it is not sent
	QVERIFY(!response.rawHead.contains("X-Injected"));
	QVERIFY(!response.rawHead.toLower().contains("x-injected"));
}


void CLiveRequestTest::UnknownPathTest()
{
	Response response;
	QVERIFY(Get("/there-is-no-such-path", response));
	QVERIFY(response.statusCode >= 400);
	QVERIFY(IsServerAlive());
}


void CLiveRequestTest::KeepAliveReuseTest()
{
	CRawHttpClient client;
	QVERIFY(Connect(client));

	for (int i = 0; i < 3; ++i){
		QVERIFY(client.Send(CRawHttpClient::CreateRequest("GET", "/echo?i=" + QByteArray::number(i))));

		Response response;
		QVERIFY(client.ReadResponse(response));
		QCOMPARE(response.statusCode, 200);
		QVERIFY(response.GetHeader("connection").toLower() != "close");
		QCOMPARE(ParseJson(response.body).value("query").toObject().value("i").toString(), QString::number(i));
	}
}


void CLiveRequestTest::SequentialRequestsTest()
{
	CRawHttpClient client;
	QVERIFY(Connect(client));

	for (int i = 0; i < 50; ++i){
		// The bodies are different, the answer of a request must not get the body of another one
		const QByteArray body = "request number " + QByteArray::number(i);

		QVERIFY(client.Send(CRawHttpClient::CreateRequest("POST", "/echo?i=" + QByteArray::number(i), Headers(), body)));

		Response response;
		QVERIFY(client.ReadResponse(response));
		QCOMPARE(response.statusCode, 200);

		const QJsonObject echo = ParseJson(response.body);
		QCOMPARE(echo.value("query").toObject().value("i").toString(), QString::number(i));
		QCOMPARE(echo.value("body").toString(), QString::fromLatin1(body));
	}
}


void CLiveRequestTest::PipelinedRequestsTest()
{
	CRawHttpClient client;
	QVERIFY(Connect(client));

	// Three requests in one write: the responses are in the order of the requests
	QByteArray requests;
	for (int i = 0; i < 3; ++i){
		requests += CRawHttpClient::CreateRequest("GET", "/echo?i=" + QByteArray::number(i));
	}

	QVERIFY(client.Send(requests));

	for (int i = 0; i < 3; ++i){
		Response response;
		QVERIFY2(client.ReadResponse(response), qPrintable(QString("response %1").arg(i)));
		QCOMPARE(response.statusCode, 200);
		QCOMPARE(ParseJson(response.body).value("query").toObject().value("i").toString(), QString::number(i));
	}
}


void CLiveRequestTest::BadRequestTest_data()
{
	QTest::addColumn<QByteArray>("request");

	QTest::newRow("not a request") << QByteArray("this is not a request\r\n\r\n");
	QTest::newRow("unknown method") << QByteArray("FOO /echo HTTP/1.1\r\nHost: localhost\r\n\r\n");
	QTest::newRow("space in the name of a header") << QByteArray("GET /echo HTTP/1.1\r\nHost: localhost\r\nBad Name: value\r\n\r\n");
	QTest::newRow("bad length") << QByteArray("POST /echo HTTP/1.1\r\nHost: localhost\r\nContent-Length: abc\r\n\r\n");
}


void CLiveRequestTest::BadRequestTest()
{
	QFETCH(QByteArray, request);

	CRawHttpClient client;
	QVERIFY(Connect(client));
	QVERIFY(client.Send(request));

	// The request that cannot be parsed is answered with 400, the connection is closed (its state is unknown). The server waits
	// for the client to close first (lingering close, RFC 9112, section 9.6) and aborts the connection after 5 seconds.
	Response response;
	QVERIFY(client.ReadResponse(response));
	QCOMPARE(response.statusCode, 400);
	QCOMPARE(response.GetHeader("connection").toLower(), QByteArray("close"));
	QVERIFY(client.WaitForClose(8000));

	QVERIFY(IsServerAlive());
}


void CLiveRequestTest::ParallelRequestsTest()
{
	const int count = 12;

	QList<QSharedPointer<CRawHttpClient>> clients;
	for (int i = 0; i < count; ++i){
		QSharedPointer<CRawHttpClient> clientPtr(new CRawHttpClient);
		QVERIFY(Connect(*clientPtr));

		clients.append(clientPtr);
	}

	// All connections are open at the same time, then all requests are sent, then the answers are read
	for (int i = 0; i < count; ++i){
		QVERIFY(clients[i]->Send(CRawHttpClient::CreateRequest("GET", "/echo?id=" + QByteArray::number(i))));
	}

	for (int i = 0; i < count; ++i){
		Response response;
		QVERIFY(clients[i]->ReadResponse(response));
		QCOMPARE(response.statusCode, 200);
		QCOMPARE(ParseJson(response.body).value("query").toObject().value("id").toString(), QString::number(i));
	}
}


void CLiveRequestTest::ServletExceptionTest()
{
	// The exception of a servlet must not stop the server: the client gets 500 and the server works further
	Response response;
	QVERIFY(Get("/behavior/throw", response));
	QCOMPARE(response.statusCode, 500);

	QVERIFY(IsServerAlive());
}


void CLiveRequestTest::ProcessingTimeoutTest()
{
	Response quick;
	QVERIFY(Get("/behavior/sleep?ms=100", quick));
	QCOMPARE(quick.statusCode, 200);

	// The servlet works longer than the server allows: the client gets 503 and does not wait for the end of the work
	CRawHttpClient client;
	QVERIFY(Connect(client));
	QVERIFY(client.Send(CRawHttpClient::CreateRequest("GET", "/behavior/sleep?ms=3500")));

	QElapsedTimer timer;
	timer.start();

	Response slow;
	QVERIFY(client.ReadResponse(slow));
	QCOMPARE(slow.statusCode, 503);
	QVERIFY(timer.elapsed() < 3000);
	QVERIFY(client.WaitForClose(8000));

	QVERIFY(IsServerAlive());
}


void CLiveRequestTest::InfoTest()
{
	Response response;
	QVERIFY(Get("/info", response));
	QCOMPARE(response.statusCode, 200);

	const QJsonObject info = ParseJson(response.body);
	QCOMPARE(info.value("port").toInt(), GetPort());
	QCOMPARE(info.value("requestInactivityTimeoutMs").toInt(), int(s_requestInactivityTimeoutMs));
	QCOMPARE(info.value("keepAliveTimeoutMs").toInt(), int(s_keepAliveTimeoutMs));
	QCOMPARE(info.value("requestProcessingTimeoutMs").toInt(), int(s_requestProcessingTimeoutMs));
}


I_ADD_TEST(CLiveRequestTest);
