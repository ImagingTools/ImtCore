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


