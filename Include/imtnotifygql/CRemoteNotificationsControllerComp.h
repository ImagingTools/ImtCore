// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ACF includes
#include <imtnotify/INotificationTypeList.h>

// ImtCore includes
#include <imtnotifygql/imtnotifygql.h>
#include <imtclientgql/TClientRequestManagerCompWrap.h>
#include <GeneratedFiles/imtnotifysdl/SDL/1.0/CPP/Notifications_fwd.h>


namespace imtnotifygql
{


/**
	Inbox of a product server: forwards the inbox commands of the client to the notification center of the
	authorization server with the session of the client, which decides the inbox. The catalog of the notification types
	is its own: the types of the modules it runs (NotificationTypes) and the ones the product adds
	(ProductNotificationTypes), with the labels in the language of the client.
	The sending commands are refused: a product server sends through CRemoteNotificationSenderComp.
*/
class CRemoteNotificationsControllerComp:
			public imtclientgql::TClientRequestManagerCompWrap<sdl::V1_0::imtnotify::CNotificationsGqlHandlerCompBase>
{
public:
	typedef imtclientgql::TClientRequestManagerCompWrap<sdl::V1_0::imtnotify::CNotificationsGqlHandlerCompBase> BaseClass;

	I_BEGIN_COMPONENT(CRemoteNotificationsControllerComp);
		I_ASSIGN_MULTI_0(m_notificationTypesCompPtr, "NotificationTypes", "Notification types of the modules this server runs", false);
		I_ASSIGN_MULTI_0(m_productNotificationTypesCompPtr, "ProductNotificationTypes", "Notification types the product adds", false);
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
	I_MULTIREF(imtnotify::INotificationTypeList, m_notificationTypesCompPtr);
	I_MULTIREF(imtnotify::INotificationTypeList, m_productNotificationTypesCompPtr);

	// Each error of the catalog is logged once, the catalog is built for every request
	mutable CReportedMessages m_reportedCatalogErrors;
};


} // namespace imtnotifygql


