// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once

/**
	\file imtchatgql.h
	\brief Main header file for the imtchatgql module - Chat GraphQL API Layer
*/

/**
	\namespace imtchatgql
	\brief Chat GraphQL API Layer Module

	The \b imtchatgql module bridges the imtchat domain model with the GraphQL
	API defined in the ImtChat SDL schema. It provides collection and object
	controller components that translate between C++ domain objects and
	SDL-generated GraphQL representation types.

	\section imtchatgql_overview Overview

	- \b CConversationCollectionControllerComp — handles ConversationsList / ConversationItem queries,
	  ConversationCreate and ConversationUpdate mutations
	- \b CMessageCollectionControllerComp — handles MessagesList / MessageItem queries,
	  MessageSend, MessageUpdate and MessageMarkRead mutations
	- \b CTicketCollectionControllerComp — handles TicketsList / TicketItem queries,
	  TicketCreate and TicketUpdate mutations

	\section imtchatgql_related Related Modules
	- \b imtchat — domain model
	- \b imtchatdb — database persistence
	- \b imtchatgui — QML user interface
*/
namespace imtchatgql {}
