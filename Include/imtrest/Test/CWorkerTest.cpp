// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include "CWorkerTest.h"


// STL includes
#include <stdexcept>

// ImtCore includes
#include <imtrest/CHttpRequest.h>
#include <imtrest/CHttpResponse.h>
#include <imtrest/CTcpRequest.h>
#include <imtrest/CWorker.h>
#include <imtrest/IProtocolEngine.h>


namespace
{


/**
	Engine that creates the responses like the engines of the protocols do, it counts the created responses.
*/
class CProtocolEngineStub: virtual public imtrest::IProtocolEngine
{
public:
	explicit CProtocolEngineStub(const QByteArray& protocolTypeId, bool isResponseCreated = true)
		:m_protocolTypeId(protocolTypeId),
		m_isResponseCreated(isResponseCreated)
	{
	}

	mutable int createResponseCount = 0;

	// reimplemented (imtrest::IProtocolEngine)
	QByteArray GetProtocolTypeId() const override
	{
		return m_protocolTypeId;
	}

	const iser::IVersionInfo* GetProtocolVersion() const override
	{
		return nullptr;
	}

	bool GetProtocolStatusCode(int /*statusCode*/, int& /*protocolStatusCode*/, QByteArray& /*statusCodeLiteral*/) const override
	{
		return false;
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
		++createResponseCount;

		if (!m_isResponseCreated){
			return nullptr;
		}

		return new imtrest::CHttpResponse(statusCode, data, dataTypeId, *this);
	}

private:
	QByteArray m_protocolTypeId;
	bool m_isResponseCreated;
};


/**
	Servlet with the behavior that is set by the test.
*/
class CServletStub: virtual public imtrest::IRequestServlet
{
public:
	enum Mode
	{
		M_RESPONSE,
		M_NO_RESPONSE,
		M_STD_EXCEPTION,
		M_UNKNOWN_EXCEPTION
	};

	explicit CServletStub(const imtrest::IProtocolEngine& engine, Mode mode = M_RESPONSE)
		:m_engine(engine),
		m_mode(mode)
	{
	}

	void SetMode(Mode mode)
	{
		m_mode = mode;
	}

	mutable int callCount = 0;
	mutable const imtrest::IRequest* lastRequestPtr = nullptr;
	mutable QByteArray lastSubCommandId;

	// reimplemented (imtrest::IRequestServlet)
	bool IsCommandSupported(const QByteArray& /*commandId*/) const override
	{
		return true;
	}

	imtrest::ConstResponsePtr ProcessRequest(const imtrest::IRequest& request, const QByteArray& subCommandId) const override
	{
		++callCount;
		lastRequestPtr = &request;
		lastSubCommandId = subCommandId;

		switch (m_mode){
		case M_NO_RESPONSE:
			return imtrest::ConstResponsePtr();

		case M_STD_EXCEPTION:
			throw std::runtime_error("failure of the servlet");

		case M_UNKNOWN_EXCEPTION:
			throw 42;

		case M_RESPONSE:
			break;
		}

		return imtrest::ConstResponsePtr(new imtrest::CHttpResponse(imtrest::IProtocolEngine::SC_OK, "result", "text/plain", m_engine));
	}

private:
	const imtrest::IProtocolEngine& m_engine;
	Mode m_mode;
};


const QByteArray s_subCommandId("sub");


} // namespace


// private slots

void CWorkerTest::ResponseOfServletIsReturnedTest()
{
	CProtocolEngineStub engine("http");
	CServletStub servlet(engine);
	imtrest::CHttpRequest request(servlet, engine);

	const imtrest::ConstResponsePtr response = imtrest::CWorker::ExecuteServlet(&servlet, request, QByteArray());

	QVERIFY(response.IsValid());
	QCOMPARE(response->GetStatusCode(), int(imtrest::IProtocolEngine::SC_OK));
	QCOMPARE(response->GetData(), QByteArray("result"));

	// The engine is not needed, the response is not replaced:
	QCOMPARE(engine.createResponseCount, 0);
}


