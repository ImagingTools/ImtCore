// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once

/**
	\file imttagdb.h
	\brief Main header file for the imttagdb module - Tags persistence
*/

/**
	\namespace imttagdb
	\brief PostgreSQL/SQLite persistence of the tag catalog, tag assignments and tag events.

	Call ImtCoreInitTagSqlResources() (imtcore/CImtCoreTagInitializer.h) in the application
	so the table creation scripts are available.
*/
namespace imttagdb {}


