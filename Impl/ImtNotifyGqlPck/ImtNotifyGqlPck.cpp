// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include "ImtNotifyGqlPck.h"


// ACF includes
#include <icomp/export.h>


namespace ImtNotifyGqlPck
{


I_EXPORT_PACKAGE(
			"ImtNotifyGqlPck",
			"Notification (Activity) GraphQL controller component package",
			IM_PROJECT("\"ImagingTools Core Framework\"") IM_COMPANY("ImagingTools") "GraphQL");

I_EXPORT_COMPONENT(
			NotificationsController,
			"Notification GraphQL query/mutation controller (list / mark-read / raise), scoped to the authenticated user",
			"Notifications Controller");

I_EXPORT_COMPONENT(
			NotificationPublisher,
			"Notification subscription publisher (pushes each new notification to its recipient)",
			"Notification Publisher");


} // namespace ImtNotifyGqlPck
