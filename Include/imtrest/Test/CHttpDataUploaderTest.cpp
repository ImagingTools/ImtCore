// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include "CHttpDataUploaderTest.h"


// Qt includes
#include <QtCore/QBuffer>
#include <QtCore/QRegularExpression>
#include <QtCore/QTemporaryFile>
#include <QtNetwork/QTcpServer>
#include <QtNetwork/QTcpSocket>
#include <QtTest/QSignalSpy>

// ImtCore includes
#include <imtrest/CHttpDataUploader.h>


namespace
{


struct CReceivedRequest
{
	QByteArray method;
	QByteArray path;
	QMap<QByteArray, QByteArray> headers;
	QByteArray body;
};


struct CContentRange
{
	bool isValid = false;
	qint64 first = -1;
	qint64 last = -1;
	qint64 total = -1;
};


/**
	Minimal HTTP/1.1 server running in the thread of the test.
	The uploader waits for the reply in a nested event loop, so the server is served while the uploader is waiting.
	Requests are parsed independently of imtrest, so the server is an independent check of the uploader.
*/
class CTestHttpServer
{
public:
	CTestHttpServer()
	{
		QObject::connect(&m_server, &QTcpServer::newConnection, &m_server, [this](){
			while (m_server.hasPendingConnections()){
				QTcpSocket* socketPtr = m_server.nextPendingConnection();

				QObject::connect(socketPtr, &QTcpSocket::readyRead, socketPtr, [this, socketPtr](){
					HandleReadyRead(*socketPtr);
				});
			}
		});
	}

	bool Start()
	{
		return m_server.listen(QHostAddress::LocalHost, 0);
	}

	void Stop()
	{
		m_server.close();
	}

	void SetResponseStatus(int status)
	{
		m_responseStatus = status;
	}

	QUrl GetUrl(const QString& path = QStringLiteral("/upload")) const
	{
		return QUrl(QStringLiteral("http://127.0.0.1:%1%2").arg(m_server.serverPort()).arg(path));
	}

	const QList<CReceivedRequest>& GetRequests() const
	{
		return m_requests;
	}

private:
	void HandleReadyRead(QTcpSocket& socket)
	{
		QByteArray& buffer = m_buffers[&socket];
		buffer += socket.readAll();

		for (;;){
			const qsizetype headEnd = buffer.indexOf("\r\n\r\n");
			if (headEnd < 0){
				return;
			}

			const QList<QByteArray> lines = buffer.left(headEnd).split('\n');
			const QList<QByteArray> requestLine = lines.first().trimmed().split(' ');

			CReceivedRequest request;
			request.method = requestLine.value(0);
			request.path = requestLine.value(1);

			for (qsizetype i = 1; i < lines.size(); ++i){
				const qsizetype colonIndex = lines[i].indexOf(':');
				if (colonIndex > 0){
					request.headers.insert(lines[i].left(colonIndex).trimmed().toLower(), lines[i].mid(colonIndex + 1).trimmed());
				}
			}

			const qsizetype bodyLength = request.headers.value("content-length").toInt();
			const qsizetype messageLength = headEnd + 4 + bodyLength;
			if (buffer.size() < messageLength){
				return;
			}

			request.body = buffer.mid(headEnd + 4, bodyLength);
			buffer.remove(0, messageLength);

			m_requests.append(request);

			socket.write("HTTP/1.1 " + QByteArray::number(m_responseStatus) + " Test\r\nContent-Length: 0\r\n\r\n");
		}
	}

private:
	QTcpServer m_server;
	QHash<QTcpSocket*, QByteArray> m_buffers;
	QList<CReceivedRequest> m_requests;
	int m_responseStatus = 201;
};


/**
	Device that reports that there is more data, but cannot deliver it.
*/
class CUnreadableDevice: public QIODevice
{
public:
	CUnreadableDevice()
	{
		open(QIODevice::ReadOnly);
	}

	bool isSequential() const override
	{
		return false;
	}

	qint64 size() const override
	{
		return 100;
	}

	bool atEnd() const override
	{
		return false;
	}

protected:
	qint64 readData(char* /*data*/, qint64 /*maxSize*/) override
	{
		return -1;
	}

