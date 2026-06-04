// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include "CTenantConnectionRequestTest.h"

// ImtCore includes
#include <imtauth/CTenantConnectionRequest.h>


namespace
{


imtauth::ConnectionRequestInfo MakeSampleInfo()
{
	imtauth::ConnectionRequestInfo info;
	info.requestId = "request-1";
	info.sourceTenantId = "tenantA";
	info.targetTenantId = "tenantB";
	info.connectionCode = "ABCD1234";
	info.message = "Let's connect";
	info.status = imtauth::CRS_PENDING;
	info.createdAt = "2026-01-01T00:00:00.000Z";
	info.respondedAt = "";
	return info;
}


} // anonymous namespace


void CTenantConnectionRequestTest::init()
{
	m_managerPtr = new imtauth::CMockConnectionManager();
	// Ensure both tenants have connection codes
	m_managerPtr->GetConnectionCode("tenantA");
	m_managerPtr->GetConnectionCode("tenantB");
}


void CTenantConnectionRequestTest::cleanup()
{
	delete m_managerPtr;
	m_managerPtr = nullptr;
}


void CTenantConnectionRequestTest::testRequestInfo_RoundTrip()
{
	imtauth::CTenantConnectionRequest request;
	imtauth::ConnectionRequestInfo info = MakeSampleInfo();
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

	imtauth::ConnectionRequestInfo info = request.GetRequestInfo();
	QVERIFY(info.requestId.isEmpty());
	QVERIFY(info.sourceTenantId.isEmpty());
	QVERIFY(info.connectionCode.isEmpty());
	QCOMPARE(info.status, imtauth::CRS_PENDING);
}


// --- Connection Code tests ---

void CTenantConnectionRequestTest::testGetConnectionCode_AutoCreates()
{
	imtauth::TenantConnectionCodeInfo code = m_managerPtr->GetConnectionCode("tenantC");
	QCOMPARE(code.tenantId, QByteArray("tenantC"));
	QVERIFY(!code.connectionCode.isEmpty());
	QVERIFY(code.allowConnectionsByCode);
	QVERIFY(!code.createdAt.isEmpty());

	// Calling again returns the same code
	imtauth::TenantConnectionCodeInfo code2 = m_managerPtr->GetConnectionCode("tenantC");
	QCOMPARE(code.connectionCode, code2.connectionCode);
}


void CTenantConnectionRequestTest::testRegenerateConnectionCode_ChangesCode()
{
	imtauth::TenantConnectionCodeInfo original = m_managerPtr->GetConnectionCode("tenantA");
	QString newCode = m_managerPtr->RegenerateConnectionCode("tenantA");
	QVERIFY(!newCode.isEmpty());
	QVERIFY(newCode != original.connectionCode);
}


void CTenantConnectionRequestTest::testSetAllowConnectionsByCode()
{
	QVERIFY(m_managerPtr->SetAllowConnectionsByCode("tenantA", false));
	imtauth::TenantConnectionCodeInfo code = m_managerPtr->GetConnectionCode("tenantA");
	QVERIFY(!code.allowConnectionsByCode);

	QVERIFY(m_managerPtr->SetAllowConnectionsByCode("tenantA", true));
	code = m_managerPtr->GetConnectionCode("tenantA");
	QVERIFY(code.allowConnectionsByCode);
}


// --- Connection Request tests ---

void CTenantConnectionRequestTest::testCreateConnectionRequest_Success()
{
	imtauth::TenantConnectionCodeInfo targetCode = m_managerPtr->GetConnectionCode("tenantB");
	QByteArray requestId = m_managerPtr->CreateConnectionRequest(
				"tenantA", targetCode.connectionCode, "Hello");
	QVERIFY(!requestId.isEmpty());
	QCOMPARE(m_managerPtr->m_requests.size(), 1);
	QCOMPARE(m_managerPtr->m_requests.first().status, imtauth::CRS_PENDING);
	QCOMPARE(m_managerPtr->m_requests.first().message, QString("Hello"));
}


