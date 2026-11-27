// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include "CFileDownloaderTest.h"
#include "TSimModelComponentPtr.h"


// STL includes
#include <atomic>
#include <memory>

// Qt includes
#include <QtCore/QCryptographicHash>
#include <QtCore/QElapsedTimer>
#include <QtCore/QFile>
#include <QtCore/QMutex>
#include <QtCore/QTemporaryDir>
#include <QtCore/QThread>
#include <QtNetwork/QTcpServer>

// ACF includes
#include <icomp/TSimComponentWrap.h>
#include <itest/CStandardTestExecutor.h>

// ImtCore includes
#include <imtcom/CFileDownloader.h>
#include <imtcom/CServerConnectionInterfaceParamComp.h>
#include <imtfile/IFileDownloadLinkProvider.h>
#include <imtrest/CHttpFileServletComp.h>
#include <imtrest/CHttpProtocolEngineComp.h>
#include <imtrest/CHttpRequest.h>
#include <imtrest/CTcpServerComp.h>
#include <imtrest/IHttpFileResolver.h>
#include <imtrest/IRequestServlet.h>
#include <imtrest/IResponseDispatcher.h>


namespace
{


typedef imtfile::IFileDownloadLinkProvider::DownloadLink DownloadLink;
typedef imtcom::CFileDownloader::Result Result;


const qint64 s_mebibyte = Q_INT64_C(1024) * 1024;
const qint64 s_fileSize = 3 * s_mebibyte;
const qint64 s_partSize = s_mebibyte;
const QString s_remotePath = QStringLiteral("/versions/1.0/setup.bin");


/**
	Data of the file: every byte depends on its position and the version, so lost, repeated and moved parts are found.
*/
QByteArray CreateContent(qint64 size, int version)
{
	QByteArray retVal(static_cast<qsizetype>(size), Qt::Uninitialized);
	const qsizetype versionOffset = static_cast<qsizetype>(version) * 101;
	for (qsizetype i = 0; i < retVal.size(); ++i){
		retVal[i] = static_cast<char>(((i * 31) + versionOffset + 7) & 0xFF);
	}

	return retVal;
}


QString GetSha256(const QByteArray& data)
{
	return QString::fromLatin1(QCryptographicHash::hash(data, QCryptographicHash::Sha256).toHex());
}


bool WriteFile(const QString& filePath, const QByteArray& data)
{
	QFile file(filePath);

	return file.open(QIODevice::WriteOnly | QIODevice::Truncate) && file.write(data) == data.size();
}


QByteArray ReadFile(const QString& filePath)
{
	QFile file(filePath);

	return file.open(QIODevice::ReadOnly) ? file.readAll() : QByteArray();
}


int FindFreePort()
{
	QTcpServer server;
	if (!server.listen(QHostAddress::LocalHost, 0)){
		return 0;
	}

	return server.serverPort();
}


// Gives the file of the test for every link, except the refused ones, and keeps what the requests asked for
class CTestFileResolverComp: public icomp::CComponentBase, virtual public imtrest::IHttpFileResolver
{
public:
	typedef icomp::CComponentBase BaseClass;
	I_BEGIN_COMPONENT(CTestFileResolverComp)
		I_REGISTER_INTERFACE(imtrest::IHttpFileResolver);
	I_END_COMPONENT

	struct ReceivedRequest
	{
		QByteArray token;
		QByteArray range;
		QByteArray ifRange;
	};

	void SetFile(const QString& filePath, const QByteArray& entityTag)
	{
		QMutexLocker locker(&m_mutex);
		m_filePath = filePath;
		m_entityTag = entityTag;
	}

	void SetRefusingAll(bool isRefusingAll)
	{
		QMutexLocker locker(&m_mutex);
		m_isRefusingAll = isRefusingAll;
	}

	void RefuseToken(const QByteArray& token)
	{
		QMutexLocker locker(&m_mutex);
		m_refusedTokens.append(token);
	}

	QList<ReceivedRequest> GetRequests() const
	{
		QMutexLocker locker(&m_mutex);

		return m_requests;
	}

