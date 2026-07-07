// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ACF includes
#include <icomp/TModelCompWrap.h>

// ImtCore includes
#include <imtnotify/CNotificationComp.h>
#include <imtnotify/CNotificationServiceComp.h>


/**
	ImtNotifyPck package
*/
namespace ImtNotifyPck
{


typedef icomp::TModelCompWrap<imtnotify::CNotificationComp> Notification;
typedef imtnotify::CNotificationServiceComp NotificationService;


} // namespace ImtNotifyPck
