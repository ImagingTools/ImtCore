// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include "CRemoteFileSystemChangeNotifierTest.h"


#include <functional>
#include <memory>
#include <utility>
#include <QtCore/QJsonArray>
#include <QtCore/QJsonDocument>
#include <QtCore/QJsonObject>
#include <QtCore/QMutex>
#include <QtCore/QThread>
#include <icomp/TModelCompWrap.h>
#include <icomp/TSimComponentWrap.h>
#include <istd/CChangeNotifier.h>
#include <itest/CStandardTestExecutor.h>
#include <imtauth/IAccessTokenProvider.h>
#include <imtbase/imtbase.h>
#include <imtcom/IConnectionStatusProvider.h>
#include <imtclientgql/CRemoteFileSystemChangeNotifierComp.h>
#include <imtclientgql/CSubscriptionManagerComp.h>
#include <imtfile/IFileSystemChangeNotifier.h>
#include <imtgql/CGqlContext.h>
#include <imtgql/CGqlRequest.h>
#include <imtgql/CGqlRequestContextManager.h>
#include <imtrest/CWebSocketProtocolEngineComp.h>
#include <imtrest/CWebSocketRequest.h>
#include <imtrest/IResponseDispatcher.h>
#include <imtrest/ITransport.h>
#include <GeneratedFiles/imtbasesdl/SDL/1.0/CPP/FileSystem.h>


namespace
{


// The delay of the notifications in the tests, and a wait long enough for a notification that is not to come
const int s_notificationDelay = 50;
const int s_quietTime = 4 * s_notificationDelay;


// Keeps the messages the subscription manager sends to the remote server
class CRecordingTransportComp: public icomp::CComponentBase, virtual public imtrest::ITransport
{
public:
	typedef icomp::CComponentBase BaseClass;
	I_BEGIN_COMPONENT(CRecordingTransportComp)
		I_REGISTER_INTERFACE(imtrest::ITransport);
	I_END_COMPONENT

	QList<QJsonObject> GetMessages(const QString& type) const
	{
		QMutexLocker locker(&m_mutex);

		QList<QJsonObject> retVal;
		for (const QJsonObject& message : std::as_const(m_messages)){
			if (message.value(QStringLiteral("type")).toString() == type){
				retVal.append(message);
			}
		}

		return retVal;
	}

	// reimplemented (imtrest::ITransport)
	bool SendData(const QByteArray& data) const override
	{
		QMutexLocker locker(&m_mutex);
		m_messages.append(QJsonDocument::fromJson(data).object());

		return true;
	}

private:
	mutable QMutex m_mutex;
	mutable QList<QJsonObject> m_messages;
};


// Keeps the types of the messages a subscription manager of a server routes to its clients
class CRecordingDispatcherComp: public icomp::CComponentBase, virtual public imtrest::IResponseDispatcher
{
public:
	typedef icomp::CComponentBase BaseClass;
	I_BEGIN_COMPONENT(CRecordingDispatcherComp)
		I_REGISTER_INTERFACE(imtrest::IResponseDispatcher);
	I_END_COMPONENT

	QStringList GetTypes() const
	{
		QMutexLocker locker(&m_mutex);

		return m_types;
	}

	// reimplemented (imtrest::IResponseDispatcher)
	bool SendResponse(const QByteArray& /*requestId*/, imtrest::ConstResponsePtr& /*response*/) const override
	{
		return false;
	}

	bool SendRequest(const QByteArray& /*requestId*/, imtrest::ConstRequestPtr& request) const override
	{
		QMutexLocker locker(&m_mutex);
		m_types.append(QJsonDocument::fromJson(request->GetBody()).object().value(QStringLiteral("type")).toString());

		return true;
	}

private:
	mutable QMutex m_mutex;
	mutable QStringList m_types;
};


class CSilentSubscriptionClient: public imtclientgql::IGqlSubscriptionClient
{
public:
	// reimplemented (imtclientgql::IGqlSubscriptionClient)
	void OnResponseReceived(const QByteArray& /*subscriptionId*/, const QByteArray& /*subscriptionData*/) override
	{
	}

