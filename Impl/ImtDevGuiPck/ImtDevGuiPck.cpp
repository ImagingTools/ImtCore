// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
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


