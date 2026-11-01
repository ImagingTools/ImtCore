// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include "ImtNotifyDbPck.h"


// ACF includes
#include <icomp/export.h>


namespace ImtNotifyDbPck
{


I_EXPORT_PACKAGE(
			"ImtNotifyDbPck",
			"User notification database component package",
			IM_PROJECT("\"ImagingTools Core Framework\"") IM_COMPANY("ImagingTools") "Notification Database");

I_EXPORT_COMPONENT(
			NotificationSender,
			"Stores the notifications of the users through the inbox controller",
			"Notification Sender");

I_EXPORT_COMPONENT(
			NotificationDbDelegate,
			"SQL delegate of the read-only collection of the notifications",
			"Notification Collection Delegate SQL");

I_EXPORT_COMPONENT(
			NotificationInboxController,
			"SQL controller of every change of the notifications; tells the changes to the sessions",
			"Notification Inbox Controller SQL");

I_EXPORT_COMPONENT(
			NotificationRetention,
			"Removes the expired notifications and the ones of a removed user",
			"Notification Retention");


} // namespace ImtNotifyDbPck
