// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include "CLiveFileTest.h"


// Qt includes
#include <QtCore/QDir>
#include <QtCore/QFile>

// ACF includes
#include <itest/CStandardTestExecutor.h>

// ImtCore includes
#include <imtrestdemo/CDemoProtocol.h>


namespace
{


const QByteArray s_helloText("Hello, imtrest!\n");


}


void CLiveFileTest::FileGetTest()
{
	Response response;
	QVERIFY(Get("/files/hello.txt", response));
	QCOMPARE(response.statusCode, 200);
	QCOMPARE(response.body, s_helloText);

	QCOMPARE(response.GetHeader("content-length"), QByteArray("16"));
	QCOMPARE(response.GetHeader("content-type"), QByteArray("text/plain; charset=utf-8"));
	QCOMPARE(response.GetHeader("accept-ranges"), QByteArray("bytes"));
	QCOMPARE(response.GetHeader("cache-control"), QByteArray("no-cache"));

	// The strong entity tag in the quotes and the date of the change
	const QByteArray eTag = response.GetHeader("etag");
	QVERIFY(eTag.size() > 2 && eTag.startsWith('"') && eTag.endsWith('"'));
	QVERIFY(response.GetHeader("last-modified").endsWith("GMT"));
}


void CLiveFileTest::FileHeadTest()
{
	Response get;
	QVERIFY(Get("/files/hello.txt", get));

	Response head;
	QVERIFY(Request("HEAD", "/files/hello.txt", head));
	QCOMPARE(head.statusCode, 200);
	QVERIFY(head.body.isEmpty());

	// RFC 9110, section 9.3.2: the same headers as of GET
	QCOMPARE(head.GetHeader("content-length"), get.GetHeader("content-length"));
	QCOMPARE(head.GetHeader("content-type"), get.GetHeader("content-type"));
	QCOMPARE(head.GetHeader("etag"), get.GetHeader("etag"));
	QCOMPARE(head.GetHeader("last-modified"), get.GetHeader("last-modified"));
	QCOMPARE(head.GetHeader("accept-ranges"), QByteArray("bytes"));
}


void CLiveFileTest::FileEmptyTest()
{
	Response response;
	QVERIFY(Get("/files/empty.txt", response));
	QCOMPARE(response.statusCode, 200);
	QCOMPARE(response.GetHeader("content-length"), QByteArray("0"));
	QVERIFY(response.body.isEmpty());

	// No byte of an empty file can be asked for
	Response range;
	QVERIFY(Get("/files/empty.txt", range, {{"Range", "bytes=0-0"}}));
	QCOMPARE(range.statusCode, 416);
	QCOMPARE(range.GetHeader("content-range"), QByteArray("bytes */0"));
}


void CLiveFileTest::FileMissingTest()
{
	Response missing;
	QVERIFY(Get("/files/nothing.txt", missing));
	QCOMPARE(missing.statusCode, 404);

	// A directory is not a file
	Response directory;
	QVERIFY(Get("/files/uploaded", directory));
	QCOMPARE(directory.statusCode, 404);

	Response root;
	QVERIFY(Get("/files/", root));
	QVERIFY(root.statusCode >= 400);
}


void CLiveFileTest::FileRangeTest_data()
{
	QTest::addColumn<QByteArray>("range");
	QTest::addColumn<int>("status");
	QTest::addColumn<QByteArray>("body");
	QTest::addColumn<QByteArray>("contentRange");

	QTest::newRow("from to") << QByteArray("bytes=0-4") << 206 << QByteArray("Hello") << QByteArray("bytes 0-4/16");
	QTest::newRow("open end") << QByteArray("bytes=7-") << 206 << QByteArray("imtrest!\n") << QByteArray("bytes 7-15/16");
	QTest::newRow("suffix") << QByteArray("bytes=-5") << 206 << QByteArray("est!\n") << QByteArray("bytes 11-15/16");
	QTest::newRow("one byte") << QByteArray("bytes=0-0") << 206 << QByteArray("H") << QByteArray("bytes 0-0/16");
	QTest::newRow("last byte") << QByteArray("bytes=15-15") << 206 << QByteArray("\n") << QByteArray("bytes 15-15/16");
	QTest::newRow("end is cut") << QByteArray("bytes=10-1000") << 206 << QByteArray("rest!\n") << QByteArray("bytes 10-15/16");
	QTest::newRow("suffix is longer") << QByteArray("bytes=-1000") << 206 << s_helloText << QByteArray("bytes 0-15/16");

	// The ranges that are ignored: the whole file with 200 (RFC 9110, section 14.2)
	QTest::newRow("two ranges") << QByteArray("bytes=0-1,3-4") << 200 << s_helloText << QByteArray();
	QTest::newRow("other unit") << QByteArray("items=0-1") << 200 << s_helloText << QByteArray();
	QTest::newRow("not numbers") << QByteArray("bytes=abc") << 200 << s_helloText << QByteArray();
	QTest::newRow("last before first") << QByteArray("bytes=5-2") << 200 << s_helloText << QByteArray();

	// Nothing of the file is in the range: 416 with the size of the file
	QTest::newRow("after the end") << QByteArray("bytes=16-") << 416 << QByteArray() << QByteArray("bytes */16");
	QTest::newRow("far after the end") << QByteArray("bytes=100-200") << 416 << QByteArray() << QByteArray("bytes */16");
}