void CTenantConnectionRequestTest::testCreateConnectionRequest_InvalidCode_Fails()
{
	QByteArray requestId = m_managerPtr->CreateConnectionRequest(
				"tenantA", "INVALID-CODE");
	QVERIFY(requestId.isEmpty());
	QCOMPARE(m_managerPtr->m_requests.size(), 0);
}


void CTenantConnectionRequestTest::testCreateConnectionRequest_SelfConnect_Fails()
{
	imtauth::TenantConnectionCodeInfo ownCode = m_managerPtr->GetConnectionCode("tenantA");
	QByteArray requestId = m_managerPtr->CreateConnectionRequest(
				"tenantA", ownCode.connectionCode);
	QVERIFY(requestId.isEmpty());
}


void CTenantConnectionRequestTest::testCreateConnectionRequest_DisabledByCode_Fails()
{
	m_managerPtr->SetAllowConnectionsByCode("tenantB", false);
	imtauth::TenantConnectionCodeInfo targetCode = m_managerPtr->GetConnectionCode("tenantB");
	QByteArray requestId = m_managerPtr->CreateConnectionRequest(
				"tenantA", targetCode.connectionCode);
	QVERIFY(requestId.isEmpty());
}


void CTenantConnectionRequestTest::testCreateConnectionRequest_DuplicateConnection_Fails()
{
	imtauth::TenantConnectionCodeInfo targetCode = m_managerPtr->GetConnectionCode("tenantB");
	QByteArray requestId = m_managerPtr->CreateConnectionRequest(
				"tenantA", targetCode.connectionCode);
	QVERIFY(!requestId.isEmpty());

	// Approve to create connection
	m_managerPtr->ApproveConnectionRequest(requestId, "tenantB");

	// Try to create another request to same tenant — should fail (connection exists)
	QByteArray requestId2 = m_managerPtr->CreateConnectionRequest(
				"tenantA", targetCode.connectionCode);
	QVERIFY(requestId2.isEmpty());
}


void CTenantConnectionRequestTest::testApproveConnectionRequest_CreatesConnection()
{
	imtauth::TenantConnectionCodeInfo targetCode = m_managerPtr->GetConnectionCode("tenantB");
	QByteArray requestId = m_managerPtr->CreateConnectionRequest(
				"tenantA", targetCode.connectionCode);

	QByteArray connectionId = m_managerPtr->ApproveConnectionRequest(requestId, "tenantB");
	QVERIFY(!connectionId.isEmpty());

	QCOMPARE(m_managerPtr->m_requests.first().status, imtauth::CRS_APPROVED);
	QCOMPARE(m_managerPtr->m_connections.size(), 1);
	QCOMPARE(m_managerPtr->m_connections.first().status, imtauth::CS_ACTIVE);
}


void CTenantConnectionRequestTest::testRejectConnectionRequest_Success()
{
	imtauth::TenantConnectionCodeInfo targetCode = m_managerPtr->GetConnectionCode("tenantB");
	QByteArray requestId = m_managerPtr->CreateConnectionRequest(
				"tenantA", targetCode.connectionCode);

	QVERIFY(m_managerPtr->RejectConnectionRequest(requestId, "tenantB"));
	QCOMPARE(m_managerPtr->m_requests.first().status, imtauth::CRS_REJECTED);
	QCOMPARE(m_managerPtr->m_connections.size(), 0);
}


void CTenantConnectionRequestTest::testCancelConnectionRequest_Success()
{
	imtauth::TenantConnectionCodeInfo targetCode = m_managerPtr->GetConnectionCode("tenantB");
	QByteArray requestId = m_managerPtr->CreateConnectionRequest(
				"tenantA", targetCode.connectionCode);

	QVERIFY(m_managerPtr->CancelConnectionRequest(requestId, "tenantA"));
	QCOMPARE(m_managerPtr->m_requests.first().status, imtauth::CRS_CANCELED);
}


