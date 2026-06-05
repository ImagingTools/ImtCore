// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ACF includes
#include <ilog/TLoggerCompWrap.h>

// Qt includes
#include <functional>

// ImtCore includes
#include <imtauth/ITenantConnectionRequest.h>
#include <imtauth/ITenantConnectionRequestData.h>
#include <imtauth/ITenantConnectionCodeData.h>
#include <imtauth/ITenantConnectionData.h>
#include <imtauth/ITenantRelationshipProposalData.h>
#include <imtauth/ITenantRelationshipInfo.h>
#include <imtauth/ITenantManager.h>
#include <imtbase/IObjectCollection.h>


namespace imtauth
{


class CTenantConnectionRequestManagerComp:
			public ilog::CLoggerComponentBase,
			virtual public imtauth::ITenantConnectionRequest
{
public:
	typedef ilog::CLoggerComponentBase BaseClass;

	I_BEGIN_COMPONENT(CTenantConnectionRequestManagerComp);
		I_REGISTER_INTERFACE(imtauth::ITenantConnectionRequest);
		I_ASSIGN(m_requestCollectionCompPtr, "RequestCollection", "Connection request collection", true, "RequestCollection");
		I_ASSIGN(m_connectionCodeCollectionCompPtr, "ConnectionCodeCollection", "Connection code collection", true, "ConnectionCodeCollection");
		I_ASSIGN(m_connectionCollectionCompPtr, "ConnectionCollection", "Connections collection", true, "ConnectionCollection");
		I_ASSIGN(m_proposalCollectionCompPtr, "ProposalCollection", "Relationship proposals collection", true, "ProposalCollection");
		I_ASSIGN(m_relationshipCollectionCompPtr, "RelationshipCollection", "Relationships collection", true, "RelationshipCollection");
		I_ASSIGN(m_requestFactoryCompPtr, "RequestFactory", "Connection request factory", false, "TenantConnectionRequestInfo");
		I_ASSIGN(m_connectionCodeFactoryCompPtr, "ConnectionCodeFactory", "Connection code factory", false, "TenantConnectionCodeInfo");
		I_ASSIGN(m_connectionFactoryCompPtr, "ConnectionFactory", "Connection factory", false, "TenantConnectionInfo");
		I_ASSIGN(m_proposalFactoryCompPtr, "ProposalFactory", "Relationship proposal factory", false, "TenantRelationshipProposalInfo");
		I_ASSIGN(m_relationshipFactoryCompPtr, "RelationshipFactory", "Relationship factory", false, "TenantRelationshipInfo");
		I_ASSIGN(m_tenantManagerCompPtr, "TenantManager", "Tenant manager", false, "TenantManager");
	I_END_COMPONENT;

	// ITenantConnectionRequest - Connection Code
	virtual TenantConnectionCodeInfo GetConnectionCode(const QByteArray& tenantId) override;
	virtual QString RegenerateConnectionCode(const QByteArray& tenantId) override;
	virtual bool SetAllowConnectionsByCode(const QByteArray& tenantId, bool allow) override;

	// ITenantConnectionRequest - Connection Requests
	virtual QByteArray CreateConnectionRequest(
				const QByteArray& sourceTenantId,
				const QString& connectionCode,
				const QString& message = QString()) override;
	virtual QByteArray ApproveConnectionRequest(const QByteArray& requestId, const QByteArray& approvingTenantId) override;
	virtual bool RejectConnectionRequest(const QByteArray& requestId, const QByteArray& tenantId) override;
	virtual bool CancelConnectionRequest(const QByteArray& requestId, const QByteArray& tenantId) override;
	virtual ConnectionRequests GetConnectionRequests(const QByteArray& tenantId) const override;

	// ITenantConnectionRequest - Connections
	virtual TenantConnections GetConnections(const QByteArray& tenantId) const override;
	virtual bool RemoveConnection(const QByteArray& connectionId, const QByteArray& tenantId) override;

	// ITenantConnectionRequest - Relationship Proposals
	virtual QByteArray CreateRelationshipProposal(const RelationshipProposalInfo& proposal) override;
	virtual QByteArray ApproveRelationshipProposal(const QByteArray& proposalId, const QByteArray& tenantId) override;
	virtual bool RejectRelationshipProposal(const QByteArray& proposalId, const QByteArray& tenantId) override;
	virtual bool CancelRelationshipProposal(const QByteArray& proposalId, const QByteArray& tenantId) override;
	virtual RelationshipProposals GetRelationshipProposals(const QByteArray& tenantId) const override;

	// ITenantConnectionRequest - Relationships
	virtual ITenantInfo::TenantRelationships GetTenantRelationships(const QByteArray& tenantId) const override;
	virtual bool RemoveTenantRelationship(const QByteArray& tenantId, const QByteArray& relationshipId) override;

private:
	QString GenerateConnectionCode() const;
	QByteArray FindTenantByConnectionCode(const QString& connectionCode) const;
	bool ConnectionExists(const QByteArray& tenantAId, const QByteArray& tenantBId) const;
	QByteArray CreateConnection(const QByteArray& tenantAId, const QByteArray& tenantBId);
	bool ApplyRelationshipProposal(const RelationshipProposalInfo& proposal);
	void ArchiveRelationshipsForConnection(const QByteArray& connectionId);

	// DB helper methods
	bool StoreConnectionCode(const QByteArray& tenantId, const TenantConnectionCodeInfo& info);
	bool StoreConnectionRequest(const QByteArray& requestId, const ConnectionRequestInfo& info);
	bool StoreConnection(const QByteArray& connectionId, const TenantConnectionInfo& info);
	bool StoreProposal(const QByteArray& proposalId, const RelationshipProposalInfo& info);
	bool StoreRelationship(const QByteArray& relationshipId, const ITenantInfo::TenantRelationship& rel);

private:
	I_REF(imtbase::IObjectCollection, m_requestCollectionCompPtr);
	I_REF(imtbase::IObjectCollection, m_connectionCodeCollectionCompPtr);
	I_REF(imtbase::IObjectCollection, m_connectionCollectionCompPtr);
	I_REF(imtbase::IObjectCollection, m_proposalCollectionCompPtr);
	I_REF(imtbase::IObjectCollection, m_relationshipCollectionCompPtr);
	I_FACT(imtauth::ITenantConnectionRequestData, m_requestFactoryCompPtr);
	I_FACT(imtauth::ITenantConnectionCodeData, m_connectionCodeFactoryCompPtr);
	I_FACT(imtauth::ITenantConnectionData, m_connectionFactoryCompPtr);
	I_FACT(imtauth::ITenantRelationshipProposalData, m_proposalFactoryCompPtr);
	I_FACT(imtauth::ITenantRelationshipInfo, m_relationshipFactoryCompPtr);
	I_REF(imtauth::ITenantManager, m_tenantManagerCompPtr);
};


} // namespace imtauth