	// reimplemented (imtrest::IHttpFileResolver)
	imtrest::IProtocolEngine::StatusCode ResolveFile(const imtrest::CHttpRequest& request, FileDescription& file) const override
	{
		ReceivedRequest receivedRequest;
		receivedRequest.token = request.GetCommandId().split('/').last();
		receivedRequest.range = request.GetHeaderValue("range");
		receivedRequest.ifRange = request.GetHeaderValue("if-range");

		QMutexLocker locker(&m_mutex);
		m_requests.append(receivedRequest);
		if (m_isRefusingAll || m_refusedTokens.contains(receivedRequest.token)){
			return imtrest::IProtocolEngine::SC_UNAUTHORIZED;
		}

		file.filePath = m_filePath;
		file.eTag = m_entityTag;

		return imtrest::IProtocolEngine::SC_OK;
	}

private:
	mutable QMutex m_mutex;
	QString m_filePath;
	QByteArray m_entityTag;
	bool m_isRefusingAll = false;
	QByteArrayList m_refusedTokens;
	mutable QList<ReceivedRequest> m_requests;
};


// Gives the links in the order of the list, the last one again and again; a link without a URL gets "<base>/files/token-<N>"
class CTestLinkProvider: virtual public imtfile::IFileDownloadLinkProvider
{
public:
	QString baseUrl;
	QList<DownloadLink> links;
	QString failure;

	int GetRequestCount() const
	{
		QMutexLocker locker(&m_mutex);

		return m_requestCount;
	}

	QString GetLastPath() const
	{
		QMutexLocker locker(&m_mutex);

		return m_lastPath;
	}

	// reimplemented (imtfile::IFileDownloadLinkProvider)
	bool GetDownloadLink(const QString& path, DownloadLink& link, QString& error) const override
	{
		QMutexLocker locker(&m_mutex);
		++m_requestCount;
		m_lastPath = path;

		if (!failure.isEmpty()){
			error = failure;

			return false;
		}

		link = links.value(qMin(m_requestCount, static_cast<int>(links.size())) - 1);
		if (link.url.isEmpty()){
			link.url = QStringLiteral("%1/files/token-%2").arg(baseUrl).arg(m_requestCount);
		}

		return true;
	}

private:
	mutable QMutex m_mutex;
	mutable int m_requestCount = 0;
	mutable QString m_lastPath;
};


// Answers a request with the response of the file servlet and frees it, as the worker manager of an application does
class CRespondingServletComp: public icomp::CComponentBase, virtual public imtrest::IRequestServlet
{
public:
	typedef icomp::CComponentBase BaseClass;
	I_BEGIN_COMPONENT(CRespondingServletComp)
		I_REGISTER_INTERFACE(imtrest::IRequestServlet);
	I_END_COMPONENT

	const imtrest::IRequestServlet* servletPtr = nullptr;
	imtrest::IResponseDispatcher* dispatcherPtr = nullptr;

	// reimplemented (imtrest::IRequestServlet)
	bool IsCommandSupported(const QByteArray& /*commandId*/) const override
	{
		return true;
	}

	imtrest::ConstResponsePtr ProcessRequest(const imtrest::IRequest& request, const QByteArray& subCommandId) const override
	{
		imtrest::ConstResponsePtr responsePtr = servletPtr->ProcessRequest(request, subCommandId);
		if (responsePtr.IsValid()){
			dispatcherPtr->SendResponse(request.GetRequestId(), responsePtr);
		}

		// The request belongs to the handler since the call
		delete &request;

		return {};
	}
};


/**
	The file server of the real components: the TCP server, the HTTP engine and the file servlet. It accepts the connections in the
	thread of the test.
*/
struct CFileServer
{
	icomp::TSimSharedComponentPtr<CTestFileResolverComp> resolver;
	icomp::TSimSharedComponentPtr<imtrest::CHttpFileServletComp> servlet;
	icomp::TSimSharedComponentPtr<CRespondingServletComp> handler;
	icomp::TSimSharedComponentPtr<imtrest::CHttpProtocolEngineComp> engine;
	TSimModelComponentPtr<imtcom::CServerConnectionInterfaceParamComp> connection;
	// Destroyed first: its threads go before the components they use
	icomp::TSimComponentWrap<imtrest::CTcpServerComp> server;
	int port = 0;

