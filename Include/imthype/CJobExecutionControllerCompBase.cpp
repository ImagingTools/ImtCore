// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imthype/CJobExecutionControllerCompBase.h>


// ImtCore includes
#include <imtbase/CSimpleReferenceCollection.h>


namespace imthype
{


// public methods

CJobExecutionControllerCompBase::CJobExecutionControllerCompBase()
	:m_stopAllTasks(false),
	m_jobQueueObserver(*this)
{
	qRegisterMetaType<imthype::CStandardJobOutput>("imthype::CStandardJobOutput");

	connect(this, &CJobExecutionControllerCompBase::EmitJobFinished, this, &CJobExecutionControllerCompBase::OnJobFinished, Qt::QueuedConnection);
	connect(this, &CJobExecutionControllerCompBase::EmitJobProgressChanged, this, &CJobExecutionControllerCompBase::OnJobProgressChanged, Qt::QueuedConnection);
}


// protected methods

void CJobExecutionControllerCompBase::ShutdownAllTasks()
{
	m_stopAllTasks = true;
}


// reimplemented (icomp::CComponentBase)

void CJobExecutionControllerCompBase::OnComponentCreated()
{
	BaseClass::OnComponentCreated();

	int idealThreadCount = QThread::idealThreadCount();

	if (idealThreadCount > 0){
		m_threadPool.setMaxThreadCount(idealThreadCount / 2);

		SendVerboseMessage(QString("Job processing will be started on %1 cores").arg(idealThreadCount / 2));
	}

	connect(&m_updateJobQueueTimer, SIGNAL(timeout()), this, SLOT(OnJobPolling()));

	m_updateJobQueueTimer.start(std::chrono::seconds(1));

	if (m_jobQueueManagerCompPtr.IsValid()){
		m_jobQueueObserver.RegisterObject(m_jobQueueManagerCompPtr.GetPtr(), &CJobExecutionControllerCompBase::OnJobQueueChanged);
	}
}


void CJobExecutionControllerCompBase::OnComponentDestroyed()
{
	m_jobQueueObserver.UnregisterAllObjects();

	ShutdownAllTasks();

	m_threadPool.waitForDone();

	BaseClass::OnComponentDestroyed();
}


// protected slots

void CJobExecutionControllerCompBase::OnJobPolling()
{
	Q_ASSERT(m_jobQueueManagerCompPtr.IsValid());

	if (m_stopAllTasks){
		SendVerboseMessage(QString("Job queue shutting down"));

		return;
	}

	imtbase::ICollectionInfo::Ids jobIds = m_jobQueueManagerCompPtr->GetElementIds();

	for (const imtbase::ICollectionInfo::Id& jobId : jobIds){
		QByteArray taskTypeId = m_jobQueueManagerCompPtr->GetTaskTypeId(jobId);

		const IJobProcessor* taskProcessorPtr = GetTaskProcessor(taskTypeId);
		if (taskProcessorPtr != nullptr){
			IJobQueueManager::ProcessingStatus jobStatus = m_jobQueueManagerCompPtr->GetProcessingStatus(jobId);

			if (jobStatus == IJobQueueManager::PS_WAITING_FOR_ACCEPTING){
				SendVerboseMessage(QString("Job accepted: %1").arg(jobId.constData()));
			
				m_jobQueueManagerCompPtr->SetProcessingStatus(jobId, IJobQueueManager::PS_WAITING_FOR_PROCESSING);
			}

			if (jobStatus == IJobQueueManager::PS_WAITING_FOR_PROCESSING){
				SendVerboseMessage(QString("Trying to start job: %1").arg(jobId.constData()));

				m_jobQueueManagerCompPtr->SetProcessingStatus(jobId, IJobQueueManager::PS_RUNNING);

				imtbase::CSimpleReferenceCollection inputs;
				imthype::IJobQueueManager::ParamsPtr processingParamsPtr;
				if (m_jobQueueManagerCompPtr->GetJobConfiguration(jobId, processingParamsPtr, inputs)){
					m_jobProgressList[jobId].reset(new JobProgressManager(*this, jobId));

					Task* runnablePtr = new Task(*this, jobId, *taskProcessorPtr, inputs, processingParamsPtr, m_jobProgressList[jobId].get());
					runnablePtr->setAutoDelete(true);

					if (!m_threadPool.tryStart(runnablePtr)){
						m_jobProgressList.remove(jobId);

						m_jobQueueManagerCompPtr->SetProcessingStatus(jobId, IJobQueueManager::PS_WAITING_FOR_PROCESSING);

						SendErrorMessage(0, QString("Job failed to start: %1").arg(jobId.constData()));
					}
					else{
						SendVerboseMessage(QString("Job started: %1").arg(jobId.constData()));
					}
				}
			}
		}
	}
}


void CJobExecutionControllerCompBase::OnJobFinished(const QByteArray& jobId, const CStandardJobOutput& results)
{
	Q_ASSERT(m_jobQueueManagerCompPtr.IsValid());

	m_jobQueueManagerCompPtr->SetJobResult(jobId, results);

	m_jobQueueManagerCompPtr->SetProcessingStatus(jobId, IJobQueueManager::PS_FINISHED);

	m_jobProgressList.remove(jobId);

	SendVerboseMessage(QString("Job finished: %1").arg(jobId.constData()));
}


void CJobExecutionControllerCompBase::OnJobProgressChanged(const QByteArray& jobId, double progress)
{
	Q_ASSERT(m_jobQueueManagerCompPtr.IsValid());

	m_jobQueueManagerCompPtr->SetProgress(jobId, progress);
}


// protected methods

void CJobExecutionControllerCompBase::OnJobQueueChanged(const istd::IChangeable::ChangeSet& changeset, const imthype::IJobQueueManager* /*modelPtr*/)
{
	if (changeset.GetChangeInfoMap().contains(imthype::IJobQueueManager::CN_JOB_STATUS_CHANGED)){
		QVariant value = changeset.GetChangeInfoMap().value(imthype::IJobQueueManager::CN_JOB_STATUS_CHANGED);
		imthype::IJobQueueManager::JobStatusInfo info = value.value<imthype::IJobQueueManager::JobStatusInfo>();

		if (m_jobProgressList.contains(info.elementId) && info.status == imthype::IJobQueueManager::PS_CANCELING){
			m_jobProgressList[info.elementId]->SetCanceled(true);
		}
	}
}


// public methods of the embedded class Task

CJobExecutionControllerCompBase::Task::Task(
			CJobExecutionControllerCompBase& parent,
			const QByteArray& jobId,
			const IJobProcessor& taskProcessor,
			const imtbase::IReferenceCollection& input,
			iprm::IParamsSetSharedPtr paramsPtr,
			JobProgressManager* progressPtr)
	:m_taskProcessor(taskProcessor),
	m_parent(parent),
	m_jobId(jobId),
	m_progressPtr(progressPtr)
{
	istd::IChangeableSharedPtr inputClonePtr(input.CloneMe());

	m_inputPtr.SetCastedPtr<istd::IChangeable>(inputClonePtr);

	m_paramsPtr = paramsPtr;
}


// reimplemented (QRunnable)

void CJobExecutionControllerCompBase::Task::run()
{
	QDateTime startTime = QDateTime::currentDateTime();

	m_jobOutput.SetStartTime(QDateTime::currentDateTime());
	int processingState = m_taskProcessor.ExecuteTask(*m_inputPtr, *m_paramsPtr, m_jobOutput, m_progressPtr);

	istd::IInformationProvider::InformationCategory result = istd::IInformationProvider::IC_NONE;

	switch (processingState){
		case iproc::IProcessor::TS_OK:
			result = istd::IInformationProvider::IC_INFO;
			
			if (m_jobOutput.GetInformationCategory() == istd::IInformationProvider::IC_WARNING){
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

	m_jobOutput.SetFinishedTime(QDateTime::currentDateTime());
	m_jobOutput.SetResultCategory(result);

	m_parent.EmitJobFinished(m_jobId, m_jobOutput);
}


// public methods of the embedded class JobProgressManager

CJobExecutionControllerCompBase::JobProgressManager::JobProgressManager(
	CJobExecutionControllerCompBase& parent,
	const QByteArray& jobId)
	:m_parent(parent),
	m_jobId(jobId)
{
}


void CJobExecutionControllerCompBase::JobProgressManager::OnProgressChanged(double cumulatedValue)
{
	m_parent.EmitJobProgressChanged(m_jobId, cumulatedValue);
}


} // namespace imthype


