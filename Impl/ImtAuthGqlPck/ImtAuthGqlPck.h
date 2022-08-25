#pragma once


// ACF includes
#include <icomp/TMakeComponentWrap.h>
#include <icomp/TModelCompWrap.h>

// ImtCore includes
#include <imtauthgql/CAuthorizationControllerComp.h>
#include <imtauthgql/CRoleCollectionControllerComp.h>
#include <imtauthgql/CRoleControllerComp.h>


/**
	ImtAuthGqlPck package
*/
namespace ImtAuthGqlPck
{


typedef imtauthgql::CAuthorizationControllerComp AuthorizationController;
typedef imtauthgql::CRoleCollectionControllerComp RoleCollectionController;
typedef imtauthgql::CRoleControllerComp RoleController;



} // namespace ImtAuthGqlPck


