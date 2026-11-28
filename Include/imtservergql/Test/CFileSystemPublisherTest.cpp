// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include "CFileSystemPublisherTest.h"


#include <memory>
#include <QtCore/QJsonDocument>
#include <QtCore/QJsonObject>
#include <icomp/TSimComponentWrap.h>
#include <itest/CStandardTestExecutor.h>
#include <imtfile/IFileSystemChangeNotifier.h>
#include <imtgql/CGqlRequest.h>
#include <imtrest/CWebSocketRequest.h>
#include <imtrest/IProtocolEngine.h>
#include <imtservergql/CFileSystemPublisherComp.h>
#include <GeneratedFiles/imtbasesdl/SDL/1.0/CPP/FileSystem.h>


namespace
{


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


// Gives out "watch-1", "watch-2"... or refuses; can send a change also to a watch that has ended, like a late call of another thread
class CScriptedChangeNotifierComp: public icomp::CComponentBase, virtual public imtfile::IFileSystemChangeNotifier
{
public:
	typedef icomp::CComponentBase BaseClass;
	I_BEGIN_COMPONENT(CScriptedChangeNotifierComp)
		I_REGISTER_INTERFACE(imtfile::IFileSystemChangeNotifier);
	I_END_COMPONENT

	QString refusal;
	QStringList watchedPaths;
	QByteArrayList removedWatchIds;

	bool AddWatch(const QString& path, imtfile::IFileSystemChangeListener& listener, QByteArray& watchId, QString& error) override
	{
		watchedPaths.append(path);
		if (!refusal.isEmpty()){
			error = refusal;

			return false;
		}

		watchId = QByteArrayLiteral("watch-") + QByteArray::number(watchedPaths.size());
		m_listeners.insert(watchId, &listener);

		return true;
	}

	void RemoveWatch(const QByteArray& watchId) override
	{
		removedWatchIds.append(watchId);
	}

	void SendChange(const QByteArray& watchId, const QString& path)
	{
		imtfile::IFileSystemChangeListener* listenerPtr = m_listeners.value(watchId);
		if (listenerPtr != nullptr){
			listenerPtr->OnFileSystemChanged(watchId, path);
		}
	}

private:
	QMap<QByteArray, imtfile::IFileSystemChangeListener*> m_listeners;
};


// Keeps the events instead of sending them
class CTestableFileSystemPublisherComp: public imtservergql::CFileSystemPublisherComp
{
public:
	typedef imtservergql::CFileSystemPublisherComp BaseClass;
	I_BEGIN_COMPONENT(CTestableFileSystemPublisherComp);
	I_END_COMPONENT;

	struct SentEvent
	{
		QByteArray connectionId;
		QByteArray subscriptionId;
		QByteArray commandId;
		QJsonObject event;
	};

	mutable QList<SentEvent> sentEvents;

protected:
	// reimplemented (imtservergql::CGqlPublisherCompBase)
	bool PushDataToSubscriber(
				const QByteArray& subscriptionId,
				const QByteArray& commandId,
				const QByteArray& data,
				const imtrest::IRequest& networkRequest,
				const bool /*useAwsStyle*/) const override
	{
		sentEvents.append({GetConnectionId(networkRequest), subscriptionId, commandId, QJsonDocument::fromJson(data).object()});

		return true;
	}
};


struct CPublisherFixture
{
	CProtocolEngineStub engine;
	icomp::TSimSharedComponentPtr<CScriptedChangeNotifierComp> notifier;
	icomp::TSimSharedComponentPtr<CTestableFileSystemPublisherComp> publisher;

	CPublisherFixture()
	{
		notifier->InitComponent();
		publisher->InsertMultiAttr("CommandIds", sdl::V1_0::imtbase::COnFileSystemChangedGqlRequest::GetCommandId());
		publisher->SetRef("ChangeNotifier", notifier);
		publisher->InitComponent();
	}

	// The WebSocket request of a subscription, as the servlet makes it
	std::unique_ptr<imtrest::CWebSocketRequest> MakeNetworkRequest(const QByteArray& connectionId, const QByteArray& subscriptionId)
	{
		auto requestPtr = std::make_unique<imtrest::CWebSocketRequest>(engine);
		requestPtr->SetConnectionId(connectionId);
		requestPtr->SetBody(QByteArrayLiteral("{\"type\": \"start\", \"id\": \"") + subscriptionId + QByteArrayLiteral("\"}"));

		return requestPtr;
	}

	bool Subscribe(const imtrest::CWebSocketRequest& networkRequest, const QString& path, QString& error)
	{
		sdl::V1_0::imtbase::OnFileSystemChangedRequestArguments arguments;
		arguments.input.emplace();
		arguments.input->path = path;

		imtgql::CGqlRequest gqlRequest(imtgql::IGqlRequest::RT_SUBSCRIPTION, sdl::V1_0::imtbase::COnFileSystemChangedGqlRequest::GetCommandId());
		if (!sdl::V1_0::imtbase::COnFileSystemChangedGqlRequest::SetupGqlRequest(gqlRequest, arguments)){
			error = QStringLiteral("The request cannot be built");

			return false;
		}
		gqlRequest.AddSimpleField(sdl::V1_0::imtbase::CFileSystemChangedEvent::FileSystemChangedEventFields::Path.toUtf8());

		return publisher->RegisterSubscription(networkRequest.GetQueryId(), gqlRequest, networkRequest, error);
	}
};


} // namespace


