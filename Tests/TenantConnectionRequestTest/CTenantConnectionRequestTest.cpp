// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include "CTenantConnectionRequestTest.h"

// ImtCore includes
#include <imtauth/CTenantConnectionRequest.h>


namespace
{


imtauth::ITenantConnectionRequest::ConnectionRequestInfo MakeSampleInfo()
{
imtauth::ITenantConnectionRequest::ConnectionRequestInfo info;
info.requestId = "request-1";
info.sourceTenantId = "tenantA";
info.targetTenantId = "tenantB";
info.connectionCode = "ABCD1234";
info.message = "Let's connect";
info.status = imtauth::ITenantConnectionRequest::CRS_PENDING;
info.createdAt = "2026-01-01T00:00:00.000Z";
info.respondedAt = "";
return info;
}


} // anonymous namespace


void CTenantConnectionRequestTest::init()
{
m_managerPtr = new imtauth::CMockConnectionManager();
}


void CTenantConnectionRequestTest::cleanup()
{
delete m_managerPtr;
m_managerPtr = nullptr;
}


// --- Data Object Tests ---

void CTenantConnectionRequestTest::testRequestInfo_RoundTrip()
{
imtauth::CTenantConnectionRequest data;
auto info = MakeSampleInfo();
data.SetRequestInfo(info);

auto retrieved = data.GetRequestInfo();
QCOMPARE(retrieved.requestId, info.requestId);
QCOMPARE(retrieved.sourceTenantId, info.sourceTenantId);
QCOMPARE(retrieved.targetTenantId, info.targetTenantId);
QCOMPARE(retrieved.connectionCode, info.connectionCode);
QCOMPARE(retrieved.message, info.message);
QCOMPARE(retrieved.status, info.status);
QCOMPARE(retrieved.createdAt, info.createdAt);
}


void CTenantConnectionRequestTest::testRequestInfo_CopyFromAndClone()
{
imtauth::CTenantConnectionRequest data;
auto info = MakeSampleInfo();
data.SetRequestInfo(info);

imtauth::CTenantConnectionRequest copy;
copy.CopyFrom(data);
auto retrieved = copy.GetRequestInfo();
QCOMPARE(retrieved.requestId, info.requestId);
QCOMPARE(retrieved.sourceTenantId, info.sourceTenantId);
QCOMPARE(retrieved.status, info.status);
}


void CTenantConnectionRequestTest::testRequestInfo_ResetDataDefaults()
{
imtauth::CTenantConnectionRequest data;
auto info = MakeSampleInfo();
data.SetRequestInfo(info);
data.ResetData();

auto retrieved = data.GetRequestInfo();
QVERIFY(retrieved.requestId.isEmpty());
QVERIFY(retrieved.sourceTenantId.isEmpty());
QCOMPARE(retrieved.status, imtauth::ITenantConnectionRequest::CRS_PENDING);
}


// --- Connection Code ---

void CTenantConnectionRequestTest::testGetConnectionCode_AutoCreates()
{
auto code = m_managerPtr->GetConnectionCode("tenantA");
QCOMPARE(code.tenantId, QByteArray("tenantA"));
QVERIFY(!code.connectionCode.isEmpty());
QVERIFY(code.allowConnectionsByCode);
}


void CTenantConnectionRequestTest::testRegenerateConnectionCode_ChangesCode()
{
auto code = m_managerPtr->GetConnectionCode("tenantA");
QString oldCode = code.connectionCode;

QString newCode = m_managerPtr->RegenerateConnectionCode("tenantA");
QVERIFY(newCode != oldCode);
QVERIFY(!newCode.isEmpty());
}


void CTenantConnectionRequestTest::testSetAllowConnectionsByCode()
{
m_managerPtr->GetConnectionCode("tenantA");
QVERIFY(m_managerPtr->SetAllowConnectionsByCode("tenantA", false));

auto code = m_managerPtr->GetConnectionCode("tenantA");
QVERIFY(!code.allowConnectionsByCode);
}


// --- Connection Requests ---

