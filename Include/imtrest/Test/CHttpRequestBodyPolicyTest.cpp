// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include "CHttpRequestBodyPolicyTest.h"


// ImtCore includes
#include <imtrest/CHttpRequest.h>
#include <imtrest/CHttpResponse.h>
#include <imtrest/IProtocolEngine.h>
#include <imtrest/IHttpRequestBodyPolicy.h>


namespace
{


typedef imtrest::IRequest IRequest;
typedef imtrest::IProtocolEngine IProtocolEngine;
typedef imtrest::IHttpRequestBodyPolicy IHttpRequestBodyPolicy;


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


/**
	Engine that creates the responses for the statuses of the refusals.
*/
class CProtocolEngineStub: virtual public imtrest::IProtocolEngine
{
public:
	explicit CProtocolEngineStub(bool isResponseCreated = true)
		:m_isResponseCreated(isResponseCreated)
	{
	}

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
		case SC_CONTINUE:
			protocolStatusCode = 100;
			statusCodeLiteral = "Continue";
			return true;

		case SC_UNAUTHORIZED:
			protocolStatusCode = 401;
			statusCodeLiteral = "Unauthorized";
			return true;

		case SC_FORBIDDEN:
			protocolStatusCode = 403;
			statusCodeLiteral = "Forbidden";
			return true;

		case SC_PAYLOAD_TOO_LARGE:
			protocolStatusCode = 413;
			statusCodeLiteral = "Payload Too Large";
			return true;

		case SC_EXPECTATION_FAILED:
			protocolStatusCode = 417;
			statusCodeLiteral = "Expectation Failed";
			return true;

		case SC_INTERNAL_SERVER_ERROR:
			protocolStatusCode = 500;
			statusCodeLiteral = "Internal Server Error";
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
				int statusCode,
				const QByteArray& data,
				const QByteArray& dataTypeId) const override
	{
		if (!m_isResponseCreated){
			return nullptr;
		}

		return new imtrest::CHttpResponse(statusCode, data, dataTypeId, *this);
	}

private:
	bool m_isResponseCreated;
};


/**
	Policy with the decision that is set by the test, it remembers what it saw.
*/
class CPolicyStub: virtual public imtrest::IHttpRequestBodyPolicy
{
public:
	IHttpRequestBodyPolicy::BodyDecision decision;

	mutable int callCount = 0;
	mutable QByteArray lastCommandId;
	mutable QString lastQuery;
	mutable QByteArray lastToken;
	mutable imtrest::CHttpRequest::MethodType lastMethod = imtrest::CHttpRequest::MT_UNKNOWN;
	mutable bool isBodyEmpty = false;

	void Refuse(int statusCode)
	{
		decision.mode = IHttpRequestBodyPolicy::BM_REJECT;
		decision.rejectStatusCode = statusCode;
	}

	// reimplemented (imtrest::IHttpRequestBodyPolicy)
	BodyDecision DecideBody(const imtrest::CHttpRequest& requestHead) const override
	{
		++callCount;
		lastCommandId = requestHead.GetCommandId();
		lastQuery = requestHead.GetUrl().query();
		lastToken = requestHead.GetHeaderValue("x-token");
		lastMethod = requestHead.GetMethodType();
		isBodyEmpty = requestHead.GetBody().isEmpty();

		return decision;
	}
};


/**
	Request with the given length of the body (empty - no Content-Length) and other headers, without the body.
*/
QByteArray CreateHead(const QByteArray& contentLength, const QByteArray& otherHeaders = QByteArray(), const QByteArray& version = "HTTP/1.1")
{
	QByteArray retVal("PUT /api/upload?x=1 " + version + "\r\nHost: localhost:7380\r\n");

	if (!contentLength.isEmpty()){
		retVal += "Content-Length: " + contentLength + "\r\n";
	}

	retVal += otherHeaders;
	retVal += "\r\n";

	return retVal;
}


const QByteArray s_expectContinue("Expect: 100-continue\r\n");

const QByteArray s_chunkedBody(
			"5\r\n"
			"hello\r\n"
			"6\r\n"
			" world\r\n"
			"0\r\n"
			"\r\n");


