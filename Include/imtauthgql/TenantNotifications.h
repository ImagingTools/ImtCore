// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ImtCore includes
#include <imtnotify/INotificationSender.h>


namespace imtauthgql
{


/**
	Notifications of the organization events for the notification center.
	They belong to no product: an organization is shared by every product of the user.
*/
namespace TenantNotification
{


static const inline QByteArray s_tenantTypeId = QByteArrayLiteral("Tenant");

// The labels and icons of the types are in TenantNotificationTypes.acc
static const inline QByteArray s_invitationReceived = QByteArrayLiteral("imtauth.InvitationReceived");
static const inline QByteArray s_invitationAccepted = QByteArrayLiteral("imtauth.InvitationAccepted");
static const inline QByteArray s_invitationRejected = QByteArrayLiteral("imtauth.InvitationRejected");
static const inline QByteArray s_invitationRevoked = QByteArrayLiteral("imtauth.InvitationRevoked");
static const inline QByteArray s_ownershipReceived = QByteArrayLiteral("imtauth.OwnershipReceived");
static const inline QByteArray s_ownershipTransferred = QByteArrayLiteral("imtauth.OwnershipTransferred");
static const inline QByteArray s_membershipPermissionsChanged = QByteArrayLiteral("imtauth.MembershipPermissionsChanged");
static const inline QByteArray s_membershipEnded = QByteArrayLiteral("imtauth.MembershipEnded");
static const inline QByteArray s_memberLeft = QByteArrayLiteral("imtauth.MemberLeft");
static const inline QByteArray s_crossTenantMessageReceived = QByteArrayLiteral("imtauth.CrossTenantMessageReceived");
static const inline QByteArray s_connectionRequestReceived = QByteArrayLiteral("imtauth.ConnectionRequestReceived");
static const inline QByteArray s_connectionRequestApproved = QByteArrayLiteral("imtauth.ConnectionRequestApproved");
static const inline QByteArray s_connectionRequestRejected = QByteArrayLiteral("imtauth.ConnectionRequestRejected");
static const inline QByteArray s_connectionRemoved = QByteArrayLiteral("imtauth.ConnectionRemoved");
static const inline QByteArray s_relationshipProposalReceived = QByteArrayLiteral("imtauth.RelationshipProposalReceived");
static const inline QByteArray s_relationshipProposalApproved = QByteArrayLiteral("imtauth.RelationshipProposalApproved");
static const inline QByteArray s_relationshipProposalRejected = QByteArrayLiteral("imtauth.RelationshipProposalRejected");
static const inline QByteArray s_relationshipRemoved = QByteArrayLiteral("imtauth.RelationshipRemoved");


/**
	Send a notification about an organization to one user.
	\param tenantId	the organization whose inbox gets it; empty for a personal notification, e.g. an invitation
					to an organization the user is not a member of yet.
	\param groupKey	the notification is withdrawn by it once it is no longer true; may be empty.
*/
inline bool Send(
			imtnotify::INotificationSender* senderPtr,
			const QByteArray& userId,
			const QByteArray& tenantId,
			const QByteArray& typeId,
			const imtnotify::ObjectLinkInfo& subject,
			const QByteArray& groupKey = QByteArray(),
			istd::IInformationProvider::InformationCategory severity = istd::IInformationProvider::IC_INFO,
			const imtnotify::ObjectLinkInfo& actor = imtnotify::ObjectLinkInfo(),
			const imtnotify::NotificationArguments& arguments = imtnotify::NotificationArguments())
{
	if (senderPtr == nullptr || userId.isEmpty()){
		return false;
	}

	imtnotify::INotificationSender::Audience audience;
	audience.tenantId = tenantId;
	audience.userIds = {userId};

	imtnotify::INotificationSender::Message message;
	message.typeId = typeId;
	message.severity = severity;
	message.subject = subject;
	message.groupKey = groupKey;
	message.actor = actor;
	message.arguments = arguments;

	return senderPtr->Send(audience, message);
}


inline imtnotify::ObjectLinkInfo UserLink(const QByteArray& userId, const QString& userName)
{
	imtnotify::ObjectLinkInfo retVal;
	retVal.id = userId;
	retVal.typeId = QByteArrayLiteral("User");
	retVal.name = userName;

	return retVal;
}


inline imtnotify::ObjectLinkInfo TenantLink(const QByteArray& tenantId, const QString& tenantName)
{
	imtnotify::ObjectLinkInfo retVal;
	retVal.id = tenantId;
	retVal.typeId = s_tenantTypeId;
	retVal.name = tenantName;

	return retVal;
}


} // namespace TenantNotification


} // namespace imtauthgql


