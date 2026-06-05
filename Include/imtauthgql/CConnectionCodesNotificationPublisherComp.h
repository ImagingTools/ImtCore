// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// Qt includes
#include <QRecursiveMutex>
#include <QMap>
#include <QList>

// ACF includes
#include <imod/CMultiModelDispatcherBase.h>
#include <imod/IModel.h>

// ImtCore includes
#include <imtauth/ITenantConnectionRequest.h>
#include <imtauth/ITenantRelationshipProposalInfo.h>
#include <imtauth/ITenantManager.h>
#include <imtservergql/CGqlPublisherCompBase.h>
#include <GeneratedFiles/imtauthsdl/SDL/1.0/CPP/Tenants_fwd.h>


namespace imtauthgql
{


/**
	Server-side publisher that broadcasts notifications when connection-related
	events occur (request created, approved, rejected, connection removed, etc.).

	The component observes the connection request manager
	(imtauth::ITenantConnectionRequest) and publishes ConnectionNotification
	events to subscribed clients.

	The component observes:
	- imtauth::ITenantConnectionRequest — connection request lifecycle
	  (new request = ConnectionRequestReceived, approval = ConnectionRequestApproved,
	  rejection = ConnectionRequestRejected)
	- Connection removals (ConnectionRemoved)
	- Relationship proposal lifecycle (RelationshipProposalReceived,
	  RelationshipProposalApproved, RelationshipProposalRejected)

	Notifications are delivered only to subscribers whose authenticated user
	matches the target userId (tenant owner).
*/
class CConnectionCodesNotificationPublisherComp:
			public imtservergql::CGqlPublisherCompBase,
			protected imod::CMultiModelDispatcherBase
{
public:
	typedef imtservergql::CGqlPublisherCompBase BaseClass;
	typedef imod::CMultiModelDispatcherBase BaseClass2;

	I_BEGIN_COMPONENT(CConnectionCodesNotificationPublisherComp);
		I_ASSIGN(m_connectionRequestManagerCompPtr, "ConnectionRequestManager", "Connection request manager to observe", true, "TenantConnectionRequestManager");
		I_ASSIGN_TO(m_connectionRequestManagerModelCompPtr, m_connectionRequestManagerCompPtr, true);
		I_ASSIGN(m_tenantManagerCompPtr, "TenantManager", "Tenant manager for resolving tenant owners", true, "TenantManager");
	I_END_COMPONENT;

protected:
	// reimplemented (imtgql::IGqlSubscriberController)
	virtual bool IsRequestSupported(const imtgql::CGqlRequest& gqlRequest) const override;

	// reimplemented (icomp::CComponentBase)
	virtual void OnComponentCreated() override;
	virtual void OnComponentDestroyed() override;

	// reimplemented (imod::CMultiModelDispatcherBase)
	virtual void OnModelChanged(int modelId, const istd::IChangeable::ChangeSet& changeSet) override;

protected:
	I_REF(imtauth::ITenantConnectionRequest, m_connectionRequestManagerCompPtr);
	I_REF(imod::IModel, m_connectionRequestManagerModelCompPtr);
	I_REF(imtauth::ITenantManager, m_tenantManagerCompPtr);

private:
	struct CachedConnectionRequest
	{
		QByteArray sourceTenantId;
		QByteArray targetTenantId;
		imtauth::ITenantConnectionRequest::ConnectionRequestStatus status;
	};

	struct CachedConnection
	{
		QByteArray tenantAId;
		QByteArray tenantBId;
		imtauth::ITenantConnectionRequest::ConnectionStatus status;
	};

	struct CachedRelationshipProposal
	{
		QByteArray initiatorTenantId;
		QByteArray counterpartyTenantId;
		QByteArray connectionId;
		imtauth::ITenantRelationshipProposalInfo::RelationshipProposalStatus status;
	};

	void PublishNotification(
				const QByteArray& targetUserId,
				sdl::V1_0::imtauth::EConnectionNotificationType notificationType,
				const QByteArray& tenantId,
				const QByteArray& relatedId) const;

	QByteArray FindTenantOwnerUserId(const QByteArray& tenantId) const;

	// Cache of requestId → state for change detection.
	mutable QMap<QByteArray, CachedConnectionRequest> m_cachedRequests;
	// Cache of connectionId → state for connection removal detection.
	mutable QMap<QByteArray, CachedConnection> m_cachedConnections;
	// Cache of proposalId → state for proposal lifecycle detection.
	mutable QMap<QByteArray, CachedRelationshipProposal> m_cachedProposals;
	// Protects all caches from concurrent access.
	mutable QRecursiveMutex m_cacheMutex;
};


} // namespace imtauthgql


