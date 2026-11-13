// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include "CWebSocketCredentialsTest.h"


#include <algorithm>
#include <atomic>
#include <functional>
#include <memory>
#include <thread>
#include <vector>
#include <QtCore/QElapsedTimer>
#include <QtCore/QJsonArray>
#include <QtCore/QSemaphore>
#include <imod/TModelWrap.h>
#include <icomp/TSimComponentWrap.h>
#include <istd/CChangeNotifier.h>
#include <imtbase/CObjectCollection.h>
#include <imtbase/CCachedObjectCollectionComp.h>
#include <imtbase/IOperationContext.h>
#include <imtclientgql/CObjectCollectionSubscriberComp.h>
#include <imtauth/CUserInfo.h>
#include <itest/CStandardTestExecutor.h>
#include <imtbase/imtbase.h>
#include <imtgql/CGqlContext.h>
#include <imtrest/CWebSocketProtocolEngineComp.h>
#include <imtservergql/CWebSocketServletComp.h>
#include <imtservergql/imtservergql.h>
#include <imtservergql/IServerTokenReceiver.h>
#include <imtauth/CPersonalAccessTokenManagerComp.h>
#include <imtauthgql/CConnectionCodesNotificationPublisherComp.h>
#include <imtauthgql/CCrossTenantMessageNotificationPublisherComp.h>
#include <imtauthgql/CTenantNotificationPublisherComp.h>
#include <imtservergql/CConnectionStatusSubscriberControllerComp.h>
#include <imtservergql/CObjectCollectionChangeNotifierComp.h>
#include <imtservergql/CGqlPublisherCompBase.h>
#include <imtservergql/CPublisherSubscriberBridgeComp.h>


namespace
{


typedef imtgql::IGqlContextCreator Creator;


class CCredentialContextComp: public icomp::CComponentBase, virtual public imtgql::IGqlContextCreator
{
public:
	typedef icomp::CComponentBase BaseClass;
	I_BEGIN_COMPONENT(CCredentialContextComp)
		I_REGISTER_INTERFACE(imtgql::IGqlContextCreator);
	I_END_COMPONENT

	mutable std::atomic<int> requestContexts{0};
	mutable std::atomic<int> newChecks{0};
	mutable std::atomic<int> establishedChecks{0};
	std::atomic<int> renewState{AS_CONFIRMED};
	std::atomic<int> refusal{CCS_FORBIDDEN};
	// The same user throughout: the access is the bound of its token
	std::atomic<int> access{0};
	std::atomic<qint64> generation{0};
	// Runs in an established check before it answers: it may read, wait or block
	std::function<void(const imtgql::IGqlContext::Headers&)> duringCheck;
	// The user the credentials stand for, as the source answers now
	QByteArray userId = "user";
	// Credentials of other users, by access token
	QHash<QByteArray, QByteArray> userOfToken;
	// Credentials an established check cannot confirm, as if its source did not answer
	std::function<bool(const imtgql::IGqlContext::Headers&)> isUnavailable;
	mutable std::atomic<int> runningChecks{0};
	mutable std::atomic<int> maxRunningChecks{0};
	QDateTime sessionExpiresAt;

	imtgql::IGqlContextUniquePtr CreateGqlContext(const imtgql::IGqlContext::Headers&, ContextCreationError& error) const override
	{
		++requestContexts;
		error.status = CCS_OK;
		return CreateAccessContext(access.load());
	}

	imtgql::IGqlContextUniquePtr RenewGqlContext(const imtgql::IGqlContext::Headers& headers, bool isEstablished, AuthorizationState& state, ContextCreationError& error, QDateTime* sessionExpiresAtPtr = nullptr) const override
	{
		if (sessionExpiresAtPtr != nullptr){
			*sessionExpiresAtPtr = sessionExpiresAt;
		}
		++(isEstablished ? establishedChecks : newChecks);
		// The checks of established subscriptions running at once
		const int running = isEstablished ? ++runningChecks : 0;
		int maxRunning = maxRunningChecks.load();
		while (running > maxRunning && !maxRunningChecks.compare_exchange_weak(maxRunning, running)){
		}
		// The access as it was read at the start of the check
		const int accessAtStart = access.load();
		if (isEstablished && duringCheck){
			duringCheck(headers);
		}
		if (isEstablished){
			--runningChecks;
		}
		state = static_cast<AuthorizationState>(renewState.load());
		if (isEstablished && isUnavailable && isUnavailable(headers)){
			state = AS_UNAVAILABLE;
		}
		if (!headers.value(imtbase::s_authenticationTokenHeaderId).startsWith("user-credential") || headers.value(imtbase::s_serverTokenHeaderId) != "server-credential"){
			// Checked with what the client sent at the registration
			state = AS_DENIED;
		}
		if (state != AS_CONFIRMED){
			error.status = state == AS_DENIED ? static_cast<ContextCreationStatus>(refusal.load()) : CCS_INTERNAL_ERROR;
			error.message = QStringLiteral("Credential refused");
			return nullptr;
		}
		return CreateAccessContext(accessAtStart, userOfToken.value(headers.value(imtbase::s_authenticationTokenHeaderId), userId));
	}

	qint64 GetAuthorizationGeneration() const override
	{
		return generation;
	}

	int Checks() const
	{
		return newChecks + establishedChecks;
	}

private:
	static imtgql::IGqlContextUniquePtr CreateAccessContext(int access, const QByteArray& userId = "user")
	{
		QByteArrayList permissions{"/Read"};
		for (int index = 0; index < access; ++index){
			permissions << "/Permission" + QByteArray::number(index);
		}

		imtgql::IGqlContextUniquePtr contextPtr(new imtgql::CGqlContext);
		contextPtr->SetUserId(userId);
		contextPtr->SetPermissionBoundary(permissions);
		return contextPtr;
	}
};


class CSubscriptionControllerComp: public icomp::CComponentBase, virtual public imtgql::IGqlSubscriberController
{
public:
	typedef icomp::CComponentBase BaseClass;
	I_BEGIN_COMPONENT(CSubscriptionControllerComp)
		I_REGISTER_INTERFACE(imtgql::IGqlSubscriberController);
	I_END_COMPONENT

	std::atomic<int> registrations{0};
	std::atomic<int> removals{0};
	std::function<void()> onRegister;
	// The command of a channel to its recipient whatever its permissions
	QByteArray recipientCommandId = "personal";
	// A channel that declares no subscriber: closed where undeclared channels are denied
	bool isDeclared = true;

	bool IsRequestSupported(const imtgql::CGqlRequest&) const override { return true; }
	bool IsOpenFor(SubscriberKind subscriberKind, const imtgql::CGqlRequest& gqlRequest) const override
	{
		return isDeclared && (subscriberKind != SK_RECIPIENT || gqlRequest.GetCommandId() == recipientCommandId);
	}
	bool RegisterSubscription(const QByteArray& subscriptionId, const imtgql::CGqlRequest& gqlRequest, const imtrest::IRequest&, QString&) override
	{
		++registrations;
		{
			QMutexLocker locker(&m_mutex);
			m_commandIds.insert(subscriptionId, gqlRequest.GetCommandId());
		}
		if (onRegister){
			onRegister();
		}
		return true;
	}
	bool UnregisterSubscription(const QByteArray& subscriptionId) override
	{
		++removals;
		QMutexLocker locker(&m_mutex);
		m_commandIds.remove(subscriptionId);
		return true;
	}

	// An event of a command reaches the subscriptions registered for it now
	int Deliver(const QByteArray& commandId) const
	{
		QMutexLocker locker(&m_mutex);
		return int(std::count(m_commandIds.cbegin(), m_commandIds.cend(), commandId));
	}

private:
	QHash<QByteArray, QByteArray> m_commandIds;
	mutable QMutex m_mutex;
};


// A model of the access: a real collection, whose reads notify nobody and whose changes notify its observers
class CAccessModelComp: public icomp::CComponentBase, virtual public imod::IModel
{
public:
	typedef icomp::CComponentBase BaseClass;
	I_BEGIN_COMPONENT(CAccessModelComp)
		I_REGISTER_INTERFACE(imod::IModel);
	I_END_COMPONENT

	imod::TModelWrap<imtbase::CObjectCollection> collection;

	bool AttachObserver(imod::IObserver* observerPtr) override { return collection.AttachObserver(observerPtr); }
	void DetachObserver(imod::IObserver* observerPtr) override { collection.DetachObserver(observerPtr); }
	void DetachAllObservers() override { collection.DetachAllObservers(); }
	bool IsAttached(const imod::IObserver* observerPtr) const override { return collection.IsAttached(observerPtr); }
};


// The server tokens of this server, observable as their manager is in a server
class CObservableTokenManagerComp: public imtauth::CPersonalAccessTokenManagerComp, public imod::CModelBase
{
public:
	// A token revoked or changed
	void ChangeToken()
	{
		istd::IChangeable::ChangeSet changeSet(imtauth::IPersonalAccessTokenManager::CF_TOKEN_CHANGED);
		NotifyBeforeChange(changeSet, false);
		NotifyAfterChange(changeSet);
	}

protected:
	void OnBeginGlobalChanges() override {}
	void OnEndGlobalChanges(const istd::IChangeable::ChangeSet&) override {}
};


class CTokenReceiverComp: public icomp::CComponentBase, virtual public imtservergql::IServerTokenReceiver
{
public:
	typedef icomp::CComponentBase BaseClass;
	I_BEGIN_COMPONENT(CTokenReceiverComp)
		I_REGISTER_INTERFACE(imtservergql::IServerTokenReceiver);
	I_END_COMPONENT

