#include "ImtGuiPck.h"


// ACF includes
#include <icomp/export.h>


I_REGISTER_QT_RESOURCE(imtgui);


namespace ImtGuiPck
{


I_EXPORT_PACKAGE(
			"ImtGuiPck",
			"ImagingTools core framework UI-component package",
			IM_PROJECT("\"ImagingTools Core\"") IM_COMPANY("ImagingTools"));

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
			ImageContoursExtender,
			"Image contours extender component",
			"Image Contours Contour Extender View Observer");

I_EXPORT_COMPONENT(
			PageNavigationController,
			"Controller for page navigation",
			"Menu Page Navigation");


} // namespace ImtGuiPck


