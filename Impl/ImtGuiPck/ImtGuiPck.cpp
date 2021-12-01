#include "ImtGuiPck.h"


// ACF includes
#include <icomp/export.h>


I_REGISTER_QT_RESOURCE(imtguilight);


namespace ImtGuiPck
{


I_EXPORT_PACKAGE(
			"ImtGuiPck",
			"ImagingTools core framework UI-component package",
			IM_PROJECT("\"ImagingTools Core Framework\"") IM_COMPANY("ImagingTools"));

I_EXPORT_COMPONENT(
			GuiComponentFactory,
			"Factor for a GUI component",
			"UI GUI Factory");

I_EXPORT_COMPONENT(
			ThumbnailDecoratorGui,
			"Thumbnail selection page view component",
			"Menu Thumbnail Selection Page");

I_EXPORT_COMPONENT(
			SingleStatusProviderGui,
			"Single information status provider GUI component",
			"Single Information Status Provider");

I_EXPORT_COMPONENT(
			MultiStatusProviderGui,
			"Multiple information status provider GUI component",
			"Multiple Information Status Provider");

I_EXPORT_COMPONENT(
			ObjectCollectionView,
			"View for a object collection",
			"Object Collection View");

I_EXPORT_COMPONENT(
			DocumentWorkspaceGui,
			"Standard document workspace",
			"Workspace Tabs View Document File MDI");

I_EXPORT_COMPONENT(
			ObjectCollectionMetaInfoView,
			"Object collection metainfo view",
			"Object Data Collection Document Meta Info Metainfo View Preview");

I_EXPORT_COMPONENT(
			DelegatedCommandsProvider,
			"Delegator for a commands provider",
			"Command Provider Delegate");

I_EXPORT_COMPONENT(
			MenuPanel,
			"Menu panel view component",
			"Menu panel page");

I_EXPORT_COMPONENT(
			MonitorInfoProvider,
			"Monitor info provider component",
			"Monitor Info Provider Resolution Size Count");

I_EXPORT_COMPONENT(
			LayoutManager,
			"LayoutManager component",
			"LayoutManager model");

I_EXPORT_COMPONENT(
			LayoutManagerGui,
			"LayoutManagerGui component",
			"LayoutManagerGui page");

I_EXPORT_COMPONENT(
			CompositeObjectView,
			"Composite object viewer",
			"Composite Object View Observer Collection");

I_EXPORT_COMPONENT(
			PageNavigationController,
			"Controller for page navigation",
			"Menu Page Navigation");

I_EXPORT_COMPONENT(
			PageSwitcher,
			"Page switcher",
			"Page Switch Login");

I_EXPORT_COMPONENT(
			StandardCollectionViewDelegate,
			"Standard collection view delegate",
			"Delegate Document View Collection");

I_EXPORT_COMPONENT(
			PluginStatusViewer,
			"Plugin status viewer",
			"Plugin Status View");

I_EXPORT_COMPONENT(
			PluginStatusMonitorViewDelegate,
			"Plugin status monitor view delegate",
			"Delegate View Collection Plugin Status");

I_EXPORT_COMPONENT(
			ApplicationSwitchCommand,
			"Command for switching between applications",
			"Command Application Switch Process Window");

I_EXPORT_COMPONENT(
			DesignManager,
			"Design Manager",
			"Style Design Manager");

I_EXPORT_COMPONENT(
			VirtualKeyboardShowCommand,
			"Command for show Virtual Keyboard",
			"Command for show Virtual Keyboard");

I_EXPORT_COMPONENT(
			FullScreenCommand,
			"Command for switch of FullScreen/Normal mode",
			"Command for switch of FullScreen/Normal mode");

I_EXPORT_COMPONENT(
			MenuPanelDelegate,
			"Menu panel delegate",
			"Delegate Menu Panel Navigation");

I_EXPORT_COMPONENT(
			MenuPanelDelegateMin,
			"Minimal menu panel delegate",
			"Delegate Menu Panel Navigation");

I_EXPORT_COMPONENT(
			SingleDocumentWorkspace,
			"SDI document workspace",
			"View Document SDI Decorator");

I_EXPORT_COMPONENT(
			DocumentCollectionViewDelegate,
			"View delegate for a collection using document manager for the data operations",
			"Object Collection Delegate View Document MDI");

I_EXPORT_COMPONENT(
		MenuBasedCommandsProvider,
		"Menu-Based commands provider",
		"MenuBased Commands Provider");

		
} // namespace ImtGuiPck


