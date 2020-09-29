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
#include <imtloggui/CRepresentationItemsFactoryComp.h>
#include <imtloggui/CGroupProviderComp.h>
#include <imtloggui/CLayerProviderComp.h>
#include <imtloggui/CRepresentationFactoryProviderComp.h>
#include <imtloggui/CRepresentationItemsFactoryComp.h>
#include <imtloggui/CRepresentationViewFactoryComp.h>


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
typedef imtloggui::CGraphicsControllerComp GraphicsController;
typedef imtloggui::CRepresentationItemsFactoryComp RepresentationItemsFactory;
typedef imtloggui::CGroupProviderComp GroupProvider;
typedef imtloggui::CLayerProviderComp LayerProvider;
typedef imtloggui::CRepresentationFactoryProviderComp RepresentationFactoryProvider;
typedef imtloggui::CRepresentationItemsFactoryComp RepresentationItemsFactory;
typedef imtloggui::CRepresentationViewFactoryComp RepresentationViewFactory;


} // namespace ImtLogGuiPck


