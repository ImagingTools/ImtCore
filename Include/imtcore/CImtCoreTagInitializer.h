#pragma once

// Qt includes
#include <QtCore/qglobal.h>


Q_ALWAYS_INLINE static void ImtCoreInitTagSqlResources()
{
	Q_INIT_RESOURCE(imttagdb);
}


Q_ALWAYS_INLINE static void ImtCoreInitTagQmlResources()
{
	Q_INIT_RESOURCE(imttagguiqml);

	Q_INIT_RESOURCE(imttagTagsSdl);
}