	qint64 writeData(const char* /*data*/, qint64 /*maxSize*/) override
	{
		return -1;
	}
};


QByteArray CreateData(int size)
{
	QByteArray data(size, 0);
	for (int i = 0; i < size; ++i){
		data[i] = char((i * 31 + 7) & 0xff);
	}

	return data;
}


CContentRange ParseContentRange(const QByteArray& value)
{
	static const QRegularExpression regExp(QStringLiteral("^bytes (\\d+)-(\\d+)/(\\d+)$"));

	CContentRange retVal;

	const QRegularExpressionMatch match = regExp.match(QString::fromLatin1(value));
	if (match.hasMatch()){
		retVal.isValid = true;
		retVal.first = match.captured(1).toLongLong();
		retVal.last = match.captured(2).toLongLong();
		retVal.total = match.captured(3).toLongLong();
	}

	return retVal;
}


/**
	Verify that the received requests carry the data in the parts of the given size, with the standard Content-Range
	(RFC 9110, 14.4): the last position is inclusive, so the number of bytes of the range is (last - first + 1).
*/
void VerifyUploadedParts(const QList<CReceivedRequest>& requests, const QByteArray& data, qint64 partSize)
{
	const qint64 expectedCount = (data.size() + partSize - 1) / partSize;

	QCOMPARE(qint64(requests.size()), expectedCount);

	QByteArray assembledData;

	for (qsizetype i = 0; i < requests.size(); ++i){
		const CReceivedRequest& request = requests[i];

		QCOMPARE(request.method, QByteArray("PUT"));
		QCOMPARE(request.path, QByteArray("/upload"));

		const CContentRange range = ParseContentRange(request.headers.value("content-range"));
		QVERIFY2(range.isValid, request.headers.value("content-range").constData());

		const qint64 expectedFirst = qint64(i) * partSize;
		QCOMPARE(range.first, expectedFirst);
		QCOMPARE(range.last, qMin<qint64>(expectedFirst + partSize, data.size()) - 1);
		QCOMPARE(range.total, qint64(data.size()));

		QCOMPARE(range.last - range.first + 1, qint64(request.body.size()));
		QCOMPARE(request.headers.value("content-length").toLongLong(), qint64(request.body.size()));

		assembledData += request.body;
	}

	QCOMPARE(assembledData, data);
}


} // namespace


// private slots

void CHttpDataUploaderTest::DefaultsTest()
{
	imtrest::CHttpDataUploader uploader;

	QCOMPARE(uploader.GetPartMaxSize(), qsizetype(1000000));
	QVERIFY(uploader.GetAdditionalHeaders().isEmpty());
	QVERIFY(uploader.GetUploadUrl().isEmpty());
}


void CHttpDataUploaderTest::PropertiesTest()
{
	imtrest::CHttpDataUploader uploader;

	QSignalSpy partSizeSpy(&uploader, &imtrest::CHttpDataUploader::PartMaxSizeChanged);
	QSignalSpy headersSpy(&uploader, &imtrest::CHttpDataUploader::AdditionalHeadersChanged);
	QSignalSpy urlSpy(&uploader, &imtrest::CHttpDataUploader::uploadUrlChanged);

	uploader.SetPartMaxSize(500);
	QCOMPARE(uploader.GetPartMaxSize(), qsizetype(500));
	QCOMPARE(partSizeSpy.count(), 1);

	// The same value is not a change:
	uploader.SetPartMaxSize(500);
	QCOMPARE(partSizeSpy.count(), 1);

	QVariantMap headers;
	headers.insert(QStringLiteral("X-Session-Id"), QStringLiteral("session"));
	uploader.SetAdditionalHeaders(headers);
	QCOMPARE(uploader.GetAdditionalHeaders(), headers);
	QCOMPARE(headersSpy.count(), 1);

	uploader.SetAdditionalHeaders(headers);
	QCOMPARE(headersSpy.count(), 1);

	const QUrl url(QStringLiteral("http://localhost:7380/upload"));
	uploader.SetUploadUrl(url);
	QCOMPARE(uploader.GetUploadUrl(), url);
	QCOMPARE(urlSpy.count(), 1);

	uploader.SetUploadUrl(url);
	QCOMPARE(urlSpy.count(), 1);

	// The other signals are not affected by a change of a property:
	QCOMPARE(partSizeSpy.count(), 1);
	QCOMPARE(headersSpy.count(), 1);
}


