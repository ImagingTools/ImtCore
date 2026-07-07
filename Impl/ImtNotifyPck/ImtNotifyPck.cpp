// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include "ImtNotifyPck.h"


// ACF includes
#include <icomp/export.h>


namespace ImtNotifyPck
{


I_EXPORT_PACKAGE(
			"ImtNotifyPck",
			"Notification (Activity) domain model component package",
			IM_PROJECT("\"ImagingTools Core Framework\"") IM_COMPANY("ImagingTools"));

I_EXPORT_COMPONENT(
			Notification,
			"Single user notification data component",
			"Notification");

I_EXPORT_COMPONENT(
			NotificationService,
			"Notification service (raise / list / mark-read notifications)",
			"Notification Service");


} // namespace ImtNotifyPck
