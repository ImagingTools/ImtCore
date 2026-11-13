// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include "CHttpRequestBodyStreamTest.h"


// STL includes
#include <memory>

// ImtCore includes
#include <imtrest/CHttpRequest.h>
#include <imtrest/CHttpResponse.h>
#include <imtrest/IHttpRequestBodyPolicy.h>
#include <imtrest/IHttpRequestBodySink.h>
#include <imtrest/IProtocolEngine.h>


namespace
{


typedef imtrest::IRequest IRequest;
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

	bool GetProtocolStatusCode(int statusCode, int& protocolStatusCode, QByteArray& statusCodeLiteral) const override
	{
		switch (statusCode){
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
		return new imtrest::CHttpResponse(statusCode, data, dataTypeId, *this);
	}
};


/**
	Receiver that remembers everything it was told.
*/
class CSinkStub: virtual public imtrest::IHttpRequestBodySink
{
public:
	QByteArray data;
	int writeCount = 0;
	int finishCount = 0;
	int abortCount = 0;

	// A part that does not fit is not taken (the size of all the taken data), -1 - no limit
	qint64 capacity = -1;
	bool isFinishFailing = false;

	// reimplemented (imtrest::IHttpRequestBodySink)
	bool Write(const char* dataPtr, qint64 size) override
	{
		++writeCount;

		if ((capacity >= 0) && (data.size() + size > capacity)){
			return false;
		}

		data.append(dataPtr, qsizetype(size));

		return true;
	}

	bool Finish() override
	{
		++finishCount;

		return !isFinishFailing;
	}

	void Abort() override
	{
		++abortCount;
	}
};


/**
	Policy with the decision that is set by the test.
*/
class CPolicyStub: virtual public imtrest::IHttpRequestBodyPolicy
{
public:
	std::shared_ptr<CSinkStub> sinkPtr = std::make_shared<CSinkStub>();
	IHttpRequestBodyPolicy::BodyMode mode = IHttpRequestBodyPolicy::BM_STREAM;
	qint64 maxBodySize = -1;
	bool isSinkMissing = false;

	mutable int callCount = 0;

	// reimplemented (imtrest::IHttpRequestBodyPolicy)
	BodyDecision DecideBody(const imtrest::CHttpRequest& /*requestHead*/) const override
	{
		++callCount;

		BodyDecision retVal;
		retVal.mode = mode;
		retVal.maxBodySize = maxBodySize;

		if (!isSinkMissing){
			retVal.sinkPtr = sinkPtr;
		}

		return retVal;
	}
};


QByteArray CreateHead(const QByteArray& contentLength, const QByteArray& otherHeaders = QByteArray())
{
	QByteArray retVal("PUT /api/upload HTTP/1.1\r\nHost: localhost:7380\r\n");

	if (!contentLength.isEmpty()){
		retVal += "Content-Length: " + contentLength + "\r\n";
	}

	retVal += otherHeaders;
	retVal += "\r\n";

	return retVal;
}


const QByteArray s_chunkedHeaders("Transfer-Encoding: chunked\r\n");

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


} // namespace


// private slots

void CHttpRequestBodyStreamTest::StreamedBodyGoesToSinkTest()
{
	CRequestServletStub servlet;
	CProtocolEngineStub engine;
	imtrest::CHttpRequest request(servlet, engine);

	CPolicyStub policy;
	request.SetBodyPolicy(policy);

	QVERIFY(request.ExecuteHttpParser(CreateHead("10") + "0123456789", nullptr));

	QCOMPARE(request.GetState(), IRequest::RS_MESSAGE_COMPLETE);

	// The body is not kept in the request
	QVERIFY(request.GetBody().isEmpty());

	QCOMPARE(policy.sinkPtr->data, QByteArray("0123456789"));
	QCOMPARE(policy.sinkPtr->finishCount, 1);
	QCOMPARE(policy.sinkPtr->abortCount, 0);

	QByteArray message;
	QCOMPARE(TakeHeadAction(request, message), int(IRequest::HA_NONE));
}


