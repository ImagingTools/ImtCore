// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once

/**
	\file imtnotify.h
	\brief Main header file for the imtnotify module - User Notification / Activity Domain Model
*/

/**
	\namespace imtnotify
	\brief User Notification (Activity feed) Domain Model Module

	The \b imtnotify module provides a generic, source-agnostic notification
	inbox. Any feature of the system (support tickets, chat, tenant membership,
	licensing, ...) can raise a notification addressed to a specific user via
	imtnotify::INotificationService. Notifications are persisted in a standard
	imtbase::IObjectCollection so that they survive restarts and can be listed,
	filtered and marked as read.

	This module is the back-end counterpart of the "Events" (Activity) page in
	the QML client, which is always visible independently of the user's
	permissions, shows an unread indicator on the menu, and raises a global
	popup when a new notification arrives.

	\section imtnotify_overview Overview
	- Persistent, per-recipient notification inbox
	- Severity levels (info / success / warning / critical)
	- Optional navigation route so the client can deep-link to the source entity
	- Real-time delivery via WebSocket subscription (imtnotifygql publisher)

	\section imtnotify_solid Design
	Producers depend only on the small imtnotify::INotificationService interface
	(Dependency Inversion). imtnotify has no knowledge of tickets, chat or any
	other producer, and producers have no knowledge of how notifications are
	stored, delivered or rendered (Single Responsibility / Open-Closed): a new
	producer is added without changing imtnotify.

	\section imtnotify_modules Related Modules
	- \b imtnotifygql — GraphQL API, collection controller and live publisher
	- \b imtnotifygui — QML "Events" page, list, global popup and menu indicator
*/
namespace imtnotify {}