	void OnSubscriptionStatusChanged(const QByteArray& /*subscriptionId*/, const SubscriptionStatus& /*status*/, const QString& /*message*/) override
	{
	}
};


// The connection of the subscription manager to the remote server
class CConnectionStatusComp: public icomp::CComponentBase, virtual public imtcom::IConnectionStatusProvider
{
public:
	typedef icomp::CComponentBase BaseClass;
	I_BEGIN_COMPONENT(CConnectionStatusComp)
		I_REGISTER_INTERFACE(imtcom::IConnectionStatusProvider);
	I_END_COMPONENT

	void SetConnectionStatus(ConnectionStatus status)
	{
		istd::IChangeable::ChangeSet changeSet(status == CS_CONNECTED ? CF_CONNECTED : CF_DISCONNECTED);
		istd::CChangeNotifier notifier(this, &changeSet);
		Q_UNUSED(notifier);

		m_status = status;
	}

	// reimplemented (imtcom::IConnectionStatusProvider)
	ConnectionStatus GetConnectionStatus() const override
	{
		return m_status;
	}

private:
	ConnectionStatus m_status = CS_UNKNOWN;
};


class CTokenProviderComp: public icomp::CComponentBase, virtual public imtauth::IAccessTokenProvider
{
public:
	typedef icomp::CComponentBase BaseClass;
	I_BEGIN_COMPONENT(CTokenProviderComp)
		I_REGISTER_INTERFACE(imtauth::IAccessTokenProvider);
	I_END_COMPONENT

	// reimplemented (imtauth::IAccessTokenProvider)
	QByteArray GetToken(const QByteArray& /*userId*/) const override
	{
		return QByteArrayLiteral("session-token");
	}
};


class CRecordingListener: virtual public imtfile::IFileSystemChangeListener
{
public:
	// Called after the change is recorded
	std::function<void(const QByteArray& watchId)> onChange;

	int GetCount(const QByteArray& watchId) const
	{
		QMutexLocker locker(&m_mutex);

		return static_cast<int>(m_changes.count(watchId));
	}

	QString GetLastPath() const
	{
		QMutexLocker locker(&m_mutex);

		return m_lastPath;
	}

	// reimplemented (imtfile::IFileSystemChangeListener)
	void OnFileSystemChanged(const QByteArray& watchId, const QString& path) override
	{
		{
			QMutexLocker locker(&m_mutex);
			m_changes.append(watchId);
			m_lastPath = path;
		}

		if (onChange){
			onChange(watchId);
		}
	}

private:
	mutable QMutex m_mutex;
	QByteArrayList m_changes;
	QString m_lastPath;
};


struct CNotifierFixture
{
	// The listener outlives the notifier, and the notifier is destroyed before the manager it uses
	CRecordingListener listener;
	icomp::TSimSharedComponentPtr<imtrest::CWebSocketProtocolEngineComp> engine;
	icomp::TSimSharedComponentPtr<CRecordingTransportComp> transport;
	icomp::TSimSharedComponentPtr<icomp::TModelCompWrap<CConnectionStatusComp>> connection;
	icomp::TSimSharedComponentPtr<icomp::TModelCompWrap<CTokenProviderComp>> tokens;
	icomp::TSimSharedComponentPtr<imtclientgql::CSubscriptionManagerComp> manager;
	icomp::TSimSharedComponentPtr<imtclientgql::CRemoteFileSystemChangeNotifierComp> notifier;

	explicit CNotifierFixture(bool withConnection = true)
	{
		engine->InitComponent();
		transport->InitComponent();
		connection->InitComponent();
		tokens->InitComponent();
		manager->SetRef("ProtocolEngine", engine);
		manager->SetRef("SubscriptionSender", transport);
		if (withConnection){
			manager->SetRef("WebLoginStatus", connection);
		}
		manager->SetRef("AccessTokenProvider", tokens);
		manager->InitComponent();
		notifier->SetRef("SubscriptionManager", manager);
		notifier->SetIntAttr("NotificationDelayMs", s_notificationDelay);
		notifier->InitComponent();

		connection->SetConnectionStatus(imtcom::IConnectionStatusProvider::CS_CONNECTED);
	}

	QByteArray AddWatch(const QString& path)
	{
		QByteArray watchId;
		QString error;
		if (!notifier->AddWatch(path, listener, watchId, error)){
			qWarning() << error;
		}

		return watchId;
	}

