// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include "ImtNotifyDbPck.h"


// ACF includes
#include <icomp/export.h>


namespace ImtNotifyDbPck
{


I_EXPORT_PACKAGE(
			"ImtNotifyDbPck",
			"Notification database delegate component package",
			IM_PROJECT("\"ImagingTools Core Framework\"") IM_COMPANY("ImagingTools"));

I_EXPORT_COMPONENT(
			NotificationDbDelegate,
			"Notification database delegate (SQL persistence with server-side filtering)",
			"Notification Database Delegate");


} // namespace ImtNotifyDbPck
