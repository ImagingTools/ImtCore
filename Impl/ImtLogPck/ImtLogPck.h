#pragma once


// ACF includes
#include <ibase/TModelObserverCompWrap.h>
#include <icomp/TMakeComponentWrap.h>
#include <icomp/TModelCompWrap.h>

// ImtCore includes
#include <imtlog/CLoginEventControllerComp.h>
#include <imtlog/CMessageGroupInfoProviderComp.h>


/**
	ImtGuiPck package
*/
namespace ImtLogPck
{


typedef ibase::TModelObserverCompWrap<imtlog::CLoginEventControllerComp> LoginEventController;
typedef icomp::TMakeComponentWrap<imtlog::CMessageGroupInfoProviderComp, imtlog::IMessageGroupInfoProvider> MessageGroupInfoProvider;


} // namespace ImtLogPck