int TakeHeadAction(imtrest::CHttpRequest& request, QByteArray& message)
{
	return int(request.TakeHeadAction(message));
}


bool IsFinalResponse(const QByteArray& message, const QByteArray& statusLine)
{
	return message.startsWith("HTTP/1.1 " + statusLine + "\r\n")
				&& message.contains("\r\nDate: ")
				&& message.contains("\r\nConnection: close\r\n")
				&& message.contains("\r\nContent-Length: 0\r\n")
				&& message.endsWith("\r\n\r\n");
}


} // namespace


// private slots

void CHttpRequestBodyPolicyTest::NoPolicyNoLimitTest()
{
	CRequestServletStub servlet;
	CProtocolEngineStub engine;
	imtrest::CHttpRequest request(servlet, engine);

	request.SetDefaultMaxBodySize(0);

	QVERIFY(request.ExecuteHttpParser(CreateHead("5") + "hello", nullptr));
	QCOMPARE(request.GetState(), IRequest::RS_MESSAGE_COMPLETE);
	QCOMPARE(request.GetBody(), QByteArray("hello"));

	QByteArray message("x");
	QCOMPARE(TakeHeadAction(request, message), int(IRequest::HA_NONE));
	QVERIFY(message.isEmpty());
}


void CHttpRequestBodyPolicyTest::HugeContentLengthDoesNotReserveMemoryTest()
{
	CRequestServletStub servlet;
	CProtocolEngineStub engine;
	imtrest::CHttpRequest request(servlet, engine);

	// The length is announced by the client, 4 GB do not fit into int. The memory is not reserved for it.
	QVERIFY(request.ExecuteHttpParser(CreateHead("4000000000") + "0123456789", nullptr));

	QCOMPARE(request.GetState(), IRequest::RS_BODY);

	const QByteArray body = request.GetBody();
	QCOMPARE(body.size(), qsizetype(10));
	QVERIFY(body.capacity() <= 17 * 1024 * 1024);
}


void CHttpRequestBodyPolicyTest::DefaultLimitRefusesBigContentLengthTest()
{
	CRequestServletStub servlet;
	CProtocolEngineStub engine;
	imtrest::CHttpRequest request(servlet, engine);

	request.SetDefaultMaxBodySize(100);

	// The body is not sent, the decision is made by the headers
	QVERIFY(request.ExecuteHttpParser(CreateHead("101"), nullptr));

	QByteArray message;
	QCOMPARE(TakeHeadAction(request, message), int(IRequest::HA_REJECT));
	QVERIFY2(IsFinalResponse(message, "413 Payload Too Large"), message.constData());

	QCOMPARE(request.GetState(), IRequest::RS_HEADERS_COMPLETE);
}


void CHttpRequestBodyPolicyTest::ContentLengthEqualToLimitTest()
{
	CRequestServletStub servlet;
	CProtocolEngineStub engine;
	imtrest::CHttpRequest request(servlet, engine);

	request.SetDefaultMaxBodySize(5);

	QVERIFY(request.ExecuteHttpParser(CreateHead("5") + "hello", nullptr));

	QByteArray message;
	QCOMPARE(TakeHeadAction(request, message), int(IRequest::HA_NONE));
	QCOMPARE(request.GetState(), IRequest::RS_MESSAGE_COMPLETE);
	QCOMPARE(request.GetBody(), QByteArray("hello"));
}


void CHttpRequestBodyPolicyTest::BodyAfterRefusalIsNotReceivedTest()
{
	CRequestServletStub servlet;
	CProtocolEngineStub engine;
	imtrest::CHttpRequest request(servlet, engine);

	request.SetDefaultMaxBodySize(5);

	// The client has sent the body with the headers, it is not kept
	QVERIFY(request.ExecuteHttpParser(CreateHead("50") + QByteArray(50, 'x'), nullptr));

	QByteArray message;
	QCOMPARE(TakeHeadAction(request, message), int(IRequest::HA_REJECT));
	QVERIFY(request.GetBody().isEmpty());
	QCOMPARE(request.GetState(), IRequest::RS_HEADERS_COMPLETE);
}


