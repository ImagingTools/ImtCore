// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include "CHttpFileStreamingServletTest.h"


// STL includes
#include <memory>

// Qt includes
#include <QtCore/QBuffer>
#include <QtCore/QFile>
#include <QtCore/QFileInfo>
#include <QtCore/QTemporaryDir>

// ImtCore includes
#include <imtrest/CHttpConditionalTools.h>
#include <imtrest/CHttpDateTools.h>
#include <imtrest/CHttpFileServletComp.h>
#include <imtrest/CHttpRequest.h>
#include <imtrest/CHttpResponse.h>
#include <imtrest/CHttpSender.h>
#include <imtrest/IHttpFileResolver.h>
#include <imtrest/IProtocolEngine.h>
#include <imtrest/IStreamingResponse.h>


namespace
{


typedef imtrest::CHttpFileServletComp Servlet;
typedef imtrest::IProtocolEngine Engine;
typedef imtrest::IResponse::Headers Headers;

const qint64 s_fileSize = 10000;

// Date of the example of RFC 9110, section 5.6.7
const qint64 s_modifiedSecs = 784111777;
const QByteArray s_modifiedDate("Sun, 06 Nov 1994 08:49:37 GMT");
const QByteArray s_laterDate("Mon, 07 Nov 1994 08:49:37 GMT");
const QByteArray s_earlierDate("Sat, 05 Nov 1994 08:49:37 GMT");

const QByteArray s_eTag("\"v1\"");


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

	bool GetProtocolStatusCode(int statusCode, int& protocolStatusCode, QByteArray& statusCodeLiteral) const override
	{
		switch (statusCode){
		case SC_OK:
			protocolStatusCode = 200;
			statusCodeLiteral = "OK";
			return true;

		case SC_PARTIAL_CONTENT:
			protocolStatusCode = 206;
			statusCodeLiteral = "Partial Content";
			return true;

		case SC_NOT_MODIFIED:
			protocolStatusCode = 304;
			statusCodeLiteral = "Not Modified";
			return true;

		case SC_RANGE_NOT_SATISFIABLE:
			protocolStatusCode = 416;
			statusCodeLiteral = "Range Not Satisfiable";
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
				int /*statusCode*/,
				const QByteArray& /*data*/,
				const QByteArray& /*dataTypeId*/) const override
	{
		return nullptr;
	}
};


// The responses and the requests refer to the engine, so it lives as long as the test
CEngineStub s_engine;


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


CRequestServletStub s_requestServlet;


/**
	Resolver that gives the file that is set by the test.
*/
class CResolverStub: virtual public imtrest::IHttpFileResolver
{
public:
	Engine::StatusCode status = Engine::SC_OK;
	FileDescription description;
	mutable QByteArray lastCommandId;

	// reimplemented (imtrest::IHttpFileResolver)
	Engine::StatusCode ResolveFile(const imtrest::CHttpRequest& request, FileDescription& file) const override
	{
		lastCommandId = request.GetCommandId();
		file = description;

		return status;
	}
};


/**
	Data of the file at the given position: every byte depends on its position, so lost, repeated and moved parts are found.
*/
QByteArray CreatePattern(qint64 offset, qint64 size)
{
	QByteArray retVal(qsizetype(size), Qt::Uninitialized);

	for (qint64 i = 0; i < size; ++i){
		retVal[qsizetype(i)] = char(((offset + i) * 31 + 7) & 0xFF);
	}

	return retVal;
}


// A file in a temporary directory
class CFile
{
public:
	bool Create(qint64 size)
	{
		if (!m_dir.isValid()){
			return false;
		}

		m_path = m_dir.filePath(QStringLiteral("data.bin"));

		QFile file(m_path);
		if (!file.open(QIODevice::WriteOnly)){
			return false;
		}

		return file.write(CreatePattern(0, size)) == size;
	}

	QString GetPath() const
	{
		return m_path;
	}

