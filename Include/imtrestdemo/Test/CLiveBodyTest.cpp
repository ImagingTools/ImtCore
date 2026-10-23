// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include "CLiveBodyTest.h"


// Qt includes
#include <QtCore/QCryptographicHash>
#include <QtCore/QDir>
#include <QtCore/QFile>
#include <QtCore/QJsonArray>

// ACF includes
#include <itest/CStandardTestExecutor.h>

// ImtCore includes
#include <imtrestdemo/CDemoProtocol.h>


namespace
{


QByteArray CalculateSha256(const QByteArray& data)
{
	return QCryptographicHash::hash(data, QCryptographicHash::Sha256).toHex();
}


QByteArray ReadFile(const QString& path)
{
	QFile file(path);

	return file.open(QIODevice::ReadOnly) ? file.readAll() : QByteArray();
}


// Send the body by parts like a client that reads a big file
bool SendBodyInParts(CRawHttpClient& client, const QByteArray& body, int partSize)
{
	for (qsizetype offset = 0; offset < body.size(); offset += partSize){
		if (!client.Send(body.mid(offset, partSize))){
			return false;
		}
	}

	return true;
}


}


void CLiveBodyTest::ExpectContinueTest()
{
	const QByteArray body = imtrestdemo::DemoProtocol::CreatePatternData(0, 1000);

	CRawHttpClient client;
	QVERIFY(Connect(client));

	// The head says that the client waits for the permission before it sends the body
	QVERIFY(client.Send(CRawHttpClient::CreateRequest("PUT", "/upload/expect.bin", WithToken({{"Content-Length", "1000"}, {"Expect", "100-continue"}}))));

	int status = 0;
	if (!client.ReadInterimResponse(status)){
		Response unexpected;
		client.ReadResponse(unexpected, false, 1000);

		QFAIL(qPrintable(QString("There is no 100, the response is %1: %2").arg(unexpected.statusCode).arg(QString::fromLatin1(unexpected.rawHead))));
	}

	QCOMPARE(status, 100);

	QVERIFY(client.Send(body));

	Response response;
	QVERIFY(client.ReadResponse(response));
	QCOMPARE(response.statusCode, 201);
	QCOMPARE(ParseJson(response.body).value("sha256").toString().toLatin1(), CalculateSha256(body));
}


void CLiveBodyTest::ExpectContinueRefusedTest()
{
	CRawHttpClient client;
	QVERIFY(Connect(client));

	// No token: the server refuses before the client has sent the body, so the body is not sent at all (and the 100 is not sent)
	QVERIFY(client.Send(CRawHttpClient::CreateRequest("PUT", "/upload/refused.bin", {{"Content-Length", "10000000"}, {"Expect", "100-continue"}})));

	Response response;
	QVERIFY(client.ReadResponse(response));
	QCOMPARE(response.statusCode, 401);
	QVERIFY(response.interimStatuses.isEmpty());
	QVERIFY(response.HasHeader("www-authenticate"));
	QVERIFY(client.WaitForClose(8000));

	QVERIFY(!QFile::exists(QDir(GetUploadDirectory()).absoluteFilePath("refused.bin")));
}


void CLiveBodyTest::ExpectUnknownTest()
{
	Response response;
	QVERIFY(Request("POST", "/echo", response, {{"Expect", "unknown-expectation"}}, "body"));

	// RFC 9110, section 10.1.1: an expectation that the server cannot meet is 417
	QCOMPARE(response.statusCode, 417);
}


void CLiveBodyTest::BodyLimitBeforeBodyTest()
{
	CRawHttpClient client;
	QVERIFY(Connect(client));

	// The head announces a body that is over the limit of /echo: the answer comes without the body
	QVERIFY(client.Send(CRawHttpClient::CreateRequest("POST", "/echo", {{"Content-Length", "100000"}})));

	Response response;
	QVERIFY(client.ReadResponse(response, false, 3000));
	QCOMPARE(response.statusCode, 413);
	QVERIFY(client.WaitForClose(8000));

	QVERIFY(IsServerAlive());
}


void CLiveBodyTest::BodyLimitChunkedTest()
{
	CRawHttpClient client;
	QVERIFY(Connect(client));

	// The size is not known from the head, so the limit works while the body is received
	QVERIFY(client.Send(CRawHttpClient::CreateRequest("POST", "/echo", {{"Transfer-Encoding", "chunked"}})));

	const QByteArray part = imtrestdemo::DemoProtocol::CreatePatternData(0, 1000);
	for (int i = 0; i < 6; ++i){
		if (!client.Send(CRawHttpClient::CreateChunk(part))){
			break;
		}
	}

	Response response;
	QVERIFY(client.ReadResponse(response, false, 5000));
	QCOMPARE(response.statusCode, 413);

	QVERIFY(IsServerAlive());
}


