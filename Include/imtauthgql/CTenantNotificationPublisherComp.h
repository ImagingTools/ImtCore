// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ACF includes
#include <imod/CMultiModelDispatcherBase.h>
#include <imod/TSingleModelObserverBase.h>

// Qt includes
#include <QRecursiveMutex>

// ImtCore includes
#include <imtauth/ITenantInvitationManager.h>
#include <imtauth/ITenantMembershipManager.h>
#include <imtauth/ITenantManager.h>
#include <imtservergql/CGqlPublisherCompBase.h>
#include <GeneratedFiles/imtauthsdl/SDL/1.0/CPP/TenantMemberships_fwd.h>


namespace imtauthgql
{


/**
	Server-side publisher that broadcasts notifications when tenant-related
	state changes: membership lifecycle, invitation lifecycle, or ownership
	transfer.

	The component observes:
	- imtauth::ITenantMembershipManager — membership state changes
	  (inactive→active = InvitationAccepted, removal of inactive = InvitationRejected)
	- imtauth::ITenantInvitationManager — invitation lifecycle
	  (new pending invitation = InvitationReceived, status transitions)
	- imtauth::ITenantManager — ownership transfers
	  (owner changed = OwnershipTransferred)

	Notifications are delivered only to subscribers whose authenticated user
	matches the target userId.

	The subscription endpoint name is configurable via the \c CommandId
	attribute and defaults to "OnMembershipNotification".
*/
class CTenantNotificationPublisherComp:
			public imtservergql::CGqlPublisherCompBase,
			protected imod::CMultiModelDispatcherBase
{
public:
	typedef imtservergql::CGqlPublisherCompBase BaseClass;
	typedef imod::CMultiModelDispatcherBase BaseClass2;

	I_BEGIN_COMPONENT(CTenantNotificationPublisherComp);
		I_ASSIGN(m_membershipManagerCompPtr, "MembershipManager", "Tenant membership manager to observe for changes", true, "TenantMembershipManager");
		I_ASSIGN_TO(m_membershipManagerModelCompPtr, m_membershipManagerCompPtr, true);
		I_ASSIGN(m_tenantManagerCompPtr, "TenantManager", "Tenant manager for resolving tenant owner and observing ownership changes", false, "TenantManager");
		I_ASSIGN_TO(m_tenantManagerModelCompPtr, m_tenantManagerCompPtr, false);
		I_ASSIGN(m_invitationManagerCompPtr, "InvitationManager", "Tenant invitation manager to observe for invitation lifecycle changes", false, "TenantInvitationManager");
		I_ASSIGN_TO(m_invitationManagerModelCompPtr, m_invitationManagerCompPtr, false);
	I_END_COMPONENT;

protected:
	// reimplemented (imtgql::IGqlSubscriberController)
	virtual bool IsRequestSupported(const imtgql::CGqlRequest& gqlRequest) const override;

	// reimplemented (icomp::CComponentBase)
	virtual void OnComponentCreated() override;
	virtual void OnComponentDestroyed() override;

	// reimplemented (imod::CMultiModelDispatcherBase)
	virtual void OnModelChanged(int modelId, const istd::IChangeable::ChangeSet & changeSet) override;

protected:
	I_REF(imtauth::ITenantMembershipManager, m_membershipManagerCompPtr);
	I_REF(imod::IModel, m_membershipManagerModelCompPtr);
	I_REF(imtauth::ITenantManager, m_tenantManagerCompPtr);
	I_REF(imod::IModel, m_tenantManagerModelCompPtr);
	I_REF(imtauth::ITenantInvitationManager, m_invitationManagerCompPtr);
	I_REF(imod::IModel, m_invitationManagerModelCompPtr);

private:
	struct CachedMembership
	{
		QByteArray userId;
		QByteArray tenantId;
		QByteArray roleId;
		bool isActive;
	};

	struct CachedTenantOwner
	{
		QByteArray ownerId;
	};

	struct CachedInvitation
	{
		QByteArray userId;
		QByteArray tenantId;
		QByteArray roleId;
		QByteArray invitedByUserId;
		imtauth::ITenantInvitation::TenantInvitationStatus status;
	};

	void PublishNotification(
		const QByteArray& targetUserId,
		sdl::V1_0::imtauth::EMembershipNotificationType notificationType,
		const QByteArray& membershipId,
		const QByteArray& userId,
		const QByteArray& tenantId,
		const QString& tenantName,
		const QByteArray& roleId) const;

	QByteArray FindTenantOwnerUserId(const QByteArray& tenantId) const;

	// Cache of membershipId → state for change detection.
	mutable QMap<QByteArray, CachedMembership> m_cachedMemberships;
	// Cache of tenantId → ownerId for ownership change detection.
	mutable QMap<QByteArray, CachedTenantOwner> m_cachedTenantOwners;
	// Cache of invitationId → state for invitation lifecycle detection.
	mutable QMap<QByteArray, CachedInvitation> m_cachedInvitations;
	// Protects all caches from concurrent access.
	mutable QRecursiveMutex m_cacheMutex;
};


} // namespace imtauthgql

