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
		DesignTokenArgumentsParser,
		"Design token arguments parser",
		"Design Token Arguments Parser");

I_EXPORT_COMPONENT(
		DesignTokenFileParser,
		"Design token file parser",
		"Design Token File Parser");

I_EXPORT_COMPONENT(
		DesignTokenProcessorsController,
		"Controls and lauches all processors",
		"Design Token StyleSheet Style Sheet Processor");

I_EXPORT_COMPONENT(
			DesignTokenIconProcessor,
			"Creates icons for theme schems",
			"Design Token Icon Processor");

I_EXPORT_COMPONENT(
		DesignTokenStyleSheetProcessor,
		"Creates style sheets for theme schems",
		"Design Token StyleSheet Style Sheet Processor");

I_EXPORT_COMPONENT(
		DesignTokenSplitProcessor,
		"Splits single file for each theme scheme",
		"Design Token Split Processor");

I_EXPORT_COMPONENT(
		DesignTokenQrcUpdater,
		"Modifyes QRC files",
		"Design Token QRC");




} // namespace ImtStylePck


