// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// Qt includes
#include <QtCore/QMap>
#include <QtCore/QMutex>
#include <QtCore/QTimer>

// ACF includes
#include <ilog/TLoggerCompWrap.h>
#include <iprm/IParamsSet.h>

// ImtCore includes
#include <imtbase/TModelUpdateBinder.h>

// Acula includes
#include <imthype/IJobQueueManager.h>
#include <imthype/IProcessingAgent.h>
#include <imthype/IProcessingAgentRegistry.h>
#include <imthype/CStandardJobOutput.h>


namespace imthype
{


/**
	Controller for distributed job execution.

	In contrast to \ref CJobExecutionControllerComp, this controller does not execute
	the queued jobs in a local thread. Instead it forwards each job to a processing
	agent obtained from the connected \ref IProcessingAgentRegistry. The selected agent
	(which may be a remote worker) performs the actual task and reports progress and the
	final result back over the \ref IProcessingAgent::IExecutionObserver interface.

	The controller polls the underlying job manager. When a job in the queue reaches the
	status \c PS_WAITING_FOR_PROCESSING, the controller asks the registry for an agent
	that supports the job's task type and is ready to accept new work. If such an agent
	is found, the job is dispatched to it; otherwise the job stays in the queue and the
	dispatch is retried during the next polling cycle.
*/
class CDistributedJobExecutionControllerComp:
			public QObject,
			public ilog::CLoggerComponentBase,
			virtual public IProcessingAgent::IExecutionObserver
{
	Q_OBJECT
public:
	typedef ilog::CLoggerComponentBase BaseClass;

	I_BEGIN_COMPONENT(CDistributedJobExecutionControllerComp);
		I_ASSIGN(m_jobQueueManagerCompPtr, "JobQueueManager", "Manager for the job queue", true, "JobQueueManager");
		I_ASSIGN(m_agentRegistryCompPtr, "AgentRegistry", "Registry of the processing agents used to perform the jobs", true, "AgentRegistry");
		I_ASSIGN(m_pollingIntervalMsAttrPtr, "PollingIntervalMs", "Interval in milliseconds used for polling the job queue", false, 1000);
	I_END_COMPONENT;

	CDistributedJobExecutionControllerComp();

	// reimplemented (imthype::IProcessingAgent::IExecutionObserver)
	virtual void OnTaskProgressChanged(const QByteArray& jobId, double progress) override;
	virtual void OnTaskFinished(const QByteArray& jobId, const IJobOutput& output) override;

protected:
	// reimplemented (icomp::CComponentBase)
	virtual void OnComponentCreated() override;
	virtual void OnComponentDestroyed() override;

protected Q_SLOTS:
	/**
		Forward the queued jobs to the available agents.
	*/
	void OnJobPolling();
	void OnJobFinished(const QByteArray& jobId, const imthype::CStandardJobOutput& results);
	void OnJobProgressChanged(const QByteArray& jobId, double progress);

Q_SIGNALS:
	void EmitJobFinished(const QByteArray& jobId, const imthype::CStandardJobOutput& results);
	void EmitJobProgressChanged(const QByteArray& jobId, double progress);

protected:
	void OnJobQueueChanged(const istd::IChangeable::ChangeSet& changeset, const imthype::IJobQueueManager* modelPtr);

	/**
		Try to dispatch the given waiting job to an agent.
		\return \c true if the job was dispatched or \c false otherwise.
	*/
	bool TryDispatchJob(const QByteArray& jobId, const QByteArray& taskTypeId);

private:
	/**
		Manager for the job queue.
	*/
	I_REF(IJobQueueManager, m_jobQueueManagerCompPtr);

	/**
		Registry of the available processing agents.
	*/
	I_REF(IProcessingAgentRegistry, m_agentRegistryCompPtr);

	I_ATTR(int, m_pollingIntervalMsAttrPtr);

	/**
		Timer used for polling the job queue.
	*/
	QTimer m_updateJobQueueTimer;

	/**
		Flag used for stopping the dispatching of new jobs.
	*/
	bool m_stopDispatching;

	/**
		Protects the map of the dispatched jobs.
	*/
	mutable QMutex m_dispatchedJobsMutex;

	/**
		Agent to which a job was dispatched, keyed by the job ID.
	*/
	QMap<QByteArray, IProcessingAgent*> m_dispatchedJobs;

	imtbase::TModelUpdateBinder<imthype::IJobQueueManager, CDistributedJobExecutionControllerComp> m_jobQueueObserver;
};


} // namespace imthype
