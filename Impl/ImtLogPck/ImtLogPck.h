#pragma once


// ACF includes
#include <ibase/TModelObserverCompWrap.h>
#include <icomp/TMakeComponentWrap.h>
#include <icomp/TModelCompWrap.h>

// ImtCore includes
#include <imtlog/CLoginEventControllerComp.h>
#include <imtlog/CEventHistoryControllerComp.h>
#include <imtlog/CStorageComp.h>
#include <imtlog/CGroupMessageIdsProviderComp.h>
#include <imtlog/CEventFilterComp.h>


/**
	ImtGuiPck package
*/
namespace ImtLogPck
{


typedef ibase::TModelObserverCompWrap<imtlog::CLoginEventControllerComp> LoginEventController;
typedef icomp::TModelCompWrap<imtlog::CEventHistoryControllerComp> EventHistoryController;
typedef imtlog::CStorageComp Storage;
typedef imtlog::CGroupMessageIdsProviderComp GroupMessageIdsProvider;
typedef imtlog::CEventFilterComp EventFilter;


} // namespace ImtLogPck