void CHttpRequestBodyStreamTest::BodyInPartsTest()
{
	CRequestServletStub servlet;
	CProtocolEngineStub engine;
	imtrest::CHttpRequest request(servlet, engine);

	CPolicyStub policy;
	request.SetBodyPolicy(policy);

	// The data come by bytes
	const QByteArray data = CreateHead("10") + "0123456789";
	for (int i = 0; i < data.size(); ++i){
		QVERIFY(request.ExecuteHttpParser(data.mid(i, 1), nullptr));
	}

	QCOMPARE(request.GetState(), IRequest::RS_MESSAGE_COMPLETE);
	QCOMPARE(policy.sinkPtr->data, QByteArray("0123456789"));
	QCOMPARE(policy.sinkPtr->writeCount, 10);
	QCOMPARE(policy.sinkPtr->finishCount, 1);
}


void CHttpRequestBodyStreamTest::ChunkedBodyTest()
{
	CRequestServletStub servlet;
	CProtocolEngineStub engine;
	imtrest::CHttpRequest request(servlet, engine);

	CPolicyStub policy;
	request.SetBodyPolicy(policy);

	// Only the content of the chunks goes to the receiver, not their framing
	QVERIFY(request.ExecuteHttpParser(CreateHead(QByteArray(), s_chunkedHeaders) + s_chunkedBody, nullptr));

	QCOMPARE(request.GetState(), IRequest::RS_MESSAGE_COMPLETE);
	QCOMPARE(policy.sinkPtr->data, QByteArray("hello world"));
	QCOMPARE(policy.sinkPtr->finishCount, 1);
}


void CHttpRequestBodyStreamTest::EmptyBodyFinishesSinkTest_data()
{
	QTest::addColumn<QByteArray>("contentLength");

	QTest::newRow("empty body") << QByteArray("0");
	QTest::newRow("no length") << QByteArray();
}


void CHttpRequestBodyStreamTest::EmptyBodyFinishesSinkTest()
{
	QFETCH(QByteArray, contentLength);

	CRequestServletStub servlet;
	CProtocolEngineStub engine;
	imtrest::CHttpRequest request(servlet, engine);

	CPolicyStub policy;
	request.SetBodyPolicy(policy);

	QVERIFY(request.ExecuteHttpParser(CreateHead(contentLength), nullptr));

	// The receiver of an empty body is told that the body is over, so it can finish its work (for example create an empty file)
	QCOMPARE(request.GetState(), IRequest::RS_MESSAGE_COMPLETE);
	QCOMPARE(policy.sinkPtr->writeCount, 0);
	QCOMPARE(policy.sinkPtr->finishCount, 1);
	QCOMPARE(policy.sinkPtr->abortCount, 0);
}


void CHttpRequestBodyStreamTest::BigBodyIsNotBufferedTest()
{
	CRequestServletStub servlet;
	CProtocolEngineStub engine;
	imtrest::CHttpRequest request(servlet, engine);

	CPolicyStub policy;
	request.SetBodyPolicy(policy);

	const qsizetype partSize = qsizetype(64) * 1024;
	const int partCount = 16;

	QVERIFY(request.ExecuteHttpParser(CreateHead(QByteArray::number(qint64(partSize) * partCount)), nullptr));

	for (int i = 0; i < partCount; ++i){
		QVERIFY(request.ExecuteHttpParser(QByteArray(partSize, char('a' + i)), nullptr));

		QVERIFY(request.GetBody().isEmpty());
	}

	QCOMPARE(request.GetState(), IRequest::RS_MESSAGE_COMPLETE);
	QCOMPARE(policy.sinkPtr->data.size(), qsizetype(partSize * partCount));
	QCOMPARE(policy.sinkPtr->data.left(3), QByteArray("aaa"));
	QCOMPARE(policy.sinkPtr->data.right(3), QByteArray("ppp"));
}


void CHttpRequestBodyStreamTest::BufferModeIgnoresSinkTest()
{
	CRequestServletStub servlet;
	CProtocolEngineStub engine;
	imtrest::CHttpRequest request(servlet, engine);

	// The receiver is used only in the mode BM_STREAM
	CPolicyStub policy;
	policy.mode = IHttpRequestBodyPolicy::BM_BUFFER;

	request.SetBodyPolicy(policy);

	QVERIFY(request.ExecuteHttpParser(CreateHead("5") + "hello", nullptr));

	QCOMPARE(request.GetBody(), QByteArray("hello"));
	QVERIFY(!request.GetBodySink());
	QCOMPARE(policy.sinkPtr->writeCount, 0);
	QCOMPARE(policy.sinkPtr->finishCount, 0);
	QCOMPARE(policy.sinkPtr->abortCount, 0);
}


