#pragma once


// Qt includes
#include <QtCore/qglobal.h>


Q_ALWAYS_INLINE static void ImtCoreInitLocalizationResources()
{
	Q_INIT_RESOURCE(AcfLoc);
	Q_INIT_RESOURCE(AcfSlnLoc);
	Q_INIT_RESOURCE(ImtCoreLoc);
}


inline void InitializeImtCoreLocalization()
{
	ImtCoreInitLocalizationResources();
}
