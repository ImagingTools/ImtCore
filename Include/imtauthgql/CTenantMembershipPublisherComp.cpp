// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtauthgql/CTenantMembershipPublisherComp.h>

// Qt includes
#include <QJsonDocument>
#include <QJsonObject>

// ImtCore includes
#include <imtgql/IGqlContext.h>
#include <imtauth/IUserInfo.h>


namespace imtauthgql
{


// public methods

void CTenantMembershipPublisherComp::PublishInvitationReceived(
	const QByteArray& targetUserId,
	const QByteArray& membershipId,
	const QByteArray& tenantId,
	const QString& tenantName,
	imtauth::ITenantMembership::TenantMemberRole role) const
{
	PublishNotification(
		targetUserId,
		sdl::imtauth::TenantMemberships::EMembershipNotificationType::InvitationReceived,
		membershipId,
		targetUserId,
		tenantId,
		tenantName,
		role);
}


void CTenantMembershipPublisherComp::PublishInvitationAccepted(
	const QByteArray& targetUserId,
	const QByteArray& acceptingUserId,
	const QByteArray& membershipId,
	const QByteArray& tenantId,
	const QString& tenantName,
	imtauth::ITenantMembership::TenantMemberRole role) const
{
	PublishNotification(
		targetUserId,
		sdl::imtauth::TenantMemberships::EMembershipNotificationType::InvitationAccepted,
		membershipId,
		acceptingUserId,
		tenantId,
		tenantName,
		role);
}


void CTenantMembershipPublisherComp::PublishInvitationRejected(
	const QByteArray& targetUserId,
	const QByteArray& rejectingUserId,
	const QByteArray& membershipId,
	const QByteArray& tenantId,
	const QString& tenantName,
	imtauth::ITenantMembership::TenantMemberRole role) const
{
	PublishNotification(
		targetUserId,
		sdl::imtauth::TenantMemberships::EMembershipNotificationType::InvitationRejected,
		membershipId,
		rejectingUserId,
		tenantId,
		tenantName,
		role);
}


// private methods

void CTenantMembershipPublisherComp::PublishNotification(
	const QByteArray& targetUserId,
	sdl::imtauth::TenantMemberships::EMembershipNotificationType notificationType,
	const QByteArray& membershipId,
	const QByteArray& userId,
	const QByteArray& tenantId,
	const QString& tenantName,
	imtauth::ITenantMembership::TenantMemberRole role) const
{
	sdl::imtauth::TenantMemberships::CMembershipNotification notification;
	notification.Version_1_0.emplace();
	notification.Version_1_0->notificationType = static_cast<sdl::imtauth::TenantMemberships::EMembershipNotificationType>(notificationType);
	notification.Version_1_0->membershipId = membershipId;
	notification.Version_1_0->userId = userId;
	notification.Version_1_0->tenantId = tenantId;
	notification.Version_1_0->tenantName = tenantName;
	notification.Version_1_0->role = static_cast<sdl::imtauth::TenantMemberships::TenantMemberRole>(role);

	QJsonObject jsonObject;
	if (!notification.WriteToJsonObject(jsonObject)){
		Q_ASSERT(false);
		return;
	}

	QJsonDocument jsonDoc;
	jsonDoc.setObject(jsonObject);
	QByteArray data = jsonDoc.toJson(QJsonDocument::Compact);

	QByteArray commandId = QByteArrayLiteral("OnMembershipNotification");

	// Filter: only push to subscribers whose context userId matches targetUserId
	PublishDataFiltered(commandId, data, [targetUserId](const imtgql::CGqlRequest& gqlRequest) -> bool {
		const imtgql::IGqlContext* contextPtr = gqlRequest.GetRequestContext();
		if (contextPtr != nullptr){
			const imtauth::IUserInfo* userInfoPtr = contextPtr->GetUserInfo();
			if (userInfoPtr != nullptr){
				return userInfoPtr->GetId() == targetUserId;
			}
		}
		return false;
	});
}


} // namespace imtauthgql

