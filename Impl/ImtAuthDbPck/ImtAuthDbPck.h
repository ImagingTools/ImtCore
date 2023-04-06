#pragma once


// ImtCore includes
#include <imtauthdb/CRoleDatabaseDelegateComp.h>
#include <imtauthdb/CUserDatabaseDelegateComp.h>
#include <imtauthdb/CUserGroupDatabaseDelegateComp.h>
#include <imtauthdb/CUsersSessionsDatabaseDelegateComp.h>
#include <imtauthdb/CUsersSettingsDatabaseDelegateComp.h>

/**
	ImtAuthDbPck package
*/
namespace ImtAuthDbPck
{


typedef imtauthdb::CRoleDatabaseDelegateComp RoleDatabaseDelegate;
typedef imtauthdb::CUserDatabaseDelegateComp UserDatabaseDelegate;
typedef imtauthdb::CUserGroupDatabaseDelegateComp UserGroupDatabaseDelegate;
typedef imtauthdb::CUsersSessionsDatabaseDelegateComp UsersSessionsDatabaseDelegate;
typedef imtauthdb::CUsersSettingsDatabaseDelegateComp UsersSettingsDatabaseDelegate;


} // namespace ImtAuthDbPck


