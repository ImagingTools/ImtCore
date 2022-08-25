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




} // namespace ImtAuthDbPck


