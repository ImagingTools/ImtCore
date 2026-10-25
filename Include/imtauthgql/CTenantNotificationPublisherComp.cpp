// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtauthgql/CTenantNotificationPublisherComp.h>
#include <GeneratedFiles/imtauthsdl/SDL/1.0/CPP/TenantMemberships.h>

// Qt includes
#include <QJsonDocument>
#include <QJsonObject>

// ImtCore includes
#include <imtgql/CGqlRequest.h>
#include <imtgql/IGqlContext.h>
#include <imtauth/TenantEvents.h>


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

void CTenantNotificationPublisherComp::OnModelChanged(int /*modelId*/, const istd::IChangeable::ChangeSet& changeSet)
{
	// The managers name each change, so only the organizations it touched are read
	for (const QVariantMap& event : imtauth::TenantEvent::GetEvents(changeSet)){
		for (const Notification& notification : CreateNotifications(event)){
			PublishNotification(notification);
		}
	}
}


// private methods

QList<CTenantNotificationPublisherComp::Notification> CTenantNotificationPublisherComp::CreateNotifications(const QVariantMap& event) const
{
	QList<Notification> notifications;

	const QString type = event.value(imtauth::TenantEvent::s_type).toString();
	const QByteArray tenantId = event.value(imtauth::TenantEvent::s_tenantId).toByteArray();
	const QByteArray userId = event.value(imtauth::TenantEvent::s_userId).toByteArray();

	if (type == imtauth::TenantEvent::s_membershipRemoved){
		const QByteArray membershipId = event.value(imtauth::TenantEvent::s_membershipId).toByteArray();
		notifications.append({userId, NotificationType::MembershipRemoved, membershipId, userId, tenantId});

		const QByteArray ownerId = FindTenantOwnerId(tenantId);
		if (!ownerId.isEmpty() && ownerId != userId){
			notifications.append({ownerId, NotificationType::MembershipRemoved, membershipId, userId, tenantId});
		}
	}
	else if (type == imtauth::TenantEvent::s_membershipPermissionsChanged){
		notifications.append({userId, NotificationType::MembershipRoleChanged, event.value(imtauth::TenantEvent::s_membershipId).toByteArray(), userId, tenantId});
	}
	else if (type == imtauth::TenantEvent::s_invitationSent || type == imtauth::TenantEvent::s_invitationStatusChanged){
		const QByteArray invitationId = event.value(imtauth::TenantEvent::s_invitationId).toByteArray();
		const int status = event.value(imtauth::TenantEvent::s_status).toInt();
		if (type == imtauth::TenantEvent::s_invitationSent){
			if (status == imtauth::ITenantInvitation::TIS_PENDING){
				notifications.append({userId, NotificationType::InvitationReceived, invitationId, userId, tenantId});
			}
		}
		else if (status == imtauth::ITenantInvitation::TIS_REVOKED){
			notifications.append({userId, NotificationType::InvitationRevoked, invitationId, userId, tenantId});
		}
		else if (status == imtauth::ITenantInvitation::TIS_ACCEPTED || status == imtauth::ITenantInvitation::TIS_REJECTED){
			// The answer goes back to whoever sent the invitation
			QByteArray senderId = event.value(imtauth::TenantEvent::s_invitedByUserId).toByteArray();
			if (senderId.isEmpty()){
				senderId = FindTenantOwnerId(tenantId);
			}

			const NotificationType notificationType = status == imtauth::ITenantInvitation::TIS_ACCEPTED
						? NotificationType::InvitationAccepted
						: NotificationType::InvitationRejected;
			notifications.append({senderId, notificationType, invitationId, userId, tenantId});
		}
	}
	else if (type == imtauth::TenantEvent::s_ownershipTransferred){
		const QByteArray previousOwnerId = event.value(imtauth::TenantEvent::s_previousOwnerId).toByteArray();
		const QByteArray ownerId = event.value(imtauth::TenantEvent::s_ownerId).toByteArray();
		if (!previousOwnerId.isEmpty()){
			notifications.append({previousOwnerId, NotificationType::OwnershipTransferred, QByteArray(), ownerId, tenantId});
		}
		if (!ownerId.isEmpty()){
			notifications.append({ownerId, NotificationType::OwnershipTransferred, QByteArray(), ownerId, tenantId});
		}
	}

	return notifications;
}


QByteArray CTenantNotificationPublisherComp::FindTenantOwnerId(const QByteArray& tenantId) const
{
	if (!m_tenantManagerCompPtr.IsValid() || tenantId.isEmpty()){
		return QByteArray();
	}

	imtauth::ITenantInfoUniquePtr tenantPtr = m_tenantManagerCompPtr->GetTenant(tenantId);

	return tenantPtr.IsValid() ? tenantPtr->GetOwnerId() : QByteArray();
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