void CWorkerTest::ServletGetsRequestAndSubCommandTest()
{
	CProtocolEngineStub engine("http");
	CServletStub servlet(engine);
	imtrest::CHttpRequest request(servlet, engine);

	imtrest::CWorker::ExecuteServlet(&servlet, request, s_subCommandId);

	QCOMPARE(servlet.lastRequestPtr, static_cast<const imtrest::IRequest*>(&request));
	QCOMPARE(servlet.lastSubCommandId, s_subCommandId);
}


void CWorkerTest::ServletIsCalledOnceTest()
{
	CProtocolEngineStub engine("http");

	// A failed request is not repeated, it can have side effects:
	for (CServletStub::Mode mode: {CServletStub::M_RESPONSE, CServletStub::M_NO_RESPONSE, CServletStub::M_STD_EXCEPTION, CServletStub::M_UNKNOWN_EXCEPTION}){
		CServletStub servlet(engine, mode);
		imtrest::CHttpRequest request(servlet, engine);

		imtrest::CWorker::ExecuteServlet(&servlet, request, QByteArray());

		QCOMPARE(servlet.callCount, 1);
	}
}


void CWorkerTest::ExceptionGetsInternalErrorTest()
{
	CProtocolEngineStub engine("http");
	CServletStub servlet(engine, CServletStub::M_STD_EXCEPTION);
	imtrest::CHttpRequest request(servlet, engine);

	// The exception does not leave the call, so it cannot stop the thread of the worker:
	const imtrest::ConstResponsePtr response = imtrest::CWorker::ExecuteServlet(&servlet, request, QByteArray());

	QVERIFY(response.IsValid());
	QCOMPARE(response->GetStatusCode(), int(imtrest::IProtocolEngine::SC_INTERNAL_SERVER_ERROR));
	QCOMPARE(servlet.callCount, 1);
}


void CWorkerTest::UnknownExceptionGetsInternalErrorTest()
{
	CProtocolEngineStub engine("http");
	CServletStub servlet(engine, CServletStub::M_UNKNOWN_EXCEPTION);
	imtrest::CHttpRequest request(servlet, engine);

	const imtrest::ConstResponsePtr response = imtrest::CWorker::ExecuteServlet(&servlet, request, QByteArray());

	QVERIFY(response.IsValid());
	QCOMPARE(response->GetStatusCode(), int(imtrest::IProtocolEngine::SC_INTERNAL_SERVER_ERROR));
}


void CWorkerTest::NoResponseGetsInternalErrorTest()
{
	CProtocolEngineStub engine("http");
	CServletStub servlet(engine, CServletStub::M_NO_RESPONSE);
	imtrest::CHttpRequest request(servlet, engine);

	// A servlet that returns nothing would leave the client without the response:
	const imtrest::ConstResponsePtr response = imtrest::CWorker::ExecuteServlet(&servlet, request, QByteArray());

	QVERIFY(response.IsValid());
	QCOMPARE(response->GetStatusCode(), int(imtrest::IProtocolEngine::SC_INTERNAL_SERVER_ERROR));
}


void CWorkerTest::MissingServletGetsInternalErrorTest()
{
	CProtocolEngineStub engine("http");
	CServletStub servlet(engine);
	imtrest::CHttpRequest request(servlet, engine);

	const imtrest::ConstResponsePtr response = imtrest::CWorker::ExecuteServlet(nullptr, request, QByteArray());

	QVERIFY(response.IsValid());
	QCOMPARE(response->GetStatusCode(), int(imtrest::IProtocolEngine::SC_INTERNAL_SERVER_ERROR));
	QCOMPARE(servlet.callCount, 0);
}


void CWorkerTest::InternalErrorResponseHasNoContentTest()
{
	CProtocolEngineStub engine("http");
	CServletStub servlet(engine, CServletStub::M_STD_EXCEPTION);
	imtrest::CHttpRequest request(servlet, engine);

	const imtrest::ConstResponsePtr response = imtrest::CWorker::ExecuteServlet(&servlet, request, QByteArray());

	// The text of the exception can have the internals of the server, so the client does not get it:
	QVERIFY(response.IsValid());
	QVERIFY(response->GetData().isEmpty());
	QVERIFY(response->GetDataTypeId().isEmpty());
}