void CHttpRequestBodyPolicyTest::ChunkedBodyOverLimitTest()
{
	CRequestServletStub servlet;
	CProtocolEngineStub engine;
	imtrest::CHttpRequest request(servlet, engine);

	request.SetDefaultMaxBodySize(8);

	// The length of the body is not known in advance, the limit is checked while the body is received
	QVERIFY(request.ExecuteHttpParser(CreateHead(QByteArray(), "Transfer-Encoding: chunked\r\n") + s_chunkedBody, nullptr));

	QByteArray message;
	QCOMPARE(TakeHeadAction(request, message), int(IRequest::HA_REJECT));
	QVERIFY2(IsFinalResponse(message, "413 Payload Too Large"), message.constData());

	QVERIFY(request.GetState() != IRequest::RS_MESSAGE_COMPLETE);
	QVERIFY(request.GetBody().size() <= 8);
}


void CHttpRequestBodyPolicyTest::ChunkedBodyWithinLimitTest()
{
	CRequestServletStub servlet;
	CProtocolEngineStub engine;
	imtrest::CHttpRequest request(servlet, engine);

	request.SetDefaultMaxBodySize(11);

	QVERIFY(request.ExecuteHttpParser(CreateHead(QByteArray(), "Transfer-Encoding: chunked\r\n") + s_chunkedBody, nullptr));

	QByteArray message;
	QCOMPARE(TakeHeadAction(request, message), int(IRequest::HA_NONE));
	QCOMPARE(request.GetState(), IRequest::RS_MESSAGE_COMPLETE);
	QCOMPARE(request.GetBody(), QByteArray("hello world"));
}


void CHttpRequestBodyPolicyTest::LimitOfPolicyTest_data()
{
	QTest::addColumn<qint64>("policyLimit");
	QTest::addColumn<qint64>("serverLimit");
	QTest::addColumn<bool>("isRefused");

	// The length of the body of the request is 50
	QTest::newRow("policy limit is exceeded") << qint64(10) << qint64(1000) << true;
	QTest::newRow("policy limit is not exceeded") << qint64(100) << qint64(10) << false;
	QTest::newRow("limit of the policy is equal to the length") << qint64(50) << qint64(10) << false;
	QTest::newRow("policy has no limit") << qint64(0) << qint64(10) << false;
	QTest::newRow("limit of the server is exceeded") << qint64(-1) << qint64(10) << true;
	QTest::newRow("limit of the server is not exceeded") << qint64(-1) << qint64(100) << false;
	QTest::newRow("no limit of the server") << qint64(-1) << qint64(0) << false;
}


void CHttpRequestBodyPolicyTest::LimitOfPolicyTest()
{
	QFETCH(qint64, policyLimit);
	QFETCH(qint64, serverLimit);
	QFETCH(bool, isRefused);

	CRequestServletStub servlet;
	CProtocolEngineStub engine;
	imtrest::CHttpRequest request(servlet, engine);

	CPolicyStub policy;
	policy.decision.maxBodySize = policyLimit;

	request.SetBodyPolicy(policy);
	request.SetDefaultMaxBodySize(serverLimit);

	QVERIFY(request.ExecuteHttpParser(CreateHead("50") + QByteArray(50, 'x'), nullptr));

	QByteArray message;
	if (isRefused){
		QCOMPARE(TakeHeadAction(request, message), int(IRequest::HA_REJECT));
		QVERIFY2(IsFinalResponse(message, "413 Payload Too Large"), message.constData());
	}
	else{
		QCOMPARE(TakeHeadAction(request, message), int(IRequest::HA_NONE));
		QCOMPARE(request.GetState(), IRequest::RS_MESSAGE_COMPLETE);
		QCOMPARE(request.GetBody().size(), qsizetype(50));
	}
}


void CHttpRequestBodyPolicyTest::PolicyRefusesRequestTest()
{
	CRequestServletStub servlet;
	CProtocolEngineStub engine;
	imtrest::CHttpRequest request(servlet, engine);

	CPolicyStub policy;
	policy.Refuse(IProtocolEngine::SC_UNAUTHORIZED);
	policy.decision.rejectHeaders.insert("WWW-Authenticate", "Bearer");

	request.SetBodyPolicy(policy);

	QVERIFY(request.ExecuteHttpParser(CreateHead("10"), nullptr));

	QByteArray message;
	QCOMPARE(TakeHeadAction(request, message), int(IRequest::HA_REJECT));
	QVERIFY2(IsFinalResponse(message, "401 Unauthorized"), message.constData());
	QVERIFY2(message.contains("\r\nWWW-Authenticate: Bearer\r\n"), message.constData());
}


