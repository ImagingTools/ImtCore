#pragma once


// Qt includes
#include <QtCore/qglobal.h>


Q_ALWAYS_INLINE static void ImtCoreInitLicResourcesGlobal()
{
	// Lic server-side resources are currently empty. SDL resources should be here. TBD...
}

Q_ALWAYS_INLINE static void ImtCoreInitLicUiResourcesGlobal()
{
	Q_INIT_RESOURCE(imtlicguilight);
	Q_INIT_RESOURCE(imtlicguidark);

	Q_INIT_RESOURCE(imtlicguiqml);
}


namespace imtcore
{


inline void InitializeImtCoreLic()
{
	ImtCoreInitLicResourcesGlobal();
}


inline void InitializeImtCoreLicUi()
{
	ImtCoreInitLicUiResourcesGlobal();
}


} // namespace imtcore
