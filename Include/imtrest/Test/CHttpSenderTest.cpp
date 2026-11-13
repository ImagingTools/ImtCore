// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include "CHttpSenderTest.h"


// ImtCore includes
#include <imtrest/CHttpDateTools.h>
#include <imtrest/CHttpResponse.h>
#include <imtrest/CHttpSender.h>
#include <imtrest/IProtocolEngine.h>
#include <imtrest/http_parser.h>


namespace
{


typedef imtrest::IResponse::Headers Headers;
typedef imtrest::CHttpSender::ResponseOptions Options;


// Date of the example of RFC 9110, section 5.6.7
const qint64 s_dateSecs = 784111777;
const QByteArray s_date("Sun, 06 Nov 1994 08:49:37 GMT");


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
		case SC_CONTINUE:
			protocolStatusCode = 100;
			statusCodeLiteral = "Continue";
			return true;

		case SC_OK:
			protocolStatusCode = 200;
			statusCodeLiteral = "OK";
			return true;

		case SC_CREATED:
			protocolStatusCode = 201;
			statusCodeLiteral = "Created";
			return true;

		case SC_NO_CONTENT:
			protocolStatusCode = 204;
			statusCodeLiteral = "No Content";
			return true;

		case SC_RESET_CONTENT:
			protocolStatusCode = 205;
			statusCodeLiteral = "Reset Content";
			return true;

		case SC_NOT_MODIFIED:
			protocolStatusCode = 304;
			statusCodeLiteral = "Not Modified";
			return true;

		case SC_NOT_FOUND:
			protocolStatusCode = 404;
			statusCodeLiteral = "Not Found";
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
				int /*statusCode*/,
				const QByteArray& /*data*/,
				const QByteArray& /*dataTypeId*/) const override
	{
		return nullptr;
	}
};


Options CreateOptions(bool isConnectionClosing = false, bool isBodyOmitted = false)
{
	Options options;
	options.isConnectionClosing = isConnectionClosing;
	options.isBodyOmitted = isBodyOmitted;
	options.dateTime = QDateTime::fromSecsSinceEpoch(s_dateSecs);

	return options;
}


/**
	Build the response, an empty array if the response cannot be built.
*/
QByteArray Build(
			int statusCode,
			const QByteArray& body,
			const QByteArray& dataTypeId,
			const Headers& headers = Headers(),
			const Options& options = CreateOptions())
{
	CEngineStub engine;

	imtrest::CHttpResponse response(statusCode, body, dataTypeId, engine);
	response.SetHeaders(headers);

	QByteArray data;
	if (!imtrest::CHttpSender::BuildResponseData(response, data, options)){
		return QByteArray();
	}

	return data;
}


/**
	Response split to the status line, the headers and the body.
*/
struct SMessage
{
	QByteArray statusLine;
	QList<QPair<QByteArray, QByteArray>> headers;
	QByteArray body;

	int Count(const QByteArray& name) const
	{
		int retVal = 0;
		for (const QPair<QByteArray, QByteArray>& header : headers){
			if (header.first.toLower() == name.toLower()){
				++retVal;
			}
		}

		return retVal;
	}

	QByteArray Get(const QByteArray& name) const
	{
		for (const QPair<QByteArray, QByteArray>& header : headers){
			if (header.first.toLower() == name.toLower()){
				return header.second;
			}
		}

		return QByteArray();
	}
};


bool Split(const QByteArray& data, SMessage& message)
{
	const qsizetype headersEnd = data.indexOf("\r\n\r\n");
	if (headersEnd < 0){
		return false;
	}

	const QByteArrayList lines = data.left(headersEnd).split('\n');
	if (lines.isEmpty()){
		return false;
	}

	message = SMessage();

	for (qsizetype i = 0; i < lines.size(); ++i){
		QByteArray line = lines[i];
		if (line.endsWith('\r')){
			line.chop(1);
		}

		if (i == 0){
			message.statusLine = line;

			continue;
		}

		const qsizetype colon = line.indexOf(':');
		if (colon < 0){
			return false;
		}

		message.headers.append(qMakePair(line.left(colon), line.mid(colon + 1).trimmed()));
	}

	message.body = data.mid(headersEnd + 4);

	return true;
}


/**
	Parsing of the responses by the HTTP parser of the library, as a client does it.
*/
struct SParsedResponse
{
	int statusCode = 0;
	QByteArray body;
};


struct SParserState
{
	QList<SParsedResponse> responses;
	SParsedResponse current;
};


int OnHeadersComplete(http_parser* parserPtr)
{
	SParserState* statePtr = static_cast<SParserState*>(parserPtr->data);

	statePtr->current = SParsedResponse();
	statePtr->current.statusCode = int(parserPtr->status_code);

	// 1xx, 204 and 304 end with the headers whatever the headers say (RFC 9112, section 6.3)
	const bool isEndedByHeaders = (parserPtr->status_code < 200) || (parserPtr->status_code == 204) || (parserPtr->status_code == 304);

	return isEndedByHeaders ? 1 : 0;
}


int OnBody(http_parser* parserPtr, const char* at, size_t length)
{
	SParserState* statePtr = static_cast<SParserState*>(parserPtr->data);

	statePtr->current.body.append(at, int(length));

	return 0;
}


int OnMessageComplete(http_parser* parserPtr)
{
	SParserState* statePtr = static_cast<SParserState*>(parserPtr->data);

	statePtr->responses.append(statePtr->current);

	return 0;
}


bool ParseResponses(const QByteArray& data, SParserState& state, bool& isKeepAlive)
{
	http_parser parser;
	http_parser_init(&parser, HTTP_RESPONSE);
	parser.data = &state;

	http_parser_settings settings;
	http_parser_settings_init(&settings);
	settings.on_headers_complete = OnHeadersComplete;
	settings.on_body = OnBody;
	settings.on_message_complete = OnMessageComplete;

	const size_t parsedCount = http_parser_execute(&parser, &settings, data.constData(), size_t(data.size()));

	isKeepAlive = (http_should_keep_alive(&parser) != 0);

	return (parsedCount == size_t(data.size())) && (HTTP_PARSER_ERRNO(&parser) == HPE_OK);
}


