// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include "CHttpRequestTest.h"


// STL includes
#include <memory>

// Qt includes
#include <QtCore/QBuffer>
#include <QtNetwork/QTcpServer>
#include <QtNetwork/QTcpSocket>

// ImtCore includes
#include <imtrest/CHttpRequest.h>
#include <imtrest/CTcpRequest.h>
#include <imtrest/IProtocolEngine.h>


namespace
{


class CRequestServletStub: virtual public imtrest::IRequestServlet
{
public:
	// reimplemented (imtrest::IRequestServlet)
	bool IsCommandSupported(const QByteArray& /*commandId*/) const override
	{
		return true;
	}

	imtrest::ConstResponsePtr ProcessRequest(const imtrest::IRequest& /*request*/, const QByteArray& /*subCommandId*/) const override
	{
		return {};
	}
};


class CProtocolEngineStub: virtual public imtrest::IProtocolEngine
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

	virtual bool GetProtocolStatusCode(int /*statusCode*/, int& /*protocolStatusCode*/, QByteArray& /*statusCodeLiteral*/) const override
	{
		return false;
	}

	virtual imtrest::IRequestUniquePtr CreateRequest(const imtrest::IRequestServlet& /*requestHandler*/) const override
	{
		return nullptr;
	}

	virtual imtrest::IRequestUniquePtr CreateRequestForSend(
				const imtrest::IRequestServlet& /*requestHandler*/,
				int /*statusCode*/,
				const QByteArray& /*data*/,
				const QByteArray& /*dataTypeId*/) const override
	{
		return nullptr;
	}

	virtual imtrest::IResponseUniquePtr CreateResponse(
				const imtrest::IRequest& /*request*/,
				int /*statusCode*/,
				const QByteArray& /*data*/,
				const QByteArray& /*dataTypeId*/) const override
	{
		return nullptr;
	}
};


typedef QMap<QByteArray, QByteArray> HeaderValues;


const QByteArray s_request(
			"POST /api/graphql?product=dogma HTTP/1.1\r\n"
			"Host: localhost:7380\r\n"
			"Content-Type: application/json\r\n"
			"Authorization: Bearer 0123456789abcdef\r\n"
			"Accept: text/html\r\n"
			"Accept: application/json\r\n"
			"Content-Length: 11\r\n"
			"\r\n"
			"{\"query\":1}");


const QByteArray s_chunkedRequest(
			"POST /api/upload HTTP/1.1\r\n"
			"Transfer-Encoding: chunked\r\n"
			"\r\n"
			"5\r\n"
			"hello\r\n"
			"6\r\n"
			" world\r\n"
			"0\r\n"
			"\r\n");


HeaderValues GetHeaderValues(const imtrest::CHttpRequest& request)
{
	HeaderValues retVal;

	const QByteArrayList headers = request.GetHeaders();
	for (const QByteArray& header : headers){
		retVal.insert(header, request.GetHeaderValue(header));
	}

	return retVal;
}


bool ParseParts(imtrest::CHttpRequest& request, const QByteArrayList& parts)
{
	for (const QByteArray& part : parts){
		if (!request.ExecuteHttpParser(part, nullptr)){
			return false;
		}
	}

	return true;
}


void VerifyParsedRequest(const imtrest::CHttpRequest& request)
{
	HeaderValues expectedHeaders;
	expectedHeaders.insert("host", "localhost:7380");
	expectedHeaders.insert("content-type", "application/json");
	expectedHeaders.insert("authorization", "Bearer 0123456789abcdef");
	expectedHeaders.insert("accept", "text/html, application/json");
	expectedHeaders.insert("content-length", "11");

	QCOMPARE(request.GetState(), imtrest::IRequest::RS_MESSAGE_COMPLETE);
	QCOMPARE(request.GetMethodType(), imtrest::CHttpRequest::MT_POST);
	QCOMPARE(GetHeaderValues(request), expectedHeaders);
	QCOMPARE(request.GetUrl().path(), QStringLiteral("/api/graphql"));
	QCOMPARE(request.GetUrl().host(), QStringLiteral("localhost"));
	QCOMPARE(request.GetUrl().port(), 7380);
	QCOMPARE(request.GetCommandParams().value("product"), QByteArray("dogma"));
	QCOMPARE(request.GetBody(), QByteArray("{\"query\":1}"));
}


} // namespace


// private slots

void CHttpRequestTest::ParseWholeRequestTest()
{
	CRequestServletStub servlet;
	CProtocolEngineStub engine;
	imtrest::CHttpRequest request(servlet, engine);

	QVERIFY(request.ExecuteHttpParser(s_request, nullptr));

	VerifyParsedRequest(request);
}


void CHttpRequestTest::ParseByteByByteTest()
{
	CRequestServletStub servlet;
	CProtocolEngineStub engine;
	imtrest::CHttpRequest request(servlet, engine);

	for (int i = 0; i < s_request.size(); ++i){
		QVERIFY(request.ExecuteHttpParser(s_request.mid(i, 1), nullptr));
	}

	VerifyParsedRequest(request);
}


void CHttpRequestTest::ParseSplitAtEveryPositionTest()
{
	CRequestServletStub servlet;
	CProtocolEngineStub engine;

	for (int splitPosition = 1; splitPosition < s_request.size(); ++splitPosition){
		imtrest::CHttpRequest request(servlet, engine);

		QVERIFY(ParseParts(request, {s_request.left(splitPosition), s_request.mid(splitPosition)}));

		VerifyParsedRequest(request);
		if (QTest::currentTestFailed()){
			qWarning() << "Request split at position" << splitPosition;

			return;
		}
	}
}


