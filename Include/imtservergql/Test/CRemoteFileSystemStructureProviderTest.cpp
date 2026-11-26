// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include "CRemoteFileSystemStructureProviderTest.h"


#include <QtCore/QJsonArray>
#include <QtCore/QJsonDocument>
#include <QtCore/QJsonObject>
#include <QtCore/QTimeZone>
#include <icomp/TSimComponentWrap.h>
#include <itest/CStandardTestExecutor.h>
#include <imtclientgql/CRemoteFileSystemStructureProviderComp.h>
#include <imtclientgql/IRemoteSession.h>
#include <imtgql/CGqlContext.h>
#include <imtgql/CGqlRequest.h>
#include <imtgql/CGqlRequestContextManager.h>
#include <imtgql/CGqlResponse.h>

#include "CFileSystemControllerTest.h"


namespace
{


// Sends each request as query text to a handler in this process and wraps the answer like the HTTP GraphQL servlet.
// The tokens of deniedTokens are refused like a server refuses them (HTTP 401).
class CLoopbackGqlClientComp: public icomp::CComponentBase, virtual public imtclientgql::IGqlClient
{
public:
	typedef icomp::CComponentBase BaseClass;
	I_BEGIN_COMPONENT(CLoopbackGqlClientComp)
		I_REGISTER_INTERFACE(imtclientgql::IGqlClient);
	I_END_COMPONENT

	const imtgql::IGqlRequestHandler* handlerPtr = nullptr;
	QByteArrayList deniedTokens;
	mutable QByteArray lastQuery;
	mutable QByteArrayList sentTokens;
	mutable QByteArrayList sentProductIds;

	GqlResult SendRequest(GqlRequestPtr requestPtr, imtbase::IUrlParam* /*urlParamPtr*/) const override
	{
		const imtgql::IGqlContext* contextPtr = requestPtr->GetRequestContext();
		const QByteArray token = contextPtr != nullptr ? contextPtr->GetToken() : QByteArray();
		sentTokens.append(token);
		sentProductIds.append(contextPtr != nullptr ? contextPtr->GetProductId() : QByteArray());
		if (deniedTokens.contains(token)){
			return GqlResult{{}, EC_ACCESS_DENIED, QStringLiteral("Unauthorized")};
		}

		lastQuery = requestPtr->GetQuery();

		imtgql::CGqlRequest serverRequest;
		qsizetype errorPosition = -1;
		if (!serverRequest.ParseQuery(lastQuery, errorPosition)){
			return GqlResult{{}, EC_INVALID_REQUEST, QStringLiteral("The query cannot be parsed")};
		}

		const QString commandId = QString::fromUtf8(serverRequest.GetCommandId());
		QString errorMessage;
		const QJsonObject handlerResponse = handlerPtr->CreateResponse(serverRequest, errorMessage);
		QJsonObject root;
		if (handlerResponse.isEmpty()){
			const QJsonObject errorObject{{QStringLiteral("message"), errorMessage}, {QStringLiteral("path"), QJsonArray{commandId}}};
			root.insert(QStringLiteral("errors"), QJsonArray{errorObject});
		}
		else{
			root.insert(QStringLiteral("data"), QJsonObject{{commandId, handlerResponse.value(QStringLiteral("data"))}});
		}

		auto* responsePtr = new imtgql::CGqlResponse(requestPtr);
		responsePtr->SetResponseData(QJsonDocument(root).toJson(QJsonDocument::Compact));
		GqlResponsePtr retVal;
		retVal.SetPtr(responsePtr);

		return GqlResult{retVal, EC_NONE, {}};
	}

	bool SendRequestNoWait(GqlRequestPtr /*requestPtr*/, imtbase::IUrlParam* /*urlParamPtr*/) const override
	{
		return false;
	}
};


// Gives out a context with the next token of the script, or the sign-in error; records the refused tokens
class CScriptedSessionComp: public icomp::CComponentBase, virtual public imtclientgql::IRemoteSession
{
public:
	typedef icomp::CComponentBase BaseClass;
	I_BEGIN_COMPONENT(CScriptedSessionComp)
		I_REGISTER_INTERFACE(imtclientgql::IRemoteSession);
	I_END_COMPONENT

