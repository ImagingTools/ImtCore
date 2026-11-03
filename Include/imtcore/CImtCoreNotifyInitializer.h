#pragma once

// Qt includes
#include <QtCore/qglobal.h>


Q_ALWAYS_INLINE static void ImtCoreInitNotifySqlResources()
{
	Q_INIT_RESOURCE(imtnotifydb);
}


Q_ALWAYS_INLINE static void ImtCoreInitNotifyQmlResources()
{
	Q_INIT_RESOURCE(imtnotifyguiqml);

	Q_INIT_RESOURCE(imtnotifyNotificationsSdl);
}