void CHttpRequestBodyStreamTest::GetBodySinkTest()
{
	CRequestServletStub servlet;
	CProtocolEngineStub engine;
	imtrest::CHttpRequest request(servlet, engine);

	CPolicyStub policy;
	request.SetBodyPolicy(policy);

	// Without the headers there is no decision yet
	QVERIFY(!request.GetBodySink());

	QVERIFY(request.ExecuteHttpParser(CreateHead("5") + "hello", nullptr));

	// The handler of the request gets the result of the receiving from the receiver
	QVERIFY(request.GetBodySink());
	QCOMPARE(request.GetBodySink().get(), static_cast<imtrest::IHttpRequestBodySink*>(policy.sinkPtr.get()));
}


void CHttpRequestBodyStreamTest::NextRequestOfObjectTest()
{
	CRequestServletStub servlet;
	CProtocolEngineStub engine;
	imtrest::CHttpRequest request(servlet, engine);

	CPolicyStub policy;
	request.SetBodyPolicy(policy);

	QVERIFY(request.ExecuteHttpParser(CreateHead("3") + "abc", nullptr));
	QVERIFY(request.ResetData());
	QVERIFY(request.ExecuteHttpParser(CreateHead("3") + "def", nullptr));

	// The policy decides for each request
	QCOMPARE(policy.callCount, 2);
	QCOMPARE(policy.sinkPtr->data, QByteArray("abcdef"));
	QCOMPARE(policy.sinkPtr->finishCount, 2);
	QCOMPARE(policy.sinkPtr->abortCount, 0);
}


void CHttpRequestBodyStreamTest::BigContentLengthIsRefusedBeforeBodyTest()
{
	CRequestServletStub servlet;
	CProtocolEngineStub engine;
	imtrest::CHttpRequest request(servlet, engine);

	CPolicyStub policy;
	policy.maxBodySize = 8;

	request.SetBodyPolicy(policy);

	QVERIFY(request.ExecuteHttpParser(CreateHead("11") + "01234567890", nullptr));

	QByteArray message;
	QCOMPARE(TakeHeadAction(request, message), int(IRequest::HA_REJECT));
	QVERIFY2(message.startsWith("HTTP/1.1 413 Payload Too Large\r\n"), message.constData());

	// The receiver gets nothing and is released
	QCOMPARE(policy.sinkPtr->writeCount, 0);
	QCOMPARE(policy.sinkPtr->finishCount, 0);
	QCOMPARE(policy.sinkPtr->abortCount, 1);
}


void CHttpRequestBodyStreamTest::ChunkedBodyOverLimitTest()
{
	CRequestServletStub servlet;
	CProtocolEngineStub engine;
	imtrest::CHttpRequest request(servlet, engine);

	CPolicyStub policy;
	policy.maxBodySize = 8;

	request.SetBodyPolicy(policy);

	QVERIFY(request.ExecuteHttpParser(CreateHead(QByteArray(), s_chunkedHeaders) + s_chunkedBody, nullptr));

	QByteArray message;
	QCOMPARE(TakeHeadAction(request, message), int(IRequest::HA_REJECT));
	QVERIFY2(message.startsWith("HTTP/1.1 413 Payload Too Large\r\n"), message.constData());

	// The first chunk is within the limit, the second is not passed to the receiver
	QCOMPARE(policy.sinkPtr->data, QByteArray("hello"));
	QCOMPARE(policy.sinkPtr->finishCount, 0);
	QCOMPARE(policy.sinkPtr->abortCount, 1);
}


void CHttpRequestBodyStreamTest::LimitCountsAllPartsTest()
{
	CRequestServletStub servlet;
	CProtocolEngineStub engine;
	imtrest::CHttpRequest request(servlet, engine);

	// The body is not in the request, so the limit counts what has been passed to the receiver
	CPolicyStub policy;
	policy.maxBodySize = 11;

	request.SetBodyPolicy(policy);

	QVERIFY(request.ExecuteHttpParser(CreateHead(QByteArray(), s_chunkedHeaders) + "6\r\nhello \r\n", nullptr));
	QVERIFY(request.ExecuteHttpParser("6\r\nworld!\r\n0\r\n\r\n", nullptr));

	QByteArray message;
	QCOMPARE(TakeHeadAction(request, message), int(IRequest::HA_REJECT));

	QCOMPARE(policy.sinkPtr->data, QByteArray("hello "));
	QCOMPARE(policy.sinkPtr->abortCount, 1);
}