void CFileSystemPublisherTest::SubscriptionWatchesItsFolderTest()
{
	CPublisherFixture fixture;
	const std::unique_ptr<imtrest::CWebSocketRequest> networkRequestPtr = fixture.MakeNetworkRequest("connection-1", "subscription-1");

	QString error;
	QVERIFY2(fixture.Subscribe(*networkRequestPtr, QStringLiteral("/Releases"), error), qPrintable(error));
	QCOMPARE(fixture.notifier->watchedPaths, QStringList{QStringLiteral("/Releases")});
}


// Two connections use the same subscription ID; each gets the changes of its own folder only
void CFileSystemPublisherTest::ChangeReachesItsSubscriberOnlyTest()
{
	CPublisherFixture fixture;
	const std::unique_ptr<imtrest::CWebSocketRequest> firstRequestPtr = fixture.MakeNetworkRequest("connection-1", "subscription");
	const std::unique_ptr<imtrest::CWebSocketRequest> secondRequestPtr = fixture.MakeNetworkRequest("connection-2", "subscription");

	QString error;
	QVERIFY2(fixture.Subscribe(*firstRequestPtr, QStringLiteral("/A"), error), qPrintable(error));
	QVERIFY2(fixture.Subscribe(*secondRequestPtr, QStringLiteral("/B"), error), qPrintable(error));

	fixture.notifier->SendChange("watch-2", QStringLiteral("/B"));

	QCOMPARE(fixture.publisher->sentEvents.size(), 1);
	const CTestableFileSystemPublisherComp::SentEvent& sentEvent = fixture.publisher->sentEvents.first();
	QCOMPARE(sentEvent.connectionId, QByteArrayLiteral("connection-2"));
	QCOMPARE(sentEvent.subscriptionId, QByteArrayLiteral("subscription"));
	QCOMPARE(sentEvent.commandId, sdl::V1_0::imtbase::COnFileSystemChangedGqlRequest::GetCommandId());
	QCOMPARE(sentEvent.event.value(sdl::V1_0::imtbase::CFileSystemChangedEvent::FileSystemChangedEventFields::Path).toString(), QStringLiteral("/B"));
}


// A folder that cannot be watched refuses the subscription, which leaves nothing registered behind
void CFileSystemPublisherTest::RefusedWatchRefusesTheSubscriptionTest()
{
	CPublisherFixture fixture;
	const std::unique_ptr<imtrest::CWebSocketRequest> networkRequestPtr = fixture.MakeNetworkRequest("connection-1", "subscription-1");
	fixture.notifier->refusal = QStringLiteral("The folder '/Missing' was not found");

	QString error;
	QVERIFY(!fixture.Subscribe(*networkRequestPtr, QStringLiteral("/Missing"), error));
	QCOMPARE(error, QStringLiteral("The folder '/Missing' was not found"));

	// The same subscription ID is free again
	fixture.notifier->refusal.clear();
	error.clear();
	QVERIFY2(fixture.Subscribe(*networkRequestPtr, QStringLiteral("/Missing"), error), qPrintable(error));
}


// A subscription the publisher refuses (here: a repeated subscription ID) watches no folder
void CFileSystemPublisherTest::RefusedSubscriptionStartsNoWatchTest()
{
	CPublisherFixture fixture;
	const std::unique_ptr<imtrest::CWebSocketRequest> networkRequestPtr = fixture.MakeNetworkRequest("connection-1", "subscription-1");

	QString error;
	QVERIFY2(fixture.Subscribe(*networkRequestPtr, QStringLiteral("/Releases"), error), qPrintable(error));
	QVERIFY(!fixture.Subscribe(*networkRequestPtr, QStringLiteral("/Releases"), error));
	QCOMPARE(fixture.notifier->watchedPaths.size(), 1);
}


// After the subscriber unsubscribes, the watch ends, and a late change is not sent
void CFileSystemPublisherTest::UnsubscribingEndsTheWatchTest()
{
	CPublisherFixture fixture;
	const std::unique_ptr<imtrest::CWebSocketRequest> networkRequestPtr = fixture.MakeNetworkRequest("connection-1", "subscription-1");

	QString error;
	QVERIFY2(fixture.Subscribe(*networkRequestPtr, QStringLiteral("/Releases"), error), qPrintable(error));
	QVERIFY(fixture.publisher->UnregisterSubscription("connection-1", "subscription-1"));
	QCOMPARE(fixture.notifier->removedWatchIds, QByteArrayList{QByteArrayLiteral("watch-1")});

	fixture.notifier->SendChange("watch-1", QStringLiteral("/Releases"));
	QVERIFY(fixture.publisher->sentEvents.isEmpty());
}


// A dropped connection ends the watches of its subscriptions
void CFileSystemPublisherTest::DroppedConnectionEndsTheWatchTest()
{
	CPublisherFixture fixture;
	std::unique_ptr<imtrest::CWebSocketRequest> networkRequestPtr = fixture.MakeNetworkRequest("connection-1", "subscription-1");

	QString error;
	QVERIFY2(fixture.Subscribe(*networkRequestPtr, QStringLiteral("/Releases"), error), qPrintable(error));
	networkRequestPtr.reset();
	QCOMPARE(fixture.notifier->removedWatchIds, QByteArrayList{QByteArrayLiteral("watch-1")});

	fixture.notifier->SendChange("watch-1", QStringLiteral("/Releases"));
	QVERIFY(fixture.publisher->sentEvents.isEmpty());
}


I_ADD_TEST(CFileSystemPublisherTest);