	imtauth::IPersonalAccessTokenManager* managerPtr = nullptr;

	QByteArray GetServerProductId() const override { return "Receiver"; }
	imtauth::IPersonalAccessTokenManager* GetTokenManager() const override { return managerPtr; }
	bool IsEnabled() const override { return true; }
};


// A publisher that declares nothing of its own
class CPlainPublisherComp: public imtservergql::CGqlPublisherCompBase
{
public:
	typedef imtservergql::CGqlPublisherCompBase BaseClass;
	I_BEGIN_COMPONENT(CPlainPublisherComp)
	I_END_COMPONENT
};


// Where a dropped subscription is told so
class CResponseDispatcherComp: public icomp::CComponentBase, virtual public imtrest::IResponseDispatcher
{
public:
	typedef icomp::CComponentBase BaseClass;
	I_BEGIN_COMPONENT(CResponseDispatcherComp)
		I_REGISTER_INTERFACE(imtrest::IResponseDispatcher);
	I_END_COMPONENT

	mutable std::atomic<int> responses{0};
	std::function<void(const QByteArray&)> onResponse;

	bool SendResponse(const QByteArray&, imtrest::ConstResponsePtr& responsePtr) const override
	{
		++responses;
		if (onResponse && responsePtr.IsValid()){
			onResponse(responsePtr->GetData());
		}
		return true;
	}
	bool SendRequest(const QByteArray&, imtrest::ConstRequestPtr&) const override { return false; }
};


class CTestableWebSocketServlet: public imtservergql::CWebSocketServletComp
{
public:
	using CWebSocketServletComp::RegisterSubscription;
	mutable QByteArray response;
	mutable QByteArray error;
	mutable QByteArray errorCode;
	mutable QByteArrayList droppedCodes;
	mutable QMutex errorMutex;

protected:
	imtrest::ConstResponsePtr CreateDataResponse(const QByteArray& data, const imtrest::IRequest&) const override
	{
		response = data;
		return imtrest::ConstResponsePtr();
	}
	imtrest::ConstResponsePtr CreateErrorResponse(const QByteArray& message, const imtrest::IRequest&, bool, const QByteArray& reasonCode) const override
	{
		QMutexLocker locker(&errorMutex);
		error = message;
		errorCode = reasonCode;
		droppedCodes << reasonCode;
		return imtrest::ConstResponsePtr();
	}
};


// The remote read facade has its own model: only subscription delivery notifies it.
class CCacheCollectionComp: public icomp::CComponentBase, public imod::TModelWrap<imtbase::CObjectCollection>
{
public:
	typedef icomp::CComponentBase BaseClass;
	I_BEGIN_COMPONENT(CCacheCollectionComp)
		I_REGISTER_INTERFACE(imtbase::IObjectCollection);
		I_REGISTER_INTERFACE(imod::IModel);
	I_END_COMPONENT

	const imtbase::IObjectCollection* sourcePtr = nullptr;
	const imtgql::IGqlContextCreator* creatorPtr = nullptr;
	mutable int reads = 0;

	bool GetObjectData(const Id& objectId, DataPtr& dataPtr, const iprm::IParamsSet* paramsPtr = nullptr) const override
	{
		++reads;
		if (sourcePtr == nullptr){
			return imtbase::CObjectCollection::GetObjectData(objectId, dataPtr, paramsPtr);
		}

		Creator::ContextCreationError error;
		const auto contextPtr = creatorPtr->CreateGqlContext({}, error);
		const QByteArray permission = objectId == "expanded" ? "/Permission1" : "/Permission0";
		if (error.status != Creator::CCS_OK || !contextPtr || !contextPtr->GetPermissionBoundary().contains(permission)){
			return false;
		}

		return sourcePtr->GetObjectData(objectId, dataPtr, paramsPtr);
	}

protected:
	istd::IChangeableUniquePtr CreateObjectInstance(const QByteArray&) const override
	{
		return new imtauth::CUserInfo;
	}
};


class CCachePublisherComp: public imtservergql::CObjectCollectionChangeNotifierComp
{
public:
	std::atomic<int> registrations{0};
	std::atomic<int> removals{0};

	const imtgql::IGqlContext* RegisteredContext() const
	{
		QMutexLocker locker(&m_mutex);
		return m_registeredSubscribers.isEmpty() ? nullptr : m_registeredSubscribers.first().gqlRequest.GetRequestContext();
	}

	bool RegisterSubscription(const QByteArray& id, const imtgql::CGqlRequest& request, const imtrest::IRequest& networkRequest, QString& error) override
	{
		++registrations;
		return CObjectCollectionChangeNotifierComp::RegisterSubscription(id, request, networkRequest, error);
	}

	bool UnregisterSubscription(const QByteArray& id) override
	{
		++removals;
		return CObjectCollectionChangeNotifierComp::UnregisterSubscription(id);
	}
};


// In-process transport only; publisher, subscription decoder and cache remain production components.
class CCacheSubscriptionManagerComp: public icomp::CComponentBase, virtual public imtclientgql::IGqlSubscriptionManager
{
public:
	typedef icomp::CComponentBase BaseClass;
	I_BEGIN_COMPONENT(CCacheSubscriptionManagerComp)
		I_REGISTER_INTERFACE(imtclientgql::IGqlSubscriptionManager);
	I_END_COMPONENT

	imtclientgql::IGqlSubscriptionClient* clientPtr = nullptr;
	QByteArray commandId;

	QByteArray RegisterSubscription(const imtgql::IGqlRequest& request, imtclientgql::IGqlSubscriptionClient& client) override
	{
		commandId = request.GetCommandId();
		clientPtr = &client;
		return "subscription";
	}

	bool UnregisterSubscription(const QByteArray&, const imtclientgql::IGqlSubscriptionClient&) override
	{
		clientPtr = nullptr;
		return true;
	}
};


class CObservableCacheComp: public imod::TModelWrap<imtbase::CCachedObjectCollectionComp>
{
public:
	typedef imod::TModelWrap<imtbase::CCachedObjectCollectionComp> BaseClass;
	I_BEGIN_COMPONENT(CObservableCacheComp)
		I_REGISTER_INTERFACE(imod::IModel);
	I_END_COMPONENT
};


struct CSubscriptionFixture
{
	icomp::TSimSharedComponentPtr<CCredentialContextComp> creator;
	icomp::TSimSharedComponentPtr<CSubscriptionControllerComp> controller;
	icomp::TSimSharedComponentPtr<CResponseDispatcherComp> dispatcher;
	// A model of the access: a collection the authorization reads, changed when the access changes
	icomp::TSimSharedComponentPtr<CAccessModelComp> users;
	icomp::TSimSharedComponentPtr<CObservableTokenManagerComp> serverTokens;
	icomp::TSimSharedComponentPtr<CTokenReceiverComp> receiver;
	icomp::TSimSharedComponentPtr<CTestableWebSocketServlet> servlet;
	imtrest::CWebSocketProtocolEngineComp engine;
	imtrest::CWebSocketRequest request{engine};
	imtrest::CWebSocketRequest otherRequest{engine};

	explicit CSubscriptionFixture(int maxCheckInterval = 1, int gracePeriod = 60,
				const icomp::TSimSharedComponentPtr<CCachePublisherComp>* publisherPtr = nullptr,
				const icomp::TSimSharedComponentPtr<CObservableCacheComp>* cachePtr = nullptr,
				bool isUndeclaredChannelDenied = false)
	{
		creator->access = 1;
		creator->InitComponent();
		controller->InitComponent();
		dispatcher->InitComponent();
		users->InitComponent();
		serverTokens->InitComponent();
		receiver->managerPtr = &*serverTokens;
		receiver->InitComponent();
		servlet->SetRef("ServerTokenReceiver", receiver);
		servlet->SetRef("GqlContextCreator", creator);
		servlet->SetRef("RequestManager", dispatcher);
		if (publisherPtr != nullptr){
			servlet->InsertMultiRef("GqlSubscriberControllers", *publisherPtr);
		}
		else{
			servlet->InsertMultiRef("GqlSubscriberControllers", controller);
		}
		servlet->InsertMultiRef("AccessModels", users);
		if (cachePtr != nullptr){
			servlet->InsertMultiRef("AccessModels", *cachePtr);
		}
		servlet->SetIntAttr("MaxCheckInterval", maxCheckInterval);
		servlet->SetIntAttr("GracePeriod", gracePeriod);
		servlet->SetBoolAttr("DenyAnonymousSubscriptions", isUndeclaredChannelDenied);
		servlet->SetBoolAttr("DenyUndeclaredUserSubscriptions", isUndeclaredChannelDenied);
		servlet->InitComponent();
		Prepare(request, "subscription", "user-credential");
		Prepare(otherRequest, "other", "user-credential-2");
	}

	static void Prepare(imtrest::CWebSocketRequest& webSocketRequest, const QString& subscriptionId, const QString& token, const QString& commandId = QStringLiteral("changed"))
	{
		webSocketRequest.SetMethodType(imtrest::CWebSocketRequest::MT_START);
		QJsonObject headers;
		headers.insert(QString::fromUtf8(imtbase::s_authenticationTokenHeaderId), token);
		headers.insert(QString::fromUtf8(imtbase::s_serverTokenHeaderId), "server-credential");
		QJsonObject message;
		message.insert("id", subscriptionId);
		message.insert("type", "start");
		message.insert("headers", headers);
		message.insert("payload", QJsonObject{{"query", QStringLiteral("subscription { %1 { id } }").arg(commandId)}});
		webSocketRequest.SetBody(QJsonDocument(message).toJson());
	}

