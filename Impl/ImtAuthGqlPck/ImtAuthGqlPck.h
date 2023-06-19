#pragma once


// ACF includes
#include <icomp/TMakeComponentWrap.h>
#include <icomp/TModelCompWrap.h>

// ImtCore includes
#include <imtauthgql/CAuthorizationControllerComp.h>
#include <imtauthgql/CRoleCollectionControllerComp.h>
#include <imtauthgql/CRoleControllerComp.h>
#include <imtauthgql/CUserCollectionControllerComp.h>
#include <imtauthgql/CUserControllerComp.h>
#include <imtauthgql/CUserGroupControllerComp.h>
#include <imtauthgql/CUserGroupCollectionControllerComp.h>
#include <imtauthgql/CAuthorizationOptionsControllerComp.h>
#include <imtauthgql/CSessionControllerComp.h>
#include <imtauthgql/CLogoutControllerComp.h>


/**
	ImtAuthGqlPck package
*/
namespace ImtAuthGqlPck
{


typedef imtauthgql::CAuthorizationControllerComp AuthorizationController;
typedef imtauthgql::CRoleCollectionControllerComp RoleCollectionController;
typedef imtauthgql::CRoleControllerComp RoleController;
typedef imtauthgql::CUserCollectionControllerComp UserCollectionController;
typedef imtauthgql::CUserControllerComp UserController;
typedef imtauthgql::CUserGroupControllerComp UserGroupController;
typedef imtauthgql::CUserGroupCollectionControllerComp UserGroupCollectionController;
typedef imtauthgql::CAuthorizationOptionsControllerComp AuthorizationOptionsController;
typedef imtauthgql::CSessionControllerComp SessionController;
typedef imtauthgql::CLogoutControllerComp LogoutController;


} // namespace ImtAuthGqlPck


