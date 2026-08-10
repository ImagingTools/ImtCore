#pragma once

// Qt includes
#include <QtCore/qglobal.h>


Q_ALWAYS_INLINE static void ImtCoreInitAcfLocalizationResourcesGlobal()
{
	Q_INIT_RESOURCE(AcfLoc);
	Q_INIT_RESOURCE(AcfSlnLoc);
}

namespace imtcore
{


inline void InitializeAcfLocalization()
{
	ImtCoreInitAcfLocalizationResourcesGlobal();
}


} // namespace imtcore
