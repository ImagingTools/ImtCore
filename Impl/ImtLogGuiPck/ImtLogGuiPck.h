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
#include <imtloggui/CGraphicsViewComp.h>
#include <imtloggui/CEventStatisticsViewComp.h>


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
typedef imtloggui::CGraphicsViewComp GraphicsView;
typedef imtloggui::CEventStatisticsViewComp EventStatisticsView;


} // namespace ImtLogGuiPck


