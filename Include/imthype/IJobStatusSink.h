// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// Qt includes
#include <QtCore/QByteArray>

// ImtCore includes
#include <imtbase/ICollectionInfo.h>


namespace imthype
{


class IJobOutput;


/**
	Executor-facing interface for writing the runtime state of a job.

	This interface intentionally exposes only the mutating operations that the
	job execution controller (the worker side) needs in order to report status,
	progress and results back into the job queue. Clients that merely submit and
	observe jobs should depend on \c IJobQueueManager instead, which provides the
	read-only queries together with the queue control operations.

	Separating the write surface from the queue control surface makes it explicit
	who is allowed to mutate a job's runtime state (Interface Segregation).
*/
class IJobStatusSink: virtual public istd::IPolymorphic
{
public:
	/**
		Processing status of a single job in the queue.
		This is the canonical status type shared by the queue manager, the job
		tickets and the per-job status objects.
	*/
	enum ProcessingStatus
	{
		/**
			No status.
		*/
		PS_NONE,

		/**
			Job is waiting for accepting by the processing pipeline.
		*/
		PS_WAITING_FOR_ACCEPTING,

		/**
			Job was accepted by the job dispatcher and now waiting for its execution.
		*/
		PS_WAITING_FOR_PROCESSING,

		/**
			Job is running.
		*/
		PS_RUNNING,

		/**
			Job is canceling.
		*/
		PS_CANCELING,

		/**
			Job results are being transported to the job requester.
		*/
		PS_RETRIEVING_RESULTS,

		/**
			Job was canceled.
		*/
		PS_CANCELED,

		/**
			Job request was rejected by the processing pipeline.
		*/
		PS_REJECTED,

		/**
			Job was finished.
			The job result can be requested.
		*/
		PS_FINISHED
	};

	I_DECLARE_ENUM(ProcessingStatus, PS_NONE, PS_WAITING_FOR_ACCEPTING, PS_WAITING_FOR_PROCESSING, PS_RUNNING, PS_CANCELING, PS_RETRIEVING_RESULTS, PS_CANCELED, PS_REJECTED, PS_FINISHED);

	/**
		Set job processing status for the job with a given ID \c jobId.
	*/
	virtual bool SetProcessingStatus(const QByteArray& jobId, ProcessingStatus status) = 0;

	/**
		Set progress of the running job in the normalized range [0.0 ... 1.0].
	*/
	virtual bool SetProgress(const QByteArray& jobId, double progress) = 0;

	/**
		Set result information for a finished job.
		\return \c true if the job results could be set or \c false otherwise.
		\note The method works only for finished jobs.
	*/
	virtual bool SetJobResult(const QByteArray& jobId, const IJobOutput& result) = 0;
};


} // namespace imthype
