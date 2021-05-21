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
			DesignTokenBasedPaletteProvider,
			"Design token based color palette provider",
			"Design Token Provider Color Palette");

} // namespace ImtStylePck


