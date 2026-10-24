// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imthype/CLocalProcessingAgentComp.h>


// Qt includes
#include <QtCore/QDateTime>
#include <QtCore/QThread>

// ImtCore includes
#include <imtbase/CSimpleReferenceCollection.h>


namespace imthype
{


// public methods

CLocalProcessingAgentComp::CLocalProcessingAgentComp()
{
}


// reimplemented (imthype::IProcessingAgent)

QByteArray CLocalProcessingAgentComp::GetAgentId() const
{
	if (m_agentIdAttrPtr.IsValid() && !(*m_agentIdAttrPtr).isEmpty()){
		return *m_agentIdAttrPtr;
	}

	return QByteArray("LocalAgent");
}


IProcessingAgent::AgentState CLocalProcessingAgentComp::GetAgentState() const
{
	QMutexLocker locker(&m_runningTasksMutex);

	int runningCount = m_runningTasks.count();
	int maxCount = m_threadPool.maxThreadCount();

	if (runningCount <= 0){
		return AS_IDLE;
	}

	if ((maxCount > 0) && (runningCount >= maxCount)){
		return AS_FULL;
	}

	return AS_BUSY;
}


bool CLocalProcessingAgentComp::IsTaskSupported(const QByteArray& taskTypeId) const
{
	return GetTaskProcessor(taskTypeId) != nullptr;
}


bool CLocalProcessingAgentComp::DispatchTask(
			const QByteArray& jobId,
			const QByteArray& taskTypeId,
			const imtbase::IReferenceCollection& input,
			const iprm::IParamsSet& params,
			IExecutionObserver& observer)
{
	const IJobProcessor* taskProcessorPtr = GetTaskProcessor(taskTypeId);
	if (taskProcessorPtr == nullptr){
		SendWarningMessage(0, QString("Agent '%1' cannot process task type: %2").arg(GetAgentId().constData()).arg(taskTypeId.constData()));

		return false;
	}

	iprm::IParamsSetSharedPtr paramsPtr;
	istd::IChangeableSharedPtr paramsClonePtr(params.CloneMe());
	paramsPtr.SetCastedPtr<istd::IChangeable>(paramsClonePtr);

	TaskProgressManagerPtr progressManagerPtr(new TaskProgressManager(jobId, observer));

	{
		QMutexLocker locker(&m_runningTasksMutex);

		m_runningTasks[jobId] = progressManagerPtr;
	}

	Task* runnablePtr = new Task(*this, jobId, *taskProcessorPtr, input, paramsPtr, *progressManagerPtr, observer);
	runnablePtr->setAutoDelete(true);

	if (!m_threadPool.tryStart(runnablePtr)){
		delete runnablePtr;

		QMutexLocker locker(&m_runningTasksMutex);

		m_runningTasks.remove(jobId);

		SendErrorMessage(0, QString("Agent '%1' failed to start task: %2").arg(GetAgentId().constData()).arg(jobId.constData()));

		return false;
	}

	SendVerboseMessage(QString("Agent '%1' accepted task: %2").arg(GetAgentId().constData()).arg(jobId.constData()));

	return true;
}


bool CLocalProcessingAgentComp::CancelTask(const QByteArray& jobId)
{
	QMutexLocker locker(&m_runningTasksMutex);

	QMap<QByteArray, TaskProgressManagerPtr>::const_iterator foundIter = m_runningTasks.constFind(jobId);
	if (foundIter == m_runningTasks.constEnd()){
		return false;
	}

	foundIter.value()->SetCanceled(true);

	return true;
}


// protected methods

// reimplemented (icomp::CComponentBase)

void CLocalProcessingAgentComp::OnComponentCreated()
{
	BaseClass::OnComponentCreated();

	int maxParallelTasks = 0;
	if (m_maxParallelTasksAttrPtr.IsValid()){
		maxParallelTasks = *m_maxParallelTasksAttrPtr;
	}

	if (maxParallelTasks <= 0){
		int idealThreadCount = QThread::idealThreadCount();
		if (idealThreadCount > 0){
			maxParallelTasks = qMax(1, idealThreadCount / 2);
		}
		else{
			maxParallelTasks = 1;
		}
	}

	m_threadPool.setMaxThreadCount(maxParallelTasks);

	SendVerboseMessage(QString("Agent '%1' will process up to %2 task(s) in parallel").arg(GetAgentId().constData()).arg(maxParallelTasks));
}


void CLocalProcessingAgentComp::OnComponentDestroyed()
{
	{
		QMutexLocker locker(&m_runningTasksMutex);

		for (const TaskProgressManagerPtr& progressManagerPtr : m_runningTasks){
			progressManagerPtr->SetCanceled(true);
		}
	}

	m_threadPool.waitForDone();

	BaseClass::OnComponentDestroyed();
}


const IJobProcessor* CLocalProcessingAgentComp::GetTaskProcessor(const QByteArray& taskTypeId) const
{
	for (int i = 0; i < m_jobTasksCompPtr.GetCount(); ++i){
		IJobProcessor* processorPtr = m_jobTasksCompPtr[i];
		if (processorPtr != nullptr){
			if (processorPtr->GetTaskTypeId() == taskTypeId){
				return processorPtr;
			}
		}
	}

	return nullptr;
}


void CLocalProcessingAgentComp::OnTaskExecutionFinished(const QByteArray& jobId)
{
	QMutexLocker locker(&m_runningTasksMutex);

	m_runningTasks.remove(jobId);
}


// embedded class TaskProgressManager

CLocalProcessingAgentComp::TaskProgressManager::TaskProgressManager(
			const QByteArray& jobId,
			IExecutionObserver& observer)
	:m_jobId(jobId),
	m_observer(observer)
{
}


void CLocalProcessingAgentComp::TaskProgressManager::OnProgressChanged(double cumulatedValue)
{
	m_observer.OnTaskProgressChanged(m_jobId, cumulatedValue);
}


// embedded class Task

CLocalProcessingAgentComp::Task::Task(
			CLocalProcessingAgentComp& parent,
			const QByteArray& jobId,
			const IJobProcessor& taskProcessor,
			const imtbase::IReferenceCollection& input,
			iprm::IParamsSetSharedPtr paramsPtr,
			TaskProgressManager& progressManager,
			IExecutionObserver& observer)
	:m_parent(parent),
	m_taskProcessor(taskProcessor),
	m_jobId(jobId),
	m_paramsPtr(paramsPtr),
	m_progressManager(progressManager),
	m_observer(observer)
{
	istd::IChangeableSharedPtr inputClonePtr(input.CloneMe());

	m_inputPtr.SetCastedPtr<istd::IChangeable>(inputClonePtr);
}


void CLocalProcessingAgentComp::Task::run()
{
	CStandardJobOutput jobOutput;

	jobOutput.SetStartTime(QDateTime::currentDateTime());

	int processingState = iproc::IProcessor::TS_INVALID;
	if (m_inputPtr.IsValid() && m_paramsPtr.IsValid()){
		processingState = m_taskProcessor.ExecuteTask(*m_inputPtr, *m_paramsPtr, jobOutput, &m_progressManager);
	}

	istd::IInformationProvider::InformationCategory result = istd::IInformationProvider::IC_NONE;

	switch (processingState){
		case iproc::IProcessor::TS_OK:
			result = istd::IInformationProvider::IC_INFO;

			if (jobOutput.GetInformationCategory() == istd::IInformationProvider::IC_WARNING){
				result = istd::IInformationProvider::IC_WARNING;
			}
			break;

		case iproc::IProcessor::TS_INVALID:
			result = istd::IInformationProvider::IC_ERROR;
			break;

		default:
			result = istd::IInformationProvider::IC_NONE;
			break;
	}

	jobOutput.SetFinishedTime(QDateTime::currentDateTime());
	jobOutput.SetResultCategory(result);

	m_observer.OnTaskFinished(m_jobId, jobOutput);

	m_parent.OnTaskExecutionFinished(m_jobId);
}


} // namespace imthype