typedef imtrest::CHttpSender::BodyFraming BodyFraming;


/**
	Build the head of a streaming response, false if it cannot be built.
*/
bool BuildHead(
			int statusCode,
			qint64 bodySize,
			const Headers& headers,
			const Options& options,
			QByteArray& head,
			BodyFraming& framing)
{
	CEngineStub engine;

	imtrest::CHttpResponse response(statusCode, QByteArrayLiteral("not used"), "application/octet-stream", engine);
	response.SetHeaders(headers);

	return imtrest::CHttpSender::BuildStreamingResponseHead(response, bodySize, options, head, framing);
}


Options CreateStreamingOptions(bool isChunkedAllowed = true, bool isBodyOmitted = false, bool isConnectionClosing = false)
{
	Options options = CreateOptions(isConnectionClosing, isBodyOmitted);
	options.isChunkedAllowed = isChunkedAllowed;

	return options;
}


const QByteArray s_headStart("HTTP/1.1 200 OK\r\nDate: " + s_date + "\r\nContent-Type: application/octet-stream\r\n");


} // namespace


// private slots

void CHttpSenderTest::ExactResponseTest()
{
	const QByteArray data = Build(imtrest::IProtocolEngine::SC_OK, "hello", "text/plain");

	QCOMPARE(data, QByteArray("HTTP/1.1 200 OK\r\n"
				"Date: " + s_date + "\r\n"
				"Content-Type: text/plain\r\n"
				"Content-Length: 5\r\n"
				"\r\n"
				"hello"));
}


void CHttpSenderTest::EmptyBodyTest()
{
	const QByteArray data = Build(imtrest::IProtocolEngine::SC_OK, QByteArray(), "text/plain");

	QCOMPARE(data, QByteArray("HTTP/1.1 200 OK\r\n"
				"Date: " + s_date + "\r\n"
				"Content-Type: text/plain\r\n"
				"Content-Length: 0\r\n"
				"\r\n"));
}


void CHttpSenderTest::BinaryBodyTest()
{
	// The body can contain anything, including the end of the headers
	const QByteArray body("a\r\n\r\n\0b\r\n", 9);
	const QByteArray data = Build(imtrest::IProtocolEngine::SC_OK, body, "application/octet-stream");

	SMessage message;
	QVERIFY(Split(data, message));

	QCOMPARE(message.Get("Content-Length"), QByteArray("9"));
	QCOMPARE(message.body, body);
}


void CHttpSenderTest::UnknownStatusTest()
{
	// The response cannot be built without the status line:
	QVERIFY(Build(imtrest::IProtocolEngine::SC_NONE, "hello", "text/plain").isEmpty());
}


void CHttpSenderTest::NoContentTypeWithoutDataTypeTest()
{
	SMessage message;
	QVERIFY(Split(Build(imtrest::IProtocolEngine::SC_OK, "hello", QByteArray()), message));

	QCOMPARE(message.Count("Content-Type"), 0);
	QCOMPARE(message.Get("Content-Length"), QByteArray("5"));
}


void CHttpSenderTest::OwnContentLengthIsReplacedTest_data()
{
	QTest::addColumn<QByteArray>("name");
	QTest::addColumn<QByteArray>("value");

	// RFC 9110, section 8.6: one Content-Length with the size of the content. A wrong value of a servlet
	// would break the next response of a persistent connection.
	QTest::newRow("same case, wrong size") << QByteArray("Content-Length") << QByteArray("99");
	QTest::newRow("lower case") << QByteArray("content-length") << QByteArray("99");
	QTest::newRow("upper case") << QByteArray("CONTENT-LENGTH") << QByteArray("99");
	QTest::newRow("correct size") << QByteArray("Content-Length") << QByteArray("5");
	QTest::newRow("invalid value") << QByteArray("Content-Length") << QByteArray("abc");
}


void CHttpSenderTest::OwnContentLengthIsReplacedTest()
{
	QFETCH(QByteArray, name);
	QFETCH(QByteArray, value);

	Headers headers;
	headers.insert(name, value);

	SMessage message;
	QVERIFY(Split(Build(imtrest::IProtocolEngine::SC_OK, "hello", "text/plain", headers), message));

	QCOMPARE(message.Count("Content-Length"), 1);
	QCOMPARE(message.Get("Content-Length"), QByteArray("5"));
	QCOMPARE(message.body, QByteArray("hello"));
}


void CHttpSenderTest::TransferEncodingHasNoContentLengthTest()
{
	// RFC 9112, section 6.2: Content-Length is not sent with Transfer-Encoding. The body is already encoded by the sender.
	Headers headers;
	headers.insert("Transfer-Encoding", "chunked");
	headers.insert("Content-Length", "5");

	const QByteArray body("5\r\nhello\r\n0\r\n\r\n");

	SMessage message;
	QVERIFY(Split(Build(imtrest::IProtocolEngine::SC_OK, body, "text/plain", headers), message));

	QCOMPARE(message.Count("Content-Length"), 0);
	QCOMPARE(message.Get("Transfer-Encoding"), QByteArray("chunked"));
	QCOMPARE(message.body, body);
}