	QString GetDirectoryPath() const
	{
		return m_dir.path();
	}

private:
	QTemporaryDir m_dir;
	QString m_path;
};


Servlet::FileRepresentation CreateRepresentation(const QString& filePath, qint64 size = s_fileSize)
{
	Servlet::FileRepresentation retVal;
	retVal.filePath = filePath;
	retVal.size = size;
	retVal.lastModified = QDateTime::fromSecsSinceEpoch(s_modifiedSecs);
	retVal.eTag = s_eTag;

	return retVal;
}


// The values are the headers of the request in lower case: range, if-range, if-match and so on
Servlet::RequestConditions CreateConditions(const Headers& headers)
{
	Servlet::RequestConditions retVal;
	retVal.range = headers.value("range");
	retVal.ifRange = headers.value("if-range");
	retVal.preconditions.ifMatch = headers.value("if-match");
	retVal.preconditions.ifNoneMatch = headers.value("if-none-match");
	retVal.preconditions.ifModifiedSince = headers.value("if-modified-since");
	retVal.preconditions.ifUnmodifiedSince = headers.value("if-unmodified-since");

	return retVal;
}


imtrest::ConstResponsePtr Respond(bool isHeadRequest, const Headers& headers, const Servlet::FileRepresentation& file)
{
	return Servlet::CreateFileResponse(s_engine, isHeadRequest, CreateConditions(headers), file);
}


const imtrest::IStreamingResponse* AsStreaming(const imtrest::ConstResponsePtr& response)
{
	return dynamic_cast<const imtrest::IStreamingResponse*>(response.GetPtr());
}


int CountHeader(const imtrest::IResponse& response, const QByteArray& name)
{
	int retVal = 0;

	const Headers headers = response.GetHeaders();
	for (Headers::ConstIterator iter = headers.constBegin(); iter != headers.constEnd(); ++iter){
		if (iter.key().toLower() == name.toLower()){
			++retVal;
		}
	}

	return retVal;
}


QByteArray GetHeader(const imtrest::IResponse& response, const QByteArray& name)
{
	const Headers headers = response.GetHeaders();
	for (Headers::ConstIterator iter = headers.constBegin(); iter != headers.constEnd(); ++iter){
		if (iter.key().toLower() == name.toLower()){
			return iter.value();
		}
	}

	return QByteArray();
}


/**
	Read the body of a streaming response as the server does, by small parts.
*/
bool ReadBody(const imtrest::ConstResponsePtr& response, QByteArray& body)
{
	const imtrest::IStreamingResponse* streamingPtr = AsStreaming(response);
	if (streamingPtr == nullptr){
		return false;
	}

	imtrest::IResponseBodySourcePtr sourcePtr = streamingPtr->CreateBodySource();
	if (!sourcePtr){
		return false;
	}

	body.clear();

	while (true){
		QByteArray part;

		switch (sourcePtr->Read(part, 4096)){
		case imtrest::IResponseBodySource::RR_DATA:
			body.append(part);
			break;

		case imtrest::IResponseBodySource::RR_END:
			return true;

		default:
			return false;
		}
	}
}


/**
	The request as the connection gives it to the servlet.
*/
std::unique_ptr<imtrest::CHttpRequest> CreateRequest(const QByteArray& method, const QByteArray& target, const Headers& headers)
{
	QByteArray raw = method + " " + target + " HTTP/1.1\r\nHost: localhost\r\n";

	for (Headers::ConstIterator iter = headers.constBegin(); iter != headers.constEnd(); ++iter){
		raw += iter.key() + ": " + iter.value() + "\r\n";
	}

	raw += "\r\n";

	std::unique_ptr<imtrest::CHttpRequest> requestPtr(new imtrest::CHttpRequest(s_requestServlet, s_engine));

	QBuffer buffer;
	buffer.setData(raw);
	buffer.open(QIODevice::ReadOnly);

	if (!requestPtr->ParseDeviceData(buffer) || (requestPtr->GetState() != imtrest::IRequest::RS_MESSAGE_COMPLETE)){
		return nullptr;
	}

	return requestPtr;
}


imtrest::ConstResponsePtr Process(const CResolverStub& resolver, const QByteArray& method, const Headers& headers)
{
	std::unique_ptr<imtrest::CHttpRequest> requestPtr = CreateRequest(method, "/files/data.bin", headers);
	if (!requestPtr){
		return imtrest::ConstResponsePtr();
	}

	return Servlet::ProcessFileRequest(resolver, *requestPtr, method == "HEAD");
}


} // namespace


// private slots

void CHttpFileStreamingServletTest::WholeFileTest()
{
	CFile file;
	QVERIFY(file.Create(s_fileSize));

	const imtrest::ConstResponsePtr response = Respond(false, Headers(), CreateRepresentation(file.GetPath()));
	QVERIFY(response.GetPtr() != nullptr);

	QCOMPARE(response->GetStatusCode(), int(Engine::SC_OK));
	QCOMPARE(response->GetDataTypeId(), QByteArray("application/octet-stream"));

	const imtrest::IStreamingResponse* streamingPtr = AsStreaming(response);
	QVERIFY(streamingPtr != nullptr);
	QCOMPARE(streamingPtr->GetBodySize(), s_fileSize);

	// The headers of the representation, and the server tells that the ranges are supported
	QCOMPARE(GetHeader(*response, "Accept-Ranges"), QByteArray("bytes"));
	QCOMPARE(GetHeader(*response, "ETag"), s_eTag);
	QCOMPARE(GetHeader(*response, "Last-Modified"), s_modifiedDate);
	QCOMPARE(CountHeader(*response, "Content-Range"), 0);

	QByteArray body;
	QVERIFY(ReadBody(response, body));
	QVERIFY(body == CreatePattern(0, s_fileSize));
}


void CHttpFileStreamingServletTest::HeadTest()
{
	// The headers of GET without the body, the file is not opened. Range is defined for GET only (RFC 9110, section 14.2).
	const imtrest::ConstResponsePtr response = Respond(true, {{"range", "bytes=0-99"}}, CreateRepresentation(QStringLiteral("/not/existing/file")));
	QVERIFY(response.GetPtr() != nullptr);

	QCOMPARE(response->GetStatusCode(), int(Engine::SC_OK));

	const imtrest::IStreamingResponse* streamingPtr = AsStreaming(response);
	QVERIFY(streamingPtr != nullptr);
	QCOMPARE(streamingPtr->GetBodySize(), s_fileSize);

	QCOMPARE(CountHeader(*response, "Content-Range"), 0);
	QCOMPARE(GetHeader(*response, "Accept-Ranges"), QByteArray("bytes"));
	QCOMPARE(GetHeader(*response, "ETag"), s_eTag);
}


void CHttpFileStreamingServletTest::RangeTest_data()
{
	QTest::addColumn<QByteArray>("range");
	QTest::addColumn<qint64>("first");
	QTest::addColumn<qint64>("length");
	QTest::addColumn<QByteArray>("contentRange");

	QTest::newRow("beginning") << QByteArray("bytes=0-99") << qint64(0) << qint64(100) << QByteArray("bytes 0-99/10000");
	QTest::newRow("from the position") << QByteArray("bytes=100-") << qint64(100) << qint64(9900) << QByteArray("bytes 100-9999/10000");
	QTest::newRow("suffix") << QByteArray("bytes=-100") << qint64(9900) << qint64(100) << QByteArray("bytes 9900-9999/10000");
	QTest::newRow("end beyond the file") << QByteArray("bytes=9900-20000") << qint64(9900) << qint64(100) << QByteArray("bytes 9900-9999/10000");
	QTest::newRow("first byte") << QByteArray("bytes=0-0") << qint64(0) << qint64(1) << QByteArray("bytes 0-0/10000");
	QTest::newRow("last byte") << QByteArray("bytes=9999-9999") << qint64(9999) << qint64(1) << QByteArray("bytes 9999-9999/10000");
	QTest::newRow("suffix longer than the file") << QByteArray("bytes=-99999") << qint64(0) << qint64(10000) << QByteArray("bytes 0-9999/10000");
	QTest::newRow("unit in upper case") << QByteArray("BYTES=5-9") << qint64(5) << qint64(5) << QByteArray("bytes 5-9/10000");
	QTest::newRow("unreachable range is skipped") << QByteArray("bytes=0-9,20000-") << qint64(0) << qint64(10) << QByteArray("bytes 0-9/10000");
}


