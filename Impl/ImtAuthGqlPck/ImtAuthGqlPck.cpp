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
		RoleController,
		"RoleController",
		"Role Controller");

I_EXPORT_COMPONENT(
		UserCollectionController,
		"User collection controller",
		"User Collection Controller");

I_EXPORT_COMPONENT(
		UserController,
		"UserController",
		"User Controller");

I_EXPORT_COMPONENT(
		UserGroupController,
		"Group of the users",
		"User Group Controller");

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
		CollectionUserInfoProvider,
		"Collection user info provider",
		"Collection UserInfo User Provider");

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
			RoleMetaInfoControllerProxy,
			"Role meta info controller proxy",
			"Role Meta Info Controller Proxy");


} // namespace ImtAuthGqlPck


