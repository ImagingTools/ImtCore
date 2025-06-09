#include "ImtDevGuiPck.h"


// ACF includes
#include <icomp/export.h>


namespace ImtDevGuiPck
{


I_EXPORT_PACKAGE(
			"ImtDatabaseGuiPck",
			"Database-related UI-component package",
			IM_PROJECT("\"ImagingTools Core Framework\"") IM_COMPANY("ImagingTools"));


I_EXPORT_COMPONENT(
			DeviceSelectionGui,
			"Editor for the device selection",
			"Device Selection Gui");


} // namespace ImtDevGuiPck


