// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include "CLiveDynamicBodyTest.h"


// STL includes
#include <memory>

// Qt includes
#include <QtCore/QCryptographicHash>
#include <QtCore/QDir>
#include <QtCore/QElapsedTimer>
#include <QtCore/QFileInfo>
#include <QtCore/QJsonDocument>

// ACF includes
#include <itest/CStandardTestExecutor.h>

// ImtCore includes
#include <imtrestdemo/CDemoProtocol.h>


namespace
{


const qint64 s_memoryBodyLimit = imtrestdemo::DemoProtocol::s_dynamicMemoryBodyLimit;
const qint64 s_maxBodySize = imtrestdemo::DemoProtocol::s_dynamicMaxBodySize;
const qint64 s_mebibyte = qint64(1024) * 1024;

// Beginning of the names of the files of the bodies (see imtrest::CHttpTemporaryFileBodySink), the other files of a temporary directory are not counted
const QLatin1String s_bodyFilePrefix("temp-file-body-");

// Name of the directory (inside of the directory of the files) that the server of the test gets as its temporary directory
const char* const s_temporaryDirectoryName = "private/received";


QByteArray CalculateSha256(const QByteArray& data)
{
	return QCryptographicHash::hash(data, QCryptographicHash::Sha256).toHex();
}


/**
	Check the answer of \c /dynamic: the place where the body was received, its size and the hash.
	\return Empty string if the answer is right, the description of the mistake otherwise.
*/
QString CheckAnswer(const CRawHttpClient::Response& response, const QByteArray& body, const QString& storage)
{
	if (response.statusCode != 200){
		return QStringLiteral("The status is %1: %2").arg(response.statusCode).arg(QString::fromLatin1(response.body.left(200)));
	}

	const QJsonObject result = QJsonDocument::fromJson(response.body).object();

	if (result.value("storage").toString() != storage){
		return QStringLiteral("The body is received to '%1', expected '%2'").arg(result.value("storage").toString(), storage);
	}

	if (qint64(result.value("size").toDouble()) != body.size()){
		return QStringLiteral("The size of the body is %1, expected %2").arg(qint64(result.value("size").toDouble())).arg(body.size());
	}

	if (result.value("sha256").toString().toLatin1() != CalculateSha256(body)){
		return QStringLiteral("The hash of the body is wrong");
	}

	// A file of a body has the name that the temporary files of imtrest have
	const QString fileName = result.value("fileName").toString();
	if (storage == QLatin1String("file") && !fileName.startsWith(s_bodyFilePrefix)){
		return QStringLiteral("The name of the file is '%1'").arg(fileName);
	}

	if (storage == QLatin1String("memory") && result.contains("fileName")){
		return QStringLiteral("The body is in the memory, but the answer has a file");
	}

	return QString();
}


QString GetReceivedDirectory(const QString& filesDirectory)
{
	return QDir(filesDirectory).absoluteFilePath(s_temporaryDirectoryName);
}


/**
	Files of the bodies of the directory. The directory is the temporary directory of the server of the test, but the files of
	other programs and of Qt itself can be there too.
*/
QStringList GetFilePaths(const QString& directory)
{
	QStringList paths;

	const QStringList nameFilters = QStringList() << (QString(s_bodyFilePrefix) + QLatin1Char('*'));
	const QFileInfoList entries = QDir(directory).entryInfoList(nameFilters, QDir::Files | QDir::NoDotAndDotDot);
	for (const QFileInfo& entry : entries){
		paths.append(entry.absoluteFilePath());
	}

	return paths;
}


/**
	The file of a body is deleted when the request is destroyed, it happens a little after the answer is sent.
*/
bool WaitForFileCount(const QString& directory, int count, int timeoutMs = 5000)
{
	QElapsedTimer timer;
	timer.start();

	while (GetFilePaths(directory).size() != count){
		if (timer.elapsed() > timeoutMs){
			return false;
		}

		QTest::qWait(10);
	}

	return true;
}


bool WaitForFileSize(const QString& filePath, qint64 size, int timeoutMs = 5000)
{
	QElapsedTimer timer;
	timer.start();

	while (QFileInfo(filePath).size() < size){
		if (timer.elapsed() > timeoutMs){
			return false;
		}

		QTest::qWait(10);
	}

	return true;
}


bool SendBodyInParts(CRawHttpClient& client, const QByteArray& body, int partSize)
{
	for (qsizetype offset = 0; offset < body.size(); offset += partSize){
		if (!client.Send(body.mid(offset, partSize))){
			return false;
		}
	}

	return true;
}


QByteArray CreateRequestHead(const QByteArray& target, qint64 contentLength, const CRawHttpClient::Headers& otherHeaders = CRawHttpClient::Headers())
{
	CRawHttpClient::Headers headers = otherHeaders;
	headers.append({"Content-Length", QByteArray::number(contentLength)});

	return CRawHttpClient::CreateRequest("PUT", target, headers);
}


} // namespace


