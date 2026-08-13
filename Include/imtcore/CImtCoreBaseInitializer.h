#pragma once

// Qt includes
#include <QtCore/qglobal.h>


Q_ALWAYS_INLINE static void ImtCoreInitBaseResources()
{
	Q_INIT_RESOURCE(imtbase);
	Q_INIT_RESOURCE(imtdb);
}


Q_ALWAYS_INLINE static void ImtCoreInitStyleResources()
{
	Q_INIT_RESOURCE(imtgui);
	Q_INIT_RESOURCE(imtguilight);
	Q_INIT_RESOURCE(imtguidark);

	Q_INIT_RESOURCE(imtguiTheme);

	Q_INIT_RESOURCE(imtstyle);
	Q_INIT_RESOURCE(imtstylelight);
	Q_INIT_RESOURCE(imtstyledark);
}


Q_ALWAYS_INLINE static void ImtCoreInitQmlApplicationCoreResources()
{
	Q_INIT_RESOURCE(imtappApplicationSdl);
	Q_INIT_RESOURCE(imtbaseCommandsSdl);
	Q_INIT_RESOURCE(imtbaseImtBaseTypesSdl);
	Q_INIT_RESOURCE(imtbaseImtCollectionSdl);
	Q_INIT_RESOURCE(imtbaseFileSystemSdl);
	Q_INIT_RESOURCE(imtbaseProgressManagerSdl);
	Q_INIT_RESOURCE(imtbaseSearchSdl);
	Q_INIT_RESOURCE(imtbaseSettingsSdl);
}


Q_ALWAYS_INLINE static void ImtCoreInitQmlCollectionResources()
{
	Q_INIT_RESOURCE(imtcolguiqml);

	Q_INIT_RESOURCE(imtbaseComplexCollectionFilterSdl);
	Q_INIT_RESOURCE(imtbaseCollectionDocumentServiceSdl);
	Q_INIT_RESOURCE(imtbaseCollectionImportSdl);
	Q_INIT_RESOURCE(imtbaseFilterableSelectSdl);
}


Q_ALWAYS_INLINE static void ImtCoreInitQmlDocumentManagementResources()
{
	ImtCoreInitQmlCollectionResources();

	Q_INIT_RESOURCE(imtdocguiqml);

	Q_INIT_RESOURCE(imtbaseDocumentRevisionSdl);
	Q_INIT_RESOURCE(imtbaseDocumentIdFilterSdl);
	Q_INIT_RESOURCE(imtbaseDocumentCollectionFilterSdl);
	Q_INIT_RESOURCE(imtbaseUndoManagerSdl);
}


Q_ALWAYS_INLINE static void ImtCoreInitQmlPaintResources()
{
	Q_INIT_RESOURCE(imt2dGeometrySdl);
	Q_INIT_RESOURCE(imt2dPaintPrimitivesSdl);
	Q_INIT_RESOURCE(imt2dPaintSystemSdl);

	Q_INIT_RESOURCE(imtcolorLabSdl);
	Q_INIT_RESOURCE(imtcolorRgbColorHexSdl);
}


Q_ALWAYS_INLINE static void ImtCoreInitQmlCoreControlsResources()
{
	Q_INIT_RESOURCE(imtguiqml);
	Q_INIT_RESOURCE(imtstylecontrolsqml);
	Q_INIT_RESOURCE(imtcontrolsqml);

	Q_INIT_RESOURCE(imtguigqlqml);
}
