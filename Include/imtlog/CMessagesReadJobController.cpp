#include <imtlog/CMessagesReadJobController.h>


// Qt includes
#include <QtCore/QUuid>

// ACF includes
#include <istd/CChangeGroup.h>


namespace imtlog
{


// public methods

CMessagesReadJobController::CMessagesReadJobController(
			QString dir,
			QString containerExtension,
			QString archiveExtension,
			iser::IVersionInfo* versionInfoPtr,
			imtfile::IFileCompression* compressorPtr,
			ilog::IMessageConsumer* logPtr)
	:m_dir(dir),
	m_containerExtension(containerExtension),
	m_archiveExtension(archiveExtension),
	m_versionInfoPtr(versionInfoPtr),
	m_compressorPtr(compressorPtr),
	m_logPtr(logPtr)
{
	start();
}


QByteArray CMessagesReadJobController::AddJob(const IEventFilter* filterPtr, const IMessageFilterParams* filterParamsPtr)
{
	Job newJob;

	newJob.status = JS_WAITING;
	newJob.uuid = QUuid::createUuid().toByteArray();
	newJob.filterPtr = filterPtr;
	newJob.filterParams.CopyFrom(*filterParamsPtr);

	QMutexLocker locker(&m_jobMutex);

	m_jobList.push_back(newJob);

	return newJob.uuid;
}


bool CMessagesReadJobController::IsJobExists(const QByteArray& jobId) const
{
	QMutexLocker locker(&m_jobMutex);

	JobList::const_iterator it = m_jobList.cbegin();
	while (it != m_jobList.end()){
		if ((*it).uuid == jobId){
			return true;
		}

		it++;
	}

	return false;
}


bool CMessagesReadJobController::TakeJobResult(const QByteArray& jobId, CMessagesReader::EventContainerPtr& resultEvents)
{
	QMutexLocker locker(&m_jobMutex);

	JobList::iterator it = m_jobList.begin();
	while(it != m_jobList.end()){
		if (it->uuid == jobId && it->status == JS_FINISHED){
			resultEvents = it->containerPtr;
			m_jobList.erase(it);

			return true;
		}

		it++;
	}

	return false;
}


// reimplemented (QThread)

void CMessagesReadJobController::run()
{
	while (!isInterruptionRequested()){
		Job workingJob;

		{
			QMutexLocker locker(&m_jobMutex);

			for (int i = 0; i < m_jobList.count(); i++){
				if (m_jobList[i].status == JS_WAITING){
					workingJob = m_jobList[i];
					break;
				}
			}
		}

		if (!workingJob.uuid.isEmpty()){
			ProcessJob(workingJob);
		}

		msleep(1);
	}
}


// private methods

void CMessagesReadJobController::ProcessJob(Job& job)
{
	job.status = JS_RUNNING;

	CMessagesReader m_reader(
				m_dir,
				m_containerExtension,
				m_archiveExtension,
				m_versionInfoPtr,
				m_compressorPtr,
				m_logPtr);

	CMessagesReader::EventContainerListPtr containerListPtr = m_reader.ReadContainers(job.filterParams.GetFilterTimeRange());

	if (containerListPtr.IsValid()){
		job.containerPtr.SetPtr(new CEventContainer());

		CMessagesReader::EventContainerList::const_iterator it = containerListPtr->begin();
		while (it != containerListPtr->end()){
			ilog::IMessageContainer::Messages messages = (*it)->GetMessages();
			for (ilog::IMessageContainer::Messages::reverse_iterator rit = messages.rbegin(); rit != messages.rend(); rit++) {
				if (job.filterPtr != nullptr){
					if(		job.filterParams.GetFilterTimeRange().Contains((*rit)->GetInformationTimeStamp()) &&
							job.filterPtr->IsMessageAccepted(*(*rit), &job.filterParams)){
						job.containerPtr->AddMessage((*rit));
					}
				}
				else{
					job.containerPtr->AddMessage((*rit));
				}
			}

			it++;
		}
	}

	job.status = JS_FINISHED;
	Q_EMIT JobFinished(job.uuid);
}


} // namespace imtlog