	// The subscription of the last "start" message
	QByteArray GetLastSubscriptionId()
	{
		const QList<QJsonObject> starts = transport->GetMessages(QStringLiteral("start"));

		return starts.isEmpty() ? QByteArray() : starts.last().value(QStringLiteral("id")).toString().toUtf8();
	}

	int GetStopCount(const QByteArray& subscriptionId)
	{
		int retVal = 0;
		const QList<QJsonObject> stops = transport->GetMessages(QStringLiteral("stop"));
		for (const QJsonObject& stop : stops){
			if (stop.value(QStringLiteral("id")).toString().toUtf8() == subscriptionId){
				++retVal;
			}
		}

		return retVal;
	}

	// A message of the remote server about the subscription
	void Receive(const QString& type, const QByteArray& subscriptionId, const QJsonValue& payload = QJsonValue())
	{
		QJsonObject message{{QStringLiteral("type"), type}, {QStringLiteral("id"), QString::fromUtf8(subscriptionId)}};
		if (!payload.isNull()){
			message.insert(QStringLiteral("payload"), payload);
		}

		imtrest::CWebSocketRequest request(*engine);
		request.SetBody(QJsonDocument(message).toJson(QJsonDocument::Compact));
		manager->ProcessRequest(request);
	}

	void ReceiveChange(const QByteArray& subscriptionId, const QString& path)
	{
		const QJsonObject event{{QStringLiteral("path"), path}};
		Receive(QStringLiteral("data"), subscriptionId,
					QJsonObject{{QStringLiteral("data"), QJsonObject{{QStringLiteral("OnFileSystemChanged"), event}}}});
	}

	void ReceiveRefusal(const QByteArray& subscriptionId)
	{
		Receive(QStringLiteral("error"), subscriptionId,
					QJsonArray{QJsonObject{{QStringLiteral("message"), QStringLiteral("The folder does not exist")}}});
	}
};


} // namespace


// The subscription names the folder in the form of the schema and goes with the token of the session, not with the context of the caller
void CRemoteFileSystemChangeNotifierTest::SubscriptionNamesTheFolderTest()
{
	CNotifierFixture fixture;

	imtgql::CGqlContext callerContext;
	callerContext.SetToken(QByteArrayLiteral("caller-token"));
	callerContext.SetProductId(QByteArrayLiteral("CallerProduct"));
	imtgql::CGqlRequestContextManager::SetContext(&callerContext);
	const QByteArray watchId = fixture.AddWatch(QStringLiteral("/data/in"));
	imtgql::CGqlRequestContextManager::Clear();
	QVERIFY(!watchId.isEmpty());

	const QList<QJsonObject> starts = fixture.transport->GetMessages(QStringLiteral("start"));
	QCOMPARE(starts.size(), 1);

	const QByteArray query = starts[0].value(QStringLiteral("payload")).toObject().value(QStringLiteral("data")).toString().toUtf8();
	imtgql::CGqlRequest receivedRequest;
	qsizetype errorPosition = -1;
	QVERIFY2(receivedRequest.ParseQuery(query, errorPosition), query.constData());
	QCOMPARE(receivedRequest.GetRequestType(), imtgql::IGqlRequest::RT_SUBSCRIPTION);

	const sdl::V1_0::imtbase::COnFileSystemChangedGqlRequest subscriptionRequest(receivedRequest, false);
	QVERIFY(subscriptionRequest.IsValid());
	const sdl::V1_0::imtbase::OnFileSystemChangedRequestArguments& arguments = subscriptionRequest.GetRequestedArguments();
	QVERIFY(arguments.input.has_value() && arguments.input->path.has_value());
	QCOMPARE(*arguments.input->path, QStringLiteral("/data/in"));
	QVERIFY(subscriptionRequest.GetRequestInfo().isPathRequested);

	const QJsonObject headers = starts[0].value(QStringLiteral("headers")).toObject();
	QCOMPARE(headers.value(QString::fromUtf8(imtbase::s_authenticationTokenHeaderId)).toString(), QStringLiteral("session-token"));
	QVERIFY(!headers.contains(QString::fromUtf8(imtbase::s_productIdHeaderId)));
}