void CHttpFileStreamingServletTest::RangeTest()
{
	QFETCH(QByteArray, range);
	QFETCH(qint64, first);
	QFETCH(qint64, length);
	QFETCH(QByteArray, contentRange);

	CFile file;
	QVERIFY(file.Create(s_fileSize));

	const imtrest::ConstResponsePtr response = Respond(false, {{"range", range}}, CreateRepresentation(file.GetPath()));
	QVERIFY(response.GetPtr() != nullptr);

	QCOMPARE(response->GetStatusCode(), int(Engine::SC_PARTIAL_CONTENT));
	QCOMPARE(GetHeader(*response, "Content-Range"), contentRange);
	QCOMPARE(CountHeader(*response, "Content-Range"), 1);

	// The validators of the representation are sent with the part (RFC 9110, section 15.3.7)
	QCOMPARE(GetHeader(*response, "ETag"), s_eTag);
	QCOMPARE(GetHeader(*response, "Accept-Ranges"), QByteArray("bytes"));

	const imtrest::IStreamingResponse* streamingPtr = AsStreaming(response);
	QVERIFY(streamingPtr != nullptr);
	QCOMPARE(streamingPtr->GetBodySize(), length);

	QByteArray body;
	QVERIFY(ReadBody(response, body));
	QVERIFY(body == CreatePattern(first, length));
}


void CHttpFileStreamingServletTest::RangeIsIgnoredTest_data()
{
	QTest::addColumn<QByteArray>("range");

	// RFC 9110, section 14.2: the header that cannot be used is ignored and the whole file is sent
	QTest::newRow("unknown unit") << QByteArray("items=0-9");
	QTest::newRow("not a number") << QByteArray("bytes=abc");
	QTest::newRow("end before start") << QByteArray("bytes=5-1");
	QTest::newRow("bad element") << QByteArray("bytes=0-9,x");
	QTest::newRow("no ranges") << QByteArray("bytes=");

	// Several ranges need multipart/byteranges, it is not supported
	QTest::newRow("two separate ranges") << QByteArray("bytes=0-9,20-29");

	QByteArray manyRanges("bytes=0-0");
	for (int i = 1; i < 17; ++i){
		manyRanges += "," + QByteArray::number(i * 2) + "-" + QByteArray::number(i * 2);
	}

	QTest::newRow("too many ranges") << manyRanges;
}


void CHttpFileStreamingServletTest::RangeIsIgnoredTest()
{
	QFETCH(QByteArray, range);

	CFile file;
	QVERIFY(file.Create(s_fileSize));

	const imtrest::ConstResponsePtr response = Respond(false, {{"range", range}}, CreateRepresentation(file.GetPath()));
	QVERIFY(response.GetPtr() != nullptr);

	QCOMPARE(response->GetStatusCode(), int(Engine::SC_OK));
	QCOMPARE(CountHeader(*response, "Content-Range"), 0);

	const imtrest::IStreamingResponse* streamingPtr = AsStreaming(response);
	QVERIFY(streamingPtr != nullptr);
	QCOMPARE(streamingPtr->GetBodySize(), s_fileSize);

	QByteArray body;
	QVERIFY(ReadBody(response, body));
	QVERIFY(body == CreatePattern(0, s_fileSize));
}


void CHttpFileStreamingServletTest::OverlappingRangesTest()
{
	CFile file;
	QVERIFY(file.Create(s_fileSize));

	// The ranges that overlap or touch are one range, so the part can be sent without multipart
	const imtrest::ConstResponsePtr overlapping = Respond(false, {{"range", "bytes=0-99,50-149"}}, CreateRepresentation(file.GetPath()));
	QVERIFY(overlapping.GetPtr() != nullptr);
	QCOMPARE(overlapping->GetStatusCode(), int(Engine::SC_PARTIAL_CONTENT));
	QCOMPARE(GetHeader(*overlapping, "Content-Range"), QByteArray("bytes 0-149/10000"));

	QByteArray body;
	QVERIFY(ReadBody(overlapping, body));
	QVERIFY(body == CreatePattern(0, 150));

	const imtrest::ConstResponsePtr touching = Respond(false, {{"range", "bytes=100-199,0-99"}}, CreateRepresentation(file.GetPath()));
	QVERIFY(touching.GetPtr() != nullptr);
	QCOMPARE(touching->GetStatusCode(), int(Engine::SC_PARTIAL_CONTENT));
	QCOMPARE(GetHeader(*touching, "Content-Range"), QByteArray("bytes 0-199/10000"));
}


void CHttpFileStreamingServletTest::RangeNotSatisfiableTest_data()
{
	QTest::addColumn<QByteArray>("range");

	QTest::newRow("start at the end") << QByteArray("bytes=10000-");
	QTest::newRow("beyond the file") << QByteArray("bytes=20000-30000");
	QTest::newRow("empty suffix") << QByteArray("bytes=-0");
	QTest::newRow("all ranges beyond the file") << QByteArray("bytes=10000-10010,20000-");
}


