// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include "CConnectionTimeoutBehaviorTest.h"


// Qt includes
#include <QtCore/QTimeZone>

// ImtCore includes
#include <imtrest/CConnectionTimeoutBehavior.h>
#include <imtrest/ProtocolTypeIds.h>


// private slots

void CConnectionTimeoutBehaviorTest::CreateHttpRequestTimeoutResponseTest_data()
{
	QTest::addColumn<qint64>("secsSinceEpoch");
	QTest::addColumn<QByteArray>("date");

	// The example of RFC 9110, section 5.6.7
	QTest::newRow("RFC example") << qint64(784111777) << QByteArray("Sun, 06 Nov 1994 08:49:37 GMT");
	QTest::newRow("epoch") << qint64(0) << QByteArray("Thu, 01 Jan 1970 00:00:00 GMT");
	QTest::newRow("leap day") << qint64(951782400) << QByteArray("Tue, 29 Feb 2000 00:00:00 GMT");
}


void CConnectionTimeoutBehaviorTest::CreateHttpRequestTimeoutResponseTest()
{
	QFETCH(qint64, secsSinceEpoch);
	QFETCH(QByteArray, date);

	const QDateTime dateTime = QDateTime::fromSecsSinceEpoch(secsSinceEpoch);

	QCOMPARE(
				imtrest::CConnectionTimeoutBehavior::CreateHttpRequestTimeoutResponse(dateTime),
				QByteArray("HTTP/1.1 408 Request Timeout\r\n"
							"Date: " + date + "\r\n"
							"Connection: close\r\n"
							"Content-Length: 0\r\n"
							"\r\n"));
}


void CConnectionTimeoutBehaviorTest::CreateHttpRequestTimeoutResponseIgnoresTimeZoneTest()
{
	// The same moment in the UTC and in a time zone with an offset gives the same date in GMT:
	const QDateTime utcTime = QDateTime::fromSecsSinceEpoch(784111777, QTimeZone::UTC);
	const QDateTime offsetTime = QDateTime::fromSecsSinceEpoch(784111777, QTimeZone::fromSecondsAheadOfUtc(5 * 3600));

	QCOMPARE(utcTime, offsetTime);
	QCOMPARE(
				imtrest::CConnectionTimeoutBehavior::CreateHttpRequestTimeoutResponse(offsetTime),
				imtrest::CConnectionTimeoutBehavior::CreateHttpRequestTimeoutResponse(utcTime));
}


void CConnectionTimeoutBehaviorTest::ProtocolTypeIdsTest()
{
	// The IDs are texts in the lower case, so other protocols can have their own IDs:
	QCOMPARE(imtrest::ProtocolTypeIds::s_http, QByteArray("http"));
	QCOMPARE(imtrest::ProtocolTypeIds::s_tcp, QByteArray("tcp"));
	QCOMPARE(imtrest::ProtocolTypeIds::s_webSocket, QByteArray("ws"));
}


void CConnectionTimeoutBehaviorTest::DefaultBehaviorTest()
{
	const imtrest::CConnectionTimeoutBehavior behavior;

	QVERIFY(!behavior.createRequestTimeoutMessage);
	QVERIFY(!behavior.createBadRequestMessage);
	QVERIFY(!behavior.createProcessingTimeoutMessage);
	QVERIFY(!behavior.createServerErrorMessage);
	QVERIFY(behavior.isLimitedBeforeFirstMessageOnly);
}


void CConnectionTimeoutBehaviorTest::HttpBehaviorTest_data()
{
	QTest::addColumn<QByteArray>("protocolTypeId");

	QTest::newRow("constant") << imtrest::ProtocolTypeIds::s_http;
	QTest::newRow("lower case") << QByteArray("http");
	QTest::newRow("upper case") << QByteArray("HTTP");
	QTest::newRow("mixed case") << QByteArray("Http");
}


void CConnectionTimeoutBehaviorTest::HttpBehaviorTest()
{
	QFETCH(QByteArray, protocolTypeId);

	const imtrest::CConnectionTimeoutBehavior behavior = imtrest::CConnectionTimeoutBehavior::CreateForProtocol(protocolTypeId);

	// The keep-alive timeout is applied between the requests, the connection is not a persistent channel:
	QVERIFY(!behavior.isLimitedBeforeFirstMessageOnly);

	QVERIFY(bool(behavior.createRequestTimeoutMessage));

	const QByteArray message = behavior.createRequestTimeoutMessage();
	QVERIFY(message.startsWith("HTTP/1.1 408 Request Timeout\r\n"));
	QVERIFY(message.contains("\r\nConnection: close\r\n"));
	QVERIFY(message.endsWith("\r\n\r\n"));

	QVERIFY(bool(behavior.createBadRequestMessage));

	const QByteArray badRequestMessage = behavior.createBadRequestMessage();
	QVERIFY(badRequestMessage.startsWith("HTTP/1.1 400 Bad Request\r\n"));
	QVERIFY(badRequestMessage.contains("\r\nConnection: close\r\n"));
	QVERIFY(badRequestMessage.endsWith("\r\n\r\n"));

	QVERIFY(bool(behavior.createProcessingTimeoutMessage));
	QVERIFY(behavior.createProcessingTimeoutMessage().startsWith("HTTP/1.1 503 Service Unavailable\r\n"));

	QVERIFY(bool(behavior.createServerErrorMessage));
	QVERIFY(behavior.createServerErrorMessage().startsWith("HTTP/1.1 500 Internal Server Error\r\n"));
}


