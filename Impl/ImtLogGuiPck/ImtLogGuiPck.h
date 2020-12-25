#pragma once


// ACF includes
#include <icomp/TMakeComponentWrap.h>
#include <icomp/TModelCompWrap.h>

// ImtCore includes
#include <imtloggui/CEventViewComp.h>
#include <imtloggui/CLoginEventFactoryComp.h>
#include <imtloggui/CIconBasedEventFactoryComp.h>
#include <imtloggui/CGeneralEventFactoryComp.h>
#include <imtloggui/CEventStatisticsViewComp.h>
#include <imtloggui/CEventItemMetaInfoViewComp.h>
#include <imtloggui/CGraphicsControllerComp.h>
#include <imtloggui/CGroupProviderComp.h>
#include <imtloggui/CGroupComp.h>
#include <imtloggui/CLayerComp.h>
#include <imtloggui/CRepresentationEventsFactoryComp.h>
#include <imtloggui/CRepresentationEventsViewFactoryComp.h>
#include <imtloggui/CRepresentationProductionFactoryComp.h>
#include <imtloggui/CRepresentationProductionViewFactoryComp.h>


/**
	ImtGuiPck package
*/
namespace ImtLogGuiPck
{


typedef imtloggui::CEventViewComp EventView;
typedef imtloggui::CLoginEventFactoryComp LoginEventFactory;
typedef imtloggui::CIconBasedEventFactoryComp IconBasedEventFactory;
typedef imtloggui::CGeneralEventFactoryComp GeneralEventFactory;
typedef imtloggui::CEventStatisticsViewComp EventStatisticsView;
typedef imtloggui::CEventItemMetaInfoViewComp EventItemMetaInfoView;
typedef icomp::TModelCompWrap<imtloggui::CGraphicsControllerComp> GraphicsController;
typedef imtloggui::CGroupProviderComp GroupProvider;
typedef imtloggui::CGroupComp Group;
typedef imtloggui::CLayerComp Layer;
typedef imtloggui::CRepresentationEventsFactoryComp RepresentationEventsFactory;
typedef imtloggui::CRepresentationEventsViewFactoryComp RepresentationEventsViewFactory;
typedef imtloggui::CRepresentationProductionFactoryComp RepresentationProductionFactory;
typedef imtloggui::CRepresentationProductionViewFactoryComp RepresentationProductionSpeedViewFactory;


} // namespace ImtLogGuiPck