	// The access changed at the authorization server
	void ChangeAccess()
	{
		istd::CChangeNotifier notifier(&users->collection);
	}

	QByteArrayList DroppedCodes()
	{
		QMutexLocker locker(&servlet->errorMutex);
		return servlet->droppedCodes;
	}
};


// Lets a check go on when released; a check left waiting is released at the end
class CCheckGate
{
public:
	~CCheckGate() { m_semaphore.release(1000); }
	void Wait() { m_semaphore.tryAcquire(1, 10000); }
	void Release() { m_semaphore.release(1000); }

private:
	QSemaphore m_semaphore;
};


} // namespace


void CWebSocketCredentialsTest::RegistrationWithoutChange()
{
	CSubscriptionFixture fixture;
	fixture.servlet->RegisterSubscription(fixture.request);
	QVERIFY2(fixture.servlet->error.isEmpty(), fixture.servlet->error.constData());
	QVERIFY(fixture.servlet->response.contains("start_ack"));

	// Registered with the access read for it, as a new subscription; nothing from the cache of the requests
	QCOMPARE(fixture.creator->newChecks.load(), 1);
	QCOMPARE(fixture.creator->establishedChecks.load(), 0);
	QCOMPARE(fixture.creator->requestContexts.load(), 0);
	QCOMPARE(fixture.controller->registrations.load(), 1);
	QCOMPARE(fixture.controller->removals.load(), 0);
}


void CWebSocketCredentialsTest::RefusedCredentialsAreNotRegistered_data()
{
	QTest::addColumn<int>("state");
	QTest::addColumn<int>("refusal");
	QTest::addColumn<QByteArray>("prefix");
	QTest::newRow("revoked") << int(Creator::AS_DENIED) << int(Creator::CCS_FORBIDDEN) << QByteArray("Forbidden: ");
	QTest::newRow("expired-access-token") << int(Creator::AS_DENIED) << int(Creator::CCS_UNAUTHORIZED) << QByteArray("Unauthorized: ");
	QTest::newRow("unavailable") << int(Creator::AS_UNAVAILABLE) << int(Creator::CCS_FORBIDDEN) << QByteArray("Credential refused");
	QTest::newRow("always-stale") << int(Creator::AS_STALE) << int(Creator::CCS_FORBIDDEN) << QByteArray("The access kept changing");
}


void CWebSocketCredentialsTest::RefusedCredentialsAreNotRegistered()
{
	QFETCH(int, state);
	QFETCH(int, refusal);
	QFETCH(QByteArray, prefix);
	CSubscriptionFixture fixture;
	fixture.creator->renewState = state;
	fixture.creator->refusal = refusal;
	fixture.servlet->RegisterSubscription(fixture.request);

	// Not even for a moment: the publisher never sees it, so it never pushes to it
	QCOMPARE(fixture.controller->registrations.load(), 0);
	QVERIFY(fixture.servlet->response.isEmpty());
	QVERIFY2(fixture.servlet->error.startsWith(prefix), fixture.servlet->error.constData());
	QCOMPARE(fixture.creator->newChecks.load(), state == Creator::AS_STALE ? 3 : 1);
}


void CWebSocketCredentialsTest::RegistrationRollback_data()
{
	QTest::addColumn<int>("failure");
	QTest::newRow("denied") << 0;
	QTest::newRow("unavailable") << 1;
	QTest::newRow("access-changed") << 2;
	QTest::newRow("always-stale") << 3;
}


void CWebSocketCredentialsTest::RegistrationRollback()
{
	QFETCH(int, failure);
	CSubscriptionFixture fixture;
	fixture.controller->onRegister = [&](){
		++fixture.creator->generation;
		if (failure == 0){
			fixture.creator->renewState = Creator::AS_DENIED;
		}
		else if (failure == 1){
			fixture.creator->renewState = Creator::AS_UNAVAILABLE;
		}
		else if (failure == 2){
			--fixture.creator->access;
		}
		else{
			fixture.creator->renewState = Creator::AS_STALE;
		}
	};
	fixture.servlet->RegisterSubscription(fixture.request);
	QCOMPARE(fixture.controller->registrations.load(), 1);
	QCOMPARE(fixture.controller->removals.load(), 1);
	QVERIFY(!fixture.servlet->error.isEmpty());
	QVERIFY(fixture.servlet->response.isEmpty());
	QCOMPARE(fixture.creator->newChecks.load(), 1 + (failure == 3 ? 3 : 1));
}


void CWebSocketCredentialsTest::RegistrationKeptWithSameAccess()
{
	CSubscriptionFixture fixture;
	fixture.controller->onRegister = [&](){ ++fixture.creator->generation; };
	fixture.servlet->RegisterSubscription(fixture.request);
	QVERIFY2(fixture.servlet->error.isEmpty(), fixture.servlet->error.constData());
	QVERIFY(fixture.servlet->response.contains("start_ack"));
	QCOMPARE(fixture.creator->newChecks.load(), 2);
	QCOMPARE(fixture.controller->removals.load(), 0);
}


void CWebSocketCredentialsTest::CheckOfSubscription_data()
{
	QTest::addColumn<int>("state");
	QTest::addColumn<bool>("changed");
	QTest::addColumn<bool>("dropped");
	QTest::newRow("same-access") << int(Creator::AS_CONFIRMED) << false << false;
	QTest::newRow("access-changed") << int(Creator::AS_CONFIRMED) << true << true;
	QTest::newRow("denied") << int(Creator::AS_DENIED) << false << true;
	QTest::newRow("unavailable") << int(Creator::AS_UNAVAILABLE) << true << false;
	QTest::newRow("stale") << int(Creator::AS_STALE) << true << false;
}


void CWebSocketCredentialsTest::CheckOfSubscription()
{
	QFETCH(int, state);
	QFETCH(bool, changed);
	QFETCH(bool, dropped);
	CSubscriptionFixture fixture;
	fixture.servlet->RegisterSubscription(fixture.request);
	QVERIFY(fixture.servlet->response.contains("start_ack"));

	fixture.creator->renewState = state;
	if (changed){
		--fixture.creator->access;
	}

	// An established subscription is checked as one: an expired access token may hold there
	if (dropped){
		QTRY_COMPARE_WITH_TIMEOUT(fixture.controller->removals.load(), 1, 5000);
	}
	else{
		QTRY_VERIFY_WITH_TIMEOUT(fixture.creator->establishedChecks.load() >= 1, 5000);
		QTest::qWait(300);
		QCOMPARE(fixture.controller->removals.load(), 0);
	}
	QVERIFY(fixture.creator->establishedChecks.load() >= 1);
	QCOMPARE(fixture.creator->newChecks.load(), 1);
}


void CWebSocketCredentialsTest::AnonymousSubscription()
{
	CSubscriptionFixture fixture;
	fixture.request.SetBody(R"({"id":"subscription","type":"start","payload":{"query":"subscription { changed { id } }"}})");
	fixture.controller->onRegister = [&](){ ++fixture.creator->generation; };
	fixture.servlet->RegisterSubscription(fixture.request);
	QVERIFY(fixture.servlet->error.isEmpty());
	QVERIFY(fixture.servlet->response.contains("start_ack"));
	QCOMPARE(fixture.creator->requestContexts.load(), 1);
	QTest::qWait(1200);
	QCOMPARE(fixture.creator->Checks(), 0);

	// A channel that declares no subscriber refuses for good, with the code that says so
	CSubscriptionFixture closedFixture(1, 60, nullptr, nullptr, true);
	closedFixture.controller->isDeclared = false;
	closedFixture.servlet->RegisterSubscription(closedFixture.request);
	QVERIFY(!closedFixture.servlet->error.isEmpty());
	QCOMPARE(closedFixture.servlet->errorCode, imtservergql::s_subscriptionForbiddenCode);
	QCOMPARE(closedFixture.controller->registrations.load(), 0);
}


void CWebSocketCredentialsTest::RegistrationRefusalIsTyped_data()
{
	QTest::addColumn<int>("state");
	QTest::addColumn<int>("refusal");
	QTest::addColumn<QByteArray>("code");

	QTest::newRow("unavailable") << int(Creator::AS_UNAVAILABLE) << int(Creator::CCS_INTERNAL_ERROR) << imtservergql::s_subscriptionAuthorizationUnavailableCode;
	QTest::newRow("unauthorized") << int(Creator::AS_DENIED) << int(Creator::CCS_UNAUTHORIZED) << imtservergql::s_subscriptionUnauthorizedCode;
	QTest::newRow("forbidden") << int(Creator::AS_DENIED) << int(Creator::CCS_FORBIDDEN) << imtservergql::s_subscriptionForbiddenCode;
}


void CWebSocketCredentialsTest::RegistrationRefusalIsTyped()
{
	QFETCH(int, state);
	QFETCH(int, refusal);
	QFETCH(QByteArray, code);

	// A client subscribing again after an unavailable authorization is told the same, not a refusal
	CSubscriptionFixture fixture;
	fixture.creator->renewState = state;
	fixture.creator->refusal = refusal;
	fixture.servlet->RegisterSubscription(fixture.request);

	QCOMPARE(fixture.servlet->errorCode, code);
	QCOMPARE(fixture.controller->registrations.load(), 0);
}


