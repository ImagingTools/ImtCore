#pragma once


// ACF includes
#include <ibase/TModelObserverCompWrap.h>
#include <icomp/TMakeComponentWrap.h>
#include <icomp/TModelCompWrap.h>
#include <ilog/CMessage.h>

// ImtCore includes
#include <imtlog/CLoginEventControllerComp.h>
#include <imtlog/CEventHistoryControllerComp.h>
#include <imtlog/CStorageComp.h>
#include <imtlog/CGroupMessageIdsProviderComp.h>
#include <imtlog/CEventFilterComp.h>
#include <imtlog/CCollectionBasedMessageConsumerComp.h>


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
typedef imtlog::CCollectionBasedMessageConsumerComp CollectionBasedMessageConsumer;
typedef icomp::TMakeComponentWrap<
			ilog::CMessage,
			istd::IInformationProvider,
			istd::IChangeable> MessageComp;


} // namespace ImtLogPck


