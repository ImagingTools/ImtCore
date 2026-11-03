// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once

/**
	\file imtnotify.h
	\brief Main header file for the imtnotify module - user notification center
*/

/**
	\namespace imtnotify
	\brief User Notification Center Module

	The \b imtnotify module keeps the notifications of the users and delivers them.
	A notification belongs to the inbox of one user in one tenant and product and is stored,
	so a user who is offline sees it after the next login.

	\section imtnotify_overview Overview
	- \b INotification / CNotification — a notification, an ilog::CMessage
	- \b INotificationSender — the interface a feature uses to notify users; CNotificationSenderComp stores into the collection
	- \b INotificationInboxController — the changes of many notifications of an inbox, which a collection cannot do

	\section imtnotify_modules Related Modules
	- \b imtnotifydb — PostgreSQL/SQLite storage
	- \b imtnotifygql — GraphQL API and WebSocket push
	- \b imtnotifygui — QML notification center
*/
namespace imtnotify {}
