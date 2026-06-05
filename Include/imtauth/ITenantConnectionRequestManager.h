// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// Qt includes
#include <QtCore/QByteArray>
#include <QtCore/QByteArrayList>
#include <QtCore/QString>

// ACF includes
#include <istd/IChangeable.h>

// ImtCore includes
#include <imtauth/ITenantRelationshipProposalInfo.h>


namespace imtauth
{


/**
Interface for managing the corporate connection and relationship workflow.

Covers: Connection Codes, Connection Requests, Connections, Relationships,
and Relationship Proposals.

\ingroup Tenant
*/
class ITenantConnectionRequestManager: virtual public istd::IChangeable
{
public:
	// --- Connection Code ---
	virtual QString GetConnectionCode(const QByteArray& tenantId) = 0;
	virtual bool GetAllowConnectionsByCode(const QByteArray& tenantId) = 0;
	virtual QString RegenerateConnectionCode(const QByteArray& tenantId) = 0;
	virtual bool SetAllowConnectionsByCode(const QByteArray& tenantId, bool allow) = 0;

	// --- Connection Requests ---
	virtual QByteArray CreateConnectionRequest(const QByteArray& sourceTenantId, const QString& connectionCode, const QString& message = QString()) = 0;
	virtual QByteArray ApproveConnectionRequest(const QByteArray& requestId, const QByteArray& approvingTenantId) = 0;
	virtual bool RejectConnectionRequest(const QByteArray& requestId, const QByteArray& tenantId) = 0;
	virtual bool CancelConnectionRequest(const QByteArray& requestId, const QByteArray& tenantId) = 0;
	virtual QByteArrayList GetConnectionRequestIds(const QByteArray& tenantId) const = 0;

	// --- Connections ---
	virtual QByteArrayList GetConnectionIds(const QByteArray& tenantId) const = 0;
	virtual bool RemoveConnection(const QByteArray& connectionId, const QByteArray& tenantId) = 0;

	// --- Relationship Proposals ---
	virtual QByteArray CreateRelationshipProposal(const ITenantRelationshipProposalInfo& proposalInfo) = 0;
	virtual QByteArray ApproveRelationshipProposal(const QByteArray& proposalId, const QByteArray& tenantId) = 0;
	virtual bool RejectRelationshipProposal(const QByteArray& proposalId, const QByteArray& tenantId) = 0;
	virtual bool CancelRelationshipProposal(const QByteArray& proposalId, const QByteArray& tenantId) = 0;
	virtual QByteArrayList GetRelationshipProposalIds(const QByteArray& tenantId) const = 0;

	// --- Relationships ---
	virtual QByteArrayList GetTenantRelationshipIds(const QByteArray& tenantId) const = 0;
	virtual bool RemoveTenantRelationship(const QByteArray& tenantId, const QByteArray& relationshipId) = 0;
};


} // namespace imtauth