void CConnectionTimeoutBehaviorTest::HttpBehaviorCreatesCurrentDateTest()
{
	const imtrest::CConnectionTimeoutBehavior behavior = imtrest::CConnectionTimeoutBehavior::CreateForProtocol(imtrest::ProtocolTypeIds::s_http);
	QVERIFY(bool(behavior.createRequestTimeoutMessage));

	const QDateTime before = QDateTime::currentDateTimeUtc();
	const QByteArray message = behavior.createRequestTimeoutMessage();
	const QDateTime after = QDateTime::currentDateTimeUtc();

	// The message is created at the moment of the call (the date has the precision of a second):
	const QByteArray responseBefore = imtrest::CConnectionTimeoutBehavior::CreateHttpRequestTimeoutResponse(before);
	const QByteArray responseAfter = imtrest::CConnectionTimeoutBehavior::CreateHttpRequestTimeoutResponse(after);

	QVERIFY((message == responseBefore) || (message == responseAfter));
}


void CConnectionTimeoutBehaviorTest::CreateHttpBadRequestResponseTest()
{
	// The example of RFC 9110, section 5.6.7
	const QDateTime dateTime = QDateTime::fromSecsSinceEpoch(784111777);

	QCOMPARE(
				imtrest::CConnectionTimeoutBehavior::CreateHttpBadRequestResponse(dateTime),
				QByteArray("HTTP/1.1 400 Bad Request\r\n"
							"Date: Sun, 06 Nov 1994 08:49:37 GMT\r\n"
							"Connection: close\r\n"
							"Content-Length: 0\r\n"
							"\r\n"));
}


void CConnectionTimeoutBehaviorTest::HttpBadRequestMessageIsCreatedAtCallTest()
{
	const imtrest::CConnectionTimeoutBehavior behavior = imtrest::CConnectionTimeoutBehavior::CreateForProtocol(imtrest::ProtocolTypeIds::s_http);
	QVERIFY(bool(behavior.createBadRequestMessage));

	const QDateTime before = QDateTime::currentDateTimeUtc();
	const QByteArray message = behavior.createBadRequestMessage();
	const QDateTime after = QDateTime::currentDateTimeUtc();

	const QByteArray responseBefore = imtrest::CConnectionTimeoutBehavior::CreateHttpBadRequestResponse(before);
	const QByteArray responseAfter = imtrest::CConnectionTimeoutBehavior::CreateHttpBadRequestResponse(after);

	QVERIFY((message == responseBefore) || (message == responseAfter));
}


void CConnectionTimeoutBehaviorTest::CreateHttpProcessingTimeoutResponseTest()
{
	// The example of RFC 9110, section 5.6.7
	const QDateTime dateTime = QDateTime::fromSecsSinceEpoch(784111777);

	// 503 is the status of the server that cannot handle the request now (RFC 9110, section 15.6.4), the connection is closed:
	QCOMPARE(
				imtrest::CConnectionTimeoutBehavior::CreateHttpProcessingTimeoutResponse(dateTime),
				QByteArray("HTTP/1.1 503 Service Unavailable\r\n"
							"Date: Sun, 06 Nov 1994 08:49:37 GMT\r\n"
							"Connection: close\r\n"
							"Content-Length: 0\r\n"
							"\r\n"));
}


void CConnectionTimeoutBehaviorTest::HttpProcessingTimeoutMessageIsCreatedAtCallTest()
{
	const imtrest::CConnectionTimeoutBehavior behavior = imtrest::CConnectionTimeoutBehavior::CreateForProtocol(imtrest::ProtocolTypeIds::s_http);
	QVERIFY(bool(behavior.createProcessingTimeoutMessage));

	const QDateTime before = QDateTime::currentDateTimeUtc();
	const QByteArray message = behavior.createProcessingTimeoutMessage();
	const QDateTime after = QDateTime::currentDateTimeUtc();

	const QByteArray responseBefore = imtrest::CConnectionTimeoutBehavior::CreateHttpProcessingTimeoutResponse(before);
	const QByteArray responseAfter = imtrest::CConnectionTimeoutBehavior::CreateHttpProcessingTimeoutResponse(after);

	QVERIFY((message == responseBefore) || (message == responseAfter));
}


