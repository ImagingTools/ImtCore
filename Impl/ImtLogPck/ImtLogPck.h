#pragma once


// ACF includes
#include <ibase/TModelObserverCompWrap.h>
#include <icomp/TMakeComponentWrap.h>
#include <icomp/TModelCompWrap.h>

// ImtCore includes
#include <imtlog/CLoginEventControllerComp.h>
#include <imtlog/CEventHistoryControllerComp.h>
#include <imtlog/CStorageComp.h>


/**
	ImtGuiPck package
*/
namespace ImtLogPck
{


typedef ibase::TModelObserverCompWrap<imtlog::CLoginEventControllerComp> LoginEventController;
typedef icomp::TModelCompWrap<imtlog::CEventHistoryControllerComp> EventHistoryController;
typedef imtlog::CStorageComp Storage;


} // namespace ImtLogPck


