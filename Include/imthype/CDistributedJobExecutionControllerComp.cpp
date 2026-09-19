// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imthype/CDistributedJobExecutionControllerComp.h>


// ImtCore includes
#include <imtbase/CSimpleReferenceCollection.h>


namespace imthype
{


// public methods

CDistributedJobExecutionControllerComp::CDistributedJobExecutionControllerComp()
	:m_stopDispatching(false),
	m_jobQueueObserver(*this)
{
	qRegisterMetaType<imthype::CStandardJobOutput>("imthype::CStandardJobOutput");

	connect(this, &CDistributedJobExecutionControllerComp::EmitJobFinished, this, &CDistributedJobExecutionControllerComp::OnJobFinished, Qt::QueuedConnection);
	connect(this, &CDistributedJobExecutionControllerComp::EmitJobProgressChanged, this, &CDistributedJobExecutionControllerComp::OnJobProgressChanged, Qt::QueuedConnection);
}


// reimplemented (imthype::IProcessingAgent::IExecutionObserver)

void CDistributedJobExecutionControllerComp::OnTaskProgressChanged(const QByteArray& jobId, double progress)
{
	// The callback may come from a foreign thread, so marshal it to the controller thread.
	emit EmitJobProgressChanged(jobId, progress);
}


void CDistributedJobExecutionControllerComp::OnTaskFinished(const QByteArray& jobId, const IJobOutput& output)
{
	CStandardJobOutput results;
	results.CopyFrom(output);

	// The callback may come from a foreign thread, so marshal it to the controller thread.
	emit EmitJobFinished(jobId, results);
}


// reimplemented (icomp::CComponentBase)

void CDistributedJobExecutionControllerComp::OnComponentCreated()
{
	BaseClass::OnComponentCreated();

	int pollingIntervalMs = 1000;
	if (m_pollingIntervalMsAttrPtr.IsValid() && (*m_pollingIntervalMsAttrPtr > 0)){
		pollingIntervalMs = *m_pollingIntervalMsAttrPtr;
	}

	connect(&m_updateJobQueueTimer, SIGNAL(timeout()), this, SLOT(OnJobPolling()));

	m_updateJobQueueTimer.start(pollingIntervalMs);

	if (m_jobQueueManagerCompPtr.IsValid()){
		m_jobQueueObserver.RegisterObject(m_jobQueueManagerCompPtr.GetPtr(), &CDistributedJobExecutionControllerComp::OnJobQueueChanged);
	}

	SendVerboseMessage(QString("Distributed job execution controller started (polling interval: %1 ms)").arg(pollingIntervalMs));
}


void CDistributedJobExecutionControllerComp::OnComponentDestroyed()
{
	m_stopDispatching = true;

	m_updateJobQueueTimer.stop();

	m_jobQueueObserver.UnregisterAllObjects();

	{
		QMutexLocker locker(&m_dispatchedJobsMutex);

		m_dispatchedJobs.clear();
	}

	BaseClass::OnComponentDestroyed();
}


// protected slots

void CDistributedJobExecutionControllerComp::OnJobPolling()
{
	Q_ASSERT(m_jobQueueManagerCompPtr.IsValid());

	if (m_stopDispatching){
		SendVerboseMessage(QString("Job dispatching is shutting down"));

		return;
	}

	if (!m_agentRegistryCompPtr.IsValid()){
		return;
	}

	imtbase::ICollectionInfo::Ids jobIds = m_jobQueueManagerCompPtr->GetElementIds();

	for (const imtbase::ICollectionInfo::Id& jobId : jobIds){
		QByteArray taskTypeId = m_jobQueueManagerCompPtr->GetTaskTypeId(jobId);

		IJobQueueManager::ProcessingStatus jobStatus = m_jobQueueManagerCompPtr->GetProcessingStatus(jobId);

		if (jobStatus == IJobQueueManager::PS_WAITING_FOR_ACCEPTING){
			SendVerboseMessage(QString("Job accepted: %1").arg(jobId.constData()));

			m_jobQueueManagerCompPtr->SetProcessingStatus(jobId, IJobQueueManager::PS_WAITING_FOR_PROCESSING);

			jobStatus = IJobQueueManager::PS_WAITING_FOR_PROCESSING;
		}

		if (jobStatus == IJobQueueManager::PS_WAITING_FOR_PROCESSING){
			TryDispatchJob(jobId, taskTypeId);
		}
	}
}


void CDistributedJobExecutionControllerComp::OnJobFinished(const QByteArray& jobId, const CStandardJobOutput& results)
{
	Q_ASSERT(m_jobQueueManagerCompPtr.IsValid());

	m_jobQueueManagerCompPtr->SetJobResult(jobId, results);

	m_jobQueueManagerCompPtr->SetProcessingStatus(jobId, IJobQueueManager::PS_FINISHED);

	{
		QMutexLocker locker(&m_dispatchedJobsMutex);

		m_dispatchedJobs.remove(jobId);
	}

	SendVerboseMessage(QString("Job finished: %1").arg(jobId.constData()));
}


void CDistributedJobExecutionControllerComp::OnJobProgressChanged(const QByteArray& jobId, double progress)
{
	Q_ASSERT(m_jobQueueManagerCompPtr.IsValid());

	m_jobQueueManagerCompPtr->SetProgress(jobId, progress);
}


// protected methods

bool CDistributedJobExecutionControllerComp::TryDispatchJob(const QByteArray& jobId, const QByteArray& taskTypeId)
{
	{
		QMutexLocker locker(&m_dispatchedJobsMutex);

		if (m_dispatchedJobs.contains(jobId)){
			// Job was already dispatched to an agent.
			return false;
		}
	}

	IProcessingAgent* agentPtr = m_agentRegistryCompPtr->FindAgentForTask(taskTypeId);
	if (agentPtr == nullptr){
		// No agent is currently available, retry on the next polling cycle.
		return false;
	}

	imtbase::CSimpleReferenceCollection inputs;
	imthype::IJobQueueManager::ParamsPtr processingParamsPtr;
	if (!m_jobQueueManagerCompPtr->GetJobConfiguration(jobId, processingParamsPtr, inputs)){
		SendErrorMessage(0, QString("Job configuration could not be retrieved: %1").arg(jobId.constData()));

		return false;
	}

	if (!processingParamsPtr.IsValid()){
		SendErrorMessage(0, QString("Job has no processing parameters: %1").arg(jobId.constData()));

		return false;
	}

	m_jobQueueManagerCompPtr->SetProcessingStatus(jobId, IJobQueueManager::PS_RUNNING);

	{
		QMutexLocker locker(&m_dispatchedJobsMutex);

		m_dispatchedJobs[jobId] = agentPtr;
	}

	SendVerboseMessage(QString("Dispatching job %1 to agent '%2'").arg(jobId.constData()).arg(agentPtr->GetAgentId().constData()));

	if (!agentPtr->DispatchTask(jobId, taskTypeId, inputs, *processingParamsPtr, *this)){
		{
			QMutexLocker locker(&m_dispatchedJobsMutex);

			m_dispatchedJobs.remove(jobId);
		}

		m_jobQueueManagerCompPtr->SetProcessingStatus(jobId, IJobQueueManager::PS_WAITING_FOR_PROCESSING);

		SendErrorMessage(0, QString("Agent '%1' rejected job: %2").arg(agentPtr->GetAgentId().constData()).arg(jobId.constData()));

		return false;
	}

	SendVerboseMessage(QString("Job dispatched: %1").arg(jobId.constData()));

	return true;
}


void CDistributedJobExecutionControllerComp::OnJobQueueChanged(const istd::IChangeable::ChangeSet& changeset, const imthype::IJobQueueManager* /*modelPtr*/)
{
	if (changeset.GetChangeInfoMap().contains(imthype::IJobQueueManager::CN_JOB_STATUS_CHANGED)){
		QVariant value = changeset.GetChangeInfoMap().value(imthype::IJobQueueManager::CN_JOB_STATUS_CHANGED);
		imthype::IJobQueueManager::JobStatusInfo info = value.value<imthype::IJobQueueManager::JobStatusInfo>();

		if (info.status == imthype::IJobQueueManager::PS_CANCELING){
			IProcessingAgent* agentPtr = nullptr;

			{
				QMutexLocker locker(&m_dispatchedJobsMutex);

				QMap<QByteArray, IProcessingAgent*>::const_iterator foundIter = m_dispatchedJobs.constFind(info.elementId);
				if (foundIter != m_dispatchedJobs.constEnd()){
					agentPtr = foundIter.value();
				}
			}

			if (agentPtr != nullptr){
				SendVerboseMessage(QString("Cancelling job %1 on agent '%2'").arg(info.elementId.constData()).arg(agentPtr->GetAgentId().constData()));

				agentPtr->CancelTask(info.elementId);
			}
		}
	}
}


} // namespace imthype