void CTenantConnectionRequestTest::testApproveConnectionRequest_NonPending_Fails()
{
	imtauth::TenantConnectionCodeInfo targetCode = m_managerPtr->GetConnectionCode("tenantB");
	QByteArray requestId = m_managerPtr->CreateConnectionRequest(
				"tenantA", targetCode.connectionCode);

	m_managerPtr->RejectConnectionRequest(requestId, "tenantB");
	// Cannot approve a rejected request
	QByteArray connectionId = m_managerPtr->ApproveConnectionRequest(requestId, "tenantB");
	QVERIFY(connectionId.isEmpty());
}


void CTenantConnectionRequestTest::testApproveConnectionRequest_WrongTenant_Fails()
{
	imtauth::TenantConnectionCodeInfo targetCode = m_managerPtr->GetConnectionCode("tenantB");
	QByteArray requestId = m_managerPtr->CreateConnectionRequest(
				"tenantA", targetCode.connectionCode);

	// Source tenant cannot approve their own request
	QByteArray connectionId = m_managerPtr->ApproveConnectionRequest(requestId, "tenantA");
	QVERIFY(connectionId.isEmpty());
}


// --- Connection tests ---

void CTenantConnectionRequestTest::testGetConnections_ReturnsActive()
{
	imtauth::TenantConnectionCodeInfo targetCode = m_managerPtr->GetConnectionCode("tenantB");
	QByteArray requestId = m_managerPtr->CreateConnectionRequest(
				"tenantA", targetCode.connectionCode);
	m_managerPtr->ApproveConnectionRequest(requestId, "tenantB");

	imtauth::TenantConnections connectionsA = m_managerPtr->GetConnections("tenantA");
	imtauth::TenantConnections connectionsB = m_managerPtr->GetConnections("tenantB");
	QCOMPARE(connectionsA.size(), 1);
	QCOMPARE(connectionsB.size(), 1);
	QCOMPARE(connectionsA.first().connectionId, connectionsB.first().connectionId);
}


void CTenantConnectionRequestTest::testRemoveConnection_CascadesRelationships()
{
	// Create connection
	imtauth::TenantConnectionCodeInfo targetCode = m_managerPtr->GetConnectionCode("tenantB");
	QByteArray requestId = m_managerPtr->CreateConnectionRequest(
				"tenantA", targetCode.connectionCode);
	QByteArray connectionId = m_managerPtr->ApproveConnectionRequest(requestId, "tenantB");

	// Create relationship via proposal
	imtauth::RelationshipProposalInfo proposal;
	proposal.connectionId = connectionId;
	proposal.initiatorTenantId = "tenantA";
	proposal.counterpartyTenantId = "tenantB";
	proposal.proposedSourceRole = imtauth::ITenantInfo::Supplier;
	proposal.proposedTargetRole = imtauth::ITenantInfo::Customer;
	QByteArray proposalId = m_managerPtr->CreateRelationshipProposal(proposal);
	m_managerPtr->ApproveRelationshipProposal(proposalId, "tenantB");

	QCOMPARE(m_managerPtr->GetTenantRelationships("tenantA").size(), 1);

	// Remove connection — should cascade archive relationships
	QVERIFY(m_managerPtr->RemoveConnection(connectionId, "tenantA"));
	QCOMPARE(m_managerPtr->m_connections.first().status, imtauth::CS_REMOVED);
	QCOMPARE(m_managerPtr->GetTenantRelationships("tenantA").size(), 0);
	QCOMPARE(m_managerPtr->m_relationships.first().status, 1); // Archived
}


// --- Relationship Proposal tests ---

void CTenantConnectionRequestTest::testCreateRelationshipProposal_Success()
{
	// Setup connection
	imtauth::TenantConnectionCodeInfo targetCode = m_managerPtr->GetConnectionCode("tenantB");
	QByteArray requestId = m_managerPtr->CreateConnectionRequest(
				"tenantA", targetCode.connectionCode);
	QByteArray connectionId = m_managerPtr->ApproveConnectionRequest(requestId, "tenantB");

	imtauth::RelationshipProposalInfo proposal;
	proposal.connectionId = connectionId;
	proposal.initiatorTenantId = "tenantA";
	proposal.counterpartyTenantId = "tenantB";
	proposal.proposalType = imtauth::RPT_CREATE;
	proposal.proposedSourceRole = imtauth::ITenantInfo::Supplier;
	proposal.proposedTargetRole = imtauth::ITenantInfo::Customer;

	QByteArray proposalId = m_managerPtr->CreateRelationshipProposal(proposal);
	QVERIFY(!proposalId.isEmpty());
	QCOMPARE(m_managerPtr->m_proposals.size(), 1);
	QCOMPARE(m_managerPtr->m_proposals.first().status, imtauth::RPS_APPROVED_BY_INITIATOR);
}