	QByteArrayList tokens;
	QString signInError;
	QByteArrayList rejectedTokens;

	imtgql::IGqlContextSharedPtr CreateRequestContext(QString& error) override
	{
		if (!signInError.isEmpty()){
			error = signInError;

			return imtgql::IGqlContextSharedPtr();
		}

		imtgql::IGqlContextUniquePtr contextPtr(new imtgql::CGqlContext);
		contextPtr->SetToken(tokens.isEmpty() ? QByteArray() : tokens.takeFirst());
		contextPtr->SetProductId(QByteArrayLiteral("RemoteProduct"));

		return imtgql::IGqlContextSharedPtr::CreateFromUnique(std::move(contextPtr));
	}

	void OnTokenRejected(const QByteArray& token) override
	{
		rejectedTokens.append(token);
	}
};


// The remote provider, connected through the loopback client to a file system controller with fake providers
struct CLoopbackFixture
{
	CMockFileSystemProvider provider;
	CMockDownloadLinkProvider linkProvider;
	CTestableFileSystemController controller;
	icomp::TSimSharedComponentPtr<CLoopbackGqlClientComp> client;
	icomp::TSimSharedComponentPtr<CScriptedSessionComp> session;
	icomp::TSimSharedComponentPtr<imtclientgql::CRemoteFileSystemStructureProviderComp> remoteProvider;

	explicit CLoopbackFixture(bool withSession = false)
	{
		controller.SetProvider(&provider);
		controller.SetDownloadLinkProvider(&linkProvider);
		client->handlerPtr = &controller;
		client->InitComponent();
		session->InitComponent();
		remoteProvider->SetRef("ApiClient", client);
		if (withSession){
			remoteProvider->SetRef("Session", session);
		}
		remoteProvider->InitComponent();
	}

	bool ListRoot(QString& error)
	{
		imtfile::IFileSystemStructureProvider::FileSystemQuery query;
		imtfile::IFileSystemStructureProvider::FileSystemListing listing;

		return remoteProvider->GetEntries(query, listing, error);
	}
};


imtfile::IFileSystemStructureProvider::FileSystemEntry MakeFolderEntry(const QString& path, const QString& name)
{
	imtfile::IFileSystemStructureProvider::FileSystemEntry entry;
	entry.name = name;
	entry.path = path;
	entry.type = imtfile::IFileSystemStructureProvider::FileSystemEntry::Type::Dir;

	return entry;
}


} // namespace


// Byte counts above 4 GiB reach the caller of the remote provider exactly
void CRemoteFileSystemStructureProviderTest::LargeSizesSurviveTheWireTest()
{
	const qint64 fileSize = Q_INT64_C(5) * 1024 * 1024 * 1024;
	const qint64 totalBytes = Q_INT64_C(4) * 1024 * 1024 * 1024 * 1024;
	const qint64 freeBytes = Q_INT64_C(3) * 1024 * 1024 * 1024 * 1024;

	CLoopbackFixture fixture;
	fixture.provider.nextListing.resolvedPath = QStringLiteral("/root");
	fixture.provider.nextListing.totalCount = 2;

	imtfile::IFileSystemStructureProvider::FileSystemEntry file;
	file.name = QStringLiteral("image.iso");
	file.path = QStringLiteral("/root/image.iso");
	file.type = imtfile::IFileSystemStructureProvider::FileSystemEntry::Type::File;
	file.size = fileSize;
	fixture.provider.nextListing.entries.append(file);

	imtfile::IFileSystemStructureProvider::FileSystemEntry drive;
	drive.name = QStringLiteral("Data (D:)");
	drive.path = QStringLiteral("D:/");
	drive.type = imtfile::IFileSystemStructureProvider::FileSystemEntry::Type::Drive;
	drive.totalBytes = totalBytes;
	drive.freeBytes = freeBytes;
	fixture.provider.nextListing.entries.append(drive);

	imtfile::IFileSystemStructureProvider::FileSystemQuery query;
	query.path = QStringLiteral("/root");
	imtfile::IFileSystemStructureProvider::FileSystemListing listing;
	QString error;
	QVERIFY2(fixture.remoteProvider->GetEntries(query, listing, error), qPrintable(error));

	QCOMPARE(listing.resolvedPath, QStringLiteral("/root"));
	QCOMPARE(listing.entries.size(), 2);
	QCOMPARE(listing.entries[0].type, imtfile::IFileSystemStructureProvider::FileSystemEntry::Type::File);
	QCOMPARE(listing.entries[0].size, fileSize);
	QCOMPARE(listing.entries[1].type, imtfile::IFileSystemStructureProvider::FileSystemEntry::Type::Drive);
	QCOMPARE(listing.entries[1].totalBytes, totalBytes);
	QCOMPARE(listing.entries[1].freeBytes, freeBytes);
}