void CWebSocketCredentialsTest::NoCheckWithoutChangeBeforeTheControl()
{
	CSubscriptionFixture fixture(300);
	fixture.servlet->RegisterSubscription(fixture.request);
	QVERIFY(fixture.servlet->response.contains("start_ack"));

	QTest::qWait(1500);
	QCOMPARE(fixture.creator->establishedChecks.load(), 0);
}


void CWebSocketCredentialsTest::ChangeOfTheAccessChecksAgain()
{
	CSubscriptionFixture fixture(300);
	fixture.servlet->RegisterSubscription(fixture.request);
	--fixture.creator->access;

	fixture.ChangeAccess();
	QTRY_COMPARE_WITH_TIMEOUT(fixture.controller->removals.load(), 1, 2000);
	QCOMPARE(fixture.DroppedCodes(), QByteArrayList{imtservergql::s_subscriptionPermissionsChangedCode});
}


void CWebSocketCredentialsTest::ChangeOfAUserChecksItsSubscriptionsOnly()
{
	CSubscriptionFixture fixture(300);
	fixture.creator->userOfToken.insert("user-credential-2", "other-user");
	fixture.servlet->RegisterSubscription(fixture.request);
	QVERIFY(fixture.servlet->response.contains("start_ack"));
	fixture.servlet->RegisterSubscription(fixture.otherRequest);
	QVERIFY(fixture.servlet->response.contains("start_ack"));

	QMutex mutex;
	QByteArrayList checkedTokens;
	fixture.creator->duringCheck = [&mutex, &checkedTokens](const imtgql::IGqlContext::Headers& headers){
		QMutexLocker locker(&mutex);
		checkedTokens << headers.value(imtbase::s_authenticationTokenHeaderId);
	};
	auto changeObject = [&fixture](const QByteArray& objectId){
		istd::IChangeable::ChangeSet changeSet(imtbase::IObjectCollection::CF_OBJECT_DATA_CHANGED);
		changeSet.SetChangeInfo(imtbase::IObjectCollection::CN_OBJECT_DATA_CHANGED, objectId);
		istd::CChangeNotifier notifier(&fixture.users->collection, &changeSet);
	};

	// The data of a subscribed user: its subscriptions only
	changeObject("other-user");
	QTRY_COMPARE_WITH_TIMEOUT(fixture.creator->establishedChecks.load(), 1, 2000);
	QTest::qWait(500);
	{
		QMutexLocker locker(&mutex);
		QCOMPARE(checkedTokens, QByteArrayList{"user-credential-2"});
	}

	// An object that is no subscribed user, a role for example: everybody
	changeObject("role");
	QTRY_COMPARE_WITH_TIMEOUT(fixture.creator->establishedChecks.load(), 3, 2000);
	QCOMPARE(fixture.controller->removals.load(), 0);
}


void CWebSocketCredentialsTest::PassedSessionDeadlineIsNotUnknown()
{
	CSubscriptionFixture fixture(300);
	fixture.creator->sessionExpiresAt = QDateTime::currentDateTimeUtc().addSecs(-60);
	fixture.servlet->RegisterSubscription(fixture.request);
	QCOMPARE(fixture.controller->registrations.load(), 1);
	QTRY_COMPARE_WITH_TIMEOUT(fixture.creator->establishedChecks.load(), 1, 12000);
}


void CWebSocketCredentialsTest::ChangesDuringACheckCoalesce()
{
	CSubscriptionFixture fixture(300);
	CCheckGate gate;
	fixture.creator->duringCheck = [&gate](const imtgql::IGqlContext::Headers&){ gate.Wait(); };
	fixture.servlet->RegisterSubscription(fixture.request);

	fixture.ChangeAccess();
	QTRY_COMPARE_WITH_TIMEOUT(fixture.creator->establishedChecks.load(), 1, 2000);

	// Changes during the check: one check after it, not one for each
	for (int index = 0; index < 20; ++index){
		fixture.ChangeAccess();
	}
	fixture.creator->duringCheck = nullptr;
	gate.Release();
	QTRY_COMPARE_WITH_TIMEOUT(fixture.creator->establishedChecks.load(), 2, 2000);
	QTest::qWait(500);
	QCOMPARE(fixture.creator->establishedChecks.load(), 2);
	QCOMPARE(fixture.controller->removals.load(), 0);
}


void CWebSocketCredentialsTest::CheckDoesNotTriggerItself()
{
	// A check reads through the model of the access: reading is no change of the access
	CSubscriptionFixture fixture(300);
	imtbase::IObjectCollection& users = fixture.users->collection;
	fixture.creator->duringCheck = [&users](const imtgql::IGqlContext::Headers&){
		users.GetElementIds();
		imtbase::IObjectCollection::DataPtr dataPtr;
		users.GetObjectData("user", dataPtr);
	};
	fixture.servlet->RegisterSubscription(fixture.request);

	fixture.ChangeAccess();
	QTRY_COMPARE_WITH_TIMEOUT(fixture.creator->establishedChecks.load(), 1, 2000);
	QTest::qWait(1500);
	QCOMPARE(fixture.creator->establishedChecks.load(), 1);
}


void CWebSocketCredentialsTest::GraceEndsAsTemporarilyUnavailable()
{
	CSubscriptionFixture fixture(300, 1);
	fixture.servlet->RegisterSubscription(fixture.request);
	fixture.creator->renewState = Creator::AS_UNAVAILABLE;

	fixture.ChangeAccess();
	QTRY_COMPARE_WITH_TIMEOUT(fixture.creator->establishedChecks.load(), 1, 2000);
	QTest::qWait(600);
	QCOMPARE(fixture.controller->removals.load(), 0);

	// Not a refusal: the client subscribes again later
	QTRY_COMPARE_WITH_TIMEOUT(fixture.controller->removals.load(), 1, 2000);
	QCOMPARE(fixture.DroppedCodes(), QByteArrayList{imtservergql::s_subscriptionAuthorizationUnavailableCode});
}


void CWebSocketCredentialsTest::HungCheckHoldsBackNothing()
{
	CSubscriptionFixture fixture(300, 1);
	CCheckGate gate;
	fixture.creator->duringCheck = [&gate](const imtgql::IGqlContext::Headers& headers){
		if (headers.value(imtbase::s_authenticationTokenHeaderId) == "user-credential"){
			gate.Wait();
		}
	};
	fixture.servlet->RegisterSubscription(fixture.request);
	fixture.servlet->RegisterSubscription(fixture.otherRequest);
	QCOMPARE(fixture.controller->registrations.load(), 2);

	// The other credentials are checked meanwhile; the hung check ends its subscription at the end of the grace period
	fixture.ChangeAccess();
	QTRY_COMPARE_WITH_TIMEOUT(fixture.creator->establishedChecks.load(), 2, 2000);
	QTRY_COMPARE_WITH_TIMEOUT(fixture.controller->removals.load(), 1, 3000);
	QCOMPARE(fixture.DroppedCodes(), QByteArrayList{imtservergql::s_subscriptionAuthorizationUnavailableCode});

	// Its late answer changes nothing
	gate.Release();
	QTest::qWait(500);
	QCOMPARE(fixture.controller->removals.load(), 1);
	QCOMPARE(fixture.DroppedCodes(), QByteArrayList{imtservergql::s_subscriptionAuthorizationUnavailableCode});
}


void CWebSocketCredentialsTest::RemovalDuringACheck()
{
	CSubscriptionFixture fixture(300);
	CCheckGate gate;
	fixture.creator->duringCheck = [&gate](const imtgql::IGqlContext::Headers&){ gate.Wait(); };
	fixture.servlet->RegisterSubscription(fixture.request);
	++fixture.creator->access;

	fixture.ChangeAccess();
	QTRY_COMPARE_WITH_TIMEOUT(fixture.creator->establishedChecks.load(), 1, 2000);

	// The client goes away during the check: its answer is not used, nothing is sent to a gone request
	fixture.servlet->OnRequestDestroyed(&fixture.request);
	gate.Release();
	QTest::qWait(500);
	QCOMPARE(fixture.controller->removals.load(), 0);
	QVERIFY(fixture.DroppedCodes().isEmpty());
}


void CWebSocketCredentialsTest::LateAnswerAfterSubscribingAgain()
{
	CSubscriptionFixture fixture(300);
	CCheckGate gate;
	fixture.creator->duringCheck = [&gate](const imtgql::IGqlContext::Headers&){ gate.Wait(); };
	fixture.servlet->RegisterSubscription(fixture.request);

	// A check reads the access, then waits for the network
	fixture.ChangeAccess();
	QTRY_COMPARE_WITH_TIMEOUT(fixture.creator->establishedChecks.load(), 1, 2000);

	// Meanwhile the client goes away and subscribes again with the same credentials, its access changed
	--fixture.creator->access;
	fixture.servlet->OnRequestDestroyed(&fixture.request);
	fixture.servlet->RegisterSubscription(fixture.request);
	QCOMPARE(fixture.controller->registrations.load(), 2);

	// The answer of the earlier check is not the one of the new subscription
	fixture.creator->duringCheck = nullptr;
	gate.Release();
	QTest::qWait(500);
	QCOMPARE(fixture.controller->removals.load(), 0);
	QVERIFY(fixture.DroppedCodes().isEmpty());
}