void CHttpRequestBodyPolicyTest::PolicyRefusalWithoutStatusTest()
{
	CRequestServletStub servlet;
	CProtocolEngineStub engine;
	imtrest::CHttpRequest request(servlet, engine);

	// A policy has to set the status, otherwise it is a mistake of the server
	CPolicyStub policy;
	policy.decision.mode = IHttpRequestBodyPolicy::BM_REJECT;

	request.SetBodyPolicy(policy);

	QVERIFY(request.ExecuteHttpParser(CreateHead("10"), nullptr));

	QByteArray message;
	QCOMPARE(TakeHeadAction(request, message), int(IRequest::HA_REJECT));
	QVERIFY2(message.startsWith("HTTP/1.1 500 Internal Server Error\r\n"), message.constData());
}


void CHttpRequestBodyPolicyTest::PolicySeesHeadOfRequestTest()
{
	CRequestServletStub servlet;
	CProtocolEngineStub engine;
	imtrest::CHttpRequest request(servlet, engine);

	CPolicyStub policy;
	request.SetBodyPolicy(policy);

	QVERIFY(request.ExecuteHttpParser(
				"POST /api/files?x=1 HTTP/1.1\r\n"
				"Host: localhost:7380\r\n"
				"Content-Length: 3\r\n"
				"X-Token: abc\r\n"
				"\r\n"
				"abc",
				nullptr));

	QCOMPARE(policy.callCount, 1);
	QCOMPARE(policy.lastCommandId, QByteArray("/api/files"));
	QCOMPARE(policy.lastQuery, QString("x=1"));
	QCOMPARE(policy.lastToken, QByteArray("abc"));
	QCOMPARE(policy.lastMethod, imtrest::CHttpRequest::MT_POST);

	// The body is received after the decision
	QVERIFY(policy.isBodyEmpty);
	QCOMPARE(request.GetBody(), QByteArray("abc"));
}


void CHttpRequestBodyPolicyTest::PolicyIsCalledOnceTest()
{
	CRequestServletStub servlet;
	CProtocolEngineStub engine;
	imtrest::CHttpRequest request(servlet, engine);

	CPolicyStub policy;
	request.SetBodyPolicy(policy);

	// The data come by bytes
	const QByteArray data = CreateHead("5") + "hello";
	for (int i = 0; i < data.size(); ++i){
		QVERIFY(request.ExecuteHttpParser(data.mid(i, 1), nullptr));
	}

	QCOMPARE(request.GetState(), IRequest::RS_MESSAGE_COMPLETE);
	QCOMPARE(policy.callCount, 1);
}


void CHttpRequestBodyPolicyTest::PolicyIsNotCalledBeforeEndOfHeadersTest()
{
	CRequestServletStub servlet;
	CProtocolEngineStub engine;
	imtrest::CHttpRequest request(servlet, engine);

	CPolicyStub policy;
	request.SetBodyPolicy(policy);

	QVERIFY(request.ExecuteHttpParser("PUT /a HTTP/1.1\r\nContent-Length: 5\r\n", nullptr));
	QCOMPARE(policy.callCount, 0);

	QVERIFY(request.ExecuteHttpParser("\r\n", nullptr));
	QCOMPARE(policy.callCount, 1);
}


void CHttpRequestBodyPolicyTest::PolicyDecidesAlsoForRequestWithoutBodyTest()
{
	CRequestServletStub servlet;
	CProtocolEngineStub engine;
	imtrest::CHttpRequest request(servlet, engine);

	// The policy can refuse a request by its address, for example if there is no access
	CPolicyStub policy;
	policy.Refuse(IProtocolEngine::SC_FORBIDDEN);

	request.SetBodyPolicy(policy);

	QVERIFY(request.ExecuteHttpParser("GET /secret HTTP/1.1\r\nHost: localhost:7380\r\n\r\n", nullptr));

	QByteArray message;
	QCOMPARE(TakeHeadAction(request, message), int(IRequest::HA_REJECT));
	QVERIFY2(IsFinalResponse(message, "403 Forbidden"), message.constData());
	QCOMPARE(request.GetState(), IRequest::RS_HEADERS_COMPLETE);
}


