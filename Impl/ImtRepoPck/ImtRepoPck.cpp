#include "ImtRepoPck.h"


// ACF includes
#include <icomp/export.h>


namespace ImtRepoPck
{


I_EXPORT_PACKAGE(
			"ImtRepoPck",
			"ImagingTools core framework component package",
			IM_PROJECT("\"ImagingTools Core\"") IM_COMPANY("ImagingTools"));

I_EXPORT_COMPONENT(
			FileCollection,
			"File-based container of the objects",
			"Object Collection File Data Model Repository");


} // namespace ImtRepoPck


