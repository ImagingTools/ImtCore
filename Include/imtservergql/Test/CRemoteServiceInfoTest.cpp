// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include "CRemoteServiceInfoTest.h"


#include <QtCore/QElapsedTimer>
#include <QtCore/QJsonDocument>
#include <QtCore/QJsonObject>
#include <QtCore/QPromise>
#include <icomp/TSimComponentWrap.h>
#include <imod/TModelWrap.h>
#include <itest/CStandardTestExecutor.h>
#include <imtclientgql/CRemoteServiceInfoComp.h>
#include <imtgql/CGqlRequest.h>
#include <imtgql/CGqlResponse.h>


namespace
{


// The service answers the requests in the order of the script, then accepts the token
class CScriptedClientComp: public icomp::CComponentBase, virtual public imtclientgql::IAsyncGqlClient
{
public:
	typedef icomp::CComponentBase BaseClass;
	I_BEGIN_COMPONENT(CScriptedClientComp)
		I_REGISTER_INTERFACE(imtclientgql::IAsyncGqlClient);
	I_END_COMPONENT

	mutable QList<ErrorCategory> script;
	mutable QList<qint64> requestTimes;
	mutable QPromise<GqlResult>* pendingSuccess = nullptr;
	QElapsedTimer clock;

	QFuture<GqlResult> SendRequest(GqlRequestPtr requestPtr, imtbase::IUrlParam*) const override
	{
		requestTimes.append(clock.elapsed());
		const ErrorCategory category = script.isEmpty() ? EC_NONE : script.takeFirst();
		if (category != EC_NONE){
			return QtFuture::makeReadyValueFuture(GqlResult{{}, category, QStringLiteral("Refused")});
		}

		const QJsonObject root{{"data", QJsonObject{{"GetApplicationInfo", QJsonObject{{"applicationName", "Puma"}}}}}};
		auto* responsePtr = new imtgql::CGqlResponse(requestPtr);
		responsePtr->SetResponseData(QJsonDocument(root).toJson(QJsonDocument::Compact));
		GqlResponsePtr retVal;
		retVal.SetPtr(responsePtr);

		GqlResult result{retVal, EC_NONE, {}};
		if (pendingSuccess != nullptr){
			pendingSuccess->start();
			pendingSuccess->addResult(result);
			QFuture<GqlResult> future = pendingSuccess->future();
			pendingSuccess = nullptr;
			return future;
		}

		return QtFuture::makeReadyValueFuture(result);
	}
};


class CConnectedStatus: virtual public imtcom::IConnectionStatusProvider
{
public:
	ConnectionStatus GetConnectionStatus() const override { return CS_CONNECTED; }
};


class CConnectionStatusComp: public icomp::CComponentBase, public imod::TModelWrap<CConnectedStatus>
{
public:
	typedef icomp::CComponentBase BaseClass;
	I_BEGIN_COMPONENT(CConnectionStatusComp)
		I_REGISTER_INTERFACE(imtcom::IConnectionStatusProvider);
		I_REGISTER_INTERFACE(imod::IModel);
	I_END_COMPONENT
};


class CCredentialComp: public icomp::CComponentBase, virtual public imtclientgql::IServerCredentialProvider
{
public:
	typedef icomp::CComponentBase BaseClass;
	I_BEGIN_COMPONENT(CCredentialComp)
		I_REGISTER_INTERFACE(imtclientgql::IServerCredentialProvider);
	I_END_COMPONENT

	QByteArray GetServerToken() const override { return "imt_srv_token"; }
};


struct CServiceInfoFixture
{
	icomp::TSimSharedComponentPtr<CScriptedClientComp> client;
	icomp::TSimSharedComponentPtr<CConnectionStatusComp> connection;
	icomp::TSimSharedComponentPtr<CCredentialComp> credential;
	icomp::TSimSharedComponentPtr<imtclientgql::CRemoteServiceInfoComp> info;

