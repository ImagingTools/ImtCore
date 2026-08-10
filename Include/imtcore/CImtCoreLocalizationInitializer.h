#pragma once

// Qt includes
#include <QtCore/qglobal.h>

// ImtCore includes
#include <imtcore/CAcfLocInitializer.h>


Q_ALWAYS_INLINE static void ImtCoreInitLocalizationResourcesGlobal()
{
	Q_INIT_RESOURCE(ImtCoreLoc);
}

namespace imtcore
{


inline void InitializeImtCoreLocalization()
{
	InitializeAcfLocalization();
	ImtCoreInitLocalizationResourcesGlobal();
}


} // namespace imtcore
