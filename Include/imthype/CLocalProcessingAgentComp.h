// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// Qt includes
#include <QtCore/QMap>
#include <QtCore/QMutex>
#include <QtCore/QThreadPool>
#include <QtCore/QRunnable>

// ACF includes
#include <icomp/CComponentBase.h>
#include <ibase/CCumulatedProgressManagerBase.h>
#include <ilog/TLoggerCompWrap.h>
#include <iprm/IParamsSet.h>

// ImtCore includes
#include <imtbase/IReferenceCollection.h>

// Acula includes
#include <imthype/IProcessingAgent.h>
#include <imthype/IJobProcessor.h>
#include <imthype/CStandardJobOutput.h>


namespace imthype
{


/**
	Processing agent executing the dispatched tasks locally.

	This is the default in-process agent. It wraps a statically connected list of
	\ref IJobProcessor instances and executes the dispatched tasks in its own thread
	pool. The progress and the result of a task are reported back over the observer
	supplied to \ref DispatchTask.

	The component allows the distributed job execution controller to be used without
	any remote infrastructure, while still going through the agent abstraction. Remote
	agents implementing \ref IProcessingAgent can be added later without changing the
	job execution controller.
*/
class CLocalProcessingAgentComp:
			public ilog::CLoggerComponentBase,
			virtual public IProcessingAgent
{
public:
	typedef ilog::CLoggerComponentBase BaseClass;

	I_BEGIN_COMPONENT(CLocalProcessingAgentComp);
		I_REGISTER_INTERFACE(IProcessingAgent);
		I_ASSIGN(m_agentIdAttrPtr, "AgentId", "Unique ID of the agent", false, "LocalAgent");
		I_ASSIGN(m_maxParallelTasksAttrPtr, "MaxParallelTasks", "Maximum number of tasks executed in parallel (0 = half of the ideal thread count)", false, 0);
		I_ASSIGN_MULTI_0(m_jobTasksCompPtr, "JobTasks", "List of processors for the corresponding task types", true);
	I_END_COMPONENT;

	CLocalProcessingAgentComp();

	// reimplemented (imthype::IProcessingAgent)
	virtual QByteArray GetAgentId() const override;
	virtual AgentState GetAgentState() const override;
	virtual bool IsTaskSupported(const QByteArray& taskTypeId) const override;
	virtual bool DispatchTask(
				const QByteArray& jobId,
				const QByteArray& taskTypeId,
				const imtbase::IReferenceCollection& input,
				const iprm::IParamsSet& params,
				IExecutionObserver& observer) override;
	virtual bool CancelTask(const QByteArray& jobId) override;

protected:
	// reimplemented (icomp::CComponentBase)
	virtual void OnComponentCreated() override;
	virtual void OnComponentDestroyed() override;

	/**
		Get the job processor instance for the given task type.
	*/
	const IJobProcessor* GetTaskProcessor(const QByteArray& taskTypeId) const;

	/**
		Called by the running task when its execution has finished.
	*/
	void OnTaskExecutionFinished(const QByteArray& jobId);

	class TaskProgressManager: public ibase::CCumulatedProgressManagerBase
	{
	public:
		TaskProgressManager(
					const QByteArray& jobId,
					IExecutionObserver& observer);

	protected:
		// reimplemented (ibase::CCumulatedProgressManagerBase)
		virtual void OnProgressChanged(double cumulatedValue) override;

	private:
		QByteArray m_jobId;
		IExecutionObserver& m_observer;
	};

	typedef std::shared_ptr<TaskProgressManager> TaskProgressManagerPtr;

	class Task: public QRunnable
	{
	public:
		Task(
					CLocalProcessingAgentComp& parent,
					const QByteArray& jobId,
					const IJobProcessor& taskProcessor,
					const imtbase::IReferenceCollection& input,
					iprm::IParamsSetSharedPtr paramsPtr,
					TaskProgressManager& progressManager,
					IExecutionObserver& observer);

		// reimplemented (QRunnable)
		virtual void run() override;

	private:
		CLocalProcessingAgentComp& m_parent;
		const IJobProcessor& m_taskProcessor;
		QByteArray m_jobId;
		imtbase::IReferenceCollectionSharedPtr m_inputPtr;
		iprm::IParamsSetSharedPtr m_paramsPtr;
		TaskProgressManager& m_progressManager;
		IExecutionObserver& m_observer;
	};

private:
	/**
		Reference to the list of job tasks for the corresponding task type.
	*/
	I_MULTIREF(IJobProcessor, m_jobTasksCompPtr);

	I_ATTR(QByteArray, m_agentIdAttrPtr);
	I_ATTR(int, m_maxParallelTasksAttrPtr);

	/**
		Thread pool used for executing the dispatched tasks.
	*/
	QThreadPool m_threadPool;

	/**
		Protects the map of running tasks.
	*/
	mutable QMutex m_runningTasksMutex;

	/**
		Progress managers of the currently running tasks, keyed by the job ID.
	*/
	QMap<QByteArray, TaskProgressManagerPtr> m_runningTasks;
};


} // namespace imthype
