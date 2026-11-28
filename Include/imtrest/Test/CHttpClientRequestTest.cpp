// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include "CHttpClientRequestTest.h"


// Qt includes
#include <QtCore/QElapsedTimer>
#include <QtCore/QTimer>
#include <QtNetwork/QTcpServer>
#include <QtNetwork/QTcpSocket>

// ImtCore includes
#include <imtrest/CHttpClientRequest.h>
#include <imtrest/CHttpClientRequestSender.h>
#include <imtrest/HttpHeaderIds.h>


namespace
{


QByteArray CreateResponse(int status, const QByteArray& reason, const QByteArray& body, const QByteArray& extraHeaders = QByteArray())
{
	return
				"HTTP/1.1 " + QByteArray::number(status) + " " + reason + "\r\n"
				"Content-Type: application/json\r\n"
				"Content-Length: " + QByteArray::number(body.size()) + "\r\n" +
				extraHeaders +
				"Connection: close\r\n\r\n" + body;
}


bool IsCompleteRequest(const QByteArray& data)
{
	int headerEnd = data.indexOf("\r\n\r\n");
	if (headerEnd < 0){
		return false;
	}

	int lengthPos = data.toLower().indexOf("content-length:");
	if ((lengthPos < 0) || (lengthPos > headerEnd)){
		return true;
	}

	int lineEnd = data.indexOf("\r\n", lengthPos);
	int length = data.mid(lengthPos + 15, lineEnd - lengthPos - 15).trimmed().toInt();

	return data.size() >= headerEnd + 4 + length;
}


} // namespace


void CHttpClientRequestTest::init()
{
	m_serverPtr = new QTcpServer(this);
	m_receivedRequest.clear();
	m_shouldAnswer = true;
}


void CHttpClientRequestTest::cleanup()
{
	delete m_serverPtr;
	m_serverPtr = nullptr;
}


// Data model

void CHttpClientRequestTest::DefaultsTest()
{
	imtrest::CHttpClientRequest request;
	QCOMPARE(request.GetState(), int(imtrest::IHttpClientRequest::RS_CREATED));
	QCOMPARE(request.GetRequestType(), QNetworkAccessManager::GetOperation);
	QCOMPARE(request.GetErrorType(), QNetworkReply::NoError);
	QVERIFY(request.GetReply() == nullptr);
	QVERIFY(request.GetReplyBody().isEmpty());
	QVERIFY(!request.GetAttribute(QNetworkRequest::HttpStatusCodeAttribute).isValid());
}


void CHttpClientRequestTest::ReadyStateTest()
{
	imtrest::CHttpClientRequest request;
	request.SetRequest(QNetworkRequest(QUrl("https://example.com/api")));
	QCOMPARE(request.GetState(), int(imtrest::IHttpClientRequest::RS_READY_TO_PROCESSING));
	QCOMPARE(request.GetUrl(), QUrl("https://example.com/api"));
}


void CHttpClientRequestTest::CopyAndResetTest()
{
	imtrest::CHttpClientRequest request;
	request.SetRequest(QNetworkRequest(QUrl("https://example.com/api")));
	request.SetRequestType(QNetworkAccessManager::PostOperation);
	request.SetRequestBody("{\"a\":1}");

	imtrest::CHttpClientRequest copy;
	QVERIFY(copy.CopyFrom(request));
	QCOMPARE(copy.GetUrl(), request.GetUrl());
	QCOMPARE(copy.GetRequestType(), QNetworkAccessManager::PostOperation);
	QCOMPARE(copy.GetRequestBody(), QByteArray("{\"a\":1}"));

	QVERIFY(copy.ResetData());
	QCOMPARE(copy.GetState(), int(imtrest::IHttpClientRequest::RS_CREATED));
	QVERIFY(copy.GetRequestBody().isEmpty());
	QCOMPARE(copy.GetRequestType(), QNetworkAccessManager::GetOperation);
}


// Sending

