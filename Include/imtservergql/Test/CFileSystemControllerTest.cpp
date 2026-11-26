// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include "CFileSystemControllerTest.h"


#include <QtCore/QTimeZone>
#include <itest/CStandardTestExecutor.h>


namespace
{


imtgql::CGqlRequest MakeGetFileSystemEntriesRequest(const QString& path = QString())
{
	imtgql::CGqlRequest request;
	sdl::V1_0::imtbase::GetFileSystemEntriesRequestArguments arguments;
	arguments.input.emplace();
	if (!path.isEmpty()){
		arguments.input->path = path;
	}

	[[maybe_unused]] auto unused = sdl::V1_0::imtbase::CGetFileSystemEntriesGqlRequest::SetupGqlRequest(request, arguments);
	request.AddSimpleField(QByteArrayLiteral("path"));
	request.AddSimpleField(QByteArrayLiteral("parentPath"));
	request.AddSimpleField(QByteArrayLiteral("totalCount"));
	request.AddSimpleField(QByteArrayLiteral("hasMore"));

	return request;
}


imtgql::CGqlRequest MakeGetDownloadLinkRequest(const QString& path)
{
	imtgql::CGqlRequest request;
	sdl::V1_0::imtbase::GetDownloadLinkRequestArguments arguments;
	arguments.input.emplace();
	arguments.input->path = path;

	[[maybe_unused]] auto unused = sdl::V1_0::imtbase::CGetDownloadLinkGqlRequest::SetupGqlRequest(request, arguments);

	return request;
}


} // namespace


void CFileSystemControllerTest::MissingProviderFailsTest()
{
	CTestableFileSystemController controller;
	controller.SetProvider(nullptr);

	const imtgql::CGqlRequest request = MakeGetFileSystemEntriesRequest();
	sdl::V1_0::imtbase::CGetFileSystemEntriesGqlRequest typedRequest(request, true);
	QString error;
	const auto payload = controller.OnGetFileSystemEntries(typedRequest, request, error);

	QVERIFY(!error.isEmpty());
	QVERIFY(!payload.path.has_value());
}


void CFileSystemControllerTest::ProviderErrorIsPropagatedTest()
{
	CMockFileSystemProvider provider;
	provider.failNext = true;
	provider.failMessage = QStringLiteral("access denied by provider");

	CTestableFileSystemController controller;
	controller.SetProvider(&provider);

	const imtgql::CGqlRequest request = MakeGetFileSystemEntriesRequest(QStringLiteral("/tmp"));
	sdl::V1_0::imtbase::CGetFileSystemEntriesGqlRequest typedRequest(request, true);
	QString error;
	const auto payload = controller.OnGetFileSystemEntries(typedRequest, request, error);

	QCOMPARE(error, QStringLiteral("access denied by provider"));
	QVERIFY(!payload.path.has_value());
}


void CFileSystemControllerTest::HappyPathMapsPayloadTest()
{
	CMockFileSystemProvider provider;
	provider.nextListing.resolvedPath = QStringLiteral("/root/sub");
	provider.nextListing.parentPath = QStringLiteral("/root");
	provider.nextListing.hasParent = true;
	provider.nextListing.totalCount = 1;
	provider.nextListing.hasMore = false;

	imtfile::IFileSystemStructureProvider::FileSystemEntry entry;
	entry.name = QStringLiteral("file.txt");
	entry.path = QStringLiteral("/root/sub/file.txt");
	entry.type = imtfile::IFileSystemStructureProvider::FileSystemEntry::Type::File;
	entry.size = 42;
	entry.lastModified = QDateTime::fromString(QStringLiteral("2024-01-02T03:04:05"), Qt::ISODate);
	provider.nextListing.entries.append(entry);

	CTestableFileSystemController controller;
	controller.SetProvider(&provider);

	const imtgql::CGqlRequest request = MakeGetFileSystemEntriesRequest(QStringLiteral("/root/sub"));
	sdl::V1_0::imtbase::CGetFileSystemEntriesGqlRequest typedRequest(request, true);
	QString error;
	const auto payload = controller.OnGetFileSystemEntries(typedRequest, request, error);

	QVERIFY2(error.isEmpty(), error.toUtf8().constData());
	QVERIFY(payload.path.has_value());
	QCOMPARE(*payload.path, QStringLiteral("/root/sub"));
	QVERIFY(payload.parentPath.has_value());
	QCOMPARE(*payload.parentPath, QStringLiteral("/root"));
	QVERIFY(payload.totalCount.has_value());
	QCOMPARE(*payload.totalCount, 1);
	QVERIFY(payload.hasMore.has_value());
	QCOMPARE(*payload.hasMore, false);
	QVERIFY(payload.entries.has_value());
	QCOMPARE(payload.entries->size(), 1);
	QVERIFY(payload.entries->at(0).has_value());
	const sdl::V1_0::imtbase::CFileSystemEntry& mapped = *payload.entries->at(0);
	QVERIFY(mapped.name.has_value());
	QCOMPARE(*mapped.name, QStringLiteral("file.txt"));
	QVERIFY(mapped.entryType.has_value());
	QCOMPARE(*mapped.entryType, sdl::V1_0::imtbase::FileSystemEntryType::File);
	QVERIFY(mapped.size.has_value());
	QCOMPARE(*mapped.size, 42);
}


