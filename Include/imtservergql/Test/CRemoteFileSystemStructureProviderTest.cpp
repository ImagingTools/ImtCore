// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include "CRemoteFileSystemStructureProviderTest.h"


#include <QtCore/QJsonArray>
#include <QtCore/QJsonDocument>
#include <QtCore/QJsonObject>
#include <icomp/TSimComponentWrap.h>
#include <itest/CStandardTestExecutor.h>
#include <imtclientgql/CRemoteFileSystemStructureProviderComp.h>
#include <imtgql/CGqlRequest.h>
#include <imtgql/CGqlResponse.h>

#include "CFileSystemControllerTest.h"


namespace
{


// Sends each request as query text to a handler in this process and wraps the answer like the HTTP GraphQL servlet
class CLoopbackGqlClientComp: public icomp::CComponentBase, virtual public imtclientgql::IGqlClient
{
public:
	typedef icomp::CComponentBase BaseClass;
	I_BEGIN_COMPONENT(CLoopbackGqlClientComp)
		I_REGISTER_INTERFACE(imtclientgql::IGqlClient);
	I_END_COMPONENT

	const imtgql::IGqlRequestHandler* handlerPtr = nullptr;

	GqlResult SendRequest(GqlRequestPtr requestPtr, imtbase::IUrlParam* /*urlParamPtr*/) const override
	{
		imtgql::CGqlRequest serverRequest;
		qsizetype errorPosition = -1;
		if (!serverRequest.ParseQuery(requestPtr->GetQuery(), errorPosition)){
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


} // namespace


// Byte counts above 4 GiB reach the caller of the remote provider exactly
void CRemoteFileSystemStructureProviderTest::LargeSizesSurviveTheWireTest()
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

	icomp::TSimSharedComponentPtr<CLoopbackGqlClientComp> client;
	client->handlerPtr = &controller;
	client->InitComponent();

	icomp::TSimSharedComponentPtr<imtclientgql::CRemoteFileSystemStructureProviderComp> remoteProvider;
	remoteProvider->SetRef("ApiClient", client);
	remoteProvider->InitComponent();

	imtfile::IFileSystemStructureProvider::FileSystemQuery query;
	query.path = QStringLiteral("/root");
	imtfile::IFileSystemStructureProvider::FileSystemListing listing;
	QString error;
	QVERIFY2(remoteProvider->GetEntries(query, listing, error), qPrintable(error));

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
	CMockFileSystemProvider provider;
	provider.failNext = true;
	provider.failMessage = QStringLiteral("access denied by provider");

	CTestableFileSystemController controller;
	controller.SetProvider(&provider);

	icomp::TSimSharedComponentPtr<CLoopbackGqlClientComp> client;
	client->handlerPtr = &controller;
	client->InitComponent();

	icomp::TSimSharedComponentPtr<imtclientgql::CRemoteFileSystemStructureProviderComp> remoteProvider;
	remoteProvider->SetRef("ApiClient", client);
	remoteProvider->InitComponent();

	imtfile::IFileSystemStructureProvider::FileSystemQuery query;
	query.path = QStringLiteral("/root");
	imtfile::IFileSystemStructureProvider::FileSystemListing listing;
	QString error;
	QVERIFY(!remoteProvider->GetEntries(query, listing, error));
	QCOMPARE(error, QStringLiteral("access denied by provider"));
}


I_ADD_TEST(CRemoteFileSystemStructureProviderTest);
