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


bool CMessagesReadJobController::IsValidJobId(const QByteArray& jobId) const
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


bool CMessagesReadJobController::PopResult(const QByteArray& jobId, ilog::CMessageContainer& resultEvents)
{
	QMutexLocker locker(&m_jobMutex);

	JobList::iterator it = m_jobList.begin();
	while(it != m_jobList.end()){
		if ((*it).uuid == jobId && (*it).status == JS_FINISHED){
			if ((*it).eventContainerPtr.IsValid()){
				static istd::IChangeable::ChangeSet changeSet(IEventProvider::RS_OK);
				istd::CChangeGroup notifier(&resultEvents, &changeSet);

				resultEvents.CopyFrom(*(*it).eventContainerPtr);
			}
			else{
				static istd::IChangeable::ChangeSet changeSet(IEventProvider::RS_CANCELED);
				istd::CChangeGroup notifier(&resultEvents, &changeSet);
			}

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
		job.eventContainerPtr.SetPtr(new ilog::CMessageContainer());

		CMessagesReader::EventContainerList::const_iterator it = containerListPtr->begin();
		while (it != containerListPtr->end()){
			ilog::IMessageContainer::Messages messages = (*it)->GetMessages();
			for (int i = messages.count() - 1; i >= 0; i--){
				if (job.filterPtr != nullptr){
					if(		job.filterParams.GetFilterTimeRange().Contains(messages[i]->GetInformationTimeStamp()) &&
							job.filterPtr->IsMessageAccepted(*messages[i], &job.filterParams)){
						dynamic_cast<ilog::CMessageContainer*>(job.eventContainerPtr.GetPtr())->AddMessage(messages[i]);
					}
				}
				else{
					dynamic_cast<ilog::CMessageContainer*>(job.eventContainerPtr.GetPtr())->AddMessage(messages[i]);
				}
			}

			it++;
		}
	}

	job.status = JS_FINISHED;
	Q_EMIT JobFinished(job.uuid);
}


} // namespace imtlog