void CHttpSenderTest::NoContentStatusesTest_data()
{
	QTest::addColumn<int>("statusCode");
	QTest::addColumn<QByteArray>("statusLine");
	QTest::addColumn<bool>("hasDate");

	// RFC 9110, section 6.4.1 and 8.6: no content and no Content-Length. Date is not required for 1xx (section 6.6.1).
	QTest::newRow("100") << int(imtrest::IProtocolEngine::SC_CONTINUE) << QByteArray("HTTP/1.1 100 Continue") << false;
	QTest::newRow("204") << int(imtrest::IProtocolEngine::SC_NO_CONTENT) << QByteArray("HTTP/1.1 204 No Content") << true;
	QTest::newRow("304") << int(imtrest::IProtocolEngine::SC_NOT_MODIFIED) << QByteArray("HTTP/1.1 304 Not Modified") << true;
}


void CHttpSenderTest::NoContentStatusesTest()
{
	QFETCH(int, statusCode);
	QFETCH(QByteArray, statusLine);
	QFETCH(bool, hasDate);

	// The servlet gives a body by mistake, it should not break the framing:
	const QByteArray data = Build(statusCode, "x", "text/plain");

	SMessage message;
	QVERIFY(Split(data, message));

	QCOMPARE(message.statusLine, statusLine);
	QCOMPARE(message.Count("Content-Length"), 0);
	QCOMPARE(message.Count("Content-Type"), 0);
	QCOMPARE(message.Count("Date"), hasDate ? 1 : 0);
	QVERIFY(message.body.isEmpty());
	QVERIFY(data.endsWith("\r\n\r\n"));
}


void CHttpSenderTest::NotModifiedKeepsOwnContentLengthTest_data()
{
	QTest::addColumn<QByteArray>("ownValue");
	QTest::addColumn<QByteArray>("expectedValue");

	// RFC 9110, section 8.6: the length of the representation that a GET would return can be sent with 304
	QTest::newRow("size") << QByteArray("1234") << QByteArray("1234");
	QTest::newRow("spaces") << QByteArray(" 12 ") << QByteArray("12");
	QTest::newRow("text") << QByteArray("abc") << QByteArray();
	QTest::newRow("empty") << QByteArray() << QByteArray();
	QTest::newRow("negative") << QByteArray("-5") << QByteArray();
	QTest::newRow("sign") << QByteArray("+5") << QByteArray();
	QTest::newRow("too large") << QByteArray("99999999999999999999") << QByteArray();
}


void CHttpSenderTest::NotModifiedKeepsOwnContentLengthTest()
{
	QFETCH(QByteArray, ownValue);
	QFETCH(QByteArray, expectedValue);

	Headers headers;
	headers.insert("Content-Length", ownValue);

	SMessage message;
	QVERIFY(Split(Build(imtrest::IProtocolEngine::SC_NOT_MODIFIED, QByteArray(), QByteArray(), headers), message));

	QCOMPARE(message.Count("Content-Length"), expectedValue.isEmpty() ? 0 : 1);
	QCOMPARE(message.Get("Content-Length"), expectedValue);
	QVERIFY(message.body.isEmpty());
}


void CHttpSenderTest::ResetContentTest()
{
	// RFC 9110, section 15.3.6: no content in 205, the length 0 ends the response on a persistent connection
	SMessage message;
	QVERIFY(Split(Build(imtrest::IProtocolEngine::SC_RESET_CONTENT, "x", "text/plain"), message));

	QCOMPARE(message.statusLine, QByteArray("HTTP/1.1 205 Reset Content"));
	QCOMPARE(message.Get("Content-Length"), QByteArray("0"));
	QCOMPARE(message.Count("Content-Type"), 0);
	QVERIFY(message.body.isEmpty());
}


void CHttpSenderTest::HeadResponseTest()
{
	// RFC 9110, section 9.3.2: the headers of GET without the content, Content-Length is the size of the content of GET
	const QByteArray data = Build(imtrest::IProtocolEngine::SC_OK, "hello", "text/plain", Headers(), CreateOptions(false, true));

	SMessage message;
	QVERIFY(Split(data, message));

	QCOMPARE(message.Get("Content-Length"), QByteArray("5"));
	QCOMPARE(message.Get("Content-Type"), QByteArray("text/plain"));
	QCOMPARE(message.Count("Date"), 1);
	QVERIFY(message.body.isEmpty());
	QVERIFY(data.endsWith("\r\n\r\n"));
}


void CHttpSenderTest::HeadResponseWithOwnLengthTest_data()
{
	QTest::addColumn<QByteArray>("body");
	QTest::addColumn<QByteArray>("ownValue");
	QTest::addColumn<QByteArray>("expectedValue");

	// A servlet that does not make the body of HEAD tells the length itself. The length is not made up:
	QTest::newRow("no body, own length") << QByteArray() << QByteArray("42") << QByteArray("42");
	QTest::newRow("no body, no length") << QByteArray() << QByteArray() << QByteArray();
	QTest::newRow("body wins") << QByteArray("hello") << QByteArray("42") << QByteArray("5");
}


void CHttpSenderTest::HeadResponseWithOwnLengthTest()
{
	QFETCH(QByteArray, body);
	QFETCH(QByteArray, ownValue);
	QFETCH(QByteArray, expectedValue);

	Headers headers;
	if (!ownValue.isEmpty()){
		headers.insert("Content-Length", ownValue);
	}

	SMessage message;
	QVERIFY(Split(Build(imtrest::IProtocolEngine::SC_OK, body, "text/plain", headers, CreateOptions(false, true)), message));

	QCOMPARE(message.Get("Content-Length"), expectedValue);
	QVERIFY(message.body.isEmpty());
}


void CHttpSenderTest::OwnContentTypeIsNotDuplicatedTest_data()
{
	QTest::addColumn<QByteArray>("name");

	QTest::newRow("same case") << QByteArray("Content-Type");
	QTest::newRow("lower case") << QByteArray("content-type");
	QTest::newRow("upper case") << QByteArray("CONTENT-TYPE");
}


