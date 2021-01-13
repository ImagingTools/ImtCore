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
	m_syncReader(
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
}


void CEventHistoryGroupController::OnSystemShutdown()
{
	m_state = CS_SHUTDOWN;

	//for (ReaderPtr readerPtr : m_readers){
	//	readerPtr->requestInterruption();
	//}

	m_timer.stop();

	{
		QMutexLocker workingQueueLocker(&m_workingQueueMutex);
		QMutexLocker writingQueueLocker(&m_writingQueueMutex);

		while (!m_workingQueue.isEmpty()){
			CEventHistoryGroupReadAccessor::EventContainerPtr containerPtr = m_workingQueue.takeFirst();
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
		m_archiveTimeRange = m_syncReader.GetGroupTimeRange();
	}

	return m_archiveTimeRange;
}


// reimplemented (imtlog::IMessageFilter)

IEventProvider::EventContainerPtr CEventHistoryGroupController::GetEvents(IEventProvider::EventFilterPtr filterPtr) const
{

	JobPtr jobPtr(new Job);
	jobPtr->containerPtr.SetPtr(new Job::Container);
	jobPtr->filterPtr = filterPtr;
	jobPtr->readerPtr.SetPtr(new Reader(
				m_persistenceParams.repositoryDir + "/" + m_persistenceParams.groupDir,
				m_persistenceParams.containerExtension,
				m_persistenceParams.archiveExtension,
				m_persistenceParams.versionInfoPtr,
				m_persistenceParams.compressorPtr));
	jobPtr->uuid = QUuid::createUuid().toString();

	qDebug() << connect(jobPtr->readerPtr.GetPtr(), &Reader::finished, this, &CEventHistoryGroupController::OnJobFinished);

	{
		QMutexLocker locker(&m_jobMutex);
		m_jobs[jobPtr->readerPtr.GetPtr()] = jobPtr;
	}

	jobPtr->readerPtr->Start(filterPtr->GetTimeRange());	
	
	return jobPtr->containerPtr;

	//return IEventProvider::EventContainerPtr();

	//IEventProvider::EventContainerPtr retVal =
	//			IEventProvider::EventContainerPtr(new CEventContainer);

	//CTimeRange timeRange = filterPtr->GetTimeRange();
	//QDateTime begin = timeRange.GetBeginTime();
	//QDateTime end = timeRange.GetEndTime();
	//QDate beginDate = begin.date();
	//QDate endDate = end.date();
	//QStringList dateList;

	////if (m_cacheTimeRange == timeRange){
	////	//for (MessageHistoryContainerPtr containerPtr : m_cache){
	////	//	imtlog::CMessageHistoryContainer::Messages messages = containerPtr->GetEvents();
	////	//	for (int j = 0; j < messages.count(); j++){
	////	//		QDateTime timeStamp = messages[j]->messagePtr->GetInformationTimeStamp();
	////	//		if (timeRange.Contains(timeStamp)){
	////	//			if (filterPtr->IsMessageAccepted(messages[j]->messagePtr.GetPtr())){
	////	//				retVal.append(messages[j]);
	////	//			}
	////	//		}
	////	//	}
	////	//}

	////	return retVal;
	////}

	////m_cacheTimeRange = timeRange;
	//m_cache.clear();

	//QMap<QDate, QString> dirMap = GetHistoryDirMap();
	//QList<QDate> dirMapKeys = dirMap.keys();

	//if (dirMap.isEmpty()){
	//	return retVal;
	//}

	//QDate date = beginDate;
	//while (date <= endDate){
	//	if (dirMap.contains(date)){
	//		QMap<QDateTime, QString> fileMap = GetHistoryFileMap(dirMap[date]);
	//		QList<QDateTime> fileMapKeys = fileMap.keys();

	//		for (int i = 0; i < fileMapKeys.count(); i++){
	//			CEventHistoryGroupReadAccessor::EventContainerPtr container = ImportContainer(fileMap[fileMapKeys[i]]);
	//			if (timeRange.Intersect(container->GetTimeRange()).IsClosed()){
	//				m_cache.append(container);
	//				ilog::CMessageContainer::Messages messages = container->GetMessages();

	//				for (int j = 0; j < messages.count(); j++){
	//					QDateTime timeStamp = messages[j]->GetInformationTimeStamp();
	//					if (timeRange.Contains(timeStamp)){
	//						if (filterPtr->IsMessageAccepted(messages[j].GetPtr())){
	//							retVal->AddMessage(messages[j]);
	//						}
	//					}
	//				}
	//			}
	//		}
	//	}

	//	date = date.addDays(1);
	//}

	//return retVal;
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

		CEventHistoryGroupReadAccessor::EventContainerPtr containerPtr = GetContainerForMessage(timestamp);
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
	m_syncReader.SetLogPtr(logPtr);
}


// private slots

void CEventHistoryGroupController::OnTimer()
{
	QMutexLocker workingQueueLocker(&m_workingQueueMutex);
	QMutexLocker writingQueueLocker(&m_writingQueueMutex);

	for (CEventHistoryGroupReadAccessor::EventContainerPtr containerPtr : m_workingQueue){
		Q_ASSERT(containerPtr->GetMessagesCount() > 0);
		if (containerPtr->GetTimeRange().GetEndTime().addSecs(m_controllerParams.writeDelay) < QDateTime::currentDateTime()){

		}
	}

	CEventHistoryGroupReadAccessor::EventContainerList::iterator it = m_workingQueue.begin();
	while (it != m_workingQueue.end()){
		CEventHistoryGroupReadAccessor::EventContainerPtr containerPtr = *it;

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


void CEventHistoryGroupController::OnJobFinished()
{
	Reader* readerPtr = dynamic_cast<Reader*>(sender());
	Q_ASSERT(readerPtr != nullptr);

	JobPtr jobPtr;
	{
		QMutexLocker locker(&m_jobMutex);
		Q_ASSERT(m_jobs.contains(readerPtr));
		jobPtr = m_jobs.take(readerPtr);
	}

	Job::Container* containerPtr = dynamic_cast<Job::Container*>(jobPtr->containerPtr.GetPtr());

	CEventHistoryGroupReadAccessor::EventContainerListPtr resultContainerListPtr = jobPtr->readerPtr->GetResult();
	if (resultContainerListPtr.IsValid()){
		istd::IChangeable::ChangeSet set(IEventProvider::RS_OK);
		istd::CChangeGroup notifier(containerPtr, &set);

		for (int i = 0; i < resultContainerListPtr->count(); i++){
			CEventHistoryGroupReadAccessor::EventContainerPtr resultContainerPtr = resultContainerListPtr->at(i);

			ilog::IMessageContainer::Messages messages = resultContainerPtr->GetMessages();
			for (int j = messages.count() - 1; i >= 0; i--){
				if (jobPtr->filterPtr->GetTimeRange().Contains(messages[j]->GetInformationTimeStamp())){
					if (jobPtr->filterPtr->IsMessageAccepted(messages[j].GetPtr())){
						containerPtr->AddMessage(messages[j]);
					}
				}
			}
		}

		return;	
	}

	istd::IChangeable::ChangeSet set(IEventProvider::RS_CANCELED);
	istd::CChangeGroup notifier(containerPtr, &set);
	containerPtr->ClearMessages();
}


// private methods

CEventHistoryGroupReadAccessor::EventContainerPtr CEventHistoryGroupController::GetContainerForMessage(const QDateTime& timestamp)
{
	QMutexLocker workingQueueLocker(&m_workingQueueMutex);

	for (CEventHistoryGroupReadAccessor::EventContainerPtr containerPtr : m_workingQueue){
		if (containerPtr->GetTimeRange().Contains(timestamp)){
			return containerPtr;
		}
	}

	if (!m_workingQueue.isEmpty()){
		Q_ASSERT(timestamp > m_workingQueue.last()->GetTimeRange().GetEndTime());
	}

	// Prepare new container

	CEventHistoryGroupReadAccessor::EventContainerPtr containerPtr(new CEventContainer);

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

		CEventHistoryGroupReadAccessor::EventContainerPtr containerPtr;
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

CEventHistoryGroupController::Reader::Reader(
			QString groupDir,
			QString containerExtension,
			QString archiveExtension,
			iser::IVersionInfo* versionInfoPtr,
			imtfile::IFileCompression* compressorPtr)
	:CEventHistoryGroupReadAccessor(
				groupDir,
				containerExtension,
				archiveExtension,
				versionInfoPtr,
				compressorPtr)
{
}

// protected slots:

void CEventHistoryGroupController::Reader::start(Priority)
{
}


void CEventHistoryGroupController::Reader::terminate()
{
}


void CEventHistoryGroupController::Reader::quit()
{
}


// protected methods of embedded class Reader

bool CEventHistoryGroupController::Reader::IsInterruptionRequested() const
{
	return isInterruptionRequested();
}


bool CEventHistoryGroupController::Reader::Start(const CTimeRange& timeRange)
{
	if (!isRunning()){
		m_timeRange = timeRange;
		BaseClass::start();
		return true;
	}

	return false;
}


CEventHistoryGroupReadAccessor::EventContainerListPtr CEventHistoryGroupController::Reader::GetResult()
{
	if (!isRunning()){
		EventContainerListPtr();
	};

	return m_containerListPtr;
}


// reimplemented (QThread)

void CEventHistoryGroupController::Reader::run()
{
	m_containerListPtr.Reset();
	m_containerListPtr = GetContainers(m_timeRange);
}


} // namespace imtlog


