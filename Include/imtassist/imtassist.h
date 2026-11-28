// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once

/**
	\file imtassist.h
	\brief Main header file for the imtassist module - generic AI assistant core
*/

/**
	\namespace imtassist
	\brief Generic AI assistant core module

	The \b imtassist module provides the provider-independent server-side core of
	the ImtCore AI assistant. Applications register their domain functions as
	actions; the assistant can only call registered actions in the context of the
	authenticated user.

	\section imtassist_overview Overview
	- Action registry with versioned parameter schemas, classified as reading or changing
	- Changing actions are split into preparation (bound, time-limited preview) and
	  execution after an explicit confirmation of exactly this preview
	- Provider adapter interface with a deterministic fake provider for tests, and HTTP adapters
	  for OpenAI-compatible APIs, Anthropic, Gemini and Amazon Bedrock with presets of the common providers
	- Registry of known UI representation types; unknown types are rejected
	- Typed, versioned messages exchanged with the UI
	- Disabled by default: without enabling and a configured provider no provider call is made

	The module is independent of \b imtchat (Service Desk chat); conversations are
	kept in memory for the running session only.

	\section imtassist_modules Related Modules
	- \b imtassistgql — GraphQL API
	- \b imtassistgui — QML panel

	\see Docs/Assistant/README.md
*/
namespace imtassist {}
