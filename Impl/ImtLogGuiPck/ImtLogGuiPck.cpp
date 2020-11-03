#include "ImtLogGuiPck.h"


// ACF includes
#include <icomp/export.h>


I_REGISTER_QT_RESOURCE(imtloggui);


namespace ImtLogGuiPck
{


I_EXPORT_PACKAGE(
			"ImtLogGuiPck",
			"ImagingTools log framework UI-component package",
			IM_PROJECT("\"ImagingTools Core\"") IM_COMPANY("ImagingTools"));


I_EXPORT_COMPONENT(
			EventView,
			"Event view component",
			"Event View Log");


I_EXPORT_COMPONENT(
			LoginEventFactory,
			"Login event factory",
			"Login Logout Event Item Factory");


I_EXPORT_COMPONENT(
			IconBasedEventFactory,
			"Icon based event factory",
			"Icon Event Item Factory");


I_EXPORT_COMPONENT(
			GeneralEventFactory,
			"General event factory",
			"General Icon Event Item Factory");

I_EXPORT_COMPONENT(
			EventStatisticsView,
			"Events statistics view",
			"Event Statistics View Message Group");

I_EXPORT_COMPONENT(
			EventItemMetaInfoView,
			"Viewer for event items metainfo",
			"Event View Metainfo Items");

I_EXPORT_COMPONENT(
			GraphicsController,
			"Graphics item controller for EventView",
			"Event View Graphics Item Controller");

I_EXPORT_COMPONENT(
			GroupProvider,
			"Events group provider",
			"Events Group Provider");

I_EXPORT_COMPONENT(
			LayerProvider,
			"Layer provider",
			"Layer Provider");

I_EXPORT_COMPONENT(
			RepresentationFactoryProvider,
			"Representation factory provider",
			"Representation Factory Provider");

I_EXPORT_COMPONENT(
			RepresentationItemsFactory,
			"Representation items factory",
			"Representation Items Factory");

I_EXPORT_COMPONENT(
			RepresentationViewFactory,
			"Representation view factory",
			"Representation View Factory");

I_EXPORT_COMPONENT(
			RepresentationIntervalStatisticsFactory,
			"Representation interval statistics factory",
			"Representation Interval Statistics Factory");


} // namespace ImtLogGuiPck


