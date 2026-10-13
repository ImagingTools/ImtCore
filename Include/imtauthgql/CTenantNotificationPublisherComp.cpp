// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtauthgql/CTenantNotificationPublisherComp.h>
#include <GeneratedFiles/imtauthsdl/SDL/1.0/CPP/TenantMemberships.h>

// Qt includes
#include <QJsonDocument>
#include <QJsonObject>
#include <QMutexLocker>

// ImtCore includes
#include <imtgql/CGqlRequest.h>
#include <imtgql/IGqlContext.h>


namespace imtauthgql
{


using NotificationType = sdl::V1_0::imtauth::EMembershipNotificationType;


// protected methods

// reimplemented (imtgql::IGqlSubscriberController)

bool CTenantNotificationPublisherComp::IsRequestSupported(const imtgql::CGqlRequest& gqlRequest) const
{
	if (m_commandIdsAttrPtr.IsValid()){
		QByteArray commandId = m_commandIdsAttrPtr[0];
		if (!commandId.isEmpty() && gqlRequest.GetCommandId() == commandId){
			return true;
		}
	}
	return BaseClass::IsRequestSupported(gqlRequest);
}


// reimplemented (icomp::CComponentBase)

void CTenantNotificationPublisherComp::OnComponentCreated()
{
	BaseClass::OnComponentCreated();

	{
		QMutexLocker locker(&m_snapshotMutex);
		m_snapshot = TakeSnapshot();
	}

	if (m_invitationManagerModelCompPtr.IsValid()){
		RegisterModel(m_invitationManagerModelCompPtr.GetPtr(), 0);
	}

	if (m_tenantManagerModelCompPtr.IsValid()){
		RegisterModel(m_tenantManagerModelCompPtr.GetPtr(), 1);
	}

	if (m_membershipManagerModelCompPtr.IsValid()){
		RegisterModel(m_membershipManagerModelCompPtr.GetPtr(), 2);
	}
}


void CTenantNotificationPublisherComp::OnComponentDestroyed()
{
	BaseClass2::UnregisterAllModels();

	BaseClass::OnComponentDestroyed();
}


// reimplemented (imod::CMultiModelDispatcherBase)

void CTenantNotificationPublisherComp::OnModelChanged(int /*modelId*/, const istd::IChangeable::ChangeSet& /*changeSet*/)
{
	QList<Notification> notifications;
	{
		QMutexLocker locker(&m_snapshotMutex);
		Snapshot current = TakeSnapshot();
		notifications = CompareSnapshots(m_snapshot, current);
		m_snapshot = current;
	}

	// Published outside the lock: PublishDataFiltered takes the publisher's own mutex.
	for (const Notification& notification : std::as_const(notifications)){
		PublishNotification(notification);
	}
}


// private methods

CTenantNotificationPublisherComp::Snapshot CTenantNotificationPublisherComp::TakeSnapshot() const
{
	Snapshot snapshot;
	if (!m_tenantManagerCompPtr.IsValid()){
		return snapshot;
	}

	const QByteArrayList tenantIds = m_tenantManagerCompPtr->GetTenantIds();
	for (const QByteArray& tenantId : tenantIds){
		imtauth::ITenantInfoUniquePtr tenantPtr = m_tenantManagerCompPtr->GetTenant(tenantId);
		if (tenantPtr.IsValid()){
			snapshot.owners.insert(tenantId, tenantPtr->GetOwnerId());
		}

		if (m_membershipManagerCompPtr.IsValid()){
			for (const QByteArray& membershipId : m_membershipManagerCompPtr->GetMembershipsByTenant(tenantId)){
				imtauth::ITenantMembershipUniquePtr membershipPtr = m_membershipManagerCompPtr->GetMembership(membershipId);
				if (membershipPtr.IsValid()){
					MembershipState state;
					state.userId = membershipPtr->GetUserId();
					state.tenantId = membershipPtr->GetTenantId();
					state.organizationPermissions = membershipPtr->GetOrganizationPermissions();
					std::sort(state.organizationPermissions.begin(), state.organizationPermissions.end());
					snapshot.memberships.insert(membershipId, state);
				}
			}
		}

		if (m_invitationManagerCompPtr.IsValid()){
			for (const QByteArray& invitationId : m_invitationManagerCompPtr->GetInvitationsByTenant(tenantId)){
				imtauth::ITenantInvitationUniquePtr invitationPtr = m_invitationManagerCompPtr->GetInvitation(invitationId);
				if (invitationPtr.IsValid()){
					InvitationState state;
					state.userId = invitationPtr->GetUserId();
					state.tenantId = invitationPtr->GetTenantId();
					state.invitedByUserId = invitationPtr->GetInvitedByUserId();
					state.status = invitationPtr->GetStatus();
					state.lastSentAt = invitationPtr->GetLastSentAt();
					snapshot.invitations.insert(invitationId, state);
				}
			}
		}
	}

	return snapshot;
}


QList<CTenantNotificationPublisherComp::Notification> CTenantNotificationPublisherComp::CompareSnapshots(
			const Snapshot& before,
			const Snapshot& after) const
{
	QList<Notification> notifications;

	// --- memberships
	for (auto it = before.memberships.constBegin(); it != before.memberships.constEnd(); ++it){
		const QByteArray& membershipId = it.key();
		const MembershipState& previous = it.value();

		if (!after.memberships.contains(membershipId)){
			notifications.append({previous.userId, NotificationType::MembershipRemoved, membershipId, previous.userId, previous.tenantId});

			const QByteArray ownerId = after.owners.value(previous.tenantId);
			if (!ownerId.isEmpty() && ownerId != previous.userId){
				notifications.append({ownerId, NotificationType::MembershipRemoved, membershipId, previous.userId, previous.tenantId});
			}
			continue;
		}

		if (after.memberships.value(membershipId).organizationPermissions != previous.organizationPermissions){
			notifications.append({previous.userId, NotificationType::MembershipRoleChanged, membershipId, previous.userId, previous.tenantId});
		}
	}

	// --- invitations
	for (auto it = after.invitations.constBegin(); it != after.invitations.constEnd(); ++it){
		const QByteArray& invitationId = it.key();
		const InvitationState& current = it.value();

		if (!before.invitations.contains(invitationId)){
			if (current.status == imtauth::ITenantInvitation::TIS_PENDING){
				notifications.append({current.userId, NotificationType::InvitationReceived, invitationId, current.userId, current.tenantId});
			}
			continue;
		}

		const InvitationState& previous = before.invitations.value(invitationId);
		if (previous.status == current.status){
			if (current.status == imtauth::ITenantInvitation::TIS_PENDING && previous.lastSentAt != current.lastSentAt){
				notifications.append({current.userId, NotificationType::InvitationReceived, invitationId, current.userId, current.tenantId});
			}
			continue;
		}

		// The answer goes back to whoever sent the invitation.
		const QByteArray senderId = current.invitedByUserId.isEmpty() ? after.owners.value(current.tenantId) : current.invitedByUserId;
		switch (current.status){
		case imtauth::ITenantInvitation::TIS_ACCEPTED:
			notifications.append({senderId, NotificationType::InvitationAccepted, invitationId, current.userId, current.tenantId});
			break;
		case imtauth::ITenantInvitation::TIS_REJECTED:
			notifications.append({senderId, NotificationType::InvitationRejected, invitationId, current.userId, current.tenantId});
			break;
		case imtauth::ITenantInvitation::TIS_REVOKED:
			notifications.append({current.userId, NotificationType::InvitationRevoked, invitationId, current.userId, current.tenantId});
			break;
		default:
			break;
		}
	}

	// --- ownership
	for (auto it = after.owners.constBegin(); it != after.owners.constEnd(); ++it){
		const QByteArray& tenantId = it.key();
		const QByteArray& ownerId = it.value();
		if (!before.owners.contains(tenantId)){
			continue;
		}

		const QByteArray previousOwnerId = before.owners.value(tenantId);
		if (previousOwnerId == ownerId){
			continue;
		}

		if (!previousOwnerId.isEmpty()){
			notifications.append({previousOwnerId, NotificationType::OwnershipTransferred, QByteArray(), ownerId, tenantId});
		}
		if (!ownerId.isEmpty()){
			notifications.append({ownerId, NotificationType::OwnershipTransferred, QByteArray(), ownerId, tenantId});
		}
	}

	return notifications;
}


void CTenantNotificationPublisherComp::PublishNotification(const Notification& notification) const
{
	if (notification.targetUserId.isEmpty()){
		return;
	}

	sdl::V1_0::imtauth::CMembershipNotification sdlNotification;
	sdlNotification.notificationType = notification.notificationType;
	sdlNotification.membershipId = notification.membershipId;
	sdlNotification.userId = notification.userId;
	sdlNotification.tenantId = notification.tenantId;
	sdlNotification.tenantName = FindTenantName(notification.tenantId);
	sdlNotification.role = QByteArray();

	QJsonObject jsonObject;
	if (!sdlNotification.WriteToJsonObject(jsonObject)){
		Q_ASSERT(false);
		return;
	}

	const QByteArray data = QJsonDocument(jsonObject).toJson(QJsonDocument::Compact);

	const QByteArray commandId = m_commandIdsAttrPtr.IsValid() && m_commandIdsAttrPtr.GetCount() > 0
			? m_commandIdsAttrPtr[0]
			: QByteArrayLiteral("OnMembershipNotification");

	const QByteArray targetUserId = notification.targetUserId;
	PublishDataFiltered(commandId, data, [targetUserId](const imtgql::CGqlRequest& gqlRequest) -> bool {
		const imtgql::IGqlContext* contextPtr = gqlRequest.GetRequestContext();
		return contextPtr != nullptr && contextPtr->GetUserId() == targetUserId;
	});
}


QString CTenantNotificationPublisherComp::FindTenantName(const QByteArray& tenantId) const
{
	if (!m_tenantManagerCompPtr.IsValid() || tenantId.isEmpty()){
		return QString();
	}

	imtauth::ITenantInfoUniquePtr tenantPtr = m_tenantManagerCompPtr->GetTenant(tenantId);

	return tenantPtr.IsValid() ? tenantPtr->GetTenantName() : QString();
}


} // namespace imtauthgql


