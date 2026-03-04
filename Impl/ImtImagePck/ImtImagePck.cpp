// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include "ImtImagePck.h"


// ACF includes
#include <icomp/export.h>


namespace ImtImagePck
{


I_EXPORT_PACKAGE(
			"ImtImagePck",
			"ImagingTools image component package",
			IM_PROJECT("\"ImagingTools Core Framework\"") IM_COMPANY("ImagingTools"));

I_EXPORT_COMPONENT(
			BitmapMetaInfoCreator,
			"Creator of the meta informations for the bitmaps",
			"Bitmap Image Meta Description Information");


} // namespace ImtImagePck


