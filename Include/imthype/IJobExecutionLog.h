#pragma once


// ACF includes
#include <ilog/IMessageContainer.h>


namespace imthype
{


/**
	Interface for job execution log.
	
	This log tracks system-level events from the job controller, including
	lifecycle events, state transitions, and control flow operations.
	This is distinct from the processor/worker log which contains actual
	processing output from the worker doing the job.
*/
class IJobExecutionLog: virtual public ilog::IMessageContainer
{
public:
	/**
		Log entry types specific to job execution tracking.
	*/
	enum ExecutionEventType
	{
		/**
			Job was created and inserted into queue.
		*/
		EET_JOB_CREATED,
		
		/**
			Job was accepted by job dispatcher.
		*/
		EET_JOB_ACCEPTED,
		
		/**
			Job execution started.
		*/
		EET_JOB_STARTED,
		
		/**
			Job status changed.
		*/
		EET_STATUS_CHANGED,
		
		/**
			Job progress updated.
		*/
		EET_PROGRESS_UPDATED,
		
		/**
			Job was paused.
		*/
		EET_JOB_PAUSED,
		
		/**
			Job was resumed.
		*/
		EET_JOB_RESUMED,
		
		/**
			Job was cancelled.
		*/
		EET_JOB_CANCELLED,
		
		/**
			Job completed successfully.
		*/
		EET_JOB_COMPLETED,
		
		/**
			Job failed with error.
		*/
		EET_JOB_FAILED
	};
	
	I_DECLARE_ENUM(ExecutionEventType, 
		EET_JOB_CREATED, EET_JOB_ACCEPTED, EET_JOB_STARTED, 
		EET_STATUS_CHANGED, EET_PROGRESS_UPDATED,
		EET_JOB_PAUSED, EET_JOB_RESUMED, EET_JOB_CANCELLED,
		EET_JOB_COMPLETED, EET_JOB_FAILED);
	
	/**
		Log an execution event.
		\param eventType Type of execution event
		\param message Event description
		\param timestamp Event timestamp
	*/
	virtual void LogExecutionEvent(
		ExecutionEventType eventType,
		const QString& message,
		const QDateTime& timestamp = QDateTime::currentDateTime()) = 0;
	
	/**
		Get all execution events of a specific type.
		\param eventType Type of events to retrieve
		\return List of messages matching the event type
	*/
	virtual QList<ilog::IMessage> GetExecutionEvents(ExecutionEventType eventType) const = 0;
};


typedef istd::TSharedInterfacePtr<IJobExecutionLog> IJobExecutionLogSharedPtr;


} // namespace imthype


