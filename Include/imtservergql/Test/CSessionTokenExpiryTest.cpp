// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include "CSessionTokenExpiryTest.h"


#include <atomic>
#include <functional>
#include <list>
#include <memory>
#include <thread>
#include <vector>
#include <QtCore/QElapsedTimer>
#include <QtCore/QSet>
#include <QtCore/QJsonDocument>
#include <QtCore/QJsonObject>
#include <QtCore/QMutex>
#include <QtCore/QUuid>
#include <imod/TModelWrap.h>
#include <icomp/TSimComponentWrap.h>
#include <iprm/CNameParam.h>
#include <iprm/CTextParam.h>
#include <iprm/ITextParam.h>
#include <itest/CStandardTestExecutor.h>
#include <imtauth/CJwtSessionControllerComp.h>
#include <imtauth/CSessionInfo.h>
#include <imtauth/CUserInfo.h>
#include <imtauth/IJwtSessionController.h>
#include <imtauth/imtauth.h>
#include <imtbase/CObjectCollection.h>
#include <imtbase/IObjectCollectionReadStatus.h>
#include <imtbase/imtbase.h>
#include <imtgql/CGqlContext.h>
#include <imtgql/CGqlRequestContextManager.h>
#include <imtgql/CGqlResponse.h>
#include <imtclientgql/IGqlClient.h>
#include <imtauthgql/CRemoteAuthorizationControllerComp.h>
#include <imtrest/CWebSocketProtocolEngineComp.h>
#include <imtrest/CWebSocketRequest.h>
#include <imtservergql/CAuthenticationManagerComp.h>
#include <imtservergql/CWebSocketServletComp.h>
#include <imtservergql/imtservergql.h>


namespace
{


typedef imtgql::IGqlContextCreator Creator;
typedef imtauth::IJwtSessionController::UserSession UserSession;

const QByteArray s_userId = "5b0f6c2e-2a3d-4c8e-9f1a-7d6e5c4b3a21";
const QByteArray s_productId = "Product";


class CContextComp: public icomp::CComponentBase, public imtgql::CGqlContext
{
public:
	typedef icomp::CComponentBase BaseClass;
	I_BEGIN_COMPONENT(CContextComp)
		I_REGISTER_INTERFACE(imtgql::IGqlContext);
	I_END_COMPONENT
};


class CSessionComp: public icomp::CComponentBase, public imtauth::CSessionInfo
{
public:
	typedef icomp::CComponentBase BaseClass;
	I_BEGIN_COMPONENT(CSessionComp)
		I_REGISTER_INTERFACE(imtauth::ISession);
	I_END_COMPONENT
};


class CSecretComp: public icomp::CComponentBase, public iprm::CNameParam
{
public:
	typedef icomp::CComponentBase BaseClass;
	I_BEGIN_COMPONENT(CSecretComp)
		I_REGISTER_INTERFACE(iprm::INameParam);
	I_END_COMPONENT
};


// A setting of the server, as the settings file holds it
class CSettingComp: public icomp::CComponentBase, public iprm::CTextParam
{
public:
	typedef icomp::CComponentBase BaseClass;
	I_BEGIN_COMPONENT(CSettingComp)
		I_REGISTER_INTERFACE(iprm::ITextParam);
	I_END_COMPONENT
};


template<class Component>
class TComponentFactory: public icomp::CSimComponentContextBase::ComponentsFactory
{
public:
	KeyList GetFactoryKeys() const override { return {QByteArray()}; }
	istd::TUniqueInterfacePtr<icomp::IComponent> CreateInstance(const QByteArray&) const override
	{
		auto* component = new icomp::TSimComponentWrap<Component>;
		component->InitComponent();
		return istd::TUniqueInterfacePtr<icomp::IComponent>(component);
	}
};


// The session table: found by its refresh token as the SQL delegate finds it, read from the threads of the checks
class CSessionStoreComp: public icomp::CComponentBase, public imod::TModelWrap<imtbase::CObjectCollection>
{
public:
	typedef icomp::CComponentBase BaseClass;
	I_BEGIN_COMPONENT(CSessionStoreComp)
		I_REGISTER_INTERFACE(imtbase::IObjectCollection);
		I_REGISTER_INTERFACE(imod::IModel);
	I_END_COMPONENT

	Id InsertNewObject(
				const QByteArray&,
				const QString&,
				const QString&,
				const istd::IChangeable* defaultValuePtr,
				const Id& proposedElementId,
				const idoc::IDocumentMetaInfo*,
				const idoc::IDocumentMetaInfo*,
				const imtbase::IOperationContext*) override
	{
		if (defaultValuePtr == nullptr){
			return Id();
		}
		const Id id = proposedElementId.isEmpty() ? QUuid::createUuid().toByteArray(QUuid::WithoutBraces) : proposedElementId;
		QMutexLocker locker(&m_mutex);
		m_sessions.insert(id, std::shared_ptr<istd::IChangeable>(defaultValuePtr->CloneMe().PopPtr()));
		return id;
	}

	bool SetObjectData(const Id& objectId, const istd::IChangeable& object, CompatibilityMode, const imtbase::IOperationContext*) override
	{
		QMutexLocker locker(&m_mutex);
		if (!m_sessions.contains(objectId)){
			return false;
		}
		m_sessions.insert(objectId, std::shared_ptr<istd::IChangeable>(object.CloneMe().PopPtr()));
		return true;
	}

