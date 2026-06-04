// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ACF includes
#include <istd/IChangeable.h>

// Qt includes
#include <QByteArray>
#include <QString>
#include <QList>

// ImtCore includes
#include <imtauth/ITenantInfo.h>


namespace imtauth
{


/**
Status of a connection request.
*/
enum ConnectionRequestStatus
{
CRS_PENDING = 0,
CRS_APPROVED,
CRS_REJECTED,
CRS_CANCELED
};


/**
Status of a confirmed connection between two tenants.
*/
enum ConnectionStatus
{
CS_ACTIVE = 0,
CS_REMOVED,
CS_SUSPENDED
};


/**
Status of a relationship.
*/
enum RelationshipStatus
{
RS_ACTIVE = 0,
RS_ARCHIVED,
RS_PENDING_APPROVAL
};


/**
Type of relationship proposal.
*/
enum RelationshipProposalType
{
RPT_CREATE = 0,
RPT_UPDATE,
RPT_DELETE
};


/**
Status of a relationship proposal.
*/
enum RelationshipProposalStatus
{
RPS_PENDING = 0,
RPS_APPROVED_BY_INITIATOR,
RPS_APPROVED_BY_COUNTERPARTY,
RPS_REJECTED,
RPS_CANCELED,
RPS_EXPIRED,
RPS_APPLIED
};


/**
Connection code info for a tenant.
*/
struct TenantConnectionCodeInfo
{
QByteArray tenantId;
QString connectionCode;
bool allowConnectionsByCode = true;
QString createdAt;
};


/**
Structure describing a connection request between two tenants.
*/
struct ConnectionRequestInfo
{
QByteArray requestId;
QByteArray sourceTenantId;
QByteArray targetTenantId;
QString connectionCode;
QString message;
ConnectionRequestStatus status = CRS_PENDING;
QString createdAt;
QString respondedAt;
QString sourceTenantName;
QString targetTenantName;

bool operator==(const ConnectionRequestInfo& other) const
{
return requestId == other.requestId
&& sourceTenantId == other.sourceTenantId
&& targetTenantId == other.targetTenantId
&& connectionCode == other.connectionCode
&& message == other.message
&& status == other.status
&& createdAt == other.createdAt
&& respondedAt == other.respondedAt;
}

bool operator!=(const ConnectionRequestInfo& other) const
{
return !(*this == other);
}
};

typedef QList<ConnectionRequestInfo> ConnectionRequests;


/**
Structure describing a confirmed connection between two tenants.
*/
struct TenantConnectionInfo
{
QByteArray connectionId;
QByteArray tenantAId;
QByteArray tenantBId;
ConnectionStatus status = CS_ACTIVE;
QString createdAt;
QString updatedAt;

bool operator==(const TenantConnectionInfo& other) const
{
return connectionId == other.connectionId
&& tenantAId == other.tenantAId
&& tenantBId == other.tenantBId
&& status == other.status;
}

bool operator!=(const TenantConnectionInfo& other) const
{
return !(*this == other);
}
};

typedef QList<TenantConnectionInfo> TenantConnections;


/**
Structure describing a relationship proposal.
*/
struct RelationshipProposalInfo
{
QByteArray proposalId;
QByteArray connectionId;
QByteArray existingRelationshipId;
RelationshipProposalType proposalType = RPT_CREATE;
QByteArray initiatorTenantId;
QByteArray counterpartyTenantId;
ITenantInfo::TenantRelationshipRole proposedSourceRole = ITenantInfo::Partner;
ITenantInfo::TenantRelationshipRole proposedTargetRole = ITenantInfo::Partner;
QString proposedScope;
QString proposedDescription;
QString proposedValidFrom;
QString proposedValidUntil;
RelationshipProposalStatus status = RPS_PENDING;
QString message;
QString createdAt;
QString updatedAt;

bool operator==(const RelationshipProposalInfo& other) const
{
return proposalId == other.proposalId
&& connectionId == other.connectionId
&& proposalType == other.proposalType
&& status == other.status;
}

bool operator!=(const RelationshipProposalInfo& other) const
{
return !(*this == other);
}
};

typedef QList<RelationshipProposalInfo> RelationshipProposals;


/**
Interface for managing the corporate connection and relationship workflow.

Covers: Connection Codes, Connection Requests, Connections, Relationships,
and Relationship Proposals.

\ingroup Tenant
*/
class ITenantConnectionRequest: virtual public istd::IChangeable
{
public:
// --- Connection Code ---

virtual TenantConnectionCodeInfo GetConnectionCode(const QByteArray& tenantId) = 0;
virtual QString RegenerateConnectionCode(const QByteArray& tenantId) = 0;
virtual bool SetAllowConnectionsByCode(const QByteArray& tenantId, bool allow) = 0;

// --- Connection Requests ---

virtual QByteArray CreateConnectionRequest(
const QByteArray& sourceTenantId,
const QString& connectionCode,
const QString& message = QString()) = 0;

virtual QByteArray ApproveConnectionRequest(const QByteArray& requestId, const QByteArray& approvingTenantId) = 0;
virtual bool RejectConnectionRequest(const QByteArray& requestId, const QByteArray& tenantId) = 0;
virtual bool CancelConnectionRequest(const QByteArray& requestId, const QByteArray& tenantId) = 0;
virtual ConnectionRequests GetConnectionRequests(const QByteArray& tenantId) const = 0;

// --- Connections ---

virtual TenantConnections GetConnections(const QByteArray& tenantId) const = 0;
virtual bool RemoveConnection(const QByteArray& connectionId, const QByteArray& tenantId) = 0;

// --- Relationship Proposals ---

virtual QByteArray CreateRelationshipProposal(const RelationshipProposalInfo& proposal) = 0;
virtual QByteArray ApproveRelationshipProposal(const QByteArray& proposalId, const QByteArray& tenantId) = 0;
virtual bool RejectRelationshipProposal(const QByteArray& proposalId, const QByteArray& tenantId) = 0;
virtual bool CancelRelationshipProposal(const QByteArray& proposalId, const QByteArray& tenantId) = 0;
virtual RelationshipProposals GetRelationshipProposals(const QByteArray& tenantId) const = 0;

// --- Relationships ---

virtual ITenantInfo::TenantRelationships GetTenantRelationships(const QByteArray& tenantId) const = 0;
virtual bool RemoveTenantRelationship(const QByteArray& tenantId, const QByteArray& relationshipId) = 0;
};


} // namespace imtauth