void CRemoteFileSystemStructureProviderTest::ServerErrorReachesTheCallerTest()
{
	CLoopbackFixture fixture;
	fixture.provider.failNext = true;
	fixture.provider.failMessage = QStringLiteral("access denied by provider");

	imtfile::IFileSystemStructureProvider::FileSystemQuery query;
	query.path = QStringLiteral("/root");
	imtfile::IFileSystemStructureProvider::FileSystemListing listing;
	QString error;
	QVERIFY(!fixture.remoteProvider->GetEntries(query, listing, error));
	QCOMPARE(error, QStringLiteral("access denied by provider"));
}


// The server sees every field of the listing among the requested ones
void CRemoteFileSystemStructureProviderTest::RequestsEveryFieldOfTheListingTest()
{
	CLoopbackFixture fixture;

	imtfile::IFileSystemStructureProvider::FileSystemQuery query;
	imtfile::IFileSystemStructureProvider::FileSystemListing listing;
	QString error;
	QVERIFY2(fixture.remoteProvider->GetEntries(query, listing, error), qPrintable(error));

	imtgql::CGqlRequest receivedRequest;
	qsizetype errorPosition = -1;
	QVERIFY(receivedRequest.ParseQuery(fixture.client->lastQuery, errorPosition));
	const sdl::V1_0::imtbase::CGetFileSystemEntriesGqlRequest typedRequest(receivedRequest, false);
	QVERIFY(typedRequest.IsValid());

	const sdl::V1_0::imtbase::GetFileSystemEntriesRequestInfo requestInfo = typedRequest.GetRequestInfo();
	QVERIFY(requestInfo.isPathRequested);
	QVERIFY(requestInfo.isParentPathRequested);
	QVERIFY(requestInfo.isEntriesRequested);
	QVERIFY(requestInfo.isTotalCountRequested);
	QVERIFY(requestInfo.isHasMoreRequested);
	QVERIFY(requestInfo.entries.isNameRequested);
	QVERIFY(requestInfo.entries.isPathRequested);
	QVERIFY(requestInfo.entries.isEntryTypeRequested);
	QVERIFY(requestInfo.entries.isSizeRequested);
	QVERIFY(requestInfo.entries.isLastModifiedRequested);
	QVERIFY(requestInfo.entries.isTotalBytesRequested);
	QVERIFY(requestInfo.entries.isFreeBytesRequested);
}


