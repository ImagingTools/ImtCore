#include "ImtQmlPck.h"


// ACF includes
#include <icomp/export.h>


I_REGISTER_QT_RESOURCE(imtqml);


namespace ImtQmlPck
{


I_EXPORT_PACKAGE(
			"ImtQmlPck",
			"ImagingTools core framework Qml-component package",
			IM_PROJECT("\"ImagingTools Core\"") IM_COMPANY("ImagingTools"));

I_EXPORT_COMPONENT(
            QuickApplication,
            "Main entry point for Qt Quick applications using GUI object as main window",
            "Application Main Entry Start Begin Quick" IM_TAG("Application"));

I_EXPORT_COMPONENT(
            QuickObject,
            "Base graphics object for QuickApplication",
            "Base graphics object for QuickApplication" IM_TAG("QuickObject"));

I_EXPORT_COMPONENT(
            GuiQuickWrap,
            "Container object for QuickObject",
            "Container object for QuickObject" IM_TAG("GuiQuickWrap"));

} // namespace ImtQmlPck