	bool GetObjectData(const Id& objectId, DataPtr& dataPtr, const iprm::IParamsSet*) const override
	{
		QMutexLocker locker(&m_mutex);
		const auto it = m_sessions.constFind(objectId);
		if (it == m_sessions.cend()){
			return false;
		}
		dataPtr.FromUnique(it.value()->CloneMe());
		return dataPtr.IsValid();
	}

	// Tells the removed sessions as a stored collection does
	bool failRemoval = false;
	bool RemoveElements(const Ids& elementIds, const imtbase::IOperationContext*) override
	{
		if (failRemoval){
			return false;
		}
		MultiElementNotifierInfo notifierInfo;
		notifierInfo.elementIds = elementIds;
		istd::IChangeable::ChangeSet changeSet(CF_REMOVED);
		changeSet.SetChangeInfo(CN_ELEMENTS_REMOVED, QVariant::fromValue(notifierInfo));
		istd::CChangeNotifier notifier(this, &changeSet);

		QMutexLocker locker(&m_mutex);
		for (const Id& id : elementIds){
			m_sessions.remove(id);
		}
		return true;
	}

	Ids GetElementIds(int, int, const iprm::IParamsSet* paramsPtr, ilog::IMessageConsumer*) const override
	{
		const iprm::ITextParam* refreshTokenPtr = (paramsPtr != nullptr) ? dynamic_cast<const iprm::ITextParam*>(paramsPtr->GetParameter("RefreshToken")) : nullptr;
		QMutexLocker locker(&m_mutex);
		Ids retVal;
		for (auto it = m_sessions.cbegin(); it != m_sessions.cend(); ++it){
			const auto* sessionPtr = dynamic_cast<const imtauth::ISession*>(it.value().get());
			if (refreshTokenPtr == nullptr || (sessionPtr != nullptr && sessionPtr->GetToken() == refreshTokenPtr->GetText().toUtf8())){
				retVal << it.key();
			}
		}
		return retVal;
	}

	int Count() const
	{
		QMutexLocker locker(&m_mutex);
		return int(m_sessions.size());
	}

private:
	QMap<Id, std::shared_ptr<istd::IChangeable>> m_sessions;
	mutable QMutex m_mutex;
};


class CUserCollectionComp:
			public icomp::CComponentBase,
			public imtbase::CObjectCollection,
			virtual public imtbase::IObjectCollectionReadStatus
{
public:
	typedef icomp::CComponentBase BaseClass;
	I_BEGIN_COMPONENT(CUserCollectionComp)
		I_REGISTER_INTERFACE(imtbase::IObjectCollection);
	I_END_COMPONENT

	imtauth::CIdentifiableUserInfo user;
	// As a remote collection: it reads with the token of the request, which the authorization server refuses once expired
	std::atomic<bool> isExpiredTokenRefused{false};

	bool GetObjectData(const Id& objectId, DataPtr& dataPtr, const iprm::IParamsSet*) const override
	{
		if (objectId != s_userId){
			return false;
		}
		dataPtr.FromUnique(user.CloneMe());
		return dataPtr.IsValid();
	}

	bool GetObjectDataWithStatus(const Id& objectId, DataPtr& dataPtr, bool& isAccessDenied, const iprm::IParamsSet* paramsPtr) const override
	{
		const imtgql::IGqlContext* contextPtr = imtgql::CGqlRequestContextManager::GetContext();
		imtauth::SessionTokenClaims claims;
		isAccessDenied = isExpiredTokenRefused && (contextPtr == nullptr
					|| !imtauth::ParseSessionTokenClaims(contextPtr->GetToken(), claims) || claims.expSecs < QDateTime::currentSecsSinceEpoch());
		if (isAccessDenied){
			return false;
		}
		return GetObjectData(objectId, dataPtr, paramsPtr);
	}

	QByteArray GetObjectTypeId(const Id&) const override
	{
		return "User";
	}

	Ids GetElementIds(int, int, const iprm::IParamsSet*, ilog::IMessageConsumer*) const override
	{
		return Ids{s_userId};
	}
};


// The checks of the established subscriptions, with the token each one was made with
class CObservedAuthenticationManager: public imtservergql::CAuthenticationManagerComp
{
public:
	struct Check
	{
		QByteArray token;
		AuthorizationState state = AS_UNAVAILABLE;
		qint64 atSecs = 0;
	};

	imtgql::IGqlContextUniquePtr RenewGqlContext(
				const imtgql::IGqlContext::Headers& headers,
				bool isEstablished,
				AuthorizationState& state,
				ContextCreationError& error,
				QDateTime* sessionExpiresAtPtr = nullptr) const override
	{
		imtgql::IGqlContextUniquePtr retVal = CAuthenticationManagerComp::RenewGqlContext(headers, isEstablished, state, error, sessionExpiresAtPtr);
		if (isEstablished){
			QMutexLocker locker(&m_checksMutex);
			m_checks.append({headers.value(imtbase::s_authenticationTokenHeaderId), state, QDateTime::currentSecsSinceEpoch()});
		}
		return retVal;
	}

	QList<Check> Checks() const
	{
		QMutexLocker locker(&m_checksMutex);
		return m_checks;
	}

private:
	mutable QList<Check> m_checks;
	mutable QMutex m_checksMutex;
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

