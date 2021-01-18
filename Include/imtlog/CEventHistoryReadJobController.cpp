#include <imtlog/CEventHistoryReadJobController.h>


// Qt includes
#include <QtCore/QUuid>

// ACF includes
#include <istd/CChangeGroup.h>


namespace imtlog
{


// public methods

CEventHistoryReadJobController::CEventHistoryReadJobController(
			QString groupDir,
			QString containerExtension,
			QString archiveExtension,
			iser::IVersionInfo* versionInfoPtr,
			imtfile::IFileCompression* compressorPtr)
	:m_groupDir(groupDir),
	m_containerExtension(containerExtension),
	m_archiveExtension(archiveExtension),
	m_versionInfoPtr(versionInfoPtr),
	m_compressorPtr(compressorPtr)
{
	start();
}


QByteArray CEventHistoryReadJobController::AddJob(IEventProvider::EventFilterPtr filterPtr)
{
	Job newJob;;

	newJob.status = JS_WAITING;
	newJob.uuid = QUuid::createUuid().toByteArray();
	newJob.filterPtr = filterPtr;

	QMutexLocker locker(&m_jobMutex);

	m_jobList.push_back(newJob);

	return newJob.uuid;
}


bool CEventHistoryReadJobController::IsValidJobId(const QByteArray& jobId) const
{
	QMutexLocker locker(&m_jobMutex);

	JobList::const_iterator it = m_jobList.cbegin();
	while (it != m_jobList.end()){
		if ((*it).uuid == jobId){
			return true;
		}
	}

	return false;
}


IEventProvider::EventFilterPtr CEventHistoryReadJobController::GetFilter(const QByteArray & jobId) const
{
	QMutexLocker locker(&m_jobMutex);

	for (int i = 0; i < m_jobList.count(); i++){
		if (m_jobList[i].uuid == jobId){
			return m_jobList[i].filterPtr;
		}
	}

	return IEventProvider::EventFilterPtr();
}


bool CEventHistoryReadJobController::PopResult(const QByteArray& jobId, ilog::CMessageContainer& resultEvents)
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
	}

	return false;
}


// reimplemented (QThread)

void CEventHistoryReadJobController::run()
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

CEventHistoryReadJobController::Job CEventHistoryReadJobController::GetJob(const QByteArray& jobId) const
{
	QMutexLocker locker(&m_jobMutex);

	for (int i = 0; i < m_jobList.count(); i++){
		if (m_jobList[i].uuid == jobId){
			return m_jobList[i];
		}
	}

	return Job();
}


void CEventHistoryReadJobController::ProcessJob(Job& job)
{
	job.status = JS_RUNNING;

	CEventHistoryGroupReader m_reader(
				m_groupDir,
				m_containerExtension,
				m_archiveExtension,
				m_versionInfoPtr,
				m_compressorPtr);

	CEventHistoryGroupReader::EventContainerListPtr containerListPtr = m_reader.ReadContainers(job.filterPtr->GetTimeRange());

	if (containerListPtr.IsValid()){
		job.eventContainerPtr.SetPtr(new ilog::CMessageContainer());

		CEventHistoryGroupReader::EventContainerList::const_iterator it = containerListPtr->begin();
		while (it != containerListPtr->end()){
			ilog::IMessageContainer::Messages messages = (*it)->GetMessages();
			for (int i = messages.count() - 1; i >= 0; i--){
				if (job.filterPtr.IsValid()){
					if(		job.filterPtr->GetTimeRange().Contains(messages[i]->GetInformationTimeStamp()) &&
							job.filterPtr->IsMessageAccepted(messages[i].GetPtr())){
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


