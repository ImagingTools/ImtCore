// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include "CLiveResumeTest.h"


// Qt includes
#include <QtCore/QCryptographicHash>
#include <QtCore/QDir>
#include <QtCore/QElapsedTimer>
#include <QtCore/QFile>
#include <QtCore/QFileInfo>
#include <QtCore/QJsonArray>

// ACF includes
#include <itest/CStandardTestExecutor.h>

// ImtCore includes
#include <imtrestdemo/CDemoProtocol.h>


namespace
{


using imtrestdemo::DemoProtocol::CreatePatternData;


QByteArray ReadFile(const QString& path)
{
	QFile file(path);

	return file.open(QIODevice::ReadOnly) ? file.readAll() : QByteArray();
}


QByteArray Sha256Hex(const QByteArray& data)
{
	return QCryptographicHash::hash(data, QCryptographicHash::Sha256).toHex();
}


QByteArray CreateRange(qint64 first, qint64 length, qint64 total)
{
	return QByteArray("bytes ") + QByteArray::number(first) + "-" + QByteArray::number(first + length - 1) + "/" + QByteArray::number(total);
}


}


void CLiveResumeTest::ResumeOnePartTest()
{
	// The whole file in one part: it is the same as the ordinary upload, but the file can be resumed if the connection breaks
	const qint64 total = 300000;

	Response response;
	QVERIFY(SendPart("one-part.bin", 0, total, total, response));
	QCOMPARE(response.statusCode, 201);
	QCOMPARE(response.GetHeader("upload-offset"), QByteArray::number(total));
	QCOMPARE(response.GetHeader("upload-length"), QByteArray::number(total));

	const QJsonObject result = ParseJson(response.body);
	QCOMPARE(result.value("complete").toBool(), true);
	QCOMPARE(result.value("size").toDouble(), double(total));
	QCOMPARE(result.value("sha256").toString().toLatin1(), Sha256Hex(CreatePatternData(0, total)));

	QCOMPARE(ReadFile(GetUploadDirectory() + "/one-part.bin"), CreatePatternData(0, total));

	// The temporary files are gone
	QVERIFY(!QFile::exists(GetDataPath("one-part.bin")));
	QVERIFY(!QFile::exists(GetDataPath("one-part.bin") + ".json"));
}


void CLiveResumeTest::ResumeByPartsTest()
{
	const qint64 total = 1000000;
	const qint64 part = 400000;

	Response response;
	QVERIFY(SendPart("parts.bin", 0, part, total, response));
	QCOMPARE(response.statusCode, 202);
	QCOMPARE(response.GetHeader("upload-offset"), QByteArray::number(part));
	QCOMPARE(ParseJson(response.body).value("complete").toBool(), false);

	// The file is not in its place until the last byte is received
	QVERIFY(!QFile::exists(GetUploadDirectory() + "/parts.bin"));
	QCOMPARE(QFileInfo(GetDataPath("parts.bin")).size(), part);

	int statusCode = 0;
	QJsonObject state = GetState("parts.bin", &statusCode);
	QCOMPARE(statusCode, 200);
	QCOMPARE(state.value("received").toDouble(), double(part));
	QCOMPARE(state.value("total").toDouble(), double(total));
	QCOMPARE(state.value("complete").toBool(), false);

	QVERIFY(SendPart("parts.bin", part, part, total, response));
	QCOMPARE(response.statusCode, 202);
	QCOMPARE(response.GetHeader("upload-offset"), QByteArray::number(2 * part));

	// The last part is shorter
	QVERIFY(SendPart("parts.bin", 2 * part, total - 2 * part, total, response));
	QCOMPARE(response.statusCode, 201);
	QCOMPARE(ParseJson(response.body).value("sha256").toString().toLatin1(), Sha256Hex(CreatePatternData(0, total)));

	QCOMPARE(ReadFile(GetUploadDirectory() + "/parts.bin"), CreatePatternData(0, total));
	QVERIFY(!QFile::exists(GetDataPath("parts.bin")));

	// The state of the finished upload
	state = GetState("parts.bin", &statusCode);
	QCOMPARE(statusCode, 200);
	QCOMPARE(state.value("complete").toBool(), true);
	QCOMPARE(state.value("received").toDouble(), double(total));
}


