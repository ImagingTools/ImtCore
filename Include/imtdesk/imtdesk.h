// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once

/**
	\file imtdesk.h
	\brief Main header file for the imtdesk module - Service Desk Ticketing Domain Model
*/

/**
	\namespace imtdesk
	\brief Service Desk Ticketing Domain Model Module

	The \b imtdesk module provides the core domain model for the Service Desk
	ticketing system. It supports ticket creation, lifecycle management, and
	integration with the chat system.

	\section imtdesk_overview Overview
	- Support tickets with type, priority, status, and environment fields
	- Ticket lifecycle: Open → InProgress → Resolved → Closed
	- Linked to conversations and messages in the imtchat module

	\section imtdesk_modules Related Modules
	- \b imtdeskdb — PostgreSQL/SQLite persistence
	- \b imtdeskgql — GraphQL API
	- \b imtdeskgui — QML user interface
	- \b imtchat — Chat domain model (linked via conversation/message IDs)
*/
namespace imtdesk {}