void CHttpSenderTest::OwnContentTypeIsNotDuplicatedTest()
{
	QFETCH(QByteArray, name);

	Headers headers;
	headers.insert(name, "application/json");

	SMessage message;
	QVERIFY(Split(Build(imtrest::IProtocolEngine::SC_OK, "{}", "text/plain", headers), message));

	QCOMPARE(message.Count("Content-Type"), 1);
	QCOMPARE(message.Get("Content-Type"), QByteArray("application/json"));
}


void CHttpSenderTest::OtherHeadersAreKeptTest()
{
	Headers headers;
	headers.insert("Cache-Control", "no-store");
	headers.insert("X-Custom", "a, b");
	headers.insert("content-disposition", "attachment; filename=\"a.txt\"");

	SMessage message;
	QVERIFY(Split(Build(imtrest::IProtocolEngine::SC_OK, "hello", "text/plain", headers), message));

	QCOMPARE(message.Count("Cache-Control"), 1);
	QCOMPARE(message.Get("Cache-Control"), QByteArray("no-store"));
	QCOMPARE(message.Count("X-Custom"), 1);
	QCOMPARE(message.Get("X-Custom"), QByteArray("a, b"));
	QCOMPARE(message.Count("Content-Disposition"), 1);
	QCOMPARE(message.Get("Content-Disposition"), QByteArray("attachment; filename=\"a.txt\""));
}


void CHttpSenderTest::DateIsGeneratedTest_data()
{
	QTest::addColumn<int>("statusCode");
	QTest::addColumn<bool>("hasDate");

	// RFC 9110, section 6.6.1: Date is required for 2xx, 3xx and 4xx, allowed for 5xx, not needed for 1xx
	QTest::newRow("200") << int(imtrest::IProtocolEngine::SC_OK) << true;
	QTest::newRow("201") << int(imtrest::IProtocolEngine::SC_CREATED) << true;
	QTest::newRow("204") << int(imtrest::IProtocolEngine::SC_NO_CONTENT) << true;
	QTest::newRow("304") << int(imtrest::IProtocolEngine::SC_NOT_MODIFIED) << true;
	QTest::newRow("404") << int(imtrest::IProtocolEngine::SC_NOT_FOUND) << true;
	QTest::newRow("500") << int(imtrest::IProtocolEngine::SC_INTERNAL_SERVER_ERROR) << true;
	QTest::newRow("100") << int(imtrest::IProtocolEngine::SC_CONTINUE) << false;
}


void CHttpSenderTest::DateIsGeneratedTest()
{
	QFETCH(int, statusCode);
	QFETCH(bool, hasDate);

	SMessage message;
	QVERIFY(Split(Build(statusCode, QByteArray(), "text/plain"), message));

	QCOMPARE(message.Count("Date"), hasDate ? 1 : 0);

	if (hasDate){
		QCOMPARE(message.Get("Date"), s_date);
	}
}


void CHttpSenderTest::OwnDateIsKeptTest()
{
	Headers headers;
	headers.insert("date", "Mon, 07 Nov 1994 08:49:37 GMT");

	SMessage message;
	QVERIFY(Split(Build(imtrest::IProtocolEngine::SC_OK, "hello", "text/plain", headers), message));

	QCOMPARE(message.Count("Date"), 1);
	QCOMPARE(message.Get("Date"), QByteArray("Mon, 07 Nov 1994 08:49:37 GMT"));
}


void CHttpSenderTest::DateIsTakenFromClockTest()
{
	CEngineStub engine;
	imtrest::CHttpResponse response(imtrest::IProtocolEngine::SC_OK, "hello", "text/plain", engine);

	// Without the time in the options the current time is used:
	const QDateTime before = QDateTime::currentDateTimeUtc();

	QByteArray data;
	QVERIFY(imtrest::CHttpSender::BuildResponseData(response, data));

	const QDateTime after = QDateTime::currentDateTimeUtc();

	SMessage message;
	QVERIFY(Split(data, message));

	// The date has the precision of a second, so the clock can change during the call:
	const QByteArray date = message.Get("Date");
	QVERIFY((date == imtrest::CHttpDateTools::ToImfFixdate(before)) || (date == imtrest::CHttpDateTools::ToImfFixdate(after)));
}


void CHttpSenderTest::ConnectionCloseTest()
{
	// RFC 9112, section 9.6: the server that closes the connection sends the "close" option
	SMessage closingMessage;
	QVERIFY(Split(Build(imtrest::IProtocolEngine::SC_OK, "hello", "text/plain", Headers(), CreateOptions(true)), closingMessage));

	QCOMPARE(closingMessage.Count("Connection"), 1);
	QCOMPARE(closingMessage.Get("Connection"), QByteArray("close"));

	SMessage persistentMessage;
	QVERIFY(Split(Build(imtrest::IProtocolEngine::SC_OK, "hello", "text/plain"), persistentMessage));

	QCOMPARE(persistentMessage.Count("Connection"), 0);
}


void CHttpSenderTest::ConnectionOfResponseIsControlledByServerTest_data()
{
	QTest::addColumn<QByteArray>("ownValue");
	QTest::addColumn<bool>("isClosing");
	QTest::addColumn<QByteArray>("expectedValue");

	// The persistence of the connection is decided by the server, other options (Upgrade) are kept
	QTest::newRow("keep-alive, closing") << QByteArray("keep-alive") << true << QByteArray("close");
	QTest::newRow("keep-alive, persistent") << QByteArray("keep-alive") << false << QByteArray();
	QTest::newRow("close, persistent") << QByteArray("close") << false << QByteArray();
	QTest::newRow("Close, closing") << QByteArray("Close") << true << QByteArray("close");
	QTest::newRow("Upgrade, persistent") << QByteArray("Upgrade") << false << QByteArray("Upgrade");
	QTest::newRow("Upgrade, closing") << QByteArray("Upgrade") << true << QByteArray("Upgrade, close");
	QTest::newRow("list, persistent") << QByteArray("keep-alive, Upgrade") << false << QByteArray("Upgrade");
	QTest::newRow("list with spaces, closing") << QByteArray("KEEP-ALIVE , upgrade") << true << QByteArray("upgrade, close");
}