void CHttpRequestBodyPolicyTest::ResetBodyPolicyTest()
{
	CRequestServletStub servlet;
	CProtocolEngineStub engine;
	imtrest::CHttpRequest request(servlet, engine);

	CPolicyStub policy;
	policy.Refuse(IProtocolEngine::SC_FORBIDDEN);

	request.SetBodyPolicy(policy);
	request.ResetBodyPolicy();

	// The request does not use the policy any more, so it can be destroyed
	QVERIFY(request.ExecuteHttpParser(CreateHead("3") + "abc", nullptr));

	QByteArray message;
	QCOMPARE(TakeHeadAction(request, message), int(IRequest::HA_NONE));
	QCOMPARE(request.GetState(), IRequest::RS_MESSAGE_COMPLETE);
	QCOMPARE(policy.callCount, 0);
}


void CHttpRequestBodyPolicyTest::ResetBodyPolicyKeepsDefaultLimitTest()
{
	CRequestServletStub servlet;
	CProtocolEngineStub engine;
	imtrest::CHttpRequest request(servlet, engine);

	CPolicyStub policy;

	request.SetDefaultMaxBodySize(5);
	request.SetBodyPolicy(policy);
	request.ResetBodyPolicy();

	// The limit of the server is not a part of the policy
	QVERIFY(request.ExecuteHttpParser(CreateHead("50"), nullptr));

	QByteArray message;
	QCOMPARE(TakeHeadAction(request, message), int(IRequest::HA_REJECT));
	QVERIFY2(IsFinalResponse(message, "413 Payload Too Large"), message.constData());
}


void CHttpRequestBodyPolicyTest::RefusalMessageIsFinalResponseTest()
{
	CRequestServletStub servlet;
	CProtocolEngineStub engine;
	imtrest::CHttpRequest request(servlet, engine);

	CPolicyStub policy;
	policy.Refuse(IProtocolEngine::SC_FORBIDDEN);

	request.SetBodyPolicy(policy);

	QVERIFY(request.ExecuteHttpParser(CreateHead("10"), nullptr));

	// The rest of the request is not received, so the connection is closed after the response (RFC 9112, section 9.6)
	QByteArray message;
	QCOMPARE(TakeHeadAction(request, message), int(IRequest::HA_REJECT));
	QVERIFY2(IsFinalResponse(message, "403 Forbidden"), message.constData());
	QVERIFY(!message.contains("100 Continue"));
}


void CHttpRequestBodyPolicyTest::RefusalWithUnknownStatusTest()
{
	CRequestServletStub servlet;
	CProtocolEngineStub engine;
	imtrest::CHttpRequest request(servlet, engine);

	// The engine does not know the status, so the response cannot be built: the client still gets a response
	CPolicyStub policy;
	policy.Refuse(IProtocolEngine::SC_NOT_FOUND);

	request.SetBodyPolicy(policy);

	QVERIFY(request.ExecuteHttpParser(CreateHead("10"), nullptr));

	QByteArray message;
	QCOMPARE(TakeHeadAction(request, message), int(IRequest::HA_REJECT));
	QVERIFY2(message.startsWith("HTTP/1.1 500 Internal Server Error\r\n"), message.constData());
	QVERIFY2(message.contains("\r\nConnection: close\r\n"), message.constData());
}


void CHttpRequestBodyPolicyTest::RefusalWhenEngineCreatesNoResponseTest()
{
	CRequestServletStub servlet;
	CProtocolEngineStub engine(false);
	imtrest::CHttpRequest request(servlet, engine);

	CPolicyStub policy;
	policy.Refuse(IProtocolEngine::SC_FORBIDDEN);

	request.SetBodyPolicy(policy);

	QVERIFY(request.ExecuteHttpParser(CreateHead("10"), nullptr));

	QByteArray message;
	QCOMPARE(TakeHeadAction(request, message), int(IRequest::HA_REJECT));
	QVERIFY2(message.startsWith("HTTP/1.1 500 Internal Server Error\r\n"), message.constData());
	QVERIFY2(message.contains("\r\nConnection: close\r\n"), message.constData());
}


