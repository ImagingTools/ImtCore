// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include "CFileSystemControllerTest.h"


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
	sdl::V1_0::imtbase::CGetFileSystemEntriesGqlRequest::SetupGqlRequest(request, arguments);
	request.AddSimpleField(QByteArrayLiteral("path"));
	request.AddSimpleField(QByteArrayLiteral("parentPath"));
	request.AddSimpleField(QByteArrayLiteral("totalCount"));
	request.AddSimpleField(QByteArrayLiteral("hasMore"));

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
	const auto payload = controller.OnGetFileSystemEntries(typedRequest, request, error.toUtf8().constData());

	QCOMPARE(error, QStringLiteral("access denied by provider"));
	QVERIFY(!payload.path.has_value());
}


void CFileSystemControllerTest::HappyPathMapsPayloadTest()
{
	CMockFileSystemProvider provider;
	provider.nextListing.resolvedPath = QStringLiteral("/root/sub");
	provider.nextListing.parentPath = QStringLiteral("/root");
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
	const auto payload = controller.OnGetFileSystemEntries(typedRequest, request, error.toUtf8().constData());

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
	const QJsonObject response = controller.CreateResponse(request, error.toUtf8().constData());

	QVERIFY(response.isEmpty());
	QVERIFY(!error.isEmpty());
}