// Every field of a link reaches the caller of the remote provider unchanged
void CRemoteFileSystemStructureProviderTest::DownloadLinkSurvivesTheWireTest()
{
	CLoopbackFixture fixture;
	imtfile::IFileDownloadLinkProvider::DownloadLink& sentLink = fixture.linkProvider.nextLink;
	sentLink.url = QStringLiteral("http://storage-1:7390/blobs/1?t=ticket");
	sentLink.alternativeUrls = QStringList{QStringLiteral("http://storage-1:7391/blobs/1?t=ticket"), QStringLiteral("http://10.0.0.5:7390/blobs/1?t=ticket")};
	sentLink.expiresAt = QDateTime(QDate(2026, 11, 26), QTime(10, 15), QTimeZone::utc());
	sentLink.size = Q_INT64_C(5) * 1024 * 1024 * 1024;
	sentLink.sha256 = QStringLiteral("9f86d081884c7d659a2feaa0c55ad015a3bf4f1b2b0b822cd15d6c15b0f00a08");
	sentLink.etag = QStringLiteral("\"sha256-9f86d081\"");

	imtfile::IFileDownloadLinkProvider::DownloadLink link;
	QString error;
	QVERIFY2(fixture.remoteProvider->GetDownloadLink(QStringLiteral("/root/setup.exe"), link, error), qPrintable(error));

	QCOMPARE(fixture.linkProvider.lastPath, QStringLiteral("/root/setup.exe"));
	QCOMPARE(link.url, sentLink.url);
	QCOMPARE(link.alternativeUrls, sentLink.alternativeUrls);
	QCOMPARE(link.expiresAt, sentLink.expiresAt);
	QCOMPARE(link.size, sentLink.size);
	QCOMPARE(link.sha256, sentLink.sha256);
	QCOMPARE(link.etag, sentLink.etag);
}


// A link with the URL only arrives without the other fields, and the absent list is no parse error
void CRemoteFileSystemStructureProviderTest::DownloadLinkWithUrlOnlySurvivesTheWireTest()
{
	CLoopbackFixture fixture;
	fixture.linkProvider.nextLink.url = QStringLiteral("http://storage-1:7390/blobs/1?t=ticket");

	imtfile::IFileDownloadLinkProvider::DownloadLink link;
	QString error;
	QVERIFY2(fixture.remoteProvider->GetDownloadLink(QStringLiteral("/root/setup.exe"), link, error), qPrintable(error));

	QCOMPARE(link.url, fixture.linkProvider.nextLink.url);
	QVERIFY(link.alternativeUrls.isEmpty());
	QVERIFY(!link.expiresAt.isValid());
	QCOMPARE(link.size, Q_INT64_C(-1));
	QVERIFY(link.sha256.isEmpty());
	QVERIFY(link.etag.isEmpty());
}


void CRemoteFileSystemStructureProviderTest::DownloadLinkErrorReachesTheCallerTest()
{
	CLoopbackFixture fixture;
	fixture.linkProvider.failNext = true;
	fixture.linkProvider.failMessage = QStringLiteral("file not found");

	imtfile::IFileDownloadLinkProvider::DownloadLink link;
	QString error;
	QVERIFY(!fixture.remoteProvider->GetDownloadLink(QStringLiteral("/root/missing.exe"), link, error));
	QCOMPARE(error, QStringLiteral("file not found"));
}


// The server sees every field of the link among the requested ones
void CRemoteFileSystemStructureProviderTest::RequestsEveryFieldOfTheLinkTest()
{
	CLoopbackFixture fixture;
	fixture.linkProvider.nextLink.url = QStringLiteral("http://storage-1:7390/blobs/1?t=ticket");

	imtfile::IFileDownloadLinkProvider::DownloadLink link;
	QString error;
	QVERIFY2(fixture.remoteProvider->GetDownloadLink(QStringLiteral("/root/setup.exe"), link, error), qPrintable(error));

	imtgql::CGqlRequest receivedRequest;
	qsizetype errorPosition = -1;
	QVERIFY(receivedRequest.ParseQuery(fixture.client->lastQuery, errorPosition));
	const sdl::V1_0::imtbase::CGetDownloadLinkGqlRequest typedRequest(receivedRequest, false);
	QVERIFY(typedRequest.IsValid());

	const sdl::V1_0::imtbase::GetDownloadLinkRequestInfo requestInfo = typedRequest.GetRequestInfo();
	QVERIFY(requestInfo.isLinkRequested);
	QVERIFY(requestInfo.link.isUrlRequested);
	QVERIFY(requestInfo.link.isAlternativeUrlsRequested);
	QVERIFY(requestInfo.link.isExpiresAtRequested);
	QVERIFY(requestInfo.link.isSizeRequested);
	QVERIFY(requestInfo.link.isSha256Requested);
	QVERIFY(requestInfo.link.isEtagRequested);
}


