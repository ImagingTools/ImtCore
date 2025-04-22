#include <imthype/CJobExecutionControllerCompBase.h>


// Qt includes
#include <QtCore/QStringList>
#include <QtCore/QFileInfo>
#include <QtCore/QElapsedTimer>

// ACF includes
#include <istd/CChangeGroup.h>

// ImtCore includes
#include <imtbase/CSimpleReferenceCollection.h>


namespace imthype
{


// public methods

CJobExecutionControllerCompBase::CJobExecutionControllerCompBase()
	:m_stopAllTasks(false)
{
	qRegisterMetaType<imthype::CStandardJobOutput>("imthype::CStandardJobOutput");

	connect(this, &CJobExecutionControllerCompBase::EmitJobFinished, this, &CJobExecutionControllerCompBase::OnJobFinished, Qt::QueuedConnection);
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

	m_updateJobQueueTimer.start(1000);
}


void CJobExecutionControllerCompBase::OnComponentDestroyed()
{
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
					Task* runnablePtr = new Task(*this, jobId, *taskProcessorPtr, inputs, processingParamsPtr);
					runnablePtr->setAutoDelete(true);

					if (!m_threadPool.tryStart(runnablePtr)){
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

	SendVerboseMessage(QString("Job finished: %1").arg(jobId.constData()));
}


// public methods of the embedded class Task

CJobExecutionControllerCompBase::Task::Task(
			CJobExecutionControllerCompBase& parent,
			const QByteArray& jobId,
			const IJobProcessor& taskProcessor,
			const imtbase::IReferenceCollection& input,
			const istd::TSmartPtr<iprm::IParamsSet>& paramsPtr)
	:m_taskProcessor(taskProcessor),
	m_parent(parent),
	m_jobId(jobId)
{
	m_inputPtr.SetCastedOrRemove(input.CloneMe());

	m_paramsPtr = paramsPtr;
}


// reimplemented (QRunnable)

void CJobExecutionControllerCompBase::Task::run()
{
	QDateTime startTime = QDateTime::currentDateTime();

	IJobQueueManager::ProcessingStatus jobStatus = IJobQueueManager::PS_NONE;

	m_jobOutput.SetStartTime(QDateTime::currentDateTime());

	int processingState = m_taskProcessor.ExecuteTask(*m_inputPtr, *m_paramsPtr, m_jobOutput);
	if (processingState == iproc::IProcessor::TS_CANCELED){
		jobStatus = IJobQueueManager::PS_CANCELED;
	}
	else{
		jobStatus = IJobQueueManager::PS_FINISHED;
	}

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


} // namespace imthype