void CHttpSenderTest::ConnectionOfResponseIsControlledByServerTest()
{
	QFETCH(QByteArray, ownValue);
	QFETCH(bool, isClosing);
	QFETCH(QByteArray, expectedValue);

	Headers headers;
	headers.insert("Connection", ownValue);

	SMessage message;
	QVERIFY(Split(Build(imtrest::IProtocolEngine::SC_OK, "hello", "text/plain", headers, CreateOptions(isClosing)), message));

	QCOMPARE(message.Count("Connection"), expectedValue.isEmpty() ? 0 : 1);
	QCOMPARE(message.Get("Connection"), expectedValue);
}


void CHttpSenderTest::InvalidHeadersAreSkippedTest_data()
{
	QTest::addColumn<QByteArray>("name");
	QTest::addColumn<QByteArray>("value");

	// RFC 9110, section 5.5 and 5.6.2: such headers would allow to inject headers and responses
	QTest::newRow("LF in value") << QByteArray("X-A") << QByteArray("a\nSet-Cookie: id=1");
	QTest::newRow("CRLF in value") << QByteArray("X-A") << QByteArray("a\r\nSet-Cookie: id=1");
	QTest::newRow("CR in value") << QByteArray("X-A") << QByteArray("a\rb");
	QTest::newRow("NUL in value") << QByteArray("X-A") << QByteArray("a\0b", 3);
	QTest::newRow("space in name") << QByteArray("X Bad") << QByteArray("v");
	QTest::newRow("colon in name") << QByteArray("X:Bad") << QByteArray("v");
	QTest::newRow("CRLF in name") << QByteArray("X\r\nSet-Cookie: id=1") << QByteArray("v");
	QTest::newRow("empty name") << QByteArray() << QByteArray("v");
}


void CHttpSenderTest::InvalidHeadersAreSkippedTest()
{
	QFETCH(QByteArray, name);
	QFETCH(QByteArray, value);

	Headers headers;
	headers.insert(name, value);
	headers.insert("X-Good", "ok");

	const QByteArray data = Build(imtrest::IProtocolEngine::SC_OK, "hello", "text/plain", headers);

	SMessage message;
	QVERIFY(Split(data, message));

	QVERIFY(!data.contains("Set-Cookie"));
	QCOMPARE(message.Get("X-Good"), QByteArray("ok"));

	// Date, Content-Type, Content-Length and X-Good:
	QCOMPARE(message.headers.size(), 4);
}


void CHttpSenderTest::InvalidDataTypeIsSkippedTest()
{
	const QByteArray data = Build(imtrest::IProtocolEngine::SC_OK, "hello", "text/plain\r\nSet-Cookie: a=b");

	SMessage message;
	QVERIFY(Split(data, message));

	QVERIFY(!data.contains("Set-Cookie"));
	QCOMPARE(message.Count("Content-Type"), 0);
	QCOMPARE(message.Get("Content-Length"), QByteArray("5"));
}


void CHttpSenderTest::ResponsesAreParsedByHttpParserTest()
{
	// The responses are sent one after another over a persistent connection. A client finds the border of each of
	// them only if the framing is correct.
	const QByteArray binary("a\r\n\r\n\0b", 7);

	QByteArray stream;
	stream += Build(imtrest::IProtocolEngine::SC_OK, "hello", "text/plain");
	stream += Build(imtrest::IProtocolEngine::SC_NO_CONTENT, "ignored", "text/plain");
	stream += Build(imtrest::IProtocolEngine::SC_NOT_FOUND, "not found", "text/plain");

	Headers lengthHeaders;
	lengthHeaders.insert("Content-Length", "1000");
	stream += Build(imtrest::IProtocolEngine::SC_NOT_MODIFIED, "ignored", "text/plain", lengthHeaders);
	stream += Build(imtrest::IProtocolEngine::SC_RESET_CONTENT, "ignored", "text/plain");
	stream += Build(imtrest::IProtocolEngine::SC_CREATED, QByteArray(), "text/plain");
	stream += Build(imtrest::IProtocolEngine::SC_OK, binary, "application/octet-stream");

	SParserState state;
	bool isKeepAlive = false;
	QVERIFY(ParseResponses(stream, state, isKeepAlive));
	QVERIFY(isKeepAlive);

	QCOMPARE(state.responses.size(), 7);

	QCOMPARE(state.responses[0].statusCode, 200);
	QCOMPARE(state.responses[0].body, QByteArray("hello"));
	QCOMPARE(state.responses[1].statusCode, 204);
	QVERIFY(state.responses[1].body.isEmpty());
	QCOMPARE(state.responses[2].statusCode, 404);
	QCOMPARE(state.responses[2].body, QByteArray("not found"));
	QCOMPARE(state.responses[3].statusCode, 304);
	QVERIFY(state.responses[3].body.isEmpty());
	QCOMPARE(state.responses[4].statusCode, 205);
	QVERIFY(state.responses[4].body.isEmpty());
	QCOMPARE(state.responses[5].statusCode, 201);
	QVERIFY(state.responses[5].body.isEmpty());
	QCOMPARE(state.responses[6].statusCode, 200);
	QCOMPARE(state.responses[6].body, binary);
}


void CHttpSenderTest::ResponsesWithConnectionCloseAreParsedTest()
{
	const QByteArray data = Build(imtrest::IProtocolEngine::SC_OK, "hello", "text/plain", Headers(), CreateOptions(true));

	SParserState state;
	bool isKeepAlive = true;
	QVERIFY(ParseResponses(data, state, isKeepAlive));

	// A client that gets "close" does not wait for the next response:
	QVERIFY(!isKeepAlive);

	QCOMPARE(state.responses.size(), 1);
	QCOMPARE(state.responses[0].body, QByteArray("hello"));
}