void CRemoteFileSystemStructureProviderTest::StatListsTheParentFolderTest_data()
{
	QTest::addColumn<QString>("path");
	QTest::addColumn<QString>("parentPath");

	QTest::newRow("folder") << QStringLiteral("/Releases/Windows/1.2") << QStringLiteral("/Releases/Windows");
	QTest::newRow("top folder") << QStringLiteral("/Releases") << QStringLiteral("/");
	QTest::newRow("file on a drive") << QStringLiteral("C:/Data/Setup.exe") << QStringLiteral("C:/Data");
	QTest::newRow("top folder of a drive") << QStringLiteral("C:/Data") << QStringLiteral("C:/");
	QTest::newRow("drive") << QStringLiteral("C:/") << QString();
}


// The parent is found by '/' whatever the OS of this side, and the entry by its whole path
void CRemoteFileSystemStructureProviderTest::StatListsTheParentFolderTest()
{
	QFETCH(QString, path);
	QFETCH(QString, parentPath);

	CLoopbackFixture fixture;
	fixture.provider.nextListing.resolvedPath = parentPath;
	fixture.provider.nextListing.entries.append(MakeFolderEntry(path, QStringLiteral("Item")));

	imtfile::IFileSystemStructureProvider::FileSystemEntry entry;
	QString error;
	QVERIFY2(fixture.remoteProvider->Stat(path, entry, error), qPrintable(error));
	QCOMPARE(fixture.provider.lastPath, parentPath);
	QCOMPARE(entry.path, path);
}


void CRemoteFileSystemStructureProviderTest::StatOfTheRootListsTheRootTest_data()
{
	QTest::addColumn<QString>("path");

	QTest::newRow("empty") << QString();
	QTest::newRow("slash") << QStringLiteral("/");
}


// The root has no parent: it is a folder when it can be listed
void CRemoteFileSystemStructureProviderTest::StatOfTheRootListsTheRootTest()
{
	QFETCH(QString, path);

	CLoopbackFixture fixture;
	fixture.provider.nextListing.resolvedPath = QStringLiteral("/");

	imtfile::IFileSystemStructureProvider::FileSystemEntry entry;
	QString error;
	QVERIFY2(fixture.remoteProvider->Stat(path, entry, error), qPrintable(error));
	QCOMPARE(fixture.provider.lastPath, path);
	QCOMPARE(entry.path, QStringLiteral("/"));
	QCOMPARE(entry.type, imtfile::IFileSystemStructureProvider::FileSystemEntry::Type::Dir);
}


// An entry is found by its whole path exactly: neither by its name alone nor in another letter case
void CRemoteFileSystemStructureProviderTest::StatMatchesTheWholePathTest()
{
	CLoopbackFixture fixture;
	fixture.provider.nextListing.resolvedPath = QStringLiteral("/Releases");
	fixture.provider.nextListing.entries.append(MakeFolderEntry(QStringLiteral("/Releases/windows"), QStringLiteral("windows")));
	fixture.provider.nextListing.entries.append(MakeFolderEntry(QStringLiteral("/Archive/Windows"), QStringLiteral("Windows")));

	imtfile::IFileSystemStructureProvider::FileSystemEntry entry;
	QString error;
	QVERIFY(!fixture.remoteProvider->Stat(QStringLiteral("/Releases/Windows"), entry, error));
	QVERIFY(!error.isEmpty());
}


