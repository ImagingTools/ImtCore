#include <imtlog/CMessagesController.h>


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

CMessagesController::CMessagesController(
			int id,
			quint32 maxContainerMessageCount,
			quint16 containerSavingPeriod,
			quint16 containerCachingTime,
			QString dir,
			QString containerExtension,
			QString archiveExtension,
			iser::IVersionInfo* versionInfoPtr,
			imtfile::IFileCompression* compressorPtr,
			ilog::IMessageConsumer* logPtr)
	:m_writer(
		*this,
		dir,
		containerExtension,
		archiveExtension,
		versionInfoPtr,
		compressorPtr),
	m_readJobController(
				dir,
				containerExtension,
				archiveExtension,
				versionInfoPtr,
				compressorPtr,
				logPtr),
	m_messagesReader(
				dir,
				containerExtension,
				archiveExtension,
				versionInfoPtr,
				compressorPtr,
				logPtr),
	m_supportedMessageId(id),
	m_maxContainerMessageCount(maxContainerMessageCount),
	m_containerSavingPeriod(containerSavingPeriod),
	m_containerCachingTime(containerCachingTime)
{
	if (thread() != qApp->thread()){
		moveToThread(qApp->thread());
		m_readJobController.moveToThread(qApp->thread());
		m_timer.moveToThread(qApp->thread());
	}

	m_archiveTimeRange = m_messagesReader.ReadTimeRange();

	connect(&m_readJobController, &CMessagesReadJobController::JobFinished, this, &CMessagesController::OnJobFinished);
	connect(&m_timer, &QTimer::timeout, this, &CMessagesController::OnTimer);
	connect(this, &CMessagesController::InitTimer, this, &CMessagesController::OnInitTimer, Qt::QueuedConnection);

	Q_EMIT InitTimer();

	m_state = CS_OK;
}


void CMessagesController::OnSystemShutdown()
{
	m_state = CS_SHUTDOWN;

	m_timer.stop();

	{
		QMutexLocker workingQueueLocker(&m_workingQueueMutex);
		QMutexLocker writingQueueLocker(&m_writingQueueMutex);

		while (!m_workingQueue.isEmpty()){
			QueueItem queueItem = m_workingQueue.takeFirst();
			if (!queueItem.containerPtr->GetMessagesCount()){
				m_writingQueue.append(queueItem.containerPtr);
			}
		}
	}

	m_writer.Start();
	m_readJobController.requestInterruption();

	m_writer.wait();
	m_readJobController.wait();
}


QByteArray CMessagesController::AddJob(
			const IEventFilter* filterPtr,
			const IMessageFilterParams* filterParamsPtr) const
{
	return m_readJobController.AddJob(filterPtr, filterParamsPtr);
}


bool CMessagesController::IsJobExists(const QByteArray& requestId) const
{
	return m_readJobController.IsJobExists(requestId);
}


bool CMessagesController::TakeJobResult(const QByteArray& requestId, CMessagesReader::EventContainerPtr& resultEvents) const
{
	return m_readJobController.TakeJobResult(requestId, resultEvents);
}


// reimplemented (imtlog::ITimeRangeProvider)

CTimeRange CMessagesController::GetTimeRange() const
{
	return m_archiveTimeRange;
}


// reimplemented (ilog::IMessageConsumer)

bool CMessagesController::IsMessageSupported(
			int /*messageCategory*/,
			int messageId,
			const istd::IInformationProvider* messagePtr) const
{
	return messageId == m_supportedMessageId || messagePtr->GetInformationId() == m_supportedMessageId;
}


void CMessagesController::AddMessage(const MessagePtr& messagePtr)
{
	if (m_state == CS_OK){
		if (messagePtr->GetInformationId() == m_supportedMessageId){
			QDateTime timestamp = messagePtr->GetInformationTimeStamp();

			QMutexLocker workingQueueLocker(&m_workingQueueMutex);

			CMessagesReader::EventContainerPtr containerPtr;

			// check last container in working queue
			if (!m_workingQueue.isEmpty()){
				CMessagesReader::EventContainerPtr lastContainerPtr = m_workingQueue.back().containerPtr;
				Q_ASSERT(lastContainerPtr->GetMessagesCount() > 0);
				if (lastContainerPtr->GetMessagesCount() < m_maxContainerMessageCount && lastContainerPtr->GetTimeRange().GetBeginTime().date() == timestamp.date()){
					containerPtr = lastContainerPtr;
				}
			}

			// prepare new container
			if (!containerPtr.IsValid()){
				containerPtr.SetPtr(new CEventContainer);

				QueueItem queueItem;
				queueItem.containerPtr = containerPtr;
				queueItem.timeout = m_containerSavingPeriod;
				m_workingQueue.append(queueItem);
			}

			containerPtr->AddMessage(messagePtr);

			istd::CChangeNotifier notifier(this);
			m_archiveTimeRange.Ensure(timestamp);
		}
	}
}


