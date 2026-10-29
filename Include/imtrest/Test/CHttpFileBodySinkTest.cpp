// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include "CHttpFileBodySinkTest.h"


// STL includes
#include <memory>

// Qt includes
#include <QtCore/QCryptographicHash>
#include <QtCore/QDir>
#include <QtCore/QFile>
#include <QtCore/QTemporaryDir>

// ImtCore includes
#include <imtrest/CHttpFileBodySink.h>
#include <imtrest/CHttpRequest.h>
#include <imtrest/IHttpRequestBodyPolicy.h>
#include <imtrest/IProtocolEngine.h>


namespace
{


typedef imtrest::IRequest IRequest;


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


class CProtocolEngineStub: virtual public imtrest::IProtocolEngine
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

	bool GetProtocolStatusCode(int /*statusCode*/, int& /*protocolStatusCode*/, QByteArray& /*statusCodeLiteral*/) const override
	{
		return false;
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


/**
	Policy that gives the receiver of the file for each request.
*/
class CFilePolicyStub: virtual public imtrest::IHttpRequestBodyPolicy
{
public:
	explicit CFilePolicyStub(const std::shared_ptr<imtrest::CHttpFileBodySink>& sinkPtr)
		:m_sinkPtr(sinkPtr)
	{
	}

	// reimplemented (imtrest::IHttpRequestBodyPolicy)
	BodyDecision DecideBody(const imtrest::CHttpRequest& /*requestHead*/) const override
	{
		BodyDecision retVal;
		retVal.mode = BM_STREAM;
		retVal.sinkPtr = m_sinkPtr;

		return retVal;
	}

private:
	std::shared_ptr<imtrest::CHttpFileBodySink> m_sinkPtr;
};


QByteArray CreateHead(const QByteArray& contentLength, const QByteArray& otherHeaders = QByteArray())
{
	QByteArray retVal("PUT /api/upload HTTP/1.1\r\nHost: localhost:7380\r\n");

	if (!contentLength.isEmpty()){
		retVal += "Content-Length: " + contentLength + "\r\n";
	}

	retVal += otherHeaders;
	retVal += "\r\n";

	return retVal;
}


QByteArray ReadFile(const QString& path)
{
	QFile file(path);
	if (!file.open(QIODevice::ReadOnly)){
		return QByteArray();
	}

	return file.readAll();
}


QStringList GetFileNames(const QTemporaryDir& dir)
{
	return QDir(dir.path()).entryList(QDir::Files | QDir::NoDotAndDotDot);
}


bool WriteFile(const QString& path, const QByteArray& data)
{
	QFile file(path);

	return file.open(QIODevice::WriteOnly) && (file.write(data) == data.size());
}


const qsizetype s_partSize = qsizetype(64) * 1024;
const int s_partCount = 256;


/**
	Part of the big body, it is different for each index.
*/
QByteArray CreatePart(int index)
{
	return QByteArray(s_partSize, char('a' + (index % 26)));
}


QByteArray CalculateBigBodyHash()
{
	QCryptographicHash hash(QCryptographicHash::Sha256);
	for (int i = 0; i < s_partCount; ++i){
		hash.addData(CreatePart(i));
	}

	return hash.result();
}


QByteArray CalculateFileHash(const QString& path)
{
	QFile file(path);
	if (!file.open(QIODevice::ReadOnly)){
		return QByteArray();
	}

	QCryptographicHash hash(QCryptographicHash::Sha256);
	hash.addData(&file);

	return hash.result();
}


} // namespace


// private slots

void CHttpFileBodySinkTest::TemporaryFileUntilFinishTest()
{
	QTemporaryDir dir;
	QVERIFY(dir.isValid());

	const QString path = dir.filePath("result.bin");

	imtrest::CHttpFileBodySink sink(path);
	QVERIFY(sink.Open());
	QVERIFY(sink.Write("hello", 5));

	// The target file does not exist until the body is received completely: the data are in a temporary file
	QVERIFY(!QFile::exists(path));
	QCOMPARE(GetFileNames(dir).size(), 1);

	QVERIFY(sink.Finish());

	QVERIFY(sink.IsFinished());
	QCOMPARE(ReadFile(path), QByteArray("hello"));

	// The temporary file is gone
	QCOMPARE(GetFileNames(dir), QStringList() << "result.bin");
}


void CHttpFileBodySinkTest::BodyInPartsTest()
{
	QTemporaryDir dir;
	QVERIFY(dir.isValid());

	const QString path = dir.filePath("result.bin");

	imtrest::CHttpFileBodySink sink(path);
	QVERIFY(sink.Open());
	QVERIFY(sink.Write("01234", 5));
	QVERIFY(sink.Write("", 0));
	QVERIFY(sink.Write("56789", 5));
	QVERIFY(sink.Finish());

	QCOMPARE(ReadFile(path), QByteArray("0123456789"));
}


void CHttpFileBodySinkTest::EmptyBodyCreatesEmptyFileTest()
{
	QTemporaryDir dir;
	QVERIFY(dir.isValid());

	const QString path = dir.filePath("empty.bin");

	imtrest::CHttpFileBodySink sink(path);
	QVERIFY(sink.Open());
	QVERIFY(sink.Finish());

	// The empty body is also a result
	QVERIFY(QFile::exists(path));
	QCOMPARE(QFileInfo(path).size(), qint64(0));
	QCOMPARE(sink.GetReceivedSize(), qint64(0));
}


void CHttpFileBodySinkTest::BigBodyTest()
{
	QTemporaryDir dir;
	QVERIFY(dir.isValid());

	const QString path = dir.filePath("big.bin");

	imtrest::CHttpFileBodySink sink(path);
	QVERIFY(sink.Open());

	for (int i = 0; i < s_partCount; ++i){
		const QByteArray part = CreatePart(i);

		QVERIFY(sink.Write(part.constData(), part.size()));
	}

	QVERIFY(sink.Finish());

	// 16 MiB
	QCOMPARE(QFileInfo(path).size(), qint64(s_partSize) * s_partCount);
	QCOMPARE(CalculateFileHash(path), CalculateBigBodyHash());
}


void CHttpFileBodySinkTest::ReceivedSizeTest()
{
	QTemporaryDir dir;
	QVERIFY(dir.isValid());

	const QString path = dir.filePath("result.bin");

	imtrest::CHttpFileBodySink sink(path);
	QCOMPARE(sink.GetFilePath(), path);
	QCOMPARE(sink.GetReceivedSize(), qint64(0));

	QVERIFY(sink.Open());
	QVERIFY(sink.Write("01234", 5));
	QVERIFY(sink.Write("567", 3));

	QCOMPARE(sink.GetReceivedSize(), qint64(8));
	QVERIFY(!sink.IsFinished());

	QVERIFY(sink.Finish());

	QCOMPARE(sink.GetReceivedSize(), qint64(8));
	QVERIFY(sink.IsFinished());
}


void CHttpFileBodySinkTest::FinishReplacesOldFileTest()
{
	QTemporaryDir dir;
	QVERIFY(dir.isValid());

	const QString path = dir.filePath("result.bin");
	QVERIFY(WriteFile(path, "old"));

	imtrest::CHttpFileBodySink sink(path);
	QVERIFY(sink.Open());
	QVERIFY(sink.Write("new data", 8));

	// The old file is intact while the new body is being received
	QCOMPARE(ReadFile(path), QByteArray("old"));

	QVERIFY(sink.Finish());

	QCOMPARE(ReadFile(path), QByteArray("new data"));
	QCOMPARE(GetFileNames(dir).size(), 1);
}


void CHttpFileBodySinkTest::AbortKeepsOldFileTest()
{
	QTemporaryDir dir;
	QVERIFY(dir.isValid());

	const QString path = dir.filePath("result.bin");
	QVERIFY(WriteFile(path, "old"));

	imtrest::CHttpFileBodySink sink(path);
	QVERIFY(sink.Open());
	QVERIFY(sink.Write("new", 3));

	sink.Abort();

	// An interrupted upload does not damage the file that was uploaded before
	QCOMPARE(ReadFile(path), QByteArray("old"));
	QCOMPARE(GetFileNames(dir), QStringList() << "result.bin");
	QVERIFY(!sink.IsFinished());
}


void CHttpFileBodySinkTest::AbortDeletesTemporaryFileTest()
{
	QTemporaryDir dir;
	QVERIFY(dir.isValid());

	const QString path = dir.filePath("result.bin");

	imtrest::CHttpFileBodySink sink(path);
	QVERIFY(sink.Open());
	QVERIFY(sink.Write("part of the body", 16));
	QCOMPARE(GetFileNames(dir).size(), 1);

	sink.Abort();

	QVERIFY(!QFile::exists(path));
	QVERIFY(GetFileNames(dir).isEmpty());

	// The second call is harmless
	sink.Abort();

	QVERIFY(GetFileNames(dir).isEmpty());
}


void CHttpFileBodySinkTest::DestructionWithoutFinishDeletesTemporaryFileTest()
{
	QTemporaryDir dir;
	QVERIFY(dir.isValid());

	{
		imtrest::CHttpFileBodySink sink(dir.filePath("result.bin"));
		QVERIFY(sink.Open());
		QVERIFY(sink.Write("part", 4));

		QCOMPARE(GetFileNames(dir).size(), 1);
	}

	// The receiver that nobody has finished or aborted leaves nothing on the disk
	QVERIFY(GetFileNames(dir).isEmpty());
}


void CHttpFileBodySinkTest::AbortAfterFinishKeepsFileTest()
{
	QTemporaryDir dir;
	QVERIFY(dir.isValid());

	const QString path = dir.filePath("result.bin");

	imtrest::CHttpFileBodySink sink(path);
	QVERIFY(sink.Open());
	QVERIFY(sink.Write("done", 4));
	QVERIFY(sink.Finish());

	sink.Abort();

	QCOMPARE(ReadFile(path), QByteArray("done"));
}


void CHttpFileBodySinkTest::OpenInMissingDirectoryFailsTest()
{
	QTemporaryDir dir;
	QVERIFY(dir.isValid());

	imtrest::CHttpFileBodySink sink(dir.filePath("missing/result.bin"));

	// The failure is known before the body is received, so the client is not asked to send it
	QVERIFY(!sink.Open());
	QVERIFY(!sink.GetErrorString().isEmpty());

	QVERIFY(!sink.Write("data", 4));
	QVERIFY(!sink.Finish());
	QVERIFY(GetFileNames(dir).isEmpty());
}


void CHttpFileBodySinkTest::UseBeforeOpenFailsTest()
{
	QTemporaryDir dir;
	QVERIFY(dir.isValid());

	imtrest::CHttpFileBodySink sink(dir.filePath("result.bin"));

	QVERIFY(!sink.Write("data", 4));
	QVERIFY(!sink.GetErrorString().isEmpty());
	QVERIFY(!sink.Finish());

	sink.Abort();

	QVERIFY(GetFileNames(dir).isEmpty());
}


void CHttpFileBodySinkTest::RepeatedOpenTest()
{
	QTemporaryDir dir;
	QVERIFY(dir.isValid());

	imtrest::CHttpFileBodySink sink(dir.filePath("result.bin"));

	QVERIFY(sink.Open());
	QVERIFY(sink.Write("abc", 3));
	QVERIFY(sink.Open());
	QVERIFY(sink.Write("def", 3));
	QVERIFY(sink.Finish());

	// The second Open() does not start the file again
	QCOMPARE(ReadFile(dir.filePath("result.bin")), QByteArray("abcdef"));
}


void CHttpFileBodySinkTest::WriteAfterFinishFailsTest()
{
	QTemporaryDir dir;
	QVERIFY(dir.isValid());

	const QString path = dir.filePath("result.bin");

	imtrest::CHttpFileBodySink sink(path);
	QVERIFY(sink.Open());
	QVERIFY(sink.Write("done", 4));
	QVERIFY(sink.Finish());

	QVERIFY(!sink.Write("more", 4));
	QCOMPARE(ReadFile(path), QByteArray("done"));
	QCOMPARE(sink.GetReceivedSize(), qint64(4));
}


void CHttpFileBodySinkTest::RequestWritesBodyToFileTest()
{
	QTemporaryDir dir;
	QVERIFY(dir.isValid());

	const QString path = dir.filePath("part.bin");

	CRequestServletStub servlet;
	CProtocolEngineStub engine;
	imtrest::CHttpRequest request(servlet, engine);

	const std::shared_ptr<imtrest::CHttpFileBodySink> sinkPtr = std::make_shared<imtrest::CHttpFileBodySink>(path);
	QVERIFY(sinkPtr->Open());

	CFilePolicyStub policy(sinkPtr);
	request.SetBodyPolicy(policy);

	QVERIFY(request.ExecuteHttpParser(CreateHead(QByteArray::number(qint64(s_partSize) * s_partCount)), nullptr));

	for (int i = 0; i < s_partCount; ++i){
		QVERIFY(request.ExecuteHttpParser(CreatePart(i), nullptr));

		// The body is not kept in the request
		QVERIFY(request.GetBody().isEmpty());
	}

	QCOMPARE(request.GetState(), IRequest::RS_MESSAGE_COMPLETE);
	QVERIFY(sinkPtr->IsFinished());

	// The part of 16 MiB is written to the disk by the parts
	QCOMPARE(sinkPtr->GetReceivedSize(), qint64(s_partSize) * s_partCount);
	QCOMPARE(CalculateFileHash(path), CalculateBigBodyHash());
	QCOMPARE(GetFileNames(dir), QStringList() << "part.bin");
}


void CHttpFileBodySinkTest::ChunkedRequestWritesBodyToFileTest()
{
	QTemporaryDir dir;
	QVERIFY(dir.isValid());

	const QString path = dir.filePath("part.bin");

	CRequestServletStub servlet;
	CProtocolEngineStub engine;
	imtrest::CHttpRequest request(servlet, engine);

	const std::shared_ptr<imtrest::CHttpFileBodySink> sinkPtr = std::make_shared<imtrest::CHttpFileBodySink>(path);
	QVERIFY(sinkPtr->Open());

	CFilePolicyStub policy(sinkPtr);
	request.SetBodyPolicy(policy);

	QVERIFY(request.ExecuteHttpParser(
				CreateHead(QByteArray(), "Transfer-Encoding: chunked\r\n") + "5\r\nhello\r\n6\r\n world\r\n0\r\n\r\n",
				nullptr));

	// The file has the content of the chunks without their framing
	QCOMPARE(ReadFile(path), QByteArray("hello world"));
}


void CHttpFileBodySinkTest::EmptyRequestCreatesEmptyFileTest()
{
	QTemporaryDir dir;
	QVERIFY(dir.isValid());

	const QString path = dir.filePath("part.bin");

	CRequestServletStub servlet;
	CProtocolEngineStub engine;
	imtrest::CHttpRequest request(servlet, engine);

	const std::shared_ptr<imtrest::CHttpFileBodySink> sinkPtr = std::make_shared<imtrest::CHttpFileBodySink>(path);
	QVERIFY(sinkPtr->Open());

	CFilePolicyStub policy(sinkPtr);
	request.SetBodyPolicy(policy);

	QVERIFY(request.ExecuteHttpParser(CreateHead("0"), nullptr));

	QVERIFY(sinkPtr->IsFinished());
	QVERIFY(QFile::exists(path));
	QCOMPARE(QFileInfo(path).size(), qint64(0));
}


void CHttpFileBodySinkTest::BrokenRequestLeavesNoFileTest()
{
	QTemporaryDir dir;
	QVERIFY(dir.isValid());

	const QString path = dir.filePath("part.bin");

	CRequestServletStub servlet;
	CProtocolEngineStub engine;

	// The receiver is alive after the request: its owner reads the result, so the request releases it by itself
	const std::shared_ptr<imtrest::CHttpFileBodySink> sinkPtr = std::make_shared<imtrest::CHttpFileBodySink>(path);
	QVERIFY(sinkPtr->Open());

	CFilePolicyStub policy(sinkPtr);

	{
		imtrest::CHttpRequest request(servlet, engine);
		request.SetBodyPolicy(policy);

		// The connection is broken in the middle of the body
		QVERIFY(request.ExecuteHttpParser(CreateHead("1000") + QByteArray(400, 'x'), nullptr));

		QCOMPARE(GetFileNames(dir).size(), 1);
	}

	QVERIFY(!QFile::exists(path));
	QVERIFY(GetFileNames(dir).isEmpty());
	QVERIFY(!sinkPtr->IsFinished());
}


void CHttpFileBodySinkTest::RefusedRequestLeavesNoFileTest()
{
	QTemporaryDir dir;
	QVERIFY(dir.isValid());

	const QString path = dir.filePath("part.bin");

	CRequestServletStub servlet;
	CProtocolEngineStub engine;
	imtrest::CHttpRequest request(servlet, engine);

	const std::shared_ptr<imtrest::CHttpFileBodySink> sinkPtr = std::make_shared<imtrest::CHttpFileBodySink>(path);
	QVERIFY(sinkPtr->Open());

	CFilePolicyStub policy(sinkPtr);
	request.SetBodyPolicy(policy);
	request.SetDefaultMaxBodySize(100);

	// The body is bigger than the limit, the temporary file is deleted at once
	QVERIFY(request.ExecuteHttpParser(CreateHead("1000"), nullptr));

	QVERIFY(!QFile::exists(path));
	QVERIFY(GetFileNames(dir).isEmpty());
}


void CHttpFileBodySinkTest::OldFileSurvivesBrokenRequestTest()
{
	QTemporaryDir dir;
	QVERIFY(dir.isValid());

	const QString path = dir.filePath("part.bin");
	QVERIFY(WriteFile(path, "old"));

	CRequestServletStub servlet;
	CProtocolEngineStub engine;

	const std::shared_ptr<imtrest::CHttpFileBodySink> sinkPtr = std::make_shared<imtrest::CHttpFileBodySink>(path);
	QVERIFY(sinkPtr->Open());

	CFilePolicyStub policy(sinkPtr);

	{
		imtrest::CHttpRequest request(servlet, engine);
		request.SetBodyPolicy(policy);

		QVERIFY(request.ExecuteHttpParser(CreateHead("1000") + QByteArray(400, 'x'), nullptr));
	}

	// The part that was uploaded before is not damaged by the interrupted upload
	QCOMPARE(ReadFile(path), QByteArray("old"));
	QCOMPARE(GetFileNames(dir), QStringList() << "part.bin");
}


I_ADD_TEST(CHttpFileBodySinkTest);