void CHttpDataUploaderTest::UploadMultiplePartsTest()
{
	CTestHttpServer server;
	QVERIFY(server.Start());

	const QByteArray data = CreateData(2500);
	QBuffer buffer;
	buffer.setData(data);
	QVERIFY(buffer.open(QIODevice::ReadOnly));

	imtrest::CHttpDataUploader uploader;
	uploader.SetPartMaxSize(1000);
	uploader.SetDevice(&buffer);

	QVERIFY(uploader.UploadFile(server.GetUrl()));

	VerifyUploadedParts(server.GetRequests(), data, 1000);

	// The ranges of the parts are exactly these:
	QCOMPARE(server.GetRequests().value(0).headers.value("content-range"), QByteArray("bytes 0-999/2500"));
	QCOMPARE(server.GetRequests().value(1).headers.value("content-range"), QByteArray("bytes 1000-1999/2500"));
	QCOMPARE(server.GetRequests().value(2).headers.value("content-range"), QByteArray("bytes 2000-2499/2500"));
}


void CHttpDataUploaderTest::UploadSinglePartTest()
{
	CTestHttpServer server;
	QVERIFY(server.Start());

	const QByteArray data = CreateData(300);
	QBuffer buffer;
	buffer.setData(data);
	QVERIFY(buffer.open(QIODevice::ReadOnly));

	imtrest::CHttpDataUploader uploader;
	uploader.SetPartMaxSize(1000);
	uploader.SetDevice(&buffer);

	QVERIFY(uploader.UploadFile(server.GetUrl()));

	VerifyUploadedParts(server.GetRequests(), data, 1000);
	QCOMPARE(server.GetRequests().value(0).headers.value("content-range"), QByteArray("bytes 0-299/300"));
}


void CHttpDataUploaderTest::UploadExactMultipleOfPartSizeTest()
{
	CTestHttpServer server;
	QVERIFY(server.Start());

	const QByteArray data = CreateData(2000);
	QBuffer buffer;
	buffer.setData(data);
	QVERIFY(buffer.open(QIODevice::ReadOnly));

	imtrest::CHttpDataUploader uploader;
	uploader.SetPartMaxSize(1000);
	uploader.SetDevice(&buffer);

	QVERIFY(uploader.UploadFile(server.GetUrl()));

	// No empty part at the end:
	VerifyUploadedParts(server.GetRequests(), data, 1000);
}


void CHttpDataUploaderTest::UploadPartSizeOneTest()
{
	CTestHttpServer server;
	QVERIFY(server.Start());

	const QByteArray data = CreateData(5);
	QBuffer buffer;
	buffer.setData(data);
	QVERIFY(buffer.open(QIODevice::ReadOnly));

	imtrest::CHttpDataUploader uploader;
	uploader.SetPartMaxSize(1);
	uploader.SetDevice(&buffer);

	QVERIFY(uploader.UploadFile(server.GetUrl()));

	VerifyUploadedParts(server.GetRequests(), data, 1);
}


void CHttpDataUploaderTest::UploadEmptyDeviceTest()
{
	CTestHttpServer server;
	QVERIFY(server.Start());

	QBuffer buffer;
	QVERIFY(buffer.open(QIODevice::ReadOnly));

	imtrest::CHttpDataUploader uploader;
	uploader.SetDevice(&buffer);

	// There is nothing to send:
	QVERIFY(uploader.UploadFile(server.GetUrl()));
	QVERIFY(server.GetRequests().isEmpty());
}


void CHttpDataUploaderTest::InitializeDeviceFromFileTest()
{
	CTestHttpServer server;
	QVERIFY(server.Start());

	const QByteArray data = CreateData(1500);

	QTemporaryFile file;
	QVERIFY(file.open());
	QCOMPARE(file.write(data), qint64(data.size()));
	file.close();

	imtrest::CHttpDataUploader uploader;
	uploader.SetPartMaxSize(1000);

	QVERIFY(uploader.InitializeDeviceFromFile(file.fileName()));
	QVERIFY(uploader.UploadFile(server.GetUrl()));

	VerifyUploadedParts(server.GetRequests(), data, 1000);
}