void CHttpSenderTest::StreamingHeadContentLengthTest()
{
	QByteArray head;
	BodyFraming framing = BodyFraming::BF_NONE;

	QVERIFY(BuildHead(imtrest::IProtocolEngine::SC_OK, 1000, Headers(), CreateStreamingOptions(), head, framing));

	// The head ends with the empty line: the body is not in it
	QCOMPARE(head, s_headStart + "Content-Length: 1000\r\n\r\n");
	QCOMPARE(framing, imtrest::CHttpSender::BF_CONTENT_LENGTH);
}


void CHttpSenderTest::StreamingHeadLargeSizeTest()
{
	QByteArray head;
	BodyFraming framing = BodyFraming::BF_NONE;

	// More than 4 GiB: the size is not cut to int
	QVERIFY(BuildHead(imtrest::IProtocolEngine::SC_OK, 10000000000LL, Headers(), CreateStreamingOptions(), head, framing));

	QCOMPARE(head, s_headStart + "Content-Length: 10000000000\r\n\r\n");
	QCOMPARE(framing, imtrest::CHttpSender::BF_CONTENT_LENGTH);
}


void CHttpSenderTest::StreamingHeadEmptyBodyTest()
{
	QByteArray head;
	BodyFraming framing = BodyFraming::BF_CONTENT_LENGTH;

	QVERIFY(BuildHead(imtrest::IProtocolEngine::SC_OK, 0, Headers(), CreateStreamingOptions(), head, framing));

	// There is nothing to stream: the body of the size 0 is not sent
	QCOMPARE(head, s_headStart + "Content-Length: 0\r\n\r\n");
	QCOMPARE(framing, imtrest::CHttpSender::BF_NONE);
}


void CHttpSenderTest::StreamingHeadChunkedTest()
{
	QByteArray head;
	BodyFraming framing = BodyFraming::BF_NONE;

	QVERIFY(BuildHead(imtrest::IProtocolEngine::SC_OK, -1, Headers(), CreateStreamingOptions(), head, framing));

	// The size is not known: no Content-Length (RFC 9112, section 6.2)
	QCOMPARE(head, s_headStart + "Transfer-Encoding: chunked\r\n\r\n");
	QCOMPARE(framing, imtrest::CHttpSender::BF_CHUNKED);
}


void CHttpSenderTest::StreamingHeadNegativeSizeIsUnknownTest()
{
	QByteArray head;
	BodyFraming framing = BodyFraming::BF_NONE;

	QVERIFY(BuildHead(imtrest::IProtocolEngine::SC_OK, -12345, Headers(), CreateStreamingOptions(), head, framing));

	QCOMPARE(head, s_headStart + "Transfer-Encoding: chunked\r\n\r\n");
	QCOMPARE(framing, imtrest::CHttpSender::BF_CHUNKED);
}


void CHttpSenderTest::StreamingHeadUntilCloseTest()
{
	QByteArray head;
	BodyFraming framing = BodyFraming::BF_NONE;

	// A client of HTTP/1.0 does not know the chunked encoding: the body ends with the connection (RFC 9112, section 6.3)
	QVERIFY(BuildHead(imtrest::IProtocolEngine::SC_OK, -1, Headers(), CreateStreamingOptions(false), head, framing));

	QCOMPARE(head, s_headStart + "Connection: close\r\n\r\n");
	QCOMPARE(framing, imtrest::CHttpSender::BF_UNTIL_CLOSE);
}


void CHttpSenderTest::StreamingHeadUntilCloseWithClosingConnectionTest()
{
	QByteArray head;
	BodyFraming framing = BodyFraming::BF_NONE;

	QVERIFY(BuildHead(imtrest::IProtocolEngine::SC_OK, -1, Headers(), CreateStreamingOptions(false, false, true), head, framing));

	// Only one Connection header:
	SMessage message;
	QVERIFY(Split(head, message));
	QCOMPARE(message.Count("Connection"), 1);
	QCOMPARE(message.Get("Connection"), QByteArray("close"));
	QCOMPARE(framing, imtrest::CHttpSender::BF_UNTIL_CLOSE);
}


void CHttpSenderTest::StreamingHeadKnownSizeNeedsNoChunksTest()
{
	QByteArray head;
	BodyFraming framing = BodyFraming::BF_NONE;

	// The size is known: the chunked encoding is not used even for a client that supports it, and the connection stays open
	QVERIFY(BuildHead(imtrest::IProtocolEngine::SC_OK, 5, Headers(), CreateStreamingOptions(false), head, framing));

	QCOMPARE(head, s_headStart + "Content-Length: 5\r\n\r\n");
	QCOMPARE(framing, imtrest::CHttpSender::BF_CONTENT_LENGTH);
}


void CHttpSenderTest::StreamingHeadIgnoresOwnFramingTest()
{
	Headers headers;
	headers.insert("Content-Length", "5");
	headers.insert("transfer-encoding", "gzip, chunked");

	QByteArray head;
	BodyFraming framing = BodyFraming::BF_NONE;

	// The sender decides how the body is framed: the values of the response do not match the body that will be sent
	QVERIFY(BuildHead(imtrest::IProtocolEngine::SC_OK, 1000, headers, CreateStreamingOptions(), head, framing));

	SMessage message;
	QVERIFY(Split(head, message));
	QCOMPARE(message.Count("Content-Length"), 1);
	QCOMPARE(message.Get("Content-Length"), QByteArray("1000"));
	QCOMPARE(message.Count("Transfer-Encoding"), 0);

	QVERIFY(BuildHead(imtrest::IProtocolEngine::SC_OK, -1, headers, CreateStreamingOptions(), head, framing));

	QVERIFY(Split(head, message));
	QCOMPARE(message.Count("Content-Length"), 0);
	QCOMPARE(message.Count("Transfer-Encoding"), 1);
	QCOMPARE(message.Get("Transfer-Encoding"), QByteArray("chunked"));
}


