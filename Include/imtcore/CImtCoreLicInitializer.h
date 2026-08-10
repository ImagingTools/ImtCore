#pragma once

// Qt includes
#include <QtCore/qglobal.h>


Q_ALWAYS_INLINE static void ImtCoreInitLicResourcesGlobal()
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


} // namespace imtcore
