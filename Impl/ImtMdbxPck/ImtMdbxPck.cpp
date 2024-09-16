#include "ImtMdbxPck.h"


// ACF includes
#include <icomp/export.h>


namespace ImtMdbxPck
{


I_EXPORT_PACKAGE(
			"ImtDatabasePck",
			"Database-related component package",
			IM_PROJECT("\"ImagingTools Core Framework\"") IM_COMPANY("ImagingTools"));

I_EXPORT_COMPONENT(
            MdbxDatabaseEngine,
            "Mdbx-based database engine",
            "Mdbx Database Engine");


} // namespace ImtMdbxPck


