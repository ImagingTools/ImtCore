// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once

/**
	\file imttag.h
	\brief Main header file for the imttag module - Tags domain model
*/

/**
	\namespace imttag
	\brief Tags Domain Model Module

	The \b imttag module provides tags (the analogue of GitHub labels) that can be
	assigned to any entity of any collection registered as taggable.

	\section imttag_overview Overview
	- A tag is a document of the tag catalog: name, hex color, description, system flag
	- System tags are visible to every tenant and managed by the superuser only
	- Tenant tags are bound to the tenant through TenantEntityBindings
	- Assignments address an entity by the pair (EntityType, EntityId)
	- Every assignment change is recorded as a Tagged/Untagged event

	\section imttag_modules Related Modules
	- \b imttagdb — PostgreSQL/SQLite persistence
	- \b imttaggql — GraphQL API
	- \b imttaggui — QML user interface

	See Docs/Architecture/Tags/TagsArchitecture.md.
*/
namespace imttag {}


