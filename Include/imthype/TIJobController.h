#pragma once


// ACF includes
#include <icomp/CComponentBase.h>
#include <iprm/IParamsSet.h>

// ImtCore includes
#include <imtbase/IProgressSessionsManager.h>
#include <imtbase/TModelUpdateBinder.h>
#include <imthype/IJobQueueManager.h>


namespace imthype
{


template<typename JobParams, typename JobResult>
class TIJobController : istd::IPolymorphic
{
public:
	enum RequestStatus
	{
		RS_INVALID_JOB_ID,
		RS_SUCCESS,
		RS_FAILED
	};

	enum JobStatus
	{
		JS_INVALID_JOB_ID,
		JS_IN_PROGRESS,
		JS_CANCELLATION,
		JS_CANCELLED,
		JS_COMPLETED,
		JS_FAILED
	};


	using JobResultPtr = std::shared_ptr<JobResult>;

	/**
		Begin new job with given params
	*/
	virtual QByteArray BeginJob(const JobParams& jobParams) = 0;

	/**
		Get the result of the job execution with given ID. Non-null if the job was successfully completed
	*/
	virtual JobResultPtr GetJobResult(const QByteArray& jobId) const = 0;

	/**
		Get status of the job with given ID
	*/
	virtual JobStatus GetJobStatus(const QByteArray& jobId) const = 0;
	
	/**
		Cancel the execution of the job with given ID
	*/
	virtual RequestStatus CancelJob(const QByteArray& jobId) = 0;

	/**
		Remove job with given ID
		Getting result of the job with given ID will be unavailable
	*/
	virtual RequestStatus RemoveJob(const QByteArray& jobId) = 0;
};


} // namespace imthype
