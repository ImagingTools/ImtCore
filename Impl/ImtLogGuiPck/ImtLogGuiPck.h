#pragma once


// ACF includes
#include <icomp/TMakeComponentWrap.h>
#include <icomp/TModelCompWrap.h>

// ImtCore includes
#include <imtloggui/CEventViewComp.h>
#include <imtloggui/CLoginEventFactoryComp.h>
#include <imtloggui/CIconBasedEventFactoryComp.h>
#include <imtloggui/CGeneralEventFactoryComp.h>
#include <imtloggui/CEventGroupControllerComp.h>
#include <imtloggui/CSingleLayerGroupComp.h>
#include <imtloggui/CEventStatisticsViewComp.h>
#include <imtloggui/CEventItemMetaInfoViewComp.h>
#include <imtloggui/CGraphicsControllerComp.h>
#include <imtloggui/CRepresentationItemsFactoryComp.h>
#include <imtloggui/CGroupProviderComp.h>
#include <imtloggui/CGroupComp.h>
#include <imtloggui/CLayerComp.h>
#include <imtloggui/CRepresentationComp.h>


/**
	ImtGuiPck package
*/
namespace ImtLogGuiPck
{


typedef imtloggui::CEventViewComp EventView;
typedef imtloggui::CLoginEventFactoryComp LoginEventFactory;
typedef imtloggui::CIconBasedEventFactoryComp IconBasedEventFactory;
typedef imtloggui::CGeneralEventFactoryComp GeneralEventFactory;
typedef imtloggui::CEventGroupControllerComp EventGroupController;
typedef imtloggui::CSingleLayerGroupComp SingleLayerGroup;
typedef imtloggui::CEventStatisticsViewComp EventStatisticsView;
typedef imtloggui::CEventItemMetaInfoViewComp EventItemMetaInfoView;
typedef imtloggui::CGraphicsControllerComp GraphicsController;
typedef imtloggui::CRepresentationItemsFactoryComp RepresentationItemsFactory;
typedef imtloggui::CGroupProviderComp GroupProvider;
typedef imtloggui::CGroupComp Group;
typedef imtloggui::CLayerComp Layer;
typedef imtloggui::CRepresentationComp Representation;


} // namespace ImtLogGuiPck


