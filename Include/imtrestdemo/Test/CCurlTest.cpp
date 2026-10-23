// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include "CCurlTest.h"


// Qt includes
#include <QtCore/QCryptographicHash>
#include <QtCore/QDir>
#include <QtCore/QElapsedTimer>
#include <QtCore/QFile>
#include <QtCore/QFileInfo>
#include <QtCore/QJsonDocument>
#include <QtCore/QProcess>
#include <QtCore/QStandardPaths>

// ACF includes
#include <itest/CStandardTestExecutor.h>

// ImtCore includes
#include <imtrestdemo/CDemoProtocol.h>


namespace
{


using imtrestdemo::DemoProtocol::CreatePatternData;


QByteArray CalculateSha256(const QByteArray& data)
{
	return QCryptographicHash::hash(data, QCryptographicHash::Sha256).toHex();
}


const QByteArray s_helloText("Hello, imtrest!\n");
const int s_requestProcessingTimeoutMs = 1000;


}


void CCurlTest::initTestCase()
{
	m_curlPath = FindCurl();
	if (m_curlPath.isEmpty()){
		QSKIP("curl is not found: set the argument --curl <path> or the environment variable IMTREST_CURL, or put curl to PATH");
	}

	// The program must work at all
	const CurlResult version = RunRawCurl({"--version"}, 10000);
	if (!version.IsOk()){
		QSKIP(qPrintable(QString("curl '%1' cannot be started").arg(m_curlPath)));
	}

	QVERIFY(m_serverDirectory.isValid());
	QVERIFY(m_workDirectory.isValid());

	imtrestdemo::CDemoServerSettings settings;
	settings.filesDirectory = m_serverDirectory.path();
	settings.requestProcessingTimeoutMs = s_requestProcessingTimeoutMs;

	QVERIFY2(m_server.Start(settings), qPrintable(m_server.GetErrorString()));
}


void CCurlTest::cleanupTestCase()
{
	m_server.Stop();
}


void CCurlTest::EchoTest()
{
	const CurlResult result = RunCurl({"-H", "X-Test: A", "-H", "x-test: B", "-H", "X-MiXeD: Case", "-H", "Cookie: a=1", "-H", "cookie: b=2"}, "/echo?a=1&b=two");
	QVERIFY2(result.IsOk(), result.standardError.constData());
	QCOMPARE(result.statusCode, 200);
	QCOMPARE(result.headers.value("content-type"), QByteArray("application/json"));
	QVERIFY(result.headers.contains("date"));

	const QJsonObject echo = ParseJson(result.body);
	QCOMPARE(echo.value("method").toString(), QString("GET"));
	QCOMPARE(echo.value("query").toObject().value("a").toString(), QString("1"));
	QCOMPARE(echo.value("query").toObject().value("b").toString(), QString("two"));

	// The repeated headers are merged, the names are case-insensitive
	const QJsonObject headers = echo.value("headers").toObject();
	QCOMPARE(headers.value("x-test").toString(), QString("A, B"));
	QCOMPARE(headers.value("x-mixed").toString(), QString("Case"));
	QCOMPARE(headers.value("cookie").toString(), QString("a=1; b=2"));
}


void CCurlTest::MethodsTest_data()
{
	QTest::addColumn<QString>("method");
	QTest::addColumn<bool>("hasBody");

	QTest::newRow("POST") << "POST" << true;
	QTest::newRow("PUT") << "PUT" << true;
	QTest::newRow("PATCH") << "PATCH" << true;
	QTest::newRow("DELETE") << "DELETE" << false;
	QTest::newRow("OPTIONS") << "OPTIONS" << false;
}


void CCurlTest::MethodsTest()
{
	QFETCH(QString, method);
	QFETCH(bool, hasBody);

	QStringList arguments = {"-X", method};
	if (hasBody){
		arguments << "--data-binary" << "body of curl";
	}

	const CurlResult result = RunCurl(arguments, "/echo");
	QVERIFY2(result.IsOk(), result.standardError.constData());
	QCOMPARE(result.statusCode, 200);

	const QJsonObject echo = ParseJson(result.body);
	QCOMPARE(echo.value("method").toString(), method);

	if (hasBody){
		QCOMPARE(echo.value("body").toString(), QString("body of curl"));
	}
}


