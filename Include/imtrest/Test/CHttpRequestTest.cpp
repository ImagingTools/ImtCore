// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include "CHttpRequestTest.h"


// ImtCore includes
#include <imtrest/CHttpRequest.h>
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


I_ADD_TEST(CHttpRequestTest);


