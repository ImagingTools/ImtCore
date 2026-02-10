// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include "ImtAppPck.h"


// ACF includes
#include <icomp/export.h>


namespace ImtAppPck
{


I_EXPORT_PACKAGE(
			"ImtAppPck",
			"ImagingTools core framework component package",
			IM_PROJECT("\"ImagingTools Core Framework\"") IM_COMPANY("ImagingTools"));

I_EXPORT_COMPONENT(
			BackupSettings,
			"Backup settings",
			"Backup Settings");

I_EXPORT_COMPONENT(
			SchedulerParams,
			"Scheduler params",
			"Scheduler Params");


} // namespace ImtAppPck