void CCurlTest::KeepAliveTest()
{
	// One curl asks three times: if the connection is persistent, only the first request opens it. Every URL needs its own -o.
	QStringList arguments = {"-sS", "--noproxy", "*", "-w", "%{num_connects}\\n"};
	for (int i = 1; i <= 3; ++i){
		arguments << "-o" << QProcess::nullDevice() << GetUrl(QString("/echo?i=%1").arg(i));
	}

	const CurlResult result = RunRawCurl(arguments);
	QVERIFY2(result.IsOk(), result.standardError.constData());
	QCOMPARE(result.standardOutput.trimmed().split('\n'), QByteArrayList({"1", "0", "0"}));
}


void CCurlTest::HeadTest()
{
	const CurlResult result = RunCurl({"--head"}, "/files/hello.txt");
	QVERIFY2(result.IsOk(), result.standardError.constData());
	QCOMPARE(result.statusCode, 200);
	QCOMPARE(result.headers.value("content-length"), QByteArray("16"));
	QCOMPARE(result.headers.value("accept-ranges"), QByteArray("bytes"));
}


void CCurlTest::Http10Test()
{
	const CurlResult result = RunCurl({"--http1.0"}, "/echo");
	QVERIFY2(result.IsOk(), result.standardError.constData());
	QCOMPARE(result.statusCode, 200);
}


void CCurlTest::FileDownloadTest()
{
	const CurlResult hello = RunCurl({}, "/files/hello.txt");
	QVERIFY2(hello.IsOk(), hello.standardError.constData());
	QCOMPARE(hello.statusCode, 200);
	QCOMPARE(hello.body, s_helloText);
	QCOMPARE(hello.headers.value("content-type"), QByteArray("text/plain; charset=utf-8"));

	const CurlResult pattern = RunCurl({}, "/files/pattern.bin");
	QVERIFY2(pattern.IsOk(), pattern.standardError.constData());
	QCOMPARE(pattern.statusCode, 200);
	QVERIFY(pattern.body == CreatePatternData(0, imtrestdemo::DemoProtocol::s_patternFileSize));

	const CurlResult missing = RunCurl({}, "/files/nothing.txt");
	QCOMPARE(missing.statusCode, 404);
}


void CCurlTest::RangeTest_data()
{
	QTest::addColumn<QString>("range");
	QTest::addColumn<int>("status");
	QTest::addColumn<QByteArray>("body");
	QTest::addColumn<QByteArray>("contentRange");

	QTest::newRow("from to") << "0-4" << 206 << QByteArray("Hello") << QByteArray("bytes 0-4/16");
	QTest::newRow("open end") << "7-" << 206 << QByteArray("imtrest!\n") << QByteArray("bytes 7-15/16");
	QTest::newRow("suffix") << "-5" << 206 << QByteArray("est!\n") << QByteArray("bytes 11-15/16");
	QTest::newRow("two ranges") << "0-1,3-4" << 200 << s_helloText << QByteArray();
	QTest::newRow("after the end") << "100-200" << 416 << QByteArray() << QByteArray("bytes */16");
}


void CCurlTest::RangeTest()
{
	QFETCH(QString, range);
	QFETCH(int, status);
	QFETCH(QByteArray, body);
	QFETCH(QByteArray, contentRange);

	const CurlResult result = RunCurl({"-r", range}, "/files/hello.txt");

	// For 416 curl may end with the error of the range, the response is what matters
	QCOMPARE(result.statusCode, status);
	QCOMPARE(result.headers.value("content-range"), contentRange);

	if (status != 416){
		QVERIFY2(result.IsOk(), result.standardError.constData());
		QCOMPARE(result.body, body);
	}
}


void CCurlTest::ResumeTest()
{
	const QByteArray whole = CreatePatternData(0, imtrestdemo::DemoProtocol::s_patternFileSize);

	// The file that is downloaded not to the end: curl asks for the rest with Range
	const QString outputPath = CreateFile("resume.bin", whole.left(400000));
	const QString dumpPath = CreateTempPath("resume.headers");

	const CurlResult result = RunRawCurl({"-sS", "--noproxy", "*", "-C", "-", "-D", dumpPath, "-o", outputPath, GetUrl("/files/pattern.bin")});
	QVERIFY2(result.IsOk(), result.standardError.constData());

	const QByteArray dump = ReadFile(dumpPath);
	QVERIFY2(dump.startsWith("HTTP/1.1 206"), dump.constData());
	QVERIFY(dump.contains("Content-Range: bytes 400000-"));
	QVERIFY(ReadFile(outputPath) == whole);
}


