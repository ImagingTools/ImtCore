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
            CompositeObjectRepresentation,
            "Composite object representation",
            "Representation Data Model Composite Combined Container");

I_EXPORT_COMPONENT(
			DatabaseSettingsDataProvider,
			"Database settings data provider",
			"Database settings data provider");

I_EXPORT_COMPONENT(
			ApplicationDataEnumProvider,
			"Application data enum provider",
			"Application data enum provider");

I_EXPORT_COMPONENT(
			PageDataEnumProvider,
			"Page data enum provider",
			"Page data enum provider");

I_EXPORT_COMPONENT(
			CommandDataEnumProvider,
			"Command data enum provider",
			"Command data enum provider");

I_EXPORT_COMPONENT(
			MenuDataEnumProvider,
			"Menu data enum provider",
			"Menu data enum provider");

I_EXPORT_COMPONENT(
			CompositeRepresentation,
			"Composite Representation",
			"Composite Representation");

I_EXPORT_COMPONENT(
			HeadersDataProvider,
			"Headers data provider",
			"Headers data provider");

I_EXPORT_COMPONENT(
			ObserverQml,
			"Observer QuickObject",
			"Observer qml quickobject");

I_EXPORT_COMPONENT(
			Context,
			"Context",
			"Context");

I_EXPORT_COMPONENT(
			ApplicationInfoDataProvider,
			"Application info data provider",
			"Application info data provider");

I_EXPORT_COMPONENT(
			ObjectViewDataProvider,
			"Object view data provider",
			"Object view data provider");

I_EXPORT_COMPONENT(
			SettingsDatabasePagesProvider,
			"Settings Database Pages Provider",
			"Settings database pages provider");

I_EXPORT_COMPONENT(
			CommandPermissionsFilter,
			"Command Permissions Filter",
			"Command permissions filter");

I_EXPORT_COMPONENT(
			ObjectRepresentation,
			"Object Representation",
			"Object Representation");

I_EXPORT_COMPONENT(
			ButtonRepresentation,
			"ButtonRepresentation",
			"Button Representation");

I_EXPORT_COMPONENT(
			ComboBoxRepresentation,
			"ComboBox Adapter",
			"ComboBox adapter");

I_EXPORT_COMPONENT(
			TextRepresentation,
			"Text Representation",
			"Text Representation");

I_EXPORT_COMPONENT(
			DatabaseLoginSettingsRepresentation,
			"Database Login Settings Representation",
			"Database Login Settings Representation");


} // namespace ImtQmlPck