void CHttpFileStreamingServletTest::RangeNotSatisfiableTest()
{
	QFETCH(QByteArray, range);

	// The file is not opened: the answer is made of the size
	const imtrest::ConstResponsePtr response = Respond(false, {{"range", range}}, CreateRepresentation(QStringLiteral("/not/existing/file")));
	QVERIFY(response.GetPtr() != nullptr);

	QCOMPARE(response->GetStatusCode(), int(Engine::SC_RANGE_NOT_SATISFIABLE));

	// RFC 9110, section 15.5.17: the unsatisfied range tells the size of the file, so the client can ask again
	QCOMPARE(GetHeader(*response, "Content-Range"), QByteArray("bytes */10000"));
	QCOMPARE(GetHeader(*response, "Accept-Ranges"), QByteArray("bytes"));

	QVERIFY(AsStreaming(response) == nullptr);
	QVERIFY(response->GetData().isEmpty());
}


void CHttpFileStreamingServletTest::EmptyFileTest()
{
	CFile file;
	QVERIFY(file.Create(0));

	const imtrest::ConstResponsePtr whole = Respond(false, Headers(), CreateRepresentation(file.GetPath(), 0));
	QVERIFY(whole.GetPtr() != nullptr);
	QCOMPARE(whole->GetStatusCode(), int(Engine::SC_OK));

	const imtrest::IStreamingResponse* streamingPtr = AsStreaming(whole);
	QVERIFY(streamingPtr != nullptr);
	QCOMPARE(streamingPtr->GetBodySize(), qint64(0));

	QByteArray body("old");
	QVERIFY(ReadBody(whole, body));
	QVERIFY(body.isEmpty());

	// No range is inside of an empty file
	const imtrest::ConstResponsePtr ranged = Respond(false, {{"range", "bytes=0-"}}, CreateRepresentation(file.GetPath(), 0));
	QVERIFY(ranged.GetPtr() != nullptr);
	QCOMPARE(ranged->GetStatusCode(), int(Engine::SC_RANGE_NOT_SATISFIABLE));
	QCOMPARE(GetHeader(*ranged, "Content-Range"), QByteArray("bytes */0"));
}


void CHttpFileStreamingServletTest::IfRangeTest_data()
{
	QTest::addColumn<QByteArray>("range");
	QTest::addColumn<QByteArray>("ifRange");
	QTest::addColumn<int>("statusCode");

	const QByteArray range("bytes=0-99");

	// RFC 9110, section 13.1.5: the part is sent only if the file is not changed, otherwise the client gets the whole file
	QTest::newRow("same tag") << range << s_eTag << int(Engine::SC_PARTIAL_CONTENT);
	QTest::newRow("other tag") << range << QByteArray("\"other\"") << int(Engine::SC_OK);
	QTest::newRow("weak tag") << range << QByteArray("W/\"v1\"") << int(Engine::SC_OK);
	QTest::newRow("same date") << range << s_modifiedDate << int(Engine::SC_PARTIAL_CONTENT);
	QTest::newRow("later date") << range << s_laterDate << int(Engine::SC_OK);
	QTest::newRow("earlier date") << range << s_earlierDate << int(Engine::SC_OK);
	QTest::newRow("not a tag and not a date") << range << QByteArray("garbage") << int(Engine::SC_OK);

	// If-Range without Range means nothing
	QTest::newRow("no range") << QByteArray() << s_eTag << int(Engine::SC_OK);
}


void CHttpFileStreamingServletTest::IfRangeTest()
{
	QFETCH(QByteArray, range);
	QFETCH(QByteArray, ifRange);
	QFETCH(int, statusCode);

	CFile file;
	QVERIFY(file.Create(s_fileSize));

	const imtrest::ConstResponsePtr response = Respond(false, {{"range", range}, {"if-range", ifRange}}, CreateRepresentation(file.GetPath()));
	QVERIFY(response.GetPtr() != nullptr);

	QCOMPARE(response->GetStatusCode(), statusCode);

	const imtrest::IStreamingResponse* streamingPtr = AsStreaming(response);
	QVERIFY(streamingPtr != nullptr);

	if (statusCode == Engine::SC_PARTIAL_CONTENT){
		QCOMPARE(streamingPtr->GetBodySize(), qint64(100));
	}
	else{
		QCOMPARE(streamingPtr->GetBodySize(), s_fileSize);
		QCOMPARE(CountHeader(*response, "Content-Range"), 0);
	}
}


void CHttpFileStreamingServletTest::PreconditionsTest_data()
{
	QTest::addColumn<bool>("isHeadRequest");
	QTest::addColumn<QByteArray>("headerName");
	QTest::addColumn<QByteArray>("headerValue");
	QTest::addColumn<int>("statusCode");

	const int ok = int(Engine::SC_OK);
	const int notModified = int(Engine::SC_NOT_MODIFIED);
	const int failed = int(Engine::SC_PRECONDITION_FAILED);
	const int badRequest = int(Engine::SC_BAD_REQUEST);

	// If-None-Match: the cache has the same version (weak comparison, RFC 9110, section 13.1.2)
	QTest::newRow("GET If-None-Match same") << false << QByteArray("If-None-Match") << s_eTag << notModified;
	QTest::newRow("GET If-None-Match weak same") << false << QByteArray("If-None-Match") << QByteArray("W/\"v1\"") << notModified;
	QTest::newRow("GET If-None-Match other") << false << QByteArray("If-None-Match") << QByteArray("\"other\"") << ok;
	QTest::newRow("GET If-None-Match star") << false << QByteArray("If-None-Match") << QByteArray("*") << notModified;
	QTest::newRow("GET If-None-Match list") << false << QByteArray("If-None-Match") << QByteArray("\"a\", \"v1\"") << notModified;
	QTest::newRow("GET If-None-Match bad") << false << QByteArray("If-None-Match") << QByteArray("not a tag") << badRequest;

	// If-Match: the client changes the version that it has seen (strong comparison, RFC 9110, section 13.1.1)
	QTest::newRow("GET If-Match same") << false << QByteArray("If-Match") << s_eTag << ok;
	QTest::newRow("GET If-Match other") << false << QByteArray("If-Match") << QByteArray("\"other\"") << failed;
	QTest::newRow("GET If-Match weak") << false << QByteArray("If-Match") << QByteArray("W/\"v1\"") << failed;
	QTest::newRow("GET If-Match star") << false << QByteArray("If-Match") << QByteArray("*") << ok;
	QTest::newRow("GET If-Match bad") << false << QByteArray("If-Match") << QByteArray("not a tag") << badRequest;

	// The dates
	QTest::newRow("GET If-Modified-Since same") << false << QByteArray("If-Modified-Since") << s_modifiedDate << notModified;
	QTest::newRow("GET If-Modified-Since later") << false << QByteArray("If-Modified-Since") << s_laterDate << notModified;
	QTest::newRow("GET If-Modified-Since earlier") << false << QByteArray("If-Modified-Since") << s_earlierDate << ok;
	QTest::newRow("GET If-Modified-Since bad") << false << QByteArray("If-Modified-Since") << QByteArray("garbage") << ok;
	QTest::newRow("GET If-Unmodified-Since earlier") << false << QByteArray("If-Unmodified-Since") << s_earlierDate << failed;
	QTest::newRow("GET If-Unmodified-Since same") << false << QByteArray("If-Unmodified-Since") << s_modifiedDate << ok;
	QTest::newRow("GET If-Unmodified-Since later") << false << QByteArray("If-Unmodified-Since") << s_laterDate << ok;

	// HEAD is answered by the same rules as GET
	QTest::newRow("HEAD If-None-Match same") << true << QByteArray("If-None-Match") << s_eTag << notModified;
	QTest::newRow("HEAD If-Modified-Since same") << true << QByteArray("If-Modified-Since") << s_modifiedDate << notModified;
	QTest::newRow("HEAD If-Match other") << true << QByteArray("If-Match") << QByteArray("\"other\"") << failed;
}


