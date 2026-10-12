// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ACF includes
#include <imod/CMultiModelDispatcherBase.h>
#include <imod/TSingleModelObserverBase.h>

// Qt includes
#include <QMap>
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
	Server-side publisher of the membership notifications of a user.

	Every change of the observed managers is compared with a snapshot of all
	memberships, invitations and owners, and each transition is announced to
	the users it concerns:
	- InvitationReceived: the invited user (also when the invitation is resent)
	- InvitationAccepted, InvitationRejected: the user who sent the invitation
	- InvitationRevoked: the invited user
	- MembershipRoleChanged: the member whose organization permissions changed
	- MembershipRemoved: the removed member and the owner of the organization
	- OwnershipTransferred: the previous and the new owner

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
	struct MembershipState
	{
		QByteArray userId;
		QByteArray tenantId;
		QByteArrayList organizationPermissions;
	};

	struct InvitationState
	{
		QByteArray userId;
		QByteArray tenantId;
		QByteArray invitedByUserId;
		imtauth::ITenantInvitation::TenantInvitationStatus status = imtauth::ITenantInvitation::TIS_PENDING;
		QString lastSentAt;
	};

	struct Snapshot
	{
		QMap<QByteArray, MembershipState> memberships;
		QMap<QByteArray, InvitationState> invitations;
		QMap<QByteArray, QByteArray> owners;
	};

	struct Notification
	{
		QByteArray targetUserId;
		sdl::V1_0::imtauth::EMembershipNotificationType notificationType;
		QByteArray membershipId;
		QByteArray userId;
		QByteArray tenantId;
	};

	Snapshot TakeSnapshot() const;
	QList<Notification> CompareSnapshots(const Snapshot& before, const Snapshot& after) const;

	void PublishNotification(const Notification& notification) const;

	QString FindTenantName(const QByteArray& tenantId) const;

	Snapshot m_snapshot;
	// Protects the snapshot from concurrent model notifications.
	mutable QRecursiveMutex m_snapshotMutex;
};


} // namespace imtauthgql