void CCurlTest::ConditionsTest()
{
	const CurlResult base = RunCurl({}, "/files/hello.txt");
	QVERIFY2(base.IsOk(), base.standardError.constData());

	const QString eTag = QString::fromLatin1(base.headers.value("etag"));
	const QString lastModified = QString::fromLatin1(base.headers.value("last-modified"));
	QVERIFY(!eTag.isEmpty());
	QVERIFY(!lastModified.isEmpty());

	struct Case
	{
		const char* name;
		QStringList arguments;
		int status;
	};

	const QList<Case> cases = {
		{"If-None-Match with the tag", {"-H", "If-None-Match: " + eTag}, 304},
		{"If-None-Match with another tag", {"-H", "If-None-Match: \"another\""}, 200},
		{"If-Match with another tag", {"-H", "If-Match: \"another\""}, 412},
		{"If-Match with the tag", {"-H", "If-Match: " + eTag}, 200},
		{"If-Modified-Since the date of the change", {"-H", "If-Modified-Since: " + lastModified}, 304},
		{"If-Modified-Since long ago", {"-H", "If-Modified-Since: Sun, 06 Nov 1994 08:49:37 GMT"}, 200},
		{"If-Unmodified-Since long ago", {"-H", "If-Unmodified-Since: Sun, 06 Nov 1994 08:49:37 GMT"}, 412},
		{"If-Range with the tag", {"-r", "0-4", "-H", "If-Range: " + eTag}, 206},
		{"If-Range with another tag", {"-r", "0-4", "-H", "If-Range: \"another\""}, 200}};

	for (const Case& testCase : cases){
		const CurlResult result = RunCurl(testCase.arguments, "/files/hello.txt");
		QVERIFY2(result.IsOk(), testCase.name);
		QVERIFY2(result.statusCode == testCase.status, qPrintable(QString("%1: the status is %2, %3 is expected").arg(testCase.name).arg(result.statusCode).arg(testCase.status)));
	}

	// The time condition of curl itself: -z sends If-Modified-Since
	const CurlResult timeCondition = RunCurl({"-z", lastModified}, "/files/hello.txt");
	QVERIFY2(timeCondition.IsOk(), timeCondition.standardError.constData());
	QCOMPARE(timeCondition.statusCode, 304);
}


void CCurlTest::PrivateFileTest()
{
	const CurlResult noToken = RunCurl({}, "/files/private/secret.txt");
	QCOMPARE(noToken.statusCode, 401);
	QVERIFY(noToken.headers.value("www-authenticate").startsWith("Bearer"));
	QVERIFY(!noToken.body.contains("secret of the demo"));

	const CurlResult withToken = RunCurl(WithToken(), "/files/private/secret.txt");
	QVERIFY2(withToken.IsOk(), withToken.standardError.constData());
	QCOMPARE(withToken.statusCode, 200);
	QCOMPARE(withToken.body, QByteArray(imtrestdemo::DemoProtocol::s_privateFileText));
}


void CCurlTest::PathSafetyTest_data()
{
	QTest::addColumn<QString>("path");

	QTest::newRow("parent directory") << "/files/../hello.txt";
	QTest::newRow("to the secret") << "/files/uploaded/../private/secret.txt";
	QTest::newRow("encoded parent directory") << "/files/%2e%2e/hello.txt";
	QTest::newRow("hidden file") << "/files/.hidden";
}


void CCurlTest::PathSafetyTest()
{
	QFETCH(QString, path);

	// curl does not normalize the path, the server gets what is written
	const CurlResult result = RunCurl({"--path-as-is"}, path);
	QVERIFY2(result.statusCode >= 400, qPrintable(QString::number(result.statusCode)));
	QVERIFY(!result.body.contains("secret of the demo"));
	QVERIFY(result.body != s_helloText);
}


