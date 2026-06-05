// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// Qt includes
#include <QtCore/QByteArray>
#include <QtCore/QString>
#include <QtCore/QList>

// ACF includes
#include <istd/IChangeable.h>


namespace imtauth
{


/**
Interface for managing the corporate connection and relationship workflow.

Covers: Connection Codes, Connection Requests, Connections, Relationships,
and Relationship Proposals.

\ingroup Tenant
*/
class ITenantConnectionRequest: virtual public istd::IChangeable
{
public:
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

	I_DECLARE_ENUM(ConnectionRequestStatus, CRS_PENDING, CRS_APPROVED, CRS_REJECTED, CRS_CANCELED);

	/**
		Status of a confirmed connection between two tenants.
	*/
	enum ConnectionStatus
	{
		CS_ACTIVE = 0,
		CS_REMOVED,
		CS_SUSPENDED
	};

	I_DECLARE_ENUM(ConnectionStatus, CS_ACTIVE, CS_REMOVED, CS_SUSPENDED);

	/**
		Connection code info for a tenant.
	*/
	struct TenantConnectionCodeInfo
	{
		QByteArray tenantId;
		QString connectionCode;
		bool allowConnectionsByCode = true;
		QString createdAt;

		bool operator==(const TenantConnectionCodeInfo& other) const
		{
			return tenantId == other.tenantId
					&& connectionCode == other.connectionCode
					&& allowConnectionsByCode == other.allowConnectionsByCode
					&& createdAt == other.createdAt;
		}
		bool operator!=(const TenantConnectionCodeInfo& other) const
		{
			return !(*this == other);
		}
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

	virtual TenantConnectionCodeInfo GetConnectionCode(const QByteArray& tenantId) = 0;
	virtual QString RegenerateConnectionCode(const QByteArray& tenantId) = 0;
	virtual bool SetAllowConnectionsByCode(const QByteArray& tenantId, bool allow) = 0;
	virtual QByteArray CreateConnectionRequest(const QByteArray& sourceTenantId, const QString& connectionCode, const QString& message = QString()) = 0;
	virtual QByteArray ApproveConnectionRequest(const QByteArray& requestId, const QByteArray& approvingTenantId) = 0;
	virtual bool RejectConnectionRequest(const QByteArray& requestId, const QByteArray& tenantId) = 0;
	virtual bool CancelConnectionRequest(const QByteArray& requestId, const QByteArray& tenantId) = 0;
	virtual ConnectionRequests GetConnectionRequests(const QByteArray& tenantId) const = 0;
	virtual TenantConnections GetConnections(const QByteArray& tenantId) const = 0;
	virtual bool RemoveConnection(const QByteArray& connectionId, const QByteArray& tenantId) = 0;
	virtual QByteArray CreateRelationshipProposal(const QByteArray& proposalId, const QByteArray& connectionId, const QByteArray& initiatorTenantId, const QByteArray& counterpartyTenantId) = 0;
	virtual QByteArray ApproveRelationshipProposal(const QByteArray& proposalId, const QByteArray& tenantId) = 0;
	virtual bool RejectRelationshipProposal(const QByteArray& proposalId, const QByteArray& tenantId) = 0;
	virtual bool CancelRelationshipProposal(const QByteArray& proposalId, const QByteArray& tenantId) = 0;
	virtual QByteArrayList GetRelationshipProposalIds(const QByteArray& tenantId) const = 0;
	virtual QByteArrayList GetTenantRelationshipIds(const QByteArray& tenantId) const = 0;
	virtual bool RemoveTenantRelationship(const QByteArray& tenantId, const QByteArray& relationshipId) = 0;
};


} // namespace imtauth

