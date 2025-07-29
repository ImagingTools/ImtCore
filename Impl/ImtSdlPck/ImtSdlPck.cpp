#include "ImtSdlPck.h"


// ACF includes
#include <icomp/export.h>


namespace ImtSdlPck
{


I_EXPORT_PACKAGE(
			"ImtSdlPck",
			"The Schema Definition Language (SDL) package",
			IM_PROJECT("\"ImagingTools Core Framework\"") IM_COMPANY("ImagingTools") IM_AUTHOR("Sergey Zastrovnyh"));

I_EXPORT_COMPONENT(
			GqlSchemaParser,
			"The parser for a human-readable GraphQL schema definition language (SDL)",
			"GQL GraphQL Schema Definition Language SDL Parser")

I_EXPORT_COMPONENT(
			BasicRequestsProxyProvider,
			"The requests provider, that filters collection handler requests",
			"GQL GraphQL Schema Definition Language SDL Parser")

I_EXPORT_COMPONENT(
			LogFilter,
			"The SDL specific log filter, used to remove unnecessary messages",
			"Schema Definition Language SDL Log Filter")


} // namespace ImtSdlPck