// protected methods

// reimplemented (CLiveTestBase)

QString CLiveDynamicBodyTest::GetServerTemporaryDirectory(const imtrestdemo::CDemoServerSettings& settings) const
{
	return GetReceivedDirectory(settings.filesDirectory);
}


// private slots

void CLiveDynamicBodyTest::EmptyBodyGoesToMemoryTest()
{
	Response response;
	QVERIFY(Get("/dynamic/empty", response));

	const QString error = CheckAnswer(response, QByteArray(), "memory");
	QVERIFY2(error.isEmpty(), qPrintable(error));

	QVERIFY(GetFilePaths(GetReceivedDirectory(GetFilesDirectory())).isEmpty());
}


void CLiveDynamicBodyTest::SmallBodyGoesToMemoryTest()
{
	const QByteArray body = imtrestdemo::DemoProtocol::CreatePatternData(0, 1000);

	Response response;
	QVERIFY(Request("PUT", "/dynamic/small.bin", response, Headers(), body));

	const QString error = CheckAnswer(response, body, "memory");
	QVERIFY2(error.isEmpty(), qPrintable(error));

	// The body was in the memory: no file was written
	QVERIFY(GetFilePaths(GetReceivedDirectory(GetFilesDirectory())).isEmpty());
}


void CLiveDynamicBodyTest::BodyEqualToMemoryLimitGoesToMemoryTest()
{
	const QByteArray body = imtrestdemo::DemoProtocol::CreatePatternData(3, s_memoryBodyLimit);

	Response response;
	QVERIFY(Request("PUT", "/dynamic/limit.bin", response, Headers(), body));

	const QString error = CheckAnswer(response, body, "memory");
	QVERIFY2(error.isEmpty(), qPrintable(error));

	QVERIFY(GetFilePaths(GetReceivedDirectory(GetFilesDirectory())).isEmpty());
}


void CLiveDynamicBodyTest::BodyAboveMemoryLimitGoesToFileTest()
{
	const QByteArray body = imtrestdemo::DemoProtocol::CreatePatternData(3, s_memoryBodyLimit + 1);

	Response response;
	QVERIFY(Request("PUT", "/dynamic/above.bin", response, Headers(), body));

	// One byte more than the limit: the same request goes to a file
	const QString error = CheckAnswer(response, body, "file");
	QVERIFY2(error.isEmpty(), qPrintable(error));

	// The servlet has not taken the file, so it is deleted with the request
	QVERIFY(WaitForFileCount(GetReceivedDirectory(GetFilesDirectory()), 0));
}


void CLiveDynamicBodyTest::BigBodyGoesToFileTest()
{
	const QString directory = GetReceivedDirectory(GetFilesDirectory());
	const QByteArray body = imtrestdemo::DemoProtocol::CreatePatternData(11, 3 * s_mebibyte + 17);

	CRawHttpClient client;
	QVERIFY(Connect(client));
	QVERIFY(client.Send(CreateRequestHead("/dynamic/big.bin", body.size())));

	// The first megabyte is sent, the rest is not: more than the memory limit is already in the file of the body, so the body is not
	// collected in the memory. The last part of less than 16 KiB can stay in the write buffer of the file until the file is closed
	// (QFile), so the whole megabyte is not waited for. The wait is short: the client must not be idle for longer than the timeout
	// of the server (see CLiveTestBase), otherwise the server answers with 408.
	QVERIFY(client.Send(body.left(s_mebibyte)));
	QVERIFY(WaitForFileCount(directory, 1));

	const QStringList paths = GetFilePaths(directory);
	QCOMPARE(paths.size(), 1);
	QVERIFY2(WaitForFileSize(paths.first(), s_mebibyte / 2), "The received part is not written to the file");

	// The file has the name of the temporary files of imtrest
	QVERIFY(QFileInfo(paths.first()).fileName().startsWith(s_bodyFilePrefix));

	QVERIFY(SendBodyInParts(client, body.mid(s_mebibyte), 64 * 1024));

	Response response;
	QVERIFY(client.ReadResponse(response));

	const QString error = CheckAnswer(response, body, "file");
	QVERIFY2(error.isEmpty(), qPrintable(error));

	QVERIFY(WaitForFileCount(directory, 0));
}