void CHttpRequestTest::SplitHeaderNameTest()
{
	CRequestServletStub servlet;
	CProtocolEngineStub engine;
	imtrest::CHttpRequest request(servlet, engine);

	QVERIFY(ParseParts(request, {"GET / HTTP/1.1\r\nCont", "ent-Type: text/plain\r\n\r\n"}));

	QCOMPARE(request.GetState(), imtrest::IRequest::RS_MESSAGE_COMPLETE);
	QCOMPARE(request.GetHeaders(), QByteArrayList({"content-type"}));
	QCOMPARE(request.GetHeaderValue("content-type"), QByteArray("text/plain"));
}


void CHttpRequestTest::SplitHeaderValueTest()
{
	CRequestServletStub servlet;
	CProtocolEngineStub engine;
	imtrest::CHttpRequest request(servlet, engine);

	QVERIFY(ParseParts(request, {"GET / HTTP/1.1\r\nAuthorization: Bearer 0123", "456789abcdef\r\n\r\n"}));

	QCOMPARE(request.GetState(), imtrest::IRequest::RS_MESSAGE_COMPLETE);
	QCOMPARE(request.GetHeaderValue("authorization"), QByteArray("Bearer 0123456789abcdef"));
}


void CHttpRequestTest::SplitUrlAndHostTest()
{
	CRequestServletStub servlet;
	CProtocolEngineStub engine;
	imtrest::CHttpRequest request(servlet, engine);

	QVERIFY(ParseParts(request, {"GET /api/gra", "phql?product=dogma HTTP/1.1\r\nHost: local", "host:7380\r\n\r\n"}));

	QCOMPARE(request.GetState(), imtrest::IRequest::RS_MESSAGE_COMPLETE);
	QCOMPARE(request.GetCommandId(), QByteArray("/api/graphql"));
	QCOMPARE(request.GetCommandParams().value("product"), QByteArray("dogma"));
	QCOMPARE(request.GetUrl().host(), QStringLiteral("localhost"));
	QCOMPARE(request.GetUrl().port(), 7380);
}


void CHttpRequestTest::RepeatedHeadersTest()
{
	CRequestServletStub servlet;
	CProtocolEngineStub engine;
	imtrest::CHttpRequest request(servlet, engine);

	QVERIFY(request.ExecuteHttpParser(
				"GET / HTTP/1.1\r\n"
				"Accept: text/html\r\n"
				"Cookie: a=1\r\n"
				"Accept: application/json\r\n"
				"Cookie: b=2\r\n"
				"\r\n",
				nullptr));

	QCOMPARE(request.GetState(), imtrest::IRequest::RS_MESSAGE_COMPLETE);
	QCOMPARE(request.GetHeaders().size(), 2);
	QCOMPARE(request.GetHeaderValue("accept"), QByteArray("text/html, application/json"));
	QCOMPARE(request.GetHeaderValue("cookie"), QByteArray("a=1; b=2"));
}


void CHttpRequestTest::EmptyHeaderValueTest()
{
	CRequestServletStub servlet;
	CProtocolEngineStub engine;
	imtrest::CHttpRequest request(servlet, engine);

	QVERIFY(request.ExecuteHttpParser(
				"GET / HTTP/1.1\r\n"
				"X-Empty:\r\n"
				"X-List: \r\n"
				"X-List: b\r\n"
				"\r\n",
				nullptr));

	QCOMPARE(request.GetState(), imtrest::IRequest::RS_MESSAGE_COMPLETE);
	QVERIFY(request.GetHeaders().contains("x-empty"));
	QVERIFY(request.GetHeaderValue("x-empty").isEmpty());
	QCOMPARE(request.GetHeaderValue("x-list"), QByteArray("b"));
}


void CHttpRequestTest::HeaderNamesAreLowerCaseTest()
{
	CRequestServletStub servlet;
	CProtocolEngineStub engine;
	imtrest::CHttpRequest request(servlet, engine);

	QVERIFY(request.ExecuteHttpParser("GET / HTTP/1.1\r\nCONTENT-type: text/plain\r\nX-Mixed-Case: Value\r\n\r\n", nullptr));

	QCOMPARE(request.GetHeaders(), QByteArrayList({"content-type", "x-mixed-case"}));
	// The value is not changed:
	QCOMPARE(request.GetHeaderValue("x-mixed-case"), QByteArray("Value"));
}


void CHttpRequestTest::HeaderValueWithColonsTest()
{
	CRequestServletStub servlet;
	CProtocolEngineStub engine;
	imtrest::CHttpRequest request(servlet, engine);

	QVERIFY(request.ExecuteHttpParser("GET / HTTP/1.1\r\nX-Time: 12:30:00\r\n\r\n", nullptr));

	QCOMPARE(request.GetHeaderValue("x-time"), QByteArray("12:30:00"));
}


void CHttpRequestTest::MissingHeaderTest()
{
	CRequestServletStub servlet;
	CProtocolEngineStub engine;
	imtrest::CHttpRequest request(servlet, engine);

	QVERIFY(request.ExecuteHttpParser("GET / HTTP/1.1\r\nAccept: text/html\r\n\r\n", nullptr));

	QVERIFY(request.GetHeaderValue("authorization").isEmpty());
	// Reading of a missing header does not add it:
	QCOMPARE(request.GetHeaders(), QByteArrayList({"accept"}));
}


