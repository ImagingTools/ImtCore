// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include "CHttpStreamingResponseTest.h"


// STL includes
#include <memory>

// Qt includes
#include <QtCore/QBuffer>

// ImtCore includes
#include <imtrest/CFileRangeBodySource.h>
#include <imtrest/CHttpStreamingResponse.h>
#include <imtrest/IProtocolEngine.h>


namespace
{


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
				int /*statusCode*/,
				const QByteArray& /*data*/,
				const QByteArray& /*dataTypeId*/) const override
	{
		return nullptr;
	}
};


CEngineStub s_engine;


imtrest::IResponseBodySourcePtr CreateSource(const QByteArray& data)
{
	std::unique_ptr<QBuffer> bufferPtr(new QBuffer());
	bufferPtr->setData(data);
	bufferPtr->open(QIODevice::ReadOnly);

	return imtrest::CFileRangeBodySource::Create(std::move(bufferPtr), 0, data.size());
}


} // namespace


// private slots

void CHttpStreamingResponseTest::DescriptionTest()
{
	const imtrest::CHttpStreamingResponse response(
				imtrest::IProtocolEngine::SC_PARTIAL_CONTENT,
				"application/pdf",
				s_engine,
				1234,
				[](){
		return imtrest::IResponseBodySourcePtr();
	});

	QCOMPARE(response.GetStatusCode(), int(imtrest::IProtocolEngine::SC_PARTIAL_CONTENT));
	QCOMPARE(response.GetDataTypeId(), QByteArray("application/pdf"));
	QCOMPARE(response.GetBodySize(), qint64(1234));
	QVERIFY(&response.GetProtocolEngine() == &s_engine);

	// The body is not in the response
	QVERIFY(response.GetData().isEmpty());
	QVERIFY(response.GetHeaders().isEmpty());
}


void CHttpStreamingResponseTest::HeadersTest()
{
	imtrest::CHttpStreamingResponse response(imtrest::IProtocolEngine::SC_OK, QByteArray(), s_engine, 0, imtrest::CHttpStreamingResponse::SourceFactory());

	imtrest::IResponse::Headers headers;
	headers.insert("ETag", "\"v1\"");
	headers.insert("Content-Range", "bytes 0-9/100");

	response.SetHeaders(headers);

	QCOMPARE(response.GetHeaders(), headers);
}


void CHttpStreamingResponseTest::UnknownBodySizeTest()
{
	// The size that is not known is sent as chunked, see IStreamingResponse::GetBodySize()
	const imtrest::CHttpStreamingResponse response(imtrest::IProtocolEngine::SC_OK, "text/plain", s_engine, -1, imtrest::CHttpStreamingResponse::SourceFactory());

	QCOMPARE(response.GetBodySize(), qint64(-1));
}


void CHttpStreamingResponseTest::SourceIsCreatedByFactoryTest()
{
	int callCount = 0;
	const QByteArray data("0123456789");

	const imtrest::CHttpStreamingResponse response(
				imtrest::IProtocolEngine::SC_OK,
				"text/plain",
				s_engine,
				data.size(),
				[&callCount, data](){
		++callCount;

		return CreateSource(data);
	});

	// The source is not created before the body is sent
	QCOMPARE(callCount, 0);

	imtrest::IResponseBodySourcePtr sourcePtr = response.CreateBodySource();
	QVERIFY(sourcePtr);
	QCOMPARE(callCount, 1);

	QByteArray part;
	QCOMPARE(sourcePtr->Read(part, 100), imtrest::IResponseBodySource::RR_DATA);
	QCOMPARE(part, data);

	// Every call creates a new source
	imtrest::IResponseBodySourcePtr otherSourcePtr = response.CreateBodySource();
	QVERIFY(otherSourcePtr);
	QCOMPARE(callCount, 2);
}


void CHttpStreamingResponseTest::NoSourceTest()
{
	// The source cannot be created (for example, the file cannot be opened): the server answers 500
	const imtrest::CHttpStreamingResponse failing(
				imtrest::IProtocolEngine::SC_OK,
				"text/plain",
				s_engine,
				10,
				[](){
		return imtrest::IResponseBodySourcePtr();
	});

	QVERIFY(!failing.CreateBodySource());

	// No function at all is the same
	const imtrest::CHttpStreamingResponse withoutFactory(imtrest::IProtocolEngine::SC_OK, "text/plain", s_engine, 10, imtrest::CHttpStreamingResponse::SourceFactory());

	QVERIFY(!withoutFactory.CreateBodySource());
}


void CHttpStreamingResponseTest::SourceDoesNotNeedResponseTest()
{
	const QByteArray data("0123456789");

	std::unique_ptr<imtrest::CHttpStreamingResponse> responsePtr(new imtrest::CHttpStreamingResponse(
				imtrest::IProtocolEngine::SC_OK,
				"text/plain",
				s_engine,
				data.size(),
				[data](){
		return CreateSource(data);
	}));

	imtrest::IResponseBodySourcePtr sourcePtr = responsePtr->CreateBodySource();
	QVERIFY(sourcePtr);

	// The server can destroy the response, when the source is created: the source has its own data
	responsePtr.reset();

	QByteArray part;
	QCOMPARE(sourcePtr->Read(part, 100), imtrest::IResponseBodySource::RR_DATA);
	QCOMPARE(part, data);
}


void CHttpStreamingResponseTest::SentAsStreamingResponseTest()
{
	// The server sends the response through the interface
	const imtrest::ConstResponsePtr responsePtr(new imtrest::CHttpStreamingResponse(
				imtrest::IProtocolEngine::SC_OK,
				"text/plain",
				s_engine,
				5,
				[](){
		return CreateSource("hello");
	}));

	const auto* streamingPtr = dynamic_cast<const imtrest::IStreamingResponse*>(responsePtr.GetPtr());
	QVERIFY(streamingPtr != nullptr);
	QCOMPARE(streamingPtr->GetBodySize(), qint64(5));
	QVERIFY(streamingPtr->CreateBodySource());
}


I_ADD_TEST(CHttpStreamingResponseTest);