void CWebSocketCredentialsTest::ExhaustedPoolHoldsBackNothing()
{
	CSubscriptionFixture fixture(300, 2);
	CCheckGate gate;
	const auto isHung = [](const imtgql::IGqlContext::Headers& headers){
		return headers.value(imtbase::s_authenticationTokenHeaderId) != "user-credential-5";
	};
	fixture.creator->duringCheck = [&gate, isHung](const imtgql::IGqlContext::Headers& headers){
		if (isHung(headers)){
			gate.Wait();
		}
	};
	fixture.creator->isUnavailable = isHung;

	std::vector<std::unique_ptr<imtrest::CWebSocketRequest>> requests;
	const auto subscribe = [&fixture, &requests](int index){
		requests.push_back(std::make_unique<imtrest::CWebSocketRequest>(fixture.engine));
		CSubscriptionFixture::Prepare(*requests.back(), QStringLiteral("subscription-%1").arg(index), QStringLiteral("user-credential-%1").arg(index));
		fixture.servlet->RegisterSubscription(*requests.back());
	};
	for (int index = 1; index <= 4; ++index){
		subscribe(index);
	}

	// Four checks hold every thread of the pool
	fixture.ChangeAccess();
	QTRY_COMPARE_WITH_TIMEOUT(fixture.creator->establishedChecks.load(), 4, 2000);

	// The fifth waits for a thread; the four running are not checked twice
	subscribe(5);
	QCOMPARE(fixture.controller->registrations.load(), 5);
	fixture.ChangeAccess();
	QTest::qWait(300);
	QCOMPARE(fixture.creator->establishedChecks.load(), 4);
	QCOMPARE(fixture.creator->maxRunningChecks.load(), 4);

	// Their requests time out: the threads are free again, the fifth is checked and confirmed
	gate.Release();
	QTRY_COMPARE_WITH_TIMEOUT(fixture.creator->establishedChecks.load(), 5, 2000);

	// The four could not be confirmed: they end with their grace period, the fifth stays
	QTRY_COMPARE_WITH_TIMEOUT(fixture.controller->removals.load(), 4, 4000);
	QTest::qWait(300);
	QCOMPARE(fixture.controller->removals.load(), 4);
	QCOMPARE(fixture.DroppedCodes(), QByteArrayList(4, imtservergql::s_subscriptionAuthorizationUnavailableCode));

	fixture.creator->isUnavailable = nullptr;
	fixture.creator->duringCheck = nullptr;
}


void CWebSocketCredentialsTest::StopWithChecksRunning()
{
	auto fixturePtr = std::make_unique<CSubscriptionFixture>(300);
	CCheckGate gate;
	fixturePtr->creator->duringCheck = [&gate](const imtgql::IGqlContext::Headers&){ gate.Wait(); };
	fixturePtr->servlet->RegisterSubscription(fixturePtr->request);
	fixturePtr->servlet->RegisterSubscription(fixturePtr->otherRequest);

	// Many changes during the checks: still one check of each set at a time
	fixturePtr->ChangeAccess();
	QTRY_COMPARE_WITH_TIMEOUT(fixturePtr->creator->establishedChecks.load(), 2, 2000);
	for (int index = 0; index < 20; ++index){
		fixturePtr->ChangeAccess();
	}
	QTest::qWait(200);
	QCOMPARE(fixturePtr->creator->establishedChecks.load(), 2);
	QCOMPARE(fixturePtr->creator->maxRunningChecks.load(), 2);

	// The server stops while the checks wait for the network: it stops once their requests end, they touch nothing afterwards
	std::thread releaser([&gate](){
		std::this_thread::sleep_for(std::chrono::milliseconds(500));
		gate.Release();
	});
	auto creator = fixturePtr->creator;
	auto controller = fixturePtr->controller;
	QElapsedTimer stopTime;
	stopTime.start();
	fixturePtr.reset();
	const qint64 stopMs = stopTime.elapsed();
	releaser.join();
	QVERIFY2(stopMs >= 300 && stopMs < 5000, qPrintable(QString::number(stopMs)));
	QCOMPARE(controller->removals.load(), 0);
	QCOMPARE(creator->establishedChecks.load(), 2);
}


void CWebSocketCredentialsTest::ExpiredQueuedCheckDoesNotReadTheSource()
{
	CSubscriptionFixture fixture(300, 1);
	CCheckGate gate;
	fixture.creator->duringCheck = [&gate](const imtgql::IGqlContext::Headers&){ gate.Wait(); };
	std::vector<std::unique_ptr<imtrest::CWebSocketRequest>> requests;
	for (int index = 0; index < 5; ++index){
		requests.push_back(std::make_unique<imtrest::CWebSocketRequest>(fixture.engine));
		CSubscriptionFixture::Prepare(*requests.back(), QStringLiteral("queued-%1").arg(index), QStringLiteral("user-credential-%1").arg(index));
		fixture.servlet->RegisterSubscription(*requests.back());
	}

	QCOMPARE(fixture.controller->registrations.load(), 5);
	fixture.ChangeAccess();
	QTRY_COMPARE_WITH_TIMEOUT(fixture.creator->establishedChecks.load(), 4, 2000);
	QTRY_COMPARE_WITH_TIMEOUT(fixture.controller->removals.load(), 5, 3000);
	gate.Release();
	QTRY_COMPARE_WITH_TIMEOUT(fixture.creator->runningChecks.load(), 0, 2000);
	QTest::qWait(200);
	QCOMPARE(fixture.creator->establishedChecks.load(), 4);
}


void CWebSocketCredentialsTest::ChangedServerTokenChecksAgain()
{
	// The manager of the server tokens of this server is observed without being listed among the models of the access
	CSubscriptionFixture fixture(300);
	fixture.servlet->RegisterSubscription(fixture.request);
	--fixture.creator->access;

	fixture.serverTokens->ChangeToken();
	QTRY_COMPARE_WITH_TIMEOUT(fixture.controller->removals.load(), 1, 2000);
	QCOMPARE(fixture.DroppedCodes(), QByteArrayList{imtservergql::s_subscriptionPermissionsChangedCode});
}


void CWebSocketCredentialsTest::MoreAccessKeepsTheSubscription()
{
	CSubscriptionFixture fixture(300);
	fixture.servlet->RegisterSubscription(fixture.request);

	// {A} then {A, B} then {A} again: compared with the registration, nothing ends it
	for (const int change : {+1, -1}){
		fixture.creator->access += change;
		const int checksBefore = fixture.creator->establishedChecks.load();
		fixture.ChangeAccess();
		QTRY_COMPARE_WITH_TIMEOUT(fixture.creator->establishedChecks.load(), checksBefore + 1, 2000);
		QTest::qWait(200);
		QCOMPARE(fixture.controller->removals.load(), 0);
	}
	QVERIFY(fixture.DroppedCodes().isEmpty());

	// Less than at the registration ends it
	--fixture.creator->access;
	fixture.ChangeAccess();
	QTRY_COMPARE_WITH_TIMEOUT(fixture.controller->removals.load(), 1, 2000);
	QCOMPARE(fixture.DroppedCodes(), QByteArrayList{imtservergql::s_subscriptionPermissionsChangedCode});
}