void CHttpSenderTest::StreamingHeadKeepsOtherHeadersTest()
{
	Headers headers;
	headers.insert("ETag", "\"v1\"");
	headers.insert("Content-Range", "bytes 0-9/100");
	headers.insert("Accept-Ranges", "bytes");
	headers.insert("Content-Type", "text/plain");

	QByteArray head;
	BodyFraming framing = BodyFraming::BF_NONE;

	QVERIFY(BuildHead(imtrest::IProtocolEngine::SC_OK, 10, headers, CreateStreamingOptions(), head, framing));

	SMessage message;
	QVERIFY(Split(head, message));

	QCOMPARE(message.Get("ETag"), QByteArray("\"v1\""));
	QCOMPARE(message.Get("Content-Range"), QByteArray("bytes 0-9/100"));
	QCOMPARE(message.Get("Accept-Ranges"), QByteArray("bytes"));

	// The own type is not duplicated by the type of the data:
	QCOMPARE(message.Count("Content-Type"), 1);
	QCOMPARE(message.Get("Content-Type"), QByteArray("text/plain"));
	QCOMPARE(message.Get("Content-Length"), QByteArray("10"));
}


void CHttpSenderTest::StreamingHeadConnectionCloseTest()
{
	QByteArray head;
	BodyFraming framing = BodyFraming::BF_NONE;

	QVERIFY(BuildHead(imtrest::IProtocolEngine::SC_OK, 5, Headers(), CreateStreamingOptions(true, false, true), head, framing));

	QCOMPARE(head, s_headStart + "Content-Length: 5\r\nConnection: close\r\n\r\n");
	QCOMPARE(framing, imtrest::CHttpSender::BF_CONTENT_LENGTH);
}


void CHttpSenderTest::StreamingHeadOfHeadRequestTest()
{
	QByteArray head;
	BodyFraming framing = BodyFraming::BF_CONTENT_LENGTH;

	// The answer to HEAD has the headers of the answer to GET and no body: the size of the representation is the length
	QVERIFY(BuildHead(imtrest::IProtocolEngine::SC_OK, 1000, Headers(), CreateStreamingOptions(true, true), head, framing));

	QCOMPARE(head, s_headStart + "Content-Length: 1000\r\n\r\n");
	QCOMPARE(framing, imtrest::CHttpSender::BF_NONE);

	// An unknown size is not sent as chunks: there is no body
	QVERIFY(BuildHead(imtrest::IProtocolEngine::SC_OK, -1, Headers(), CreateStreamingOptions(true, true), head, framing));

	QCOMPARE(head, s_headStart + "\r\n");
	QCOMPARE(framing, imtrest::CHttpSender::BF_NONE);

	// The length set by the response is the only source if the size is not known (RFC 9110, section 9.3.2)
	Headers headers;
	headers.insert("Content-Length", "777");

	QVERIFY(BuildHead(imtrest::IProtocolEngine::SC_OK, -1, headers, CreateStreamingOptions(true, true), head, framing));

	QCOMPARE(head, s_headStart + "Content-Length: 777\r\n\r\n");
	QCOMPARE(framing, imtrest::CHttpSender::BF_NONE);

	// HEAD for a client of HTTP/1.0 with an unknown size: no body, so the connection is not closed because of it
	QVERIFY(BuildHead(imtrest::IProtocolEngine::SC_OK, -1, Headers(), CreateStreamingOptions(false, true), head, framing));

	QCOMPARE(head, s_headStart + "\r\n");
	QCOMPARE(framing, imtrest::CHttpSender::BF_NONE);
}


void CHttpSenderTest::StreamingHeadWithoutBodyStatusesTest()
{
	QByteArray head;
	BodyFraming framing = BodyFraming::BF_CONTENT_LENGTH;
	SMessage message;

	// 204: no body and no length even if the size of the representation is given
	QVERIFY(BuildHead(imtrest::IProtocolEngine::SC_NO_CONTENT, 1000, Headers(), CreateStreamingOptions(), head, framing));
	QVERIFY(Split(head, message));
	QCOMPARE(message.Count("Content-Length"), 0);
	QCOMPARE(message.Count("Transfer-Encoding"), 0);
	QCOMPARE(framing, imtrest::CHttpSender::BF_NONE);

	// 304: no body, the length of the representation is the value of the response
	Headers headers;
	headers.insert("Content-Length", "42");

	framing = BodyFraming::BF_CHUNKED;
	QVERIFY(BuildHead(imtrest::IProtocolEngine::SC_NOT_MODIFIED, -1, headers, CreateStreamingOptions(), head, framing));
	QVERIFY(Split(head, message));
	QCOMPARE(message.Get("Content-Length"), QByteArray("42"));
	QCOMPARE(message.Count("Transfer-Encoding"), 0);
	QCOMPARE(framing, imtrest::CHttpSender::BF_NONE);

	// 205: no body and the length 0
	framing = BodyFraming::BF_CHUNKED;
	QVERIFY(BuildHead(imtrest::IProtocolEngine::SC_RESET_CONTENT, 1000, Headers(), CreateStreamingOptions(), head, framing));
	QVERIFY(Split(head, message));
	QCOMPARE(message.Get("Content-Length"), QByteArray("0"));
	QCOMPARE(message.Count("Transfer-Encoding"), 0);
	QCOMPARE(framing, imtrest::CHttpSender::BF_NONE);

	// 1xx: no body, no length and no Date
	framing = BodyFraming::BF_CHUNKED;
	QVERIFY(BuildHead(imtrest::IProtocolEngine::SC_CONTINUE, 1000, Headers(), CreateStreamingOptions(), head, framing));
	QVERIFY(Split(head, message));
	QCOMPARE(message.Count("Content-Length"), 0);
	QCOMPARE(message.Count("Transfer-Encoding"), 0);
	QCOMPARE(message.Count("Date"), 0);
	QCOMPARE(framing, imtrest::CHttpSender::BF_NONE);
}


