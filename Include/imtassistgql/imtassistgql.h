// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once

/**
	\file imtassistgql.h
	\brief Main header file for the imtassistgql module - GraphQL API of the AI assistant
*/

/**
	\namespace imtassistgql
	\brief GraphQL API of the AI assistant

	The \b imtassistgql module connects the assistant service of \b imtassist to the
	GraphQL servlet of ImtCore. The request context (user, tenant) is taken from the
	authenticated GraphQL context, never from the request payload.

	\section imtassistgql_overview Overview
	- \b CAssistantControllerComp — commands AssistantStartConversation, AssistantSendMessage,
	  AssistantConfirmPreview, AssistantRejectPreview, AssistantCancelRequest,
	  AssistantGetHistory and AssistantCloseConversation

	\section imtassistgql_related Related Modules
	- \b imtassist — assistant core
	- \b imtassistgui — QML panel

	\see Docs/Assistant/README.md
*/
namespace imtassistgql {}
