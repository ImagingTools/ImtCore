// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ImtCore includes
#include <imtnotify/CNotificationTypeComp.h>
#include <imtnotify/CNotificationTypeListComp.h>
#include <imtnotifygql/CNotificationCollectionControllerComp.h>
#include <imtnotifygql/CNotificationsControllerComp.h>
#include <imtnotifygql/CNotificationPublisherComp.h>
#include <imtnotifygql/CRemoteNotificationsControllerComp.h>
#include <imtnotifygql/CRemoteNotificationSenderComp.h>


/**
	ImtNotifyGqlPck package
*/
namespace ImtNotifyGqlPck
{


typedef imtnotifygql::CNotificationsControllerComp NotificationsController;
typedef imtnotifygql::CNotificationCollectionControllerComp NotificationCollectionController;
typedef imtnotifygql::CNotificationPublisherComp NotificationPublisher;
typedef imtnotifygql::CRemoteNotificationsControllerComp RemoteNotificationsController;
typedef imtnotifygql::CRemoteNotificationSenderComp RemoteNotificationSender;
typedef imtnotify::CNotificationTypeComp NotificationType;
typedef imtnotify::CNotificationTypeListComp NotificationTypeList;


} // namespace ImtNotifyGqlPck