void CLiveResumeTest::ResumeBrokenUploadTest()
{
	const qint64 total = 3 * 1024 * 1024;

	// The connection breaks in the middle of the file
	QVERIFY(SendBrokenPart("broken.bin", 0, total, total, 1000 * 1000));

	QJsonObject state;
	QVERIFY(WaitForIdle("broken.bin", state));

	// The server has what it received: not all, not nothing, and it is the beginning of the file
	const qint64 received = qint64(state.value("received").toDouble());
	QVERIFY2(received > 0 && received < total, qPrintable(QString::number(received)));
	QCOMPARE(state.value("total").toDouble(), double(total));
	QCOMPARE(state.value("complete").toBool(), false);
	QCOMPARE(ReadFile(GetDataPath("broken.bin")), CreatePatternData(0, received));

	// The file is not in its place
	QVERIFY(!QFile::exists(GetUploadDirectory() + "/broken.bin"));

	// The client sends the rest
	Response response;
	QVERIFY(SendPart("broken.bin", received, total - received, total, response));
	QCOMPARE(response.statusCode, 201);
	QCOMPARE(ParseJson(response.body).value("size").toDouble(), double(total));
	QCOMPARE(ParseJson(response.body).value("sha256").toString().toLatin1(), Sha256Hex(CreatePatternData(0, total)));
	QVERIFY(ReadFile(GetUploadDirectory() + "/broken.bin") == CreatePatternData(0, total));
	QVERIFY(IsServerAlive());
}


void CLiveResumeTest::ResumeBrokenManyTimesTest()
{
	// The upload breaks three times and is resumed after every break from the place that the server tells
	const qint64 total = 4 * 1024 * 1024;
	qint64 received = 0;

	for (int attempt = 0; attempt < 3; ++attempt){
		QVERIFY(SendBrokenPart("many.bin", received, total - received, total, 500 * 1000));

		QJsonObject state;
		QVERIFY(WaitForIdle("many.bin", state));

		const qint64 now = qint64(state.value("received").toDouble());
		QVERIFY2(now > received && now < total, qPrintable(QString::number(now)));
		received = now;
	}

	Response response;
	QVERIFY(SendPart("many.bin", received, total - received, total, response));
	QCOMPARE(response.statusCode, 201);
	QVERIFY(ReadFile(GetUploadDirectory() + "/many.bin") == CreatePatternData(0, total));
}


void CLiveResumeTest::ResumeWrongOffsetTest()
{
	const qint64 total = 500;

	Response response;

	// The first part must start at the beginning
	QVERIFY(SendPart("offset.bin", 100, 100, total, response));
	QCOMPARE(response.statusCode, 409);
	QCOMPARE(response.GetHeader("upload-offset"), QByteArray("0"));
	QCOMPARE(response.GetHeader("upload-state"), QByteArray("offset-mismatch"));
	QVERIFY(!QFile::exists(GetDataPath("offset.bin")));

	QVERIFY(SendPart("offset.bin", 0, 100, total, response));
	QCOMPARE(response.statusCode, 202);

	// A gap, and the start from the beginning again: the server tells where to go on
	QVERIFY(SendPart("offset.bin", 200, 100, total, response));
	QCOMPARE(response.statusCode, 409);
	QCOMPARE(response.GetHeader("upload-offset"), QByteArray("100"));
	QCOMPARE(response.GetHeader("upload-length"), QByteArray::number(total));
	QCOMPARE(response.GetHeader("upload-state"), QByteArray("offset-mismatch"));

	QVERIFY(SendPart("offset.bin", 0, 100, total, response));
	QCOMPARE(response.statusCode, 409);
	QCOMPARE(response.GetHeader("upload-offset"), QByteArray("100"));

	// The other size of the file is another file
	QVERIFY(SendPart("offset.bin", 100, 100, 900, response));
	QCOMPARE(response.statusCode, 409);
	QCOMPARE(response.GetHeader("upload-state"), QByteArray("total-mismatch"));
	QCOMPARE(response.GetHeader("upload-length"), QByteArray::number(total));

	// Nothing is changed by the refused parts
	QCOMPARE(QFileInfo(GetDataPath("offset.bin")).size(), qint64(100));
	QCOMPARE(ReadFile(GetDataPath("offset.bin")), CreatePatternData(0, 100));

	QVERIFY(SendPart("offset.bin", 100, 400, total, response));
	QCOMPARE(response.statusCode, 201);
	QVERIFY(ReadFile(GetUploadDirectory() + "/offset.bin") == CreatePatternData(0, total));
}