void CHttpFileStreamingServletTest::PreconditionsTest()
{
	QFETCH(bool, isHeadRequest);
	QFETCH(QByteArray, headerName);
	QFETCH(QByteArray, headerValue);
	QFETCH(int, statusCode);

	CFile file;
	QVERIFY(file.Create(s_fileSize));

	const imtrest::ConstResponsePtr response = Respond(isHeadRequest, {{headerName.toLower(), headerValue}}, CreateRepresentation(file.GetPath()));
	QVERIFY(response.GetPtr() != nullptr);

	QCOMPARE(response->GetStatusCode(), statusCode);

	if (statusCode == Engine::SC_OK){
		QVERIFY(AsStreaming(response) != nullptr);

		return;
	}

	// The answers without a body
	QVERIFY(AsStreaming(response) == nullptr);
	QVERIFY(response->GetData().isEmpty());

	if (statusCode == Engine::SC_NOT_MODIFIED){
		// RFC 9110, section 15.4.5: the validators are sent, so the cache can refresh the entry
		QCOMPARE(GetHeader(*response, "ETag"), s_eTag);
		QCOMPARE(GetHeader(*response, "Last-Modified"), s_modifiedDate);
		QCOMPARE(CountHeader(*response, "Content-Range"), 0);
	}
}


void CHttpFileStreamingServletTest::PreconditionIsBeforeRangeTest()
{
	CFile file;
	QVERIFY(file.Create(s_fileSize));

	// The preconditions are checked before the range (RFC 9110, section 13.2.2): the client that has the file gets 304, not a part of it
	const imtrest::ConstResponsePtr notModified = Respond(false, {{"if-none-match", s_eTag}, {"range", "bytes=0-9"}}, CreateRepresentation(file.GetPath()));
	QVERIFY(notModified.GetPtr() != nullptr);
	QCOMPARE(notModified->GetStatusCode(), int(Engine::SC_NOT_MODIFIED));

	const imtrest::ConstResponsePtr failed = Respond(false, {{"if-match", "\"other\""}, {"range", "bytes=0-9"}}, CreateRepresentation(file.GetPath()));
	QVERIFY(failed.GetPtr() != nullptr);
	QCOMPARE(failed->GetStatusCode(), int(Engine::SC_PRECONDITION_FAILED));

	// A range that is not satisfiable is answered after the preconditions as well
	const imtrest::ConstResponsePtr notSatisfiable = Respond(false, {{"if-none-match", s_eTag}, {"range", "bytes=20000-"}}, CreateRepresentation(file.GetPath()));
	QVERIFY(notSatisfiable.GetPtr() != nullptr);
	QCOMPARE(notSatisfiable->GetStatusCode(), int(Engine::SC_NOT_MODIFIED));

	// The condition is not met: the part is sent
	const imtrest::ConstResponsePtr part = Respond(false, {{"if-none-match", "\"other\""}, {"range", "bytes=0-9"}}, CreateRepresentation(file.GetPath()));
	QVERIFY(part.GetPtr() != nullptr);
	QCOMPARE(part->GetStatusCode(), int(Engine::SC_PARTIAL_CONTENT));
}


void CHttpFileStreamingServletTest::HeadersOfResponsesTest()
{
	CFile file;
	QVERIFY(file.Create(s_fileSize));

	Servlet::FileRepresentation representation = CreateRepresentation(file.GetPath());
	representation.headers.insert("Content-Disposition", "attachment; filename=\"a.bin\"");
	representation.headers.insert("Cache-Control", "no-store");

	// The headers of the servlet are in the responses that describe the representation
	const imtrest::ConstResponsePtr whole = Respond(false, Headers(), representation);
	const imtrest::ConstResponsePtr part = Respond(false, {{"range", "bytes=0-9"}}, representation);
	const imtrest::ConstResponsePtr notModified = Respond(false, {{"if-none-match", s_eTag}}, representation);

	QVERIFY(whole.GetPtr() != nullptr);
	QVERIFY(part.GetPtr() != nullptr);
	QVERIFY(notModified.GetPtr() != nullptr);

	const imtrest::ConstResponsePtr responses[] = {whole, part, notModified};
	for (const imtrest::ConstResponsePtr& response : responses){
		QCOMPARE(GetHeader(*response, "Content-Disposition"), QByteArray("attachment; filename=\"a.bin\""));
		QCOMPARE(GetHeader(*response, "Cache-Control"), QByteArray("no-store"));
	}

	// The validators are sent only if they are known
	Servlet::FileRepresentation withoutValidators = CreateRepresentation(file.GetPath());
	withoutValidators.eTag.clear();
	withoutValidators.lastModified = QDateTime();

	const imtrest::ConstResponsePtr response = Respond(false, Headers(), withoutValidators);
	QVERIFY(response.GetPtr() != nullptr);
	QCOMPARE(CountHeader(*response, "ETag"), 0);
	QCOMPARE(CountHeader(*response, "Last-Modified"), 0);
	QCOMPARE(GetHeader(*response, "Accept-Ranges"), QByteArray("bytes"));
}


