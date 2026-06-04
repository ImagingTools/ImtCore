// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once

// Qt includes
#include <QtCore/QObject>
#include <QtCore/QDateTime>
#include <QtCore/QList>
#include <QtCore/QUuid>
#include <QtTest/QtTest>

// ImtCore includes
#include <imtauth/ITenantConnectionRequest.h>


namespace imtauth
{


/**
	Self-contained mock replicating the lifecycle semantics of
	CTenantConnectionRequestManagerComp for the new corporate connection model.

	Covers: Connection Codes, Connection Requests, Connections,
	Relationships, and Relationship Proposals.
*/
class CMockConnectionManager
{
public:
	// --- Connection Code ---

	TenantConnectionCodeInfo GetConnectionCode(const QByteArray& tenantId)
	{
		for (const TenantConnectionCodeInfo& code : m_codes){
			if (code.tenantId == tenantId){
				return code;
			}
		}
		TenantConnectionCodeInfo newCode;
		newCode.tenantId = tenantId;
		newCode.connectionCode = GenerateCode();
		newCode.allowConnectionsByCode = true;
		newCode.createdAt = QDateTime::currentDateTimeUtc().toString(Qt::ISODateWithMs);
		m_codes.append(newCode);
		return newCode;
	}

	QString RegenerateConnectionCode(const QByteArray& tenantId)
	{
		for (TenantConnectionCodeInfo& code : m_codes){
			if (code.tenantId == tenantId){
				code.connectionCode = GenerateCode();
				return code.connectionCode;
			}
		}
		TenantConnectionCodeInfo newCode;
		newCode.tenantId = tenantId;
		newCode.connectionCode = GenerateCode();
		newCode.allowConnectionsByCode = true;
		newCode.createdAt = QDateTime::currentDateTimeUtc().toString(Qt::ISODateWithMs);
		m_codes.append(newCode);
		return newCode.connectionCode;
	}

	bool SetAllowConnectionsByCode(const QByteArray& tenantId, bool allow)
	{
		for (TenantConnectionCodeInfo& code : m_codes){
			if (code.tenantId == tenantId){
				code.allowConnectionsByCode = allow;
				return true;
			}
		}
		return false;
	}

	// --- Connection Requests ---

	QByteArray CreateConnectionRequest(
				const QByteArray& sourceTenantId,
				const QString& connectionCode,
				const QString& message = QString())
	{
		if (sourceTenantId.isEmpty() || connectionCode.isEmpty()){
			return QByteArray();
		}

		// Find target tenant by connection code
		QByteArray targetTenantId;
		for (const TenantConnectionCodeInfo& code : m_codes){
			if (code.connectionCode == connectionCode){
				if (!code.allowConnectionsByCode){
					return QByteArray();
				}
				targetTenantId = code.tenantId;
				break;
			}
		}
		if (targetTenantId.isEmpty()){
			return QByteArray();
		}

		// Cannot connect to self
		if (sourceTenantId == targetTenantId){
			return QByteArray();
		}

		// Check for existing connection
		for (const TenantConnectionInfo& conn : m_connections){
			if (conn.status == CS_ACTIVE){
				if ((conn.tenantAId == sourceTenantId && conn.tenantBId == targetTenantId)
					|| (conn.tenantAId == targetTenantId && conn.tenantBId == sourceTenantId)){
					return QByteArray();
				}
			}
		}

		ConnectionRequestInfo info;
		info.requestId = QByteArray::number(++m_counter);
		info.sourceTenantId = sourceTenantId;
		info.targetTenantId = targetTenantId;
		info.connectionCode = connectionCode;
		info.message = message;
		info.status = CRS_PENDING;
		info.createdAt = QDateTime::currentDateTimeUtc().toString(Qt::ISODateWithMs);

		m_requests.append(info);
		return info.requestId;
	}

	QByteArray ApproveConnectionRequest(const QByteArray& requestId, const QByteArray& approvingTenantId)
	{
		for (ConnectionRequestInfo& info : m_requests){
			if (info.requestId != requestId){
				continue;
			}
			if (info.status != CRS_PENDING){
				return QByteArray();
			}
			if (info.targetTenantId != approvingTenantId){
				return QByteArray();
			}

			info.status = CRS_APPROVED;
			info.respondedAt = QDateTime::currentDateTimeUtc().toString(Qt::ISODateWithMs);

			// Create connection (canonical order)
			TenantConnectionInfo conn;
			conn.connectionId = QByteArray::number(++m_counter);
			if (info.sourceTenantId < info.targetTenantId){
				conn.tenantAId = info.sourceTenantId;
				conn.tenantBId = info.targetTenantId;
			} else {
				conn.tenantAId = info.targetTenantId;
				conn.tenantBId = info.sourceTenantId;
			}
			conn.status = CS_ACTIVE;
			conn.createdAt = QDateTime::currentDateTimeUtc().toString(Qt::ISODateWithMs);
			m_connections.append(conn);
			return conn.connectionId;
		}
		return QByteArray();
	}