void CWebSocketCredentialsTest::MoreAccessKeepsTheCacheFresh()
{
	icomp::TSimSharedComponentPtr<CCacheCollectionComp> source;
	source->InitComponent();
	imtauth::CUserInfo initial;
	initial.SetMail("before");
	QCOMPARE(source->InsertNewObject("User", "Original", "", &initial, "original"), QByteArray("original"));
	QCOMPARE(source->InsertNewObject("User", "Expanded", "", &initial, "expanded"), QByteArray("expanded"));

	icomp::TSimSharedComponentPtr<CResponseDispatcherComp> delivery;
	delivery->InitComponent();
	icomp::TSimSharedComponentPtr<CCachePublisherComp> publisher;
	publisher->SetRef("ObjectCollection", source);
	publisher->SetRef("RequestManager", delivery);
	publisher->SetIdAttr("CollectionId", "Users");
	publisher->SetBoolAttr("OpenToUsers", true);
	publisher->InitComponent();

	CSubscriptionFixture fixture(300, 60, &publisher);
	icomp::TSimSharedComponentPtr<CCacheCollectionComp> remote;
	remote->sourcePtr = &*source;
	remote->creatorPtr = &*fixture.creator;
	remote->InitComponent();
	icomp::TSimSharedComponentPtr<imtbase::CCachedObjectCollectionComp> cache;
	cache->SetRef("ObjectCollection", remote);
	cache->SetIntAttr("ObjectCacheLimit", 100);
	cache->SetIntAttr("MetaInfoCacheLimit", 100);
	cache->InitComponent();
	icomp::TSimSharedComponentPtr<CCacheSubscriptionManagerComp> transport;
	transport->InitComponent();
	icomp::TSimSharedComponentPtr<imtclientgql::CObjectCollectionSubscriberComp> subscriber;
	subscriber->SetRef("ObjectCollection", remote);
	subscriber->SetRef("SubscriptionManager", transport);
	subscriber->InsertMultiAttr("Command", QByteArray("OnUsersCollectionChanged"));
	subscriber->InitComponent();
	QVERIFY(transport->clientPtr != nullptr);
	QCOMPARE(transport->commandId, QByteArray("OnUsersCollectionChanged"));

	CSubscriptionFixture::Prepare(fixture.request, "subscription", "user-credential", QString::fromUtf8(transport->commandId));
	fixture.servlet->RegisterSubscription(fixture.request);
	QVERIFY2(fixture.servlet->error.isEmpty(), fixture.servlet->error.constData());
	QVERIFY(fixture.servlet->response.contains("start_ack"));
	QVERIFY(transport->clientPtr != nullptr);
	transport->clientPtr->OnSubscriptionStatusChanged("subscription", imtclientgql::IGqlSubscriptionClient::SS_REGISTERED, {});
	const auto* registeredContextPtr = publisher->RegisteredContext();
	QVERIFY(registeredContextPtr != nullptr);
	QVERIFY(!registeredContextPtr->GetPermissionBoundary().contains("/Permission1"));

	QJsonObject notification;
	delivery->onResponse = [&transport, &notification](const QByteArray& bytes){
		const auto message = QJsonDocument::fromJson(bytes).object();
		notification = message;
		transport->clientPtr->OnResponseReceived(message.value("id").toString().toUtf8(),
					QJsonDocument(message.value("payload").toObject()).toJson());
	};

	imtbase::IObjectCollection::DataPtr data;
	QVERIFY(cache->GetObjectData("original", data));
	const int warmReads = remote->reads;
	QVERIFY(cache->GetObjectData("original", data));
	QCOMPARE(remote->reads, warmReads);
	QVERIFY(!cache->GetObjectData("expanded", data));

	// Authorization expands, but neither a new subscription nor a cache reset is requested.
	++fixture.creator->access;
	fixture.ChangeAccess();
	QTRY_COMPARE_WITH_TIMEOUT(fixture.creator->establishedChecks.load(), 1, 2000);
	QTest::qWait(200);
	QCOMPARE(publisher->registrations.load(), 1);
	QCOMPARE(publisher->removals.load(), 0);
	QCOMPARE(publisher->RegisteredContext(), registeredContextPtr);
	QVERIFY(fixture.DroppedCodes().isEmpty());
	QCOMPARE(delivery->responses.load(), 0);

	const int readsBeforeDiscovery = remote->reads;
	QVERIFY(cache->GetObjectData("original", data));
	QCOMPARE(remote->reads, readsBeforeDiscovery);
	QVERIFY(cache->GetObjectData("expanded", data));
	QCOMPARE(remote->reads, readsBeforeDiscovery + 1);
	const auto* userPtr = dynamic_cast<const imtauth::IUserInfo*>(data.GetPtr());
	QVERIFY(userPtr != nullptr);
	QCOMPARE(userPtr->GetMail(), QString("before"));
	QVERIFY(cache->GetObjectData("expanded", data));
	QCOMPARE(remote->reads, readsBeforeDiscovery + 1);

	// A real source change must travel through the original server subscription to the remote model.
	imtauth::CUserInfo changed;
	changed.SetMail("after");
	QVERIFY(source->SetObjectData("expanded", changed));
	QCOMPARE(delivery->responses.load(), 1);
	QCOMPARE(notification.value("id").toString(), QString("subscription"));
	const auto event = notification.value("payload").toObject().value("data").toObject().value("OnUsersCollectionChanged").toObject();
	QCOMPARE(event.value("itemId").toString(), QString("expanded"));
	QCOMPARE(event.value("typeOperation").toString(), QString("updated"));
	QCOMPARE(publisher->registrations.load(), 1);
	QCOMPARE(publisher->removals.load(), 0);
	QCOMPARE(publisher->RegisteredContext(), registeredContextPtr);
	QVERIFY(!registeredContextPtr->GetPermissionBoundary().contains("/Permission1"));

	QVERIFY(cache->GetObjectData("expanded", data));
	userPtr = dynamic_cast<const imtauth::IUserInfo*>(data.GetPtr());
	QVERIFY(userPtr != nullptr);
	QCOMPARE(userPtr->GetMail(), QString("after"));
	QCOMPARE(remote->reads, readsBeforeDiscovery + 2);
	QVERIFY(cache->GetObjectData("expanded", data));
	QCOMPARE(remote->reads, readsBeforeDiscovery + 2);

	delivery->onResponse = {};
}


void CWebSocketCredentialsTest::RecipientChannelOutlivesANarrowing_data()
{
	QTest::addColumn<bool>("isDeliveredFirst");

	QTest::newRow("notification before the check") << true;
	QTest::newRow("check before the notification") << false;
}


void CWebSocketCredentialsTest::RecipientChannelOutlivesANarrowing()
{
	QFETCH(bool, isDeliveredFirst);

	// The data of the organization and the personal notifications, with the same credentials
	CSubscriptionFixture fixture(300);
	CSubscriptionFixture::Prepare(fixture.otherRequest, "personal-subscription", "user-credential", "personal");
	fixture.servlet->RegisterSubscription(fixture.request);
	fixture.servlet->RegisterSubscription(fixture.otherRequest);
	QCOMPARE(fixture.controller->Deliver("changed"), 1);
	QCOMPARE(fixture.controller->Deliver("personal"), 1);

	// The access is taken away and the user is told so, in either order
	--fixture.creator->access;
	if (isDeliveredFirst){
		QCOMPARE(fixture.controller->Deliver("personal"), 1);
	}
	fixture.ChangeAccess();
	QTRY_COMPARE_WITH_TIMEOUT(fixture.controller->removals.load(), 1, 2000);

	// The data subscription ended at once; the personal one still reaches its recipient
	QCOMPARE(fixture.DroppedCodes(), QByteArrayList{imtservergql::s_subscriptionPermissionsChangedCode});
	QCOMPARE(fixture.controller->Deliver("changed"), 0);
	QCOMPARE(fixture.controller->Deliver("personal"), 1);
	QTest::qWait(300);
	QCOMPARE(fixture.controller->removals.load(), 1);

	// Subscribed again once the access is stable, on the access of now
	fixture.servlet->response.clear();
	fixture.servlet->RegisterSubscription(fixture.request);
	QVERIFY(fixture.servlet->response.contains("start_ack"));
	QCOMPARE(fixture.controller->Deliver("changed"), 1);
}


void CWebSocketCredentialsTest::RecipientChannelEndsWhenRefused()
{
	CSubscriptionFixture fixture(300);
	CSubscriptionFixture::Prepare(fixture.request, "personal-subscription", "user-credential", "personal");
	fixture.servlet->RegisterSubscription(fixture.request);

	fixture.creator->renewState = Creator::AS_DENIED;
	fixture.ChangeAccess();
	QTRY_COMPARE_WITH_TIMEOUT(fixture.controller->removals.load(), 1, 2000);
	QCOMPARE(fixture.DroppedCodes(), QByteArrayList{imtservergql::s_subscriptionForbiddenCode});
}


void CWebSocketCredentialsTest::RecipientChannelEndsWithTheGrace()
{
	// No exception from a confirmed access: a personal channel waits as long as any other
	CSubscriptionFixture fixture(300, 1);
	CSubscriptionFixture::Prepare(fixture.request, "personal-subscription", "user-credential", "personal");
	fixture.servlet->RegisterSubscription(fixture.request);

	fixture.creator->renewState = Creator::AS_UNAVAILABLE;
	fixture.ChangeAccess();
	QTRY_COMPARE_WITH_TIMEOUT(fixture.controller->removals.load(), 1, 3000);
	QCOMPARE(fixture.DroppedCodes(), QByteArrayList{imtservergql::s_subscriptionAuthorizationUnavailableCode});
}


