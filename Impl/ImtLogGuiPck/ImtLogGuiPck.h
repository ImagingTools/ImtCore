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
#include <imtloggui/CGroupViewProviderComp.h>
#include <imtloggui/CGroupViewComp.h>
#include <imtloggui/CLayerViewComp.h>
#include <imtloggui/CRepresentationEventsComp.h>
#include <imtloggui/CRepresentationEventsViewComp.h>
#include <imtloggui/CRepresentationProductionComp.h>
#include <imtloggui/CRepresentationProductionViewComp.h>


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
typedef imtloggui::CGroupViewProviderComp GroupViewProvider;
typedef imtloggui::CGroupViewComp GroupView;
typedef imtloggui::CLayerViewComp LayerView;
typedef imtloggui::CRepresentationEventsComp RepresentationEvents;
typedef imtloggui::CRepresentationEventsViewComp RepresentationEventsView;
typedef imtloggui::CRepresentationProductionComp RepresentationProduction;
typedef imtloggui::CRepresentationProductionViewComp RepresentationProductionView;


} // namespace ImtLogGuiPck


