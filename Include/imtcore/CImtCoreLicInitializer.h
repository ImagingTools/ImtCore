#pragma once


// Qt includes
#include <QtCore/qglobal.h>


Q_ALWAYS_INLINE static void ImtCoreInitLicResources()
{
	// Lic server-side resources are currently empty. SDL resources should be here. TBD...
}

Q_ALWAYS_INLINE static void ImtCoreInitLicUiResources()
{
	Q_INIT_RESOURCE(imtlicguilight);
	Q_INIT_RESOURCE(imtlicguidark);

	Q_INIT_RESOURCE(imtlicguiqml);
}


inline void InitializeImtCoreLic()
{
	ImtCoreInitLicResources();
}


inline void InitializeImtCoreLicUi()
{
	ImtCoreInitLicUiResources();
}
