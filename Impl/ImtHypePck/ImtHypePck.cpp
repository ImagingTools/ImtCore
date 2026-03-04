// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include "ImtHypePck.h"


// ACF includes
#include <icomp/export.h>


namespace ImtHypePck
{


I_EXPORT_PACKAGE(
			"ImtHypePck",
			"Acula's task-related components",
			IM_PROJECT("\"ImagingTools Core Framework\"") IM_COMPANY("ImagingTools") "Job Task");

I_EXPORT_COMPONENT(
			TaskCollection,
			"Algorithm task manager",
			"Task Workflow Algorithm Management");

I_EXPORT_COMPONENT(
			PluginBasedTaskCollection,
			"Plugin-based task collection",
			"Task Workflow Algorithm Management");

I_EXPORT_COMPONENT(
			JobQueueManager,
			"Job queue manager",
			"Task Job Queue Processing Management");

I_EXPORT_COMPONENT(
			JobExecutionController,
			"Controller for the job execution",
			"Task Job Execution Run Process");

I_EXPORT_COMPONENT(
			TaskResultCollection,
			"Collection of the task results",
			"Task Result Processing");

I_EXPORT_COMPONENT(
			TaskParamsSet,
			"Extended parameter set with collection-context info",
			"Task Parameter ID");

I_EXPORT_COMPONENT(
			TaskCollectionContext,
			"Task collection context",
			"Task Collection Context");

I_EXPORT_COMPONENT(
			TaskCollectionContextDemultiplexer,
			"Task collection context demultiplexer",
			"Task Collection Context Demultiplexer");

I_EXPORT_COMPONENT(
			PluginBasedJobQueueManager,
			"Plugin-based job queue manager",
			"Job Processing Management Ticket Tasks Pipeline Plugin");

I_EXPORT_COMPONENT(
			PluginBasedJobExecution,
			"Plugin-based job executor/worker",
			"Job Processing Execution Worker Processor Plugin");

I_EXPORT_COMPONENT(
			JobTicketInfo,
			"Job ticket info",
			"Job Ticket Info");

I_EXPORT_COMPONENT(
			JobTicket,
			"Job ticket component",
			"Job Ticket Component Factory");

I_EXPORT_COMPONENT(
			JobTicketMetaInfoCreator,
			"Job ticket meta info creator",
			"Job Ticket Meta Info Creator");


} // namespace ImtHypePck


