// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ACF includes
#include <imod/CMultiModelDispatcherBase.h>
#include <imod/TSingleModelObserverBase.h>

// Qt includes
#include <QList>
#include <QMutex>
#include <QPair>
#include <QSet>
#include <QTimer>
#include <QVariantMap>

// ImtCore includes
#include <imtauth/ITenantInvitationManager.h>
#include <imtauth/ITenantMembershipManager.h>
#include <imtauth/ITenantManager.h>
#include <imtbase/IObjectCollection.h>
#include <imtservergql/CGqlPublisherCompBase.h>
#include <imtnotify/INotificationInboxController.h>
#include <imtnotify/INotificationSender.h>
#include <GeneratedFiles/imtauthsdl/SDL/1.0/CPP/TenantMemberships_fwd.h>


namespace imtauthgql
{


/**
	Server-side publisher of the membership notifications of a user.

	The observed managers name each change they make (imtauth::TenantEvent),
	and each one is announced to the users it concerns:
	- InvitationReceived: the invited user (also when the invitation is resent)
	- InvitationAccepted, InvitationRejected: the user who sent the invitation
	- InvitationRevoked: the invited user
	- MembershipRoleChanged: the member whose organization permissions changed
	- MembershipRemoved: the removed member and the owner of the organization
	- OwnershipTransferred: the previous and the new owner

	Notifications are delivered only to subscribers whose authenticated user
	matches the target userId.

	Any of the changes above and a new membership are also announced by OnOrganizationMembersChanged
	to every subscriber whose session is in the organization.

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
		I_ASSIGN(m_notificationSenderCompPtr, "NotificationSender", "Notification center that keeps the notifications of the users", false, "NotificationSender");
		I_ASSIGN(m_userCollectionCompPtr, "UserCollection", "Users: the names of the users the notifications are about", false, "UserCollection");
		I_ASSIGN(m_inboxControllerCompPtr, "InboxController", "Inboxes of the users: a removed member loses the notifications of the organization", false, "InboxController");
	I_END_COMPONENT;

protected:
	// reimplemented (imtgql::IGqlSubscriberController)
	virtual bool IsRequestSupported(const imtgql::CGqlRequest& gqlRequest) const override;
	virtual bool IsOpenFor(SubscriberKind subscriberKind, const imtgql::CGqlRequest& gqlRequest) const override;

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
	I_REF(imtnotify::INotificationSender, m_notificationSenderCompPtr);
	I_REF(imtnotify::INotificationInboxController, m_inboxControllerCompPtr);
	I_REF(imtbase::IObjectCollection, m_userCollectionCompPtr);

private:
	struct Notification
	{
		QByteArray targetUserId;
		sdl::V1_0::imtauth::EMembershipNotificationType notificationType;
		QByteArray membershipId;
		QByteArray userId;
		QByteArray tenantId;

		// Who sent an invitation
		QByteArray inviterId = QByteArray();
	};

	/**
		A removed member loses the notifications of the organization; a failed removal is retried.
	*/
	void RemoveMemberNotifications(const QByteArray& tenantId, const QByteArray& userId);
	void RetryMemberCleanups();

	QList<Notification> CreateNotifications(const QVariantMap& event) const;
	void PublishNotification(const Notification& notification) const;

	/**
		Keep the notification in the inbox of its user; an invitation that is answered or revoked is withdrawn.
	*/
	void SendToNotificationCenter(const Notification& notification) const;
	void PublishMembersChanged(const QByteArray& tenantId) const;

	QString FindTenantName(const QByteArray& tenantId) const;
	imtnotify::ObjectLinkInfo GetUserLink(const QByteArray& userId) const;

	/**
		The user whose request made the change, named from the users when the request has no name.
	*/
	imtnotify::ObjectLinkInfo GetRequestUserLink() const;
	QByteArray FindTenantOwnerId(const QByteArray& tenantId) const;

private:
	// Tenant and user of the removed memberships whose notifications could not be removed yet
	QSet<QPair<QByteArray, QByteArray>> m_pendingMemberCleanups;
	QMutex m_pendingMemberCleanupsMutex;
	QTimer m_retryTimer;
};


} // namespace imtauthgql

