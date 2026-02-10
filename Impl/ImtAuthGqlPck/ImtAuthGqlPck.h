// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ACF includes
#include <icomp/TMakeComponentWrap.h>
#include <icomp/TModelCompWrap.h>

// ImtCore includes
#include <imtauthgql/CAuthorizationControllerComp.h>
#include <imtauthgql/CRoleCollectionControllerComp.h>
#include <imtauthgql/CUserCollectionControllerComp.h>
#include <imtauthgql/CUserControllerComp.h>
#include <imtauthgql/CUserGroupCollectionControllerComp.h>
#include <imtauthgql/CAuthorizationOptionsControllerComp.h>
#include <imtauthgql/CSessionControllerComp.h>
#include <imtauthgql/CClientRequestRemoteSuperuserProviderComp.h>
#include <imtauthgql/CRemoteSuperuserControllerComp.h>
#include <imtauthgql/CSimpleLoginWrapComp.h>
#include <imtauthgql/CRemotePermissionCheckerComp.h>
#include <imtauthgql/CSessionModelObserverComp.h>
#include <imtauthgql/CSessionModelSubscriberControllerComp.h>
#include <imtauthgql/CLdapAuthorizationControllerComp.h>
#include <imtauthgql/CUserSerializableCollectionControllerComp.h>
#include <imtauthgql/CUserSettingsControllerComp.h>
#include <imtauthgql/CProfileControllerComp.h>
#include <imtauthgql/CRoleRemoteCollectionControllerComp.h>
#include <imtauthgql/CRemoteJwtSessionControllerComp.h>
#include <imtauthgql/CGqlJwtSessionControllerComp.h>
#include <imtauthgql/CRoleMetaInfoDelegateComp.h>
#include <imtauthgql/CUserMetaInfoDelegateComp.h>
#include <imtauthgql/CGroupMetaInfoDelegateComp.h>
#include <imtauthgql/CRemoteUserControllerComp.h>
#include <imtauthgql/CRemoteAuthorizationControllerComp.h>
#include <imtauthgql/CUserActionCollectionControllerComp.h>
#include <imtauthgql/CUserActionMetaInfoDelegateComp.h>
#include <imtauthgql/CSimpleLoginComp.h>
#include <imtauthgql/CClientRequestRoleManagerComp.h>
#include <imtauthgql/CClientRequestUserManagerComp.h>
#include <imtauthgql/CClientRequestGroupManagerComp.h>
#include <imtauthgql/CClientRequestUserInfoProviderComp.h>
#include <imtauthgql/CClientRequestRoleInfoProviderComp.h>
#include <imtauthgql/CClientRequestGroupInfoProviderComp.h>
#include <imtauthgql/CTokenBasedPermissionsProviderComp.h>
#include <imtauthgql/CPersonalAccessTokenControllerComp.h>
#include <imtauthgql/CClientRequestPersonalAccessTokenManagerComp.h>


/**
	ImtAuthGqlPck package
*/
namespace ImtAuthGqlPck
{


typedef imtauthgql::CAuthorizationControllerComp AuthorizationController;
typedef imtauthgql::CRoleCollectionControllerComp RoleCollectionController;
typedef imtauthgql::CUserCollectionControllerComp UserCollectionController;
typedef imtauthgql::CUserControllerComp UserController;
typedef imtauthgql::CUserGroupCollectionControllerComp UserGroupCollectionController;
typedef imtauthgql::CAuthorizationOptionsControllerComp AuthorizationOptionsController;
typedef imtauthgql::CSessionControllerComp SessionController;
typedef imtauthgql::CClientRequestRemoteSuperuserProviderComp ClientRequestRemoteSuperuserProvider;
typedef imtauthgql::CRemoteSuperuserControllerComp RemoteSuperuserController;
typedef icomp::TModelCompWrap<imtauthgql::CSimpleLoginWrapComp> SimpleLoginWrap;
typedef imtauthgql::CRemotePermissionCheckerComp RemotePermissionChecker;
typedef imtauthgql::CSessionModelObserverComp SessionModelObserver;
typedef imtauthgql::CSessionModelSubscriberControllerComp SessionModelSubscriberController;
typedef imtauthgql::CLdapAuthorizationControllerComp LdapAuthorizationController;
typedef imtauthgql::CUserSerializableCollectionControllerComp UserSerializableCollectionController;
typedef imtauthgql::CUserSettingsControllerComp UserSettingsController;
typedef imtauthgql::CProfileControllerComp ProfileController;
typedef imtauthgql::CRoleRemoteCollectionControllerComp RoleRemoteCollectionController;
typedef imtauthgql::CRemoteJwtSessionControllerComp RemoteJwtSessionController;
typedef imtauthgql::CGqlJwtSessionControllerComp GqlJwtSessionController;
typedef imtauthgql::CRoleMetaInfoDelegateComp RoleMetaInfoDelegate;
typedef imtauthgql::CUserMetaInfoDelegateComp UserMetaInfoDelegate;
typedef imtauthgql::CGroupMetaInfoDelegateComp GroupMetaInfoDelegate;
typedef imtauthgql::CRemoteUserControllerComp RemoteUserController;
typedef imtauthgql::CRemoteAuthorizationControllerComp RemoteAuthorizationController;
typedef imtauthgql::CUserActionCollectionControllerComp UserActionCollectionController;
typedef imtauthgql::CUserActionMetaInfoDelegateComp UserActionMetaInfoDelegate;
typedef icomp::TModelCompWrap<imtauthgql::CSimpleLoginComp> SimpleLogin;
typedef imtauthgql::CClientRequestRoleManagerComp ClientRequestRoleManager;
typedef imtauthgql::CClientRequestUserManagerComp ClientRequestUserManager;
typedef imtauthgql::CClientRequestGroupManagerComp ClientRequestGroupManager;
typedef imtauthgql::CClientRequestUserInfoProviderComp ClientRequestUserInfoProvider;
typedef imtauthgql::CClientRequestRoleInfoProviderComp ClientRequestRoleInfoProvider;
typedef imtauthgql::CClientRequestGroupInfoProviderComp ClientRequestGroupInfoProvider;
typedef imtauthgql::CTokenBasedPermissionsProviderComp ClientRequestTokenBasedPermissionsProvider;
typedef imtauthgql::CPersonalAccessTokenControllerComp PersonalAccessTokenController;
typedef imtauthgql::CClientRequestPersonalAccessTokenManagerComp ClientRequestPersonalAccessTokenManager;


} // namespace ImtAuthGqlPck