// With a session the request carries its token and its product, not the context of the request being handled
void CRemoteFileSystemStructureProviderTest::SessionContextReplacesTheCallerContextTest()
{
	CLoopbackFixture fixture(true);
	fixture.session->tokens = QByteArrayList{QByteArrayLiteral("session-token")};

	imtgql::CGqlContext callerContext;
	callerContext.SetToken(QByteArrayLiteral("caller-token"));
	callerContext.SetProductId(QByteArrayLiteral("CallerProduct"));
	imtgql::CGqlRequestContextManager::SetContext(&callerContext);

	QString error;
	const bool isListed = fixture.ListRoot(error);
	imtgql::CGqlRequestContextManager::Clear();

	QVERIFY2(isListed, qPrintable(error));
	QCOMPARE(fixture.client->sentTokens, QByteArrayList{QByteArrayLiteral("session-token")});
	QCOMPARE(fixture.client->sentProductIds, QByteArrayList{QByteArrayLiteral("RemoteProduct")});
}


// Without a session the request goes on behalf of the caller, as before
void CRemoteFileSystemStructureProviderTest::WithoutSessionTheCallerContextGoesOnTest()
{
	CLoopbackFixture fixture;

	imtgql::CGqlContext callerContext;
	callerContext.SetToken(QByteArrayLiteral("caller-token"));
	imtgql::CGqlRequestContextManager::SetContext(&callerContext);

	QString error;
	const bool isListed = fixture.ListRoot(error);
	imtgql::CGqlRequestContextManager::Clear();

	QVERIFY2(isListed, qPrintable(error));
	QCOMPARE(fixture.client->sentTokens, QByteArrayList{QByteArrayLiteral("caller-token")});
}


// After a refused token the session gives a new one, and the request goes once more
void CRemoteFileSystemStructureProviderTest::RefusedTokenIsReplacedOnceTest()
{
	CLoopbackFixture fixture(true);
	fixture.session->tokens = QByteArrayList{QByteArrayLiteral("expired-token"), QByteArrayLiteral("new-token")};
	fixture.client->deniedTokens = QByteArrayList{QByteArrayLiteral("expired-token")};

	QString error;
	QVERIFY2(fixture.ListRoot(error), qPrintable(error));
	QCOMPARE(fixture.session->rejectedTokens, QByteArrayList{QByteArrayLiteral("expired-token")});
	QCOMPARE(fixture.client->sentTokens, (QByteArrayList{QByteArrayLiteral("expired-token"), QByteArrayLiteral("new-token")}));
}


// A server that refuses every token gets the request twice, not again and again
void CRemoteFileSystemStructureProviderTest::TokenRefusedAgainFailsTest()
{
	CLoopbackFixture fixture(true);
	fixture.session->tokens = QByteArrayList{QByteArrayLiteral("token-1"), QByteArrayLiteral("token-2"), QByteArrayLiteral("token-3")};
	fixture.client->deniedTokens = fixture.session->tokens;

	QString error;
	QVERIFY(!fixture.ListRoot(error));
	QVERIFY(!error.isEmpty());
	QCOMPARE(fixture.client->sentTokens, (QByteArrayList{QByteArrayLiteral("token-1"), QByteArrayLiteral("token-2")}));
	QCOMPARE(fixture.session->rejectedTokens, (QByteArrayList{QByteArrayLiteral("token-1"), QByteArrayLiteral("token-2")}));
}


// A refused sign-in: nothing goes to the remote server, and the reason reaches the caller
void CRemoteFileSystemStructureProviderTest::RefusedSignInSendsNothingTest()
{
	CLoopbackFixture fixture(true);
	fixture.session->signInError = QStringLiteral("The sign-in was refused");

	QString error;
	QVERIFY(!fixture.ListRoot(error));
	QCOMPARE(error, QStringLiteral("The sign-in was refused"));
	QVERIFY(fixture.client->sentTokens.isEmpty());
}


I_ADD_TEST(CRemoteFileSystemStructureProviderTest);