	CFileServer()
	{
		port = FindFreePort();

		resolver->InitComponent();
		servlet->SetRef("FileResolver", resolver);
		servlet->SetIdAttr("CommandId", "files/*");
		servlet->InitComponent();
		handler->servletPtr = &*servlet;
		handler->dispatcherPtr = &server;
		handler->InitComponent();
		engine->InitComponent();
		connection->SetIntAttr("DefaultHttpPort", port);
		connection->InitComponent();
		server.SetIntAttr("ThreadsLimit", 4);
		server.SetBoolAttr("StartServerOnCreate", true);
		server.SetRef("RequestHandler", handler);
		server.SetRef("ProtocolEngine", engine);
		server.SetRef("ServerInterface", connection);
		server.InitComponent();
	}

	QString GetBaseUrl() const
	{
		return QStringLiteral("http://127.0.0.1:%1").arg(port);
	}
};


struct CDownloadFixture
{
	QTemporaryDir directory;
	CFileServer server;
	CTestLinkProvider links;
	imtcom::CFileDownloader downloader{links};
	QByteArray content = CreateContent(s_fileSize, 1);
	QString sourcePath;
	QString filePath;
	QString partFilePath;
	QString entityTagFilePath;

	QMutex progressMutex;
	QList<QPair<qint64, qint64>> progress;

	CDownloadFixture()
	{
		sourcePath = directory.filePath(QStringLiteral("source-1.bin"));
		filePath = directory.filePath(QStringLiteral("setup.bin"));
		partFilePath = filePath + QStringLiteral(".part");
		entityTagFilePath = partFilePath + QStringLiteral(".etag");

		WriteFile(sourcePath, content);
		server.resolver->SetFile(sourcePath, QByteArrayLiteral("\"v1\""));
		links.baseUrl = server.GetBaseUrl();
		links.links.append(CreateLink(content, QStringLiteral("\"v1\"")));
	}

	static DownloadLink CreateLink(const QByteArray& data, const QString& entityTag)
	{
		DownloadLink link;
		link.size = data.size();
		link.sha256 = GetSha256(data);
		link.etag = entityTag;

		return link;
	}

	// Leaves a part file of the given bytes, of the given entity tag
	void CreatePartFile(const QByteArray& data, const QByteArray& entityTag) const
	{
		WriteFile(partFilePath, data);
		if (!entityTag.isEmpty()){
			WriteFile(entityTagFilePath, entityTag);
		}
	}

	// The download runs in a thread of its own, the server answers in this one
	Result Download(QString& error, const std::atomic<bool>* isCanceledPtr = nullptr)
	{
		std::atomic<bool> isNotCanceled{false};
		const std::atomic<bool>& isCanceled = (isCanceledPtr != nullptr) ? *isCanceledPtr : isNotCanceled;

		Result result = Result::Failed;
		const imtcom::CFileDownloader::ProgressFunction progressFunction = [this](qint64 receivedBytes, qint64 totalBytes){
			QMutexLocker locker(&progressMutex);
			progress.append(qMakePair(receivedBytes, totalBytes));
		};

		std::unique_ptr<QThread> threadPtr(QThread::create([this, &result, &isCanceled, &progressFunction, &error](){
			result = downloader.Download(s_remotePath, filePath, isCanceled, progressFunction, error);
		}));
		threadPtr->start();
		while (!threadPtr->wait(10)){
			QCoreApplication::processEvents();
		}

		return result;
	}

	QList<CTestFileResolverComp::ReceivedRequest> GetRequests()
	{
		return server.resolver->GetRequests();
	}
};


} // namespace


