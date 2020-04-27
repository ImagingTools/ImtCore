#include "ImtLogGuiPck.h"


// ACF includes
#include <icomp/export.h>


I_REGISTER_QT_RESOURCE(imtloggui);


namespace ImtLogGuiPck
{


I_EXPORT_PACKAGE(
			"ImtLogGuiPck",
			"ImagingTools log framework UI-component package",
			IM_PROJECT("\"ImagingTools Core\"") IM_COMPANY("ImagingTools"));


I_EXPORT_COMPONENT(
			EventView,
			"Event view component",
			"Event View Log");


} // namespace ImtLogGuiPck