// The confirmation of the subscription is reported after the delay, not by AddWatch
void CRemoteFileSystemChangeNotifierTest::ConfirmedSubscriptionIsReportedTest()
{
	CNotifierFixture fixture;
	const QByteArray watchId = fixture.AddWatch(QStringLiteral("/data/in"));
	QVERIFY(!watchId.isEmpty());
	QCOMPARE(fixture.listener.GetCount(watchId), 0);

	fixture.Receive(QStringLiteral("start_ack"), fixture.GetLastSubscriptionId());
	QCOMPARE(fixture.listener.GetCount(watchId), 0);
	QTRY_COMPARE(fixture.listener.GetCount(watchId), 1);
	QCOMPARE(fixture.listener.GetLastPath(), QStringLiteral("/data/in"));

	QTest::qWait(s_quietTime);
	QCOMPARE(fixture.listener.GetCount(watchId), 1);
}


// The changes within the delay are reported once; a later change is reported again
void CRemoteFileSystemChangeNotifierTest::ChangesWithinTheDelayAreReportedOnceTest()
{
	CNotifierFixture fixture;
	const QByteArray watchId = fixture.AddWatch(QStringLiteral("/data/in"));
	const QByteArray subscriptionId = fixture.GetLastSubscriptionId();
	fixture.Receive(QStringLiteral("start_ack"), subscriptionId);
	QTRY_COMPARE(fixture.listener.GetCount(watchId), 1);

	fixture.ReceiveChange(subscriptionId, QStringLiteral("/data/in"));
	fixture.ReceiveChange(subscriptionId, QStringLiteral("/data/in"));
	fixture.ReceiveChange(subscriptionId, QStringLiteral("/data/in"));
	QTRY_COMPARE(fixture.listener.GetCount(watchId), 2);
	QTest::qWait(s_quietTime);
	QCOMPARE(fixture.listener.GetCount(watchId), 2);

	fixture.ReceiveChange(subscriptionId, QStringLiteral("/data/in"));
	QTRY_COMPARE(fixture.listener.GetCount(watchId), 3);
}


// After a reconnection the subscription is sent again, and its confirmation is reported: the changes meanwhile are not known
void CRemoteFileSystemChangeNotifierTest::ReconnectionIsReportedTest()
{
	CNotifierFixture fixture;
	const QByteArray watchId = fixture.AddWatch(QStringLiteral("/data/in"));
	const QByteArray subscriptionId = fixture.GetLastSubscriptionId();
	fixture.Receive(QStringLiteral("start_ack"), subscriptionId);
	QTRY_COMPARE(fixture.listener.GetCount(watchId), 1);

	fixture.connection->SetConnectionStatus(imtcom::IConnectionStatusProvider::CS_DISCONNECTED);
	QTest::qWait(s_quietTime);
	QCOMPARE(fixture.listener.GetCount(watchId), 1);

	fixture.connection->SetConnectionStatus(imtcom::IConnectionStatusProvider::CS_CONNECTED);
	QCOMPARE(fixture.transport->GetMessages(QStringLiteral("start")).size(), 2);
	QCOMPARE(fixture.GetLastSubscriptionId(), subscriptionId);

	fixture.Receive(QStringLiteral("start_ack"), subscriptionId);
	QTRY_COMPARE(fixture.listener.GetCount(watchId), 2);
}


// A refusal is reported once, not after each retry; a refused subscription is not stopped at the server
void CRemoteFileSystemChangeNotifierTest::RefusalIsReportedOnceTest()
{
	CNotifierFixture fixture;
	const QByteArray watchId = fixture.AddWatch(QStringLiteral("/data/missing"));
	const QByteArray subscriptionId = fixture.GetLastSubscriptionId();

	fixture.ReceiveRefusal(subscriptionId);
	QTRY_COMPARE(fixture.listener.GetCount(watchId), 1);

	fixture.ReceiveRefusal(subscriptionId);
	QTest::qWait(s_quietTime);
	QCOMPARE(fixture.listener.GetCount(watchId), 1);

	fixture.notifier->RemoveWatch(watchId);
	QCOMPARE(fixture.GetStopCount(subscriptionId), 0);
}


