// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once

/**
	\file imtnotifydb.h
	\brief Main header file for the imtnotifydb module - Notification Database Persistence Layer
*/

/**
	\namespace imtnotifydb
	\brief Notification (Activity) Database Persistence Layer Module

	The \b imtnotifydb module provides SQL persistence for the imtnotify domain
	model. \b CNotificationDbDelegateComp maps imtnotify::INotification objects to
	an explicitly-columned "Notifications" table (PostgreSQL / SQLite), enabling
	server-side filtering (recipient, read state, category), sorting and
	pagination at the database level.

	\section imtnotifydb_related Related Modules
	- \b imtnotify — domain model and INotificationService
	- \b imtnotifygql — GraphQL API and live publisher
*/
namespace imtnotifydb {}