void CLiveResumeTest::ResumeBadRangeTest_data()
{
	QTest::addColumn<QByteArray>("contentRange");
	QTest::addColumn<QByteArray>("contentLength");
	QTest::addColumn<int>("statusCode");

	QTest::newRow("last is not less than the size") << QByteArray("bytes 0-9/5") << QByteArray("10") << 400;
	QTest::newRow("first is greater than last") << QByteArray("bytes 5-2/10") << QByteArray("1") << 400;
	QTest::newRow("another unit") << QByteArray("items 0-9/10") << QByteArray("10") << 400;
	QTest::newRow("size is not known") << QByteArray("bytes 0-9/*") << QByteArray("10") << 400;
	QTest::newRow("unsatisfied range") << QByteArray("bytes */10") << QByteArray("10") << 400;
	QTest::newRow("not a number") << QByteArray("bytes a-b/10") << QByteArray("10") << 400;
	QTest::newRow("body shorter than the range") << QByteArray("bytes 0-9/10") << QByteArray("5") << 400;
	QTest::newRow("body longer than the range") << QByteArray("bytes 0-9/100") << QByteArray("20") << 400;
}


void CLiveResumeTest::ResumeBadRangeTest()
{
	QFETCH(QByteArray, contentRange);
	QFETCH(QByteArray, contentLength);
	QFETCH(int, statusCode);

	Response response;
	QVERIFY(Request("PUT", "/upload/bad-range.bin", response, WithToken({{"Content-Range", contentRange}, {"Content-Length", contentLength}})));
	QCOMPARE(response.statusCode, statusCode);

	// The server does not keep anything of a refused request
	QVERIFY(!QFile::exists(GetDataPath("bad-range.bin")));
	QVERIFY(!QFile::exists(GetUploadDirectory() + "/bad-range.bin"));
	QVERIFY(IsServerAlive());
}


void CLiveResumeTest::ResumeStateAndDeleteTest()
{
	int statusCode = 0;
	GetState("nothing.bin", &statusCode);
	QCOMPARE(statusCode, 404);

	Response response;
	QVERIFY(Request("DELETE", "/upload/nothing.bin", response, WithToken()));
	QCOMPARE(response.statusCode, 404);

	// The state needs the token like the upload
	QVERIFY(Get("/upload/nothing.bin", response));
	QCOMPARE(response.statusCode, 401);

	QVERIFY(SendPart("state.bin", 0, 1000, 5000, response));
	QCOMPARE(response.statusCode, 202);

	// The list shows the upload that is not finished apart from the files
	QVERIFY(Get("/upload", response, WithToken()));
	QCOMPARE(response.statusCode, 200);

	bool isListed = false;
	for (const QJsonValue& file : ParseJson(response.body).value("files").toArray()){
		isListed = isListed || file.toObject().value("name").toString().startsWith("state.bin");
	}

	QVERIFY2(!isListed, "The temporary files are not files");

	bool isUploading = false;
	for (const QJsonValue& part : ParseJson(response.body).value("uploading").toArray()){
		if (part.toObject().value("name").toString() == "state.bin"){
			isUploading = true;
			QCOMPARE(part.toObject().value("received").toDouble(), 1000.0);
			QCOMPARE(part.toObject().value("total").toDouble(), 5000.0);
		}
	}

	QVERIFY(isUploading);

	// DELETE removes the data of the upload that is not finished: the client can start again
	QVERIFY(Request("DELETE", "/upload/state.bin", response, WithToken()));
	QCOMPARE(response.statusCode, 204);
	QVERIFY(!QFile::exists(GetDataPath("state.bin")));
	GetState("state.bin", &statusCode);
	QCOMPARE(statusCode, 404);

	// ... also with another size of the file
	QVERIFY(SendPart("state.bin", 0, 1000, 2000, response));
	QCOMPARE(response.statusCode, 202);

	QVERIFY(Request("DELETE", "/upload/state.bin", response, WithToken()));
	QCOMPARE(response.statusCode, 204);
}