	bool IsRequestSupported(const imtgql::CGqlRequest&) const override { return true; }
	bool IsOpenFor(SubscriberKind, const imtgql::CGqlRequest&) const override { return true; }
	bool RegisterSubscription(const QByteArray&, const imtgql::CGqlRequest&, const imtrest::IRequest&, QString&) override
	{
		++registrations;
		return true;
	}
	bool UnregisterSubscription(const QByteArray&, const QByteArray&) override
	{
		++removals;
		return true;
	}
};


// The authorization server behind a product server: it ends the session on a logout
class CAuthorizationServerStubComp: public icomp::CComponentBase, virtual public imtclientgql::IGqlClient
{
public:
	typedef icomp::CComponentBase BaseClass;
	I_BEGIN_COMPONENT(CAuthorizationServerStubComp)
		I_REGISTER_INTERFACE(imtclientgql::IGqlClient);
	I_END_COMPONENT

	std::function<void()> onLogout;

	GqlResult SendRequest(GqlRequestPtr requestPtr, imtbase::IUrlParam*) const override
	{
		const imtgql::CGqlRequest* gqlRequestPtr = dynamic_cast<const imtgql::CGqlRequest*>(requestPtr.GetPtr());
		if (gqlRequestPtr == nullptr || gqlRequestPtr->GetCommandId() != "Logout"){
			return {{}, EC_INVALID_REQUEST, QStringLiteral("Unexpected request")};
		}
		if (onLogout){
			onLogout();
		}

		auto* responsePtr = new imtgql::CGqlResponse(requestPtr);
		responsePtr->SetResponseData(R"({"data":{"Logout":{"ok":true}}})");
		GqlResponsePtr retVal;
		retVal.SetPtr(responsePtr);
		return {retVal, EC_NONE, {}};
	}

	bool SendRequestNoWait(GqlRequestPtr, imtbase::IUrlParam*) const override { return false; }
};


// Where a subscription the server ends is told so
class CResponseDispatcherComp: public icomp::CComponentBase, virtual public imtrest::IResponseDispatcher
{
public:
	typedef icomp::CComponentBase BaseClass;
	I_BEGIN_COMPONENT(CResponseDispatcherComp)
		I_REGISTER_INTERFACE(imtrest::IResponseDispatcher);
	I_END_COMPONENT

	bool SendResponse(const QByteArray&, imtrest::ConstResponsePtr&) const override { return true; }
	bool SendRequest(const QByteArray&, imtrest::ConstRequestPtr&) const override { return false; }
};


class CTestServlet: public imtservergql::CWebSocketServletComp
{
public:
	using CWebSocketServletComp::RegisterSubscription;
	using CWebSocketServletComp::UnregisterSubscription;

	QByteArrayList Responses() const
	{
		QMutexLocker locker(&m_mutex);
		return m_responses;
	}

	// The codes of the refusals and of the ends of subscriptions, in their order
	QByteArrayList ErrorCodes() const
	{
		QMutexLocker locker(&m_mutex);
		return m_errorCodes;
	}

protected:
	imtrest::ConstResponsePtr CreateDataResponse(const QByteArray& data, const imtrest::IRequest&) const override
	{
		QMutexLocker locker(&m_mutex);
		m_responses << data;
		return imtrest::ConstResponsePtr();
	}
	imtrest::ConstResponsePtr CreateErrorResponse(const QByteArray&, const imtrest::IRequest&, bool, const QByteArray& reasonCode) const override
	{
		QMutexLocker locker(&m_mutex);
		m_errorCodes << reasonCode;
		return imtrest::ConstResponsePtr();
	}

private:
	mutable QByteArrayList m_responses;
	mutable QByteArrayList m_errorCodes;
	mutable QMutex m_mutex;
};


struct CExpiryFixture
{
	TComponentFactory<CContextComp> contextFactory;
	TComponentFactory<CSessionComp> sessionFactory;
	icomp::TSimSharedComponentPtr<CSessionStoreComp> store;
	icomp::TSimSharedComponentPtr<CSecretComp> secret;
	// The controller of the server: it checks every token and issues the refreshed ones
	icomp::TSimSharedComponentPtr<imtauth::CJwtSessionControllerComp> sessions;
	// Issues sessions whose first access token has already expired
	icomp::TSimSharedComponentPtr<imtauth::CJwtSessionControllerComp> expiredIssuer;
	icomp::TSimSharedComponentPtr<CUserCollectionComp> users;
	icomp::TSimSharedComponentPtr<CObservedAuthenticationManager> authentication;
	icomp::TSimSharedComponentPtr<CSubscriptionControllerComp> controller;
	icomp::TSimSharedComponentPtr<CResponseDispatcherComp> dispatcher;
	icomp::TSimSharedComponentPtr<CTestServlet> servlet;
	imtrest::CWebSocketProtocolEngineComp engine;
	// The frames of one connection; destroyed before the servlet, as a closed connection
	std::list<std::unique_ptr<imtrest::CWebSocketRequest>> frames;