	bool RejectConnectionRequest(const QByteArray& requestId, const QByteArray& tenantId)
	{
		for (ConnectionRequestInfo& info : m_requests){
			if (info.requestId == requestId && info.targetTenantId == tenantId && info.status == CRS_PENDING){
				info.status = CRS_REJECTED;
				info.respondedAt = QDateTime::currentDateTimeUtc().toString(Qt::ISODateWithMs);
				return true;
			}
		}
		return false;
	}

	bool CancelConnectionRequest(const QByteArray& requestId, const QByteArray& tenantId)
	{
		for (ConnectionRequestInfo& info : m_requests){
			if (info.requestId == requestId && info.sourceTenantId == tenantId && info.status == CRS_PENDING){
				info.status = CRS_CANCELED;
				info.respondedAt = QDateTime::currentDateTimeUtc().toString(Qt::ISODateWithMs);
				return true;
			}
		}
		return false;
	}

	ConnectionRequests GetConnectionRequests(const QByteArray& tenantId) const
	{
		ConnectionRequests result;
		for (const ConnectionRequestInfo& info : m_requests){
			if (info.sourceTenantId == tenantId || info.targetTenantId == tenantId){
				result.append(info);
			}
		}
		return result;
	}

	// --- Connections ---

	TenantConnections GetConnections(const QByteArray& tenantId) const
	{
		TenantConnections result;
		for (const TenantConnectionInfo& conn : m_connections){
			if (conn.status == CS_ACTIVE && (conn.tenantAId == tenantId || conn.tenantBId == tenantId)){
				result.append(conn);
			}
		}
		return result;
	}

	bool RemoveConnection(const QByteArray& connectionId, const QByteArray& /*tenantId*/)
	{
		for (TenantConnectionInfo& conn : m_connections){
			if (conn.connectionId == connectionId && conn.status == CS_ACTIVE){
				conn.status = CS_REMOVED;
				conn.updatedAt = QDateTime::currentDateTimeUtc().toString(Qt::ISODateWithMs);
				// Archive related relationships
				for (ITenantInfo::TenantRelationship& rel : m_relationships){
					if (rel.connectionId == connectionId && rel.status == 0){
						rel.status = 1; // Archived
						rel.updatedAt = QDateTime::currentDateTimeUtc().toString(Qt::ISODateWithMs);
					}
				}
				return true;
			}
		}
		return false;
	}

	// --- Relationship Proposals ---

	QByteArray CreateRelationshipProposal(const RelationshipProposalInfo& proposal)
	{
		if (proposal.connectionId.isEmpty() || proposal.initiatorTenantId.isEmpty()){
			return QByteArray();
		}
		// Verify connection exists and is active
		bool found = false;
		for (const TenantConnectionInfo& conn : m_connections){
			if (conn.connectionId == proposal.connectionId && conn.status == CS_ACTIVE){
				found = true;
				break;
			}
		}
		if (!found){
			return QByteArray();
		}

		RelationshipProposalInfo stored = proposal;
		stored.proposalId = QByteArray::number(++m_counter);
		stored.status = RPS_APPROVED_BY_INITIATOR;
		stored.createdAt = QDateTime::currentDateTimeUtc().toString(Qt::ISODateWithMs);
		m_proposals.append(stored);
		return stored.proposalId;
	}

	QByteArray ApproveRelationshipProposal(const QByteArray& proposalId, const QByteArray& tenantId)
	{
		for (RelationshipProposalInfo& p : m_proposals){
			if (p.proposalId != proposalId){
				continue;
			}
			if (p.counterpartyTenantId != tenantId){
				return QByteArray();
			}
			if (p.status != RPS_APPROVED_BY_INITIATOR){
				return QByteArray();
			}
			p.status = RPS_APPLIED;
			p.updatedAt = QDateTime::currentDateTimeUtc().toString(Qt::ISODateWithMs);

			// Create relationship
			ITenantInfo::TenantRelationship rel;
			rel.relationshipId = QByteArray::number(++m_counter);
			rel.connectionId = p.connectionId;
			rel.sourceTenantId = p.initiatorTenantId;
			rel.targetTenantId = p.counterpartyTenantId;
			rel.sourceRole = p.proposedSourceRole;
			rel.targetRole = p.proposedTargetRole;
			rel.scope = p.proposedScope;
			rel.description = p.proposedDescription;
			rel.validFrom = p.proposedValidFrom;
			rel.validUntil = p.proposedValidUntil;
			rel.status = 0; // Active
			rel.createdAt = QDateTime::currentDateTimeUtc().toString(Qt::ISODateWithMs);
			m_relationships.append(rel);
			return rel.relationshipId;
		}
		return QByteArray();
	}