void CHttpRequestTest::MethodTypeTest_data()
{
	QTest::addColumn<QByteArray>("method");
	QTest::addColumn<int>("expectedType");

	QTest::newRow("GET") << QByteArray("GET") << int(imtrest::CHttpRequest::MT_GET);
	QTest::newRow("POST") << QByteArray("POST") << int(imtrest::CHttpRequest::MT_POST);
	QTest::newRow("PUT") << QByteArray("PUT") << int(imtrest::CHttpRequest::MT_PUT);
	QTest::newRow("PATCH") << QByteArray("PATCH") << int(imtrest::CHttpRequest::MT_PATCH);
	QTest::newRow("DELETE") << QByteArray("DELETE") << int(imtrest::CHttpRequest::MT_DELETE);
	QTest::newRow("HEAD") << QByteArray("HEAD") << int(imtrest::CHttpRequest::MT_HEAD);
	QTest::newRow("OPTIONS") << QByteArray("OPTIONS") << int(imtrest::CHttpRequest::MT_OPTIONS);
	QTest::newRow("TRACE") << QByteArray("TRACE") << int(imtrest::CHttpRequest::MT_UNKNOWN);
}


void CHttpRequestTest::MethodTypeTest()
{
	QFETCH(QByteArray, method);
	QFETCH(int, expectedType);

	CRequestServletStub servlet;
	CProtocolEngineStub engine;
	imtrest::CHttpRequest request(servlet, engine);

	QVERIFY(request.ExecuteHttpParser(method + " / HTTP/1.1\r\n\r\n", nullptr));

	QCOMPARE(request.GetState(), imtrest::IRequest::RS_MESSAGE_COMPLETE);
	QCOMPARE(int(request.GetMethodType()), expectedType);
}


void CHttpRequestTest::StateTransitionsTest()
{
	CRequestServletStub servlet;
	CProtocolEngineStub engine;
	imtrest::CHttpRequest request(servlet, engine);

	QCOMPARE(request.GetState(), imtrest::IRequest::RS_NON_STARTED);

	QVERIFY(request.ExecuteHttpParser("POST /a HTTP/1.1\r\n", nullptr));
	QCOMPARE(request.GetState(), imtrest::IRequest::RS_URL);

	QVERIFY(request.ExecuteHttpParser("Content-Length: 5\r\n", nullptr));
	QCOMPARE(request.GetState(), imtrest::IRequest::RS_HEADERS);

	QVERIFY(request.ExecuteHttpParser("\r\n", nullptr));
	QCOMPARE(request.GetState(), imtrest::IRequest::RS_HEADERS_COMPLETE);

	QVERIFY(request.ExecuteHttpParser("he", nullptr));
	QCOMPARE(request.GetState(), imtrest::IRequest::RS_BODY);
	QVERIFY(request.GetHeaderValue("content-length") == "5");

	QVERIFY(request.ExecuteHttpParser("llo", nullptr));
	QCOMPARE(request.GetState(), imtrest::IRequest::RS_MESSAGE_COMPLETE);
	QCOMPARE(request.GetBody(), QByteArray("hello"));
}


void CHttpRequestTest::SchemeWithoutSocketTest()
{
	CRequestServletStub servlet;
	CProtocolEngineStub engine;
	imtrest::CHttpRequest request(servlet, engine);

	QVERIFY(request.ExecuteHttpParser("GET /a HTTP/1.1\r\n\r\n", nullptr));

	QCOMPARE(request.GetUrl().scheme(), QStringLiteral("http"));
}


void CHttpRequestTest::CommandParamsTest()
{
	CRequestServletStub servlet;
	CProtocolEngineStub engine;
	imtrest::CHttpRequest request(servlet, engine);

	QVERIFY(request.ExecuteHttpParser("GET /files/list?folder=a%20b&name=%D1%82%D0%B5%D1%81%D1%82&flag=1 HTTP/1.1\r\n\r\n", nullptr));

	QCOMPARE(request.GetCommandId(), QByteArray("/files/list"));

	const imtrest::IRequest::CommandParams params = request.GetCommandParams();
	QCOMPARE(params.size(), 3);
	QCOMPARE(params.value("folder"), QByteArray("a b"));
	QCOMPARE(params.value("name"), QStringLiteral("\u0442\u0435\u0441\u0442").toUtf8());
	QCOMPARE(params.value("flag"), QByteArray("1"));
}


void CHttpRequestTest::CommandParamsWithoutQueryTest()
{
	CRequestServletStub servlet;
	CProtocolEngineStub engine;
	imtrest::CHttpRequest request(servlet, engine);

	QVERIFY(request.ExecuteHttpParser("GET /files/list HTTP/1.1\r\n\r\n", nullptr));

	QCOMPARE(request.GetCommandId(), QByteArray("/files/list"));
	QVERIFY(request.GetCommandParams().isEmpty());
}


void CHttpRequestTest::ChunkedBodyTest()
{
	CRequestServletStub servlet;
	CProtocolEngineStub engine;
	imtrest::CHttpRequest request(servlet, engine);

	QVERIFY(request.ExecuteHttpParser(s_chunkedRequest, nullptr));

	QCOMPARE(request.GetState(), imtrest::IRequest::RS_MESSAGE_COMPLETE);
	QCOMPARE(request.GetBody(), QByteArray("hello world"));
}


