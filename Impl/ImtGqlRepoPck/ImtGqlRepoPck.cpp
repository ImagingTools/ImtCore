#include "ImtGqlRepoPck.h"


// ACF includes
#include <icomp/export.h>


namespace ImtGqlRepoPck
{


I_EXPORT_PACKAGE(
			"ImtGqlRepoPck",
			"GraphQL repository components package",
			IM_PROJECT("\"ImagingTools Core Framework\"") IM_COMPANY("ImagingTools") IM_AUTHOR("Sergey Zastrovnyh"));

I_EXPORT_COMPONENT(
			GqlFileRepository,
			"File-based GraphQL collection repository",
			IM_AUTHOR("Sergey Zastrovnyh")"File Repository GQL GraphQL Collection Object Provider");


} // namespace ImtGqlRepoPck


