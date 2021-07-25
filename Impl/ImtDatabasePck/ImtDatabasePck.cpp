#include "ImtDatabasePck.h"


// ACF includes
#include <icomp/export.h>


namespace ImtDatabasePck
{


I_EXPORT_PACKAGE(
			"ImtDatabasePck",
			"Database-related component package",
			IM_PROJECT("\"ImagingTools Core Framework\"") IM_COMPANY("ImagingTools"));


I_EXPORT_COMPONENT(
			SqlDatabaseEngine,
			"SQL-based database engine",
			"SQL Database");

I_EXPORT_COMPONENT(
			SqlDatabaseCollection,
			"SQL-based object collection",
			"SQL Database");


} // namespace ImtDatabasePck