void CHttpRequestTest::ChunkedBodySplitAtEveryPositionTest()
{
	CRequestServletStub servlet;
	CProtocolEngineStub engine;

	for (int splitPosition = 1; splitPosition < s_chunkedRequest.size(); ++splitPosition){
		imtrest::CHttpRequest request(servlet, engine);

		QVERIFY(ParseParts(request, {s_chunkedRequest.left(splitPosition), s_chunkedRequest.mid(splitPosition)}));

		QCOMPARE(request.GetState(), imtrest::IRequest::RS_MESSAGE_COMPLETE);
		QCOMPARE(request.GetBody(), QByteArray("hello world"));
		if (QTest::currentTestFailed()){
			qWarning() << "Request split at position" << splitPosition;

			return;
		}
	}
}


void CHttpRequestTest::BinaryBodyTest()
{
	CRequestServletStub servlet;
	CProtocolEngineStub engine;
	imtrest::CHttpRequest request(servlet, engine);

	// Zero bytes and line breaks are the data of the body, not a part of the protocol:
	const QByteArray body("a\0b\r\n\r\nc", 8);

	QVERIFY(request.ExecuteHttpParser("PUT /data HTTP/1.1\r\nContent-Length: 8\r\n\r\n" + body, nullptr));

	QCOMPARE(request.GetState(), imtrest::IRequest::RS_MESSAGE_COMPLETE);
	QCOMPARE(request.GetBody(), body);
}


void CHttpRequestTest::LargeBodyTest()
{
	CRequestServletStub servlet;
	CProtocolEngineStub engine;
	imtrest::CHttpRequest request(servlet, engine);

	const int bodySize = 1024 * 1024;
	QByteArray body(bodySize, 0);
	for (int i = 0; i < bodySize; ++i){
		body[i] = char((i * 13 + 5) & 0xff);
	}

	QVERIFY(request.ExecuteHttpParser("PUT /data HTTP/1.1\r\nContent-Length: " + QByteArray::number(bodySize) + "\r\n\r\n", nullptr));

	// The body comes in the parts of the size that is not related to the size of the data:
	const int partSize = 4093;
	for (int offset = 0; offset < bodySize; offset += partSize){
		QVERIFY(request.ExecuteHttpParser(body.mid(offset, partSize), nullptr));
	}

	QCOMPARE(request.GetState(), imtrest::IRequest::RS_MESSAGE_COMPLETE);
	QCOMPARE(request.GetBody().size(), body.size());
	QVERIFY(request.GetBody() == body);
}


void CHttpRequestTest::LongHeaderValueTest()
{
	CRequestServletStub servlet;
	CProtocolEngineStub engine;
	imtrest::CHttpRequest request(servlet, engine);

	// A long value (a token or a cookie) is below the limit of the parser (80 KiB), but comes in many parts:
	const QByteArray value(70000, 'v');
	const QByteArray requestData = "GET / HTTP/1.1\r\nX-Long: " + value + "\r\n\r\n";

	for (qsizetype offset = 0; offset < requestData.size(); offset += 1000){
		QVERIFY(request.ExecuteHttpParser(requestData.mid(offset, 1000), nullptr));
	}

	QCOMPARE(request.GetState(), imtrest::IRequest::RS_MESSAGE_COMPLETE);
	QVERIFY(request.GetHeaderValue("x-long") == value);
}


void CHttpRequestTest::HeadersAboveLimitTest()
{
	CRequestServletStub servlet;
	CProtocolEngineStub engine;
	imtrest::CHttpRequest request(servlet, engine);

	const QByteArray requestData = "GET / HTTP/1.1\r\nX-Long: " + QByteArray(100000, 'v') + "\r\n\r\n";

	// The parser limits the size of the headers, so a request cannot make the server to collect them without the limit:
	QVERIFY(!request.ExecuteHttpParser(requestData, nullptr));
	QVERIFY(request.GetState() != imtrest::IRequest::RS_MESSAGE_COMPLETE);
}


void CHttpRequestTest::InvalidRequestTest()
{
	CRequestServletStub servlet;
	CProtocolEngineStub engine;
	imtrest::CHttpRequest request(servlet, engine);

	QVERIFY(!request.ExecuteHttpParser("GARBAGE / HTTP/1.1\r\n\r\n", nullptr));
	QVERIFY(request.GetState() != imtrest::IRequest::RS_MESSAGE_COMPLETE);

	// The parser does not recover:
	QVERIFY(!request.ExecuteHttpParser("GET / HTTP/1.1\r\n\r\n", nullptr));
}


void CHttpRequestTest::ResetDataTest()
{
	CRequestServletStub servlet;
	CProtocolEngineStub engine;
	imtrest::CHttpRequest request(servlet, engine);

	QVERIFY(request.ExecuteHttpParser(s_request, nullptr));
	QCOMPARE(request.GetState(), imtrest::IRequest::RS_MESSAGE_COMPLETE);

	QVERIFY(request.ResetData());

	QCOMPARE(request.GetState(), imtrest::IRequest::RS_NON_STARTED);
	QVERIFY(request.GetHeaders().isEmpty());
	QVERIFY(request.GetBody().isEmpty());
	QVERIFY(request.GetUrl().isEmpty());
	QVERIFY(request.GetCommandId().isEmpty());
	QVERIFY(request.GetCommandParams().isEmpty());
}