void CLiveDynamicBodyTest::BodyEqualToMaxBodySizeGoesToFileTest()
{
	const QByteArray body = imtrestdemo::DemoProtocol::CreatePatternData(5, s_maxBodySize);

	CRawHttpClient client;
	QVERIFY(Connect(client));
	QVERIFY(client.Send(CreateRequestHead("/dynamic/biggest.bin", body.size())));
	QVERIFY(SendBodyInParts(client, body, 256 * 1024));

	// The biggest accepted body
	Response response;
	QVERIFY(client.ReadResponse(response, false, 30000));

	const QString error = CheckAnswer(response, body, "file");
	QVERIFY2(error.isEmpty(), qPrintable(error));

	QVERIFY(WaitForFileCount(GetReceivedDirectory(GetFilesDirectory()), 0));
}


void CLiveDynamicBodyTest::ExpectContinueForFileBodyTest()
{
	const QByteArray body = imtrestdemo::DemoProtocol::CreatePatternData(7, s_mebibyte);

	CRawHttpClient client;
	QVERIFY(Connect(client));
	QVERIFY(client.Send(CreateRequestHead("/dynamic/expect.bin", body.size(), {{"Expect", "100-continue"}})));

	// The decision is made by the head: the client is allowed to send the body that goes to a file
	int status = 0;
	QVERIFY(client.ReadInterimResponse(status));
	QCOMPARE(status, 100);

	QVERIFY(SendBodyInParts(client, body, 64 * 1024));

	Response response;
	QVERIFY(client.ReadResponse(response));

	const QString error = CheckAnswer(response, body, "file");
	QVERIFY2(error.isEmpty(), qPrintable(error));

	QVERIFY(WaitForFileCount(GetReceivedDirectory(GetFilesDirectory()), 0));
}


void CLiveDynamicBodyTest::ChunkedBodyGoesToFileTest()
{
	const QByteArray body = imtrestdemo::DemoProtocol::CreatePatternData(13, 5000);

	CRawHttpClient client;
	QVERIFY(Connect(client));
	QVERIFY(client.Send(CRawHttpClient::CreateRequest("PUT", "/dynamic/chunked.bin", {{"Transfer-Encoding", "chunked"}})));

	// The size is not known before the end, so even this small body goes to a file: it cannot be checked against the limit of the memory in advance
	for (qsizetype offset = 0; offset < body.size(); offset += 1000){
		QVERIFY(client.Send(CRawHttpClient::CreateChunk(body.mid(offset, 1000))));
	}

	QVERIFY(client.Send(CRawHttpClient::CreateLastChunk()));

	Response response;
	QVERIFY(client.ReadResponse(response));

	const QString error = CheckAnswer(response, body, "file");
	QVERIFY2(error.isEmpty(), qPrintable(error));

	QVERIFY(WaitForFileCount(GetReceivedDirectory(GetFilesDirectory()), 0));
}


void CLiveDynamicBodyTest::ParallelBodiesTest()
{
	const QString directory = GetReceivedDirectory(GetFilesDirectory());

	const int count = 4;
	const int partSize = 256 * 1024;

	QList<QByteArray> bodies;
	QList<std::shared_ptr<CRawHttpClient>> clients;

	for (int i = 0; i < count; ++i){
		bodies.append(imtrestdemo::DemoProtocol::CreatePatternData(100 * i, s_mebibyte + 1000 * (i + 1)));

		clients.append(std::make_shared<CRawHttpClient>());
		QVERIFY(Connect(*clients.last()));
		QVERIFY(clients.last()->Send(CreateRequestHead("/dynamic/parallel-" + QByteArray::number(i) + ".bin", bodies.last().size())));

		// Every body has its own file, so the bodies do not mix
		QVERIFY(clients.last()->Send(bodies.last().left(partSize)));
	}

	QVERIFY2(WaitForFileCount(directory, count), "The bodies that are received at the same time are not in different files");

	// The rest of the bodies is sent turn by turn
	for (qsizetype offset = partSize; offset < s_mebibyte + 1000 * (count + 1); offset += partSize){
		for (int i = 0; i < count; ++i){
			if (offset < bodies[i].size()){
				QVERIFY(clients[i]->Send(bodies[i].mid(offset, partSize)));
			}
		}
	}

	for (int i = 0; i < count; ++i){
		Response response;
		QVERIFY(clients[i]->ReadResponse(response));

		const QString error = CheckAnswer(response, bodies[i], "file");
		QVERIFY2(error.isEmpty(), qPrintable(QString("Body %1: %2").arg(i).arg(error)));
	}

	QVERIFY(WaitForFileCount(directory, 0));
}


