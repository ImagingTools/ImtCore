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
			CommandDataProvider,
			"Command item based data provider",
			"Command item based data provider");

I_EXPORT_COMPONENT(
			PageDataProvider,
			"Page item based data provider",
			"Page item based data provider");

I_EXPORT_COMPONENT(
			HeadersDataProvider,
			"Headers data provider",
			"Headers data provider");

I_EXPORT_COMPONENT(
			ObserverQml,
			"Observer QuickObject",
			"Observer qml quickobject");

I_EXPORT_COMPONENT(
			ClientUserContext,
			"Client-specific context for control global properties of QML engine",
			"Client Localization QML-Engine");

I_EXPORT_COMPONENT(
			ApplicationInfoDataProvider,
			"Application info data provider",
			"Application info data provider");

I_EXPORT_COMPONENT(
			ObjectViewDataProvider,
			"Object view data provider",
			"Object view data provider");

I_EXPORT_COMPONENT(
			CommandPermissionsFilter,
			"Command Permissions Filter",
			"Command permissions filter");

I_EXPORT_COMPONENT(
			PageGuiElementModel,
			"Page based model",
			"Page Based Model");

I_EXPORT_COMPONENT(
			CommandsObserverQml,
			"Commands observer qml",
			"Commands Observer Qml QML Provider");


} // namespace ImtQmlPck


