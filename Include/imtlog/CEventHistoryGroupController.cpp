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
				persistenceParams.compressorPtr),
	m_readJobController(
		persistenceParams.repositoryDir + "/" + m_persistenceParams.groupDir,
		persistenceParams.containerExtension,
		persistenceParams.archiveExtension,
		persistenceParams.versionInfoPtr,
		persistenceParams.compressorPtr)
{
	m_timer.setInterval(1000);
	connect(&m_timer, &QTimer::timeout, this, &CEventHistoryGroupController::OnTimer);
	m_timer.start();

	m_writer.SetParams(persistenceParams);

	m_state = CS_OK;

	connect(&m_readJobController, &CEventHistoryReadJobController::JobFinished, this, &CEventHistoryGroupController::OnJobFinished);
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


QByteArray CEventHistoryGroupController::RequestEvents(IEventProvider::EventFilterPtr filterPtr) const
{
	return m_readJobController.AddJob(filterPtr);
}


bool CEventHistoryGroupController::IsValidRequestId(const QByteArray& requestId) const
{
	return m_readJobController.IsValidJobId(requestId);
}


IEventProvider::EventFilterPtr CEventHistoryGroupController::GetFilter(const QByteArray& requestId) const
{
	return m_readJobController.GetFilter(requestId);
}


bool CEventHistoryGroupController::PopResult(const QByteArray& requestId, ilog::CMessageContainer& resultEvents) const
{
	return m_readJobController.PopResult(requestId, resultEvents);
}

// reimplemented (imtlog::ITimeRangeProvider)

CTimeRange CEventHistoryGroupController::GetTimeRange() const
{
	if (m_archiveTimeRange.IsNull()){
		m_archiveTimeRange = m_completeGroupReader.ReadGroupTimeRange();
	}

	return m_archiveTimeRange;
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
	Q_EMIT RequestFinished(jobId);
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


} // namespace imtlog


