// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include "CTenantConnectionRequestTest.h"

// ImtCore includes
#include <imtauth/CTenantConnectionRequest.h>


namespace
{


imtauth::TenantConnectionRequestInfo MakeSampleInfo()
{
	imtauth::TenantConnectionRequestInfo info;
	info.requestId = "request-1";
	info.sourceTenantId = "tenantA";
	info.targetTenantId = "tenantB";
	info.targetIdentifier = "tenantB@example.com";
	info.connectCode = "ABCD1234EFGH";
	info.proposedSourceRole = imtauth::ITenantInfo::Supplier;
	info.proposedTargetRole = imtauth::ITenantInfo::Customer;
	info.message = "Let's connect";
	info.status = imtauth::TCS_PENDING;
	info.createdAt = "2026-01-01T00:00:00.000Z";
	info.expiresAt = "2030-01-01T00:00:00.000Z";
	info.respondedAt = "";
	return info;
}


} // anonymous namespace


void CTenantConnectionRequestTest::init()
{
	m_managerPtr = new imtauth::CMockTenantConnectionRequestManager();
}


void CTenantConnectionRequestTest::cleanup()
{
	delete m_managerPtr;
	m_managerPtr = nullptr;
}


void CTenantConnectionRequestTest::testRequestInfo_RoundTrip()
{
	imtauth::CTenantConnectionRequest request;
	imtauth::TenantConnectionRequestInfo info = MakeSampleInfo();
	request.SetRequestInfo(info);

	QCOMPARE(request.GetRequestId(), QByteArray("request-1"));
	QVERIFY(request.GetRequestInfo() == info);
}


void CTenantConnectionRequestTest::testRequestInfo_CopyFromAndClone()
{
	imtauth::CTenantConnectionRequest source;
	source.SetRequestInfo(MakeSampleInfo());

	imtauth::CTenantConnectionRequest target;
	QVERIFY(target.CopyFrom(source));
	QVERIFY(target.GetRequestInfo() == source.GetRequestInfo());

	istd::IChangeableUniquePtr clonePtr = source.CloneMe();
	QVERIFY(clonePtr.IsValid());
	const imtauth::ITenantConnectionRequestData* clonedPtr =
				dynamic_cast<const imtauth::ITenantConnectionRequestData*>(clonePtr.GetPtr());
	QVERIFY(clonedPtr != nullptr);
	QVERIFY(clonedPtr->GetRequestInfo() == source.GetRequestInfo());
}


void CTenantConnectionRequestTest::testRequestInfo_ResetDataDefaults()
{
	imtauth::CTenantConnectionRequest request;
	request.SetRequestInfo(MakeSampleInfo());

	QVERIFY(request.ResetData());

	imtauth::TenantConnectionRequestInfo info = request.GetRequestInfo();
	QVERIFY(info.requestId.isEmpty());
	QVERIFY(info.sourceTenantId.isEmpty());
	QVERIFY(info.connectCode.isEmpty());
	QCOMPARE(info.proposedSourceRole, imtauth::ITenantInfo::Partner);
	QCOMPARE(info.proposedTargetRole, imtauth::ITenantInfo::Partner);
	QCOMPARE(info.status, imtauth::TCS_PENDING);
}


void CTenantConnectionRequestTest::testCreateRequest_Success()
{
	QByteArray requestId = m_managerPtr->CreateConnectionRequest(
				"tenantA", "tenantB@example.com",
				imtauth::ITenantInfo::Supplier, imtauth::ITenantInfo::Customer);
	QVERIFY(!requestId.isEmpty());
	QCOMPARE(m_managerPtr->m_requests.size(), 1);

	imtauth::TenantConnectionRequestInfo info = m_managerPtr->GetConnectionRequest(requestId);
	QCOMPARE(info.status, imtauth::TCS_PENDING);
	QCOMPARE(info.proposedSourceRole, imtauth::ITenantInfo::Supplier);
}


void CTenantConnectionRequestTest::testCreateRequest_MissingFields_Fails()
{
	QVERIFY(m_managerPtr->CreateConnectionRequest(
				"", "tenantB@example.com",
				imtauth::ITenantInfo::Partner, imtauth::ITenantInfo::Partner).isEmpty());
	QVERIFY(m_managerPtr->CreateConnectionRequest(
				"tenantA", "",
				imtauth::ITenantInfo::Partner, imtauth::ITenantInfo::Partner).isEmpty());
	QCOMPARE(m_managerPtr->m_requests.size(), 0);
}


void CTenantConnectionRequestTest::testCreateConnectCode_Success()
{
	QString code;
	QByteArray requestId = m_managerPtr->CreateConnectCode(
				"tenantA", imtauth::ITenantInfo::Supplier, imtauth::ITenantInfo::Customer,
				QString(), QString(), code);
	QVERIFY(!requestId.isEmpty());
	QVERIFY(!code.isEmpty());
	QCOMPARE(m_managerPtr->m_requests.size(), 1);
}


