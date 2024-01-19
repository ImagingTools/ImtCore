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
		SdlGeneralManager,
		"The manager of SDL processors and argument paresrs",
		"Schema Definition Language SDL Parser Argument General Manager")

I_EXPORT_COMPONENT(
		SdlProcessArgumentsParser,
		"The command line argument parser for processors",
		"Schema Definition Language SDL Argument Parser")

I_EXPORT_COMPONENT(
		GqlSchemaParser,
		"The parser for a human-readable GraphQL schema definition language (SDL)",
		"GQL GraphQL Schema Definition Language SDL Parser")

I_EXPORT_COMPONENT(
		SdlClassCodeGenerator,
		"The C++ class generator of SDL types",
		"Schema Definition Language SDL Class Generator")

} // namespace ImtSdlPck