	explicit CExpiryFixture(int jwtLifetime = 3600, int sessionLifetime = 3600, int maxCheckInterval = 300, int reuseLeeway = 30)
	{
		store->InitComponent();
		secret->InitComponent();
		secret->SetName(QStringLiteral("test-secret"));
		for (auto* controllerPtr : {&sessions, &expiredIssuer}){
			(*controllerPtr)->SetRef("SessionCollection", store);
			(*controllerPtr)->SetRef("SecretKeyParam", secret);
			(*controllerPtr)->SetFactory("SessionFactory", &sessionFactory);
			(*controllerPtr)->SetIntAttr("RefreshTokenLifetime", sessionLifetime);
		}
		sessions->SetIntAttr("JwtLifetime", jwtLifetime);
		sessions->SetIntAttr("RefreshTokenReuseLeeway", reuseLeeway);
		sessions->InitComponent();
		expiredIssuer->SetIntAttr("JwtLifetime", -60);
		expiredIssuer->InitComponent();

		users->user.SetObjectUuid(s_userId);
		users->user.SetId("expiry-user");
		users->user.SetEnabled(true);
		users->InitComponent();

		authentication->SetRef("SlaveJwtSessionController", sessions);
		authentication->SetRef("SessionCollection", store);
		authentication->SetRef("UserCollection", users);
		authentication->SetFactory("GqlContextFactory", &contextFactory);
		authentication->InitComponent();

		controller->InitComponent();
		dispatcher->InitComponent();
		servlet->SetRef("GqlContextCreator", authentication);
		servlet->SetRef("RequestManager", dispatcher);
		servlet->InsertMultiRef("GqlSubscriberControllers", controller);
		servlet->SetIntAttr("MaxCheckInterval", maxCheckInterval);
		servlet->SetIntAttr("GracePeriod", 60);
		servlet->SetBoolAttr("DenyAnonymousSubscriptions", true);
		servlet->SetBoolAttr("DenyUndeclaredUserSubscriptions", true);
		servlet->InitComponent();
	}

	// Signs in, as the login of a client does
	bool Login(UserSession& userSession, bool isTokenExpired = false)
	{
		userSession = UserSession();
		userSession.productId = s_productId;
		const imtauth::IJwtSessionController& issuer = isTokenExpired ? *expiredIssuer : *sessions;
		return issuer.CreateNewSession(s_userId, QByteArray(), userSession);
	}

	// Refreshes the session, as the RefreshToken request of a client does
	bool Refresh(const QByteArray& refreshToken, UserSession& userSession)
	{
		userSession = UserSession();
		userSession.productId = s_productId;
		return authentication->RefreshToken(refreshToken, userSession);
	}

	static imtgql::IGqlContext::Headers Headers(const QByteArray& token)
	{
		imtgql::IGqlContext::Headers headers;
		headers.insert(imtbase::s_authenticationTokenHeaderId, token);
		headers.insert(imtbase::s_productIdHeaderId, s_productId);
		return headers;
	}

	// A GraphQL request with the token: OK, or why not
	Creator::ContextCreationStatus Request(const QByteArray& token)
	{
		Creator::ContextCreationError error;
		const imtgql::IGqlContextUniquePtr contextPtr = authentication->CreateGqlContext(Headers(token), error);
		imtgql::CGqlRequestContextManager::Clear();
		if (contextPtr.IsValid()){
			return error.status == Creator::CCS_OK ? Creator::CCS_OK : Creator::CCS_INTERNAL_ERROR;
		}
		return error.status;
	}

	imtrest::CWebSocketRequest& Frame(const QString& type, const QString& subscriptionId, const QByteArray& token)
	{
		frames.push_back(std::make_unique<imtrest::CWebSocketRequest>(engine));
		imtrest::CWebSocketRequest& frame = *frames.back();
		frame.SetConnectionId("connection");
		QJsonObject headers;
		headers.insert(QString::fromUtf8(imtbase::s_authenticationTokenHeaderId), QString::fromUtf8(token));
		headers.insert(QString::fromUtf8(imtbase::s_productIdHeaderId), QString::fromUtf8(s_productId));
		QJsonObject message;
		message.insert("id", subscriptionId);
		message.insert("type", type);
		message.insert("headers", headers);
		message.insert("payload", type == "start" ? QJsonObject{{"query", QStringLiteral("subscription { OnChanged { id } }")}} : QJsonObject());
		frame.SetBody(QJsonDocument(message).toJson(QJsonDocument::Compact));
		return frame;
	}

	// Sends a start over the connection: true when it is acknowledged
	bool Start(const QString& subscriptionId, const QByteArray& token)
	{
		const int responses = servlet->Responses().size();
		servlet->RegisterSubscription(Frame("start", subscriptionId, token));
		const QByteArrayList all = servlet->Responses();
		return all.size() == responses + 1 && all.last().contains("start_ack");
	}

	void Stop(const QString& subscriptionId, const QByteArray& token)
	{
		servlet->UnregisterSubscription(Frame("stop", subscriptionId, token));
	}

	// The established checks made after the token expired, with that token
	int ChecksAfterExpiry(const QByteArray& token, qint64 expSecs, Creator::AuthorizationState state)
	{
		int count = 0;
		for (const CObservedAuthenticationManager::Check& check : authentication->Checks()){
			if (check.token == token && check.atSecs > expSecs && check.state == state){
				++count;
			}
		}
		return count;
	}

	static qint64 Expiry(const QByteArray& token)
	{
		imtauth::SessionTokenClaims claims;
		return imtauth::ParseSessionTokenClaims(token, claims) ? claims.expSecs : 0;
	}

	static QByteArray SessionOf(const QByteArray& token)
	{
		imtauth::SessionTokenClaims claims;
		return imtauth::ParseSessionTokenClaims(token, claims) ? claims.sessionId : QByteArray();
	}

	// Waits until the token is expired by the second the session controller counts in
	static bool WaitForExpiry(const QByteArray& token)
	{
		const qint64 expSecs = Expiry(token);
		QElapsedTimer timer;
		timer.start();
		while (QDateTime::currentSecsSinceEpoch() <= expSecs){
			if (timer.elapsed() > 5000){
				return false;
			}
			QTest::qWait(50);
		}
		return true;
	}
};


} // namespace