void CHttpFileStreamingServletTest::ContentTypeTest()
{
	CFile file;
	QVERIFY(file.Create(s_fileSize));

	Servlet::FileRepresentation representation = CreateRepresentation(file.GetPath());
	representation.dataTypeId = "application/pdf";

	const imtrest::ConstResponsePtr whole = Respond(false, Headers(), representation);
	const imtrest::ConstResponsePtr part = Respond(false, {{"range", "bytes=0-9"}}, representation);

	QVERIFY(whole.GetPtr() != nullptr);
	QVERIFY(part.GetPtr() != nullptr);

	QCOMPARE(whole->GetDataTypeId(), QByteArray("application/pdf"));
	QCOMPARE(part->GetDataTypeId(), QByteArray("application/pdf"));
}


void CHttpFileStreamingServletTest::HeadOfResponseTest()
{
	CFile file;
	QVERIFY(file.Create(s_fileSize));

	imtrest::CHttpSender::ResponseOptions options;
	options.isChunkedAllowed = true;
	options.dateTime = QDateTime::fromSecsSinceEpoch(s_modifiedSecs);

	QByteArray head;
	imtrest::CHttpSender::BodyFraming framing = imtrest::CHttpSender::BF_NONE;

	// The head of the whole file: the length is known
	const imtrest::ConstResponsePtr whole = Respond(false, Headers(), CreateRepresentation(file.GetPath()));
	QVERIFY(whole.GetPtr() != nullptr);
	QVERIFY(imtrest::CHttpSender::BuildStreamingResponseHead(*whole, AsStreaming(whole)->GetBodySize(), options, head, framing));

	QVERIFY(head.startsWith("HTTP/1.1 200 OK\r\n"));
	QVERIFY(head.contains("\r\nContent-Length: 10000\r\n"));
	QVERIFY(head.contains("\r\nContent-Type: application/octet-stream\r\n"));
	QVERIFY(head.contains("\r\nAccept-Ranges: bytes\r\n"));
	QVERIFY(head.contains("\r\nETag: \"v1\"\r\n"));
	QVERIFY(!head.contains("Transfer-Encoding"));
	QCOMPARE(framing, imtrest::CHttpSender::BF_CONTENT_LENGTH);

	// The head of a part: the length is the length of the part
	const imtrest::ConstResponsePtr part = Respond(false, {{"range", "bytes=0-99"}}, CreateRepresentation(file.GetPath()));
	QVERIFY(part.GetPtr() != nullptr);
	QVERIFY(imtrest::CHttpSender::BuildStreamingResponseHead(*part, AsStreaming(part)->GetBodySize(), options, head, framing));

	QVERIFY(head.startsWith("HTTP/1.1 206 Partial Content\r\n"));
	QVERIFY(head.contains("\r\nContent-Range: bytes 0-99/10000\r\n"));
	QVERIFY(head.contains("\r\nContent-Length: 100\r\n"));
	QCOMPARE(framing, imtrest::CHttpSender::BF_CONTENT_LENGTH);

	// HEAD: the length of the file and no body
	imtrest::CHttpSender::ResponseOptions headOptions = options;
	headOptions.isBodyOmitted = true;

	const imtrest::ConstResponsePtr headResponse = Respond(true, Headers(), CreateRepresentation(file.GetPath()));
	QVERIFY(headResponse.GetPtr() != nullptr);
	QVERIFY(imtrest::CHttpSender::BuildStreamingResponseHead(*headResponse, AsStreaming(headResponse)->GetBodySize(), headOptions, head, framing));

	QVERIFY(head.startsWith("HTTP/1.1 200 OK\r\n"));
	QVERIFY(head.contains("\r\nContent-Length: 10000\r\n"));
	QCOMPARE(framing, imtrest::CHttpSender::BF_NONE);

	// 416: no body, the size is in the unsatisfied range
	const imtrest::ConstResponsePtr notSatisfiable = Respond(false, {{"range", "bytes=20000-"}}, CreateRepresentation(file.GetPath()));
	QVERIFY(notSatisfiable.GetPtr() != nullptr);

	QByteArray data;
	QVERIFY(imtrest::CHttpSender::BuildResponseData(*notSatisfiable, data, options));

	QVERIFY(data.startsWith("HTTP/1.1 416 Range Not Satisfiable\r\n"));
	QVERIFY(data.contains("\r\nContent-Range: bytes */10000\r\n"));
	QVERIFY(data.contains("\r\nContent-Length: 0\r\n"));
	QVERIFY(data.endsWith("\r\n\r\n"));
}


