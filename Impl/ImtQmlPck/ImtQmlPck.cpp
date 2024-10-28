#include "ImtQmlPck.h"


// ACF includes
#include <icomp/export.h>


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
			ObserverQml,
			"Observer QuickObject",
			"Observer qml quickobject");

I_EXPORT_COMPONENT(
			ClientUserContext,
			"Client-specific context for control global properties of QML engine",
			"Client Localization QML-Engine");

I_EXPORT_COMPONENT(
			PageGuiElementModel,
			"Page based model",
			"Page Based Model");

I_EXPORT_COMPONENT(
			ClientMainWindowQml,
			"Client main window qml",
			"Client Main Window Qml");

I_EXPORT_COMPONENT(
			CommandsObserverQml,
			"Commands observer qml",
			"Commands Observer Qml QML Provider");

I_EXPORT_COMPONENT(
			QmlObjectCollection,
			"Qml Object Collection",
			"Qml Object Collection");

I_EXPORT_COMPONENT(
	ObjectViewDataProvider,
	"Object view data provider",
	"Object view data provider");


} // namespace ImtQmlPck