void CWebSocketCredentialsTest::RecipientChannelsAreDeclaredByCommand()
{
	typedef imtgql::IGqlSubscriberController Controller;
	const auto request = [](const QByteArray& commandId){
		return imtgql::CGqlRequest(imtgql::IGqlRequest::RT_SUBSCRIPTION, commandId);
	};
	const auto IsOpen = [](const Controller& controller, Controller::SubscriberKind subscriberKind, const imtgql::CGqlRequest& gqlRequest){
		return controller.IsOpenFor(subscriberKind, gqlRequest);
	};

	// The personal notifications are open to their recipient whatever its permissions, by their own command only
	icomp::TSimSharedComponentPtr<imtauthgql::CTenantNotificationPublisherComp> membership;
	membership->InsertMultiAttr("CommandIds", QString("OnMembershipNotification"));
	membership->InitComponent();
	QVERIFY(IsOpen(*membership, Controller::SK_RECIPIENT, request("OnMembershipNotification")));
	QVERIFY(IsOpen(*membership, Controller::SK_USER, request("OnMembershipNotification")));
	QVERIFY(!IsOpen(*membership, Controller::SK_RECIPIENT, request("OnOrganizationMembersChanged")));

	icomp::TSimSharedComponentPtr<imtauthgql::CConnectionCodesNotificationPublisherComp> connectionCodes;
	connectionCodes->InsertMultiAttr("CommandIds", QString("OnConnectionCodesNotification"));
	connectionCodes->InitComponent();
	QVERIFY(IsOpen(*connectionCodes, Controller::SK_RECIPIENT, request("OnConnectionCodesNotification")));
	QVERIFY(!IsOpen(*connectionCodes, Controller::SK_RECIPIENT, request("OnMembershipNotification")));

	icomp::TSimSharedComponentPtr<imtauthgql::CCrossTenantMessageNotificationPublisherComp> messages;
	messages->InsertMultiAttr("CommandIds", QString("OnCrossTenantMessageNotification"));
	messages->InitComponent();
	QVERIFY(IsOpen(*messages, Controller::SK_RECIPIENT, request("OnCrossTenantMessageNotification")));

	icomp::TSimSharedComponentPtr<imtauthgql::CTenantNotificationPublisherComp> restrictedMembership;
	restrictedMembership->InsertMultiAttr("CommandIds", QString("OnMembershipNotification"));
	restrictedMembership->InsertMultiAttr("ReadPermissions", QByteArray("/Read"));
	restrictedMembership->InitComponent();
	QVERIFY(!IsOpen(*restrictedMembership, Controller::SK_RECIPIENT, request("OnMembershipNotification")));

	icomp::TSimSharedComponentPtr<imtauthgql::CConnectionCodesNotificationPublisherComp> restrictedCodes;
	restrictedCodes->InsertMultiAttr("CommandIds", QString("OnConnectionCodesNotification"));
	restrictedCodes->InsertMultiAttr("ReadPermissions", QByteArray("/Read"));
	restrictedCodes->InitComponent();
	QVERIFY(!IsOpen(*restrictedCodes, Controller::SK_RECIPIENT, request("OnConnectionCodesNotification")));

	icomp::TSimSharedComponentPtr<imtauthgql::CCrossTenantMessageNotificationPublisherComp> restrictedMessages;
	restrictedMessages->InsertMultiAttr("CommandIds", QString("OnCrossTenantMessageNotification"));
	restrictedMessages->InsertMultiAttr("ReadPermissions", QByteArray("/Read"));
	restrictedMessages->InitComponent();
	QVERIFY(!IsOpen(*restrictedMessages, Controller::SK_RECIPIENT, request("OnCrossTenantMessageNotification")));

	// Not by default
	icomp::TSimSharedComponentPtr<CPlainPublisherComp> plain;
	plain->InsertMultiAttr("CommandIds", QString("OnPlainChanged"));
	plain->InitComponent();
	QVERIFY(IsOpen(*plain, Controller::SK_USER, request("OnPlainChanged")));
	QVERIFY(!IsOpen(*plain, Controller::SK_RECIPIENT, request("OnPlainChanged")));

	// Nothing else declares it: neither an open channel nor one a user may read
	icomp::TSimSharedComponentPtr<imtservergql::CObjectCollectionChangeNotifierComp> collection;
	collection->InsertMultiAttr("CommandIds", QString("OnUsersCollectionChanged"));
	collection->SetBoolAttr("OpenToUsers", true);
	collection->InitComponent();
	QVERIFY(IsOpen(*collection, Controller::SK_USER, request("OnUsersCollectionChanged")));
	QVERIFY(!IsOpen(*collection, Controller::SK_RECIPIENT, request("OnUsersCollectionChanged")));

	icomp::TSimSharedComponentPtr<imtservergql::CConnectionStatusSubscriberControllerComp> connectionStatus;
	connectionStatus->InitComponent();
	QVERIFY(IsOpen(*connectionStatus, Controller::SK_ANONYMOUS, request("OnConnectionStatusChanged")));
	QVERIFY(!IsOpen(*connectionStatus, Controller::SK_RECIPIENT, request("OnConnectionStatusChanged")));

	icomp::TSimSharedComponentPtr<CResponseDispatcherComp> statusDelivery;
	statusDelivery->InitComponent();
	icomp::TSimSharedComponentPtr<imtservergql::CConnectionStatusSubscriberControllerComp> restrictedStatus;
	restrictedStatus->SetRef("RequestManager", statusDelivery);
	restrictedStatus->InsertMultiAttr("CommandIds", QByteArray("OnConnectionStatusChanged"));
	restrictedStatus->InsertMultiAttr("ReadPermissions", QByteArray("/Read"));
	restrictedStatus->InitComponent();
	QVERIFY(!IsOpen(*restrictedStatus, Controller::SK_ANONYMOUS, request("OnConnectionStatusChanged")));
	QVERIFY(IsOpen(*restrictedStatus, Controller::SK_USER, request("OnConnectionStatusChanged")));
	QVERIFY(!IsOpen(*restrictedStatus, Controller::SK_RECIPIENT, request("OnConnectionStatusChanged")));
	imtrest::CWebSocketProtocolEngineComp engine;
	imtrest::CWebSocketRequest networkRequest(engine);
	imtgql::CGqlRequest restrictedRequest = request("OnConnectionStatusChanged");
	restrictedRequest.AddField("status", imtgql::CGqlFieldObject());
	QString error;
	Controller& controller = *restrictedStatus;
	QVERIFY(!controller.RegisterSubscription("restricted", restrictedRequest, networkRequest, error));
	QVERIFY(!error.isEmpty());
	QCOMPARE(statusDelivery->responses.load(), 0);
	QVERIFY(!controller.UnregisterSubscription("restricted"));
}


void CWebSocketCredentialsTest::MoreAccessDuringTheRegistration()
{
	// The access grows while the subscription is registered: the second check finds more, not less
	CSubscriptionFixture fixture;
	fixture.controller->onRegister = [&](){
		++fixture.creator->generation;
		++fixture.creator->access;
	};
	fixture.servlet->RegisterSubscription(fixture.request);

	QVERIFY(fixture.servlet->response.contains("start_ack"));
	QCOMPARE(fixture.controller->removals.load(), 0);
	QCOMPARE(fixture.creator->newChecks.load(), 2);
}


void CWebSocketCredentialsTest::RecipientChannelEndsForAnotherRecipient()
{
	// The same credentials stand for another user now: the personal channel is not its own
	CSubscriptionFixture fixture(300);
	CSubscriptionFixture::Prepare(fixture.request, "personal-subscription", "user-credential", "personal");
	fixture.servlet->RegisterSubscription(fixture.request);

	fixture.creator->userId = "another-user";
	fixture.ChangeAccess();
	QTRY_COMPARE_WITH_TIMEOUT(fixture.controller->removals.load(), 1, 2000);
	QCOMPARE(fixture.DroppedCodes(), QByteArrayList{imtservergql::s_subscriptionPermissionsChangedCode});
}


void CWebSocketCredentialsTest::PublisherReadPermissions_data()
{
	QTest::addColumn<bool>("hasUser");
	QTest::addColumn<QByteArrayList>("required");
	QTest::addColumn<QByteArrayList>("held");
	QTest::addColumn<QByteArrayList>("organization");
	QTest::addColumn<bool>("allowed");
	QTest::newRow("unrestricted") << false << QByteArrayList{} << QByteArrayList{} << QByteArrayList{} << true;
	QTest::newRow("no-user") << false << QByteArrayList{"/Read"} << QByteArrayList{"/Read"} << QByteArrayList{} << false;
	QTest::newRow("missing-permission") << true << QByteArrayList{"/Read"} << QByteArrayList{} << QByteArrayList{} << false;
	QTest::newRow("wrong-permission") << true << QByteArrayList{"/Read"} << QByteArrayList{"/Write"} << QByteArrayList{} << false;
	QTest::newRow("user-permission") << true << QByteArrayList{"/Read"} << QByteArrayList{"/Read"} << QByteArrayList{} << true;
	QTest::newRow("organization-permission") << true << QByteArrayList{"/Read"} << QByteArrayList{} << QByteArrayList{"/Read"} << true;
	QTest::newRow("second-alternative") << true << QByteArrayList{"/Write", "/Read"} << QByteArrayList{"/Read"} << QByteArrayList{} << true;
}


void CWebSocketCredentialsTest::PublisherReadPermissions()
{
	QFETCH(bool, hasUser);
	QFETCH(QByteArrayList, required);
	QFETCH(QByteArrayList, held);
	QFETCH(QByteArrayList, organization);
	QFETCH(bool, allowed);

	imtauth::CUserInfo user;
	user.SetLocalPermissions("Product", held);
	imtgql::IGqlContextSharedPtr context(new imtgql::CGqlContext);
	context->SetProductId("Product");
	context->SetOrganizationPermissions(organization);
	if (hasUser){
		context->SetUserInfo(&user);
	}
	imtrest::CWebSocketProtocolEngineComp engine;
	icomp::TSimSharedComponentPtr<CPlainPublisherComp> publisher;
	publisher->InsertMultiAttr("CommandIds", QByteArray("OnPlainChanged"));
	for (const QByteArray& permission: required){
		publisher->InsertMultiAttr("ReadPermissions", permission);
	}
	publisher->InitComponent();
	imtrest::CWebSocketRequest networkRequest(engine);
	imtgql::CGqlRequest request(imtgql::IGqlRequest::RT_SUBSCRIPTION, "OnPlainChanged");
	request.AddField("id", imtgql::CGqlFieldObject());
	request.SetGqlContext(context);
	QString error;
	QCOMPARE(publisher->RegisterSubscription("subscription", request, networkRequest, error), allowed);
	QCOMPARE(error.isEmpty(), allowed);
	QCOMPARE(publisher->UnregisterSubscription("subscription"), allowed);
}


void CWebSocketCredentialsTest::CollectionNotificationContainsOnlyIdentifiers_data()
{
	QTest::addColumn<int>("change");
	QTest::addColumn<QByteArray>("infoKey");
	QTest::addColumn<QString>("operation");
	QTest::newRow("insert") << int(imtbase::ICollectionInfo::CF_ADDED) << imtbase::ICollectionInfo::CN_ELEMENT_INSERTED << QString("inserted");
	QTest::newRow("update") << int(imtbase::IObjectCollection::CF_OBJECT_DATA_CHANGED) << imtbase::IObjectCollection::CN_OBJECT_DATA_CHANGED << QString("updated");
	QTest::newRow("remove") << int(imtbase::ICollectionInfo::CF_REMOVED) << imtbase::ICollectionInfo::CN_ELEMENTS_REMOVED << QString("removed");
}