void CSessionTokenExpiryTest::LifetimesComeFromSettings_data()
{
	QTest::addColumn<QString>("jwtSetting");
	QTest::addColumn<QString>("sessionSetting");
	QTest::addColumn<int>("jwtLifetime");
	QTest::addColumn<int>("sessionLifetime");
	QTest::newRow("settings") << "20" << "75" << 20 << 75;
	QTest::newRow("padded") << " 30 " << "90" << 30 << 90;
	QTest::newRow("empty: the attributes") << "" << "" << 600 << 1200;
	QTest::newRow("not a number: the attributes") << "soon" << "-5" << 600 << 1200;
	QTest::newRow("zero: the attributes") << "0" << "0" << 600 << 1200;
}


void CSessionTokenExpiryTest::LifetimesComeFromSettings()
{
	QFETCH(QString, jwtSetting);
	QFETCH(QString, sessionSetting);
	QFETCH(int, jwtLifetime);
	QFETCH(int, sessionLifetime);

	TComponentFactory<CSessionComp> sessionFactory;
	icomp::TSimSharedComponentPtr<CSessionStoreComp> store;
	icomp::TSimSharedComponentPtr<CSecretComp> secret;
	icomp::TSimSharedComponentPtr<CSettingComp> jwtParam;
	icomp::TSimSharedComponentPtr<CSettingComp> sessionParam;
	icomp::TSimSharedComponentPtr<imtauth::CJwtSessionControllerComp> sessions;
	store->InitComponent();
	secret->InitComponent();
	secret->SetName(QStringLiteral("test-secret"));
	jwtParam->InitComponent();
	jwtParam->SetText(jwtSetting);
	sessionParam->InitComponent();
	sessionParam->SetText(sessionSetting);
	sessions->SetRef("SessionCollection", store);
	sessions->SetRef("SecretKeyParam", secret);
	sessions->SetFactory("SessionFactory", &sessionFactory);
	sessions->SetIntAttr("JwtLifetime", 600);
	sessions->SetIntAttr("RefreshTokenLifetime", 1200);
	sessions->SetRef("JwtLifetimeParam", jwtParam);
	sessions->SetRef("RefreshTokenLifetimeParam", sessionParam);
	sessions->InitComponent();
	const imtauth::IJwtSessionController& controller = *sessions;

	UserSession session;
	session.productId = s_productId;
	const qint64 before = QDateTime::currentSecsSinceEpoch();
	QVERIFY(controller.CreateNewSession(s_userId, QByteArray(), session));
	const qint64 life = CExpiryFixture::Expiry(session.accessToken) - before;
	QVERIFY2(life >= jwtLifetime && life <= jwtLifetime + 1, qPrintable(QString::number(life)));

	const auto CheckSession = [&](){
		const imtauth::ISessionSharedPtr sessionPtr = controller.GetSession(CExpiryFixture::SessionOf(session.accessToken));
		QVERIFY(sessionPtr.IsValid());
		const qint64 sessionLife = QDateTime::currentDateTimeUtc().secsTo(sessionPtr->GetExpirationDate());
		QVERIFY2(sessionLife >= sessionLifetime - 2 && sessionLife <= sessionLifetime, qPrintable(QString::number(sessionLife)));
	};
	CheckSession();

	// A refresh takes them too, read anew: a changed setting holds for the next token
	jwtParam->SetText(QStringLiteral("45"));
	UserSession refreshed;
	refreshed.productId = s_productId;
	QVERIFY(controller.RefreshToken(session.refreshToken, refreshed));
	const qint64 refreshedLife = CExpiryFixture::Expiry(refreshed.accessToken) - QDateTime::currentSecsSinceEpoch();
	QVERIFY2(refreshedLife >= 44 && refreshedLife <= 45, qPrintable(QString::number(refreshedLife)));
	CheckSession();
}


void CSessionTokenExpiryTest::ExpiredTokenIsRefusedAsUnauthorized()
{
	CExpiryFixture fixture;
	UserSession session;
	QVERIFY(fixture.Login(session, true));
	QVERIFY(CExpiryFixture::Expiry(session.accessToken) < QDateTime::currentSecsSinceEpoch());

	// A request: the client refreshes, it is not signed out
	QCOMPARE(fixture.Request(session.accessToken), Creator::CCS_UNAUTHORIZED);
	// The second one is answered from the cache of expired tokens, the same way
	QCOMPARE(fixture.Request(session.accessToken), Creator::CCS_UNAUTHORIZED);

	// A new subscription: refused with the code that tells the client to refresh, nothing registered
	QVERIFY(!fixture.Start("subscription", session.accessToken));
	QCOMPARE(fixture.servlet->ErrorCodes(), QByteArrayList{imtservergql::s_subscriptionUnauthorizedCode});
	QCOMPARE(fixture.controller->registrations.load(), 0);
}


void CSessionTokenExpiryTest::RefreshKeepsTheSession()
{
	CExpiryFixture fixture;
	UserSession session;
	QVERIFY(fixture.Login(session, true));
	QCOMPARE(fixture.Request(session.accessToken), Creator::CCS_UNAUTHORIZED);

	UserSession refreshed;
	QVERIFY(fixture.Refresh(session.refreshToken, refreshed));
	QVERIFY(refreshed.accessToken != session.accessToken);
	QVERIFY(refreshed.refreshToken != session.refreshToken);
	QCOMPARE(refreshed.userId, s_userId);
	QCOMPARE(CExpiryFixture::SessionOf(refreshed.accessToken), CExpiryFixture::SessionOf(session.accessToken));
	QCOMPARE(fixture.store->Count(), 1);

	// The refreshed token opens requests and subscriptions
	QCOMPARE(fixture.Request(refreshed.accessToken), Creator::CCS_OK);
	QVERIFY(fixture.Start("subscription", refreshed.accessToken));
	QCOMPARE(fixture.controller->registrations.load(), 1);

	// The expired one stays refused: the refresh revives no old token
	QCOMPARE(fixture.Request(session.accessToken), Creator::CCS_UNAUTHORIZED);

	// A token issued before a refresh holds until its own end
	UserSession again;
	QVERIFY(fixture.Refresh(refreshed.refreshToken, again));
	QCOMPARE(fixture.Request(refreshed.accessToken), Creator::CCS_OK);
	QCOMPARE(fixture.Request(again.accessToken), Creator::CCS_OK);
}