void CTenantConnectionRequestTest::testCreateConnectionRequest_Success()
{
auto codeB = m_managerPtr->GetConnectionCode("tenantB");
QByteArray requestId = m_managerPtr->CreateConnectionRequest("tenantA", codeB.connectionCode, "Hello");
QVERIFY(!requestId.isEmpty());
QCOMPARE(m_managerPtr->m_requests.size(), 1);
QCOMPARE(m_managerPtr->m_requests.first().status, imtauth::ITenantConnectionRequest::CRS_PENDING);
}


void CTenantConnectionRequestTest::testCreateConnectionRequest_InvalidCode_Fails()
{
QByteArray requestId = m_managerPtr->CreateConnectionRequest("tenantA", "INVALID_CODE");
QVERIFY(requestId.isEmpty());
}


void CTenantConnectionRequestTest::testCreateConnectionRequest_SelfConnect_Fails()
{
auto codeA = m_managerPtr->GetConnectionCode("tenantA");
QByteArray requestId = m_managerPtr->CreateConnectionRequest("tenantA", codeA.connectionCode);
QVERIFY(requestId.isEmpty());
}


void CTenantConnectionRequestTest::testCreateConnectionRequest_DisabledByCode_Fails()
{
m_managerPtr->GetConnectionCode("tenantB");
m_managerPtr->SetAllowConnectionsByCode("tenantB", false);
auto codeB = m_managerPtr->GetConnectionCode("tenantB");
QByteArray requestId = m_managerPtr->CreateConnectionRequest("tenantA", codeB.connectionCode);
QVERIFY(requestId.isEmpty());
}


void CTenantConnectionRequestTest::testCreateConnectionRequest_DuplicateConnection_Fails()
{
auto codeB = m_managerPtr->GetConnectionCode("tenantB");
QByteArray reqId = m_managerPtr->CreateConnectionRequest("tenantA", codeB.connectionCode);
m_managerPtr->ApproveConnectionRequest(reqId, "tenantB");

// Try to create another connection request to same pair
QByteArray duplicateReqId = m_managerPtr->CreateConnectionRequest("tenantA", codeB.connectionCode);
QVERIFY(duplicateReqId.isEmpty());
}


void CTenantConnectionRequestTest::testApproveConnectionRequest_CreatesConnection()
{
auto codeB = m_managerPtr->GetConnectionCode("tenantB");
QByteArray reqId = m_managerPtr->CreateConnectionRequest("tenantA", codeB.connectionCode);

QByteArray connId = m_managerPtr->ApproveConnectionRequest(reqId, "tenantB");
QVERIFY(!connId.isEmpty());
QCOMPARE(m_managerPtr->m_requests.first().status, imtauth::ITenantConnectionRequest::CRS_APPROVED);
QCOMPARE(m_managerPtr->m_connections.size(), 1);
QCOMPARE(m_managerPtr->m_connections.first().status, imtauth::ITenantConnectionRequest::CS_ACTIVE);
}


void CTenantConnectionRequestTest::testRejectConnectionRequest_Success()
{
auto codeB = m_managerPtr->GetConnectionCode("tenantB");
QByteArray reqId = m_managerPtr->CreateConnectionRequest("tenantA", codeB.connectionCode);

QVERIFY(m_managerPtr->RejectConnectionRequest(reqId, "tenantB"));
QCOMPARE(m_managerPtr->m_requests.first().status, imtauth::ITenantConnectionRequest::CRS_REJECTED);
}


void CTenantConnectionRequestTest::testCancelConnectionRequest_Success()
{
auto codeB = m_managerPtr->GetConnectionCode("tenantB");
QByteArray reqId = m_managerPtr->CreateConnectionRequest("tenantA", codeB.connectionCode);

QVERIFY(m_managerPtr->CancelConnectionRequest(reqId, "tenantA"));
QCOMPARE(m_managerPtr->m_requests.first().status, imtauth::ITenantConnectionRequest::CRS_CANCELED);
}


void CTenantConnectionRequestTest::testApproveConnectionRequest_NonPending_Fails()
{
auto codeB = m_managerPtr->GetConnectionCode("tenantB");
QByteArray reqId = m_managerPtr->CreateConnectionRequest("tenantA", codeB.connectionCode);
m_managerPtr->RejectConnectionRequest(reqId, "tenantB");

QByteArray connId = m_managerPtr->ApproveConnectionRequest(reqId, "tenantB");
QVERIFY(connId.isEmpty());
}