	explicit CServiceInfoFixture(const QList<imtclientgql::IAsyncGqlClient::ErrorCategory>& script)
	{
		client->script = script;
		client->clock.start();
		client->InitComponent();
		connection->InitComponent();
		credential->InitComponent();
		info->SetIdAttr("ServiceId", "AuthServer");
		info->SetIntAttr("RejectionRecheckInterval", 100);
		info->SetRef("AsyncClient", client);
		info->SetRef("ConnectionStatus", connection);
		info->SetRef("ServerCredentialProvider", credential);
		info->InitComponent();
	}

	imtcom::IServiceInfoProvider::CredentialStatus GetCredentialStatus()
	{
		return info->GetServiceInfo().credentialStatus;
	}
};


} // namespace


// A service that has just started may refuse a valid token for a moment: the status follows it in seconds, not in minutes
void CRemoteServiceInfoTest::RefusedTokenIsCheckedAgainSoon()
{
	CServiceInfoFixture fixture({imtclientgql::IAsyncGqlClient::EC_ACCESS_DENIED, imtclientgql::IAsyncGqlClient::EC_ACCESS_DENIED});

	QTRY_COMPARE(fixture.GetCredentialStatus(), imtcom::IServiceInfoProvider::CRS_REJECTED);
	QTRY_COMPARE_WITH_TIMEOUT(fixture.GetCredentialStatus(), imtcom::IServiceInfoProvider::CRS_VALID, 3000);

	const QList<qint64> times = fixture.client->requestTimes;
	QCOMPARE(times.size(), 3);
	const qint64 firstPause = times[1] - times[0];
	const qint64 secondPause = times[2] - times[1];
	QVERIFY2(firstPause >= 90, qPrintable(QString::number(firstPause)));
	QVERIFY2(secondPause >= 190, qPrintable(QStringLiteral("Twice as long: %1").arg(secondPause)));
	QCOMPARE(fixture.info->GetServiceInfo().applicationName, QStringLiteral("Puma"));

	// Accepted: back to the periodic check
	QTest::qWait(500);
	QCOMPARE(fixture.client->requestTimes.size(), 3);
}


// The service refused the token, then could not answer while it started: it is still asked again soon, not in minutes
void CRemoteServiceInfoTest::FailureDuringTheRefusalKeepsCheckingSoon()
{
	CServiceInfoFixture fixture({imtclientgql::IAsyncGqlClient::EC_ACCESS_DENIED, imtclientgql::IAsyncGqlClient::EC_NETWORK});

	QTRY_COMPARE(fixture.GetCredentialStatus(), imtcom::IServiceInfoProvider::CRS_REJECTED);
	QTRY_COMPARE_WITH_TIMEOUT(fixture.GetCredentialStatus(), imtcom::IServiceInfoProvider::CRS_UNAVAILABLE, 2000);
	QTRY_COMPARE_WITH_TIMEOUT(fixture.GetCredentialStatus(), imtcom::IServiceInfoProvider::CRS_VALID, 3000);
	QCOMPARE(fixture.client->requestTimes.size(), 3);
}


void CRemoteServiceInfoTest::SlowCredentialCheckIsNotSuperseded()
{
	QPromise<imtclientgql::IAsyncGqlClient::GqlResult> pending;
	CServiceInfoFixture fixture({imtclientgql::IAsyncGqlClient::EC_ACCESS_DENIED});
	fixture.client->pendingSuccess = &pending;

	QTRY_COMPARE(fixture.client->requestTimes.size(), 2);
	QTest::qWait(300);
	QCOMPARE(fixture.client->requestTimes.size(), 2);
	QCOMPARE(fixture.GetCredentialStatus(), imtcom::IServiceInfoProvider::CRS_REJECTED);
	pending.finish();
	QTRY_COMPARE(fixture.GetCredentialStatus(), imtcom::IServiceInfoProvider::CRS_VALID);
}


void CRemoteServiceInfoTest::AcceptedTokenIsCheckedPeriodically()
{
	CServiceInfoFixture fixture({});

	QTRY_COMPARE(fixture.GetCredentialStatus(), imtcom::IServiceInfoProvider::CRS_VALID);
	QTest::qWait(500);
	QCOMPARE(fixture.client->requestTimes.size(), 1);
}


I_ADD_TEST(CRemoteServiceInfoTest);
