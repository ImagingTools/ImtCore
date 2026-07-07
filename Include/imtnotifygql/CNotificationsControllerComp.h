// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once

// ImtCore includes
#include <imtnotify/INotificationService.h>
#include <imtbase/IObjectCollection.h>
#include <GeneratedFiles/imtnotifysdl/SDL/1.0/CPP/Notifications_fwd.h>


namespace imtnotifygql
{


/**
	GraphQL query/mutation handler for the notification (Activity) subsystem.

	Every read operation is scoped to the authenticated user taken from the
	GraphQL request context, so a user can only ever see and modify their own
	notifications. Reads reuse imtnotify::INotificationService for the id lists /
	counts and the notification collection to materialise object data; writes are
	fully delegated to the service (Dependency Inversion).
*/
class CNotificationsControllerComp:
		public sdl::V1_0::imtnotify::CNotificationsGqlHandlerCompBase
{
public:
	typedef sdl::V1_0::imtnotify::CNotificationsGqlHandlerCompBase BaseClass;

	I_BEGIN_COMPONENT(CNotificationsControllerComp);
		I_ASSIGN(m_notificationServiceCompPtr, "NotificationService", "Notification service used for reads and writes", true, "NotificationService");
		I_ASSIGN(m_notificationCollectionCompPtr, "NotificationCollection", "Collection storing notifications", true, "NotificationCollection");
	I_END_COMPONENT;

protected:
	// reimplemented (sdl::V1_0::imtnotify::CNotificationsGqlHandlerCompBase)
	virtual sdl::V1_0::imtnotify::CNotificationsListPayload OnNotificationsList(
				const sdl::V1_0::imtnotify::CNotificationsListGqlRequest& request,
				const ::imtgql::CGqlRequest& gqlRequest,
				QString& errorMessage) const override;
	virtual sdl::V1_0::imtnotify::CNotificationData OnNotificationItem(
				const sdl::V1_0::imtnotify::CNotificationItemGqlRequest& request,
				const ::imtgql::CGqlRequest& gqlRequest,
				QString& errorMessage) const override;
	virtual sdl::V1_0::imtnotify::CUnreadNotificationsCountPayload OnUnreadNotificationsCount(
				const sdl::V1_0::imtnotify::CUnreadNotificationsCountGqlRequest& request,
				const ::imtgql::CGqlRequest& gqlRequest,
				QString& errorMessage) const override;
	virtual sdl::V1_0::imtbase::CUpdatedNotificationPayload OnMarkNotificationRead(
				const sdl::V1_0::imtnotify::CMarkNotificationReadGqlRequest& request,
				const ::imtgql::CGqlRequest& gqlRequest,
				QString& errorMessage) const override;
	virtual sdl::V1_0::imtnotify::CMarkAllNotificationsReadPayload OnMarkAllNotificationsRead(
				const sdl::V1_0::imtnotify::CMarkAllNotificationsReadGqlRequest& request,
				const ::imtgql::CGqlRequest& gqlRequest,
				QString& errorMessage) const override;
	virtual sdl::V1_0::imtbase::CRemovedNotificationPayload OnDeleteNotification(
				const sdl::V1_0::imtnotify::CDeleteNotificationGqlRequest& request,
				const ::imtgql::CGqlRequest& gqlRequest,
				QString& errorMessage) const override;
	virtual sdl::V1_0::imtbase::CAddedNotificationPayload OnRaiseNotification(
				const sdl::V1_0::imtnotify::CRaiseNotificationGqlRequest& request,
				const ::imtgql::CGqlRequest& gqlRequest,
				QString& errorMessage) const override;

private:
	// Resolve the recipient scope for the current request: the authenticated
	// user from the context, falling back to an explicitly supplied id only
	// when the context has no user (e.g. internal/service calls).
	QByteArray ResolveRecipientId(const ::imtgql::CGqlRequest& gqlRequest, const QByteArray& explicitRecipientId) const;

	// Verify that the notification with the given id belongs to the recipient.
	bool IsOwnedBy(const QByteArray& notificationId, const QByteArray& recipientId) const;

private:
	I_REF(imtnotify::INotificationService, m_notificationServiceCompPtr);
	I_REF(imtbase::IObjectCollection, m_notificationCollectionCompPtr);
};


} // namespace imtnotifygql
