// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include "CHttpRootServletCompTest.h"


// Qt includes
#include <QtCore/QtEndian>

// ImtCore includes
#include <imtrest/CHttpResponse.h>
#include <imtrest/CHttpRootServletComp.h>
#include <imtrest/IProtocolEngine.h>


namespace
{


typedef imtrest::IResponse::Headers Headers;


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

	bool GetProtocolStatusCode(int /*statusCode*/, int& protocolStatusCode, QByteArray& statusCodeLiteral) const override
	{
		protocolStatusCode = 200;
		statusCodeLiteral = "OK";

		return true;
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


/**
	Gives the test the compression of the servlet.
*/
class CServlet: public imtrest::CHttpRootServletComp
{
public:
	QByteArray Deflate(const QByteArray& commandId, const imtrest::IResponse& response) const
	{
		return GetCompressedData(commandId, response, ET_DEFLATE);
	}

	QByteArray Gzip(const QByteArray& commandId, const imtrest::IResponse& response) const
	{
		return GetCompressedData(commandId, response, ET_GZIP);
	}
};


/**
	The data of a deflate body: it is a zlib stream, qUncompress expects its size in front.
*/
QByteArray Inflate(const QByteArray& deflated, qsizetype size)
{
	QByteArray prefixed(4, '\0');
	qToBigEndian<quint32>(quint32(size), prefixed.data());
	prefixed.append(deflated);

	return qUncompress(prefixed);
}


QByteArray CreateBody(char fill)
{
	return QByteArray(100000, fill) + QByteArray("end");
}


} // namespace


// private slots

void CHttpRootServletCompTest::DeflateRoundTripTest()
{
	const QByteArray body = CreateBody('a');
	imtrest::CHttpResponse response(200, body, "text/javascript", s_engine);

	CServlet servlet;
	const QByteArray deflated = servlet.Deflate("Views/html/index.js", response);

	QVERIFY(deflated.size() < body.size());
	QCOMPARE(Inflate(deflated, body.size()), body);
}


void CHttpRootServletCompTest::GzipHeaderTest()
{
	imtrest::CHttpResponse response(200, CreateBody('a'), "text/javascript", s_engine);

	CServlet servlet;
	const QByteArray gzipped = servlet.Gzip("Views/html/index.js", response);

	QVERIFY(gzipped.size() > 10);
	QCOMPARE(quint8(gzipped[0]), quint8(0x1f));
	QCOMPARE(quint8(gzipped[1]), quint8(0x8b));
}


void CHttpRootServletCompTest::SameTagIsCompressedOnceTest()
{
	const QByteArray firstBody = CreateBody('a');
	imtrest::CHttpResponse response(200, firstBody, "text/javascript", s_engine);
	response.SetHeaders(Headers{{"etag", "\"v1\""}});

	CServlet servlet;
	const QByteArray first = servlet.Deflate("Views/html/index.js", response);

	// A body under the same tag is the same body: the servlet does not compress it again, so a changed body shows
	// whether the answer came from the cache.
	response.SetData(CreateBody('b'));
	const QByteArray second = servlet.Deflate("Views/html/index.js", response);

	QCOMPARE(second, first);
	QCOMPARE(Inflate(second, firstBody.size()), firstBody);
}


void CHttpRootServletCompTest::TagHeaderNameIgnoresCaseTest()
{
	const QByteArray firstBody = CreateBody('a');
	imtrest::CHttpResponse response(200, firstBody, "text/javascript", s_engine);
	response.SetHeaders(Headers{{"ETag", "\"v1\""}});

	CServlet servlet;
	const QByteArray first = servlet.Deflate("Files/data.json", response);

	response.SetData(CreateBody('b'));

	QCOMPARE(servlet.Deflate("Files/data.json", response), first);
}


void CHttpRootServletCompTest::NewTagIsCompressedAgainTest()
{
	imtrest::CHttpResponse response(200, CreateBody('a'), "text/javascript", s_engine);
	response.SetHeaders(Headers{{"etag", "\"v1\""}});

	CServlet servlet;
	servlet.Deflate("Views/html/index.js", response);

	const QByteArray secondBody = CreateBody('b');
	response.SetData(secondBody);
	response.SetHeaders(Headers{{"etag", "\"v2\""}});

	QCOMPARE(Inflate(servlet.Deflate("Views/html/index.js", response), secondBody.size()), secondBody);
}


void CHttpRootServletCompTest::OtherCommandIsCompressedApartTest()
{
	// CHttpFileServletComp makes a tag of the size and the time of a file, two files can have the same one
	imtrest::CHttpResponse response(200, CreateBody('a'), "text/javascript", s_engine);
	response.SetHeaders(Headers{{"etag", "\"v1\""}});

	CServlet servlet;
	servlet.Deflate("Files/first.js", response);

	const QByteArray secondBody = CreateBody('b');
	response.SetData(secondBody);

	QCOMPARE(Inflate(servlet.Deflate("Files/second.js", response), secondBody.size()), secondBody);
}


void CHttpRootServletCompTest::EncodingsAreCachedApartTest()
{
	imtrest::CHttpResponse response(200, CreateBody('a'), "text/javascript", s_engine);
	response.SetHeaders(Headers{{"etag", "\"v1\""}});

	CServlet servlet;
	const QByteArray deflated = servlet.Deflate("Views/html/index.js", response);
	const QByteArray gzipped = servlet.Gzip("Views/html/index.js", response);

	QVERIFY(gzipped != deflated);
	QCOMPARE(quint8(gzipped[0]), quint8(0x1f));
	QCOMPARE(servlet.Deflate("Views/html/index.js", response), deflated);
}


void CHttpRootServletCompTest::ResponseWithoutTagIsAlwaysCompressedTest()
{
	imtrest::CHttpResponse response(200, CreateBody('a'), "application/json", s_engine);

	CServlet servlet;
	servlet.Deflate("graphql", response);

	const QByteArray secondBody = CreateBody('b');
	response.SetData(secondBody);

	QCOMPARE(Inflate(servlet.Deflate("graphql", response), secondBody.size()), secondBody);
}


I_ADD_TEST(CHttpRootServletCompTest);