void CWebSocketCredentialsTest::CollectionNotificationContainsOnlyIdentifiers()
{
	QFETCH(int, change);
	QFETCH(QByteArray, infoKey);
	QFETCH(QString, operation);
	icomp::TSimSharedComponentPtr<CCacheCollectionComp> source;
	source->InitComponent();
	imtauth::CUserInfo user;
	user.SetMail("private-content");
	QCOMPARE(source->InsertNewObject("User", "Private name", "", &user, "object"), QByteArray("object"));
	icomp::TSimSharedComponentPtr<CResponseDispatcherComp> delivery;
	delivery->InitComponent();
	imtrest::CWebSocketProtocolEngineComp engine;
	icomp::TSimSharedComponentPtr<CCachePublisherComp> publisher;
	publisher->SetRef("ObjectCollection", source);
	publisher->SetRef("RequestManager", delivery);
	publisher->SetIdAttr("CollectionId", "Users");
	publisher->InitComponent();
	imtrest::CWebSocketRequest networkRequest(engine);
	imtgql::CGqlRequest request(imtgql::IGqlRequest::RT_SUBSCRIPTION, "OnUsersCollectionChanged");
	QString error;
	QVERIFY2(publisher->RegisterSubscription("subscription", request, networkRequest, error), qPrintable(error));
	QJsonObject payload;
	delivery->onResponse = [&payload](const QByteArray& bytes){
		payload = QJsonDocument::fromJson(bytes).object().value("payload").toObject()
					.value("data").toObject().value("OnUsersCollectionChanged").toObject();
	};
	istd::IChangeable::ChangeSet changes(change);
	if (operation == "removed"){
		imtbase::ICollectionInfo::MultiElementNotifierInfo info;
		info.elementIds = {"object", "another"};
		changes.SetChangeInfo(infoKey, QVariant::fromValue(info));
	}
	else{
		changes.SetChangeInfo(infoKey, QByteArray("object"));
	}
	changes.SetChangeInfo(imtbase::IOperationContext::OPERATION_CONTEXT_INFO,
				QVariant::fromValue(imtbase::IOperationContext::OperationContextInfo("owner", "Private owner name")));
	const int readsBeforeNotification = source->reads;
	{
		istd::CChangeNotifier notifier(&*source, &changes);
	}
	QCOMPARE(delivery->responses.load(), 1);
	QCOMPARE(source->reads, readsBeforeNotification);
	QCOMPARE(payload.keys(), (QStringList{operation == "removed" ? "itemIds" : "itemId", "operationContext", "typeOperation"}));
	QCOMPARE(payload.value("typeOperation").toString(), operation);
	QCOMPARE(payload.value("operationContext").toObject(), (QJsonObject{{"ownerId", "owner"}}));
	if (operation == "removed"){
		QCOMPARE(payload.value("itemIds").toArray(), (QJsonArray{"object", "another"}));
	}
	else{
		QCOMPARE(payload.value("itemId").toString(), QString("object"));
	}
	QVERIFY(publisher->UnregisterSubscription("subscription"));
}


void CWebSocketCredentialsTest::ReconnectInvalidatesCacheAndRechecksAccess()
{
	icomp::TSimSharedComponentPtr<CCacheCollectionComp> source;
	source->InitComponent();
	imtauth::CUserInfo user;
	user.SetMail("before");
	QCOMPARE(source->InsertNewObject("User", "Original", "", &user, "original"), QByteArray("original"));
	icomp::TSimSharedComponentPtr<CCredentialContextComp> reader;
	reader->access = 1;
	reader->InitComponent();
	icomp::TSimSharedComponentPtr<CCacheCollectionComp> remote;
	remote->sourcePtr = &*source;
	remote->creatorPtr = &*reader;
	remote->InitComponent();
	icomp::TSimSharedComponentPtr<CObservableCacheComp> cache;
	cache->SetRef("ObjectCollection", remote);
	cache->SetIntAttr("ObjectCacheLimit", 100);
	cache->InitComponent();
	icomp::TSimSharedComponentPtr<CCacheSubscriptionManagerComp> transport;
	transport->InitComponent();
	icomp::TSimSharedComponentPtr<imtclientgql::CObjectCollectionSubscriberComp> subscriber;
	subscriber->SetRef("ObjectCollection", remote);
	subscriber->SetRef("SubscriptionManager", transport);
	subscriber->InsertMultiAttr("Command", QByteArray("OnUsersCollectionChanged"));
	subscriber->InitComponent();
	QVERIFY(transport->clientPtr != nullptr);
	transport->clientPtr->OnSubscriptionStatusChanged("subscription", imtclientgql::IGqlSubscriptionClient::SS_REGISTERED, {});

	CSubscriptionFixture fixture(300, 60, nullptr, &cache);
	fixture.servlet->RegisterSubscription(fixture.request);
	QVERIFY(fixture.servlet->response.contains("start_ack"));
	imtbase::IObjectCollection::DataPtr data;
	QVERIFY(cache->GetObjectData("original", data));
	const int warmReads = remote->reads;
	QVERIFY(cache->GetObjectData("original", data));
	QCOMPARE(remote->reads, warmReads);

	transport->clientPtr->OnSubscriptionStatusChanged("subscription", imtclientgql::IGqlSubscriptionClient::SS_IN_REGISTRATION, {});
	QTRY_COMPARE_WITH_TIMEOUT(fixture.creator->establishedChecks.load(), 1, 2000);
	user.SetMail("during-gap");
	QVERIFY(source->SetObjectData("original", user));
	QVERIFY(cache->GetObjectData("original", data));
	const auto* currentUser = dynamic_cast<const imtauth::IUserInfo*>(data.GetPtr());
	QVERIFY(currentUser != nullptr);
	QCOMPARE(currentUser->GetMail(), QString("during-gap"));
	const int unwatchedReads = remote->reads;
	QVERIFY(cache->GetObjectData("original", data));
	QCOMPARE(remote->reads, unwatchedReads + 1);

	fixture.creator->access = 0;
	transport->clientPtr->OnSubscriptionStatusChanged("subscription", imtclientgql::IGqlSubscriptionClient::SS_REGISTERED, {});
	QTRY_COMPARE_WITH_TIMEOUT(fixture.controller->removals.load(), 1, 2000);
	QCOMPARE(fixture.DroppedCodes(), QByteArrayList{imtservergql::s_subscriptionPermissionsChangedCode});
	QVERIFY(cache->GetObjectData("original", data));
	const int resumedReads = remote->reads;
	QVERIFY(cache->GetObjectData("original", data));
	QCOMPARE(remote->reads, resumedReads);
}


void CWebSocketCredentialsTest::ForwardedRefusalKeepsItsCode()
{
	// A refusal of the server behind reaches the client with its reason: a retry or a new subscription is told from a refusal
	icomp::TSimSharedComponentPtr<CCacheSubscriptionManagerComp> transport;
	transport->InitComponent();
	icomp::TSimSharedComponentPtr<CResponseDispatcherComp> dispatcher;
	QByteArrayList sent;
	dispatcher->onResponse = [&sent](const QByteArray& data){ sent << data; };
	dispatcher->InitComponent();
	icomp::TSimSharedComponentPtr<imtservergql::CPublisherSubscriberBridgeComp> bridge;
	bridge->InsertMultiAttr("CommandIds", QByteArray("OnChanged"));
	bridge->SetRef("SubscriptionManager", transport);
	bridge->SetRef("RequestManager", dispatcher);
	bridge->InitComponent();

	imtrest::CWebSocketProtocolEngineComp engine;
	imtrest::CWebSocketRequest request(engine);
	request.SetBody(R"({"id":"client","type":"start"})");
	imtgql::CGqlRequest gqlRequest(imtgql::IGqlRequest::RT_SUBSCRIPTION, "OnChanged");
	imtgql::CGqlFieldObject field;
	field.InsertField("id");
	gqlRequest.AddField("data", field);
	QString errorMessage;
	QVERIFY2(bridge->RegisterSubscription("client", gqlRequest, request, errorMessage), qPrintable(errorMessage));
	QVERIFY(transport->clientPtr != nullptr);

	for (const QByteArray& code : {imtservergql::s_subscriptionPermissionsChangedCode, imtservergql::s_subscriptionAuthorizationUnavailableCode, QByteArray()}){
		sent.clear();
		QJsonObject extensions{{"type", "Warning"}};
		if (!code.isEmpty()){
			extensions.insert("code", QString::fromUtf8(code));
		}
		const QJsonObject frame{{"id", "subscription"}, {"type", "error"}, {"payload", QJsonArray{QJsonObject{{"message", "Refused"}, {"extensions", extensions}}}}};
		transport->clientPtr->OnSubscriptionStatusChanged("subscription", imtclientgql::IGqlSubscriptionClient::SS_REFUSED, QString::fromUtf8(QJsonDocument(frame).toJson()));
		QCOMPARE(sent.size(), 1);
		const QByteArray& data = sent.first();
		const QJsonObject forwarded = QJsonDocument::fromJson(data.mid(data.indexOf('{'))).object();
		QCOMPARE(forwarded.value("id").toString(), QString("client"));
		const QJsonObject forwardedExtensions = forwarded.value("payload").toArray().first().toObject().value("extensions").toObject();
		QCOMPARE(forwardedExtensions.value("code").toString().toUtf8(), code);
	}
}


I_ADD_TEST(CWebSocketCredentialsTest);
