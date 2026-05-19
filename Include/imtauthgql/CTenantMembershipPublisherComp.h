// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ACF includes
#include <imod/TSingleModelObserverBase.h>

// ImtCore includes
#include <imtauth/ITenantMembershipManager.h>
#include <imtauth/ITenantManager.h>
#include <imtservergql/CGqlPublisherCompBase.h>
#include <GeneratedFiles/imtauthsdl/SDL/1.0/CPP/TenantMemberships.h>


namespace imtauthgql
{


/**
	Server-side publisher that broadcasts notifications when tenant membership
	state changes (invitation, acceptance, rejection) or when tenant ownership
	is transferred.

	The component observes imtauth::ITenantMembershipManager via
	imod::TSingleModelObserverBase. When a membership changes, it compares
	the cached state to detect:
	- New inactive memberships → InvitationReceived (notifies invited user)
	- Previously inactive membership becoming active → InvitationAccepted (notifies owner)
	- Previously inactive membership removed → InvitationRejected (notifies owner)

	Additionally, the component observes imtauth::ITenantManager to detect
	ownership transfers:
	- Owner changed → OwnershipTransferred (notifies both old and new owner)

	Notifications are delivered only to subscribers whose authenticated user
	matches the target userId.

	The subscription endpoint name is configurable via the \c CommandId
	attribute and defaults to "OnMembershipNotification".
*/
class CTenantMembershipPublisherComp:
			public imtservergql::CGqlPublisherCompBase,
			protected imod::TSingleModelObserverBase<imtauth::ITenantMembershipManager>
{
public:
	typedef imtservergql::CGqlPublisherCompBase BaseClass;
	typedef imod::TSingleModelObserverBase<imtauth::ITenantMembershipManager> BaseClass2;

	I_BEGIN_COMPONENT(CTenantMembershipPublisherComp);
		I_ASSIGN(m_membershipManagerCompPtr, "MembershipManager", "Tenant membership manager to observe for changes", true, "TenantMembershipManager");
		I_ASSIGN_TO(m_membershipManagerModelCompPtr, m_membershipManagerCompPtr, true);
		I_ASSIGN(m_tenantManagerCompPtr, "TenantManager", "Tenant manager for resolving tenant owner and observing ownership changes", false, "TenantManager");
		I_ASSIGN_TO(m_tenantManagerModelCompPtr, m_tenantManagerCompPtr, false);
	I_END_COMPONENT;

protected:
	// reimplemented (imtgql::IGqlSubscriberController)
	virtual bool IsRequestSupported(const imtgql::CGqlRequest& gqlRequest) const override;

	// reimplemented (icomp::CComponentBase)
	virtual void OnComponentCreated() override;
	virtual void OnComponentDestroyed() override;

	// reimplemented (imod::CSingleModelObserverBase)
	virtual void OnUpdate(const istd::IChangeable::ChangeSet& changeSet) override;

protected:
	I_REF(imtauth::ITenantMembershipManager, m_membershipManagerCompPtr);
	I_REF(imod::IModel, m_membershipManagerModelCompPtr);
	I_REF(imtauth::ITenantManager, m_tenantManagerCompPtr);
	I_REF(imod::IModel, m_tenantManagerModelCompPtr);

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

	void HandleMembershipChanges();
	void HandleOwnershipChanges();

	void PublishNotification(
		const QByteArray& targetUserId,
		sdl::imtauth::TenantMemberships::EMembershipNotificationType notificationType,
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
};


} // namespace imtauthgql

