#include "ImtQmlPck.h"


// ACF includes
#include <icomp/export.h>


I_REGISTER_QT_RESOURCE(imtqml);


namespace ImtQmlPck
{


I_EXPORT_PACKAGE(
			"ImtQmlPck",
			"Package for QML-related components",
			IM_PROJECT("\"ImagingTools Core Framework\"") IM_COMPANY("ImagingTools"));

I_EXPORT_COMPONENT(
			QuickApplication,
			"Main entry point for Qt Quick applications using GUI object as main window",
			"Application Main Quick QML");

I_EXPORT_COMPONENT(
			QuickObject,
			"Base graphics object for QuickApplication",
			"Quick QML");

I_EXPORT_COMPONENT(
			GuiQuickWrap,
			"Container object for QuickObject",
			"Quick QML");


I_EXPORT_COMPONENT(
			ApplicationDataProvider,
			"Application item based data provider",
			"Application item based data provider");

I_EXPORT_COMPONENT(
			ApplicationRepresentationDataProvider,
			"Application representation data provider",
			"Application representation data provider");

I_EXPORT_COMPONENT(
			CommandDataProvider,
			"Command item based data provider",
			"Command item based data provider");

I_EXPORT_COMPONENT(
			PageDataProvider,
			"Page item based data provider",
			"Page item based data provider");

I_EXPORT_COMPONENT(
			EnumApplicationDataProvider,
			"Application Enums data provider",
			"Application Enums data provider");

I_EXPORT_COMPONENT(
			EnumPageDataProvider,
			"Page Enums data provider",
			"Page Enums data provider");


} // namespace ImtQmlPck