void CSessionTokenExpiryTest::RotatedRefreshTokenIsRefused()
{
	CExpiryFixture fixture;
	UserSession session;
	QVERIFY(fixture.Login(session, true));

	UserSession refreshed;
	QVERIFY(fixture.Refresh(session.refreshToken, refreshed));

	// The answer was lost, or another client refreshed at the same time: the replaced token gives the current session again
	UserSession replayed;
	QVERIFY(fixture.Refresh(session.refreshToken, replayed));
	QCOMPARE(replayed.refreshToken, refreshed.refreshToken);
	QCOMPARE(fixture.Request(replayed.accessToken), Creator::CCS_OK);

	// Nothing was rotated by it: the successor works once its holder uses it, then the replaced token no more
	UserSession next;
	QVERIFY(fixture.Refresh(refreshed.refreshToken, next));
	QVERIFY(next.refreshToken != refreshed.refreshToken);
	UserSession late;
	QVERIFY2(!fixture.Refresh(session.refreshToken, late), "Its successor was replaced too");
	QCOMPARE(fixture.Request(next.accessToken), Creator::CCS_OK);
}


// A rotated refresh token used after the leeway was copied: the session is closed for its holders too
void CSessionTokenExpiryTest::ReusedRefreshTokenClosesTheSession()
{
	CExpiryFixture fixture(3600, 3600, 300, 0);
	UserSession session;
	QVERIFY(fixture.Login(session, true));
	UserSession refreshed;
	QVERIFY(fixture.Refresh(session.refreshToken, refreshed));
	UserSession current;
	QVERIFY(fixture.Refresh(refreshed.refreshToken, current));
	QCOMPARE(fixture.Request(current.accessToken), Creator::CCS_OK);
	QTest::qWait(20);

	// Replaced twice: not a lost answer any more
	UserSession replayed;
	fixture.store->failRemoval = true;
	QVERIFY(!fixture.Refresh(session.refreshToken, replayed));
	QCOMPARE(fixture.store->Count(), 1);
	fixture.store->failRemoval = false;
	QVERIFY(!fixture.Refresh(session.refreshToken, replayed));
	QCOMPARE(fixture.store->Count(), 0);
	QCOMPARE(fixture.Request(current.accessToken), Creator::CCS_UNAUTHORIZED);
	UserSession next;
	QVERIFY2(!fixture.Refresh(current.refreshToken, next), "The current refresh token goes with the session");
}


// Within the leeway a token replaced twice is only refused: clients of the session refreshing at once
void CSessionTokenExpiryTest::ReusedRefreshTokenWithinTheLeewayIsRefused()
{
	CExpiryFixture fixture;
	UserSession session;
	QVERIFY(fixture.Login(session, true));
	UserSession refreshed;
	QVERIFY(fixture.Refresh(session.refreshToken, refreshed));
	UserSession current;
	QVERIFY(fixture.Refresh(refreshed.refreshToken, current));

	UserSession replayed;
	QVERIFY(!fixture.Refresh(session.refreshToken, replayed));

	QCOMPARE(fixture.Request(current.accessToken), Creator::CCS_OK);
	UserSession next;
	QVERIFY(fixture.Refresh(current.refreshToken, next));
}


// A session refreshed again and again keeps its rotated tokens: an old one is told from an unknown one while it lives
void CSessionTokenExpiryTest::RotatedTokensLiveWithTheirSession()
{
	CExpiryFixture fixture(3600, 2, 300, 0);
	UserSession session;
	QVERIFY(fixture.Login(session, true));
	UserSession first;
	QVERIFY(fixture.Refresh(session.refreshToken, first));
	QTest::qWait(1200);
	UserSession second;
	QVERIFY(fixture.Refresh(first.refreshToken, second));
	QTest::qWait(1200);
	// Past the end the session had when the first token was replaced, before the end it has now
	UserSession third;
	QVERIFY(fixture.Refresh(second.refreshToken, third));

	UserSession replayed;
	QVERIFY(!fixture.Refresh(session.refreshToken, replayed));
	UserSession next;
	QVERIFY2(!fixture.Refresh(third.refreshToken, next), "The old token was taken as unknown: the session stays open");
}


// An unknown token tells nothing about a session: nothing is closed
void CSessionTokenExpiryTest::UnknownRefreshTokenClosesNothing()
{
	CExpiryFixture fixture(3600, 3600, 300, 0);
	UserSession session;
	QVERIFY(fixture.Login(session));
	QCOMPARE(fixture.Request(session.accessToken), Creator::CCS_OK);

	UserSession refused;
	QVERIFY(!fixture.Refresh("unknown-refresh-token", refused));

	QCOMPARE(fixture.Request(session.accessToken), Creator::CCS_OK);
	UserSession refreshed;
	QVERIFY(fixture.Refresh(session.refreshToken, refreshed));
}


