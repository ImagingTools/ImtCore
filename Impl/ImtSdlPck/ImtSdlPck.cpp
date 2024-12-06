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
			CacheController,
			"The cache controller, used to operate a cache, save and load",
			"Schema Definition Language SDL Cache Controller")

I_EXPORT_COMPONENT(
			CacheMultiManager,
			"The cache multi manager, used to resolve paths for types ",
			"Schema Definition Language SDL Cache Manager")


} // namespace ImtSdlPck