void CHttpRequestBodyStreamTest::BodyWithinLimitTest()
{
	CRequestServletStub servlet;
	CProtocolEngineStub engine;
	imtrest::CHttpRequest request(servlet, engine);

	CPolicyStub policy;
	policy.maxBodySize = 10;

	request.SetBodyPolicy(policy);

	QVERIFY(request.ExecuteHttpParser(CreateHead("10") + "01234", nullptr));
	QVERIFY(request.ExecuteHttpParser("56789", nullptr));

	QByteArray message;
	QCOMPARE(TakeHeadAction(request, message), int(IRequest::HA_NONE));
	QCOMPARE(request.GetState(), IRequest::RS_MESSAGE_COMPLETE);
	QCOMPARE(policy.sinkPtr->data, QByteArray("0123456789"));
	QCOMPARE(policy.sinkPtr->abortCount, 0);
}


void CHttpRequestBodyStreamTest::WriteFailureGetsInternalErrorTest()
{
	CRequestServletStub servlet;
	CProtocolEngineStub engine;
	imtrest::CHttpRequest request(servlet, engine);

	CPolicyStub policy;
	policy.sinkPtr->capacity = 4;

	request.SetBodyPolicy(policy);

	QVERIFY(request.ExecuteHttpParser(CreateHead("10") + "0123456789", nullptr));

	// For example there is no place on the disk
	QByteArray message;
	QCOMPARE(TakeHeadAction(request, message), int(IRequest::HA_REJECT));
	QVERIFY2(message.startsWith("HTTP/1.1 500 Internal Server Error\r\n"), message.constData());
	QVERIFY2(message.contains("\r\nConnection: close\r\n"), message.constData());

	QVERIFY(request.GetState() != IRequest::RS_MESSAGE_COMPLETE);
	QCOMPARE(policy.sinkPtr->finishCount, 0);
	QCOMPARE(policy.sinkPtr->abortCount, 1);
}


void CHttpRequestBodyStreamTest::FinishFailureGetsInternalErrorTest()
{
	CRequestServletStub servlet;
	CProtocolEngineStub engine;
	imtrest::CHttpRequest request(servlet, engine);

	CPolicyStub policy;
	policy.sinkPtr->isFinishFailing = true;

	request.SetBodyPolicy(policy);

	QVERIFY(request.ExecuteHttpParser(CreateHead("5") + "hello", nullptr));

	// The request is received but the receiver cannot complete it: the request is refused and is not handled
	QByteArray message;
	QCOMPARE(TakeHeadAction(request, message), int(IRequest::HA_REJECT));
	QVERIFY2(message.startsWith("HTTP/1.1 500 Internal Server Error\r\n"), message.constData());

	// Abort() is not called after Finish()
	QCOMPARE(policy.sinkPtr->finishCount, 1);
	QCOMPARE(policy.sinkPtr->abortCount, 0);
}


void CHttpRequestBodyStreamTest::MissingSinkGetsInternalErrorTest()
{
	CRequestServletStub servlet;
	CProtocolEngineStub engine;
	imtrest::CHttpRequest request(servlet, engine);

	// A policy that streams the body has to give the receiver, it is a mistake of the server
	CPolicyStub policy;
	policy.isSinkMissing = true;

	request.SetBodyPolicy(policy);

	QVERIFY(request.ExecuteHttpParser(CreateHead("5") + "hello", nullptr));

	QByteArray message;
	QCOMPARE(TakeHeadAction(request, message), int(IRequest::HA_REJECT));
	QVERIFY2(message.startsWith("HTTP/1.1 500 Internal Server Error\r\n"), message.constData());
	QVERIFY(!request.GetBodySink());
}


void CHttpRequestBodyStreamTest::UnfinishedSinkIsAbortedOnDestructionTest()
{
	CRequestServletStub servlet;
	CProtocolEngineStub engine;

	CPolicyStub policy;

	{
		imtrest::CHttpRequest request(servlet, engine);
		request.SetBodyPolicy(policy);

		// The connection is broken in the middle of the body
		QVERIFY(request.ExecuteHttpParser(CreateHead("10") + "01234", nullptr));

		QCOMPARE(policy.sinkPtr->abortCount, 0);
	}

	QCOMPARE(policy.sinkPtr->data, QByteArray("01234"));
	QCOMPARE(policy.sinkPtr->finishCount, 0);
	QCOMPARE(policy.sinkPtr->abortCount, 1);
}


