// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include "ImtAssistGqlPck.h"


// ACF includes
#include <icomp/export.h>


namespace ImtAssistGqlPck
{


I_EXPORT_PACKAGE(
			"ImtAssistGqlPck",
			"GraphQL API of the AI assistant",
			IM_PROJECT("\"ImagingTools Core Framework\"") IM_COMPANY("ImagingTools"));

I_EXPORT_COMPONENT(
			AssistantController,
			"GraphQL controller of the assistant: conversation, confirmation, cancellation",
			"Assistant AI GraphQL Controller");

I_EXPORT_COMPONENT(
			AssistantConfigurationController,
			"GraphQL controller of the assistant configuration of the tenant, for the administration",
			"Assistant AI GraphQL Configuration Administration Tenant");

I_EXPORT_COMPONENT(
			AdministrationSearchAction,
			"Assistant action: search the users, groups or roles of the organization with the rights of the user",
			"Assistant AI Action Administration Users Groups Roles Search");

I_EXPORT_COMPONENT(
			AdministrationAddUserToGroupAction,
			"Assistant action: add a user to a group of the organization after confirmation",
			"Assistant AI Action Administration Group User Change");

I_EXPORT_COMPONENT(
			AssistantConfigurationDatabaseDelegate,
			"SQL delegate of the assistant configurations of the tenants",
			"Assistant AI Configuration Database SQL Delegate");

I_EXPORT_COMPONENT(
			AssistantApiCatalog,
			"Public GraphQL API of the application from its SDL files, with the policy of the product for the assistant",
			"Assistant AI API Catalog SDL Policy");

I_EXPORT_COMPONENT(
			AssistantApiAction,
			"Assistant actions api.listCommands, api.describeCommand and api.query with the rights of the user",
			"Assistant AI Action API Query");

I_EXPORT_COMPONENT(
			AssistantApiPlanAction,
			"Assistant action api.planChanges: a plan of mutations confirmed as a whole",
			"Assistant AI Action API Plan Changes");

I_EXPORT_COMPONENT(
			AssistantChangeSetAction,
			"Assistant actions changes.list and changes.rollback: roll back a confirmed change set of documents with revisions",
			"Assistant AI Action Change Set Rollback Undo");


} // namespace ImtAssistGqlPck