void CHttpRequestBodyPolicyTest::RefusalIsKeptTest()
{
	CRequestServletStub servlet;
	CProtocolEngineStub engine;
	imtrest::CHttpRequest request(servlet, engine);

	request.SetDefaultMaxBodySize(5);

	QVERIFY(request.ExecuteHttpParser(CreateHead("50"), nullptr));

	// A request that is refused stays refused
	QByteArray firstMessage;
	QByteArray secondMessage;
	QCOMPARE(TakeHeadAction(request, firstMessage), int(IRequest::HA_REJECT));
	QCOMPARE(TakeHeadAction(request, secondMessage), int(IRequest::HA_REJECT));
	QCOMPARE(secondMessage.left(20), firstMessage.left(20));
}


void CHttpRequestBodyPolicyTest::ResetDataClearsRefusalTest()
{
	CRequestServletStub servlet;
	CProtocolEngineStub engine;
	imtrest::CHttpRequest request(servlet, engine);

	request.SetDefaultMaxBodySize(5);

	QVERIFY(request.ExecuteHttpParser(CreateHead("50"), nullptr));

	QByteArray message;
	QCOMPARE(TakeHeadAction(request, message), int(IRequest::HA_REJECT));

	// The next request of the object is not refused because of the previous one
	QVERIFY(request.ResetData());
	QVERIFY(request.ExecuteHttpParser(CreateHead("3") + "abc", nullptr));

	QCOMPARE(TakeHeadAction(request, message), int(IRequest::HA_NONE));
	QCOMPARE(request.GetState(), IRequest::RS_MESSAGE_COMPLETE);
	QCOMPARE(request.GetBody(), QByteArray("abc"));
}


void CHttpRequestBodyPolicyTest::ExpectContinueGetsInterimMessageTest()
{
	CRequestServletStub servlet;
	CProtocolEngineStub engine;
	imtrest::CHttpRequest request(servlet, engine);

	// The client waits for the permission, the body is not sent yet
	QVERIFY(request.ExecuteHttpParser(CreateHead("10", s_expectContinue), nullptr));
	QCOMPARE(request.GetState(), IRequest::RS_HEADERS_COMPLETE);

	QByteArray message;
	QCOMPARE(TakeHeadAction(request, message), int(IRequest::HA_SEND_INTERIM));
	QCOMPARE(message, QByteArray("HTTP/1.1 100 Continue\r\n\r\n"));
}


void CHttpRequestBodyPolicyTest::InterimMessageIsTakenOnceTest()
{
	CRequestServletStub servlet;
	CProtocolEngineStub engine;
	imtrest::CHttpRequest request(servlet, engine);

	QVERIFY(request.ExecuteHttpParser(CreateHead("10", s_expectContinue), nullptr));

	QByteArray message;
	QCOMPARE(TakeHeadAction(request, message), int(IRequest::HA_SEND_INTERIM));

	// The client gets 100 only once, however many times the data of the request come
	QCOMPARE(TakeHeadAction(request, message), int(IRequest::HA_NONE));
	QVERIFY(message.isEmpty());
}


void CHttpRequestBodyPolicyTest::ExpectContinueValueTest_data()
{
	QTest::addColumn<QByteArray>("value");

	// The expectation is not case sensitive (RFC 9110, section 10.1.1)
	QTest::newRow("lower case") << QByteArray("100-continue");
	QTest::newRow("mixed case") << QByteArray("100-Continue");
	QTest::newRow("upper case") << QByteArray("100-CONTINUE");
	QTest::newRow("spaces") << QByteArray("  100-continue  ");
}


void CHttpRequestBodyPolicyTest::ExpectContinueValueTest()
{
	QFETCH(QByteArray, value);

	CRequestServletStub servlet;
	CProtocolEngineStub engine;
	imtrest::CHttpRequest request(servlet, engine);

	QVERIFY(request.ExecuteHttpParser(CreateHead("10", "Expect: " + value + "\r\n"), nullptr));

	QByteArray message;
	QCOMPARE(TakeHeadAction(request, message), int(IRequest::HA_SEND_INTERIM));
}