void CHttpRequestTest::SecondRequestOnSameObjectTest()
{
	CRequestServletStub servlet;
	CProtocolEngineStub engine;
	imtrest::CHttpRequest request(servlet, engine);

	// The socket reuses the object of the request for the next request of the connection:
	QVERIFY(request.ExecuteHttpParser(s_request, nullptr));
	VerifyParsedRequest(request);

	QVERIFY(request.ResetData());

	QVERIFY(request.ExecuteHttpParser("GET /second?x=1 HTTP/1.1\r\nAccept: text/plain\r\n\r\n", nullptr));

	QCOMPARE(request.GetState(), imtrest::IRequest::RS_MESSAGE_COMPLETE);
	QCOMPARE(request.GetMethodType(), imtrest::CHttpRequest::MT_GET);
	QCOMPARE(request.GetCommandId(), QByteArray("/second"));
	QCOMPARE(request.GetCommandParams().value("x"), QByteArray("1"));
	// Nothing is left from the first request:
	QCOMPARE(request.GetHeaders(), QByteArrayList({"accept"}));
	QCOMPARE(request.GetHeaderValue("accept"), QByteArray("text/plain"));
	QVERIFY(request.GetBody().isEmpty());
}


void CHttpRequestTest::RequestIdTest()
{
	CRequestServletStub servlet;
	CProtocolEngineStub engine;
	imtrest::CHttpRequest firstRequest(servlet, engine);
	imtrest::CHttpRequest secondRequest(servlet, engine);

	QVERIFY(!firstRequest.GetRequestId().isEmpty());
	QVERIFY(firstRequest.GetRequestId() != secondRequest.GetRequestId());

	// The ID identifies the connection and survives the next request:
	const QByteArray requestId = firstRequest.GetRequestId();
	QVERIFY(firstRequest.ResetData());
	QCOMPARE(firstRequest.GetRequestId(), requestId);

	QCOMPARE(&firstRequest.GetProtocolEngine(), static_cast<const imtrest::IProtocolEngine*>(&engine));
}


void CHttpRequestTest::SettersTest()
{
	CRequestServletStub servlet;
	CProtocolEngineStub engine;
	imtrest::CHttpRequest request(servlet, engine);

	request.SetHeader("X-Test-Header", "value");
	QCOMPARE(request.GetHeaders(), QByteArrayList({"x-test-header"}));
	QCOMPARE(request.GetHeaderValue("x-test-header"), QByteArray("value"));

	request.SetBody("body");
	QCOMPARE(request.GetBody(), QByteArray("body"));

	request.SetCommandId("/command");
	QCOMPARE(request.GetCommandId(), QByteArray("/command"));

	request.SetMethodType(imtrest::CHttpRequest::MT_POST);
	QCOMPARE(request.GetMethodType(), imtrest::CHttpRequest::MT_POST);
	request.SetMethodType(imtrest::CHttpRequest::MT_GET);
	QCOMPARE(request.GetMethodType(), imtrest::CHttpRequest::MT_GET);
	request.SetMethodType(imtrest::CHttpRequest::MT_PUT);
	QCOMPARE(request.GetMethodType(), imtrest::CHttpRequest::MT_PUT);
}


void CHttpRequestTest::ConnectionPersistenceTest_data()
{
	QTest::addColumn<QByteArray>("request");
	QTest::addColumn<bool>("isPersistent");

	// RFC 9112, section 9.3: HTTP/1.1 is persistent unless the client says "close", HTTP/1.0 only with "keep-alive"
	QTest::newRow("HTTP/1.1") << QByteArray("GET / HTTP/1.1\r\nHost: a\r\n\r\n") << true;
	QTest::newRow("HTTP/1.1 close") << QByteArray("GET / HTTP/1.1\r\nConnection: close\r\n\r\n") << false;
	QTest::newRow("HTTP/1.1 Close") << QByteArray("GET / HTTP/1.1\r\nConnection: Close\r\n\r\n") << false;
	QTest::newRow("HTTP/1.1 keep-alive") << QByteArray("GET / HTTP/1.1\r\nConnection: keep-alive\r\n\r\n") << true;
	QTest::newRow("HTTP/1.0") << QByteArray("GET / HTTP/1.0\r\n\r\n") << false;
	QTest::newRow("HTTP/1.0 keep-alive") << QByteArray("GET / HTTP/1.0\r\nConnection: keep-alive\r\n\r\n") << true;
	QTest::newRow("HTTP/1.0 close") << QByteArray("GET / HTTP/1.0\r\nConnection: close\r\n\r\n") << false;
	QTest::newRow("POST with body") << QByteArray("POST / HTTP/1.1\r\nContent-Length: 2\r\nConnection: close\r\n\r\nhi") << false;
}


void CHttpRequestTest::ConnectionPersistenceTest()
{
	QFETCH(QByteArray, request);
	QFETCH(bool, isPersistent);

	CRequestServletStub servlet;
	CProtocolEngineStub engine;
	imtrest::CHttpRequest httpRequest(servlet, engine);

	QVERIFY(httpRequest.ExecuteHttpParser(request, nullptr));
	QCOMPARE(httpRequest.GetState(), imtrest::IRequest::RS_MESSAGE_COMPLETE);

	QCOMPARE(httpRequest.IsConnectionPersistent(), isPersistent);
}


