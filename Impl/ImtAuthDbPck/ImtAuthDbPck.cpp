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
			UserGroupDatabaseDelegate,
			"User group object delegate for SQL table",
			"SQL User Group Delegate");

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


} // namespace ImtAuthDbPck