void CTenantConnectionRequestTest::testApproveConnectionRequest_WrongTenant_Fails()
{
auto codeB = m_managerPtr->GetConnectionCode("tenantB");
QByteArray reqId = m_managerPtr->CreateConnectionRequest("tenantA", codeB.connectionCode);

QByteArray connId = m_managerPtr->ApproveConnectionRequest(reqId, "tenantA");
QVERIFY(connId.isEmpty());
}


// --- Connections ---

void CTenantConnectionRequestTest::testGetConnections_ReturnsActive()
{
auto codeB = m_managerPtr->GetConnectionCode("tenantB");
QByteArray reqId = m_managerPtr->CreateConnectionRequest("tenantA", codeB.connectionCode);
m_managerPtr->ApproveConnectionRequest(reqId, "tenantB");

auto connections = m_managerPtr->GetConnections("tenantA");
QCOMPARE(connections.size(), 1);
connections = m_managerPtr->GetConnections("tenantB");
QCOMPARE(connections.size(), 1);
connections = m_managerPtr->GetConnections("tenantC");
QCOMPARE(connections.size(), 0);
}


void CTenantConnectionRequestTest::testRemoveConnection_CascadesRelationships()
{
auto codeB = m_managerPtr->GetConnectionCode("tenantB");
QByteArray reqId = m_managerPtr->CreateConnectionRequest("tenantA", codeB.connectionCode);
QByteArray connId = m_managerPtr->ApproveConnectionRequest(reqId, "tenantB");

// Create a relationship via proposal
QByteArray proposalId = m_managerPtr->CreateRelationshipProposal(QByteArray(), connId, "tenantA", "tenantB");
m_managerPtr->ApproveRelationshipProposal(proposalId, "tenantB");

QCOMPARE(m_managerPtr->GetTenantRelationshipIds("tenantA").size(), 1);

// Remove connection - should cascade
QVERIFY(m_managerPtr->RemoveConnection(connId, "tenantA"));
QCOMPARE(m_managerPtr->m_connections.first().status, imtauth::ITenantConnectionRequest::CS_REMOVED);
QCOMPARE(m_managerPtr->GetTenantRelationshipIds("tenantA").size(), 0);
}


// --- Relationship Proposals ---

void CTenantConnectionRequestTest::testCreateRelationshipProposal_Success()
{
auto codeB = m_managerPtr->GetConnectionCode("tenantB");
QByteArray reqId = m_managerPtr->CreateConnectionRequest("tenantA", codeB.connectionCode);
QByteArray connId = m_managerPtr->ApproveConnectionRequest(reqId, "tenantB");

QByteArray proposalId = m_managerPtr->CreateRelationshipProposal(QByteArray(), connId, "tenantA", "tenantB");
QVERIFY(!proposalId.isEmpty());
QCOMPARE(m_managerPtr->m_proposals.size(), 1);
QCOMPARE(m_managerPtr->m_proposals.first().status, imtauth::ITenantRelationshipProposalData::RPS_APPROVED_BY_INITIATOR);
}


void CTenantConnectionRequestTest::testCreateRelationshipProposal_NoConnection_Fails()
{
QByteArray proposalId = m_managerPtr->CreateRelationshipProposal(QByteArray(), "nonexistent-conn", "tenantA", "tenantB");
QVERIFY(proposalId.isEmpty());
}


void CTenantConnectionRequestTest::testApproveRelationshipProposal_CreatesRelationship()
{
auto codeB = m_managerPtr->GetConnectionCode("tenantB");
QByteArray reqId = m_managerPtr->CreateConnectionRequest("tenantA", codeB.connectionCode);
QByteArray connId = m_managerPtr->ApproveConnectionRequest(reqId, "tenantB");

QByteArray proposalId = m_managerPtr->CreateRelationshipProposal(QByteArray(), connId, "tenantA", "tenantB");
QByteArray relId = m_managerPtr->ApproveRelationshipProposal(proposalId, "tenantB");
QVERIFY(!relId.isEmpty());
QCOMPARE(m_managerPtr->m_proposals.first().status, imtauth::ITenantRelationshipProposalData::RPS_APPLIED);

QByteArrayList relIds = m_managerPtr->GetTenantRelationshipIds("tenantA");
QCOMPARE(relIds.size(), 1);
QCOMPARE(relIds.first(), relId);
}


