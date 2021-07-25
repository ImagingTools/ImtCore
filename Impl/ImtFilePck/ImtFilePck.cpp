#include "ImtFilePck.h"


// ACF includes
#include <icomp/export.h>


namespace ImtFilePck
{


I_EXPORT_PACKAGE(
			"ImtFilePck",
			"File management component package",
			IM_PROJECT("\"ImagingTools File Core Framework\"") IM_COMPANY("ImagingTools"));

I_EXPORT_COMPONENT(
			FileProvider,
			"File provider component",
			"File Provider");

I_EXPORT_COMPONENT(
			FileFormatConversion,
			"File format conversion",
			"File Format Conversion");


} // namespace ImtFilePck


