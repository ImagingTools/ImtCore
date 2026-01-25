#pragma once


// ImtCore includes
#include <imthype/IJobQueueManager.h>


namespace imthype
{


/**
	Interface for managing job execution status and progress.
	
	This interface is separated from IJobTicket to follow the Single Responsibility Principle.
	IJobTicket represents the job configuration and identity, while IJobStatus tracks the 
	runtime state (status and progress) of a job's execution.
	
	Design Rationale:
	- Job ticket is immutable job configuration (what to execute)
	- Job status is mutable execution state (how execution is progressing)
	- Separation allows independent persistence, caching, and change tracking
	- Enables lighter-weight status queries without loading full job configuration
*/
class IJobStatus: virtual public istd::IChangeable
{
public:
	virtual ~IJobStatus() = default;

	/**
		Get current processing status of the job.
	*/
	virtual IJobQueueManager::ProcessingStatus GetProcessingStatus() const = 0;
	
	/**
		Set processing status of the job.
		\param status New processing status
	*/
	virtual void SetProcessingStatus(IJobQueueManager::ProcessingStatus status) = 0;

	/**
		Get current progress of the job (0.0 to 1.0).
	*/
	virtual double GetProgress() const = 0;
	
	/**
		Set progress of the job.
		\param progress Progress value between 0.0 and 1.0
	*/
	virtual void SetProgress(double progress) = 0;
};


typedef istd::TSharedInterfacePtr<IJobStatus> IJobStatusSharedPtr;


} // namespace imthype


