// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include "ImtAuthGqlPck.h"


// ACF includes
#include <icomp/export.h>


namespace ImtAuthGqlPck
{


I_EXPORT_PACKAGE(
		"ImtAuthGqlPck",
		"ImagingTools core framework GraphQL-component package",
		IM_PROJECT("\"ImagingTools Core Framework\"") IM_COMPANY("ImagingTools") "GraphQL");

I_EXPORT_COMPONENT(
		AuthorizationController,
		"AuthorizationController",
		"AuthorizationController");

I_EXPORT_COMPONENT(
		RoleCollectionController,
		"RoleCollectionController",
		"Role Collection Controller");

I_EXPORT_COMPONENT(
		UserCollectionController,
		"User collection controller",
		"User Collection Controller");

I_EXPORT_COMPONENT(
		UserController,
		"UserController",
		"User Controller");

I_EXPORT_COMPONENT(
		UserGroupCollectionController,
		"User group collection controller",
		"User Group Collection Controller");

I_EXPORT_COMPONENT(
		AuthorizationOptionsController,
		"AuthorizationOptionsController",
		"Authorization Options Controller");

I_EXPORT_COMPONENT(
		SessionController,
		"Session controller",
		"Session Controller");

I_EXPORT_COMPONENT(
		ClientRequestRemoteSuperuserProvider,
		"Remote superuser provider for client requests",
		"Remote Superuser Provider Client");

I_EXPORT_COMPONENT(
		RemoteSuperuserController,
		"Puma superuser controller",
		"Puma Superuser Controller");

I_EXPORT_COMPONENT(
			SimpleLoginWrap,
			"Simple login wrap",
			"Simple Login Wrap");

I_EXPORT_COMPONENT(
			RemotePermissionChecker,
			"Remote permission ckecker",
			"Remote Permission Checker");

I_EXPORT_COMPONENT(
			SessionModelObserver,
			"Session model observer",
			"Session Model Observer");

I_EXPORT_COMPONENT(
			SessionModelSubscriberController,
			"Session subscriber controller",
			"Session Subscriber Subscription Controller");

I_EXPORT_COMPONENT(
			LdapAuthorizationController,
			"LDAP authorization controller",
			"LDAP Authorization Controller");

I_EXPORT_COMPONENT(
			UserSerializableCollectionController,
			"User serializable collection controller",
			"User Serializable Collection Controller");

I_EXPORT_COMPONENT(
			UserSettingsController,
			"Client settings controller",
			"GraphQL Controller Model Client User");

I_EXPORT_COMPONENT(
			ProfileController,
			"Profile controller",
			"Profile Controller");

I_EXPORT_COMPONENT(
			RoleRemoteCollectionController,
			"Role remote collection controller",
			"Role Remote Collection Controller");

I_EXPORT_COMPONENT(
			RemoteJwtSessionController,
			"Remote JWT session controller",
			"Remote Jwt Session Controller");

I_EXPORT_COMPONENT(
			GqlJwtSessionController,
			"GraphQL JWT session controller",
			"GraphQL GQL Jwt Session Controller");

I_EXPORT_COMPONENT(
			RoleMetaInfoDelegate,
			"Role meta info delegate",
			"Role Meta Info Delegate");

I_EXPORT_COMPONENT(
			UserMetaInfoDelegate,
			"User meta info delegate",
			"User Meta Info Delegate");

I_EXPORT_COMPONENT(
			GroupMetaInfoDelegate,
			"Group meta info delegate",
			"Group Meta Info Delegate");

I_EXPORT_COMPONENT(
			RemoteUserController,
			"Remote user controller",
			"Remote User Controller");

I_EXPORT_COMPONENT(
			RemoteAuthorizationController,
			"Remote authorization controller",
			"Remote Authorization Controller");

I_EXPORT_COMPONENT(
			UserActionCollectionController,
			"User action collection controller",
			"User Action Collection Controller");

I_EXPORT_COMPONENT(
			UserActionMetaInfoDelegate,
			"User action meta info delegate",
			"User Action Meta Info Delegate");

I_EXPORT_COMPONENT(
			SimpleLogin,
			"Simple login",
			"Simple Login");

I_EXPORT_COMPONENT(
			ClientRequestRoleManager,
			"Client request role manager",
			"Client Request Role Manager");

I_EXPORT_COMPONENT(
			ClientRequestUserManager,
			"Client request user manager",
			"Client Request User Manager");

I_EXPORT_COMPONENT(
			ClientRequestGroupManager,
			"Client request group manager",
			"Client Request Group Manager");

I_EXPORT_COMPONENT(
			ClientRequestUserInfoProvider,
			"Client request user info provider",
			"Client Request User Info Provider");

I_EXPORT_COMPONENT(
			ClientRequestRoleInfoProvider,
			"Client request role info provider",
			"Client Request Role Info Provider");

I_EXPORT_COMPONENT(
			ClientRequestGroupInfoProvider,
			"Client request group info provider",
			"Client Request Group Info Provider");

I_EXPORT_COMPONENT(
			ClientRequestTokenBasedPermissionsProvider,
			"Client request token based permissions provider",
			"Client Request Token Permissions Provider");

I_EXPORT_COMPONENT(
			PersonalAccessTokenController,
			"Personal access token controllerr",
			"Personal Access Token Controller");

I_EXPORT_COMPONENT(
			ClientRequestPersonalAccessTokenManager,
			"Client request personal access token manager",
			"Client Request Personal Access Token Manager");

I_EXPORT_COMPONENT(
			TenantCollectionController,
			"Tenant collection controller",
			"Tenant Collection Controller");

I_EXPORT_COMPONENT(
			TenantMetaInfoDelegate,
			"Tenant meta info delegate",
			"Tenant Meta Info Delegate");

I_EXPORT_COMPONENT(
			TenantCollectionDocumentService,
			"Tenant collection document manager",
			"Tenant Collection Document Manager");

I_EXPORT_COMPONENT(
			TenantCollectionDocumentServiceController,
			"Tenant collection document service controller with tenant access checks",
			"Tenant Collection Document Service Controller");

I_EXPORT_COMPONENT(
			TenantExclusionParamsSetJoiner,
			"Params set joiner that applies tenant filter in Exclude mode",
			"Tenant Exclusion Params Set Joiner");

I_EXPORT_COMPONENT(
			RemoteTenantController,
			"Remote tenant controller",
			"Remote Tenant Controller");

I_EXPORT_COMPONENT(
			RemoteTenantMembershipManagerController,
			"Remote tenant membership manager controller",
			"Remote Tenant Membership Manager Controller");

I_EXPORT_COMPONENT(
			RemoteProfileController,
			"Remote profile controller",
			"Remote Profile Controller");

I_EXPORT_COMPONENT(
			JwtSessionControllerProxy,
			"JWT session controller proxy",
			"JWT Session Controller Proxy");

I_EXPORT_COMPONENT(
			TenantManagerController,
			"Tenant manager controller",
			"Tenant Manager Controller");


I_EXPORT_COMPONENT(
			TenantMembershipManagerController,
			"GraphQL handler for tenant membership queries and mutations",
			"Tenant Membership Manager Controller");

I_EXPORT_COMPONENT(
			TenantMembershipPublisher,
			"Publisher for tenant membership invitation/response notifications via WebSocket",
			"Tenant Membership Publisher");

I_EXPORT_COMPONENT(
			RemoteTenantCollectionDocumentService,
			"Remote tenant collection document service with product permissions enrichment",
			"Remote Tenant Collection Document Service");

I_EXPORT_COMPONENT(
			RoleCollectionDocumentService,
			"Role collection document service: GetRoleRepresentation / UpdateRoleFromRepresentation",
			"Role Collection Document Service");

I_EXPORT_COMPONENT(
			GroupCollectionDocumentService,
			"Group collection document service: GetGroupRepresentation / UpdateGroupFromRepresentation",
			"Group Collection Document Service");

I_EXPORT_COMPONENT(
			UserCollectionDocumentService,
			"User collection document service: GetUserRepresentation / UpdateUserFromRepresentation",
			"User Collection Document Service");


} // namespace ImtAuthGqlPck


