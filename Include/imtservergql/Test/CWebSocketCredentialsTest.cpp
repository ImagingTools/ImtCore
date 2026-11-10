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


class CCredentialContextComp: public icomp::CComponentBase, virtual public imtgql::IGqlContextCreator
{
public:
	typedef icomp::CComponentBase BaseClass;
	I_BEGIN_COMPONENT(CCredentialContextComp)
		I_REGISTER_INTERFACE(imtgql::IGqlContextCreator);
	I_END_COMPONENT

	mutable std::atomic<int> userChecks{0};
	mutable std::atomic<int> serverChecks{0};
	std::atomic<int> userStatus{CCS_OK};
	std::atomic<int> serverStatus{CCS_OK};
	std::atomic<bool> changed{false};

	imtgql::IGqlContextUniquePtr CreateGqlContext(const imtgql::IGqlContext::Headers&, ContextCreationError& error) const override
	{
		error.status = CCS_OK;
		return imtgql::IGqlContextUniquePtr(new imtgql::CGqlContext);
	}

	bool IsRevocableToken(const QByteArray& token) const override
	{
		return !token.isEmpty();
	}

	ContextCreationStatus CheckToken(const QByteArray& token, QByteArrayList& scopes, QString& error) const override
	{
		const bool isUser = token == "user-credential";
		(isUser ? userChecks : serverChecks).fetch_add(1);
		const auto status = static_cast<ContextCreationStatus>((isUser ? userStatus : serverStatus).load());
		scopes = changed ? QByteArrayList{"read"} : QByteArrayList{"read", "write"};
		if (status != CCS_OK){
			error = QStringLiteral("Credential refused");
		}
		return status;
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


void CWebSocketCredentialsTest::RegistrationRollback_data()
{
	QTest::addColumn<int>("failure");
	QTest::newRow("user-revoked") << 0;
	QTest::newRow("server-revoked") << 1;
	QTest::newRow("check-unavailable") << 2;
	QTest::newRow("scopes-changed") << 3;
}


void CWebSocketCredentialsTest::RegistrationRollback()
{
	QFETCH(int, failure);
	CSubscriptionFixture fixture;
	fixture.controller->onRegister = [&](){
		if (failure == 0){
			fixture.creator->userStatus = imtgql::IGqlContextCreator::CCS_FORBIDDEN;
		}
		else if (failure == 1){
			fixture.creator->serverStatus = imtgql::IGqlContextCreator::CCS_FORBIDDEN;
		}
		else if (failure == 2){
			fixture.creator->userStatus = imtgql::IGqlContextCreator::CCS_INTERNAL_ERROR;
		}
		else{
			fixture.creator->changed = true;
		}
	};
	fixture.servlet->RegisterSubscription(fixture.request);
	QCOMPARE(fixture.controller->registrations.load(), 1);
	QCOMPARE(fixture.controller->removals.load(), 1);
	QVERIFY(!fixture.servlet->error.isEmpty());
	QVERIFY(fixture.servlet->response.isEmpty());
}


void CWebSocketCredentialsTest::BothCredentialsAreMonitored_data()
{
	QTest::addColumn<bool>("user");
	QTest::newRow("user-token") << true;
	QTest::newRow("server-token") << false;
}


void CWebSocketCredentialsTest::BothCredentialsAreMonitored()
{
	QFETCH(bool, user);
	CSubscriptionFixture fixture;
	fixture.servlet->RegisterSubscription(fixture.request);
	QVERIFY2(fixture.servlet->error.isEmpty(), fixture.servlet->error.constData());
	QVERIFY(fixture.servlet->response.contains("start_ack"));
	QVERIFY(fixture.creator->userChecks >= 2);
	QVERIFY(fixture.creator->serverChecks >= 2);
	auto& status = user ? fixture.creator->userStatus : fixture.creator->serverStatus;
	status = imtgql::IGqlContextCreator::CCS_INTERNAL_ERROR;
	QTest::qWait(1200);
	QCOMPARE(fixture.controller->removals.load(), 0);
	status = imtgql::IGqlContextCreator::CCS_FORBIDDEN;
	QTRY_COMPARE_WITH_TIMEOUT(fixture.controller->removals.load(), 1, 3000);
}


void CWebSocketCredentialsTest::AnonymousSubscription()
{
	CSubscriptionFixture fixture;
	fixture.request.SetBody(R"({"id":"subscription","type":"start","payload":{"query":"subscription { changed { id } }"}})");
	fixture.servlet->RegisterSubscription(fixture.request);
	QVERIFY(fixture.servlet->error.isEmpty());
	QVERIFY(fixture.servlet->response.contains("start_ack"));
	QCOMPARE(fixture.creator->userChecks.load(), 0);
	QCOMPARE(fixture.creator->serverChecks.load(), 0);
}


I_ADD_TEST(CWebSocketCredentialsTest);