// The file is downloaded and replaces the one that was there; nothing else stays
void CFileDownloaderTest::DownloadsTheFileTest()
{
	CDownloadFixture fixture;
	QVERIFY(WriteFile(fixture.filePath, QByteArrayLiteral("the previous version")));

	QString error;
	QCOMPARE(fixture.Download(error), Result::Completed);
	QVERIFY2(error.isEmpty(), qPrintable(error));
	QVERIFY(ReadFile(fixture.filePath) == fixture.content);
	QVERIFY(!QFile::exists(fixture.partFilePath));
	QVERIFY(!QFile::exists(fixture.entityTagFilePath));

	const QList<CTestFileResolverComp::ReceivedRequest> requests = fixture.GetRequests();
	QCOMPARE(requests.size(), 1);
	QVERIFY(requests[0].range.isEmpty());
	QCOMPARE(fixture.links.GetRequestCount(), 1);
	QCOMPARE(fixture.links.GetLastPath(), s_remotePath);

	QVERIFY(!fixture.progress.isEmpty());
	QCOMPARE(fixture.progress.last(), qMakePair(s_fileSize, s_fileSize));
}


// A part file of the entity tag of the link is continued from its end
void CFileDownloaderTest::PartFileIsContinuedTest()
{
	CDownloadFixture fixture;
	fixture.CreatePartFile(fixture.content.left(s_partSize), QByteArrayLiteral("\"v1\""));

	QString error;
	QCOMPARE(fixture.Download(error), Result::Completed);
	QVERIFY(ReadFile(fixture.filePath) == fixture.content);
	QVERIFY(!QFile::exists(fixture.partFilePath));
	QVERIFY(!QFile::exists(fixture.entityTagFilePath));

	const QList<CTestFileResolverComp::ReceivedRequest> requests = fixture.GetRequests();
	QCOMPARE(requests.size(), 1);
	QCOMPARE(requests[0].range, QByteArray("bytes=") + QByteArray::number(s_partSize) + '-');
	QCOMPARE(requests[0].ifRange, QByteArrayLiteral("\"v1\""));
}


// A part file of another entity tag is of other bytes: the download starts anew
void CFileDownloaderTest::PartFileOfAnotherVersionStartsAnewTest()
{
	CDownloadFixture fixture;
	fixture.CreatePartFile(CreateContent(s_partSize, 0), QByteArrayLiteral("\"v0\""));

	QString error;
	QCOMPARE(fixture.Download(error), Result::Completed);
	QVERIFY(ReadFile(fixture.filePath) == fixture.content);

	const QList<CTestFileResolverComp::ReceivedRequest> requests = fixture.GetRequests();
	QCOMPARE(requests.size(), 1);
	QVERIFY(requests[0].range.isEmpty());
}


// Without an entity tag a part file cannot be continued: the download starts anew and keeps no entity tag
void CFileDownloaderTest::LinkWithoutEntityTagStartsAnewTest()
{
	CDownloadFixture fixture;
	fixture.links.links = {CDownloadFixture::CreateLink(fixture.content, QString())};
	fixture.CreatePartFile(fixture.content.left(s_partSize), QByteArray());

	QString error;
	QCOMPARE(fixture.Download(error), Result::Completed);
	QVERIFY(ReadFile(fixture.filePath) == fixture.content);
	QVERIFY(!QFile::exists(fixture.entityTagFilePath));

	const QList<CTestFileResolverComp::ReceivedRequest> requests = fixture.GetRequests();
	QCOMPARE(requests.size(), 1);
	QVERIFY(requests[0].range.isEmpty());
	QVERIFY(requests[0].ifRange.isEmpty());
}