void CLiveBodyTest::BodyWithinLimitTest()
{
	const QByteArray body = imtrestdemo::DemoProtocol::CreatePatternData(5, 4000);

	Response response;
	QVERIFY(Request("POST", "/echo", response, Headers(), body));
	QCOMPARE(response.statusCode, 200);

	const QJsonObject echo = ParseJson(response.body);
	QCOMPARE(echo.value("bodySize").toInt(), 4000);
	QCOMPARE(echo.value("bodySha256").toString().toLatin1(), CalculateSha256(body));
}


void CLiveBodyTest::RefusedPathTest()
{
	CRawHttpClient client;
	QVERIFY(Connect(client));

	QVERIFY(client.Send(CRawHttpClient::CreateRequest("POST", "/refuse", {{"Content-Length", "10"}})));

	Response response;
	QVERIFY(client.ReadResponse(response, false, 3000));
	QCOMPARE(response.statusCode, 403);

	Response get;
	QVERIFY(Get("/refuse", get));
	QCOMPARE(get.statusCode, 403);
}


void CLiveBodyTest::UploadTest()
{
	const QByteArray body = imtrestdemo::DemoProtocol::CreatePatternData(0, 8 * 1024 * 1024 + 17);

	CRawHttpClient client;
	QVERIFY(Connect(client));
	QVERIFY(client.Send(CRawHttpClient::CreateRequest("PUT", "/upload/big.bin", WithToken({{"Content-Length", QByteArray::number(body.size())}}))));
	QVERIFY(SendBodyInParts(client, body, 64 * 1024));

	Response response;
	QVERIFY(client.ReadResponse(response));
	QCOMPARE(response.statusCode, 201);
	QCOMPARE(response.GetHeader("location"), QByteArray("/files/uploaded/big.bin"));

	const QJsonObject result = ParseJson(response.body);
	QCOMPARE(result.value("name").toString(), QString("big.bin"));
	QCOMPARE(qint64(result.value("size").toDouble()), qint64(body.size()));
	QCOMPARE(result.value("sha256").toString().toLatin1(), CalculateSha256(body));

	// The file is on the disk, byte by byte the same
	QVERIFY(ReadFile(QDir(GetUploadDirectory()).absoluteFilePath("big.bin")) == body);

	// And it can be downloaded back
	QVERIFY(client.Send(CRawHttpClient::CreateRequest("GET", "/files/uploaded/big.bin")));

	Response download;
	QVERIFY(client.ReadResponse(download));
	QCOMPARE(download.statusCode, 200);
	QVERIFY(download.body == body);

	QVERIFY(client.Send(CRawHttpClient::CreateRequest("DELETE", "/upload/big.bin", WithToken())));

	Response removed;
	QVERIFY(client.ReadResponse(removed));
	QCOMPARE(removed.statusCode, 204);
	QVERIFY(!QFile::exists(QDir(GetUploadDirectory()).absoluteFilePath("big.bin")));

	QVERIFY(client.Send(CRawHttpClient::CreateRequest("DELETE", "/upload/big.bin", WithToken())));

	Response missing;
	QVERIFY(client.ReadResponse(missing));
	QCOMPARE(missing.statusCode, 404);
}


void CLiveBodyTest::UploadChunkedTest()
{
	const QByteArray body = imtrestdemo::DemoProtocol::CreatePatternData(100, 3 * 1024 * 1024 + 5);

	CRawHttpClient client;
	QVERIFY(Connect(client));
	QVERIFY(client.Send(CRawHttpClient::CreateRequest("PUT", "/upload/chunked.bin", WithToken({{"Transfer-Encoding", "chunked"}}))));

	// The chunks have different sizes
	qsizetype offset = 0;
	int chunkSize = 1;
	while (offset < body.size()){
		const QByteArray chunk = body.mid(offset, chunkSize);
		QVERIFY(client.Send(CRawHttpClient::CreateChunk(chunk)));

		offset += chunk.size();
		chunkSize = qMin(chunkSize * 3 + 1, 200000);
	}

	QVERIFY(client.Send(CRawHttpClient::CreateLastChunk()));

	Response response;
	QVERIFY(client.ReadResponse(response));
	QCOMPARE(response.statusCode, 201);
	QCOMPARE(ParseJson(response.body).value("sha256").toString().toLatin1(), CalculateSha256(body));
	QVERIFY(ReadFile(QDir(GetUploadDirectory()).absoluteFilePath("chunked.bin")) == body);
}


