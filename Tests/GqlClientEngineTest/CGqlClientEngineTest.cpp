// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include "CGqlClientEngineTest.h"


// STL includes
#include <memory>

// Qt includes
#include <QtNetwork/QNetworkRequest>

// ACF includes
#include <icomp/TSimComponentWrap.h>
#include <itest/CStandardTestExecutor.h>

// ImtCore includes
#include <imtbase/imtbase.h>
#include <imtbase/CUrlParam.h>
#include <imtclientgql/CGqlClientEngineComp.h>
#include <imtgql/CGqlContext.h>
#include <imtgql/CGqlRequest.h>
#include <imtservergql/DocumentChange.h>


namespace
{


// Headers of a request the server received: the HTTP request gives them in lower case
std::unique_ptr<QNetworkRequest> CreateForwardedRequest(const imtgql::IGqlContext::Headers* headersPtr)
{
	icomp::TSimComponentWrap<imtclientgql::CGqlClientEngineComp> engine;
	engine.InitComponent();

	imtgql::CGqlRequest request(imtgql::IGqlRequest::RT_MUTATION, "UpdateTenantFromRepresentation");
	if (headersPtr != nullptr){
		imtgql::IGqlContextSharedPtr contextPtr = new imtgql::CGqlContext;
		contextPtr->SetHeaders(*headersPtr);
		request.SetGqlContext(contextPtr);
	}

	imtbase::CUrlParam urlParam;
	urlParam.SetUrl(QUrl("http://localhost:7777/Puma/graphql"));

	return std::unique_ptr<QNetworkRequest>(engine.CreateNetworkRequest(request, &urlParam));
}


// A request the server received with these headers
imtgql::CGqlRequest CreateReceivedRequest(const imtgql::IGqlContext::Headers& headers)
{
	imtgql::CGqlRequest request(imtgql::IGqlRequest::RT_MUTATION, "UpdateTenantFromRepresentation");
	imtgql::IGqlContextSharedPtr contextPtr = new imtgql::CGqlContext;
	contextPtr->SetHeaders(headers);
	request.SetGqlContext(contextPtr);

	return request;
}


} // namespace


void CGqlClientEngineTest::testChangeIdIsForwarded()
{
	imtgql::IGqlContext::Headers headers;
	headers.insert("documentchangeid", "change-1");

	std::unique_ptr<QNetworkRequest> networkRequestPtr = CreateForwardedRequest(&headers);

	QVERIFY(networkRequestPtr != nullptr);
	QCOMPARE(networkRequestPtr->rawHeader(imtbase::s_documentChangeIdHeaderId), QByteArray("change-1"));
}


void CGqlClientEngineTest::testNoChangeIdNoHeader()
{
	imtgql::IGqlContext::Headers headers;
	headers.insert("documentchangeid", "");

	std::unique_ptr<QNetworkRequest> networkRequestPtr = CreateForwardedRequest(&headers);

	QVERIFY(networkRequestPtr != nullptr);
	QVERIFY(!networkRequestPtr->hasRawHeader(imtbase::s_documentChangeIdHeaderId));
}


void CGqlClientEngineTest::testIncomingHeadersAreNotForwarded()
{
	imtgql::IGqlContext::Headers headers;
	headers.insert("documentchangeid", "change-2");
	headers.insert("host", "prolife:7778");
	headers.insert("content-length", "12");
	headers.insert("permissionpath", "/OrganizationManagement/Edit");

	std::unique_ptr<QNetworkRequest> networkRequestPtr = CreateForwardedRequest(&headers);

	QVERIFY(networkRequestPtr != nullptr);
	QCOMPARE(networkRequestPtr->rawHeader(imtbase::s_documentChangeIdHeaderId), QByteArray("change-2"));
	QVERIFY(!networkRequestPtr->hasRawHeader("host"));
	QVERIFY(!networkRequestPtr->hasRawHeader("content-length"));
	QVERIFY(!networkRequestPtr->hasRawHeader("permissionpath"));
}


void CGqlClientEngineTest::testNoContextNoHeader()
{
	std::unique_ptr<QNetworkRequest> networkRequestPtr = CreateForwardedRequest(nullptr);

	QVERIFY(networkRequestPtr != nullptr);
	QVERIFY(!networkRequestPtr->hasRawHeader(imtbase::s_documentChangeIdHeaderId));
}


void CGqlClientEngineTest::testServerTakesUuidChangeId()
{
	imtgql::IGqlContext::Headers headers;
	headers.insert("documentchangeid", "6f1c2a3e-6a43-4c1d-9b0e-4c2f1d7b8a90");

	QCOMPARE(imtservergql::GetDocumentChangeId(CreateReceivedRequest(headers)), QByteArray("6f1c2a3e-6a43-4c1d-9b0e-4c2f1d7b8a90"));
}


// The change-ID reaches the notifications of other users: a value that is no UUID is not taken
void CGqlClientEngineTest::testServerDropsNonUuidChangeId()
{
	const QList<QByteArray> values = {"change-1", "<script>", "null", ""};
	for (const QByteArray& value : values){
		imtgql::IGqlContext::Headers headers;
		headers.insert("documentchangeid", value);

		QVERIFY2(imtservergql::GetDocumentChangeId(CreateReceivedRequest(headers)).isEmpty(), value.constData());
	}
}


I_ADD_TEST(CGqlClientEngineTest);


