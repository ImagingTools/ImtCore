// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ACF includes
#include <icomp/TModelCompWrap.h>

// ImtCore includes
#include <imtnotify/CNotificationRetentionComp.h>
#include <imtnotify/CNotificationSenderComp.h>
#include <imtnotifydb/CNotificationDbDelegateComp.h>
#include <imtnotifydb/CSqlNotificationInboxControllerComp.h>


/**
	ImtNotifyDbPck package
*/
namespace ImtNotifyDbPck
{


typedef imtnotifydb::CNotificationDbDelegateComp NotificationDbDelegate;
typedef icomp::TModelCompWrap<imtnotifydb::CSqlNotificationInboxControllerComp> NotificationInboxController;
typedef imtnotify::CNotificationSenderComp NotificationSender;
typedef imtnotify::CNotificationRetentionComp NotificationRetention;


} // namespace ImtNotifyDbPck


