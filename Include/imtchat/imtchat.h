// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once

/**
	\file imtchat.h
	\brief Main header file for the imtchat module - Internal Service Desk Chat Domain Model
*/

/**
	\namespace imtchat
	\brief Internal Service Desk Chat Domain Model Module

	The \b imtchat module provides the core domain model for the internal Service Desk
	chat system embedded in the desktop application. It supports direct and group
	messaging, and entity references.

	\section imtchat_overview Overview
	- Personal messages (1:1 Direct conversations)
	- Group chats and support channels
	- Entity references: link any ImtCore entity in a message
	- Real-time delivery via WebSocket subscriptions

	\section imtchat_modules Related Modules
	- \b imtchatdb — PostgreSQL/SQLite persistence
	- \b imtchatgql — GraphQL API
	- \b imtchatgui — QML user interface
	- \b imtdesk — Service Desk ticketing domain model
*/
namespace imtchat {}
