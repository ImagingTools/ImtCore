// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ACF includes
#include <icomp/CComponentBase.h>

// ImtCore includes
#include <imtauth/ITenantMembershipManager.h>
#include <imtservergql/CGqlPublisherCompBase.h>
#include <GeneratedFiles/imtauthsdl/SDL/1.0/CPP/TenantMemberships.h>


namespace imtauthgql
{


/**
 * Publisher component for tenant membership notifications.
 * Notifies invited users about new invitations and notifies inviters
 * about invitation acceptance/rejection.
 *
 * Publishes to subscribers filtered by target userId so that only
 * the relevant user receives the notification.
 */
class CTenantMembershipPublisherComp: public imtservergql::CGqlPublisherCompBase
{
public:
	typedef imtservergql::CGqlPublisherCompBase BaseClass;

	I_BEGIN_COMPONENT(CTenantMembershipPublisherComp);
	I_END_COMPONENT;

	/**
	 * Publish an invitation notification to the invited user.
	 * @param targetUserId  The user being invited (receives the notification)
	 * @param membershipId  The membership ID
	 * @param tenantId      The tenant ID
	 * @param tenantName    Display name of the tenant
	 * @param role          Assigned role
	 */
	void PublishInvitationReceived(
		const QByteArray& targetUserId,
		const QByteArray& membershipId,
		const QByteArray& tenantId,
		const QString& tenantName,
		imtauth::ITenantMembership::TenantMemberRole role) const;

	/**
	 * Publish an acceptance notification to the inviter (tenant owner).
	 * @param targetUserId  The owner/inviter who receives this notification
	 * @param acceptingUserId  The user who accepted
	 * @param membershipId  The membership ID
	 * @param tenantId      The tenant ID
	 * @param tenantName    Display name of the tenant
	 * @param role          The role of the accepted member
	 */
	void PublishInvitationAccepted(
		const QByteArray& targetUserId,
		const QByteArray& acceptingUserId,
		const QByteArray& membershipId,
		const QByteArray& tenantId,
		const QString& tenantName,
		imtauth::ITenantMembership::TenantMemberRole role) const;

	/**
	 * Publish a rejection notification to the inviter (tenant owner).
	 * @param targetUserId  The owner/inviter who receives this notification
	 * @param rejectingUserId  The user who rejected
	 * @param membershipId  The membership ID
	 * @param tenantId      The tenant ID
	 * @param tenantName    Display name of the tenant
	 * @param role          The role that was offered
	 */
	void PublishInvitationRejected(
		const QByteArray& targetUserId,
		const QByteArray& rejectingUserId,
		const QByteArray& membershipId,
		const QByteArray& tenantId,
		const QString& tenantName,
		imtauth::ITenantMembership::TenantMemberRole role) const;

private:
	void PublishNotification(
		const QByteArray& targetUserId,
		sdl::imtauth::TenantMemberships::EMembershipNotificationType notificationType,
		const QByteArray& membershipId,
		const QByteArray& userId,
		const QByteArray& tenantId,
		const QString& tenantName,
		imtauth::ITenantMembership::TenantMemberRole role) const;
};


} // namespace imtauthgql