// private slots

void CMessagesController::OnInitTimer()
{
	m_timer.setInterval(1000);
	m_timer.start();
}


void CMessagesController::OnTimer()
{
	QMutexLocker workingQueueLocker(&m_workingQueueMutex);
	QMutexLocker writingQueueLocker(&m_writingQueueMutex);
	QMutexLocker cacheQueueLocker(&m_cacheMutex);

	// Save and cache management
	QList<QueueItem>::iterator it = m_workingQueue.begin();
	while (it != m_workingQueue.end()){
		if (--it->timeout == 0){
			m_writingQueue.append(it->containerPtr);
			it->timeout = m_containerSavingPeriod;
		}

		if (it->containerPtr->GetMessagesCount() >= m_maxContainerMessageCount){
			QueueItem item = *it;

			m_writingQueue.append(item.containerPtr);
			m_cache.append(item);
			item.timeout = m_containerCachingTime;

			it = m_workingQueue.erase(it);
			continue;
		}

		it++;
	}

	if (!m_writingQueue.isEmpty()){
		m_writer.Start();
	}

	// Cleanup cache
	{
		QList<QueueItem>::iterator cacheIter = m_cache.begin();

		while (cacheIter != m_cache.end()){
			cacheIter->timeout--;
			if (cacheIter->timeout == 0){
				cacheIter = m_cache.erase(cacheIter);
				continue;
			}

			cacheIter++;
		}
	}
}


void CMessagesController::OnJobFinished(const QByteArray& jobId)
{
	Q_EMIT ReadFinished(jobId);
}


// public methods of embedded class Writer

CMessagesController::Writer::Writer(
			CMessagesController& parent,
			QString dir,
			QString containerExtension,
			QString archiveExtension,
			iser::IVersionInfo* versionInfoPtr,
			imtfile::IFileCompression* compressorPtr)
	:m_parent(parent),
	m_dir(dir),
	m_containerExtension(containerExtension),
	m_archiveExtension(archiveExtension),
	m_versionInfoPtr(versionInfoPtr),
	m_compressorPtr(compressorPtr)
{
}


// private methods of embedded class Writer

// reimplemented (QThread)

void CMessagesController::Writer::run()
{
	if (m_dir.isEmpty()){
		QMutexLocker writingQueueLocker(&m_parent.m_writingQueueMutex);
		m_parent.m_writingQueue.clear();

		return;
	}

	while (!m_parent.m_writingQueue.isEmpty()){
		if (isInterruptionRequested()){
			break;
		}

		CMessagesReader::EventContainerPtr containerPtr;
		{
			QMutexLocker writingQueueLocker(&m_parent.m_writingQueueMutex);
			containerPtr = m_parent.m_writingQueue.takeFirst();
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
				QString containerFileName = containerFileBaseName + "." + m_containerExtension;
				QString containerPath = tempDir + "/" + containerFileName;
				ifile::CCompactXmlFileWriteArchive xmlArchive(containerPath, m_versionInfoPtr);

				if (!containerPtr->Serialize(xmlArchive)){
					m_parent.SendErrorMessage(
								0, tr("Unable to serialize message history container \"%1\". Event container skipped")
								.arg(containerPath));

					break;
				}

				xmlArchive.Flush();

				QString archiveFileName = containerFileBaseName + "." + m_archiveExtension;
				QString archivePath = tempDir + "/" + archiveFileName;

				if (m_compressorPtr != nullptr){
					if (!m_compressorPtr->CompressFile(containerPath, archivePath)){
						m_parent.SendErrorMessage(
									0, tr("Unable to compress message history container \"%1\". Event container skipped")
									.arg(archivePath));

						break;
					}
				}

				QString outPath = m_dir + "/" +
							containerPtr->GetTimeRange().GetBeginTime().date().toString("dd.MM.yyyy") + "/" +
							archiveFileName;

				if (!istd::CSystem::FileCopy(archivePath, outPath, true)){
					m_parent.SendErrorMessage(
								0, tr("Unable to copy history archive to repository. Event container skipped")
								.arg(archivePath));

					break;
				}			
			} while(false);

			istd::CSystem::RemoveDirectory(tempDir);
		}
		else{
			m_parent.SendErrorMessage(0, tr("Cannot create temporary folder. Event container skipped"));
		}
	}
}


} // namespace imtlog