void CHttpClientRequestTest::PostReplyTest()
{
	StartServer(CreateResponse(200, "OK", "{\"ok\":true}", "X-Request-Id: r-1\r\n"));

	QNetworkRequest networkRequest(GetServerUrl("/v1/chat"));
	networkRequest.setRawHeader(imtrest::HttpHeaderIds::s_contentType, "application/json");
	networkRequest.setRawHeader(imtrest::HttpHeaderIds::s_authorization, "Bearer token");

	imtrest::CHttpClientRequest request;
	request.SetRequest(networkRequest);
	request.SetRequestType(QNetworkAccessManager::PostOperation);
	request.SetRequestBody("{\"model\":\"m\"}");

	QCOMPARE(imtrest::CHttpClientRequestSender::Send(request, 5000), imtrest::CHttpClientRequestSender::SR_FINISHED);
	QCOMPARE(request.GetAttribute(QNetworkRequest::HttpStatusCodeAttribute).toInt(), 200);
	QCOMPARE(request.GetReplyBody(), QByteArray("{\"ok\":true}"));
	QCOMPARE(request.GetHeader("x-request-id"), QByteArray("r-1"));
	QCOMPARE(request.GetState(), int(imtrest::IHttpClientRequest::RS_FINISHED | imtrest::IHttpClientRequest::RS_NO_ERROR));

	QVERIFY(m_receivedRequest.startsWith("POST /v1/chat HTTP/1.1"));
	QVERIFY(m_receivedRequest.contains("Bearer token"));
	QVERIFY(m_receivedRequest.endsWith("{\"model\":\"m\"}"));
}


void CHttpClientRequestTest::GetReplyTest()
{
	StartServer(CreateResponse(200, "OK", "[]"));

	imtrest::CHttpClientRequest request;
	request.SetRequest(QNetworkRequest(GetServerUrl("/items")));

	QCOMPARE(imtrest::CHttpClientRequestSender::Send(request, 5000), imtrest::CHttpClientRequestSender::SR_FINISHED);
	QCOMPARE(request.GetReplyBody(), QByteArray("[]"));
	QVERIFY(m_receivedRequest.startsWith("GET /items HTTP/1.1"));
}


void CHttpClientRequestTest::HttpErrorStatusTest()
{
	StartServer(CreateResponse(401, "Unauthorized", "{\"error\":{\"type\":\"auth\"}}"));

	QNetworkRequest networkRequest(GetServerUrl("/v1/chat"));
	networkRequest.setRawHeader(imtrest::HttpHeaderIds::s_contentType, "application/json");

	imtrest::CHttpClientRequest request;
	request.SetRequest(networkRequest);
	request.SetRequestType(QNetworkAccessManager::PostOperation);
	request.SetRequestBody("{}");

	QCOMPARE(imtrest::CHttpClientRequestSender::Send(request, 5000), imtrest::CHttpClientRequestSender::SR_FINISHED);
	QCOMPARE(request.GetAttribute(QNetworkRequest::HttpStatusCodeAttribute).toInt(), 401);
	QCOMPARE(request.GetReplyBody(), QByteArray("{\"error\":{\"type\":\"auth\"}}"));
	QVERIFY(request.GetErrorType() != QNetworkReply::NoError);
	QVERIFY(request.GetState() & imtrest::IHttpClientRequest::RS_ERROR);
}


void CHttpClientRequestTest::TimeoutTest()
{
	StartServer(QByteArray(), false);

	QNetworkRequest networkRequest(GetServerUrl("/slow"));
	networkRequest.setRawHeader(imtrest::HttpHeaderIds::s_contentType, "application/json");

	imtrest::CHttpClientRequest request;
	request.SetRequest(networkRequest);
	request.SetRequestType(QNetworkAccessManager::PostOperation);
	request.SetRequestBody("{}");

	QElapsedTimer timer;
	timer.start();
	QCOMPARE(imtrest::CHttpClientRequestSender::Send(request, 300), imtrest::CHttpClientRequestSender::SR_TIMEOUT);
	QVERIFY(timer.elapsed() < 3000);
	QCOMPARE(request.GetErrorType(), QNetworkReply::TimeoutError);
	QCOMPARE(request.GetState(), int(imtrest::IHttpClientRequest::RS_FINISHED | imtrest::IHttpClientRequest::RS_ERROR));
}