void CCurlTest::UploadTest()
{
	const QByteArray content = CreatePatternData(3, 5 * 1024 * 1024 + 11);
	const QString path = CreateFile("upload.bin", content);

	// More than 1 MB: curl asks for 100 Continue by itself
	const CurlResult result = RunCurl(WithToken({"-T", path}), "/upload/curl.bin");
	QVERIFY2(result.IsOk(), result.standardError.constData());
	QCOMPARE(result.statusCode, 201);
	QCOMPARE(result.headers.value("location"), QByteArray("/files/uploaded/curl.bin"));
	QCOMPARE(ParseJson(result.body).value("sha256").toString().toLatin1(), CalculateSha256(content));

	QVERIFY(ReadFile(QDir(m_server.GetUploadDirectory()).absoluteFilePath("curl.bin")) == content);

	// The file is available for the download
	const CurlResult download = RunCurl({}, "/files/uploaded/curl.bin");
	QVERIFY2(download.IsOk(), download.standardError.constData());
	QVERIFY(download.body == content);

	const CurlResult removed = RunCurl(WithToken({"-X", "DELETE"}), "/upload/curl.bin");
	QCOMPARE(removed.statusCode, 204);

	const CurlResult post = RunCurl(WithToken({"-X", "POST", "--data-binary", "@" + path}), "/upload/curl-post.bin");
	QVERIFY2(post.IsOk(), post.standardError.constData());
	QCOMPARE(post.statusCode, 201);
	QCOMPARE(ParseJson(post.body).value("sha256").toString().toLatin1(), CalculateSha256(content));
}


void CCurlTest::UploadChunkedTest()
{
	const QByteArray content = CreatePatternData(9, 2 * 1024 * 1024 + 3);
	const QString path = CreateFile("chunked.bin", content);

	// curl does not know the size of the body: it sends it in the chunks
	const CurlResult result = RunCurl(WithToken({"-T", path, "-H", "Transfer-Encoding: chunked"}), "/upload/curl-chunked.bin");
	QVERIFY2(result.IsOk(), result.standardError.constData());
	QCOMPARE(result.statusCode, 201);
	QCOMPARE(ParseJson(result.body).value("sha256").toString().toLatin1(), CalculateSha256(content));
	QVERIFY(ReadFile(QDir(m_server.GetUploadDirectory()).absoluteFilePath("curl-chunked.bin")) == content);
}


void CCurlTest::UploadWithoutTokenTest()
{
	const QString path = CreateFile("notoken.bin", CreatePatternData(0, 3 * 1024 * 1024));

	// The server refuses before the body: curl got 100 neither, nor sent the 3 MB
	const CurlResult result = RunCurl({"-T", path}, "/upload/curl-notoken.bin");
	QCOMPARE(result.statusCode, 401);
	QVERIFY(!QFile::exists(QDir(m_server.GetUploadDirectory()).absoluteFilePath("curl-notoken.bin")));
}


void CCurlTest::ExpectContinueTest()
{
	const QByteArray content = CreatePatternData(0, 2000);
	const QString path = CreateFile("expect.bin", content);

	const CurlResult result = RunCurl(WithToken({"-T", path, "-H", "Expect: 100-continue"}), "/upload/curl-expect.bin");
	QVERIFY2(result.IsOk(), result.standardError.constData());
	QCOMPARE(result.statusCode, 201);

	// curl has received the interim response before it sent the body
	QVERIFY2(result.headerDump.contains("HTTP/1.1 100 Continue"), result.headerDump.constData());
}


void CCurlTest::BodyLimitTest()
{
	const QString path = CreateFile("limit.bin", CreatePatternData(0, 100000));

	// The limit of /echo is 4096 bytes: the server refuses at once, as the head says the length
	const CurlResult result = RunCurl({"-X", "POST", "--data-binary", "@" + path, "-H", "Expect: 100-continue"}, "/echo");
	QCOMPARE(result.statusCode, 413);
	QVERIFY(!result.headerDump.contains("100 Continue"));
}


void CCurlTest::RefusedPathTest()
{
	const QString path = CreateFile("refused.bin", CreatePatternData(0, 100000));

	const CurlResult result = RunCurl({"-X", "POST", "--data-binary", "@" + path, "-H", "Expect: 100-continue"}, "/refuse");
	QCOMPARE(result.statusCode, 403);
}


