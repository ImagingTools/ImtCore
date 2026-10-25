// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// Qt includes
#include <QList>

// ACF includes
#include <imod/CMultiModelDispatcherBase.h>
#include <imod/IModel.h>

// ImtCore includes
#include <imtauth/ITenantConnectionRequestManager.h>
#include <imtauth/ITenantConnectionRequestInfo.h>
#include <imtauth/ITenantConnectionInfo.h>
#include <imtauth/ITenantRelationshipProposalInfo.h>
#include <imtauth/ITenantRelationshipInfo.h>
#include <imtauth/ITenantManager.h>
#include <imtservergql/CGqlPublisherCompBase.h>
#include <GeneratedFiles/imtauthsdl/SDL/1.0/CPP/Tenants_fwd.h>


namespace imtauthgql
{


/**
	Server-side publisher that broadcasts notifications when connection-related
	events occur (request created, approved, rejected, connection removed, etc.).

	The component observes the connection request manager
	(imtauth::ITenantConnectionRequestManager) and publishes ConnectionNotification
	events to subscribed clients.

	The component observes:
	- imtauth::ITenantConnectionRequestManager — connection request lifecycle
	  (new request = ConnectionRequestReceived, approval = ConnectionRequestApproved,
	  rejection = ConnectionRequestRejected)
	- Connection removals (ConnectionRemoved)
	- Relationship proposal lifecycle: the counterparty owner receives a new
	  proposal (RelationshipProposalReceived), the initiator owner learns about
	  the answer (RelationshipProposalApproved, RelationshipProposalRejected)

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
	I_REF(imtauth::ITenantConnectionRequestManager, m_connectionRequestManagerCompPtr);
	I_REF(imod::IModel, m_connectionRequestManagerModelCompPtr);
	I_REF(imtauth::ITenantManager, m_tenantManagerCompPtr);

private:
	void PublishNotification(
				const QByteArray& targetUserId,
				sdl::V1_0::imtauth::EConnectionNotificationType notificationType,
				const QByteArray& tenantId,
				const QByteArray& relatedId) const;

	QByteArray FindTenantOwnerUserId(const QByteArray& tenantId) const;
};


} // namespace imtauthgql

