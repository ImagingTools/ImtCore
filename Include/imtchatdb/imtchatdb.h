// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once

/**
	\file imtchatdb.h
	\brief Main header file for the imtchatdb module - Chat Database Persistence Layer
*/

/**
	\namespace imtchatdb
	\brief Chat Database Persistence Layer Module

	The \b imtchatdb module provides SQL database persistence for the imtchat
	domain model. It implements database delegate components that handle
	conversation, message, ticket, participant, and message-status storage
	in PostgreSQL or SQLite databases.

	\section imtchatdb_overview Overview

	This module acts as the persistence layer for imtchat, implementing database
	delegates that handle:
	- Conversation creation and retrieval
	- Message storage with full-text search support (PostgreSQL)
	- Per-user message delivery/read status tracking
	- Conversation participant management
	- Service Desk ticket CRUD operations

	\section imtchatdb_delegates Database Delegates

	| Component                       | Table                    |
	|---------------------------------|--------------------------|
	| CConversationDbDelegateComp     | Conversations            |
	| CMessageDbDelegateComp          | Messages                 |
	| CMessageStatusDbDelegateComp    | MessageStatus            |
	| CParticipantDbDelegateComp      | ConversationParticipants |
	| CTicketDbDelegateComp           | Tickets                  |

	\section imtchatdb_databases Supported Databases
	- \b PostgreSQL — primary production database; uses UUID and JSONB types
	- \b SQLite — embedded / offline database; uses TEXT for IDs and JSON
*/
namespace imtchatdb {}
