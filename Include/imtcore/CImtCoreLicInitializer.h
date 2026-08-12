#pragma once


// Qt includes
#include <QtCore/qglobal.h>


Q_ALWAYS_INLINE static void ImtCoreInitLicUiResources()
{
	Q_INIT_RESOURCE(imtlicguilight);
	Q_INIT_RESOURCE(imtlicguidark);
}


Q_ALWAYS_INLINE static void ImtCoreInitLicQmlUiResources()
{
	Q_INIT_RESOURCE(imtlicguiqml);
}