void CHttpRequestBodyPolicyTest::ExpectContinueWithChunkedBodyTest()
{
	CRequestServletStub servlet;
	CProtocolEngineStub engine;
	imtrest::CHttpRequest request(servlet, engine);

	// A chunked body has no length but is a body as well
	QVERIFY(request.ExecuteHttpParser(CreateHead(QByteArray(), s_expectContinue + "Transfer-Encoding: chunked\r\n"), nullptr));

	QByteArray message;
	QCOMPARE(TakeHeadAction(request, message), int(IRequest::HA_SEND_INTERIM));
}


void CHttpRequestBodyPolicyTest::ExpectWithoutBodyIsIgnoredTest_data()
{
	QTest::addColumn<QByteArray>("contentLength");

	// The client does not wait for 100 if there is nothing to send
	QTest::newRow("empty body") << QByteArray("0");
	QTest::newRow("no length") << QByteArray();
}


void CHttpRequestBodyPolicyTest::ExpectWithoutBodyIsIgnoredTest()
{
	QFETCH(QByteArray, contentLength);

	CRequestServletStub servlet;
	CProtocolEngineStub engine;
	imtrest::CHttpRequest request(servlet, engine);

	QVERIFY(request.ExecuteHttpParser(CreateHead(contentLength, s_expectContinue), nullptr));

	QByteArray message;
	QCOMPARE(TakeHeadAction(request, message), int(IRequest::HA_NONE));
	QVERIFY(message.isEmpty());
}


void CHttpRequestBodyPolicyTest::ExpectIsIgnoredForHttp10Test()
{
	CRequestServletStub servlet;
	CProtocolEngineStub engine;
	imtrest::CHttpRequest request(servlet, engine);

	// A server does not send 1xx to HTTP/1.0 (RFC 9110, section 15.2) and ignores the expectation
	QVERIFY(request.ExecuteHttpParser(CreateHead("10", s_expectContinue, "HTTP/1.0"), nullptr));

	QByteArray message;
	QCOMPARE(TakeHeadAction(request, message), int(IRequest::HA_NONE));
	QVERIFY(message.isEmpty());
}


void CHttpRequestBodyPolicyTest::ExpectWithReceivedBodyIsIgnoredTest()
{
	CRequestServletStub servlet;
	CProtocolEngineStub engine;
	imtrest::CHttpRequest request(servlet, engine);

	// The client has not waited and sent the body, a server can omit 100 (RFC 9110, section 10.1.1)
	QVERIFY(request.ExecuteHttpParser(CreateHead("10", s_expectContinue) + "abc", nullptr));
	QCOMPARE(request.GetState(), IRequest::RS_BODY);

	QByteArray message;
	QCOMPARE(TakeHeadAction(request, message), int(IRequest::HA_NONE));
	QVERIFY(message.isEmpty());
}


void CHttpRequestBodyPolicyTest::BodyAfterInterimMessageIsReceivedTest()
{
	CRequestServletStub servlet;
	CProtocolEngineStub engine;
	imtrest::CHttpRequest request(servlet, engine);

	QVERIFY(request.ExecuteHttpParser(CreateHead("10", s_expectContinue), nullptr));

	QByteArray message;
	QCOMPARE(TakeHeadAction(request, message), int(IRequest::HA_SEND_INTERIM));

	// The client sends the body after 100
	QVERIFY(request.ExecuteHttpParser("01234", nullptr));
	QCOMPARE(TakeHeadAction(request, message), int(IRequest::HA_NONE));

	QVERIFY(request.ExecuteHttpParser("56789", nullptr));
	QCOMPARE(TakeHeadAction(request, message), int(IRequest::HA_NONE));

	QCOMPARE(request.GetState(), IRequest::RS_MESSAGE_COMPLETE);
	QCOMPARE(request.GetBody(), QByteArray("0123456789"));
}


void CHttpRequestBodyPolicyTest::UnknownExpectationIsRefusedTest_data()
{
	QTest::addColumn<QByteArray>("value");

	QTest::newRow("other expectation") << QByteArray("200-ok");
	QTest::newRow("word") << QByteArray("something");
	QTest::newRow("list") << QByteArray("100-continue, something");
}


