#include <imtlog/CEventHistoryGroupController.h>


// Qt includes
#include <QtCore/QDir>
#include <QtCore/QUuid>

// ACF includes
#include <ifile/CCompactXmlFileWriteArchive.h>
#include <ifile/CCompactXmlFileReadArchive.h>
#include <iser/CPrimitiveTypesSerializer.h>
#include <istd/CSystem.h>
#include <istd/CChangeGroup.h>


namespace imtlog
{


// public methods

CEventHistoryGroupController::CEventHistoryGroupController(
			const EventHistoryGroupControllerParams& controllerParams,
			const EventHistoryGroupPersistenceParams& persistenceParams)
	:m_controllerParams(controllerParams),
	m_persistenceParams(persistenceParams),
	m_writer(this),
	m_writerState(TS_IDLE),
	m_completeGroupReader(
				persistenceParams.repositoryDir + "/" + m_persistenceParams.groupDir,
				persistenceParams.containerExtension,
				persistenceParams.archiveExtension,
				persistenceParams.versionInfoPtr,
				persistenceParams.compressorPtr)
{
	qRegisterMetaType<MessagePtr>("MessagePtr");
	qRegisterMetaType<EventContainerPtr>("EventContainerPtr");

	m_timer.setInterval(1000);
	connect(&m_timer, &QTimer::timeout, this, &CEventHistoryGroupController::OnTimer);
	m_timer.start();

	m_writer.SetParams(persistenceParams);

	m_state = CS_OK;

	connect(&m_readJobController, &CReadJobController::JobFinished, this, &CEventHistoryGroupController::OnJobFinished);
}


void CEventHistoryGroupController::OnSystemShutdown()
{
	m_state = CS_SHUTDOWN;

	m_timer.stop();

	{
		QMutexLocker workingQueueLocker(&m_workingQueueMutex);
		QMutexLocker writingQueueLocker(&m_writingQueueMutex);

		while (!m_workingQueue.isEmpty()){
			CEventHistoryGroupReader::EventContainerPtr containerPtr = m_workingQueue.takeFirst();
			if (!containerPtr->GetMessages().isEmpty()){
				containerPtr->SetTimeRange(containerPtr->GetMessagesTimeRange());
				m_writingQueue.append(containerPtr);
			}
		}
	}

	StartWriter();
	m_writer.wait();
}


// reimplemented (imtlog::ITimeRangeProvider)

CTimeRange CEventHistoryGroupController::GetTimeRange() const
{
	if (m_archiveTimeRange.IsNull()){
		m_archiveTimeRange = m_completeGroupReader.ReadGroupTimeRange();
	}

	return m_archiveTimeRange;
}


// reimplemented (imtlog::IMessageFilter)

IEventProvider::EventContainerPtr CEventHistoryGroupController::GetEvents(IEventProvider::EventFilterPtr filterPtr) const
{
	QByteArray jobId = m_readJobController.AddJob(
				filterPtr,
				m_persistenceParams.repositoryDir + "/" + m_persistenceParams.groupDir,
				m_persistenceParams.containerExtension,
				m_persistenceParams.archiveExtension,
				m_persistenceParams.versionInfoPtr,
				m_persistenceParams.compressorPtr);
	if (jobId.isEmpty()){
		return IEventProvider::EventContainerPtr();
	}

	EventContainerPtr eventContainerPtr(new imod::TModelWrap<ilog::CMessageContainer>);

	m_jobs[jobId] = eventContainerPtr;

	return eventContainerPtr;
}


// reimplemented (ilog::IMessageConsumer)

bool CEventHistoryGroupController::IsMessageSupported(
			int /*messageCategory*/,
			int /*messageId*/,
			const istd::IInformationProvider* /*messagePt*/) const
{
	return true;
}


void CEventHistoryGroupController::AddMessage(const MessagePtr& messagePtr)
{
	if (m_state == CS_OK){
		QDateTime timestamp = messagePtr->GetInformationTimeStamp();

		CEventHistoryGroupReader::EventContainerPtr containerPtr = GetContainerForMessage(timestamp);
		containerPtr->AddMessage(messagePtr);

		if (m_archiveTimeRange.GetBeginTime().isValid()){
			if (m_archiveTimeRange.GetBeginTime() > timestamp){
				istd::CChangeNotifier notifier(this);
				m_archiveTimeRange.SetBeginTime(timestamp);
			}

			if (m_archiveTimeRange.GetEndTime() < timestamp){
				istd::CChangeNotifier notifier(this);
				m_archiveTimeRange.SetEndTime(timestamp);
			}
		}
		else{
			istd::CChangeNotifier notifier(this);
			m_archiveTimeRange.SetTimeRange(timestamp, timestamp);
		}
	}
}


// reimplemented (ilog::ILoggable)

void CEventHistoryGroupController::SetLogPtr(ilog::IMessageConsumer* logPtr)
{
	BaseClass::SetLogPtr(logPtr);

	m_completeGroupReader.SetLogPtr(logPtr);
}


// private slots

void CEventHistoryGroupController::OnTimer()
{
	QMutexLocker workingQueueLocker(&m_workingQueueMutex);
	QMutexLocker writingQueueLocker(&m_writingQueueMutex);

	for (CEventHistoryGroupReader::EventContainerPtr containerPtr : m_workingQueue){
		Q_ASSERT(containerPtr->GetMessagesCount() > 0);
		if (containerPtr->GetTimeRange().GetEndTime().addSecs(m_controllerParams.writeDelay) < QDateTime::currentDateTime()){

		}
	}

	CEventHistoryGroupReader::EventContainerList::iterator it = m_workingQueue.begin();
	while (it != m_workingQueue.end()){
		CEventHistoryGroupReader::EventContainerPtr containerPtr = *it;

		if (containerPtr->GetTimeRange().GetEndTime().addSecs(m_controllerParams.writeDelay) < QDateTime::currentDateTime()){
			containerPtr->SetTimeRange(containerPtr->GetMessagesTimeRange());
			m_writingQueue.append(containerPtr);
		}

		if (containerPtr->GetTimeRange().GetEndTime().addSecs(m_controllerParams.removeDelay) < QDateTime::currentDateTime()){
			it = m_workingQueue.erase(it);
			continue;
		}

		it++;
	}

	if (!m_writingQueue.isEmpty()){
		StartWriter();
	}
}


void CEventHistoryGroupController::OnJobFinished(const QByteArray& jobId)
{
	EventContainerPtr resultContainerPtr;

	{
		QMutexLocker locker(&m_jobMutex);

		Q_ASSERT(m_jobs.contains(jobId));
		resultContainerPtr = m_jobs.take(jobId);
	}

	m_readJobController.GetResult(jobId, *resultContainerPtr);
}


// private methods

CEventHistoryGroupReader::EventContainerPtr CEventHistoryGroupController::GetContainerForMessage(const QDateTime& timestamp)
{
	QMutexLocker workingQueueLocker(&m_workingQueueMutex);

	for (CEventHistoryGroupReader::EventContainerPtr containerPtr : m_workingQueue){
		if (containerPtr->GetTimeRange().Contains(timestamp)){
			return containerPtr;
		}
	}

	if (!m_workingQueue.isEmpty()){
		Q_ASSERT(timestamp > m_workingQueue.last()->GetTimeRange().GetEndTime());
	}

	// Prepare new container

	CEventHistoryGroupReader::EventContainerPtr containerPtr(new CEventContainer);

	QDateTime beginTime = timestamp;
	QDateTime endTime = beginTime.addMSecs(m_controllerParams.containerDuration * 1000 - 1);

	if (endTime.date() != beginTime.date()){
		endTime = QDateTime(beginTime.date(), QTime(23, 59, 59, 999));
	}

	containerPtr->SetTimeRange(CTimeRange(beginTime, endTime));

	m_workingQueue.append(containerPtr);

	return containerPtr;
}


void CEventHistoryGroupController::StartWriter()
{
	if (m_writerState == TS_IDLE){
		m_writerState = TS_RUNNING;
		m_writer.start();
	}
	else{
		m_writerState = TS_PENDING;
	}
}


void CEventHistoryGroupController::OnWriterFinished()
{
	if (m_writerState == TS_PENDING){
		m_writerState = TS_IDLE;
		StartWriter();
	}
	else{
		m_writerState = TS_IDLE;
	}
}


// public methods of embedded class Writer

CEventHistoryGroupController::Writer::Writer(CEventHistoryGroupController* parentPtr)
	:m_parentPtr(parentPtr)
{
}


// private methods of embedded class Writer

// reimplemented (QThread)

void CEventHistoryGroupController::Writer::SetParams(const EventHistoryGroupPersistenceParams& params)
{
	m_params = params;
}


void CEventHistoryGroupController::Writer::run()
{
	if (m_params.repositoryDir.isEmpty()){
		QMutexLocker writingQueueLocker(&m_parentPtr->m_writingQueueMutex);
		m_parentPtr->m_writingQueue.clear();

		return;
	}

	while (!m_parentPtr->m_writingQueue.isEmpty()){
		if (isInterruptionRequested()){
			break;
		}

		CEventHistoryGroupReader::EventContainerPtr containerPtr;
		{
			QMutexLocker writingQueueLocker(&m_parentPtr->m_writingQueueMutex);
			containerPtr = m_parentPtr->m_writingQueue.takeFirst();
		}

		Q_ASSERT(containerPtr.IsValid());

		if (containerPtr->GetMessagesCount() == 0){
			continue;
		}

		QString uuid = QUuid::createUuid().toString();
		QString tempDir = QDir::tempPath() + "/ImtCore/" + uuid;
		if (istd::CSystem::EnsurePathExists(tempDir)){
			do{
				QString containerFileBaseName = containerPtr->GetTimeRange().GetBeginTime().toString("dd.MM.yyyy hh-mm-ss.zzz");
				QString containerFileName = containerFileBaseName + "." + m_params.containerExtension;
				QString containerPath = tempDir + "/" + containerFileName;
				ifile::CCompactXmlFileWriteArchive xmlArchive(containerPath, m_params.versionInfoPtr);

				if (!containerPtr->Serialize(xmlArchive)){
					m_parentPtr->SendErrorMessage(
								0, tr("Unable to serialize message history container \"%1\". Event container skipped")
								.arg(containerPath));

					break;
				}

				xmlArchive.Flush();

				QString archiveFileName = containerFileBaseName + "." + m_params.archiveExtension;
				QString archivePath = tempDir + "/" + archiveFileName;

				if (m_params.compressorPtr != nullptr){
					if (!m_params.compressorPtr->CompressFile(containerPath, archivePath)){
						m_parentPtr->SendErrorMessage(
									0, tr("Unable to compress message history container \"%1\". Event container skipped")
									.arg(archivePath));

						break;
					}
				}

				QString outPath =
							m_params.repositoryDir + "/" +
							m_params.groupDir + "/" +
							containerPtr->GetTimeRange().GetBeginTime().date().toString("dd.MM.yyyy") + "/" +
							archiveFileName;

				if (!istd::CSystem::FileCopy(archivePath, outPath)){
					m_parentPtr->SendErrorMessage(
								0, tr("Unable to copy history archive to repository. Event container skipped")
								.arg(archivePath));

					break;
				}			
			} while(false);

			istd::CSystem::RemoveDirectory(tempDir);
		}
		else{
			m_parentPtr->SendErrorMessage(0, tr("Cannot create temporary folder. Event container skipped"));
		}
	}

	QMetaObject::invokeMethod(m_parentPtr, "OnWriterFinished", Qt::QueuedConnection);
}


// public methods of embedded class Reader

CEventHistoryGroupController::CReadJobController::CReadJobController()
{
}


QByteArray CEventHistoryGroupController::CReadJobController::AddJob(
			EventFilterPtr filterPtr,
			QString groupDir,
			QString containerExtension,
			QString archiveExtension,
			iser::IVersionInfo* versionInfoPtr,
			imtfile::IFileCompression* compressorPtr)
{
	Job newJob;
	newJob.uuid = QUuid::createUuid().toByteArray();
	newJob.filterPtr = filterPtr;
	newJob.readerPtr.SetPtr(new CEventHistoryGroupReader(
				groupDir,
				containerExtension,
				archiveExtension,
				versionInfoPtr,
				compressorPtr));
	newJob.jobStatus = JS_WAITING;

	// MUTEX!!!
	m_jobList.push_back(newJob);

	return newJob.uuid;
}


IEventProvider::EventFilterPtr CEventHistoryGroupController::CReadJobController::GetFilter(const QByteArray & jobId)
{
	return EventFilterPtr();
}


bool CEventHistoryGroupController::CReadJobController::GetResult(const QByteArray& jobId, ilog::CMessageContainer& resultMessages) const
{
	for (const Job& jobItem : m_jobList){
		if (jobItem.uuid == jobId && jobItem.jobStatus == JS_FINISHED){
			if (jobItem.resultContainerListPtr.IsValid()){
				static istd::IChangeable::ChangeSet changeSet(IEventProvider::RS_OK);
				istd::CChangeGroup notifier(&resultMessages, &changeSet);

				for (int i = 0; i < jobItem.resultContainerListPtr->count(); i++){
					CEventHistoryGroupReader::EventContainerPtr resultContainerPtr = jobItem.resultContainerListPtr->at(i);

					ilog::IMessageContainer::Messages messages = resultContainerPtr->GetMessages();
					for (int j = messages.count() - 1; i >= 0; i--){
						if (jobItem.filterPtr.IsValid()){
							if (jobItem.filterPtr->GetTimeRange().Contains(messages[j]->GetInformationTimeStamp()) && jobItem.filterPtr->IsMessageAccepted(messages[j].GetPtr())){
								resultMessages.AddMessage(messages[j]);
							}
						}
						else{
							resultMessages.AddMessage(messages[j]);
						}
					}
				}
			}
		}
	}

	return false;
}


// reimplemented (QThread)

void CEventHistoryGroupController::CReadJobController::run()
{
}


} // namespace imtlog


