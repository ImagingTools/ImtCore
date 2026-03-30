// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once

/**
	\file imtdeskdb.h
	\brief Main header file for the imtdeskdb module - Service Desk Database Persistence Layer
*/

/**
	\namespace imtdeskdb
	\brief Service Desk Database Persistence Layer Module

	The \b imtdeskdb module provides SQL database persistence for the imtdesk
	domain model. It implements database delegate components that handle
	ticket storage in PostgreSQL or SQLite databases.

	\section imtdeskdb_delegates Database Delegates

	| Component                          | Table    |
	|------------------------------------|----------|
	| CSupportTicketDbDelegateComp       | Tickets  |

	\section imtdeskdb_databases Supported Databases
	- \b PostgreSQL — primary production database; uses UUID and JSONB types
	- \b SQLite — embedded / offline database; uses TEXT for IDs and JSON
*/
namespace imtdeskdb {}