void CLiveBodyTest::UploadReplacesFileTest()
{
	const QString path = QDir(GetUploadDirectory()).absoluteFilePath("replace.bin");

	Response first;
	QVERIFY(Request("PUT", "/upload/replace.bin", first, WithToken(), "the first content"));
	QCOMPARE(first.statusCode, 201);
	QCOMPARE(ReadFile(path), QByteArray("the first content"));

	Response second;
	QVERIFY(Request("PUT", "/upload/replace.bin", second, WithToken(), "the second"));
	QCOMPARE(second.statusCode, 201);
	QCOMPARE(ReadFile(path), QByteArray("the second"));
}


void CLiveBodyTest::UploadEmptyBodyTest()
{
	Response response;
	QVERIFY(Request("PUT", "/upload/empty.bin", response, WithToken({{"Content-Length", "0"}})));
	QCOMPARE(response.statusCode, 201);

	const QJsonObject result = ParseJson(response.body);
	QCOMPARE(result.value("size").toInt(), 0);
	QCOMPARE(result.value("sha256").toString(), QString("e3b0c44298fc1c149afbf4c8996fb92427ae41e4649b934ca495991b7852b855"));
	QVERIFY(QFile::exists(QDir(GetUploadDirectory()).absoluteFilePath("empty.bin")));
}


void CLiveBodyTest::UploadWithoutTokenTest()
{
	Response noToken;
	QVERIFY(Request("PUT", "/upload/notoken.bin", noToken, Headers(), "data"));
	QCOMPARE(noToken.statusCode, 401);

	Response wrongToken;
	QVERIFY(Request("PUT", "/upload/notoken.bin", wrongToken, {{"Authorization", "Bearer wrong"}}, "data"));
	QCOMPARE(wrongToken.statusCode, 401);

	QVERIFY(!QFile::exists(QDir(GetUploadDirectory()).absoluteFilePath("notoken.bin")));
}


void CLiveBodyTest::UploadBadNameTest()
{
	Response hidden;
	QVERIFY(Request("PUT", "/upload/.hidden", hidden, WithToken(), "data"));
	QCOMPARE(hidden.statusCode, 400);
	QVERIFY(!QFile::exists(QDir(GetUploadDirectory()).absoluteFilePath(".hidden")));
}


void CLiveBodyTest::UploadAbortedTest()
{
	{
		CRawHttpClient client;
		QVERIFY(Connect(client));

		// The client announces 5 MiB, sends 1 MiB and breaks the connection
		QVERIFY(client.Send(CRawHttpClient::CreateRequest("PUT", "/upload/aborted.bin", WithToken({{"Content-Length", "5242880"}}))));
		QVERIFY(SendBodyInParts(client, imtrestdemo::DemoProtocol::CreatePatternData(0, 1024 * 1024), 64 * 1024));
	}

	QTest::qSleep(700);

	// The incomplete file must not appear, and no temporary file stays on the disk
	const QStringList files = QDir(GetUploadDirectory()).entryList(QDir::Files | QDir::Hidden, QDir::Name);
	for (const QString& name : files){
		QVERIFY2(!name.startsWith("aborted"), qPrintable(name));
	}

	QVERIFY(IsServerAlive());
}


void CLiveBodyTest::UploadListTest()
{
	Response noToken;
	QVERIFY(Get("/upload", noToken));
	QCOMPARE(noToken.statusCode, 401);

	Response put;
	QVERIFY(Request("PUT", "/upload/listed.txt", put, WithToken(), "12345"));
	QCOMPARE(put.statusCode, 201);

	Response list;
	QVERIFY(Get("/upload", list, WithToken()));
	QCOMPARE(list.statusCode, 200);

	bool isFound = false;
	const QJsonArray files = ParseJson(list.body).value("files").toArray();
	for (const QJsonValue& file : files){
		if (file.toObject().value("name").toString() == "listed.txt"){
			isFound = true;

			QCOMPARE(file.toObject().value("size").toInt(), 5);
		}
	}

	QVERIFY(isFound);
}


I_ADD_TEST(CLiveBodyTest);