void CTenantConnectionRequestTest::testRejectRelationshipProposal_Success()
{
auto codeB = m_managerPtr->GetConnectionCode("tenantB");
QByteArray reqId = m_managerPtr->CreateConnectionRequest("tenantA", codeB.connectionCode);
QByteArray connId = m_managerPtr->ApproveConnectionRequest(reqId, "tenantB");

QByteArray proposalId = m_managerPtr->CreateRelationshipProposal(QByteArray(), connId, "tenantA", "tenantB");
QVERIFY(m_managerPtr->RejectRelationshipProposal(proposalId, "tenantB"));
QCOMPARE(m_managerPtr->m_proposals.first().status, imtauth::ITenantRelationshipProposalData::RPS_REJECTED);
}


void CTenantConnectionRequestTest::testCancelRelationshipProposal_Success()
{
auto codeB = m_managerPtr->GetConnectionCode("tenantB");
QByteArray reqId = m_managerPtr->CreateConnectionRequest("tenantA", codeB.connectionCode);
QByteArray connId = m_managerPtr->ApproveConnectionRequest(reqId, "tenantB");

QByteArray proposalId = m_managerPtr->CreateRelationshipProposal(QByteArray(), connId, "tenantA", "tenantB");
QVERIFY(m_managerPtr->CancelRelationshipProposal(proposalId, "tenantA"));
QCOMPARE(m_managerPtr->m_proposals.first().status, imtauth::ITenantRelationshipProposalData::RPS_CANCELED);
}


void CTenantConnectionRequestTest::testApproveRelationshipProposal_WrongTenant_Fails()
{
auto codeB = m_managerPtr->GetConnectionCode("tenantB");
QByteArray reqId = m_managerPtr->CreateConnectionRequest("tenantA", codeB.connectionCode);
QByteArray connId = m_managerPtr->ApproveConnectionRequest(reqId, "tenantB");

QByteArray proposalId = m_managerPtr->CreateRelationshipProposal(QByteArray(), connId, "tenantA", "tenantB");
// TenantA is initiator, not counterparty
QByteArray relId = m_managerPtr->ApproveRelationshipProposal(proposalId, "tenantA");
QVERIFY(relId.isEmpty());
}


// --- Relationships ---

void CTenantConnectionRequestTest::testGetTenantRelationships_ReturnsActive()
{
auto codeB = m_managerPtr->GetConnectionCode("tenantB");
QByteArray reqId = m_managerPtr->CreateConnectionRequest("tenantA", codeB.connectionCode);
QByteArray connId = m_managerPtr->ApproveConnectionRequest(reqId, "tenantB");

QByteArray proposalId = m_managerPtr->CreateRelationshipProposal(QByteArray(), connId, "tenantA", "tenantB");
m_managerPtr->ApproveRelationshipProposal(proposalId, "tenantB");

QByteArrayList relsA = m_managerPtr->GetTenantRelationshipIds("tenantA");
QByteArrayList relsB = m_managerPtr->GetTenantRelationshipIds("tenantB");
QCOMPARE(relsA.size(), 1);
QCOMPARE(relsB.size(), 1);
QCOMPARE(relsA.first(), relsB.first());
}


void CTenantConnectionRequestTest::testRemoveTenantRelationship_Archives()
{
auto codeB = m_managerPtr->GetConnectionCode("tenantB");
QByteArray reqId = m_managerPtr->CreateConnectionRequest("tenantA", codeB.connectionCode);
QByteArray connId = m_managerPtr->ApproveConnectionRequest(reqId, "tenantB");

QByteArray proposalId = m_managerPtr->CreateRelationshipProposal(QByteArray(), connId, "tenantA", "tenantB");
QByteArray relationshipId = m_managerPtr->ApproveRelationshipProposal(proposalId, "tenantB");

QVERIFY(m_managerPtr->RemoveTenantRelationship("tenantA", relationshipId));
QCOMPARE(m_managerPtr->GetTenantRelationshipIds("tenantA").size(), 0);
}


QTEST_MAIN(CTenantConnectionRequestTest)
