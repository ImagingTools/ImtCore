// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ACF includes
#include <ilog/TLoggerCompWrap.h>

// Qt includes
#include <functional>

// ImtCore includes
#include <imtauth/ITenantConnectionRequestManager.h>
#include <imtauth/ITenantConnectionRequestInfo.h>
#include <imtauth/ITenantConnectionCodeInfo.h>
#include <imtauth/ITenantConnectionInfo.h>
#include <imtauth/ITenantRelationshipProposalInfo.h>
#include <imtauth/ITenantRelationshipInfo.h>
#include <imtauth/ITenantManager.h>
#include <imtbase/IObjectCollection.h>


namespace imtauth
{


class CTenantConnectionRequestManagerComp:
			public ilog::CLoggerComponentBase,
			virtual public imtauth::ITenantConnectionRequestManager
{
public:
	typedef ilog::CLoggerComponentBase BaseClass;
	typedef ITenantRelationshipProposalInfo::RelationshipProposalType RelationshipProposalType;
	typedef ITenantRelationshipProposalInfo::RelationshipProposalStatus RelationshipProposalStatus;
	typedef ITenantConnectionRequestInfo::ConnectionRequestStatus ConnectionRequestStatus;
	typedef ITenantConnectionInfo::ConnectionStatus ConnectionStatus;

	I_BEGIN_COMPONENT(CTenantConnectionRequestManagerComp);
		I_REGISTER_INTERFACE(imtauth::ITenantConnectionRequestManager);
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

	// ITenantConnectionRequestManager - Connection Code
	virtual QString GetConnectionCode(const QByteArray& tenantId) override;
	virtual bool GetAllowConnectionsByCode(const QByteArray& tenantId) override;
	virtual QString RegenerateConnectionCode(const QByteArray& tenantId) override;
	virtual bool SetAllowConnectionsByCode(const QByteArray& tenantId, bool allow) override;

	// ITenantConnectionRequestManager - Connection Requests
	virtual QByteArray CreateConnectionRequest(
				const QByteArray& sourceTenantId,
				const QString& connectionCode,
				const QString& message = QString()) override;
	virtual QByteArray ApproveConnectionRequest(const QByteArray& requestId, const QByteArray& approvingTenantId) override;
	virtual bool RejectConnectionRequest(const QByteArray& requestId, const QByteArray& tenantId) override;
	virtual bool CancelConnectionRequest(const QByteArray& requestId, const QByteArray& tenantId) override;
	virtual QByteArrayList GetConnectionRequestIds(const QByteArray& tenantId) const override;
	virtual ITenantConnectionRequestInfoUniquePtr GetConnectionRequest(const QByteArray& requestId) const override;

	// ITenantConnectionRequestManager - Connections
	virtual QByteArrayList GetConnectionIds(const QByteArray& tenantId) const override;
	virtual ITenantConnectionInfoUniquePtr GetConnection(const QByteArray& connectionId) const override;
	virtual bool RemoveConnection(const QByteArray& connectionId, const QByteArray& tenantId) override;

	// ITenantConnectionRequestManager - Relationship Proposals
	virtual QByteArray CreateRelationshipProposal(const ITenantRelationshipProposalInfo& proposalInfo) override;
	virtual QByteArray ApproveRelationshipProposal(const QByteArray& proposalId, const QByteArray& tenantId) override;
	virtual bool RejectRelationshipProposal(const QByteArray& proposalId, const QByteArray& tenantId) override;
	virtual bool CancelRelationshipProposal(const QByteArray& proposalId, const QByteArray& tenantId) override;
	virtual QByteArrayList GetRelationshipProposalIds(const QByteArray& tenantId) const override;

	// ITenantConnectionRequestManager - Relationships
	virtual QByteArrayList GetTenantRelationshipIds(const QByteArray& tenantId) const override;
	virtual bool RemoveTenantRelationship(const QByteArray& tenantId, const QByteArray& relationshipId) override;

private:
	QString GenerateConnectionCode() const;
	QByteArray FindTenantByConnectionCode(const QString& connectionCode) const;
	bool ConnectionExists(const QByteArray& tenantAId, const QByteArray& tenantBId) const;
	QByteArray CreateConnection(const QByteArray& tenantAId, const QByteArray& tenantBId);
	QByteArray ApplyRelationshipProposal(const ITenantRelationshipProposalInfo* proposalPtr);
	void ArchiveRelationshipsForConnection(const QByteArray& connectionId);

	// DB helper methods
	bool StoreConnectionCode(const QByteArray& tenantId, const ITenantConnectionCodeInfo& codeInfo);
	bool StoreConnectionRequest(const ITenantConnectionRequestInfo& requestInfo);
	bool StoreConnection(const ITenantConnectionInfo& connectionInfo);
	bool StoreProposal(const QByteArray& proposalId, const ITenantRelationshipProposalInfo& proposalInfo);

	// Helper to get code object from collection
	const ITenantConnectionCodeInfo* GetConnectionCodeObject(const QByteArray& tenantId);
	void EnsureConnectionCode(const QByteArray& tenantId);

private:
	I_REF(imtbase::IObjectCollection, m_requestCollectionCompPtr);
	I_REF(imtbase::IObjectCollection, m_connectionCodeCollectionCompPtr);
	I_REF(imtbase::IObjectCollection, m_connectionCollectionCompPtr);
	I_REF(imtbase::IObjectCollection, m_proposalCollectionCompPtr);
	I_REF(imtbase::IObjectCollection, m_relationshipCollectionCompPtr);
	I_FACT(imtauth::ITenantConnectionRequestInfo, m_requestFactoryCompPtr);
	I_FACT(imtauth::ITenantConnectionCodeInfo, m_connectionCodeFactoryCompPtr);
	I_FACT(imtauth::ITenantConnectionInfo, m_connectionFactoryCompPtr);
	I_FACT(imtauth::ITenantRelationshipProposalInfo, m_proposalFactoryCompPtr);
	I_FACT(imtauth::ITenantRelationshipInfo, m_relationshipFactoryCompPtr);
	I_REF(imtauth::ITenantManager, m_tenantManagerCompPtr);
};


} // namespace imtauth