void CWorkerTest::InternalErrorResponseUsesEngineOfRequestTest()
{
	CProtocolEngineStub engine("http");
	CServletStub servlet(engine);
	imtrest::CHttpRequest request(servlet, engine);

	const imtrest::ConstResponsePtr response = imtrest::CWorker::CreateInternalErrorResponse(request);

	QVERIFY(response.IsValid());
	QCOMPARE(engine.createResponseCount, 1);
	QCOMPARE(&response->GetProtocolEngine(), static_cast<const imtrest::IProtocolEngine*>(&engine));
}


void CWorkerTest::FailureDoesNotAffectNextRequestTest()
{
	CProtocolEngineStub engine("http");
	CServletStub servlet(engine, CServletStub::M_STD_EXCEPTION);
	imtrest::CHttpRequest request(servlet, engine);

	imtrest::ConstResponsePtr response = imtrest::CWorker::ExecuteServlet(&servlet, request, QByteArray());
	QCOMPARE(response->GetStatusCode(), int(imtrest::IProtocolEngine::SC_INTERNAL_SERVER_ERROR));

	// The servlet is not marked as broken, the next request is processed as usual:
	servlet.SetMode(CServletStub::M_RESPONSE);

	response = imtrest::CWorker::ExecuteServlet(&servlet, request, QByteArray());

	QVERIFY(response.IsValid());
	QCOMPARE(response->GetStatusCode(), int(imtrest::IProtocolEngine::SC_OK));
	QCOMPARE(servlet.callCount, 2);
}


void CWorkerTest::EngineWithoutResponseTest()
{
	CProtocolEngineStub engine("http", false);
	CServletStub servlet(engine, CServletStub::M_STD_EXCEPTION);
	imtrest::CHttpRequest request(servlet, engine);

	// If the engine cannot create the response, the result is an invalid pointer and not a crash:
	const imtrest::ConstResponsePtr response = imtrest::CWorker::ExecuteServlet(&servlet, request, QByteArray());

	QVERIFY(!response.IsValid());
	QCOMPARE(engine.createResponseCount, 1);
}


void CWorkerTest::FailureOfOtherProtocolHasNoResponseTest_data()
{
	QTest::addColumn<int>("mode");

	QTest::newRow("exception") << int(CServletStub::M_STD_EXCEPTION);
	QTest::newRow("unknown exception") << int(CServletStub::M_UNKNOWN_EXCEPTION);
	QTest::newRow("no response") << int(CServletStub::M_NO_RESPONSE);
}


void CWorkerTest::FailureOfOtherProtocolHasNoResponseTest()
{
	QFETCH(int, mode);

	// A message of a channel can have no response or many, an invented response would break the channel:
	CProtocolEngineStub engine("tcp");
	CServletStub servlet(engine, CServletStub::Mode(mode));
	imtrest::CTcpRequest request(servlet, engine);

	const imtrest::ConstResponsePtr response = imtrest::CWorker::ExecuteServlet(&servlet, request, QByteArray());

	QVERIFY(!response.IsValid());
	QCOMPARE(engine.createResponseCount, 0);
}


void CWorkerTest::ResponseOfOtherProtocolIsReturnedTest()
{
	CProtocolEngineStub engine("tcp");
	CServletStub servlet(engine);
	imtrest::CTcpRequest request(servlet, engine);

	const imtrest::ConstResponsePtr response = imtrest::CWorker::ExecuteServlet(&servlet, request, QByteArray());

	QVERIFY(response.IsValid());
	QCOMPARE(response->GetStatusCode(), int(imtrest::IProtocolEngine::SC_OK));
}


void CWorkerTest::CreateInternalErrorResponseOfOtherProtocolTest()
{
	CProtocolEngineStub engine("ws");
	CServletStub servlet(engine);
	imtrest::CTcpRequest request(servlet, engine);

	QVERIFY(!imtrest::CWorker::CreateInternalErrorResponse(request).IsValid());
	QCOMPARE(engine.createResponseCount, 0);
}


I_ADD_TEST(CWorkerTest);


