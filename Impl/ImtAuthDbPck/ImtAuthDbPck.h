// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ImtCore includes
#include <imtauthdb/CRoleDatabaseDelegateComp.h>
#include <imtauthdb/CUserDatabaseDelegateComp.h>
#include <imtauthdb/CUserGroupDatabaseDelegateComp.h>
#include <imtauthdb/CUsersSessionsDatabaseDelegateComp.h>
#include <imtauthdb/CUsersSettingsDatabaseDelegateComp.h>
#include <imtauthdb/CSuperuserProviderComp.h>
#include <imtauthdb/CSqliteUserGroupDatabaseDelegateComp.h>
#include <imtauthdb/CSqliteUserDatabaseDelegateComp.h>
#include <imtauthdb/CSqliteRoleDatabaseDelegateComp.h>
#include <imtauthdb/CUsersSqlPathExtractorComp.h>
#include <imtauthdb/CUserActionDatabaseDelegateComp.h>
#include <imtauthdb/CPersonalAccessTokenDatabaseDelegateComp.h>


/**
	ImtAuthDbPck package
*/
namespace ImtAuthDbPck
{


typedef imtauthdb::CRoleDatabaseDelegateComp RoleDatabaseDelegate;
typedef imtauthdb::CUserDatabaseDelegateComp UserDatabaseDelegate;
typedef imtauthdb::CSqliteUserDatabaseDelegateComp SqliteUserDatabaseDelegate;
typedef imtauthdb::CUserGroupDatabaseDelegateComp UserGroupDatabaseDelegate;
typedef imtauthdb::CSqliteUserGroupDatabaseDelegateComp SqliteUserGroupDatabaseDelegate;
typedef imtauthdb::CUsersSessionsDatabaseDelegateComp UsersSessionsDatabaseDelegate;
typedef imtauthdb::CUsersSettingsDatabaseDelegateComp UsersSettingsDatabaseDelegate;
typedef imtauthdb::CSuperuserProviderComp SuperuserProvider;
typedef imtauthdb::CRoleDatabaseDelegateComp SqlJsonRoleDatabaseDelegate;
typedef imtauthdb::CSqliteRoleDatabaseDelegateComp SqliteJsonRoleDatabaseDelegate;
typedef imtauthdb::CUsersSqlPathExtractorComp UsersSqlPathExtractor;
typedef imtauthdb::CUserActionDatabaseDelegateComp UserActionDatabaseDelegate;
typedef imtauthdb::CPersonalAccessTokenDatabaseDelegateComp PersonalAccessTokenDatabaseDelegate;


} // namespace ImtAuthDbPck


