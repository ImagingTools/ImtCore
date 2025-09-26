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
		RemoteRoleInfoProvider,
		"Remote role info provider",
		"Remote Role Info Provider");

I_EXPORT_COMPONENT(
		RemoteUserGroupInfoProvider,
		"Remote user group info provider",
		"Remote User Group Info Provider");

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


} // namespace ImtAuthGqlPck


