// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
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