void CHttpFileStreamingServletTest::BodySourceOfChangedFileTest()
{
	CFile file;
	QVERIFY(file.Create(s_fileSize));

	const imtrest::ConstResponsePtr response = Respond(false, Headers(), CreateRepresentation(file.GetPath()));
	QVERIFY(response.GetPtr() != nullptr);

	const imtrest::IStreamingResponse* streamingPtr = AsStreaming(response);
	QVERIFY(streamingPtr != nullptr);

	// The file became shorter after the head was made: the response would be broken, so the body is not started and the client gets 500
	QFile changedFile(file.GetPath());
	QVERIFY(changedFile.open(QIODevice::ReadWrite));
	QVERIFY(changedFile.resize(5000));
	changedFile.close();

	QVERIFY(!streamingPtr->CreateBodySource());

	// The file is removed
	QVERIFY(QFile::remove(file.GetPath()));
	QVERIFY(!streamingPtr->CreateBodySource());
}


void CHttpFileStreamingServletTest::DefaultETagTest()
{
	const QDateTime time = QDateTime::fromMSecsSinceEpoch(784111777123);

	const QByteArray eTag = Servlet::CreateDefaultETag(100, time);
	QVERIFY(!eTag.isEmpty());

	// A valid strong tag: the ranges can use it
	imtrest::CHttpConditionalTools::EntityTag parsed;
	QVERIFY(imtrest::CHttpConditionalTools::ParseEntityTag(eTag, parsed));
	QVERIFY(!parsed.isWeak);

	QCOMPARE(Servlet::CreateDefaultETag(100, time), eTag);

	// It changes with the content: the size or the time, even by a millisecond
	QVERIFY(Servlet::CreateDefaultETag(101, time) != eTag);
	QVERIFY(Servlet::CreateDefaultETag(100, time.addMSecs(1)) != eTag);
	QVERIFY(Servlet::CreateDefaultETag(100, time.addSecs(1)) != eTag);

	// A file whose time is not known still has a valid tag
	imtrest::CHttpConditionalTools::EntityTag parsedWithoutTime;
	QVERIFY(imtrest::CHttpConditionalTools::ParseEntityTag(Servlet::CreateDefaultETag(0, QDateTime()), parsedWithoutTime));

	// A time before the epoch does not break the tag
	imtrest::CHttpConditionalTools::EntityTag parsedOldTime;
	QVERIFY(imtrest::CHttpConditionalTools::ParseEntityTag(Servlet::CreateDefaultETag(5, QDateTime::fromMSecsSinceEpoch(-1000)), parsedOldTime));
}


void CHttpFileStreamingServletTest::ServletGetTest()
{
	CFile file;
	QVERIFY(file.Create(s_fileSize));

	CResolverStub resolver;
	resolver.description.filePath = file.GetPath();
	resolver.description.dataTypeId = "text/plain";

	const imtrest::ConstResponsePtr response = Process(resolver, "GET", Headers());
	QVERIFY(response.GetPtr() != nullptr);

	// The resolver gets the path of the request
	QCOMPARE(resolver.lastCommandId, QByteArray("/files/data.bin"));
	QCOMPARE(response->GetStatusCode(), int(Engine::SC_OK));
	QCOMPARE(response->GetDataTypeId(), QByteArray("text/plain"));

	const imtrest::IStreamingResponse* streamingPtr = AsStreaming(response);
	QVERIFY(streamingPtr != nullptr);
	QCOMPARE(streamingPtr->GetBodySize(), s_fileSize);

	QByteArray body;
	QVERIFY(ReadBody(response, body));
	QVERIFY(body == CreatePattern(0, s_fileSize));
}


void CHttpFileStreamingServletTest::ServletHeadTest()
{
	CFile file;
	QVERIFY(file.Create(s_fileSize));

	CResolverStub resolver;
	resolver.description.filePath = file.GetPath();

	const imtrest::ConstResponsePtr response = Process(resolver, "HEAD", {{"Range", "bytes=0-9"}});
	QVERIFY(response.GetPtr() != nullptr);

	QCOMPARE(response->GetStatusCode(), int(Engine::SC_OK));
	QCOMPARE(GetHeader(*response, "Accept-Ranges"), QByteArray("bytes"));

	const imtrest::IStreamingResponse* streamingPtr = AsStreaming(response);
	QVERIFY(streamingPtr != nullptr);
	QCOMPARE(streamingPtr->GetBodySize(), s_fileSize);
}


void CHttpFileStreamingServletTest::ServletHeadersReachDecisionsTest()
{
	CFile file;
	QVERIFY(file.Create(s_fileSize));

	CResolverStub resolver;
	resolver.description.filePath = file.GetPath();

	// The names of the headers of the request are not case-sensitive
	const imtrest::ConstResponsePtr part = Process(resolver, "GET", {{"RANGE", "bytes=0-9"}});
	QVERIFY(part.GetPtr() != nullptr);
	QCOMPARE(part->GetStatusCode(), int(Engine::SC_PARTIAL_CONTENT));

	QByteArray body;
	QVERIFY(ReadBody(part, body));
	QVERIFY(body == CreatePattern(0, 10));

	const QFileInfo fileInfo(file.GetPath());
	const QByteArray eTag = Servlet::CreateDefaultETag(fileInfo.size(), fileInfo.lastModified());

	const imtrest::ConstResponsePtr notModified = Process(resolver, "GET", {{"If-None-Match", eTag}});
	QVERIFY(notModified.GetPtr() != nullptr);
	QCOMPARE(notModified->GetStatusCode(), int(Engine::SC_NOT_MODIFIED));

	const imtrest::ConstResponsePtr failed = Process(resolver, "GET", {{"If-Match", "\"other\""}});
	QVERIFY(failed.GetPtr() != nullptr);
	QCOMPARE(failed->GetStatusCode(), int(Engine::SC_PRECONDITION_FAILED));

	// The client that continues the download of the same file gets the rest, and the client with a different file gets the whole file
	const imtrest::ConstResponsePtr resumed = Process(resolver, "GET", {{"Range", "bytes=100-"}, {"If-Range", eTag}});
	QVERIFY(resumed.GetPtr() != nullptr);
	QCOMPARE(resumed->GetStatusCode(), int(Engine::SC_PARTIAL_CONTENT));

	const imtrest::ConstResponsePtr restarted = Process(resolver, "GET", {{"Range", "bytes=100-"}, {"If-Range", "\"other\""}});
	QVERIFY(restarted.GetPtr() != nullptr);
	QCOMPARE(restarted->GetStatusCode(), int(Engine::SC_OK));
}