	bool RejectRelationshipProposal(const QByteArray& proposalId, const QByteArray& tenantId)
	{
		for (RelationshipProposalInfo& p : m_proposals){
			if (p.proposalId == proposalId && p.counterpartyTenantId == tenantId
				&& p.status == RPS_APPROVED_BY_INITIATOR){
				p.status = RPS_REJECTED;
				p.updatedAt = QDateTime::currentDateTimeUtc().toString(Qt::ISODateWithMs);
				return true;
			}
		}
		return false;
	}

	bool CancelRelationshipProposal(const QByteArray& proposalId, const QByteArray& tenantId)
	{
		for (RelationshipProposalInfo& p : m_proposals){
			if (p.proposalId == proposalId && p.initiatorTenantId == tenantId
				&& (p.status == RPS_PENDING || p.status == RPS_APPROVED_BY_INITIATOR)){
				p.status = RPS_CANCELED;
				p.updatedAt = QDateTime::currentDateTimeUtc().toString(Qt::ISODateWithMs);
				return true;
			}
		}
		return false;
	}

	RelationshipProposals GetRelationshipProposals(const QByteArray& tenantId) const
	{
		RelationshipProposals result;
		for (const RelationshipProposalInfo& p : m_proposals){
			if (p.initiatorTenantId == tenantId || p.counterpartyTenantId == tenantId){
				result.append(p);
			}
		}
		return result;
	}

	// --- Relationships ---

	ITenantInfo::TenantRelationships GetTenantRelationships(const QByteArray& tenantId) const
	{
		ITenantInfo::TenantRelationships result;
		for (const ITenantInfo::TenantRelationship& rel : m_relationships){
			if (rel.status == 0 && (rel.sourceTenantId == tenantId || rel.targetTenantId == tenantId)){
				result.append(rel);
			}
		}
		return result;
	}

	bool RemoveTenantRelationship(const QByteArray& /*tenantId*/, const QByteArray& relationshipId)
	{
		for (ITenantInfo::TenantRelationship& rel : m_relationships){
			if (rel.relationshipId == relationshipId && rel.status == 0){
				rel.status = 1; // Archived
				rel.updatedAt = QDateTime::currentDateTimeUtc().toString(Qt::ISODateWithMs);
				return true;
			}
		}
		return false;
	}

	// --- Internal data ---
	QList<TenantConnectionCodeInfo> m_codes;
	QList<ConnectionRequestInfo> m_requests;
	QList<TenantConnectionInfo> m_connections;
	QList<RelationshipProposalInfo> m_proposals;
	QList<ITenantInfo::TenantRelationship> m_relationships;
	int m_counter = 0;

private:
	QString GenerateCode() const
	{
		return QUuid::createUuid().toString(QUuid::WithoutBraces).left(8).toUpper();
	}
};


} // namespace imtauth


class CTenantConnectionRequestTest: public QObject
{
	Q_OBJECT

private Q_SLOTS:
	void init();
	void cleanup();

	// Data object tests
	void testRequestInfo_RoundTrip();
	void testRequestInfo_CopyFromAndClone();
	void testRequestInfo_ResetDataDefaults();

	// Connection Code
	void testGetConnectionCode_AutoCreates();
	void testRegenerateConnectionCode_ChangesCode();
	void testSetAllowConnectionsByCode();

	// Connection Requests
	void testCreateConnectionRequest_Success();
	void testCreateConnectionRequest_InvalidCode_Fails();
	void testCreateConnectionRequest_SelfConnect_Fails();
	void testCreateConnectionRequest_DisabledByCode_Fails();
	void testCreateConnectionRequest_DuplicateConnection_Fails();
	void testApproveConnectionRequest_CreatesConnection();
	void testRejectConnectionRequest_Success();
	void testCancelConnectionRequest_Success();
	void testApproveConnectionRequest_NonPending_Fails();
	void testApproveConnectionRequest_WrongTenant_Fails();

	// Connections
	void testGetConnections_ReturnsActive();
	void testRemoveConnection_CascadesRelationships();

	// Relationship Proposals
	void testCreateRelationshipProposal_Success();
	void testCreateRelationshipProposal_NoConnection_Fails();
	void testApproveRelationshipProposal_CreatesRelationship();
	void testRejectRelationshipProposal_Success();
	void testCancelRelationshipProposal_Success();
	void testApproveRelationshipProposal_WrongTenant_Fails();

	// Relationships
	void testGetTenantRelationships_ReturnsActive();
	void testRemoveTenantRelationship_Archives();

private:
	imtauth::CMockConnectionManager* m_managerPtr = nullptr;
};

