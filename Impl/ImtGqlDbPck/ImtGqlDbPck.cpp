#include "ImtGqlDbPck.h"


// ACF includes
#include <icomp/export.h>


namespace ImtGqlDbPck
{


I_EXPORT_PACKAGE(
			"ImtGqlDbPck",
			"GraphQL component package with database implementation",
			IM_PROJECT("\"ImagingTools Core Framework\"") IM_COMPANY("ImagingTools"));

I_EXPORT_COMPONENT(
			StructureController,
			"GraphQL-structure base controller",
			"GQL GraphQL Structure Controller");


} // namespace ImtGqlDbPck


