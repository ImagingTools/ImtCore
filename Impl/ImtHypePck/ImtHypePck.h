// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ACF includes
#include <icomp/TModelCompWrap.h>
#include <icomp/TMakeComponentWrap.h>

// Acula includes
#include <imthype/CTaskCollectionComp.h>
#include <imthype/CPluginBasedTaskCollectionComp.h>
#include <imthype/CJobQueueManagerComp.h>
#include <imthype/CJobExecutionControllerComp.h>
#include <imthype/CTaskResultCollectionComp.h>
#include <imthype/CTaskParamsComp.h>
#include <imthype/CTaskCollectionContextComp.h>
#include <imthype/CTaskCollectionContextDemultiplexerComp.h>
#include <imthype/CPluginBasedJobQueueManagerComp.h>
#include <imthype/CPluginBasedJobExecutionComp.h>
#include <imthype/CJobTicket.h>
#include <imthype/CJobTicketComp.h>
#include <imthype/CJobTicketMetaInfoCreatorComp.h>


/**
	ImtHypePck package
*/
namespace ImtHypePck
{


typedef icomp::TModelCompWrap<imthype::CTaskCollectionComp> TaskCollection;
typedef icomp::TModelCompWrap<imthype::CPluginBasedTaskCollectionComp> PluginBasedTaskCollection;
typedef icomp::TModelCompWrap<imthype::CJobQueueManagerComp> JobQueueManager;
typedef imthype::CJobExecutionControllerComp JobExecutionController;
typedef icomp::TModelCompWrap<imthype::CTaskResultCollectionComp> TaskResultCollection;
typedef icomp::TModelCompWrap<imthype::CTaskParamsComp> TaskParamsSet;
typedef icomp::TModelCompWrap<imthype::CTaskCollectionContextComp> TaskCollectionContext;
typedef imthype::CTaskCollectionContextDemultiplexerComp TaskCollectionContextDemultiplexer;
typedef imthype::CPluginBasedJobQueueManagerComp PluginBasedJobQueueManager;
typedef imthype::CPluginBasedJobExecutionComp PluginBasedJobExecution;
typedef icomp::TModelCompWrap<
			icomp::TMakeComponentWrap<
						imthype::CJobTicket,
						imthype::IJobTicket,
						iser::ISerializable,
						istd::IChangeable>> JobTicketInfo;
typedef icomp::TModelCompWrap<imthype::CJobTicketComp> JobTicket;
typedef imthype::CJobTicketMetaInfoCreatorComp JobTicketMetaInfoCreator;


} // namespace ImtHypePck



