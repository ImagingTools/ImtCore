// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// Qt includes
#include <QtCore/QByteArray>
#include <QtCore/QByteArrayList>
#include <QtCore/QDateTime>
#include <QtCore/QString>

// ACF includes
#include <istd/IChangeable.h>

// ImtCore includes
#include <imtauth/ITenantRelationshipProposalInfo.h>
#include <imtauth/ITenantConnectionRequestInfo.h>
#include <imtauth/ITenantConnectionInfo.h>
#include <imtauth/ITenantRelationshipInfo.h>


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
	virtual QString GetConnectionCode(const QByteArray& tenantId) = 0;
	virtual bool GetAllowConnectionsByCode(const QByteArray& tenantId) = 0;
	virtual QString RegenerateConnectionCode(const QByteArray& tenantId) = 0;
	virtual bool SetAllowConnectionsByCode(const QByteArray& tenantId, bool allow) = 0;
	/**
		Create a connection request to the organization with the given connection code.
		\param errorMessagePtr Receives the reason when the request is refused.
		\param proposedRelationshipPtr [optional] Terms of the relationship the request proposes along: roles (the source role is
			the one of the requesting organization) and scope. The approval of the request then brings the relationship into force too.
	*/
	virtual QByteArray CreateConnectionRequest(
				const QByteArray& sourceTenantId,
				const QString& connectionCode,
				const QString& message = QString(),
				QString* errorMessagePtr = nullptr,
				const ITenantRelationshipProposalInfo* proposedRelationshipPtr = nullptr) = 0;
	/**
		Approve a connection request: the organizations get connected, and the relationship the request proposes comes into force.
		A request whose relationship cannot come into force stays pending, and a connection made for it is removed again.
		\param relationshipIdPtr [optional] Receives the relationship of a request that proposes one.
		\return The connection, empty when the request is not approved.
	*/
	virtual QByteArray ApproveConnectionRequest(const QByteArray& requestId, const QByteArray& approvingTenantId, QByteArray* relationshipIdPtr = nullptr) = 0;
	virtual bool RejectConnectionRequest(const QByteArray& requestId, const QByteArray& tenantId) = 0;
	virtual bool CancelConnectionRequest(const QByteArray& requestId, const QByteArray& tenantId) = 0;
	virtual QByteArrayList GetConnectionRequestIds(const QByteArray& tenantId) const = 0;
	virtual ITenantConnectionRequestInfoUniquePtr GetConnectionRequest(const QByteArray& requestId) const = 0;
	virtual QByteArrayList GetConnectionIds(const QByteArray& tenantId) const = 0;
	virtual ITenantConnectionInfoUniquePtr GetConnection(const QByteArray& connectionId) const = 0;
	virtual bool RemoveConnection(const QByteArray& connectionId, const QByteArray& tenantId) = 0;
	virtual QByteArray CreateRelationshipProposal(const ITenantRelationshipProposalInfo& proposalInfo) = 0;
	virtual QByteArray ApproveRelationshipProposal(const QByteArray& proposalId, const QByteArray& tenantId) = 0;
	virtual bool RejectRelationshipProposal(const QByteArray& proposalId, const QByteArray& tenantId) = 0;
	virtual bool CancelRelationshipProposal(const QByteArray& proposalId, const QByteArray& tenantId) = 0;
	virtual QByteArrayList GetRelationshipProposalIds(const QByteArray& tenantId) const = 0;
	virtual ITenantRelationshipProposalInfoUniquePtr GetRelationshipProposal(const QByteArray& proposalId) const = 0;
	/// \param isReadPtr [optional] False when the storage could not be read completely, so the list may miss relationships.
	virtual QByteArrayList GetTenantRelationshipIds(const QByteArray& tenantId, bool* isReadPtr = nullptr) const = 0;
	virtual ITenantRelationshipInfoUniquePtr GetTenantRelationship(const QByteArray& relationshipId) const = 0;
	virtual bool RemoveTenantRelationship(const QByteArray& tenantId, const QByteArray& relationshipId) = 0;
	/**
		Start of the uninterrupted period in which the two tenants had an active relationship, archived relationships included.
		Invalid when they have no active relationship now, or a start time is unknown.
		\param isReadPtr [optional] False when the history could not be read completely, so the start may be too late.
	*/
	virtual QDateTime GetRelatedSince(const QByteArray& tenantId, const QByteArray& otherTenantId, bool* isReadPtr = nullptr) const = 0;
};


} // namespace imtauth
