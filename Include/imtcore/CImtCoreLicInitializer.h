#pragma once


// Qt includes
#include <QtCore/qglobal.h>


Q_ALWAYS_INLINE static void ImtCoreInitLicStyleResources()
{
	Q_INIT_RESOURCE(imtlicguilight);
	Q_INIT_RESOURCE(imtlicguidark);
}


Q_ALWAYS_INLINE static void ImtCoreInitLicQmlResources()
{
	Q_INIT_RESOURCE(imtlicguiqml);
}