void CLiveResumeTest::ResumeConcurrentUploadTest()
{
	const qint64 total = 2 * 1024 * 1024;

	// The first client sends a part of the file and waits, its request is not finished
	CRawHttpClient first;
	QVERIFY(Connect(first));
	QVERIFY(first.Send(CRawHttpClient::CreateRequest("PUT", "/upload/busy.bin", WithToken({{"Content-Range", CreateRange(0, total, total)}, {"Content-Length", QByteArray::number(total)}}))));
	QVERIFY(first.Send(CreatePatternData(0, 200000)));

	QTest::qSleep(300);

	// The other client cannot write the same file now
	Response response;
	QVERIFY(SendPart("busy.bin", 0, 1000, total, response));
	QCOMPARE(response.statusCode, 409);
	QCOMPARE(response.GetHeader("upload-state"), QByteArray("in-progress"));

	int statusCode = 0;
	QCOMPARE(GetState("busy.bin", &statusCode).value("inProgress").toBool(), true);

	// Another file is not affected
	QVERIFY(SendPart("not-busy.bin", 0, 1000, 1000, response));
	QCOMPARE(response.statusCode, 201);
}


void CLiveResumeTest::ResumeTemporaryNamesTest()
{
	Response response;
	QVERIFY(Request("PUT", "/upload/name.uploading", response, WithToken(), "x"));
	QCOMPARE(response.statusCode, 400);

	QVERIFY(Request("PUT", "/upload/name.uploading.json", response, WithToken(), "x"));
	QCOMPARE(response.statusCode, 400);

	QVERIFY(SendPart("name.uploading", 0, 10, 10, response));
	QCOMPARE(response.statusCode, 400);
}


void CLiveResumeTest::ResumeReplacesFileTest()
{
	Response response;
	QVERIFY(Request("PUT", "/upload/replaced.bin", response, WithToken(), "old content"));
	QCOMPARE(response.statusCode, 201);

	// The old file stays while the new one is received by parts
	QVERIFY(SendPart("replaced.bin", 0, 1000, 3000, response));
	QCOMPARE(response.statusCode, 202);
	QCOMPARE(ReadFile(GetUploadDirectory() + "/replaced.bin"), QByteArray("old content"));

	QVERIFY(SendPart("replaced.bin", 1000, 2000, 3000, response));
	QCOMPARE(response.statusCode, 201);
	QVERIFY(ReadFile(GetUploadDirectory() + "/replaced.bin") == CreatePatternData(0, 3000));
}


// private methods

bool CLiveResumeTest::SendPart(const QByteArray& name, qint64 first, qint64 length, qint64 total, Response& response, const Headers& more)
{
	Headers headers = more;
	headers.append(qMakePair(QByteArray("Content-Range"), CreateRange(first, length, total)));

	return Request("PUT", "/upload/" + name, response, WithToken(headers), CreatePatternData(first, length));
}


bool CLiveResumeTest::SendBrokenPart(const QByteArray& name, qint64 first, qint64 length, qint64 total, qint64 sent)
{
	CRawHttpClient client;
	if (!Connect(client)){
		return false;
	}

	if (!client.Send(CRawHttpClient::CreateRequest("PUT", "/upload/" + name, WithToken({{"Content-Range", CreateRange(first, length, total)}, {"Content-Length", QByteArray::number(length)}})))){
		return false;
	}

	// The body goes by parts of 64 KiB, then the client goes away
	const QByteArray body = CreatePatternData(first, sent);
	for (qint64 position = 0; position < body.size(); position += 65536){
		if (!client.Send(body.mid(position, 65536))){
			return false;
		}
	}

	return true;
}


QJsonObject CLiveResumeTest::GetState(const QByteArray& name, int* statusCode)
{
	Response response;
	if (!Get("/upload/" + name, response, WithToken())){
		return QJsonObject();
	}

	if (statusCode != nullptr){
		*statusCode = response.statusCode;
	}

	return ParseJson(response.body);
}


bool CLiveResumeTest::WaitForIdle(const QByteArray& name, QJsonObject& state)
{
	QElapsedTimer timer;
	timer.start();

	while (timer.elapsed() < 8000){
		int statusCode = 0;
		state = GetState(name, &statusCode);
		if (statusCode == 200 && !state.value("inProgress").toBool()){
			return true;
		}

		QTest::qSleep(50);
	}

	return false;
}


QString CLiveResumeTest::GetDataPath(const QString& name) const
{
	return GetUploadDirectory() + "/" + name + ".uploading";
}


I_ADD_TEST(CLiveResumeTest);