void CCurlTest::StreamTest_data()
{
	QTest::addColumn<QString>("path");
	QTest::addColumn<qint64>("size");
	QTest::addColumn<bool>("isChunked");
	QTest::addColumn<QString>("version");

	QTest::newRow("with the size") << "/stream/3000000" << qint64(3000000) << false << QString();
	QTest::newRow("chunked") << "/stream/3000000?chunked=1" << qint64(3000000) << true << QString();
	QTest::newRow("slow") << "/stream/20000?part=5000&delay=30" << qint64(20000) << false << QString();
	QTest::newRow("chunked for HTTP/1.0") << "/stream/100000?chunked=1" << qint64(100000) << false << QString("--http1.0");
	QTest::newRow("zero") << "/stream/0" << qint64(0) << false << QString();
}


void CCurlTest::StreamTest()
{
	QFETCH(QString, path);
	QFETCH(qint64, size);
	QFETCH(bool, isChunked);
	QFETCH(QString, version);

	QStringList arguments;
	if (!version.isEmpty()){
		arguments << version;
	}

	const CurlResult result = RunCurl(arguments, path);
	QVERIFY2(result.IsOk(), result.standardError.constData());
	QCOMPARE(result.statusCode, 200);
	QVERIFY(result.body == CreatePatternData(0, size));
	QCOMPARE(result.headers.value("transfer-encoding").toLower().contains("chunked"), isChunked);

	if (!isChunked && version.isEmpty()){
		QCOMPARE(result.headers.value("content-length"), QByteArray::number(size));
	}
}


void CCurlTest::StreamHeadTest()
{
	const CurlResult result = RunCurl({"--head"}, "/stream/1000");
	QVERIFY2(result.IsOk(), result.standardError.constData());
	QCOMPARE(result.statusCode, 200);
	QCOMPARE(result.headers.value("content-length"), QByteArray("1000"));
}


void CCurlTest::StatusTest_data()
{
	QTest::addColumn<int>("status");

	for (int status : {200, 201, 204, 400, 401, 403, 404, 500, 503}){
		QTest::newRow(qPrintable(QString::number(status))) << status;
	}
}


void CCurlTest::StatusTest()
{
	QFETCH(int, status);

	const CurlResult result = RunCurl({}, "/behavior/status/" + QString::number(status));
	QVERIFY2(result.IsOk(), result.standardError.constData());
	QCOMPARE(result.statusCode, status);

	if (status == 204){
		QVERIFY(result.body.isEmpty());
		QVERIFY(!result.headers.contains("content-length"));
	}
}


void CCurlTest::RedirectTest()
{
	const CurlResult result = RunCurl({"-L"}, "/behavior/status/302");
	QVERIFY2(result.IsOk(), result.standardError.constData());
	QCOMPARE(result.statusCode, 200);
	QCOMPARE(ParseJson(result.body).value("path").toString(), QString("/echo"));
}


void CCurlTest::ServletExceptionTest()
{
	const CurlResult failed = RunCurl({}, "/behavior/throw");
	QCOMPARE(failed.statusCode, 500);

	const CurlResult alive = RunCurl({}, "/echo");
	QCOMPARE(alive.statusCode, 200);
}


void CCurlTest::ProcessingTimeoutTest()
{
	QElapsedTimer timer;
	timer.start();

	const CurlResult result = RunCurl({}, "/behavior/sleep?ms=3500");
	QCOMPARE(result.statusCode, 503);
	QVERIFY(timer.elapsed() < 3300);

	const CurlResult alive = RunCurl({}, "/echo");
	QCOMPARE(alive.statusCode, 200);
}


void CCurlTest::ParallelTest()
{
	QStringList arguments = {"-sS", "--noproxy", "*", "--parallel", "--parallel-max", "8", "-w", "%{http_code}\\n"};
	for (int i = 0; i < 16; ++i){
		arguments << "-o" << QProcess::nullDevice() << GetUrl(QString("/echo?i=%1").arg(i));
	}

	const CurlResult result = RunRawCurl(arguments);

	// The old versions of curl do not know --parallel
	if (result.exitCode == 2 || result.standardError.contains("--parallel")){
		QSKIP("This curl does not support --parallel");
	}

	QVERIFY2(result.IsOk(), result.standardError.constData());

	const QByteArrayList codes = result.standardOutput.trimmed().split('\n');
	QCOMPARE(codes.size(), 16);
	for (const QByteArray& code : codes){
		QCOMPARE(code.trimmed(), QByteArray("200"));
	}
}