void CFileSystemControllerTest::LargeSizesAreNotTruncatedTest()
{
	const qint64 fileSize = Q_INT64_C(5) * 1024 * 1024 * 1024;
	const qint64 totalBytes = Q_INT64_C(4) * 1024 * 1024 * 1024 * 1024;
	const qint64 freeBytes = Q_INT64_C(3) * 1024 * 1024 * 1024 * 1024;

	CMockFileSystemProvider provider;
	provider.nextListing.resolvedPath = QStringLiteral("/root");
	provider.nextListing.totalCount = 2;

	imtfile::IFileSystemStructureProvider::FileSystemEntry file;
	file.name = QStringLiteral("image.iso");
	file.path = QStringLiteral("/root/image.iso");
	file.type = imtfile::IFileSystemStructureProvider::FileSystemEntry::Type::File;
	file.size = fileSize;
	provider.nextListing.entries.append(file);

	imtfile::IFileSystemStructureProvider::FileSystemEntry drive;
	drive.name = QStringLiteral("Data (D:)");
	drive.path = QStringLiteral("D:/");
	drive.type = imtfile::IFileSystemStructureProvider::FileSystemEntry::Type::Drive;
	drive.totalBytes = totalBytes;
	drive.freeBytes = freeBytes;
	provider.nextListing.entries.append(drive);

	CTestableFileSystemController controller;
	controller.SetProvider(&provider);

	const imtgql::CGqlRequest request = MakeGetFileSystemEntriesRequest(QStringLiteral("/root"));
	sdl::V1_0::imtbase::CGetFileSystemEntriesGqlRequest typedRequest(request, true);
	QString error;
	const auto payload = controller.OnGetFileSystemEntries(typedRequest, request, error);

	QVERIFY2(error.isEmpty(), error.toUtf8().constData());
	QVERIFY(payload.entries.has_value());
	QCOMPARE(payload.entries->size(), 2);
	QVERIFY(payload.entries->at(0).has_value());
	QVERIFY(payload.entries->at(0)->size.has_value());
	QCOMPARE(*payload.entries->at(0)->size, fileSize);
	QVERIFY(payload.entries->at(1).has_value());
	QVERIFY(payload.entries->at(1)->totalBytes.has_value());
	QCOMPARE(*payload.entries->at(1)->totalBytes, totalBytes);
	QVERIFY(payload.entries->at(1)->freeBytes.has_value());
	QCOMPARE(*payload.entries->at(1)->freeBytes, freeBytes);
}


void CFileSystemControllerTest::PermissionDeniedCreateResponseTest()
{
	CMockFileSystemProvider provider;
	provider.nextListing.resolvedPath = QStringLiteral("/root");
	provider.nextListing.totalCount = 0;

	CTestableFileSystemController controller;
	controller.SetProvider(&provider);
	controller.SetPermissionsAllowed(false);

	// Without CommandPermissions configured, base CheckPermissions returns true.
	// Our test subclass overrides CheckPermissions; CreateResponse must refuse.
	const imtgql::CGqlRequest request = MakeGetFileSystemEntriesRequest();
	QString error;
	const QJsonObject response = controller.CreateResponse(request, error);

	QVERIFY(response.isEmpty());
	QVERIFY(!error.isEmpty());
}


void CFileSystemControllerTest::DownloadLinkWithoutProviderIsRefusedTest()
{
	CMockFileSystemProvider provider;
	CTestableFileSystemController controller;
	controller.SetProvider(&provider);

	const imtgql::CGqlRequest request = MakeGetDownloadLinkRequest(QStringLiteral("/root/setup.exe"));
	sdl::V1_0::imtbase::CGetDownloadLinkGqlRequest typedRequest(request, true);
	QString error;
	const auto payload = controller.OnGetDownloadLink(typedRequest, request, error);

	QVERIFY(!error.isEmpty());
	QVERIFY(!payload.link.has_value());
}