void CLiveFileTest::FileRangeTest()
{
	QFETCH(QByteArray, range);
	QFETCH(int, status);
	QFETCH(QByteArray, body);
	QFETCH(QByteArray, contentRange);

	Response response;
	QVERIFY(Get("/files/hello.txt", response, {{"Range", range}}));
	QCOMPARE(response.statusCode, status);
	QCOMPARE(response.body, body);
	QCOMPARE(response.GetHeader("content-range"), contentRange);

	if (status == 206){
		QCOMPARE(response.GetHeader("content-length").toInt(), int(body.size()));
	}
}


void CLiveFileTest::FileLargeRangeTest()
{
	const qint64 size = imtrestdemo::DemoProtocol::s_patternFileSize;

	Response middle;
	QVERIFY(Get("/files/pattern.bin", middle, {{"Range", "bytes=500000-600000"}}));
	QCOMPARE(middle.statusCode, 206);
	QCOMPARE(middle.GetHeader("content-range"), "bytes 500000-600000/" + QByteArray::number(size));
	QVERIFY(middle.body == imtrestdemo::DemoProtocol::CreatePatternData(500000, 100001));

	Response suffix;
	QVERIFY(Get("/files/pattern.bin", suffix, {{"Range", "bytes=-1000"}}));
	QCOMPARE(suffix.statusCode, 206);
	QVERIFY(suffix.body == imtrestdemo::DemoProtocol::CreatePatternData(size - 1000, 1000));

	Response whole;
	QVERIFY(Get("/files/pattern.bin", whole));
	QCOMPARE(whole.statusCode, 200);
	QCOMPARE(qint64(whole.body.size()), size);
	QVERIFY(whole.body == imtrestdemo::DemoProtocol::CreatePatternData(0, size));
}


void CLiveFileTest::FileRangeOfHeadIsIgnoredTest()
{
	// RFC 9110, section 14.2: Range is for GET, HEAD gives the headers of the whole file
	Response head;
	QVERIFY(Request("HEAD", "/files/hello.txt", head, {{"Range", "bytes=0-4"}}));
	QCOMPARE(head.statusCode, 200);
	QCOMPARE(head.GetHeader("content-length"), QByteArray("16"));
	QVERIFY(!head.HasHeader("content-range"));
}


void CLiveFileTest::FileConditionsTest()
{
	Response base;
	QVERIFY(Get("/files/hello.txt", base));

	const QByteArray eTag = base.GetHeader("etag");
	const QByteArray lastModified = base.GetHeader("last-modified");
	const QByteArray otherTag = "\"another-tag\"";
	const QByteArray longAgo = "Sun, 06 Nov 1994 08:49:37 GMT";

	struct Case
	{
		const char* name;
		CLiveFileTest::Headers headers;
		int status;
	};

	const QList<Case> cases = {
		{"If-None-Match with the tag", {{"If-None-Match", eTag}}, 304},
		{"If-None-Match with a weak tag", {{"If-None-Match", "W/" + eTag}}, 304},
		{"If-None-Match with a list", {{"If-None-Match", otherTag + ", " + eTag}}, 304},
		{"If-None-Match with another tag", {{"If-None-Match", otherTag}}, 200},
		{"If-None-Match with any", {{"If-None-Match", "*"}}, 304},
		{"If-Match with another tag", {{"If-Match", otherTag}}, 412},
		{"If-Match with the tag", {{"If-Match", eTag}}, 200},
		{"If-Match with any", {{"If-Match", "*"}}, 200},
		{"If-Modified-Since the date of the change", {{"If-Modified-Since", lastModified}}, 304},
		{"If-Modified-Since long ago", {{"If-Modified-Since", longAgo}}, 200},
		{"If-Unmodified-Since long ago", {{"If-Unmodified-Since", longAgo}}, 412},
		{"If-Unmodified-Since the date of the change", {{"If-Unmodified-Since", lastModified}}, 200},
		{"If-Range with the tag", {{"Range", "bytes=0-4"}, {"If-Range", eTag}}, 206},
		{"If-Range with another tag", {{"Range", "bytes=0-4"}, {"If-Range", otherTag}}, 200},
		{"If-Range with the date", {{"Range", "bytes=0-4"}, {"If-Range", lastModified}}, 206},
		{"If-Range with an old date", {{"Range", "bytes=0-4"}, {"If-Range", longAgo}}, 200},
		{"the condition is checked before the range", {{"Range", "bytes=0-4"}, {"If-None-Match", eTag}}, 304},
		{"If-Match fails before the range", {{"Range", "bytes=0-4"}, {"If-Match", otherTag}}, 412}};

	for (const Case& testCase : cases){
		Response response;
		QVERIFY2(Get("/files/hello.txt", response, testCase.headers), testCase.name);
		QVERIFY2(response.statusCode == testCase.status, qPrintable(QString("%1: the status is %2, %3 is expected").arg(testCase.name).arg(response.statusCode).arg(testCase.status)));

		if (testCase.status == 304){
			// RFC 9110, section 15.4.5: no body, the validators are sent
			QVERIFY2(response.body.isEmpty(), testCase.name);
			QCOMPARE(response.GetHeader("etag"), eTag);
		}

		if (testCase.status == 200){
			QVERIFY2(response.body == s_helloText, testCase.name);
		}

		if (testCase.status == 206){
			QVERIFY2(response.body == "Hello", testCase.name);
		}
	}
}


