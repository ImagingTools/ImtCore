#include "ImtAuthDbPck.h"


// ACF includes
#include <icomp/export.h>


namespace ImtAuthDbPck
{


I_EXPORT_PACKAGE(
			"ImtAuthDbPck",
			"Database-related authorization component package",
			IM_PROJECT("\"ImagingTools Core Framework\"") IM_COMPANY("ImagingTools"));

I_EXPORT_COMPONENT(
			RoleDatabaseDelegate,
			"Role object delegate for SQL table",
			"SQL Role Delegate");

I_EXPORT_COMPONENT(
			UserDatabaseDelegate,
			"User object delegate for SQL table",
			"SQL User Delegate");

I_EXPORT_COMPONENT(
			SqliteUserDatabaseDelegate,
			"User object delegate for SQLite table",
			"SQL SQLite User Delegate");

I_EXPORT_COMPONENT(
			UserGroupDatabaseDelegate,
			"User group object delegate for SQL table",
			"SQL User Group Delegate");

I_EXPORT_COMPONENT(
			SqliteUserGroupDatabaseDelegate,
			"User group object delegate for SQLite table",
			"SQL SQLite User Group Delegate");

I_EXPORT_COMPONENT(
			UsersSessionsDatabaseDelegate,
			"User sessions delegate for SQL table",
			"SQL User Session Delegate");

I_EXPORT_COMPONENT(
			UsersSettingsDatabaseDelegate,
			"User settings delegate for SQL table",
			"SQL User Settings Delegate");

I_EXPORT_COMPONENT(
			SuperuserProvider,
			"Superuser provider",
			"SQL User su Superuser SuperUser admin");

I_EXPORT_COMPONENT(
			SqlJsonRoleDatabaseDelegate,
			"Role object json delegate for SQL table",
			"SQL JSON Role Delegate");

I_EXPORT_COMPONENT(
			SqliteJsonRoleDatabaseDelegate,
			"Role object json delegate for SQLite table",
			"SQL SQLite JSON Role Delegate");

I_EXPORT_COMPONENT(
			UsersSqlPathExtractor,
			"Users sql path extractor",
			"Users Sql Path Extractor");


} // namespace ImtAuthDbPck