void CHttpFileStreamingServletTest::ServletDefaultETagTest()
{
	CFile file;
	QVERIFY(file.Create(s_fileSize));

	CResolverStub resolver;
	resolver.description.filePath = file.GetPath();

	const imtrest::ConstResponsePtr response = Process(resolver, "GET", Headers());
	QVERIFY(response.GetPtr() != nullptr);

	// The size and the time are taken from the file system
	const QFileInfo fileInfo(file.GetPath());
	QCOMPARE(GetHeader(*response, "ETag"), Servlet::CreateDefaultETag(fileInfo.size(), fileInfo.lastModified()));
	QCOMPARE(GetHeader(*response, "Last-Modified"), imtrest::CHttpDateTools::ToImfFixdate(fileInfo.lastModified()));
}


void CHttpFileStreamingServletTest::ServletOwnETagTest()
{
	CFile file;
	QVERIFY(file.Create(s_fileSize));

	CResolverStub resolver;
	resolver.description.filePath = file.GetPath();
	resolver.description.eTag = "\"mine\"";

	const imtrest::ConstResponsePtr response = Process(resolver, "GET", Headers());
	QVERIFY(response.GetPtr() != nullptr);
	QCOMPARE(GetHeader(*response, "ETag"), QByteArray("\"mine\""));

	const imtrest::ConstResponsePtr notModified = Process(resolver, "GET", {{"If-None-Match", "\"mine\""}});
	QVERIFY(notModified.GetPtr() != nullptr);
	QCOMPARE(notModified->GetStatusCode(), int(Engine::SC_NOT_MODIFIED));
}


void CHttpFileStreamingServletTest::ServletRefusalTest_data()
{
	QTest::addColumn<int>("statusCode");

	QTest::newRow("not found") << int(Engine::SC_NOT_FOUND);
	QTest::newRow("forbidden") << int(Engine::SC_FORBIDDEN);
	QTest::newRow("unauthorized") << int(Engine::SC_UNAUTHORIZED);
}


void CHttpFileStreamingServletTest::ServletRefusalTest()
{
	QFETCH(int, statusCode);

	CFile file;
	QVERIFY(file.Create(s_fileSize));

	// The resolver refuses: the file exists, but the client has no right to it. The file is not sent
	CResolverStub resolver;
	resolver.status = Engine::StatusCode(statusCode);
	resolver.description.filePath = file.GetPath();

	const imtrest::ConstResponsePtr response = Process(resolver, "GET", Headers());
	QVERIFY(response.GetPtr() != nullptr);

	QCOMPARE(response->GetStatusCode(), statusCode);
	QVERIFY(AsStreaming(response) == nullptr);
	QVERIFY(response->GetData().isEmpty());
	QCOMPARE(CountHeader(*response, "ETag"), 0);
}


void CHttpFileStreamingServletTest::ServletMissingFileTest()
{
	CFile file;
	QVERIFY(file.Create(s_fileSize));

	CResolverStub resolver;

	// The file does not exist
	resolver.description.filePath = file.GetDirectoryPath() + QStringLiteral("/missing.bin");

	const imtrest::ConstResponsePtr missing = Process(resolver, "GET", Headers());
	QVERIFY(missing.GetPtr() != nullptr);
	QCOMPARE(missing->GetStatusCode(), int(Engine::SC_NOT_FOUND));

	// A directory is not a file
	resolver.description.filePath = file.GetDirectoryPath();

	const imtrest::ConstResponsePtr directory = Process(resolver, "GET", Headers());
	QVERIFY(directory.GetPtr() != nullptr);
	QCOMPARE(directory->GetStatusCode(), int(Engine::SC_NOT_FOUND));

	// The resolver did not give a path
	resolver.description.filePath.clear();

	const imtrest::ConstResponsePtr empty = Process(resolver, "HEAD", Headers());
	QVERIFY(empty.GetPtr() != nullptr);
	QCOMPARE(empty->GetStatusCode(), int(Engine::SC_NOT_FOUND));
}


void CHttpFileStreamingServletTest::ServletRefusalKeepsHeadersTest()
{
	CResolverStub resolver;
	resolver.status = Engine::SC_UNAUTHORIZED;
	resolver.description.headers.insert("WWW-Authenticate", "Bearer");

	// The refusal needs its own headers (RFC 9110, section 11.6.1)
	const imtrest::ConstResponsePtr response = Process(resolver, "GET", Headers());
	QVERIFY(response.GetPtr() != nullptr);

	QCOMPARE(response->GetStatusCode(), int(Engine::SC_UNAUTHORIZED));
	QCOMPARE(GetHeader(*response, "WWW-Authenticate"), QByteArray("Bearer"));
}


void CHttpFileStreamingServletTest::ServletWithoutResolverTest()
{
	// The servlet is assembled without the resolver (a mistake of the assembly): the client gets 500 and the server goes on
	Servlet servlet;

	const QByteArray methods[] = {QByteArrayLiteral("GET"), QByteArrayLiteral("HEAD")};
	for (const QByteArray& method : methods){
		std::unique_ptr<imtrest::CHttpRequest> requestPtr = CreateRequest(method, "/files/data.bin", Headers());
		QVERIFY(requestPtr);

		const imtrest::ConstResponsePtr response = servlet.ProcessRequest(*requestPtr);
		QVERIFY(response.GetPtr() != nullptr);

		QCOMPARE(response->GetStatusCode(), int(Engine::SC_INTERNAL_SERVER_ERROR));
		QVERIFY(AsStreaming(response) == nullptr);
		QVERIFY(response->GetData().isEmpty());
	}
}


I_ADD_TEST(CHttpFileStreamingServletTest);