void CHttpClientRequestTest::CancelTest()
{
	StartServer(QByteArray(), false);

	imtrest::CHttpClientRequest request;
	request.SetRequest(QNetworkRequest(GetServerUrl("/slow")));

	bool isCanceled = false;
	QTimer::singleShot(200, [&isCanceled](){ isCanceled = true; });

	QElapsedTimer timer;
	timer.start();
	QCOMPARE(imtrest::CHttpClientRequestSender::Send(request, 10000, [&isCanceled](){ return isCanceled; }), imtrest::CHttpClientRequestSender::SR_CANCELED);
	QVERIFY(timer.elapsed() < 3000);
	QCOMPARE(request.GetErrorType(), QNetworkReply::OperationCanceledError);

	// A request cancelled before it starts is not sent.
	m_receivedRequest.clear();
	QCOMPARE(imtrest::CHttpClientRequestSender::Send(request, 10000, [](){ return true; }), imtrest::CHttpClientRequestSender::SR_CANCELED);
	QTest::qWait(100);
	QVERIFY(m_receivedRequest.isEmpty());
}


void CHttpClientRequestTest::TooLargeReplyTest()
{
	StartServer(CreateResponse(200, "OK", QByteArray(4096, 'x')));

	imtrest::CHttpClientRequest request;
	request.SetRequest(QNetworkRequest(GetServerUrl("/large")));

	QCOMPARE(imtrest::CHttpClientRequestSender::Send(request, 5000, imtrest::CHttpClientRequestSender::CancellationCheck(), 1024), imtrest::CHttpClientRequestSender::SR_TOO_LARGE);
	QVERIFY(request.GetReplyBody().isEmpty());
	QVERIFY(request.GetState() & imtrest::IHttpClientRequest::RS_ERROR);
}


void CHttpClientRequestTest::InvalidUrlTest()
{
	imtrest::CHttpClientRequest request;
	QCOMPARE(imtrest::CHttpClientRequestSender::Send(request, 1000), imtrest::CHttpClientRequestSender::SR_INVALID_REQUEST);
	QVERIFY(request.GetState() & imtrest::IHttpClientRequest::RS_ERROR);
}


void CHttpClientRequestTest::ConnectionRefusedTest()
{
	QVERIFY(m_serverPtr->listen(QHostAddress::LocalHost));
	quint16 port = m_serverPtr->serverPort();
	m_serverPtr->close();

	imtrest::CHttpClientRequest request;
	request.SetRequest(QNetworkRequest(QUrl(QStringLiteral("http://127.0.0.1:%1/").arg(port))));

	// Windows retries a refused connection to the local machine for some seconds.
	QCOMPARE(imtrest::CHttpClientRequestSender::Send(request, 20000), imtrest::CHttpClientRequestSender::SR_FINISHED);
	QVERIFY(!request.GetAttribute(QNetworkRequest::HttpStatusCodeAttribute).isValid());
	QCOMPARE(request.GetErrorType(), QNetworkReply::ConnectionRefusedError);
}


// private methods

void CHttpClientRequestTest::StartServer(const QByteArray& response, bool shouldAnswer)
{
	m_response = response;
	m_shouldAnswer = shouldAnswer;

	QVERIFY(m_serverPtr->listen(QHostAddress::LocalHost));

	connect(m_serverPtr, &QTcpServer::newConnection, this, [this](){
		QTcpSocket* socketPtr = m_serverPtr->nextPendingConnection();
		connect(socketPtr, &QTcpSocket::readyRead, this, [this, socketPtr](){
			m_receivedRequest += socketPtr->readAll();
			if (m_shouldAnswer && IsCompleteRequest(m_receivedRequest)){
				socketPtr->write(m_response);
				socketPtr->disconnectFromHost();
			}
		});
	});
}


QUrl CHttpClientRequestTest::GetServerUrl(const QString& path) const
{
	return QUrl(QStringLiteral("http://127.0.0.1:%1%2").arg(m_serverPtr->serverPort()).arg(path));
}


I_ADD_TEST(CHttpClientRequestTest);