void CTenantConnectionRequestTest::testCreateConnectCode_MissingSource_Fails()
{
	QString code;
	QVERIFY(m_managerPtr->CreateConnectCode(
				"", imtauth::ITenantInfo::Partner, imtauth::ITenantInfo::Partner,
				QString(), QString(), code).isEmpty());
	QCOMPARE(m_managerPtr->m_requests.size(), 0);
}


void CTenantConnectionRequestTest::testAcceptRequest_EstablishesRelationship()
{
	QByteArray requestId = m_managerPtr->CreateConnectionRequest(
				"tenantA", "tenantB@example.com",
				imtauth::ITenantInfo::Supplier, imtauth::ITenantInfo::Customer);

	QVERIFY(m_managerPtr->AcceptConnectionRequest(requestId, "tenantB"));

	imtauth::TenantConnectionRequestInfo info = m_managerPtr->GetConnectionRequest(requestId);
	QCOMPARE(info.status, imtauth::TCS_ACCEPTED);
	QCOMPARE(info.targetTenantId, QByteArray("tenantB"));
	QVERIFY(!info.respondedAt.isEmpty());
	// Two mirror relationship entries created (source + accepting tenant).
	QCOMPARE(m_managerPtr->m_relationshipCount, 2);

	// Accepting an already accepted request fails.
	QVERIFY(!m_managerPtr->AcceptConnectionRequest(requestId, "tenantB"));
}


void CTenantConnectionRequestTest::testAcceptRequest_SameTenant_Fails()
{
	QByteArray requestId = m_managerPtr->CreateConnectionRequest(
				"tenantA", "tenantA@example.com",
				imtauth::ITenantInfo::Partner, imtauth::ITenantInfo::Partner);

	QVERIFY(!m_managerPtr->AcceptConnectionRequest(requestId, "tenantA"));
	QCOMPARE(m_managerPtr->m_relationshipCount, 0);
}


void CTenantConnectionRequestTest::testAcceptRequest_Expired_Fails()
{
	QByteArray requestId = m_managerPtr->CreateConnectionRequest(
				"tenantA", "tenantB@example.com",
				imtauth::ITenantInfo::Partner, imtauth::ITenantInfo::Partner,
				QString(), "2000-01-01T00:00:00.000Z");

	QVERIFY(!m_managerPtr->AcceptConnectionRequest(requestId, "tenantB"));

	imtauth::TenantConnectionRequestInfo info = m_managerPtr->GetConnectionRequest(requestId);
	QCOMPARE(info.status, imtauth::TCS_EXPIRED);
}


void CTenantConnectionRequestTest::testAcceptConnectCode_Success()
{
	QString code;
	m_managerPtr->CreateConnectCode(
				"tenantA", imtauth::ITenantInfo::Supplier, imtauth::ITenantInfo::Customer,
				QString(), QString(), code);

	QByteArray requestId = m_managerPtr->AcceptConnectCode(code, "tenantB");
	QVERIFY(!requestId.isEmpty());

	imtauth::TenantConnectionRequestInfo info = m_managerPtr->GetConnectionRequest(requestId);
	QCOMPARE(info.status, imtauth::TCS_ACCEPTED);
	QCOMPARE(info.targetTenantId, QByteArray("tenantB"));
	QCOMPARE(m_managerPtr->m_relationshipCount, 2);
}


void CTenantConnectionRequestTest::testAcceptConnectCode_Unknown_Fails()
{
	QVERIFY(m_managerPtr->AcceptConnectCode("DOES-NOT-EXIST", "tenantB").isEmpty());
}


void CTenantConnectionRequestTest::testRejectRequest_Pending()
{
	QByteArray requestId = m_managerPtr->CreateConnectionRequest(
				"tenantA", "tenantB@example.com",
				imtauth::ITenantInfo::Partner, imtauth::ITenantInfo::Partner);

	QVERIFY(m_managerPtr->RejectConnectionRequest(requestId));
	QCOMPARE(m_managerPtr->GetConnectionRequest(requestId).status, imtauth::TCS_REJECTED);
	QCOMPARE(m_managerPtr->m_relationshipCount, 0);
}


void CTenantConnectionRequestTest::testRevokeRequest_Pending()
{
	QByteArray requestId = m_managerPtr->CreateConnectionRequest(
				"tenantA", "tenantB@example.com",
				imtauth::ITenantInfo::Partner, imtauth::ITenantInfo::Partner);

	QVERIFY(m_managerPtr->RevokeConnectionRequest(requestId));
	QCOMPARE(m_managerPtr->GetConnectionRequest(requestId).status, imtauth::TCS_REVOKED);
}


void CTenantConnectionRequestTest::testRejectRequest_NonPending_Fails()
{
	QByteArray requestId = m_managerPtr->CreateConnectionRequest(
				"tenantA", "tenantB@example.com",
				imtauth::ITenantInfo::Partner, imtauth::ITenantInfo::Partner);

	QVERIFY(m_managerPtr->AcceptConnectionRequest(requestId, "tenantB"));
	// Cannot reject an already accepted request.
	QVERIFY(!m_managerPtr->RejectConnectionRequest(requestId));
}

