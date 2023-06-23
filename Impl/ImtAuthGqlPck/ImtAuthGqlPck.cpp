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
		LogoutController,
		"Logout controller",
		"Logout Controller");

I_EXPORT_COMPONENT(
		PumaRoleInfoProvider,
		"Puma role info provider",
		"Puma Role Info Provider");

I_EXPORT_COMPONENT(
		PumaUserGroupInfoProvider,
		"Puma user group info provider",
		"Puma User Group Info Provider");

I_EXPORT_COMPONENT(
		PumaSuperuserProvider,
		"Puma superuser provider for client requests",
		"Puma Superuser Provider Client");

I_EXPORT_COMPONENT(
			SimpleLoginWrap,
			"Simple login wrap",
			"Simple Login Wrap");


} // namespace ImtAuthGqlPck


