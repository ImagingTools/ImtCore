#pragma once

// Qt includes
#include <QtCore/qglobal.h>


Q_ALWAYS_INLINE static void ImtCoreInitDeskResourcesGlobal()
{
	Q_INIT_RESOURCE(imtchatdb);
	Q_INIT_RESOURCE(imtdeskdb);
	Q_INIT_RESOURCE(imtdb);

	Q_INIT_RESOURCE(imtappApplicationSdl);
	Q_INIT_RESOURCE(imtcolorLabSdl);
	Q_INIT_RESOURCE(imtcolorRgbColorHexSdl);
	Q_INIT_RESOURCE(imt2dGeometrySdl);
	Q_INIT_RESOURCE(imt2dPaintPrimitivesSdl);
	Q_INIT_RESOURCE(imt2dPaintSystemSdl);
}


Q_ALWAYS_INLINE static void ImtCoreInitDeskUiResourcesGlobal()
{

	Q_INIT_RESOURCE(imtgui);
	Q_INIT_RESOURCE(imtguilight);
	Q_INIT_RESOURCE(imtguidark);

	Q_INIT_RESOURCE(imtguiTheme);

	Q_INIT_RESOURCE(imtguiqml);
	Q_INIT_RESOURCE(imtcolguiqml);
	Q_INIT_RESOURCE(imtdocguiqml);
	Q_INIT_RESOURCE(imtguigqlqml);
}


namespace imtcore
{


inline void InitializeImtCoreDesk()
{
	ImtCoreInitDeskResourcesGlobal();
}

inline void InitializeImtCoreDeskUi()
{
	ImtCoreInitDeskUiResourcesGlobal();
}


} // namespace imtcore
