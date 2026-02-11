// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// Qt includes
#include <QtCore/QByteArray>
#include <QtCore/QList>

// ImtCore includes
#include <imtbase/ICollectionInfo.h>
#include <imtbase/IReferenceCollection.h>


namespace iprm
{
	class IParamsSet;
	class IOptionsList;
}


namespace ilog
{
	class IMessageConsumer;
}



namespace imthype
{


class IJobSchedulerParams;
class IJobOutput;


/**
	Interface for controlling the job processing queue.
	Job list itself is described by imtbase::ICollectionInfo interface.
*/
class IJobQueueManager: virtual public imtbase::ICollectionInfo
{
public:
	typedef iprm::IParamsSetSharedPtr ParamsPtr;

	/**
		Processing status of a single job in the queue.
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
			JoB was accepted by the job dispatcher and now wating for its execution.
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
			Job results are beeing transported to the job requester.
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

	struct JobStatusInfo: public NotifierInfo
	{
		ProcessingStatus status;
	};
	static const QByteArray CN_JOB_STATUS_CHANGED;

	struct JobProgressInfo : public NotifierInfo
	{
		double progress;
	};
	static const QByteArray CN_JOB_PROGRESS_CHANGED;

	typedef NotifierInfo JobResultInfo;
	static const QByteArray CN_JOB_RESULT_CHANGED;

	/**
		Operational flags for a single job or the whole job queue.
	*/
	enum JobFlags
	{
		/**
			Job can be paused and resumed.
		*/
		JF_RESUMABLE = 1,

		/**
			Job can be canceled.
		*/
		JF_CANCELABLE = 2
	};

	/**
		Get the list of supported task types.
	*/
	virtual const iprm::IOptionsList* GetSupportedTasks() const = 0;

	/**
		Get the type-ID of the given job task.
	*/
	virtual QByteArray GetTaskTypeId(const QByteArray& jobId) const = 0;

	/**
		Create processing parameters of the given type for the job.
	*/
	virtual iprm::IParamsSetUniquePtr CreateJobParameters(
				const QByteArray& contextId,
				const QByteArray& taskTypeId,
				const iprm::IParamsSet* defaultParamPtr) const = 0;

	/**
		Insert processing job into the processing queue.
		If the operation was successful, the method will return the UUID of the created job or an empty value otherwise.
		You can use the returned UUID for controlling the job execution and retrieving results.
		\param contextId					ID of the job context.
		\param typeId						Type-ID for the job. Will be used for pairing the corresponding job worker.
		\param input						List of job inputs given as a collection of object links. Each input object should be persistency placed into a storage (IObjectCollection) bevor call this method.
		\param jobProcessingParamsPtr		Processing parameters for the job (so called job ticket).
		\param schedulerParamsPtr			Parameters of the job scheduler (optional).
		\param logPtr						Job related log.
	*/
	virtual QByteArray InsertNewJobIntoQueue(
				const QByteArray& contextId,
				const QByteArray& typeId,
				const imtbase::IReferenceCollection& input,
				const iprm::IParamsSet* jobProcessingParamsPtr,
				const IJobSchedulerParams* schedulerParamsPtr = nullptr,
				ilog::IMessageConsumer* logPtr = nullptr) = 0;

	/**
		Cancel processing of the given job.

		\return true if operation was successful or \c false otherwise.
	*/
	virtual bool CancelJob(const QByteArray& jobId) = 0;

	/**
		Resume a paused job.

		\note The operation is supported only if the job is in paused or in finished state.

		\return true if operation was successful or \c false otherwise.
	*/
	virtual bool ResumeJob(const QByteArray& jobId) = 0;

	/**
		Remove a job from the queue.

		\return true if operation was successful or \c false otherwise.
	*/
	virtual bool RemoveJob(const QByteArray& jobId) = 0;

	/**
		Get job configuration for a given job.
	*/
	virtual bool GetJobConfiguration(
				const QByteArray& jobId,
				ParamsPtr& processingParamsPtr,
				imtbase::IReferenceCollection& input) const = 0;

	/**
		Get corresponded job processing status for the job with a given ID \c jobId.
	*/
	virtual ProcessingStatus GetProcessingStatus(const QByteArray& jobId) const = 0;

	/**
		Set job processing status for the job with a given ID \c jobId.
	*/
	virtual bool SetProcessingStatus(const QByteArray& jobId, ProcessingStatus status) = 0;
	
	/**
		Get progress of the runnung job.
	*/
	virtual double GetProgress(const QByteArray& jobId) const = 0;

	/**
		Set progress of the runnung job.
	*/
	virtual bool SetProgress(const QByteArray& jobId, double progress) = 0;

	/**
		Get result information for a finished job.
		\return \c true if the retrieving of the job results was successful or \c false otherwise
		\note The method works only for finished jobs. 
	*/
	virtual bool GetJobResult(const QByteArray& jobId, IJobOutput& result) const = 0;

	/**
		Set result information for a finished job.
		\return \c true if the job results could be set or \c false otherwise.
		\note The method works only for finished jobs. 
	*/
	virtual bool SetJobResult(const QByteArray& jobId, const IJobOutput& result) = 0;
};


} // namespace imthype


Q_DECLARE_METATYPE(imthype::IJobQueueManager::JobStatusInfo);
Q_DECLARE_METATYPE(imthype::IJobQueueManager::JobProgressInfo);