void CHttpRequestTest::ConnectionPersistenceSplitTest()
{
	const QByteArray request("GET / HTTP/1.1\r\nHost: a\r\nConnection: close\r\n\r\n");

	// The header can be split at any position between the reads:
	for (qsizetype position = 1; position < request.size(); ++position){
		CRequestServletStub servlet;
		CProtocolEngineStub engine;
		imtrest::CHttpRequest httpRequest(servlet, engine);

		QVERIFY(httpRequest.ExecuteHttpParser(request.left(position), nullptr));
		QVERIFY(httpRequest.ExecuteHttpParser(request.mid(position), nullptr));

		QVERIFY2(!httpRequest.IsConnectionPersistent(), qPrintable(QString::number(position)));
	}
}


void CHttpRequestTest::ResponseBodyOmittedTest_data()
{
	QTest::addColumn<QByteArray>("request");
	QTest::addColumn<bool>("isOmitted");

	// RFC 9110, section 9.3.2: the response to HEAD has no content
	QTest::newRow("HEAD") << QByteArray("HEAD / HTTP/1.1\r\n\r\n") << true;
	QTest::newRow("GET") << QByteArray("GET / HTTP/1.1\r\n\r\n") << false;
	QTest::newRow("POST") << QByteArray("POST / HTTP/1.1\r\nContent-Length: 2\r\n\r\nhi") << false;
	QTest::newRow("OPTIONS") << QByteArray("OPTIONS / HTTP/1.1\r\n\r\n") << false;
}


void CHttpRequestTest::ResponseBodyOmittedTest()
{
	QFETCH(QByteArray, request);
	QFETCH(bool, isOmitted);

	CRequestServletStub servlet;
	CProtocolEngineStub engine;
	imtrest::CHttpRequest httpRequest(servlet, engine);

	QVERIFY(httpRequest.ExecuteHttpParser(request, nullptr));

	QCOMPARE(httpRequest.IsResponseBodyOmitted(), isOmitted);
}


void CHttpRequestTest::TcpRequestIsPersistentTest()
{
	CRequestServletStub servlet;
	CProtocolEngineStub engine;

	// A message of the raw TCP channel is not an HTTP request, the channel is never closed by the request:
	imtrest::CTcpRequest tcpRequest(servlet, engine);

	QVERIFY(tcpRequest.IsConnectionPersistent());
	QVERIFY(!tcpRequest.IsResponseBodyOmitted());
}


void CHttpRequestTest::RequestStopsAtItsEndTest_data()
{
	QTest::addColumn<QByteArray>("request");
	QTest::addColumn<QByteArray>("body");

	QTest::newRow("no body") << QByteArray("GET /a HTTP/1.1\r\nHost: x\r\n\r\n") << QByteArray();
	QTest::newRow("body") << QByteArray("POST /a HTTP/1.1\r\nContent-Length: 5\r\n\r\nhello") << QByteArray("hello");
	QTest::newRow("empty body") << QByteArray("POST /a HTTP/1.1\r\nContent-Length: 0\r\n\r\n") << QByteArray();
	QTest::newRow("chunked body") << QByteArray("POST /a HTTP/1.1\r\nTransfer-Encoding: chunked\r\n\r\n5\r\nhello\r\n0\r\n\r\n") << QByteArray("hello");
	QTest::newRow("Connection: close") << QByteArray("GET /a HTTP/1.1\r\nConnection: close\r\n\r\n") << QByteArray();
	QTest::newRow("HTTP/1.0") << QByteArray("GET /a HTTP/1.0\r\n\r\n") << QByteArray();
}


void CHttpRequestTest::RequestStopsAtItsEndTest()
{
	QFETCH(QByteArray, request);
	QFETCH(QByteArray, body);

	CRequestServletStub servlet;
	CProtocolEngineStub engine;
	imtrest::CHttpRequest httpRequest(servlet, engine);

	// The next request follows in the same data:
	const QByteArray nextRequest("GET /b HTTP/1.1\r\n\r\n");

	qsizetype consumedCount = 0;
	QVERIFY(httpRequest.ExecuteHttpParser(request + nextRequest, nullptr, consumedCount));

	QCOMPARE(consumedCount, request.size());
	QCOMPARE(httpRequest.GetState(), imtrest::IRequest::RS_MESSAGE_COMPLETE);
	QCOMPARE(httpRequest.GetCommandId(), QByteArray("/a"));
	QCOMPARE(httpRequest.GetBody(), body);
}


void CHttpRequestTest::ConsumedCountOfSingleRequestTest()
{
	CRequestServletStub servlet;
	CProtocolEngineStub engine;
	imtrest::CHttpRequest httpRequest(servlet, engine);

	qsizetype consumedCount = 0;
	QVERIFY(httpRequest.ExecuteHttpParser(s_request, nullptr, consumedCount));

	QCOMPARE(consumedCount, s_request.size());
	QCOMPARE(httpRequest.GetState(), imtrest::IRequest::RS_MESSAGE_COMPLETE);
}


