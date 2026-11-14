// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// Qt includes
#include <QtCore/QByteArray>

// ACF includes
#include <istd/IPolymorphic.h>
#include <istd/TIFactory.h>

// ImtCore includes
#include <imtbase/IReferenceCollection.h>


namespace iprm
{
	class IParamsSet;
}


namespace imthype
{


class IJobOutput;


/**
	Interface of a processing agent.

	A processing agent is an abstraction of a worker that is able to execute job
	tasks. In contrast to the local job execution (where the task is executed in a
	thread of the calling process), an agent represents a possibly remote and
	independent execution backend. The job execution controller forwards tasks to
	agents which then perform the actual work and report the result back.

	The execution is asynchronous: a dispatched task notifies the caller about
	its progress and completion using the \ref IExecutionObserver interface.
	Because an agent may run the task in another thread or process, observer
	callbacks can be invoked from an arbitrary thread.
*/
class IProcessingAgent: virtual public istd::IPolymorphic
{
public:
	/**
		Observer used to report progress and completion of a dispatched task.

		\note The callbacks may be invoked from a thread different from the one
		that dispatched the task. The implementation has to take care of the
		correct thread synchronization.
	*/
	class IExecutionObserver
	{
	public:
		virtual ~IExecutionObserver() {}

		/**
			Called when the progress of a dispatched task has changed.
			\param jobId	ID of the dispatched job.
			\param progress	Progress value between 0.0 and 1.0.
		*/
		virtual void OnTaskProgressChanged(const QByteArray& jobId, double progress) = 0;

		/**
			Called when a dispatched task has finished its execution.
			\param jobId	ID of the dispatched job.
			\param output	Result of the task processing.
		*/
		virtual void OnTaskFinished(const QByteArray& jobId, const IJobOutput& output) = 0;
	};

	/**
		Current state of the agent.
	*/
	enum AgentState
	{
		/**
			Agent is not reachable and cannot accept tasks.
		*/
		AS_OFFLINE,

		/**
			Agent is connected and ready to accept new tasks.
		*/
		AS_IDLE,

		/**
			Agent is currently executing one or more tasks but may still accept more.
		*/
		AS_BUSY,

		/**
			Agent is fully loaded and cannot accept new tasks at the moment.
		*/
		AS_FULL
	};

	/**
		Get the unique ID of the agent.
	*/
	virtual QByteArray GetAgentId() const = 0;

	/**
		Get the current state of the agent.
	*/
	virtual AgentState GetAgentState() const = 0;

	/**
		Check whether the agent is able to process tasks of the given type.
		\param taskTypeId	Type-ID of the task.
	*/
	virtual bool IsTaskSupported(const QByteArray& taskTypeId) const = 0;

	/**
		Dispatch a task to the agent for asynchronous execution.

		If the task was accepted, the agent takes the ownership of the execution
		and reports progress and the final result over the given \c observer.

		\param jobId		ID of the job. Used to identify the task in observer callbacks and for cancellation.
		\param taskTypeId	Type-ID of the task to be performed.
		\param input		List of IDs for identifying input objects in the storage.
		\param params		Processing parameters.
		\param observer		Observer used for reporting task progress and completion.
		\return \c true if the task was accepted for execution or \c false otherwise.
	*/
	virtual bool DispatchTask(
				const QByteArray& jobId,
				const QByteArray& taskTypeId,
				const imtbase::IReferenceCollection& input,
				const iprm::IParamsSet& params,
				IExecutionObserver& observer) = 0;

	/**
		Cancel a previously dispatched task.
		\param jobId	ID of the dispatched job.
		\return \c true if the cancellation request was accepted or \c false otherwise.
	*/
	virtual bool CancelTask(const QByteArray& jobId) = 0;
};


typedef istd::TIFactory<IProcessingAgent> IProcessingAgentFactory;
typedef istd::TUniqueInterfacePtr<IProcessingAgent> IProcessingAgentUniquePtr;
typedef istd::TSharedInterfacePtr<IProcessingAgent> IProcessingAgentSharedPtr;


} // namespace imthype
