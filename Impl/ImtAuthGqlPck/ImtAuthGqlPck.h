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
#include <imtauthgql/CRemoteRoleInfoProviderComp.h>
#include <imtauthgql/CRemoteUserGroupInfoProviderComp.h>
#include <imtauthgql/CClientRequestRemoteSuperuserProviderComp.h>
#include <imtauthgql/CRemoteSuperuserControllerComp.h>
#include <imtauthgql/CSimpleLoginWrapComp.h>
#include <imtauthgql/CCollectionUserInfoProviderComp.h>
#include <imtauthgql/CRemotePermissionCheckerComp.h>
#include <imtauthgql/CSessionModelObserverComp.h>
#include <imtauthgql/CSessionModelSubscriberControllerComp.h>
#include <imtauthgql/CLdapAuthorizationControllerComp.h>
#include <imtauthgql/CUserSerializableCollectionControllerComp.h>
#include <imtauthgql/CUserSettingsControllerComp.h>
#include <imtauthgql/CProfileControllerComp.h>
#include <imtauthgql/CRoleMetaInfoControllerProxyComp.h>
#include <imtauthgql/CRemoteJwtSessionControllerComp.h>
#include <imtauthgql/CGqlJwtSessionControllerComp.h>


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
typedef imtauthgql::CRemoteRoleInfoProviderComp RemoteRoleInfoProvider;
typedef imtauthgql::CRemoteUserGroupInfoProviderComp RemoteUserGroupInfoProvider;
typedef imtauthgql::CClientRequestRemoteSuperuserProviderComp ClientRequestRemoteSuperuserProvider;
typedef imtauthgql::CRemoteSuperuserControllerComp RemoteSuperuserController;
typedef imtauthgql::CCollectionUserInfoProviderComp CollectionUserInfoProvider;
typedef icomp::TModelCompWrap<imtauthgql::CSimpleLoginWrapComp> SimpleLoginWrap;
typedef imtauthgql::CRemotePermissionCheckerComp RemotePermissionChecker;
typedef imtauthgql::CSessionModelObserverComp SessionModelObserver;
typedef imtauthgql::CSessionModelSubscriberControllerComp SessionModelSubscriberController;
typedef imtauthgql::CLdapAuthorizationControllerComp LdapAuthorizationController;
typedef imtauthgql::CUserSerializableCollectionControllerComp UserSerializableCollectionController;
typedef imtauthgql::CUserSettingsControllerComp UserSettingsController;
typedef imtauthgql::CProfileControllerComp ProfileController;
typedef imtauthgql::CRoleMetaInfoControllerProxyComp RoleMetaInfoControllerProxy;
typedef imtauthgql::CRemoteJwtSessionControllerComp RemoteJwtSessionController;
typedef imtauthgql::CGqlJwtSessionControllerComp GqlJwtSessionController;


} // namespace ImtAuthGqlPck


