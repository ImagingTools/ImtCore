#pragma once


// ACF includes
#include <icomp/TMakeComponentWrap.h>
#include <icomp/TModelCompWrap.h>
#include <ilog/CMessageContainer.h>

// ImtCore includes
#include <imtloggui/CEventViewComp.h>
#include <imtloggui/CLoginEventFactoryComp.h>
#include <imtloggui/CIconBasedEventFactoryComp.h>
#include <imtloggui/CGeneralEventFactoryComp.h>
#include <imtloggui/CEventStatisticsViewComp.h>
#include <imtloggui/CEventItemMetaInfoViewComp.h>
#include <imtloggui/CGraphicsControllerComp.h>
#include <imtloggui/CGroupProviderComp.h>
#include <imtloggui/CGroupViewProviderComp.h>
#include <imtloggui/CGroupComp.h>
#include <imtloggui/CLayerComp.h>
#include <imtloggui/CGroupViewComp.h>
#include <imtloggui/CLayerViewComp.h>
#include <imtloggui/CGroupVisualSettingsComp.h>
#include <imtloggui/CEventBasedRepresentationControllerComp.h>
#include <imtloggui/CEventBasedRepresentationViewComp.h>
#include <imtloggui/CProductionRepresentationControllerComp.h>
#include <imtloggui/CProductionRepresentationComp.h>
#include <imtloggui/CProductionRepresentationViewComp.h>


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
typedef icomp::TModelCompWrap<imtloggui::CGroupProviderComp> GroupProvider;
typedef imtloggui::CGroupViewProviderComp GroupViewProvider;
typedef icomp::TModelCompWrap<imtloggui::CGroupComp> Group;
typedef icomp::TModelCompWrap<imtloggui::CLayerComp> Layer;
typedef icomp::TModelCompWrap<imtloggui::CGroupViewComp> GroupView;
typedef icomp::TModelCompWrap<imtloggui::CLayerViewComp> LayerView;
typedef imtloggui::CGroupVisualSettingsComp GroupVisualSettings;
typedef imtloggui::CEventBasedRepresentationControllerComp EventBasedRepresentationController;
typedef icomp::TModelCompWrap<
			icomp::TMakeComponentWrap<
						ilog::CMessageContainer,
						ilog::IMessageConsumer,
						ilog::IMessageContainer>> EventBasedRepresentation;
typedef icomp::TModelCompWrap<imtloggui::CEventBasedRepresentationViewComp> EventBasedRepresentationView;
typedef imtloggui::CProductionRepresentationControllerComp ProductionRepresentationController;
typedef icomp::TModelCompWrap<imtloggui::CProductionRepresentationComp> ProductionRepresentation;
typedef icomp::TModelCompWrap<imtloggui::CProductionRepresentationViewComp> ProductionRepresentationView;


} // namespace ImtLogGuiPck