void CTenantConnectionRequestTest::testCreateRelationshipProposal_NoConnection_Fails()
{
	imtauth::RelationshipProposalInfo proposal;
	proposal.connectionId = "nonexistent";
	proposal.initiatorTenantId = "tenantA";
	proposal.counterpartyTenantId = "tenantB";

	QByteArray proposalId = m_managerPtr->CreateRelationshipProposal(proposal);
	QVERIFY(proposalId.isEmpty());
}


void CTenantConnectionRequestTest::testApproveRelationshipProposal_CreatesRelationship()
{
	// Setup connection
	imtauth::TenantConnectionCodeInfo targetCode = m_managerPtr->GetConnectionCode("tenantB");
	QByteArray requestId = m_managerPtr->CreateConnectionRequest(
				"tenantA", targetCode.connectionCode);
	QByteArray connectionId = m_managerPtr->ApproveConnectionRequest(requestId, "tenantB");

	// Create proposal
	imtauth::RelationshipProposalInfo proposal;
	proposal.connectionId = connectionId;
	proposal.initiatorTenantId = "tenantA";
	proposal.counterpartyTenantId = "tenantB";
	proposal.proposedSourceRole = imtauth::ITenantInfo::Supplier;
	proposal.proposedTargetRole = imtauth::ITenantInfo::Customer;
	proposal.proposedScope = "Manufacturing";
	QByteArray proposalId = m_managerPtr->CreateRelationshipProposal(proposal);

	// Counterparty approves
	QByteArray relationshipId = m_managerPtr->ApproveRelationshipProposal(proposalId, "tenantB");
	QVERIFY(!relationshipId.isEmpty());
	QCOMPARE(m_managerPtr->m_proposals.first().status, imtauth::RPS_APPLIED);
	QCOMPARE(m_managerPtr->m_relationships.size(), 1);

	imtauth::ITenantInfo::TenantRelationship rel = m_managerPtr->m_relationships.first();
	QCOMPARE(rel.sourceRole, imtauth::ITenantInfo::Supplier);
	QCOMPARE(rel.targetRole, imtauth::ITenantInfo::Customer);
	QCOMPARE(rel.scope, QString("Manufacturing"));
	QCOMPARE(rel.status, 0); // Active
}


void CTenantConnectionRequestTest::testRejectRelationshipProposal_Success()
{
	// Setup connection
	imtauth::TenantConnectionCodeInfo targetCode = m_managerPtr->GetConnectionCode("tenantB");
	QByteArray requestId = m_managerPtr->CreateConnectionRequest(
				"tenantA", targetCode.connectionCode);
	QByteArray connectionId = m_managerPtr->ApproveConnectionRequest(requestId, "tenantB");

	imtauth::RelationshipProposalInfo proposal;
	proposal.connectionId = connectionId;
	proposal.initiatorTenantId = "tenantA";
	proposal.counterpartyTenantId = "tenantB";
	QByteArray proposalId = m_managerPtr->CreateRelationshipProposal(proposal);

	QVERIFY(m_managerPtr->RejectRelationshipProposal(proposalId, "tenantB"));
	QCOMPARE(m_managerPtr->m_proposals.first().status, imtauth::RPS_REJECTED);
	QCOMPARE(m_managerPtr->m_relationships.size(), 0);
}