void CFileSystemControllerTest::DownloadLinkMapsAllFieldsTest()
{
	CMockDownloadLinkProvider linkProvider;
	linkProvider.nextLink.url = QStringLiteral("http://storage-1:7390/blobs/1?t=ticket");
	linkProvider.nextLink.alternativeUrls = QStringList{QStringLiteral("http://storage-1:7391/blobs/1?t=ticket"), QStringLiteral("http://10.0.0.5:7390/blobs/1?t=ticket")};
	linkProvider.nextLink.expiresAt = QDateTime(QDate(2026, 11, 26), QTime(13, 15), QTimeZone::fromSecondsAheadOfUtc(3 * 3600));
	linkProvider.nextLink.size = Q_INT64_C(5) * 1024 * 1024 * 1024;
	linkProvider.nextLink.sha256 = QStringLiteral("9f86d081884c7d659a2feaa0c55ad015a3bf4f1b2b0b822cd15d6c15b0f00a08");
	linkProvider.nextLink.etag = QStringLiteral("\"sha256-9f86d081\"");

	CTestableFileSystemController controller;
	controller.SetDownloadLinkProvider(&linkProvider);

	const imtgql::CGqlRequest request = MakeGetDownloadLinkRequest(QStringLiteral("/root/setup.exe"));
	sdl::V1_0::imtbase::CGetDownloadLinkGqlRequest typedRequest(request, true);
	QString error;
	const auto payload = controller.OnGetDownloadLink(typedRequest, request, error);

	QVERIFY2(error.isEmpty(), error.toUtf8().constData());
	QCOMPARE(linkProvider.lastPath, QStringLiteral("/root/setup.exe"));
	QVERIFY(payload.link.has_value());
	const sdl::V1_0::imtbase::CDownloadLink& link = *payload.link;
	QVERIFY(link.url.has_value());
	QCOMPARE(*link.url, linkProvider.nextLink.url);
	QVERIFY(link.alternativeUrls.has_value());
	QCOMPARE(link.alternativeUrls->ToList(), linkProvider.nextLink.alternativeUrls);
	// The moment of the link in UTC, whatever the time zone of the provider
	QVERIFY(link.expiresAt.has_value());
	QCOMPARE(*link.expiresAt, QStringLiteral("2026-11-26T10:15:00Z"));
	QVERIFY(link.size.has_value());
	QCOMPARE(*link.size, linkProvider.nextLink.size);
	QVERIFY(link.sha256.has_value());
	QCOMPARE(*link.sha256, linkProvider.nextLink.sha256);
	// The entity tag goes on unchanged, with its quotes
	QVERIFY(link.etag.has_value());
	QCOMPARE(*link.etag, QStringLiteral("\"sha256-9f86d081\""));
}


void CFileSystemControllerTest::DownloadLinkOmitsUnknownFieldsTest()
{
	CMockDownloadLinkProvider linkProvider;
	linkProvider.nextLink.url = QStringLiteral("http://storage-1:7390/blobs/1?t=ticket");

	CTestableFileSystemController controller;
	controller.SetDownloadLinkProvider(&linkProvider);

	const imtgql::CGqlRequest request = MakeGetDownloadLinkRequest(QStringLiteral("/root/setup.exe"));
	sdl::V1_0::imtbase::CGetDownloadLinkGqlRequest typedRequest(request, true);
	QString error;
	const auto payload = controller.OnGetDownloadLink(typedRequest, request, error);

	QVERIFY2(error.isEmpty(), error.toUtf8().constData());
	QVERIFY(payload.link.has_value());
	QVERIFY(payload.link->url.has_value());
	QVERIFY(!payload.link->alternativeUrls.has_value());
	QVERIFY(!payload.link->expiresAt.has_value());
	QVERIFY(!payload.link->size.has_value());
	QVERIFY(!payload.link->sha256.has_value());
	QVERIFY(!payload.link->etag.has_value());
}


void CFileSystemControllerTest::DownloadLinkProviderErrorIsPropagatedTest()
{
	CMockDownloadLinkProvider linkProvider;
	linkProvider.failNext = true;
	linkProvider.failMessage = QStringLiteral("file not found");

	CTestableFileSystemController controller;
	controller.SetDownloadLinkProvider(&linkProvider);

	const imtgql::CGqlRequest request = MakeGetDownloadLinkRequest(QStringLiteral("/root/missing.exe"));
	sdl::V1_0::imtbase::CGetDownloadLinkGqlRequest typedRequest(request, true);
	QString error;
	const auto payload = controller.OnGetDownloadLink(typedRequest, request, error);

	QCOMPARE(error, QStringLiteral("file not found"));
	QVERIFY(!payload.link.has_value());
}


// A failure must not pass for a success: the generated handler tells them apart by the message only
void CFileSystemControllerTest::DownloadLinkFailureWithoutMessageIsAnErrorTest()
{
	CMockDownloadLinkProvider linkProvider;
	linkProvider.failNext = true;
	linkProvider.failMessage.clear();

	CTestableFileSystemController controller;
	controller.SetDownloadLinkProvider(&linkProvider);

	const imtgql::CGqlRequest request = MakeGetDownloadLinkRequest(QStringLiteral("/root/setup.exe"));
	QString error;
	const QJsonObject response = controller.CreateResponse(request, error);

	QVERIFY(response.isEmpty());
	QVERIFY(!error.isEmpty());
}


I_ADD_TEST(CFileSystemControllerTest);
