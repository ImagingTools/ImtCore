#include "ImtLogPck.h"


// ACF includes
#include <icomp/export.h>


namespace ImtLogPck
{


I_EXPORT_PACKAGE(
			"ImtLogPck",
			"ImagingTools log framework component package",
			IM_PROJECT("\"ImagingTools Core\"") IM_COMPANY("ImagingTools"));

I_EXPORT_COMPONENT(
			LoginEventController,
			"Login event controller component",
			"Login Event Controller");

I_EXPORT_COMPONENT(
			EventHistoryController,
			"History for events",
			"Event Log History Controller");

I_EXPORT_COMPONENT(
			Storage,
			"Storage for objects",
			"Event Log Cache Storage Object");


} // namespace ImtLogPck