// A removed watch is not reported, not even a change that came before; its subscription is stopped at the server
void CRemoteFileSystemChangeNotifierTest::RemovedWatchIsNotReportedTest()
{
	CNotifierFixture fixture;
	const QByteArray watchId = fixture.AddWatch(QStringLiteral("/data/in"));
	const QByteArray subscriptionId = fixture.GetLastSubscriptionId();

	fixture.Receive(QStringLiteral("start_ack"), subscriptionId);
	fixture.notifier->RemoveWatch(watchId);
	QCOMPARE(fixture.GetStopCount(subscriptionId), 1);

	fixture.ReceiveChange(subscriptionId, QStringLiteral("/data/in"));
	QTest::qWait(s_quietTime);
	QCOMPARE(fixture.listener.GetCount(watchId), 0);

	// A second removal does nothing
	fixture.notifier->RemoveWatch(watchId);
	QCOMPARE(fixture.GetStopCount(subscriptionId), 1);
}


// The watches of one folder share a subscription, which is stopped with its last watch
void CRemoteFileSystemChangeNotifierTest::WatchesOfOneFolderShareASubscriptionTest()
{
	CNotifierFixture fixture;
	const QByteArray firstWatchId = fixture.AddWatch(QStringLiteral("/data/in"));
	const QByteArray secondWatchId = fixture.AddWatch(QStringLiteral("/data/in"));
	QVERIFY(firstWatchId != secondWatchId);
	QCOMPARE(fixture.transport->GetMessages(QStringLiteral("start")).size(), 1);
	const QByteArray subscriptionId = fixture.GetLastSubscriptionId();

	fixture.Receive(QStringLiteral("start_ack"), subscriptionId);
	QTRY_COMPARE(fixture.listener.GetCount(firstWatchId), 1);
	QTRY_COMPARE(fixture.listener.GetCount(secondWatchId), 1);

	fixture.notifier->RemoveWatch(firstWatchId);
	QCOMPARE(fixture.GetStopCount(subscriptionId), 0);

	fixture.ReceiveChange(subscriptionId, QStringLiteral("/data/in"));
	QTRY_COMPARE(fixture.listener.GetCount(secondWatchId), 2);
	QCOMPARE(fixture.listener.GetCount(firstWatchId), 1);

	fixture.notifier->RemoveWatch(secondWatchId);
	QCOMPARE(fixture.GetStopCount(subscriptionId), 1);
}


// The listener may remove its own watch
void CRemoteFileSystemChangeNotifierTest::ListenerMayRemoveItsWatchTest()
{
	CNotifierFixture fixture;
	fixture.listener.onChange = [&fixture](const QByteArray& watchId){
		fixture.notifier->RemoveWatch(watchId);
	};

	const QByteArray watchId = fixture.AddWatch(QStringLiteral("/data/in"));
	const QByteArray subscriptionId = fixture.GetLastSubscriptionId();
	fixture.Receive(QStringLiteral("start_ack"), subscriptionId);
	QTRY_COMPARE(fixture.listener.GetCount(watchId), 1);
	QCOMPARE(fixture.GetStopCount(subscriptionId), 1);

	fixture.ReceiveChange(subscriptionId, QStringLiteral("/data/in"));
	QTest::qWait(s_quietTime);
	QCOMPARE(fixture.listener.GetCount(watchId), 1);
}


// A watch removed by the listener of another watch is not reported, though its change came with the same notification
void CRemoteFileSystemChangeNotifierTest::WatchRemovedDuringTheNotificationIsNotReportedTest()
{
	CNotifierFixture fixture;
	const QByteArray firstWatchId = fixture.AddWatch(QStringLiteral("/data/in"));
	const QByteArray secondWatchId = fixture.AddWatch(QStringLiteral("/data/in"));
	fixture.listener.onChange = [&fixture, &firstWatchId, &secondWatchId](const QByteArray& watchId){
		fixture.notifier->RemoveWatch(watchId == firstWatchId ? secondWatchId : firstWatchId);
	};

	fixture.Receive(QStringLiteral("start_ack"), fixture.GetLastSubscriptionId());
	QTRY_COMPARE(fixture.listener.GetCount(firstWatchId) + fixture.listener.GetCount(secondWatchId), 1);
	QTest::qWait(s_quietTime);
	QCOMPARE(fixture.listener.GetCount(firstWatchId) + fixture.listener.GetCount(secondWatchId), 1);
}