void CHttpSenderTest::StreamingHeadUnknownStatusTest()
{
	QByteArray head;
	BodyFraming framing = BodyFraming::BF_NONE;

	QVERIFY(!BuildHead(imtrest::IProtocolEngine::SC_NONE, 10, Headers(), CreateStreamingOptions(), head, framing));
}


void CHttpSenderTest::StreamingHeadSkipsInvalidHeadersTest()
{
	Headers headers;
	headers.insert("X-Good", "value");
	headers.insert("X-Bad", "a\r\nSet-Cookie: injected");

	QByteArray head;
	BodyFraming framing = BodyFraming::BF_NONE;

	QVERIFY(BuildHead(imtrest::IProtocolEngine::SC_OK, 5, headers, CreateStreamingOptions(), head, framing));

	QVERIFY(head.contains("X-Good: value\r\n"));
	QVERIFY(!head.contains("Set-Cookie"));
	QVERIFY(!head.contains("X-Bad"));
}


void CHttpSenderTest::CreateChunkTest_data()
{
	QTest::addColumn<QByteArray>("data");
	QTest::addColumn<QByteArray>("expected");

	QTest::newRow("text") << QByteArray("hello") << QByteArray("5\r\nhello\r\n");
	QTest::newRow("one byte") << QByteArray("a") << QByteArray("1\r\na\r\n");
	QTest::newRow("end of line inside") << QByteArray("\r\n") << QByteArray("2\r\n\r\n\r\n");
	QTest::newRow("binary") << QByteArray("a\0b", 3) << QByteArray("3\r\na\0b\r\n", 8);
	QTest::newRow("10 bytes") << QByteArray(10, 'x') << QByteArray("a\r\n") + QByteArray(10, 'x') + QByteArray("\r\n");
	QTest::newRow("255 bytes") << QByteArray(255, 'x') << QByteArray("ff\r\n") + QByteArray(255, 'x') + QByteArray("\r\n");
	QTest::newRow("256 bytes") << QByteArray(256, 'x') << QByteArray("100\r\n") + QByteArray(256, 'x') + QByteArray("\r\n");
	QTest::newRow("256 KiB") << QByteArray(262144, 'x') << QByteArray("40000\r\n") + QByteArray(262144, 'x') + QByteArray("\r\n");
}


void CHttpSenderTest::CreateChunkTest()
{
	QFETCH(QByteArray, data);
	QFETCH(QByteArray, expected);

	QCOMPARE(imtrest::CHttpSender::CreateChunk(data), expected);
}


void CHttpSenderTest::CreateChunkOfEmptyDataTest()
{
	// A chunk of the size 0 finishes the body, so empty data give no chunk
	QVERIFY(imtrest::CHttpSender::CreateChunk(QByteArray()).isEmpty());
}


void CHttpSenderTest::CreateLastChunkTest()
{
	QCOMPARE(imtrest::CHttpSender::CreateLastChunk(), QByteArray("0\r\n\r\n"));
}


void CHttpSenderTest::ChunkedResponseIsParsedByHttpParserTest()
{
	QByteArray head;
	BodyFraming framing = BodyFraming::BF_NONE;
	QVERIFY(BuildHead(imtrest::IProtocolEngine::SC_OK, -1, Headers(), CreateStreamingOptions(), head, framing));

	// The parts are not equal and one of them contains the text of a chunk: a client takes the body by the sizes only
	const QByteArray firstPart("hello ");
	const QByteArray secondPart("5\r\nworld\r\n0\r\n\r\n");
	const QByteArray thirdPart(300000, 'z');

	QByteArray stream = head;
	stream += imtrest::CHttpSender::CreateChunk(firstPart);
	stream += imtrest::CHttpSender::CreateChunk(secondPart);
	stream += imtrest::CHttpSender::CreateChunk(thirdPart);
	stream += imtrest::CHttpSender::CreateLastChunk();

	// The next response on the same connection:
	stream += Build(imtrest::IProtocolEngine::SC_OK, "next", "text/plain");

	SParserState state;
	bool isKeepAlive = false;
	QVERIFY(ParseResponses(stream, state, isKeepAlive));
	QVERIFY(isKeepAlive);

	QCOMPARE(state.responses.size(), 2);
	QCOMPARE(state.responses[0].statusCode, 200);
	QCOMPARE(state.responses[0].body, firstPart + secondPart + thirdPart);
	QCOMPARE(state.responses[1].body, QByteArray("next"));
}


void CHttpSenderTest::SizedStreamingResponseIsParsedByHttpParserTest()
{
	const QByteArray body("0123456789");

	QByteArray head;
	BodyFraming framing = BodyFraming::BF_NONE;
	QVERIFY(BuildHead(imtrest::IProtocolEngine::SC_OK, body.size(), Headers(), CreateStreamingOptions(), head, framing));

	QByteArray stream = head + body;

	// The next response on the same connection is not taken for a part of the body:
	stream += Build(imtrest::IProtocolEngine::SC_OK, "next", "text/plain");

	SParserState state;
	bool isKeepAlive = false;
	QVERIFY(ParseResponses(stream, state, isKeepAlive));
	QVERIFY(isKeepAlive);

	QCOMPARE(state.responses.size(), 2);
	QCOMPARE(state.responses[0].body, body);
	QCOMPARE(state.responses[1].body, QByteArray("next"));
}


I_ADD_TEST(CHttpSenderTest);