void CTenantConnectionRequestTest::testCancelRelationshipProposal_Success()
{
	// Setup connection
	imtauth::TenantConnectionCodeInfo targetCode = m_managerPtr->GetConnectionCode("tenantB");
	QByteArray requestId = m_managerPtr->CreateConnectionRequest(
				"tenantA", targetCode.connectionCode);
	QByteArray connectionId = m_managerPtr->ApproveConnectionRequest(requestId, "tenantB");

	imtauth::RelationshipProposalInfo proposal;
	proposal.connectionId = connectionId;
	proposal.initiatorTenantId = "tenantA";
	proposal.counterpartyTenantId = "tenantB";
	QByteArray proposalId = m_managerPtr->CreateRelationshipProposal(proposal);

	// Initiator cancels their own proposal
	QVERIFY(m_managerPtr->CancelRelationshipProposal(proposalId, "tenantA"));
	QCOMPARE(m_managerPtr->m_proposals.first().status, imtauth::RPS_CANCELED);
}


void CTenantConnectionRequestTest::testApproveRelationshipProposal_WrongTenant_Fails()
{
	// Setup connection
	imtauth::TenantConnectionCodeInfo targetCode = m_managerPtr->GetConnectionCode("tenantB");
	QByteArray requestId = m_managerPtr->CreateConnectionRequest(
				"tenantA", targetCode.connectionCode);
	QByteArray connectionId = m_managerPtr->ApproveConnectionRequest(requestId, "tenantB");

	imtauth::RelationshipProposalInfo proposal;
	proposal.connectionId = connectionId;
	proposal.initiatorTenantId = "tenantA";
	proposal.counterpartyTenantId = "tenantB";
	QByteArray proposalId = m_managerPtr->CreateRelationshipProposal(proposal);

	// Initiator cannot approve their own proposal
	QByteArray relationshipId = m_managerPtr->ApproveRelationshipProposal(proposalId, "tenantA");
	QVERIFY(relationshipId.isEmpty());
}


// --- Relationship tests ---

void CTenantConnectionRequestTest::testGetTenantRelationships_ReturnsActive()
{
	// Setup connection and relationship
	imtauth::TenantConnectionCodeInfo targetCode = m_managerPtr->GetConnectionCode("tenantB");
	QByteArray requestId = m_managerPtr->CreateConnectionRequest(
				"tenantA", targetCode.connectionCode);
	QByteArray connectionId = m_managerPtr->ApproveConnectionRequest(requestId, "tenantB");

	imtauth::RelationshipProposalInfo proposal;
	proposal.connectionId = connectionId;
	proposal.initiatorTenantId = "tenantA";
	proposal.counterpartyTenantId = "tenantB";
	QByteArray proposalId = m_managerPtr->CreateRelationshipProposal(proposal);
	m_managerPtr->ApproveRelationshipProposal(proposalId, "tenantB");

	imtauth::ITenantInfo::TenantRelationships relsA = m_managerPtr->GetTenantRelationships("tenantA");
	imtauth::ITenantInfo::TenantRelationships relsB = m_managerPtr->GetTenantRelationships("tenantB");
	QCOMPARE(relsA.size(), 1);
	QCOMPARE(relsB.size(), 1);
	QCOMPARE(relsA.first().relationshipId, relsB.first().relationshipId);
}


void CTenantConnectionRequestTest::testRemoveTenantRelationship_Archives()
{
	// Setup connection and relationship
	imtauth::TenantConnectionCodeInfo targetCode = m_managerPtr->GetConnectionCode("tenantB");
	QByteArray requestId = m_managerPtr->CreateConnectionRequest(
				"tenantA", targetCode.connectionCode);
	QByteArray connectionId = m_managerPtr->ApproveConnectionRequest(requestId, "tenantB");

	imtauth::RelationshipProposalInfo proposal;
	proposal.connectionId = connectionId;
	proposal.initiatorTenantId = "tenantA";
	proposal.counterpartyTenantId = "tenantB";
	QByteArray proposalId = m_managerPtr->CreateRelationshipProposal(proposal);
	QByteArray relationshipId = m_managerPtr->ApproveRelationshipProposal(proposalId, "tenantB");

	QVERIFY(m_managerPtr->RemoveTenantRelationship("tenantA", relationshipId));
	QCOMPARE(m_managerPtr->GetTenantRelationships("tenantA").size(), 0);
	QCOMPARE(m_managerPtr->m_relationships.first().status, 1); // Archived
}

