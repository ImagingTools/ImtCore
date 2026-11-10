// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include "CWebSocketCredentialsTest.h"


#include <atomic>
#include <functional>
#include <icomp/TSimComponentWrap.h>
#include <itest/CStandardTestExecutor.h>
#include <imtbase/imtbase.h>
#include <imtgql/CGqlContext.h>
#include <imtrest/CWebSocketProtocolEngineComp.h>
#include <imtservergql/CWebSocketServletComp.h>


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

	mutable std::atomic<int> renewals{0};
	std::atomic<int> renewState{AS_CONFIRMED};
	// What the access is decided on: the user of the context stands for it
	std::atomic<int> access{0};
	std::atomic<qint64> generation{0};

	imtgql::IGqlContextUniquePtr CreateGqlContext(const imtgql::IGqlContext::Headers&, ContextCreationError& error) const override
	{
		error.status = CCS_OK;
		return CreateAccessContext();
	}

	imtgql::IGqlContextUniquePtr RenewGqlContext(const imtgql::IGqlContext::Headers& headers, AuthorizationState& state, QString& error) const override
	{
		++renewals;
		state = static_cast<AuthorizationState>(renewState.load());
		if (headers.value(imtbase::s_authenticationTokenHeaderId) != "user-credential" || headers.value(imtbase::s_serverTokenHeaderId) != "server-credential"){
			// Checked with what the client sent at the registration
			state = AS_DENIED;
		}
		if (state != AS_CONFIRMED){
			error = QStringLiteral("Credential refused");
			return nullptr;
		}
		return CreateAccessContext();
	}

	qint64 GetAuthorizationGeneration() const override
	{
		return generation;
	}

private:
	imtgql::IGqlContextUniquePtr CreateAccessContext() const
	{
		imtgql::IGqlContextUniquePtr contextPtr(new imtgql::CGqlContext);
		contextPtr->SetUserId(QByteArray::number(access.load()));
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

	bool IsRequestSupported(const imtgql::CGqlRequest&) const override { return true; }
	bool IsOpenFor(SubscriberKind, const imtgql::CGqlRequest&) const override { return true; }
	bool RegisterSubscription(const QByteArray&, const imtgql::CGqlRequest&, const imtrest::IRequest&, QString&) override
	{
		++registrations;
		if (onRegister){
			onRegister();
		}
		return true;
	}
	bool UnregisterSubscription(const QByteArray&) override
	{
		++removals;
		return true;
	}
};


class CTestableWebSocketServlet: public imtservergql::CWebSocketServletComp
{
public:
	using CWebSocketServletComp::RegisterSubscription;
	mutable QByteArray response;
	mutable QByteArray error;

protected:
	imtrest::ConstResponsePtr CreateDataResponse(const QByteArray& data, const imtrest::IRequest&) const override
	{
		response = data;
		return imtrest::ConstResponsePtr();
	}
	imtrest::ConstResponsePtr CreateErrorResponse(const QByteArray& message, const imtrest::IRequest&, bool) const override
	{
		error = message;
		return imtrest::ConstResponsePtr();
	}
};


struct CSubscriptionFixture
{
	icomp::TSimSharedComponentPtr<CCredentialContextComp> creator;
	icomp::TSimSharedComponentPtr<CSubscriptionControllerComp> controller;
	icomp::TSimSharedComponentPtr<CTestableWebSocketServlet> servlet;
	imtrest::CWebSocketProtocolEngineComp engine;
	imtrest::CWebSocketRequest request{engine};

	CSubscriptionFixture()
	{
		creator->InitComponent();
		controller->InitComponent();
		servlet->SetRef("GqlContextCreator", creator);
		servlet->InsertMultiRef("GqlSubscriberControllers", controller);
		servlet->SetIntAttr("TokenCheckInterval", 1);
		servlet->InitComponent();
		request.SetMethodType(imtrest::CWebSocketRequest::MT_START);
		QJsonObject headers;
		headers.insert(QString::fromUtf8(imtbase::s_authenticationTokenHeaderId), "user-credential");
		headers.insert(QString::fromUtf8(imtbase::s_serverTokenHeaderId), "server-credential");
		QJsonObject message;
		message.insert("id", "subscription");
		message.insert("type", "start");
		message.insert("headers", headers);
		message.insert("payload", QJsonObject{{"query", "subscription { changed { id } }"}});
		request.SetBody(QJsonDocument(message).toJson());
	}
};


} // namespace


void CWebSocketCredentialsTest::RegistrationWithoutChange()
{
	CSubscriptionFixture fixture;
	fixture.servlet->RegisterSubscription(fixture.request);
	QVERIFY2(fixture.servlet->error.isEmpty(), fixture.servlet->error.constData());
	QVERIFY(fixture.servlet->response.contains("start_ack"));

	// Nothing changed during the registration: the context it was registered with is current
	QCOMPARE(fixture.creator->renewals.load(), 0);
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
			++fixture.creator->access;
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
	QCOMPARE(fixture.creator->renewals.load(), failure == 3 ? 3 : 1);
}


void CWebSocketCredentialsTest::RegistrationKeptWithSameAccess()
{
	CSubscriptionFixture fixture;
	fixture.controller->onRegister = [&](){ ++fixture.creator->generation; };
	fixture.servlet->RegisterSubscription(fixture.request);
	QVERIFY2(fixture.servlet->error.isEmpty(), fixture.servlet->error.constData());
	QVERIFY(fixture.servlet->response.contains("start_ack"));
	QCOMPARE(fixture.creator->renewals.load(), 1);
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
		++fixture.creator->access;
	}

	if (dropped){
		QTRY_COMPARE_WITH_TIMEOUT(fixture.controller->removals.load(), 1, 5000);
	}
	else{
		// Two checks without an end
		QTRY_VERIFY_WITH_TIMEOUT(fixture.creator->renewals.load() >= 2, 5000);
		QCOMPARE(fixture.controller->removals.load(), 0);
	}
}


void CWebSocketCredentialsTest::AnonymousSubscription()
{
	CSubscriptionFixture fixture;
	fixture.request.SetBody(R"({"id":"subscription","type":"start","payload":{"query":"subscription { changed { id } }"}})");
	fixture.controller->onRegister = [&](){ ++fixture.creator->generation; };
	fixture.servlet->RegisterSubscription(fixture.request);
	QVERIFY(fixture.servlet->error.isEmpty());
	QVERIFY(fixture.servlet->response.contains("start_ack"));
	QTest::qWait(1200);
	QCOMPARE(fixture.creator->renewals.load(), 0);
}


I_ADD_TEST(CWebSocketCredentialsTest);