void CHttpDataUploaderTest::InitializeDeviceFromMissingFileTest()
{
	QTemporaryFile file;
	QVERIFY(file.open());
	const QString missingFilePath = file.fileName() + QStringLiteral(".missing");

	imtrest::CHttpDataUploader uploader;

	QVERIFY(!uploader.InitializeDeviceFromFile(missingFilePath));
}


void CHttpDataUploaderTest::SetDeviceReplacesFileDeviceTest()
{
	CTestHttpServer server;
	QVERIFY(server.Start());

	QTemporaryFile file;
	QVERIFY(file.open());
	QCOMPARE(file.write(CreateData(10)), qint64(10));
	file.close();

	const QByteArray data = CreateData(400);
	QBuffer buffer;
	buffer.setData(data);
	QVERIFY(buffer.open(QIODevice::ReadOnly));

	imtrest::CHttpDataUploader uploader;
	uploader.SetPartMaxSize(1000);

	QVERIFY(uploader.InitializeDeviceFromFile(file.fileName()));
	uploader.SetDevice(&buffer);

	QVERIFY(uploader.UploadFile(server.GetUrl()));

	VerifyUploadedParts(server.GetRequests(), data, 1000);
}


void CHttpDataUploaderTest::UploadAdditionalHeadersTest()
{
	CTestHttpServer server;
	QVERIFY(server.Start());

	const QByteArray data = CreateData(2500);
	QBuffer buffer;
	buffer.setData(data);
	QVERIFY(buffer.open(QIODevice::ReadOnly));

	imtrest::CHttpDataUploader uploader;
	uploader.SetPartMaxSize(1000);
	uploader.SetDevice(&buffer);
	QVariantMap headers;
	headers.insert(QStringLiteral("X-Session-Id"), QStringLiteral("session-1"));
	headers.insert(QStringLiteral("X-File-Id"), QStringLiteral("file-1"));
	headers.insert(QStringLiteral("X-File-Name"), QStringLiteral("name.bin"));
	uploader.SetAdditionalHeaders(headers);

	QVERIFY(uploader.UploadFile(server.GetUrl()));

	QCOMPARE(server.GetRequests().size(), 3);

	// Each part carries the headers:
	for (const CReceivedRequest& request : server.GetRequests()){
		QCOMPARE(request.headers.value("x-session-id"), QByteArray("session-1"));
		QCOMPARE(request.headers.value("x-file-id"), QByteArray("file-1"));
		QCOMPARE(request.headers.value("x-file-name"), QByteArray("name.bin"));
	}

	VerifyUploadedParts(server.GetRequests(), data, 1000);
}


void CHttpDataUploaderTest::UploadUsesDefaultUrlTest()
{
	CTestHttpServer server;
	QVERIFY(server.Start());

	const QByteArray data = CreateData(100);
	QBuffer buffer;
	buffer.setData(data);
	QVERIFY(buffer.open(QIODevice::ReadOnly));

	imtrest::CHttpDataUploader uploader;
	uploader.SetDevice(&buffer);
	uploader.SetUploadUrl(server.GetUrl());

	QVERIFY(uploader.UploadFile());

	VerifyUploadedParts(server.GetRequests(), data, 1000000);
}


void CHttpDataUploaderTest::UploadExplicitUrlOverridesDefaultUrlTest()
{
	CTestHttpServer server;
	QVERIFY(server.Start());

	CTestHttpServer otherServer;
	QVERIFY(otherServer.Start());

	const QByteArray data = CreateData(100);
	QBuffer buffer;
	buffer.setData(data);
	QVERIFY(buffer.open(QIODevice::ReadOnly));

	imtrest::CHttpDataUploader uploader;
	uploader.SetDevice(&buffer);
	uploader.SetUploadUrl(otherServer.GetUrl());

	QVERIFY(uploader.UploadFile(server.GetUrl()));

	VerifyUploadedParts(server.GetRequests(), data, 1000000);
	QVERIFY(otherServer.GetRequests().isEmpty());
}


