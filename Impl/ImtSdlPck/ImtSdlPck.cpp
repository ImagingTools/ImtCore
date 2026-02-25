// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
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

I_EXPORT_COMPONENT(
			SimpleFileJoiner,
			"Simple file joiner. Joins text files into a single",
			"Simple File Join Text")

I_EXPORT_COMPONENT(
			SdlProcessArgumentsParser,
			"The command line argument parser for processors",
			"Schema Definition Language SDL Argument Parser")

I_EXPORT_COMPONENT(
			QtResourceFilePersistence,
			"A Qt resource file persistence",
			"Qt Resource File Persistence")

I_EXPORT_COMPONENT(
			CodeGeneratorExecutor,
			"The central component of SDL generator (entry point)",
			"SDL Processor")


} // namespace ImtSdlPck


