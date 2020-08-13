#pragma once


// ACF includes
#include <ibase/TModelObserverCompWrap.h>
#include <icomp/TMakeComponentWrap.h>
#include <icomp/TModelCompWrap.h>

// ImtCore includes
#include <imtlog/CLoginEventControllerComp.h>
#include <imtlog/CMessageGroupInfoProviderComp.h>
#include <imtlog/CEventHistoryControllerComp.h>
#include <imtlog/CMultithreadMessageGeneratorComp.h>


/**
	ImtGuiPck package
*/
namespace ImtLogPck
{


typedef ibase::TModelObserverCompWrap<imtlog::CLoginEventControllerComp> LoginEventController;
typedef icomp::TMakeComponentWrap<imtlog::CMessageGroupInfoProviderComp, imtlog::IMessageGroupInfoProvider> MessageGroupInfoProvider;
typedef imtlog::CEventHistoryControllerComp EventHistoryController;
typedef imtlog::CMultithreadMessageGeneratorComp MultithreadMessageGenerator;


} // namespace ImtLogPck