void CLiveFileTest::FileMalformedConditionTest()
{
	// A value that is not an entity tag: RFC 9110 does not allow to guess what the client meant
	Response response;
	QVERIFY(Get("/files/hello.txt", response, {{"If-Match", "not-quoted"}}));
	QCOMPARE(response.statusCode, 400);

	// A date that cannot be read is ignored
	Response ignored;
	QVERIFY(Get("/files/hello.txt", ignored, {{"If-Modified-Since", "yesterday"}}));
	QCOMPARE(ignored.statusCode, 200);
}


void CLiveFileTest::FilePrivateTest()
{
	Response noToken;
	QVERIFY(Get("/files/private/secret.txt", noToken));
	QCOMPARE(noToken.statusCode, 401);
	QVERIFY(noToken.GetHeader("www-authenticate").startsWith("Bearer"));
	QVERIFY(!noToken.body.contains("secret of the demo"));

	Response wrongToken;
	QVERIFY(Get("/files/private/secret.txt", wrongToken, {{"Authorization", "Bearer wrong"}}));
	QCOMPARE(wrongToken.statusCode, 401);

	Response withToken;
	QVERIFY(Get("/files/private/secret.txt", withToken, WithToken()));
	QCOMPARE(withToken.statusCode, 200);
	QCOMPARE(withToken.body, QByteArray(imtrestdemo::DemoProtocol::s_privateFileText));
}


void CLiveFileTest::FilePathSafetyTest_data()
{
	QTest::addColumn<QByteArray>("target");

	QTest::newRow("parent directory") << QByteArray("/files/../hello.txt");
	QTest::newRow("encoded parent directory") << QByteArray("/files/%2e%2e/hello.txt");
	QTest::newRow("parent inside") << QByteArray("/files/uploaded/../hello.txt");
	QTest::newRow("to the secret") << QByteArray("/files/uploaded/../private/secret.txt");
	QTest::newRow("encoded slash") << QByteArray("/files/uploaded/..%2Fprivate%2Fsecret.txt");
	QTest::newRow("hidden file") << QByteArray("/files/.hidden");
	QTest::newRow("backslash") << QByteArray("/files/uploaded%5C..%5Chello.txt");
	QTest::newRow("drive letter") << QByteArray("/files/c:/windows/win.ini");
	QTest::newRow("empty segment") << QByteArray("/files//hello.txt");
}


void CLiveFileTest::FilePathSafetyTest()
{
	QFETCH(QByteArray, target);

	Response response;
	QVERIFY(Get(target, response));

	// Whatever the answer is, the file outside of the allowed place or the file with a token is not given
	QVERIFY2(response.statusCode >= 400, qPrintable(QString("status %1").arg(response.statusCode)));
	QVERIFY(!response.body.contains("secret of the demo"));
	QVERIFY(response.body != s_helloText);
}


void CLiveFileTest::FileLinkOutOfRootTest()
{
	const QString linkPath = QDir(GetFilesDirectory()).absoluteFilePath("link-out.txt");
	QFile::remove(linkPath);

	// The link inside of the directory that leads out of it
	const QString outside = QDir(GetFilesDirectory()).absoluteFilePath("../outside-of-root.txt");
	QFile outsideFile(outside);
	if (!outsideFile.open(QIODevice::WriteOnly) || !QFile::link(outside, linkPath)){
		QSKIP("The link cannot be created on this system");
	}

	outsideFile.write("outside");
	outsideFile.close();

	Response response;
	const bool isReceived = Get("/files/link-out.txt", response);

	QFile::remove(linkPath);
	QFile::remove(outside);

	QVERIFY(isReceived);
	QCOMPARE(response.statusCode, 403);
	QVERIFY(!response.body.contains("outside"));
}


void CLiveFileTest::FileDownloadAttributeTest()
{
	Response response;
	QVERIFY(Get("/files/hello.txt?download=1", response));
	QCOMPARE(response.statusCode, 200);
	QCOMPARE(response.GetHeader("content-disposition"), QByteArray("attachment; filename=\"hello.txt\""));
}


I_ADD_TEST(CLiveFileTest);