void CLiveDynamicBodyTest::BodyAboveMaxBodySizeIsRefusedBeforeBodyTest()
{
	CRawHttpClient client;
	QVERIFY(Connect(client));

	// The head announces one byte more than the limit: the answer comes at once, the body is not sent
	QVERIFY(client.Send(CreateRequestHead("/dynamic/refused.bin", s_maxBodySize + 1)));

	Response response;
	QVERIFY(client.ReadResponse(response, false, 3000));
	QCOMPARE(response.statusCode, 413);
	QVERIFY(client.WaitForClose(8000));

	// No file was made for the body that is refused
	QVERIFY(GetFilePaths(GetReceivedDirectory(GetFilesDirectory())).isEmpty());
	QVERIFY(IsServerAlive());
}


void CLiveDynamicBodyTest::ExpectContinueIsRefusedForBodyAboveMaxBodySizeTest()
{
	CRawHttpClient client;
	QVERIFY(Connect(client));

	// The client waits for the permission before it sends a body that is too big: it gets the refusal and does not send it
	QVERIFY(client.Send(CreateRequestHead("/dynamic/refused.bin", 100 * s_mebibyte, {{"Expect", "100-continue"}})));

	Response response;
	QVERIFY(client.ReadResponse(response, false, 3000));
	QCOMPARE(response.statusCode, 413);
	QVERIFY(response.interimStatuses.isEmpty());
	QVERIFY(client.WaitForClose(8000));

	QVERIFY(GetFilePaths(GetReceivedDirectory(GetFilesDirectory())).isEmpty());
	QVERIFY(IsServerAlive());
}


void CLiveDynamicBodyTest::ChunkedBodyAboveMaxBodySizeIsRefusedWhileReceivedTest()
{
	const QString directory = GetReceivedDirectory(GetFilesDirectory());

	CRawHttpClient client;
	QVERIFY(Connect(client));

	// The size is not known from the head, so the limit works while the body is received
	QVERIFY(client.Send(CRawHttpClient::CreateRequest("PUT", "/dynamic/refused-chunked.bin", {{"Transfer-Encoding", "chunked"}})));

	const QByteArray part = imtrestdemo::DemoProtocol::CreatePatternData(0, 512 * 1024);
	for (int i = 0; i < 20; ++i){
		if (!client.Send(CRawHttpClient::CreateChunk(part))){
			break;
		}
	}

	Response response;
	QVERIFY(client.ReadResponse(response, false, 5000));
	QCOMPARE(response.statusCode, 413);

	// The part of the body that is received is not needed: its file is deleted
	QVERIFY(WaitForFileCount(directory, 0));
	QVERIFY(IsServerAlive());
}


void CLiveDynamicBodyTest::BrokenUploadLeavesNoFileTest()
{
	const QString directory = GetReceivedDirectory(GetFilesDirectory());
	const QByteArray body = imtrestdemo::DemoProtocol::CreatePatternData(1, s_mebibyte);

	{
		CRawHttpClient client;
		QVERIFY(Connect(client));
		QVERIFY(client.Send(CreateRequestHead("/dynamic/broken.bin", body.size())));
		QVERIFY(client.Send(body.left(100 * 1024)));
		QVERIFY(WaitForFileCount(directory, 1));

		// The connection is broken in the middle of the body
	}

	QVERIFY2(WaitForFileCount(directory, 0), "The file of the broken upload is left on the disk");
	QVERIFY(IsServerAlive());
}


I_ADD_TEST(CLiveDynamicBodyTest);


