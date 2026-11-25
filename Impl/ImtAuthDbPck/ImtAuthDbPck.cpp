// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include "ImtAuthDbPck.h"


// ACF includes
#include <icomp/export.h>


namespace ImtAuthDbPck
{


I_EXPORT_PACKAGE(
			"ImtAuthDbPck",
			"Database-related authorization component package",
			IM_PROJECT("\"ImagingTools Core Framework\"") IM_COMPANY("ImagingTools"));

I_EXPORT_COMPONENT(
			RoleDatabaseDelegate,
			"Role object delegate for SQL table",
			"SQL Role Delegate");

I_EXPORT_COMPONENT(
			UserDatabaseDelegate,
			"User object delegate for SQL table",
			"SQL User Delegate");

I_EXPORT_COMPONENT(
			SqliteUserDatabaseDelegate,
			"User object delegate for SQLite table",
			"SQL SQLite User Delegate");

I_EXPORT_COMPONENT(
			UserGroupDatabaseDelegate,
			"User group object delegate for SQL table",
			"SQL User Group Delegate");

I_EXPORT_COMPONENT(
			SqliteUserGroupDatabaseDelegate,
			"User group object delegate for SQLite table",
			"SQL SQLite User Group Delegate");

I_EXPORT_COMPONENT(
			UsersSessionsDatabaseDelegate,
			"User sessions delegate for SQL table",
			"SQL User Session Delegate");

I_EXPORT_COMPONENT(
			UsersSettingsDatabaseDelegate,
			"User settings delegate for SQL table",
			"SQL User Settings Delegate");

I_EXPORT_COMPONENT(
			SuperuserProvider,
			"Superuser provider",
			"SQL User su Superuser SuperUser admin");

I_EXPORT_COMPONENT(
			SqlJsonRoleDatabaseDelegate,
			"Role object json delegate for SQL table",
			"SQL JSON Role Delegate");

I_EXPORT_COMPONENT(
			SqliteJsonRoleDatabaseDelegate,
			"Role object json delegate for SQLite table",
			"SQL SQLite JSON Role Delegate");

I_EXPORT_COMPONENT(
			UsersSqlPathExtractor,
			"Users sql path extractor",
			"Users Sql Path Extractor");

I_EXPORT_COMPONENT(
			UserActionDatabaseDelegate,
			"User action database delegate",
			"User Action Database Delegate");

I_EXPORT_COMPONENT(
			ProductCatalogDatabaseDelegate,
			"Product catalog and permission snapshot database delegate",
			"SQL Product Catalog Snapshot Delegate");

I_EXPORT_COMPONENT(
			PersonalAccessTokenDatabaseDelegate,
			"Personal access token database delegate",
			"Personal Access Token Database Delegate");

I_EXPORT_COMPONENT(
			ExternalIdentityDatabaseDelegate,
			"External identity link database delegate",
			"External Identity Database Delegate OIDC");

I_EXPORT_COMPONENT(
			TenantDatabaseDelegate,
			"Tenant database delegate",
			"Tenant Database Delegate");


I_EXPORT_COMPONENT(
			TenantMembershipDatabaseDelegate,
			"SQL delegate for tenant memberships (hydration, INSERT/UPDATE/DELETE)",
			"Tenant Membership Database Delegate");

I_EXPORT_COMPONENT(
			TenantEntityBindingDatabaseDelegate,
			"SQL delegate for tenant entity bindings (hydration, INSERT/DELETE)",
			"Tenant Entity Binding Database Delegate");

I_EXPORT_COMPONENT(
			TenantInvitationDatabaseDelegate,
			"SQL delegate for tenant invitations (hydration, INSERT/UPDATE/DELETE)",
			"Tenant Invitation Database Delegate");


I_EXPORT_COMPONENT(
			DocumentShareDatabaseDelegate,
			"SQL delegate for document shares between tenants",
			"Document Share Database Delegate");

I_EXPORT_COMPONENT(
			ContractDatabaseDelegate,
			"SQL delegate of the contracts and the index of their parties",
			"Contract Database Delegate");

I_EXPORT_COMPONENT(
			ContractPartyPrivateDatabaseDelegate,
			"SQL delegate of the records the parties of the contracts keep for themselves",
			"Contract Party Private Database Delegate");

I_EXPORT_COMPONENT(
			ContractEventDatabaseDelegate,
			"SQL delegate of the journals of the contracts",
			"Contract Event Database Delegate");

I_EXPORT_COMPONENT(
			PartyRoleDatabaseDelegate,
			"SQL delegate of the roles of the parties of the organizations",
			"Party Role Database Delegate");

I_EXPORT_COMPONENT(
			ContractTypeDatabaseDelegate,
			"SQL delegate of the types of contracts of the organizations",
			"Contract Type Database Delegate");

I_EXPORT_COMPONENT(
			TenantDirectoryProfileDatabaseDelegate,
			"SQL delegate of the profiles of the organizations in the directory",
			"Tenant Directory Profile Database Delegate");

I_EXPORT_COMPONENT(
			TenantConnectionRequestDatabaseDelegate,
			"SQL delegate for tenant connection requests (hydration, INSERT/UPDATE/DELETE)",
			"Tenant Connection Request Database Delegate");

I_EXPORT_COMPONENT(
			TenantConnectionCodeDatabaseDelegate,
			"SQL delegate for tenant connection codes (hydration, INSERT/UPDATE/DELETE)",
			"Tenant Connection Code Database Delegate");

I_EXPORT_COMPONENT(
			TenantConnectionDatabaseDelegate,
			"SQL delegate for tenant connections (hydration, INSERT/UPDATE/DELETE)",
			"Tenant Connection Database Delegate");


I_EXPORT_COMPONENT(
			CrossTenantMessageDatabaseDelegate,
			"SQL delegate for cross-tenant messages (hydration, INSERT/UPDATE/DELETE)",
			"Cross Tenant Message Database Delegate");

I_EXPORT_COMPONENT(
			OrderRequestDatabaseDelegate,
			"SQL delegate for order requests (hydration, INSERT/UPDATE/DELETE)",
			"Order Request Database Delegate");

I_EXPORT_COMPONENT(
			PermissionPathMigration,
			"Database migration step that rewrites permissions stored by feature id as full feature paths",
			"Permission Path Migration");

} // namespace ImtAuthDbPck