void CHttpDataUploaderTest::UploadResponseStatusTest_data()
{
	QTest::addColumn<int>("status");
	QTest::addColumn<bool>("isUploaded");

	QTest::newRow("200 OK") << 200 << true;
	QTest::newRow("201 Created") << 201 << true;
	QTest::newRow("204 No Content") << 204 << true;
	QTest::newRow("206 Partial Content") << 206 << true;
	QTest::newRow("400 Bad Request") << 400 << false;
	QTest::newRow("401 Unauthorized") << 401 << false;
	QTest::newRow("403 Forbidden") << 403 << false;
	QTest::newRow("413 Payload Too Large") << 413 << false;
	QTest::newRow("500 Internal Server Error") << 500 << false;
	QTest::newRow("503 Service Unavailable") << 503 << false;
}


void CHttpDataUploaderTest::UploadResponseStatusTest()
{
	QFETCH(int, status);
	QFETCH(bool, isUploaded);

	CTestHttpServer server;
	QVERIFY(server.Start());
	server.SetResponseStatus(status);

	const QByteArray data = CreateData(100);
	QBuffer buffer;
	buffer.setData(data);
	QVERIFY(buffer.open(QIODevice::ReadOnly));

	imtrest::CHttpDataUploader uploader;
	uploader.SetDevice(&buffer);

	QCOMPARE(uploader.UploadFile(server.GetUrl()), isUploaded);
	QCOMPARE(server.GetRequests().size(), 1);
}


void CHttpDataUploaderTest::UploadStopsAtFirstFailedPartTest()
{
	CTestHttpServer server;
	QVERIFY(server.Start());
	server.SetResponseStatus(500);

	const QByteArray data = CreateData(2500);
	QBuffer buffer;
	buffer.setData(data);
	QVERIFY(buffer.open(QIODevice::ReadOnly));

	imtrest::CHttpDataUploader uploader;
	uploader.SetPartMaxSize(1000);
	uploader.SetDevice(&buffer);

	QVERIFY(!uploader.UploadFile(server.GetUrl()));

	// The remaining parts are not sent:
	QCOMPARE(server.GetRequests().size(), 1);
}


void CHttpDataUploaderTest::UploadConnectionRefusedTest()
{
	QUrl url;
	{
		CTestHttpServer server;
		QVERIFY(server.Start());
		url = server.GetUrl();
		server.Stop();
	}

	const QByteArray data = CreateData(100);
	QBuffer buffer;
	buffer.setData(data);
	QVERIFY(buffer.open(QIODevice::ReadOnly));

	imtrest::CHttpDataUploader uploader;
	uploader.SetDevice(&buffer);

	QVERIFY(!uploader.UploadFile(url));
}


void CHttpDataUploaderTest::UploadInvalidPartSizeTest_data()
{
	QTest::addColumn<qsizetype>("partSize");

	QTest::newRow("zero") << qsizetype(0);
	QTest::newRow("negative") << qsizetype(-5);
}


void CHttpDataUploaderTest::UploadInvalidPartSizeTest()
{
	QFETCH(qsizetype, partSize);

	CTestHttpServer server;
	QVERIFY(server.Start());

	const QByteArray data = CreateData(100);
	QBuffer buffer;
	buffer.setData(data);
	QVERIFY(buffer.open(QIODevice::ReadOnly));

	imtrest::CHttpDataUploader uploader;
	uploader.SetPartMaxSize(partSize);
	uploader.SetDevice(&buffer);

	// Such a part size used to repeat sending of empty parts forever:
	QVERIFY(!uploader.UploadFile(server.GetUrl()));
	QVERIFY(server.GetRequests().isEmpty());
}


void CHttpDataUploaderTest::UploadUnreadableDeviceTest()
{
	CTestHttpServer server;
	QVERIFY(server.Start());

	CUnreadableDevice device;

	imtrest::CHttpDataUploader uploader;
	uploader.SetDevice(&device);

	// A device that does not deliver data used to repeat sending of empty parts forever:
	QVERIFY(!uploader.UploadFile(server.GetUrl()));
	QVERIFY(server.GetRequests().isEmpty());
}


I_ADD_TEST(CHttpDataUploaderTest);