// A removal in another thread returns after the call of the listener in progress
void CRemoteFileSystemChangeNotifierTest::RemovalWaitsForTheListenerTest()
{
	CNotifierFixture fixture;
	const QByteArray watchId = fixture.AddWatch(QStringLiteral("/data/in"));

	std::unique_ptr<QThread> removalThreadPtr;
	bool isRemovalWaiting = false;
	fixture.listener.onChange = [&fixture, &removalThreadPtr, &isRemovalWaiting](const QByteArray& changedWatchId){
		if (removalThreadPtr != nullptr){
			return;
		}

		removalThreadPtr.reset(QThread::create([&fixture, changedWatchId](){
			fixture.notifier->RemoveWatch(changedWatchId);
		}));
		removalThreadPtr->start();

		// The removal does not end while this call goes on
		isRemovalWaiting = !removalThreadPtr->wait(s_quietTime);
	};

	fixture.Receive(QStringLiteral("start_ack"), fixture.GetLastSubscriptionId());
	QTRY_VERIFY(removalThreadPtr != nullptr);
	QVERIFY(removalThreadPtr->wait(5000));
	QVERIFY(isRemovalWaiting);
	QCOMPARE(fixture.listener.GetCount(watchId), 1);
}


// A subscription the manager does not register is no watch
void CRemoteFileSystemChangeNotifierTest::FailedRegistrationFailsTheWatchTest()
{
	// The manager registers no subscription without a connection status
	CNotifierFixture fixture(false);

	QByteArray watchId;
	QString error;
	QVERIFY(!fixture.notifier->AddWatch(QStringLiteral("/data/in"), fixture.listener, watchId, error));
	QVERIFY(watchId.isEmpty());
	QVERIFY(error.contains(QStringLiteral("/data/in")));
	QVERIFY(fixture.transport->GetMessages(QStringLiteral("start")).isEmpty());
}


// A subscription a server routes to one of its clients is not stopped with its last client: their WebSocket client does not handle a stop
void CRemoteFileSystemChangeNotifierTest::RoutedSubscriptionIsNotStoppedTest()
{
	icomp::TSimSharedComponentPtr<imtrest::CWebSocketProtocolEngineComp> engine;
	icomp::TSimSharedComponentPtr<CRecordingDispatcherComp> dispatcher;
	icomp::TSimSharedComponentPtr<icomp::TModelCompWrap<CConnectionStatusComp>> connection;
	icomp::TSimSharedComponentPtr<imtclientgql::CSubscriptionManagerComp> manager;
	engine->InitComponent();
	dispatcher->InitComponent();
	connection->InitComponent();
	manager->SetRef("ProtocolEngine", engine);
	manager->SetRef("RequestManager", dispatcher);
	manager->SetRef("WebLoginStatus", connection);
	manager->InitComponent();
	connection->SetConnectionStatus(imtcom::IConnectionStatusProvider::CS_CONNECTED);

	imtgql::CGqlRequest request(imtgql::IGqlRequest::RT_SUBSCRIPTION, "OnTestChanged");
	request.AddSimpleField("id");
	imtgql::IGqlContextUniquePtr contextPtr(new imtgql::CGqlContext);
	imtgql::IGqlContext::Headers headers;
	headers.insert("clientid", "client");
	contextPtr->SetHeaders(headers);
	request.SetGqlContext(imtgql::IGqlContextSharedPtr::CreateFromUnique(std::move(contextPtr)));

	CSilentSubscriptionClient client;
	const QByteArray subscriptionId = manager->RegisterSubscription(request, client);
	QVERIFY(!subscriptionId.isEmpty());
	QCOMPARE(dispatcher->GetTypes(), QStringList{QStringLiteral("start")});

	QVERIFY(manager->UnregisterSubscription(subscriptionId, client));
	QCOMPARE(dispatcher->GetTypes(), QStringList{QStringLiteral("start")});
}


I_ADD_TEST(CRemoteFileSystemChangeNotifierTest);