void CSessionTokenExpiryTest::ConcurrentRefreshRotatesOnce()
{
	CExpiryFixture fixture;
	UserSession session;
	QVERIFY(fixture.Login(session, true));

	// Several requests fail with the expired token at once, each refreshes with the same refresh token
	const int threadCount = 8;
	std::atomic<int> successes{0};
	QMutex tokensMutex;
	QByteArrayList accessTokens;
	QSet<QByteArray> refreshTokens;
	std::vector<std::thread> threads;
	for (int index = 0; index < threadCount; ++index){
		threads.emplace_back([&fixture, &session, &successes, &tokensMutex, &accessTokens, &refreshTokens](){
			UserSession refreshed;
			if (fixture.Refresh(session.refreshToken, refreshed)){
				++successes;
				QMutexLocker locker(&tokensMutex);
				accessTokens << refreshed.accessToken;
				refreshTokens.insert(refreshed.refreshToken);
			}
		});
	}
	for (std::thread& thread : threads){
		thread.join();
	}

	// Rotated once: every request gets the same new refresh token, none is refused
	QCOMPARE(successes.load(), threadCount);
	QCOMPARE(refreshTokens.size(), 1);
	QVERIFY(!refreshTokens.contains(session.refreshToken));
	QCOMPARE(fixture.store->Count(), 1);
	for (const QByteArray& accessToken : accessTokens){
		QCOMPARE(fixture.Request(accessToken), Creator::CCS_OK);
	}
}


void CSessionTokenExpiryTest::CachedTokenIsNotServedPastExpiry()
{
	CExpiryFixture fixture(1);
	UserSession session;
	QVERIFY(fixture.Login(session));

	// Validated once and cached
	QCOMPARE(fixture.Request(session.accessToken), Creator::CCS_OK);
	QCOMPARE(fixture.Request(session.accessToken), Creator::CCS_OK);

	QVERIFY(CExpiryFixture::WaitForExpiry(session.accessToken));

	// The cache does not outlive the token
	QCOMPARE(fixture.Request(session.accessToken), Creator::CCS_UNAUTHORIZED);

	UserSession refreshed;
	QVERIFY(fixture.Refresh(session.refreshToken, refreshed));
	QCOMPARE(fixture.Request(refreshed.accessToken), Creator::CCS_OK);
}


void CSessionTokenExpiryTest::EstablishedSubscriptionOutlivesTheToken()
{
	// The access is checked again every second
	CExpiryFixture fixture(1, 3600, 1);
	UserSession session;
	QVERIFY(fixture.Login(session));
	QVERIFY(fixture.Start("subscription", session.accessToken));

	const qint64 expSecs = CExpiryFixture::Expiry(session.accessToken);
	QVERIFY(CExpiryFixture::WaitForExpiry(session.accessToken));

	// Its session lives: the subscription is confirmed with the expired token, again and again
	QTRY_VERIFY_WITH_TIMEOUT(fixture.ChecksAfterExpiry(session.accessToken, expSecs, Creator::AS_CONFIRMED) >= 2, 5000);
	QCOMPARE(fixture.ChecksAfterExpiry(session.accessToken, expSecs, Creator::AS_DENIED), 0);
	QCOMPARE(fixture.controller->removals.load(), 0);
	QVERIFY(fixture.servlet->ErrorCodes().isEmpty());

	// While a request with the same token is refused: the client has to refresh for anything new
	QCOMPARE(fixture.Request(session.accessToken), Creator::CCS_UNAUTHORIZED);
}


void CSessionTokenExpiryTest::ResubscribeWithTheRefreshedToken()
{
	CExpiryFixture fixture(1, 3600, 1);
	UserSession session;
	QVERIFY(fixture.Login(session));
	QVERIFY(fixture.Start("subscription", session.accessToken));
	QVERIFY(CExpiryFixture::WaitForExpiry(session.accessToken));

	// A request fails, the client refreshes and starts its subscriptions again: a stop, then a start with the new token
	QCOMPARE(fixture.Request(session.accessToken), Creator::CCS_UNAUTHORIZED);
	UserSession refreshed;
	QVERIFY(fixture.Refresh(session.refreshToken, refreshed));
	fixture.Stop("subscription", session.accessToken);
	QVERIFY(fixture.Start("subscription", refreshed.accessToken));
	QCOMPARE(fixture.controller->registrations.load(), 2);
	QCOMPARE(fixture.controller->removals.load(), 1);

	// The new credentials are checked, the old ones not any more
	QTRY_VERIFY_WITH_TIMEOUT(fixture.ChecksAfterExpiry(refreshed.accessToken, 0, Creator::AS_CONFIRMED) >= 2, 5000);
	const QList<CObservedAuthenticationManager::Check> checks = fixture.authentication->Checks();
	bool isRefreshedChecked = false;
	for (const CObservedAuthenticationManager::Check& check : checks){
		isRefreshedChecked = isRefreshedChecked || check.token == refreshed.accessToken;
		QVERIFY(!isRefreshedChecked || check.token == refreshed.accessToken);
		QVERIFY(check.state == Creator::AS_CONFIRMED);
	}
	QCOMPARE(fixture.controller->removals.load(), 1);
	QVERIFY(fixture.servlet->ErrorCodes().isEmpty());
}


