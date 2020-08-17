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
			EventGroupController,
			"Event group controller",
			"Event Group Controller");


I_EXPORT_COMPONENT(
			SingleLayerGroup,
			"Single layer group",
			"Event Group Single Layer");


I_EXPORT_COMPONENT(
			GraphicsView,
			"Graphics view",
			"Graphics View");


I_EXPORT_COMPONENT(
			EventStatisticsView,
			"Events statistics view",
			"Event Statistics View Message Group");

I_EXPORT_COMPONENT(
			EventItemMetaInfoView,
			"Viewer for event items metainfo",
			"Event View Metainfo Items");


} // namespace ImtLogGuiPck


