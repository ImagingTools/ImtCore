// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// Qt includes
#include <QtCore/QByteArray>
#include <QtCore/QList>

// ImtCore includes
#include <imtbase/ICollectionInfo.h>
#include <imtbase/IReferenceCollection.h>

// Acula includes
#include <imthype/IJobStatusSink.h>


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

	This is the client-facing interface: it bundles queue control (submit,
	cancel, resume, remove), the read-only queries about jobs, and the
	parameter/task factory helpers. The mutating runtime-state operations used
	by the job executor are inherited from \c IJobStatusSink.
*/
class IJobQueueManager:
			virtual public imtbase::ICollectionInfo,
			virtual public IJobStatusSink
{
public:
	typedef iprm::IParamsSetSharedPtr ParamsPtr;

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
		\param input						List of job inputs given as a collection of object links. Each input object should be persistently placed into a storage (IObjectCollection) before calling this method.
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
		Get progress of the running job in the normalized range [0.0 ... 1.0].
	*/
	virtual double GetProgress(const QByteArray& jobId) const = 0;

	/**
		Get result information for a finished job.
		\return \c true if the retrieving of the job results was successful or \c false otherwise
		\note The method works only for finished jobs. 
	*/
	virtual bool GetJobResult(const QByteArray& jobId, IJobOutput& result) const = 0;
};


} // namespace imthype


Q_DECLARE_METATYPE(imthype::IJobQueueManager::JobStatusInfo);
Q_DECLARE_METATYPE(imthype::IJobQueueManager::JobProgressInfo);

