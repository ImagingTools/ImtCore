// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include "ImtNotifyGqlPck.h"


// ACF includes
#include <icomp/export.h>


namespace ImtNotifyGqlPck
{


I_EXPORT_PACKAGE(
			"ImtNotifyGqlPck",
			"User notification GraphQL component package",
			IM_PROJECT("\"ImagingTools Core Framework\"") IM_COMPANY("ImagingTools") "GraphQL");

I_EXPORT_COMPONENT(
			NotificationsController,
			"GraphQL queries and mutations of the inbox of the caller",
			"Notifications Controller");

I_EXPORT_COMPONENT(
			NotificationCollectionController,
			"The inbox of the caller as the collection Notifications",
			"Notification Collection Controller");

I_EXPORT_COMPONENT(
			NotificationPublisher,
			"OnNotificationEvent subscription: pushes inbox changes to the sessions of the user",
			"Notification Publisher");

I_EXPORT_COMPONENT(
			RemoteNotificationsController,
			"Inbox of a product server: forwards the inbox commands to the notification center of the authorization server",
			"Remote Notifications Controller");

I_EXPORT_COMPONENT(
			RemoteNotificationSender,
			"Sender of a product server: sends to the notification center of the authorization server with the server account",
			"Remote Notification Sender");

I_EXPORT_COMPONENT(
			NotificationType,
			"A notification type, or a type of the objects of notifications: id, label and icon",
			"Notification Type");

I_EXPORT_COMPONENT(
			NotificationTypeList,
			"The notification types of a module and the types of their objects",
			"Notification Type List");


} // namespace ImtNotifyGqlPck