CCurlTest::CurlResult CCurlTest::RunCurl(const QStringList& arguments, const QString& path, int timeoutMs)
{
	static int s_counter = 0;
	++s_counter;

	const QString headersPath = CreateTempPath(QString("headers-%1.txt").arg(s_counter));
	const QString bodyPath = CreateTempPath(QString("body-%1.bin").arg(s_counter));

	QStringList fullArguments = {"-sS", "--noproxy", "*", "--max-time", QString::number(timeoutMs / 1000), "-D", headersPath, "-o", bodyPath, "-w", "%{http_code}"};
	fullArguments += arguments;
	fullArguments << GetUrl(path);

	CurlResult result = RunRawCurl(fullArguments, timeoutMs);

	result.headerDump = ReadFile(headersPath);
	result.body = ReadFile(bodyPath);

	// -D writes all heads of the exchange: the interim responses (100) and the redirects, the last one is the answer
	QByteArrayList blocks = result.headerDump.split('\n');

	// The last head that is not an interim one
	QList<QByteArrayList> heads;
	QByteArrayList current;
	for (const QByteArray& rawLine : blocks){
		const QByteArray line = rawLine.trimmed();
		if (line.isEmpty()){
			if (!current.isEmpty()){
				heads.append(current);
				current.clear();
			}

			continue;
		}

		current.append(line);
	}

	if (!current.isEmpty()){
		heads.append(current);
	}

	for (int i = heads.size() - 1; i >= 0; --i){
		const QByteArrayList& head = heads[i];

		if (head.first().split(' ').value(1) == "100"){
			continue;
		}

		result.statusCode = head.first().split(' ').value(1).toInt();

		for (int lineIndex = 1; lineIndex < head.size(); ++lineIndex){
			const qsizetype colon = head[lineIndex].indexOf(':');
			if (colon > 0){
				result.headers.insert(head[lineIndex].left(colon).trimmed().toLower(), head[lineIndex].mid(colon + 1).trimmed());
			}
		}

		break;
	}

	if (result.statusCode == 0){
		result.statusCode = result.standardOutput.trimmed().right(3).toInt();
	}

	return result;
}


CCurlTest::CurlResult CCurlTest::RunRawCurl(const QStringList& arguments, int timeoutMs)
{
	CurlResult result;

	QProcess process;
	process.start(m_curlPath, arguments);

	if (!process.waitForStarted(10000)){
		result.standardError = process.errorString().toUtf8();

		return result;
	}

	if (!process.waitForFinished(timeoutMs)){
		process.kill();
		process.waitForFinished(2000);
		result.standardError = "curl is killed after the timeout";

		return result;
	}

	result.exitCode = process.exitStatus() == QProcess::NormalExit ? process.exitCode() : -1;
	result.standardOutput = process.readAllStandardOutput();
	result.standardError = process.readAllStandardError();

	return result;
}


QString CCurlTest::GetUrl(const QString& path) const
{
	return QStringLiteral("http://localhost:%1%2").arg(m_server.GetPort()).arg(path);
}


QString CCurlTest::CreateTempPath(const QString& name) const
{
	return QDir(m_workDirectory.path()).absoluteFilePath(name);
}


QString CCurlTest::CreateFile(const QString& name, const QByteArray& content) const
{
	const QString path = CreateTempPath(name);

	QFile file(path);
	if (file.open(QIODevice::WriteOnly)){
		file.write(content);
	}

	return path;
}


QByteArray CCurlTest::ReadFile(const QString& path)
{
	QFile file(path);

	return file.open(QIODevice::ReadOnly) ? file.readAll() : QByteArray();
}


QStringList CCurlTest::WithToken(const QStringList& arguments)
{
	QStringList retVal = arguments;
	retVal << "-H" << QString("Authorization: Bearer %1").arg(imtrestdemo::DemoProtocol::s_uploadToken);

	return retVal;
}


QJsonObject CCurlTest::ParseJson(const QByteArray& data)
{
	return QJsonDocument::fromJson(data).object();
}


QString CCurlTest::FindCurl()
{
	const QString fromEnvironment = qEnvironmentVariable("IMTREST_CURL");
	if (!fromEnvironment.isEmpty()){
		// The path that is given explicitly is not replaced by another curl: the test is skipped if it is wrong
		return QFileInfo(fromEnvironment).isExecutable() ? fromEnvironment : QString();
	}

	return QStandardPaths::findExecutable("curl");
}


I_ADD_TEST(CCurlTest);
