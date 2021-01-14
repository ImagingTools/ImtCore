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
}


QByteArray CEventHistoryReadJobController::AddJob(IEventProvider::EventFilterPtr filterPtr)
{
	Job* newJobPtr = new Job();
	newJobPtr->status = JS_WAITING;
	newJobPtr->uuid = QUuid::createUuid().toByteArray();
	newJobPtr->filterPtr = filterPtr;

	QMutexLocker locker(&m_jobMutex);
	m_jobList.push_back(newJobPtr);
	start();

	return newJobPtr->uuid;
}


bool CEventHistoryReadJobController::IsValidJobId(const QByteArray& jobId) const
{
	QMutexLocker locker(&m_jobMutex);

	JobList::const_iterator it = m_jobList.cbegin();
	while (it != m_jobList.end()){
		if ((*it)->uuid == jobId){
			return true;
		}
	}

	return false;
}


IEventProvider::EventFilterPtr CEventHistoryReadJobController::GetFilter(const QByteArray & jobId) const
{
	QMutexLocker locker(&m_jobMutex);

	for (int i = 0; i < m_jobList.count(); i++){
		if (m_jobList[i]->uuid == jobId){
			return m_jobList[i]->filterPtr;
		}
	}

	return IEventProvider::EventFilterPtr();
}


bool CEventHistoryReadJobController::GetResult(const QByteArray& jobId, ilog::CMessageContainer& resultMessages)
{
	QMutexLocker locker(&m_jobMutex);

	JobList::iterator it = m_jobList.begin();
	while(it != m_jobList.end()){
		if ((*it)->uuid == jobId && (*it)->status == JS_FINISHED){
			if ((*it)->eventContainerPtr.IsValid()){
				static istd::IChangeable::ChangeSet changeSet(IEventProvider::RS_OK);
				istd::CChangeGroup notifier(&resultMessages, &changeSet);

				resultMessages.CopyFrom(*(*it)->eventContainerPtr);
			}
			else{
				static istd::IChangeable::ChangeSet changeSet(IEventProvider::RS_CANCELED);
				istd::CChangeGroup notifier(&resultMessages, &changeSet);
			}

			delete *it;
			m_jobList.erase(it);
			return true;
		}
	}

	return false;
}


// reimplemented (QThread)

void CEventHistoryReadJobController::run()
{
	Job* jobPtr;
	{
		jobPtr = nullptr;

		{
			QMutexLocker locker(&m_jobMutex);

			for (int i = 0; i < m_jobList.count(); i++){
				if (m_jobList[i]->status == JS_WAITING){
					jobPtr = m_jobList[i];
					break;
				}
			}
		}

		if (jobPtr != nullptr){
			ProcessJob(jobPtr);
		}
	} while (jobPtr != nullptr);
}


// private methods

CEventHistoryReadJobController::Job* CEventHistoryReadJobController::GetJob(const QByteArray& jobId) const
{
	QMutexLocker locker(&m_jobMutex);

	for (int i = 0; i < m_jobList.count(); i++){
		if (m_jobList[i]->uuid == jobId){
			return m_jobList[i];
		}
	}

	return nullptr;
}


void CEventHistoryReadJobController::ProcessJob(Job* jobPtr)
{
	jobPtr->status = JS_RUNNING;

	CEventHistoryGroupReader m_reader(
				m_groupDir,
				m_containerExtension,
				m_archiveExtension,
				m_versionInfoPtr,
				m_compressorPtr);

	CEventHistoryGroupReader::EventContainerListPtr containerListPtr =
				m_reader.ReadContainers(jobPtr->filterPtr->GetTimeRange());

	if (containerListPtr.IsValid()){
		jobPtr->eventContainerPtr.SetPtr(new ilog::CMessageContainer());

		CEventHistoryGroupReader::EventContainerList::const_iterator it = containerListPtr->begin();
		while (it != containerListPtr->end()){
			ilog::IMessageContainer::Messages messages = (*it)->GetMessages();
			for (int i = messages.count() - 1; i >= 0; i--){
				if (jobPtr->filterPtr.IsValid()){
					if(jobPtr->filterPtr->GetTimeRange().Contains(messages[i]->GetInformationTimeStamp()) &&
								jobPtr->filterPtr->IsMessageAccepted(messages[i].GetPtr())){
						jobPtr->eventContainerPtr->AddMessage(messages[i]);
					}
				}
				else{
					jobPtr->eventContainerPtr->AddMessage(messages[i]);
				}
			}

			it++;
		}
	}

	jobPtr->status = JS_FINISHED;
	Q_EMIT JobFinished(jobPtr->uuid);
}


} // namespace imtlog