void CHttpRequestTest::ConsumedCountOfIncompleteRequestTest()
{
	CRequestServletStub servlet;
	CProtocolEngineStub engine;
	imtrest::CHttpRequest httpRequest(servlet, engine);

	// All data of an incomplete request belong to it:
	const QByteArray part = s_request.left(s_request.size() - 3);

	qsizetype consumedCount = 0;
	QVERIFY(httpRequest.ExecuteHttpParser(part, nullptr, consumedCount));

	QCOMPARE(consumedCount, part.size());
	QVERIFY(httpRequest.GetState() != imtrest::IRequest::RS_MESSAGE_COMPLETE);
}


void CHttpRequestTest::CompleteRequestTakesNothingMoreTest()
{
	CRequestServletStub servlet;
	CProtocolEngineStub engine;
	imtrest::CHttpRequest httpRequest(servlet, engine);

	QVERIFY(httpRequest.ExecuteHttpParser("GET /a HTTP/1.1\r\n\r\n", nullptr));

	// The data of the next request do not change the complete request:
	qsizetype consumedCount = -1;
	QVERIFY(httpRequest.ExecuteHttpParser("GET /b HTTP/1.1\r\n\r\n", nullptr, consumedCount));

	QCOMPARE(consumedCount, qsizetype(0));
	QCOMPARE(httpRequest.GetState(), imtrest::IRequest::RS_MESSAGE_COMPLETE);
	QCOMPARE(httpRequest.GetCommandId(), QByteArray("/a"));
}


void CHttpRequestTest::NextRequestIsParsedByAnotherObjectTest()
{
	CRequestServletStub servlet;
	CProtocolEngineStub engine;
	imtrest::CHttpRequest firstRequest(servlet, engine);
	imtrest::CHttpRequest secondRequest(servlet, engine);

	const QByteArray firstData("POST /a HTTP/1.1\r\nContent-Length: 2\r\n\r\nhi");
	const QByteArray secondData("GET /b?x=1 HTTP/1.1\r\nAccept: text/plain\r\n\r\n");

	qsizetype consumedCount = 0;
	QVERIFY(firstRequest.ExecuteHttpParser(firstData + secondData, nullptr, consumedCount));
	QCOMPARE(consumedCount, firstData.size());

	// The rest of the data is the whole second request:
	QVERIFY(secondRequest.ExecuteHttpParser((firstData + secondData).mid(consumedCount), nullptr, consumedCount));

	QCOMPARE(consumedCount, secondData.size());
	QCOMPARE(secondRequest.GetState(), imtrest::IRequest::RS_MESSAGE_COMPLETE);
	QCOMPARE(secondRequest.GetCommandId(), QByteArray("/b"));
	QCOMPARE(secondRequest.GetCommandParams().value("x"), QByteArray("1"));
	QVERIFY(secondRequest.GetBody().isEmpty());

	// The first request is not changed by it:
	QCOMPARE(firstRequest.GetCommandId(), QByteArray("/a"));
	QCOMPARE(firstRequest.GetBody(), QByteArray("hi"));
}


void CHttpRequestTest::PipelinedStreamAtEveryChunkSizeTest()
{
	const QList<QByteArray> requests = {
		"GET /a HTTP/1.1\r\nHost: x\r\n\r\n",
		"POST /b HTTP/1.1\r\nContent-Length: 5\r\n\r\nhello",
		"POST /c HTTP/1.1\r\nTransfer-Encoding: chunked\r\n\r\n3\r\nabc\r\n0\r\n\r\n",
		"GET /d HTTP/1.1\r\n\r\n"};

	QByteArray stream;
	for (const QByteArray& request : requests){
		stream += request;
	}

	CRequestServletStub servlet;
	CProtocolEngineStub engine;

	// The socket collects the data in portions of any size, hands each complete request to the handler and goes on
	// with the rest of the data:
	for (qsizetype chunkSize = 1; chunkSize <= stream.size(); ++chunkSize){
		QList<QByteArray> commands;
		QList<QByteArray> bodies;

		std::unique_ptr<imtrest::CHttpRequest> requestPtr(new imtrest::CHttpRequest(servlet, engine));

		QByteArray buffer;
		for (qsizetype position = 0; position < stream.size(); position += chunkSize){
			buffer += stream.mid(position, chunkSize);

			while (!buffer.isEmpty()){
				qsizetype consumedCount = 0;
				QVERIFY2(requestPtr->ExecuteHttpParser(buffer, nullptr, consumedCount), qPrintable(QString::number(chunkSize)));

				buffer.remove(0, consumedCount);

				if (requestPtr->GetState() != imtrest::IRequest::RS_MESSAGE_COMPLETE){
					QVERIFY(buffer.isEmpty());

					break;
				}

				commands.append(requestPtr->GetCommandId());
				bodies.append(requestPtr->GetBody());

				requestPtr.reset(new imtrest::CHttpRequest(servlet, engine));
			}
		}

		QVERIFY2(commands == QByteArrayList({"/a", "/b", "/c", "/d"}), qPrintable(QString::number(chunkSize)));
		QVERIFY2(bodies == QByteArrayList({"", "hello", "abc", ""}), qPrintable(QString::number(chunkSize)));
	}
}


