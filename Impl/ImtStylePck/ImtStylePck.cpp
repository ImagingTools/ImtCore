#include "ImtStylePck.h"


// ACF includes
#include <icomp/export.h>


namespace ImtStylePck
{


I_EXPORT_PACKAGE(
			"ImtStylePck",
			"Design style package",
			IM_PROJECT("\"ImagingTools Core Framework\"") IM_COMPANY("ImagingTools"));

I_EXPORT_COMPONENT(
			DesignTokenBasedResourceProvider,
			"Design token based resource provider",
			"Design Token Resource Color Palette Font Icons");

I_EXPORT_COMPONENT(
			DesignTokenProcessor,
			"Creates theme schems",
			"Design Token Processor");

I_EXPORT_COMPONENT(
		DesignTokenArgumentsParser,
		"Creates theme schems",
		"Design Token Arguments Parser");

I_EXPORT_COMPONENT(
		DesignTokenFileParser,
		"Design token file parser",
		"Design Token File Parser");


} // namespace ImtStylePck