// The file has changed since the link was given: the answer of another entity tag takes a new link, and the new version is downloaded
void CFileDownloaderTest::ChangedFileIsDownloadedWithANewLinkTest()
{
	CDownloadFixture fixture;
	fixture.CreatePartFile(fixture.content.left(s_partSize), QByteArrayLiteral("\"v1\""));

	const QByteArray newContent = CreateContent(s_fileSize + s_partSize, 2);
	const QString newSourcePath = fixture.directory.filePath(QStringLiteral("source-2.bin"));
	QVERIFY(WriteFile(newSourcePath, newContent));
	fixture.server.resolver->SetFile(newSourcePath, QByteArrayLiteral("\"v2\""));
	fixture.links.links.append(CDownloadFixture::CreateLink(newContent, QStringLiteral("\"v2\"")));

	QString error;
	QCOMPARE(fixture.Download(error), Result::Completed);
	QVERIFY(ReadFile(fixture.filePath) == newContent);
	QCOMPARE(fixture.links.GetRequestCount(), 2);

	const QList<CTestFileResolverComp::ReceivedRequest> requests = fixture.GetRequests();
	QCOMPARE(requests.size(), 2);
	QCOMPARE(requests[0].ifRange, QByteArrayLiteral("\"v1\""));
	QVERIFY(requests[1].range.isEmpty());
}


// A refused link is replaced by a new one
void CFileDownloaderTest::RefusedLinkIsRenewedTest()
{
	CDownloadFixture fixture;
	fixture.server.resolver->RefuseToken(QByteArrayLiteral("token-1"));

	QString error;
	QCOMPARE(fixture.Download(error), Result::Completed);
	QVERIFY(ReadFile(fixture.filePath) == fixture.content);
	QCOMPARE(fixture.links.GetRequestCount(), 2);

	const QList<CTestFileResolverComp::ReceivedRequest> requests = fixture.GetRequests();
	QCOMPARE(requests.size(), 2);
	QCOMPARE(requests[1].token, QByteArrayLiteral("token-2"));
}


// A download takes not more new links than allowed
void CFileDownloaderTest::LinkRenewalsAreLimitedTest()
{
	CDownloadFixture fixture;
	fixture.server.resolver->SetRefusingAll(true);

	QString error;
	QCOMPARE(fixture.Download(error), Result::Failed);
	QVERIFY2(error.contains(QStringLiteral("401")), qPrintable(error));
	QCOMPARE(fixture.links.GetRequestCount(), 1 + imtcom::CFileDownloader::s_defaultMaxLinkRenewals);
	QVERIFY(!QFile::exists(fixture.filePath));
}


// No link is no download
void CFileDownloaderTest::FailedLinkFailsTheDownloadTest()
{
	CDownloadFixture fixture;
	fixture.links.failure = QStringLiteral("The file does not exist");

	QString error;
	QCOMPARE(fixture.Download(error), Result::Failed);
	QCOMPARE(error, QStringLiteral("The file does not exist"));
	QVERIFY(fixture.GetRequests().isEmpty());
	QVERIFY(!QFile::exists(fixture.partFilePath));
}


// Bytes of another SHA-256 than the link says are not kept: the next download starts anew
void CFileDownloaderTest::WrongChecksumDiscardsThePartTest()
{
	CDownloadFixture fixture;
	fixture.links.links[0].sha256 = GetSha256(QByteArrayLiteral("other bytes"));

	QString error;
	QCOMPARE(fixture.Download(error), Result::Failed);
	QVERIFY2(error.contains(QStringLiteral("SHA-256")), qPrintable(error));
	QVERIFY(!QFile::exists(fixture.filePath));
	QVERIFY(!QFile::exists(fixture.partFilePath));
	QVERIFY(!QFile::exists(fixture.entityTagFilePath));
}


// More bytes than the size of the link are not taken
void CFileDownloaderTest::OversizedFileDiscardsThePartTest()
{
	CDownloadFixture fixture;
	fixture.links.links[0].size = s_partSize;

	QString error;
	QCOMPARE(fixture.Download(error), Result::Failed);
	QVERIFY(!error.isEmpty());
	QVERIFY(!QFile::exists(fixture.filePath));
	QVERIFY(!QFile::exists(fixture.partFilePath));
}


