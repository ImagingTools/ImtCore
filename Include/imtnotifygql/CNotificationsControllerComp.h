// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ImtCore includes
#include <imtnotifygql/imtnotifygql.h>
#include <imtnotify/INotificationTypeList.h>
#include <imtauth/ITenantMembershipManager.h>
#include <imtbase/IObjectCollection.h>
#include <imtnotify/INotificationInboxController.h>
#include <imtnotify/INotificationSender.h>
#include <GeneratedFiles/imtnotifysdl/SDL/1.0/CPP/Notifications_fwd.h>


namespace imtnotifygql
{


/**
	GraphQL queries and mutations of the notification inbox.
	The inbox is always the one of the caller: user, tenant and product come from the request context.
	A request without a signed-in user is refused.
	SendNotification and WithdrawNotifications are for product servers: only an access token with the send scope
	of the product may call them, for that product only, and only if a sender is configured.
*/
class CNotificationsControllerComp: public sdl::V1_0::imtnotify::CNotificationsGqlHandlerCompBase
{
public:
	typedef sdl::V1_0::imtnotify::CNotificationsGqlHandlerCompBase BaseClass;

	I_BEGIN_COMPONENT(CNotificationsControllerComp);
		I_ASSIGN(m_collectionCompPtr, "NotificationCollection", "Collection of the notifications", true, "NotificationCollection");
		I_ASSIGN(m_inboxControllerCompPtr, "InboxController", "Controller of the changes of many notifications of an inbox", true, "InboxController");
		I_ASSIGN(m_senderCompPtr, "NotificationSender", "Sender for the notifications of product servers", false, "NotificationSender");
		I_ASSIGN(m_membershipManagerCompPtr, "TenantMembershipManager", "Members of the tenants: a product server notifies only the members of a tenant", false, "TenantMembershipManager");
		I_ASSIGN_MULTI_0(m_notificationTypesCompPtr, "NotificationTypes", "Notification types of the modules this server runs", false);
	I_END_COMPONENT;

protected:
	// reimplemented (sdl::V1_0::imtnotify::CNotificationsGqlHandlerCompBase)
	virtual sdl::V1_0::imtnotify::CNotificationsListPayload OnNotificationsList(
				const sdl::V1_0::imtnotify::CNotificationsListGqlRequest& notificationsListRequest,
				const ::imtgql::CGqlRequest& gqlRequest,
				QString& errorMessage) const override;
	virtual sdl::V1_0::imtnotify::CNotificationsUnreadCountPayload OnNotificationsUnreadCount(
				const sdl::V1_0::imtnotify::CNotificationsUnreadCountGqlRequest& notificationsUnreadCountRequest,
				const ::imtgql::CGqlRequest& gqlRequest,
				QString& errorMessage) const override;
	virtual sdl::V1_0::imtnotify::CNotificationTypesPayload OnNotificationTypes(
				const sdl::V1_0::imtnotify::CNotificationTypesGqlRequest& notificationTypesRequest,
				const ::imtgql::CGqlRequest& gqlRequest,
				QString& errorMessage) const override;
	virtual sdl::V1_0::imtnotify::CNotificationsSyncPayload OnNotificationsSync(
				const sdl::V1_0::imtnotify::CNotificationsSyncGqlRequest& notificationsSyncRequest,
				const ::imtgql::CGqlRequest& gqlRequest,
				QString& errorMessage) const override;
	virtual sdl::V1_0::imtnotify::CNotificationsChangedPayload OnMarkNotificationsRead(
				const sdl::V1_0::imtnotify::CMarkNotificationsReadGqlRequest& markNotificationsReadRequest,
				const ::imtgql::CGqlRequest& gqlRequest,
				QString& errorMessage) const override;
	virtual sdl::V1_0::imtnotify::CNotificationsChangedPayload OnMarkAllNotificationsRead(
				const sdl::V1_0::imtnotify::CMarkAllNotificationsReadGqlRequest& markAllNotificationsReadRequest,
				const ::imtgql::CGqlRequest& gqlRequest,
				QString& errorMessage) const override;
	virtual sdl::V1_0::imtnotify::CNotificationsChangedPayload OnAcknowledgeNotificationsDelivered(
				const sdl::V1_0::imtnotify::CAcknowledgeNotificationsDeliveredGqlRequest& acknowledgeNotificationsDeliveredRequest,
				const ::imtgql::CGqlRequest& gqlRequest,
				QString& errorMessage) const override;
	virtual sdl::V1_0::imtnotify::CNotificationsChangedPayload OnAcknowledgeAllNotificationsDelivered(
				const sdl::V1_0::imtnotify::CAcknowledgeAllNotificationsDeliveredGqlRequest& acknowledgeAllNotificationsDeliveredRequest,
				const ::imtgql::CGqlRequest& gqlRequest,
				QString& errorMessage) const override;
	virtual sdl::V1_0::imtnotify::CNotificationsChangedPayload OnRemoveNotifications(
				const sdl::V1_0::imtnotify::CRemoveNotificationsGqlRequest& removeNotificationsRequest,
				const ::imtgql::CGqlRequest& gqlRequest,
				QString& errorMessage) const override;
	virtual sdl::V1_0::imtnotify::CNotificationsSentPayload OnSendNotification(
				const sdl::V1_0::imtnotify::CSendNotificationGqlRequest& sendNotificationRequest,
				const ::imtgql::CGqlRequest& gqlRequest,
				QString& errorMessage) const override;
	virtual sdl::V1_0::imtnotify::CNotificationsSentPayload OnWithdrawNotifications(
				const sdl::V1_0::imtnotify::CWithdrawNotificationsGqlRequest& withdrawNotificationsRequest,
				const ::imtgql::CGqlRequest& gqlRequest,
				QString& errorMessage) const override;

private:
	/**
		The inbox of the caller; false with an error message for a request without a signed-in user.
	*/
	bool GetCallerScope(const ::imtgql::CGqlRequest& gqlRequest, imtnotify::NotificationScope& scope, QString& errorMessage) const;

	/**
		Whether the caller may send notifications of the product: a signed-in user, the request made for that product.
		Refuses when no sender is configured.
	*/
	bool CanSend(const ::imtgql::CGqlRequest& gqlRequest, const QByteArray& productId, QString& errorMessage) const;

	/**
		The payload of a change: the changed ids and the unread count after it.
	*/
	sdl::V1_0::imtnotify::CNotificationsChangedPayload CreateChangedPayload(
				const imtnotify::NotificationScope& scope,
				bool isChanged,
				const QByteArrayList& changedIds,
				QString& errorMessage) const;

private:
	I_REF(imtbase::IObjectCollection, m_collectionCompPtr);
	I_REF(imtnotify::INotificationInboxController, m_inboxControllerCompPtr);
	I_REF(imtnotify::INotificationSender, m_senderCompPtr);
	I_REF(imtauth::ITenantMembershipManager, m_membershipManagerCompPtr);
	I_MULTIREF(imtnotify::INotificationTypeList, m_notificationTypesCompPtr);

	// Each error of the catalog is logged once, the catalog is built for every request
	mutable CReportedMessages m_reportedCatalogErrors;
};


} // namespace imtnotifygql