void CHttpRequestBodyPolicyTest::UnknownExpectationIsRefusedTest()
{
	QFETCH(QByteArray, value);

	CRequestServletStub servlet;
	CProtocolEngineStub engine;
	imtrest::CHttpRequest request(servlet, engine);

	// The expectation cannot be met (RFC 9110, section 10.1.1)
	QVERIFY(request.ExecuteHttpParser(CreateHead("10", "Expect: " + value + "\r\n"), nullptr));

	QByteArray message;
	QCOMPARE(TakeHeadAction(request, message), int(IRequest::HA_REJECT));
	QVERIFY2(IsFinalResponse(message, "417 Expectation Failed"), message.constData());
}


void CHttpRequestBodyPolicyTest::ExpectIsRefusedByPolicyTest()
{
	CRequestServletStub servlet;
	CProtocolEngineStub engine;
	imtrest::CHttpRequest request(servlet, engine);

	CPolicyStub policy;
	policy.Refuse(IProtocolEngine::SC_UNAUTHORIZED);

	request.SetBodyPolicy(policy);

	// The client gets the final status without 100, so it does not send the body
	QVERIFY(request.ExecuteHttpParser(CreateHead("10", s_expectContinue), nullptr));

	QByteArray message;
	QCOMPARE(TakeHeadAction(request, message), int(IRequest::HA_REJECT));
	QVERIFY2(IsFinalResponse(message, "401 Unauthorized"), message.constData());
	QVERIFY(!message.contains("100 Continue"));
}


void CHttpRequestBodyPolicyTest::ExpectIsRefusedByLimitTest()
{
	CRequestServletStub servlet;
	CProtocolEngineStub engine;
	imtrest::CHttpRequest request(servlet, engine);

	request.SetDefaultMaxBodySize(5);

	QVERIFY(request.ExecuteHttpParser(CreateHead("50", s_expectContinue), nullptr));

	QByteArray message;
	QCOMPARE(TakeHeadAction(request, message), int(IRequest::HA_REJECT));
	QVERIFY2(IsFinalResponse(message, "413 Payload Too Large"), message.constData());
}


void CHttpRequestBodyPolicyTest::PolicyRefusalBeatsUnknownExpectationTest()
{
	CRequestServletStub servlet;
	CProtocolEngineStub engine;
	imtrest::CHttpRequest request(servlet, engine);

	// The reason of the refusal by the policy is more important than the expectation
	CPolicyStub policy;
	policy.Refuse(IProtocolEngine::SC_UNAUTHORIZED);

	request.SetBodyPolicy(policy);

	QVERIFY(request.ExecuteHttpParser(CreateHead("10", "Expect: something\r\n"), nullptr));

	QByteArray message;
	QCOMPARE(TakeHeadAction(request, message), int(IRequest::HA_REJECT));
	QVERIFY2(IsFinalResponse(message, "401 Unauthorized"), message.constData());
}


void CHttpRequestBodyPolicyTest::ExpectAcceptedByPolicyTest()
{
	CRequestServletStub servlet;
	CProtocolEngineStub engine;
	imtrest::CHttpRequest request(servlet, engine);

	CPolicyStub policy;
	request.SetBodyPolicy(policy);

	QVERIFY(request.ExecuteHttpParser(CreateHead("10", s_expectContinue), nullptr));

	QByteArray message;
	QCOMPARE(TakeHeadAction(request, message), int(IRequest::HA_SEND_INTERIM));
	QCOMPARE(policy.callCount, 1);
}


void CHttpRequestBodyPolicyTest::NoExpectNoActionTest()
{
	CRequestServletStub servlet;
	CProtocolEngineStub engine;
	imtrest::CHttpRequest request(servlet, engine);

	// A usual request gets no message before its body is received
	QVERIFY(request.ExecuteHttpParser(CreateHead("10"), nullptr));

	QByteArray message;
	QCOMPARE(TakeHeadAction(request, message), int(IRequest::HA_NONE));
	QVERIFY(message.isEmpty());
}


I_ADD_TEST(CHttpRequestBodyPolicyTest);


