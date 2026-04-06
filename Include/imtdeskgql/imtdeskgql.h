// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once

/**
	\file imtdeskgql.h
	\brief Main header file for the imtdeskgql module - Service Desk GraphQL API Layer
*/

/**
	\namespace imtdeskgql
	\brief Service Desk GraphQL API Layer Module

	The \b imtdeskgql module bridges the imtdesk domain model with the GraphQL
	API defined in the ImtDesk SDL schema. It provides collection and object
	controller components that translate between C++ domain objects and
	SDL-generated GraphQL representation types.

	\section imtdeskgql_overview Overview

	- \b CTicketCollectionControllerComp — handles TicketsList / TicketItem queries,
	  TicketCreate and TicketUpdate mutations
	- \b CTicketCollectionDocumentManagerComp — handles GetTicketRepresentation query,
	  UpdateTicketFromRepresentation mutation for the document editor

	\section imtdeskgql_related Related Modules
	- \b imtdesk — domain model
	- \b imtdeskdb — database persistence
	- \b imtdeskgui — QML user interface
*/
namespace imtdeskgql {}