void CConnectionTimeoutBehaviorTest::CreateHttpInternalServerErrorResponseTest()
{
	const QDateTime dateTime = QDateTime::fromSecsSinceEpoch(784111777);

	// The connection is kept, the request was read correctly and the next one can be processed:
	QCOMPARE(
				imtrest::CConnectionTimeoutBehavior::CreateHttpInternalServerErrorResponse(dateTime),
				QByteArray("HTTP/1.1 500 Internal Server Error\r\n"
							"Date: Sun, 06 Nov 1994 08:49:37 GMT\r\n"
							"Content-Length: 0\r\n"
							"\r\n"));
}


void CConnectionTimeoutBehaviorTest::CreateHttpInternalServerErrorResponseClosingTest()
{
	const QDateTime dateTime = QDateTime::fromSecsSinceEpoch(784111777);

	QCOMPARE(
				imtrest::CConnectionTimeoutBehavior::CreateHttpInternalServerErrorResponse(dateTime, true),
				QByteArray("HTTP/1.1 500 Internal Server Error\r\n"
							"Date: Sun, 06 Nov 1994 08:49:37 GMT\r\n"
							"Connection: close\r\n"
							"Content-Length: 0\r\n"
							"\r\n"));
}


void CConnectionTimeoutBehaviorTest::HttpServerErrorMessageIsCreatedAtCallTest()
{
	const imtrest::CConnectionTimeoutBehavior behavior = imtrest::CConnectionTimeoutBehavior::CreateForProtocol(imtrest::ProtocolTypeIds::s_http);
	QVERIFY(bool(behavior.createServerErrorMessage));

	const QDateTime before = QDateTime::currentDateTimeUtc();
	const QByteArray message = behavior.createServerErrorMessage();
	const QDateTime after = QDateTime::currentDateTimeUtc();

	const QByteArray responseBefore = imtrest::CConnectionTimeoutBehavior::CreateHttpInternalServerErrorResponse(before);
	const QByteArray responseAfter = imtrest::CConnectionTimeoutBehavior::CreateHttpInternalServerErrorResponse(after);

	QVERIFY((message == responseBefore) || (message == responseAfter));
}


void CConnectionTimeoutBehaviorTest::RequestResponseCycleTest()
{
	// Each request of HTTP has a response, so the next request waits for it:
	QVERIFY(imtrest::CConnectionTimeoutBehavior::CreateForProtocol(imtrest::ProtocolTypeIds::s_http).IsRequestResponseCycle());

	// The messages of the other protocols can be one-way or have many responses:
	QVERIFY(!imtrest::CConnectionTimeoutBehavior::CreateForProtocol(imtrest::ProtocolTypeIds::s_tcp).IsRequestResponseCycle());
	QVERIFY(!imtrest::CConnectionTimeoutBehavior::CreateForProtocol(imtrest::ProtocolTypeIds::s_webSocket).IsRequestResponseCycle());
	QVERIFY(!imtrest::CConnectionTimeoutBehavior::CreateForProtocol(QByteArray()).IsRequestResponseCycle());
	QVERIFY(!imtrest::CConnectionTimeoutBehavior().IsRequestResponseCycle());
}


void CConnectionTimeoutBehaviorTest::OtherProtocolBehaviorTest_data()
{
	QTest::addColumn<QByteArray>("protocolTypeId");

	QTest::newRow("tcp") << imtrest::ProtocolTypeIds::s_tcp;
	QTest::newRow("web socket") << imtrest::ProtocolTypeIds::s_webSocket;
	QTest::newRow("ftp") << QByteArray("ftp");
	QTest::newRow("smtp") << QByteArray("smtp");
	QTest::newRow("empty") << QByteArray();
}


void CConnectionTimeoutBehaviorTest::OtherProtocolBehaviorTest()
{
	QFETCH(QByteArray, protocolTypeId);

	const imtrest::CConnectionTimeoutBehavior behavior = imtrest::CConnectionTimeoutBehavior::CreateForProtocol(protocolTypeId);

	// The unknown protocols get the safest behavior: no message, the limit only before the first message:
	QVERIFY(!behavior.createRequestTimeoutMessage);
	QVERIFY(!behavior.createBadRequestMessage);
	QVERIFY(!behavior.createProcessingTimeoutMessage);
	QVERIFY(!behavior.createServerErrorMessage);
	QVERIFY(behavior.isLimitedBeforeFirstMessageOnly);
}


I_ADD_TEST(CConnectionTimeoutBehaviorTest);


