// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include "CCollectionDocumentServicePublisherTest.h"


// Qt includes
#include <QtCore/QJsonDocument>
#include <QtCore/QJsonObject>

// ACF includes
#include <icomp/TSimComponentWrap.h>
#include <itest/CStandardTestExecutor.h>

// ImtCore includes
#include <imtbase/imtbase.h>
#include <imtdoc/CDocumentChangedEvent.h>
#include <imtgql/CGqlContext.h>
#include <imtgql/CGqlRequestContextManager.h>
#include <imtrest/CWebSocketRequest.h>
#include <imtrest/IProtocolEngine.h>
#include <GeneratedFiles/imtbasesdl/SDL/1.0/CPP/CollectionDocumentService.h>


namespace
{


const QByteArray s_requestId = QByteArrayLiteral("1b4e28ba-2fa1-11d2-883f-0016d3cca427");


class CProtocolEngineStub: virtual public imtrest::IProtocolEngine
{
public:
	// reimplemented (imtrest::IProtocolEngine)
	QByteArray GetProtocolTypeId() const override
	{
		return QByteArrayLiteral("websocket");
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


// Publishes a DocumentChanged event while the thread has a request context with these headers (null: no context)
QJsonObject PublishDocumentChanged(const imtgql::IGqlContext::Headers* headersPtr)
{
	CProtocolEngineStub engine;
	imtrest::CWebSocketRequest networkRequest(engine);

	icomp::TSimSharedComponentPtr<CTestableCollectionDocumentServicePublisher> publisherPtr;
	publisherPtr->SetIdAttr("CollectionId", QByteArrayLiteral("Documents"));
	publisherPtr->InsertMultiAttr("CommandIds", QString(sdl::V1_0::imtbase::COnDocumentManagerChangedGqlRequest::GetCommandId()));
	publisherPtr->SetIntAttr("CloseDocumentTimeout", 0);
	publisherPtr->InitComponent();

	publisherPtr->AddSubscriber(
				QByteArrayLiteral("subscription"),
				sdl::V1_0::imtbase::COnDocumentManagerChangedGqlRequest::GetCommandId(),
				networkRequest);

	if (headersPtr != nullptr){
		imtgql::CGqlContext context;
		context.SetHeaders(*headersPtr);
		imtgql::CGqlRequestContextManager::SetContext(&context);
	}

	istd::IChangeable document;
	imtdoc::CDocumentChangedEvent event(
				QByteArrayLiteral("user"),
				QByteArrayLiteral("document"),
				QByteArrayLiteral("type"),
				QStringLiteral("Name"),
				QByteArray(),
				true,
				document,
				istd::IChangeable::GetAnyChange());
	imtdoc::IDocumentServiceEventHandler& eventHandler = *publisherPtr;
	eventHandler.ProcessEvent(&event);

	imtgql::CGqlRequestContextManager::Clear();

	if (publisherPtr->m_pushedData.count() != 1){
		return QJsonObject();
	}

	return QJsonDocument::fromJson(publisherPtr->m_pushedData.first().data).object();
}


} // namespace


// public methods of the class CTestableCollectionDocumentServicePublisher

void CTestableCollectionDocumentServicePublisher::AddSubscriber(
			const QByteArray& subscriptionId,
			const QByteArray& commandId,
			const imtrest::IRequest& networkRequest)
{
	RequestNetworks subscriber;
	subscriber.gqlRequest.CopyFrom(imtgql::CGqlRequest(imtgql::IGqlRequest::RT_SUBSCRIPTION, commandId));
	subscriber.networkRequests.insert(subscriptionId, &networkRequest);

	m_registeredSubscribers.append(subscriber);
}


// reimplemented (imtservergql::CGqlPublisherCompBase)

bool CTestableCollectionDocumentServicePublisher::PushDataToSubscriber(
			const QByteArray& /*subscriptionId*/,
			const QByteArray& commandId,
			const QByteArray& data,
			const imtrest::IRequest& /*networkRequest*/,
			const bool /*useAwsStyle*/) const
{
	const_cast<CTestableCollectionDocumentServicePublisher*>(this)->m_pushedData.append({commandId, data});

	return true;
}


// private slots of the class CCollectionDocumentServicePublisherTest

void CCollectionDocumentServicePublisherTest::cleanup()
{
	imtgql::CGqlRequestContextManager::Clear();
}


void CCollectionDocumentServicePublisherTest::RequestIdOfTheRequestContextTest()
{
	imtgql::IGqlContext::Headers headers;
	headers.insert(imtbase::s_requestIdHeaderId.toLower(), s_requestId);

	const QJsonObject notification = PublishDocumentChanged(&headers);

	QCOMPARE(notification.value(QStringLiteral("documentOperation")).toString(), QStringLiteral("DocumentChanged"));
	QCOMPARE(notification.value(QStringLiteral("requestId")).toString().toUtf8(), s_requestId);
}


void CCollectionDocumentServicePublisherTest::NoRequestContextTest()
{
	const QJsonObject notification = PublishDocumentChanged(nullptr);

	QCOMPARE(notification.value(QStringLiteral("documentOperation")).toString(), QStringLiteral("DocumentChanged"));
	QVERIFY2(!notification.contains(QStringLiteral("requestId")), "A change made outside of a request names no request");
}


void CCollectionDocumentServicePublisherTest::RequestContextWithoutRequestIdTest()
{
	imtgql::IGqlContext::Headers headers;
	headers.insert(QByteArrayLiteral("productid"), QByteArrayLiteral("product"));

	const QJsonObject notification = PublishDocumentChanged(&headers);

	QCOMPARE(notification.value(QStringLiteral("documentOperation")).toString(), QStringLiteral("DocumentChanged"));
	QVERIFY(!notification.contains(QStringLiteral("requestId")));
}


void CCollectionDocumentServicePublisherTest::InvalidRequestIdTest()
{
	imtgql::IGqlContext::Headers headers;
	headers.insert(imtbase::s_requestIdHeaderId.toLower(), QByteArrayLiteral("not-a-uuid"));

	const QJsonObject notification = PublishDocumentChanged(&headers);

	QCOMPARE(notification.value(QStringLiteral("documentOperation")).toString(), QStringLiteral("DocumentChanged"));
	QVERIFY2(!notification.contains(QStringLiteral("requestId")), "The notification reaches other sessions: only a UUID is passed on");
}


I_ADD_TEST(CCollectionDocumentServicePublisherTest);