// An answer that ends before the size of the link is no file: the part file stays for the next attempt
void CFileDownloaderTest::ShortAnswerKeepsThePartTest()
{
	CDownloadFixture fixture;
	fixture.links.links[0].size = s_fileSize + 1;

	QString error;
	QCOMPARE(fixture.Download(error), Result::Failed);
	QVERIFY2(error.contains(QStringLiteral("ended after")), qPrintable(error));
	QVERIFY(!QFile::exists(fixture.filePath));
	QVERIFY(ReadFile(fixture.partFilePath) == fixture.content);
	QVERIFY(QFile::exists(fixture.entityTagFilePath));
}


// A part file longer than the file on the server (the link gives no size) gets 416: the download starts anew
void CFileDownloaderTest::TooLongPartFileStartsAnewTest()
{
	CDownloadFixture fixture;
	fixture.links.links[0].size = -1;
	fixture.CreatePartFile(fixture.content + CreateContent(s_partSize, 0), QByteArrayLiteral("\"v1\""));

	QString error;
	QCOMPARE(fixture.Download(error), Result::Completed);
	QVERIFY(ReadFile(fixture.filePath) == fixture.content);

	const QList<CTestFileResolverComp::ReceivedRequest> requests = fixture.GetRequests();
	QCOMPARE(requests.size(), 2);
	QCOMPARE(requests[0].range, QByteArray("bytes=") + QByteArray::number(s_fileSize + s_partSize) + '-');
	QVERIFY(requests[1].range.isEmpty());
}


// A URL without a server is replaced by the next URL of the link
void CFileDownloaderTest::AlternativeUrlIsTriedTest()
{
	CDownloadFixture fixture;
	fixture.links.links[0].url = QStringLiteral("http://127.0.0.1:%1/files/token-unreachable").arg(FindFreePort());
	fixture.links.links[0].alternativeUrls = QStringList{fixture.server.GetBaseUrl() + QStringLiteral("/files/token-mirror")};

	QString error;
	QCOMPARE(fixture.Download(error), Result::Completed);
	QVERIFY(ReadFile(fixture.filePath) == fixture.content);
	QCOMPARE(fixture.links.GetRequestCount(), 1);

	const QList<CTestFileResolverComp::ReceivedRequest> requests = fixture.GetRequests();
	QCOMPARE(requests.size(), 1);
	QCOMPARE(requests[0].token, QByteArrayLiteral("token-mirror"));
}


// A connection that brings no data is given up after the idle timeout; the part file stays
void CFileDownloaderTest::SilentServerTimesOutTest()
{
	CDownloadFixture fixture;
	QTcpServer silentServer;
	QVERIFY(silentServer.listen(QHostAddress::LocalHost, 0));
	fixture.links.links[0].url = QStringLiteral("http://127.0.0.1:%1/files/token-silent").arg(silentServer.serverPort());
	fixture.downloader.SetIdleTimeout(300);

	QElapsedTimer timer;
	timer.start();
	QString error;
	QCOMPARE(fixture.Download(error), Result::Failed);
	QVERIFY2(error.contains(QStringLiteral("No data")), qPrintable(error));
	QVERIFY(timer.elapsed() < 5000);
	QVERIFY(QFile::exists(fixture.entityTagFilePath));
}


// A canceled download stops without waiting for the data and keeps its part file
void CFileDownloaderTest::CancelStopsTheDownloadTest()
{
	CDownloadFixture fixture;
	QTcpServer silentServer;
	QVERIFY(silentServer.listen(QHostAddress::LocalHost, 0));
	fixture.links.links[0].url = QStringLiteral("http://127.0.0.1:%1/files/token-silent").arg(silentServer.serverPort());

	std::atomic<bool> isCanceled{false};
	QTimer::singleShot(200, &silentServer, [&isCanceled](){
		isCanceled = true;
	});

	QElapsedTimer timer;
	timer.start();
	QString error;
	QCOMPARE(fixture.Download(error, &isCanceled), Result::Canceled);
	QVERIFY(timer.elapsed() < 5000);
	QVERIFY(!QFile::exists(fixture.filePath));
	QVERIFY(QFile::exists(fixture.entityTagFilePath));
}


I_ADD_TEST(CFileDownloaderTest);
