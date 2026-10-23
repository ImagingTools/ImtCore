// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include "CHttpTemporaryFileBodySinkTest.h"


// STL includes
#include <memory>
#include <vector>

// Qt includes
#include <QtCore/QCryptographicHash>
#include <QtCore/QDir>
#include <QtCore/QFile>
#include <QtCore/QFileInfo>
#include <QtCore/QSemaphore>
#include <QtCore/QSet>
#include <QtCore/QTemporaryDir>
#include <QtCore/QThread>

// ImtCore includes
#include <imtrest/CHttpRequest.h>
#include <imtrest/CHttpTemporaryFileBodySink.h>
#include <imtrest/IHttpRequestBodyPolicy.h>
#include <imtrest/IProtocolEngine.h>


namespace
{


typedef imtrest::IRequest IRequest;
typedef imtrest::CHttpTemporaryFileBodySink Sink;


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
	Policy that gives the receiver for each request.
*/
class CSinkPolicyStub: virtual public imtrest::IHttpRequestBodyPolicy
{
public:
	explicit CSinkPolicyStub(const std::shared_ptr<Sink>& sinkPtr)
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
	std::shared_ptr<Sink> m_sinkPtr;
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


const qsizetype s_partSize = qsizetype(64) * 1024;
const int s_partCount = 128;
const int s_receiverCount = 50;


/**
	Part of the big body, it is different for each index.
*/
QByteArray CreatePart(int index)
{
	return QByteArray(s_partSize, char('a' + (index % 26)));
}


/**
	Body of the receiver with the given index, it is different for each receiver: if two receivers write to the same file, it is seen.
*/
QByteArray CreateReceiverBody(int index)
{
	return QByteArray("receiver ") + QByteArray::number(index) + QByteArray(1000, char('a' + (index % 26)));
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

void CHttpTemporaryFileBodySinkTest::FileIsCreatedByOpenTest()
{
	QTemporaryDir dir;
	QVERIFY(dir.isValid());

	Sink sink(dir.path());

	// There is no file until the receiver is opened
	QVERIFY(sink.GetFilePath().isEmpty());
	QVERIFY(GetFileNames(dir).isEmpty());

	QVERIFY2(sink.Open(), qPrintable(sink.GetErrorString()));

	const QString path = sink.GetFilePath();
	QVERIFY(!path.isEmpty());
	QVERIFY(QFileInfo::exists(path));
	QCOMPARE(QFileInfo(path).canonicalPath(), QFileInfo(dir.path()).canonicalFilePath());
	QVERIFY(QFileInfo(path).fileName().startsWith(QStringLiteral("imtrest-body-")));
	QCOMPARE(GetFileNames(dir).size(), 1);

	QVERIFY(!sink.IsFinished());
	QCOMPARE(sink.GetReceivedSize(), qint64(0));
}


void CHttpTemporaryFileBodySinkTest::NamesOfFilesAreUniqueTest()
{
	QTemporaryDir dir;
	QVERIFY(dir.isValid());

	// Many bodies are received to the same directory at the same time. All the receivers have the same template of the name,
	// Qt replaces its random part, so they must not write to the same file
	std::vector<std::unique_ptr<Sink>> sinks;
	for (int i = 0; i < s_receiverCount; ++i){
		sinks.push_back(std::make_unique<Sink>(dir.path()));

		QVERIFY2(sinks.back()->Open(), qPrintable(sinks.back()->GetErrorString()));
	}

	QCOMPARE(GetFileNames(dir).size(), s_receiverCount);

	// The bodies are written in turn: if a name is used twice, the second body overwrites the first one
	for (int i = 0; i < s_receiverCount; ++i){
		const QByteArray body = CreateReceiverBody(i);

		QVERIFY(sinks[i]->Write(body.constData(), body.size()));
		QVERIFY(sinks[i]->Finish());
	}

	QSet<QString> paths;
	for (int i = 0; i < s_receiverCount; ++i){
		const QString path = sinks[i]->GetFilePath();

		QVERIFY(QFileInfo(path).fileName().startsWith(QStringLiteral("imtrest-body-")));
		QCOMPARE(ReadFile(path), CreateReceiverBody(i));

		paths.insert(path);
	}

	QCOMPARE(paths.size(), s_receiverCount);
	QCOMPARE(GetFileNames(dir).size(), s_receiverCount);

	// Every receiver deletes its own file only
	sinks.clear();

	QVERIFY(GetFileNames(dir).isEmpty());
}


void CHttpTemporaryFileBodySinkTest::ParallelReceiversUseDifferentFilesTest()
{
	QTemporaryDir dir;
	QVERIFY(dir.isValid());

	// The connections of a server are served in different threads, so the files are created in different threads at the same moment.
	// Every thread writes to its own element of the vector only.
	std::vector<std::unique_ptr<Sink>> sinks(s_receiverCount);

	QSemaphore readySemaphore;
	QSemaphore startSemaphore;

	std::vector<std::unique_ptr<QThread>> threads;
	for (int i = 0; i < s_receiverCount; ++i){
		threads.push_back(std::unique_ptr<QThread>(QThread::create([&, i](){
			readySemaphore.release();
			startSemaphore.acquire();

			std::unique_ptr<Sink> sinkPtr = std::make_unique<Sink>(dir.path());
			const QByteArray body = CreateReceiverBody(i);

			if (sinkPtr->Open() && sinkPtr->Write(body.constData(), body.size()) && sinkPtr->Finish()){
				sinks[i] = std::move(sinkPtr);
			}
		})));

		threads.back()->start();
	}

	readySemaphore.acquire(s_receiverCount);
	startSemaphore.release(s_receiverCount);

	for (const std::unique_ptr<QThread>& threadPtr : threads){
		threadPtr->wait();
	}

	QSet<QString> paths;
	for (int i = 0; i < s_receiverCount; ++i){
		QVERIFY2(sinks[i] != nullptr, qPrintable(QStringLiteral("The receiver %1 has failed").arg(i)));

		const QString path = sinks[i]->GetFilePath();

		QCOMPARE(ReadFile(path), CreateReceiverBody(i));

		paths.insert(path);
	}

	QCOMPARE(paths.size(), s_receiverCount);
	QCOMPARE(GetFileNames(dir).size(), s_receiverCount);

	// The receivers are released in another thread than they were created in, as in a server: the connection creates a request,
	// and the worker manager destroys it
	sinks.clear();

	QVERIFY(GetFileNames(dir).isEmpty());
}


void CHttpTemporaryFileBodySinkTest::BodyInPartsTest()
{
	QTemporaryDir dir;
	QVERIFY(dir.isValid());

	Sink sink(dir.path());
	QVERIFY(sink.Open());

	QVERIFY(sink.Write("hello", 5));
	QVERIFY(sink.Write(" ", 1));
	QVERIFY(sink.Write("world", 5));
	QCOMPARE(sink.GetReceivedSize(), qint64(11));
	QVERIFY(!sink.IsFinished());

	QVERIFY2(sink.Finish(), qPrintable(sink.GetErrorString()));
	QVERIFY(sink.IsFinished());
	QCOMPARE(sink.GetReceivedSize(), qint64(11));

	QCOMPARE(ReadFile(sink.GetFilePath()), QByteArray("hello world"));
}


void CHttpTemporaryFileBodySinkTest::EmptyBodyTest()
{
	QTemporaryDir dir;
	QVERIFY(dir.isValid());

	Sink sink(dir.path());
	QVERIFY(sink.Open());
	QVERIFY(sink.Finish());

	// A request with an empty body is received as well, its file exists and is empty
	QVERIFY(sink.IsFinished());
	QCOMPARE(sink.GetReceivedSize(), qint64(0));
	QVERIFY(QFileInfo::exists(sink.GetFilePath()));
	QCOMPARE(QFileInfo(sink.GetFilePath()).size(), qint64(0));
}


void CHttpTemporaryFileBodySinkTest::BigBodyTest()
{
	QTemporaryDir dir;
	QVERIFY(dir.isValid());

	Sink sink(dir.path());
	QVERIFY(sink.Open());

	for (int i = 0; i < s_partCount; ++i){
		const QByteArray part = CreatePart(i);

		QVERIFY(sink.Write(part.constData(), part.size()));
	}

	QVERIFY(sink.Finish());

	QCOMPARE(sink.GetReceivedSize(), qint64(s_partSize) * s_partCount);
	QCOMPARE(QFileInfo(sink.GetFilePath()).size(), qint64(s_partSize) * s_partCount);
	QCOMPARE(CalculateFileHash(sink.GetFilePath()), CalculateBigBodyHash());
}


void CHttpTemporaryFileBodySinkTest::FileIsPrivateTest()
{
#ifdef Q_OS_UNIX
	QTemporaryDir dir;
	QVERIFY(dir.isValid());

	Sink sink(dir.path());
	QVERIFY(sink.Open());

	// The body of a request can be secret: only the owner of the server reads it
	const QFileDevice::Permissions otherRights = QFileDevice::ReadGroup | QFileDevice::WriteGroup | QFileDevice::ExeGroup
				| QFileDevice::ReadOther | QFileDevice::WriteOther | QFileDevice::ExeOther;

	QCOMPARE(QFileInfo(sink.GetFilePath()).permissions() & otherRights, QFileDevice::Permissions());
#else
	QSKIP("The rights of the files are checked on Unix only");
#endif
}


void CHttpTemporaryFileBodySinkTest::EmptyDirectoryMeansSystemTemporaryDirectoryTest()
{
	Sink sink((QString()));
	QVERIFY2(sink.Open(), qPrintable(sink.GetErrorString()));

	const QString path = sink.GetFilePath();
	QVERIFY(QFileInfo::exists(path));
	QCOMPARE(QFileInfo(path).canonicalPath(), QFileInfo(QDir::tempPath()).canonicalFilePath());

	// The file is not left in the temporary directory of the system
	sink.Abort();

	QVERIFY(!QFileInfo::exists(path));
}


void CHttpTemporaryFileBodySinkTest::FinishedFileIsDeletedWithReceiverTest()
{
	QTemporaryDir dir;
	QVERIFY(dir.isValid());

	QString path;

	{
		Sink sink(dir.path());
		QVERIFY(sink.Open());
		QVERIFY(sink.Write("data", 4));
		QVERIFY(sink.Finish());

		path = sink.GetFilePath();

		// The file is there as long as the handler can use the receiver
		QVERIFY(QFileInfo::exists(path));
	}

	// The handler did not take the file: it is not left on the disk
	QVERIFY(!QFileInfo::exists(path));
	QVERIFY(GetFileNames(dir).isEmpty());
}


void CHttpTemporaryFileBodySinkTest::TakenFileOutlivesReceiverTest()
{
	QTemporaryDir dir;
	QVERIFY(dir.isValid());

	std::unique_ptr<QTemporaryFile> filePtr;
	QString path;

	{
		Sink sink(dir.path());
		QVERIFY(sink.Open());
		QVERIFY(sink.Write("data", 4));
		QVERIFY(sink.Finish());

		path = sink.GetFilePath();

		filePtr = sink.TakeFile();
		QVERIFY(filePtr != nullptr);
	}

	// The file belongs to the one who has taken it: the receiver does not delete it
	QCOMPARE(filePtr->fileName(), path);
	QCOMPARE(ReadFile(path), QByteArray("data"));
	QCOMPARE(GetFileNames(dir).size(), 1);
}


void CHttpTemporaryFileBodySinkTest::TakenFileStateIsNotChangedTest()
{
	QTemporaryDir dir;
	QVERIFY(dir.isValid());

	Sink sink(dir.path());
	QVERIFY(sink.Open());
	QVERIFY(sink.Write("data", 4));
	QVERIFY(sink.Finish());

	const QString path = sink.GetFilePath();

	const std::unique_ptr<QTemporaryFile> filePtr = sink.TakeFile();
	QVERIFY(filePtr != nullptr);

	// It is the state that Finish() has left: the file is closed and it is still removed when the object is destroyed
	QCOMPARE(filePtr->fileName(), path);
	QVERIFY(!filePtr->isOpen());
	QVERIFY(filePtr->autoRemove());
	QCOMPARE(QFileInfo(path).size(), qint64(4));

	// The owner reads the file through the same object
	QVERIFY(filePtr->open());
	QCOMPARE(filePtr->readAll(), QByteArray("data"));
}


void CHttpTemporaryFileBodySinkTest::TakenFileIsDeletedWithItsOwnerTest()
{
	QTemporaryDir dir;
	QVERIFY(dir.isValid());

	std::unique_ptr<QTemporaryFile> filePtr;
	QString path;

	{
		Sink sink(dir.path());
		QVERIFY(sink.Open());
		QVERIFY(sink.Write("data", 4));
		QVERIFY(sink.Finish());

		path = sink.GetFilePath();
		filePtr = sink.TakeFile();
	}

	QVERIFY(QFileInfo::exists(path));

	// The receiver is not responsible for the file any more: it is deleted as soon as its new owner is destroyed
	filePtr.reset();

	QVERIFY(!QFileInfo::exists(path));
	QVERIFY(GetFileNames(dir).isEmpty());
}


void CHttpTemporaryFileBodySinkTest::TakeBeforeFinishReturnsNothingTest()
{
	QTemporaryDir dir;
	QVERIFY(dir.isValid());

	Sink sink(dir.path());

	// There is no file yet
	QVERIFY(sink.TakeFile() == nullptr);

	QVERIFY(sink.Open());
	QVERIFY(sink.TakeFile() == nullptr);

	QVERIFY(sink.Write("part", 4));
	QVERIFY(sink.TakeFile() == nullptr);

	// The refusals change nothing: the body is received to the end and the file with the whole body is taken after that
	QCOMPARE(GetFileNames(dir).size(), 1);
	QVERIFY(sink.Write("ial", 3));
	QVERIFY(sink.Finish());

	const std::unique_ptr<QTemporaryFile> filePtr = sink.TakeFile();
	QVERIFY(filePtr != nullptr);
	QCOMPARE(ReadFile(filePtr->fileName()), QByteArray("partial"));
}


void CHttpTemporaryFileBodySinkTest::TakeAfterAbortReturnsNothingTest()
{
	QTemporaryDir dir;
	QVERIFY(dir.isValid());

	{
		Sink sink(dir.path());
		QVERIFY(sink.Open());
		QVERIFY(sink.Write("partial", 7));

		sink.Abort();

		QVERIFY(sink.TakeFile() == nullptr);
	}

	{
		// Abort() deletes the file of the received body that nobody has taken yet
		Sink sink(dir.path());
		QVERIFY(sink.Open());
		QVERIFY(sink.Write("data", 4));
		QVERIFY(sink.Finish());

		sink.Abort();

		QVERIFY(sink.IsFinished());
		QVERIFY(sink.TakeFile() == nullptr);
		QVERIFY(GetFileNames(dir).isEmpty());
	}
}


void CHttpTemporaryFileBodySinkTest::SecondTakeReturnsNothingTest()
{
	QTemporaryDir dir;
	QVERIFY(dir.isValid());

	Sink sink(dir.path());
	QVERIFY(sink.Open());
	QVERIFY(sink.Write("data", 4));
	QVERIFY(sink.Finish());

	const std::unique_ptr<QTemporaryFile> filePtr = sink.TakeFile();
	QVERIFY(filePtr != nullptr);

	QVERIFY(sink.TakeFile() == nullptr);

	// The second call does not touch the file that is taken
	QCOMPARE(ReadFile(filePtr->fileName()), QByteArray("data"));
}


void CHttpTemporaryFileBodySinkTest::TakenReceiverCannotBeUsedTest()
{
	QTemporaryDir dir;
	QVERIFY(dir.isValid());

	Sink sink(dir.path());
	QVERIFY(sink.Open());
	QVERIFY(sink.Write("data", 4));
	QVERIFY(sink.Finish());

	const std::unique_ptr<QTemporaryFile> filePtr = sink.TakeFile();
	QVERIFY(filePtr != nullptr);

	QVERIFY(sink.GetFilePath().isEmpty());

	// The body is received, the receiver remembers it
	QVERIFY(sink.IsFinished());
	QCOMPARE(sink.GetReceivedSize(), qint64(4));

	// The receiver has no file and does not create a new one instead of the taken file
	QVERIFY(!sink.Write("more", 4));
	QVERIFY(!sink.Finish());
	QVERIFY(!sink.Open());
	QVERIFY(!sink.GetErrorString().isEmpty());

	QCOMPARE(GetFileNames(dir).size(), 1);
	QCOMPARE(ReadFile(filePtr->fileName()), QByteArray("data"));
}


void CHttpTemporaryFileBodySinkTest::AbortDeletesFileTest()
{
	QTemporaryDir dir;
	QVERIFY(dir.isValid());

	Sink sink(dir.path());
	QVERIFY(sink.Open());
	QVERIFY(sink.Write("partial", 7));

	const QString path = sink.GetFilePath();
	QVERIFY(QFileInfo::exists(path));

	sink.Abort();

	QVERIFY(!QFileInfo::exists(path));
	QVERIFY(GetFileNames(dir).isEmpty());
	QVERIFY(sink.GetFilePath().isEmpty());
	QVERIFY(!sink.IsFinished());

	// There is nothing to take
	QVERIFY(sink.TakeFile() == nullptr);

	// The repeated call does nothing
	sink.Abort();

	QVERIFY(GetFileNames(dir).isEmpty());
}


void CHttpTemporaryFileBodySinkTest::AbortAfterTakeKeepsFileTest()
{
	QTemporaryDir dir;
	QVERIFY(dir.isValid());

	Sink sink(dir.path());
	QVERIFY(sink.Open());
	QVERIFY(sink.Write("data", 4));
	QVERIFY(sink.Finish());

	const std::unique_ptr<QTemporaryFile> filePtr = sink.TakeFile();
	QVERIFY(filePtr != nullptr);

	// The body is received and the file is taken, a late call does not delete somebody's file
	sink.Abort();

	QCOMPARE(ReadFile(filePtr->fileName()), QByteArray("data"));
	QCOMPARE(GetFileNames(dir).size(), 1);
}


void CHttpTemporaryFileBodySinkTest::DestructionWithoutFinishDeletesFileTest()
{
	QTemporaryDir dir;
	QVERIFY(dir.isValid());

	{
		Sink sink(dir.path());
		QVERIFY(sink.Open());
		QVERIFY(sink.Write("partial", 7));

		QCOMPARE(GetFileNames(dir).size(), 1);
	}

	QVERIFY(GetFileNames(dir).isEmpty());
}


void CHttpTemporaryFileBodySinkTest::OpenInMissingDirectoryFailsTest()
{
	QTemporaryDir dir;
	QVERIFY(dir.isValid());

	const QString missingDirectory = dir.filePath("missing");

	Sink sink(missingDirectory);

	QVERIFY(!sink.Open());
	QVERIFY(!sink.GetErrorString().isEmpty());
	QVERIFY(sink.GetFilePath().isEmpty());

	// Nothing is created instead of the directory
	QVERIFY(!QFileInfo::exists(missingDirectory));
	QVERIFY(GetFileNames(dir).isEmpty());
}


void CHttpTemporaryFileBodySinkTest::UseBeforeOpenFailsTest()
{
	QTemporaryDir dir;
	QVERIFY(dir.isValid());

	Sink sink(dir.path());

	QVERIFY(!sink.Write("data", 4));
	QVERIFY(!sink.GetErrorString().isEmpty());
	QVERIFY(!sink.Finish());
	QVERIFY(!sink.IsFinished());

	// The receiver that is not opened has nothing to delete
	sink.Abort();

	QVERIFY(GetFileNames(dir).isEmpty());
}


void CHttpTemporaryFileBodySinkTest::RepeatedOpenTest()
{
	QTemporaryDir dir;
	QVERIFY(dir.isValid());

	Sink sink(dir.path());
	QVERIFY(sink.Open());

	const QString path = sink.GetFilePath();
	QVERIFY(sink.Open());

	QCOMPARE(sink.GetFilePath(), path);
	QCOMPARE(GetFileNames(dir).size(), 1);
}


void CHttpTemporaryFileBodySinkTest::UseAfterAbortFailsTest()
{
	QTemporaryDir dir;
	QVERIFY(dir.isValid());

	Sink sink(dir.path());
	QVERIFY(sink.Open());

	sink.Abort();

	QVERIFY(!sink.Open());
	QVERIFY(!sink.Write("data", 4));
	QVERIFY(!sink.Finish());
	QVERIFY(GetFileNames(dir).isEmpty());
}


void CHttpTemporaryFileBodySinkTest::RequestWritesBodyToFileTest()
{
	QTemporaryDir dir;
	QVERIFY(dir.isValid());

	CRequestServletStub servlet;
	CProtocolEngineStub engine;

	std::shared_ptr<Sink> sinkPtr = std::make_shared<Sink>(dir.path());
	QVERIFY(sinkPtr->Open());

	{
		imtrest::CHttpRequest request(servlet, engine);

		CSinkPolicyStub policy(sinkPtr);
		request.SetBodyPolicy(policy);

		QVERIFY(request.ExecuteHttpParser(CreateHead(QByteArray::number(qint64(s_partSize) * s_partCount)), nullptr));

		for (int i = 0; i < s_partCount; ++i){
			QVERIFY(request.ExecuteHttpParser(CreatePart(i), nullptr));

			// The body is not kept in the request
			QVERIFY(request.GetBody().isEmpty());
		}

		QCOMPARE(request.GetState(), IRequest::RS_MESSAGE_COMPLETE);
		QVERIFY(sinkPtr->IsFinished());

		// The handler of the request finds the file through the request
		QVERIFY(std::dynamic_pointer_cast<Sink>(request.GetBodySink()) == sinkPtr);
		QCOMPARE(sinkPtr->GetReceivedSize(), qint64(s_partSize) * s_partCount);
		QCOMPARE(CalculateFileHash(sinkPtr->GetFilePath()), CalculateBigBodyHash());
	}

	// The request is handled and destroyed, the receiver is held by the test only
	QCOMPARE(GetFileNames(dir).size(), 1);

	sinkPtr.reset();

	QVERIFY(GetFileNames(dir).isEmpty());
}


void CHttpTemporaryFileBodySinkTest::ChunkedRequestWritesBodyToFileTest()
{
	QTemporaryDir dir;
	QVERIFY(dir.isValid());

	CRequestServletStub servlet;
	CProtocolEngineStub engine;
	imtrest::CHttpRequest request(servlet, engine);

	const std::shared_ptr<Sink> sinkPtr = std::make_shared<Sink>(dir.path());
	QVERIFY(sinkPtr->Open());

	CSinkPolicyStub policy(sinkPtr);
	request.SetBodyPolicy(policy);

	const QByteArray body(
				"5\r\n"
				"hello\r\n"
				"6\r\n"
				" world\r\n"
				"0\r\n"
				"\r\n");

	QVERIFY(request.ExecuteHttpParser(CreateHead(QByteArray(), "Transfer-Encoding: chunked\r\n") + body, nullptr));

	QCOMPARE(request.GetState(), IRequest::RS_MESSAGE_COMPLETE);
	QVERIFY(sinkPtr->IsFinished());
	QCOMPARE(ReadFile(sinkPtr->GetFilePath()), QByteArray("hello world"));
}


void CHttpTemporaryFileBodySinkTest::TakenFileOutlivesRequestTest()
{
	QTemporaryDir dir;
	QVERIFY(dir.isValid());

	CRequestServletStub servlet;
	CProtocolEngineStub engine;

	std::unique_ptr<QTemporaryFile> filePtr;

	{
		imtrest::CHttpRequest request(servlet, engine);

		const std::shared_ptr<Sink> sinkPtr = std::make_shared<Sink>(dir.path());
		QVERIFY(sinkPtr->Open());

		CSinkPolicyStub policy(sinkPtr);
		request.SetBodyPolicy(policy);

		QVERIFY(request.ExecuteHttpParser(CreateHead("4") + "data", nullptr));
		QCOMPARE(request.GetState(), IRequest::RS_MESSAGE_COMPLETE);

		// The handler finds the receiver through the request and takes the file
		const std::shared_ptr<Sink> requestSinkPtr = std::dynamic_pointer_cast<Sink>(request.GetBodySink());
		QVERIFY(requestSinkPtr != nullptr);

		filePtr = requestSinkPtr->TakeFile();
		QVERIFY(filePtr != nullptr);
	}

	// The request and the receiver are destroyed, the file belongs to the handler
	QCOMPARE(ReadFile(filePtr->fileName()), QByteArray("data"));
	QCOMPARE(GetFileNames(dir).size(), 1);

	filePtr.reset();

	QVERIFY(GetFileNames(dir).isEmpty());
}


void CHttpTemporaryFileBodySinkTest::BrokenRequestLeavesNoFileTest()
{
	QTemporaryDir dir;
	QVERIFY(dir.isValid());

	CRequestServletStub servlet;
	CProtocolEngineStub engine;

	// The test holds the receiver after the request: the request releases the file by itself
	const std::shared_ptr<Sink> sinkPtr = std::make_shared<Sink>(dir.path());
	QVERIFY(sinkPtr->Open());

	CSinkPolicyStub policy(sinkPtr);

	{
		imtrest::CHttpRequest request(servlet, engine);
		request.SetBodyPolicy(policy);

		// The connection is broken in the middle of the body
		QVERIFY(request.ExecuteHttpParser(CreateHead("1000") + QByteArray(400, 'x'), nullptr));

		QCOMPARE(GetFileNames(dir).size(), 1);
	}

	QVERIFY(GetFileNames(dir).isEmpty());
	QVERIFY(!sinkPtr->IsFinished());
}


void CHttpTemporaryFileBodySinkTest::RefusedRequestLeavesNoFileTest()
{
	QTemporaryDir dir;
	QVERIFY(dir.isValid());

	CRequestServletStub servlet;
	CProtocolEngineStub engine;
	imtrest::CHttpRequest request(servlet, engine);

	const std::shared_ptr<Sink> sinkPtr = std::make_shared<Sink>(dir.path());
	QVERIFY(sinkPtr->Open());

	CSinkPolicyStub policy(sinkPtr);
	request.SetBodyPolicy(policy);
	request.SetDefaultMaxBodySize(100);

	// The body is bigger than the limit, the file is deleted at once, before the body is sent
	QVERIFY(request.ExecuteHttpParser(CreateHead("1000"), nullptr));

	QVERIFY(GetFileNames(dir).isEmpty());
	QVERIFY(!sinkPtr->IsFinished());
}


I_ADD_TEST(CHttpTemporaryFileBodySinkTest);