void CHttpRequestTest::ParseDeviceDataTakesOnlyRequestTest()
{
	CRequestServletStub servlet;
	CProtocolEngineStub engine;
	imtrest::CHttpRequest firstRequest(servlet, engine);
	imtrest::CHttpRequest secondRequest(servlet, engine);

	const QByteArray firstData("POST /a HTTP/1.1\r\nContent-Length: 2\r\n\r\nhi");
	const QByteArray secondData("GET /b HTTP/1.1\r\n\r\n");

	QByteArray data = firstData + secondData;
	QBuffer device(&data);
	QVERIFY(device.open(QIODevice::ReadOnly));

	QVERIFY(firstRequest.ParseDeviceData(device));

	// Only the first request is taken from the device:
	QCOMPARE(firstRequest.GetState(), imtrest::IRequest::RS_MESSAGE_COMPLETE);
	QCOMPARE(firstRequest.GetCommandId(), QByteArray("/a"));
	QCOMPARE(firstRequest.GetBody(), QByteArray("hi"));
	QCOMPARE(device.bytesAvailable(), qint64(secondData.size()));

	QVERIFY(secondRequest.ParseDeviceData(device));

	QCOMPARE(secondRequest.GetState(), imtrest::IRequest::RS_MESSAGE_COMPLETE);
	QCOMPARE(secondRequest.GetCommandId(), QByteArray("/b"));
	QCOMPARE(device.bytesAvailable(), qint64(0));
}


void CHttpRequestTest::ParseDeviceDataOfIncompleteRequestTest()
{
	CRequestServletStub servlet;
	CProtocolEngineStub engine;
	imtrest::CHttpRequest request(servlet, engine);

	QByteArray firstPart("POST /a HTTP/1.1\r\nContent-Length: 5\r\n\r\nhe");
	QBuffer firstDevice(&firstPart);
	QVERIFY(firstDevice.open(QIODevice::ReadOnly));

	QVERIFY(request.ParseDeviceData(firstDevice));

	// All data of an incomplete request are taken:
	QVERIFY(request.GetState() != imtrest::IRequest::RS_MESSAGE_COMPLETE);
	QCOMPARE(firstDevice.bytesAvailable(), qint64(0));

	QByteArray secondPart("llo");
	QBuffer secondDevice(&secondPart);
	QVERIFY(secondDevice.open(QIODevice::ReadOnly));

	QVERIFY(request.ParseDeviceData(secondDevice));

	QCOMPARE(request.GetState(), imtrest::IRequest::RS_MESSAGE_COMPLETE);
	QCOMPARE(request.GetBody(), QByteArray("hello"));
}


void CHttpRequestTest::ParseDeviceDataOfEmptyDeviceTest()
{
	CRequestServletStub servlet;
	CProtocolEngineStub engine;
	imtrest::CHttpRequest request(servlet, engine);

	QByteArray data;
	QBuffer device(&data);
	QVERIFY(device.open(QIODevice::ReadOnly));

	QVERIFY(request.ParseDeviceData(device));
	QCOMPARE(request.GetState(), imtrest::IRequest::RS_NON_STARTED);
}


void CHttpRequestTest::ParseDeviceDataOfInvalidRequestTest()
{
	CRequestServletStub servlet;
	CProtocolEngineStub engine;
	imtrest::CHttpRequest request(servlet, engine);

	QByteArray data("GARBAGE / HTTP/1.1\r\n\r\n");
	QBuffer device(&data);
	QVERIFY(device.open(QIODevice::ReadOnly));

	QVERIFY(!request.ParseDeviceData(device));
	QVERIFY(request.GetState() != imtrest::IRequest::RS_MESSAGE_COMPLETE);
}


void CHttpRequestTest::ParseDeviceDataOfSocketTest()
{
	// The socket is the device of the server: the data of the next request stay in its buffer
	QTcpServer server;
	QVERIFY(server.listen(QHostAddress::LocalHost, 0));

	QTcpSocket client;
	client.connectToHost(QHostAddress::LocalHost, server.serverPort());
	QVERIFY(server.waitForNewConnection(5000));

	QTcpSocket* serverSocketPtr = server.nextPendingConnection();
	QVERIFY(serverSocketPtr != nullptr);

	const QByteArray firstData("POST /a HTTP/1.1\r\nContent-Length: 2\r\n\r\nhi");
	const QByteArray secondData("GET /b HTTP/1.1\r\n\r\n");

	QVERIFY(client.waitForConnected(5000));
	client.write(firstData + secondData);
	QVERIFY(client.waitForBytesWritten(5000));

	QTRY_VERIFY_WITH_TIMEOUT(serverSocketPtr->bytesAvailable() >= firstData.size() + secondData.size(), 5000);

	CRequestServletStub servlet;
	CProtocolEngineStub engine;
	imtrest::CHttpRequest firstRequest(servlet, engine);
	imtrest::CHttpRequest secondRequest(servlet, engine);

	QVERIFY(firstRequest.ParseDeviceData(*serverSocketPtr));

	QCOMPARE(firstRequest.GetState(), imtrest::IRequest::RS_MESSAGE_COMPLETE);
	QCOMPARE(firstRequest.GetBody(), QByteArray("hi"));
	QCOMPARE(serverSocketPtr->bytesAvailable(), qint64(secondData.size()));

	QVERIFY(secondRequest.ParseDeviceData(*serverSocketPtr));

	QCOMPARE(secondRequest.GetState(), imtrest::IRequest::RS_MESSAGE_COMPLETE);
	QCOMPARE(secondRequest.GetCommandId(), QByteArray("/b"));
	QCOMPARE(serverSocketPtr->bytesAvailable(), qint64(0));
}


I_ADD_TEST(CHttpRequestTest);