void CHttpRequestBodyStreamTest::FinishedSinkIsNotAbortedTest()
{
	CRequestServletStub servlet;
	CProtocolEngineStub engine;

	CPolicyStub policy;

	{
		imtrest::CHttpRequest request(servlet, engine);
		request.SetBodyPolicy(policy);

		QVERIFY(request.ExecuteHttpParser(CreateHead("5") + "hello", nullptr));
	}

	QCOMPARE(policy.sinkPtr->finishCount, 1);
	QCOMPARE(policy.sinkPtr->abortCount, 0);
}


void CHttpRequestBodyStreamTest::RefusedRequestAbortsSinkOnceTest()
{
	CRequestServletStub servlet;
	CProtocolEngineStub engine;

	CPolicyStub policy;
	policy.maxBodySize = 3;

	{
		imtrest::CHttpRequest request(servlet, engine);
		request.SetBodyPolicy(policy);

		QVERIFY(request.ExecuteHttpParser(CreateHead("10"), nullptr));

		QByteArray message;
		QCOMPARE(TakeHeadAction(request, message), int(IRequest::HA_REJECT));
		QCOMPARE(policy.sinkPtr->abortCount, 1);
	}

	// The destruction of the request does not repeat the call
	QCOMPARE(policy.sinkPtr->abortCount, 1);
}


void CHttpRequestBodyStreamTest::UnknownExpectationAbortsSinkTest()
{
	CRequestServletStub servlet;
	CProtocolEngineStub engine;
	imtrest::CHttpRequest request(servlet, engine);

	CPolicyStub policy;
	request.SetBodyPolicy(policy);

	// The policy has created the receiver, then the request is refused for another reason: the receiver is released
	QVERIFY(request.ExecuteHttpParser(CreateHead("10", "Expect: something\r\n"), nullptr));

	QByteArray message;
	QCOMPARE(TakeHeadAction(request, message), int(IRequest::HA_REJECT));
	QVERIFY2(message.startsWith("HTTP/1.1 417 Expectation Failed\r\n"), message.constData());

	QCOMPARE(policy.sinkPtr->writeCount, 0);
	QCOMPARE(policy.sinkPtr->abortCount, 1);
}


void CHttpRequestBodyStreamTest::ResetDataAbortsUnfinishedSinkTest()
{
	CRequestServletStub servlet;
	CProtocolEngineStub engine;

	CPolicyStub policy;

	{
		imtrest::CHttpRequest request(servlet, engine);
		request.SetBodyPolicy(policy);

		QVERIFY(request.ExecuteHttpParser(CreateHead("10") + "01234", nullptr));
		QVERIFY(request.ResetData());

		QCOMPARE(policy.sinkPtr->abortCount, 1);
		QVERIFY(!request.GetBodySink());
	}

	QCOMPARE(policy.sinkPtr->abortCount, 1);
}


void CHttpRequestBodyStreamTest::ResetDataOfFinishedRequestDoesNotAbortTest()
{
	CRequestServletStub servlet;
	CProtocolEngineStub engine;
	imtrest::CHttpRequest request(servlet, engine);

	CPolicyStub policy;
	request.SetBodyPolicy(policy);

	QVERIFY(request.ExecuteHttpParser(CreateHead("5") + "hello", nullptr));
	QVERIFY(request.ResetData());

	QCOMPARE(policy.sinkPtr->finishCount, 1);
	QCOMPARE(policy.sinkPtr->abortCount, 0);
}


void CHttpRequestBodyStreamTest::ExpectContinueWithStreamTest()
{
	CRequestServletStub servlet;
	CProtocolEngineStub engine;
	imtrest::CHttpRequest request(servlet, engine);

	CPolicyStub policy;
	request.SetBodyPolicy(policy);

	QVERIFY(request.ExecuteHttpParser(CreateHead("10", "Expect: 100-continue\r\n"), nullptr));

	QByteArray message;
	QCOMPARE(TakeHeadAction(request, message), int(IRequest::HA_SEND_INTERIM));
	QCOMPARE(message, QByteArray("HTTP/1.1 100 Continue\r\n\r\n"));

	// The client sends the body after 100
	QVERIFY(request.ExecuteHttpParser("0123456789", nullptr));

	QCOMPARE(request.GetState(), IRequest::RS_MESSAGE_COMPLETE);
	QCOMPARE(policy.sinkPtr->data, QByteArray("0123456789"));
	QCOMPARE(policy.sinkPtr->finishCount, 1);
}


I_ADD_TEST(CHttpRequestBodyStreamTest);


