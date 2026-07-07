// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once

/**
	\file imtnotifygql.h
	\brief Main header file for the imtnotifygql module - Notification GraphQL API Layer
*/

/**
	\namespace imtnotifygql
	\brief Notification (Activity) GraphQL API Layer Module

	The \b imtnotifygql module bridges the imtnotify domain model with the
	GraphQL API defined in the Notifications SDL schema.

	\section imtnotifygql_overview Overview

	- \b CNotificationsControllerComp — handles NotificationsList / NotificationItem /
	  UnreadNotificationsCount queries and the MarkNotificationRead /
	  MarkAllNotificationsRead / DeleteNotification / RaiseNotification mutations,
	  scoped to the authenticated user
	- \b CNotificationPublisherComp — server-side WebSocket publisher that pushes
	  each newly raised notification to its recipient (OnNotificationReceived)

	\section imtnotifygql_related Related Modules
	- \b imtnotify — domain model and INotificationService
	- \b imtnotifygui — QML "Events" page, global popup and menu indicator
*/
namespace imtnotifygql {}