void CSessionTokenExpiryTest::RemoteSourceAsksForANewToken()
{
	// A product server without a token of its own reads the user with the token of the user; once expired, it is refused
	CExpiryFixture fixture(1, 3600, 1);
	fixture.users->isExpiredTokenRefused = true;
	UserSession session;
	QVERIFY(fixture.Login(session));
	QVERIFY(fixture.Start("subscription", session.accessToken));
	QVERIFY(CExpiryFixture::WaitForExpiry(session.accessToken));

	// Not a refusal of the access, which the client would not recover from: it refreshes and subscribes again
	QTRY_COMPARE_WITH_TIMEOUT(fixture.controller->removals.load(), 1, 5000);
	QCOMPARE(fixture.servlet->ErrorCodes(), QByteArrayList{imtservergql::s_subscriptionUnauthorizedCode});

	UserSession refreshed;
	QVERIFY(fixture.Refresh(session.refreshToken, refreshed));
	QVERIFY(fixture.Start("subscription", refreshed.accessToken));
	QCOMPARE(fixture.controller->registrations.load(), 2);

	// A disabled user stays refused
	CExpiryFixture disabledFixture;
	disabledFixture.users->isExpiredTokenRefused = true;
	disabledFixture.users->user.SetEnabled(false);
	UserSession disabledSession;
	QVERIFY(disabledFixture.Login(disabledSession));
	QVERIFY(!disabledFixture.Start("subscription", disabledSession.accessToken));
	QCOMPARE(disabledFixture.servlet->ErrorCodes(), QByteArrayList{imtservergql::s_subscriptionForbiddenCode});
}


void CSessionTokenExpiryTest::LogoutThroughProductServerDropsTheToken()
{
	CExpiryFixture fixture;
	icomp::TSimSharedComponentPtr<CAuthorizationServerStubComp> network;
	network->onLogout = [&fixture](){ fixture.store->RemoveElements(fixture.store->GetElementIds(0, -1, nullptr, nullptr), nullptr); };
	network->InitComponent();
	icomp::TSimSharedComponentPtr<imtauthgql::CRemoteAuthorizationControllerComp> remoteAuthorization;
	remoteAuthorization->SetRef("ApiClient", network);
	remoteAuthorization->SetRef("JwtSessionController", fixture.authentication);
	remoteAuthorization->InitComponent();

	UserSession session;
	QVERIFY(fixture.Login(session));
	QCOMPARE(fixture.Request(session.accessToken), Creator::CCS_OK);

	imtgql::CGqlRequest logout(imtgql::IGqlRequest::RT_MUTATION, "Logout");
	imtgql::CGqlParamObject input;
	input.InsertParam("accessToken", QVariant(session.accessToken));
	logout.AddParam("input", input);
	logout.AddSimpleField("ok");
	imtgql::IGqlContextUniquePtr contextPtr(new imtgql::CGqlContext);
	contextPtr->SetToken(session.accessToken);
	logout.SetGqlContext(imtgql::IGqlContextSharedPtr::CreateFromUnique(std::move(contextPtr)));

	QString errorMessage;
	const QJsonObject answer = remoteAuthorization->CreateResponse(logout, errorMessage);
	QVERIFY2(errorMessage.isEmpty(), qPrintable(errorMessage));
	QVERIFY2(QJsonDocument(answer).toJson().contains("true"), QJsonDocument(answer).toJson().constData());

	// The session ended at the authorization server; this server drops the token it had validated
	QCOMPARE(fixture.Request(session.accessToken), Creator::CCS_UNAUTHORIZED);
}


void CSessionTokenExpiryTest::SessionEndEndsTheSubscription()
{
	// No control check within the test: only the end of the session triggers one
	CExpiryFixture fixture(1, 2, 300);
	UserSession session;
	QVERIFY(fixture.Login(session));
	QVERIFY(fixture.Start("subscription", session.accessToken));

	QTRY_COMPARE_WITH_TIMEOUT(fixture.controller->removals.load(), 1, 6000);
	QCOMPARE(fixture.servlet->ErrorCodes(), QByteArrayList{imtservergql::s_subscriptionUnauthorizedCode});

	// The session is over: no refresh brings it back
	UserSession refreshed;
	QVERIFY(!fixture.Refresh(session.refreshToken, refreshed));
}


void CSessionTokenExpiryTest::RefreshMovesTheSessionEnd()
{
	// A session of 3 seconds, refreshed after 1.5 seconds by a request; the subscription keeps its first token
	CExpiryFixture fixture(1, 3, 300);
	UserSession session;
	QVERIFY(fixture.Login(session));
	QElapsedTimer timer;
	timer.start();
	QVERIFY(fixture.Start("subscription", session.accessToken));

	QTest::qWait(1500);
	UserSession refreshed;
	QVERIFY(fixture.Refresh(session.refreshToken, refreshed));

	// The check at the first end of the session finds it moved and keeps the subscription
	QTRY_VERIFY_WITH_TIMEOUT(fixture.authentication->Checks().size() >= 1, 4000);
	QVERIFY2(timer.elapsed() < 4400, qPrintable(QString::number(timer.elapsed())));
	QCOMPARE(fixture.authentication->Checks().first().state, Creator::AS_CONFIRMED);
	QCOMPARE(fixture.controller->removals.load(), 0);
	QVERIFY(fixture.servlet->ErrorCodes().isEmpty());

	// And ends it at the new end, when no refresh came
	QTRY_COMPARE_WITH_TIMEOUT(fixture.controller->removals.load(), 1, 4000);
	QVERIFY2(timer.elapsed() >= 4400, qPrintable(QString::number(timer.elapsed())));
	QCOMPARE(fixture.servlet->ErrorCodes(), QByteArrayList{imtservergql::s_subscriptionUnauthorizedCode});
}


I_ADD_TEST(CSessionTokenExpiryTest);
