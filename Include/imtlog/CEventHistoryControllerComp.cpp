#include <imtlog/CEventHistoryControllerComp.h>


// Qt includes
#include <QtCore/QDebug>
#include <QtCore/QDir>
#include <QtCore/QUuid>

// ACF includes
#include <ifile/CCompactXmlFileWriteArchive.h>
#include <ifile/CCompactXmlFileReadArchive.h>
#include <iser/CPrimitiveTypesSerializer.h>
#include <istd/CSystem.h>


namespace imtlog
{


// public methods

CEventHistoryControllerComp::CEventHistoryControllerComp()
	:m_reader(this),
	m_writer(this),
	m_readerState(TS_IDLE),
	m_writerState(TS_IDLE),
	m_controllerState(CS_INIT)
{
	qRegisterMetaType<MessagePtr>("MessagePtr");
	qRegisterMetaType<EventContainerPtr>("EventContainerPtr");

	m_containerCheckTimer.callOnTimeout(this, &CEventHistoryControllerComp::OnContainerCheckTimer);
}


// reimplemented (ilog::IMessageConsumer)

bool CEventHistoryControllerComp::IsMessageSupported(
			int messageCategory,
			int messageId,
			const istd::IInformationProvider* messagePt) const
{
	return true;
}


void CEventHistoryControllerComp::AddMessage(const MessagePtr& messagePtr)
{
	if (m_controllerState == CS_OK){
		EventContainerPtr containerPtr = GetContainerForMessage(messagePtr);
		if (containerPtr.isNull()){
			SendWarningMessage(0, QString("Event container for message at \"%1\" not found").arg(messagePtr->GetInformationTimeStamp().toString()));
		}
		else{
			containerPtr->AddMessage(messagePtr);
		}
	}

	if (m_slaveMessageConsumerCompPtr.IsValid()){
		m_slaveMessageConsumerCompPtr->AddMessage(messagePtr);
	}
}


// reimplemented (ibase::TRuntimeStatusHanderCompWrap)
void CEventHistoryControllerComp::OnSystemShutdown()
{
	m_controllerState = CS_SHUTDOWN;

	m_containerCheckTimer.stop();

	m_reader.requestInterruption();
	m_reader.wait();

	{
		QMutexLocker workingQueueLocker(&m_workingQueueMutex);
		QMutexLocker writingQueueLocker(&m_writingQueueMutex);

		while (!m_workingQueue.isEmpty()){
			EventContainerPtr containerPtr = m_workingQueue.dequeue();
			if (containerPtr->GetMessagesCount() > 0){
				if (containerPtr->GetEndTime() > QDateTime::currentDateTime()){
					containerPtr->SetEndTime(QDateTime::currentDateTime());
				}
				m_writingQueue.enqueue(containerPtr);
			}
		}
	}

	StartWriter();
	m_writer.wait();
}


void CEventHistoryControllerComp::OnComponentCreated()
{
	BaseClass::OnComponentCreated();

	if (!m_logFolderCompPtr.IsValid() || (m_logFolderCompPtr->GetPathType() != ifile::IFileNameParam::PT_DIRECTORY)){
		m_controllerState = CS_FAILED;
		SendWarningMessage(0, tr("Log folder not specified. Event history disabled"));
		return;
	}

	if (!istd::CSystem::EnsurePathExists(m_logFolderCompPtr->GetPath())){
		m_controllerState = CS_FAILED;
		SendErrorMessage(0, tr("Unable access log folder. Event history disabled"));
		return;
	}

	PrepareWorkingContainers();

	m_controllerState = CS_OK;

	m_systemStartTime = QDateTime::currentDateTime();
	m_containerCheckTimer.start(1000);

	StartReader();
}


// private slots

void CEventHistoryControllerComp::OnContainerCheckTimer()
{
	QMutexLocker workingQueueLocker(&m_workingQueueMutex);
	QMutexLocker writingQueueLocker(&m_writingQueueMutex);

	while (!m_workingQueue.isEmpty()){
		if (m_workingQueue.head()->GetEndTime().addSecs(*m_containerWriteDelayAttrPtr) < QDateTime::currentDateTime()){
			EventContainerPtr containerPtr = m_workingQueue.dequeue();
			if (containerPtr->GetMessagesCount() > 0){
				m_writingQueue.enqueue(containerPtr);
				StartWriter();
			}

			continue;
		}

		break;
	}

	workingQueueLocker.unlock();
	writingQueueLocker.unlock();

	PrepareWorkingContainers();
}


// private methods


CEventHistoryControllerComp::EventContainerPtr CEventHistoryControllerComp::GetContainerForMessage(const MessagePtr& messagePtr)
{
	QDateTime time = messagePtr->GetInformationTimeStamp();

	QMutexLocker workingQueueLocker(&m_workingQueueMutex);

	for (EventContainerPtr containerPtr : m_workingQueue){
		if (time >= containerPtr->GetBeginTime() && time <= containerPtr->GetEndTime()){
			return containerPtr;
		}
	}

	return EventContainerPtr(nullptr);
}


CEventHistoryControllerComp::TimeRange CEventHistoryControllerComp::CalculateContainerTimeRange(const QDateTime& lastContainerEndTime)
{
	QDateTime beginTime = QDateTime::fromMSecsSinceEpoch(lastContainerEndTime.toMSecsSinceEpoch() + 1);
	QDateTime endTime = QDateTime::fromMSecsSinceEpoch(
				beginTime.toMSecsSinceEpoch() +
				*m_containerTimeDurationAttrPtr * 1000 - 1);

	if (endTime.date() != beginTime.date()){
		endTime = QDateTime(beginTime.date(), QTime(23, 59, 59, 999));
	}

	TimeRange timeRange;
	timeRange.beginTime = beginTime;
	timeRange.endTime = endTime;
	
	return timeRange;
}


void CEventHistoryControllerComp::PrepareWorkingContainers()
{
	QMutexLocker workingQueueLocker(&m_workingQueueMutex);
	
	TimeRange timeRange;

	int workQueueSize = *m_containerWriteDelayAttrPtr / *m_containerTimeDurationAttrPtr + 10;

	while (m_workingQueue.count() < workQueueSize){
		if (m_workingQueue.isEmpty()){
			timeRange = CalculateContainerTimeRange(QDateTime::currentDateTime());
		}
		else{
			timeRange = CalculateContainerTimeRange(m_workingQueue.last()->GetEndTime());
		}

		EventContainerPtr containerPtr(new EventContainer);
		containerPtr->SetBeginTime(timeRange.beginTime);
		containerPtr->SetEndTime(timeRange.endTime);
		m_workingQueue.enqueue(containerPtr);
	}
}


bool CEventHistoryControllerComp::SerializeContainer(EventContainerPtr containerPtr, iser::IArchive& archive)
{
	int messageCount = containerPtr->GetMessagesCount();

	QDateTime beginTime;
	QDateTime endTime;

	if (archive.IsStoring()){
		beginTime = containerPtr->GetBeginTime();
		endTime = containerPtr->GetEndTime();
	}
	else{
		messageCount = 0;

		containerPtr->ClearMessages();
	}

	static iser::CArchiveTag beginTimeTag("BeginTime", "Container begin time", iser::CArchiveTag::TT_LEAF);
	static iser::CArchiveTag endTimeTag("EndTime", "Container end time", iser::CArchiveTag::TT_LEAF);

	bool retVal = true;

	retVal = retVal && archive.BeginTag(beginTimeTag);
	retVal = retVal && iser::CPrimitiveTypesSerializer::SerializeDateTime(archive, beginTime);
	retVal = retVal && archive.EndTag(beginTimeTag);

	retVal = retVal && archive.BeginTag(endTimeTag);
	retVal = retVal && iser::CPrimitiveTypesSerializer::SerializeDateTime(archive, endTime);
	retVal = retVal && archive.EndTag(endTimeTag);

	if (!archive.IsStoring()){
		containerPtr->SetBeginTime(beginTime);
		containerPtr->SetEndTime(endTime);
	}

	retVal = retVal && containerPtr->Serialize(archive);

	return retVal;
}


void CEventHistoryControllerComp::StartReader()
{
	m_reader.start();
}


void CEventHistoryControllerComp::OnReaderFinished()
{
	m_readerState = TS_IDLE;
}


void CEventHistoryControllerComp::OnContainerReadComplete(EventContainerPtr containerPtr)
{
	if (m_slaveMessageConsumerCompPtr.IsValid()){
		if (containerPtr->GetEndTime() < m_systemStartTime){
			for (ilog::IMessageConsumer::MessagePtr messagePtr : containerPtr->GetMessages()){
				m_slaveMessageConsumerCompPtr->AddMessage(messagePtr);
			}
		}
	}
}


void CEventHistoryControllerComp::StartWriter()
{
	if (m_writerState == TS_IDLE){
		m_writerState = TS_RUNNING;
		m_writer.start();
	}
	else{
		m_writerState = TS_PENDING;
	}
}


void CEventHistoryControllerComp::OnWriterFinished()
{
	if (m_writerState == TS_PENDING){
		m_writerState = TS_IDLE;
		StartWriter();
	}
	else{
		m_writerState = TS_IDLE;
	}
}


// public methods of embedded class EventContainer

const QDateTime& CEventHistoryControllerComp::EventContainer::GetBeginTime()
{
	QMutexLocker locker(&m_mutex);
	return m_beginTime;
}


void CEventHistoryControllerComp::EventContainer::SetBeginTime(const QDateTime& time)
{
	QMutexLocker locker(&m_mutex);
	m_beginTime = time;
}


const QDateTime& CEventHistoryControllerComp::EventContainer::GetEndTime()
{
	QMutexLocker locker(&m_mutex);
	return m_endTime;
}


void CEventHistoryControllerComp::EventContainer::SetEndTime(const QDateTime& time)
{
	QMutexLocker locker(&m_mutex);
	m_endTime = time;
}


int CEventHistoryControllerComp::EventContainer::GetMessagesCount()
{
	QMutexLocker locker(&m_mutex);
	return m_messageContainer.GetMessagesCount();
}


// reimplemented (ilog::IMessageConsumer)

bool CEventHistoryControllerComp::EventContainer::IsMessageSupported(
			int messageCategory,
			int messageId,
			const istd::IInformationProvider* messagePtr) const
{
	QMutexLocker locker(&m_mutex);
	return m_messageContainer.IsMessageSupported(messageCategory, messageId, messagePtr);
}


void CEventHistoryControllerComp::EventContainer::AddMessage(const MessagePtr& messagePtr)
{
	QMutexLocker locker(&m_mutex);
	m_messageContainer.AddMessage(messagePtr);
}


// reimplemented (ilog::IMessageContainer)

int CEventHistoryControllerComp::EventContainer::GetWorstCategory() const
{
	QMutexLocker locker(&m_mutex);
	return m_messageContainer.GetWorstCategory();
}


ilog::IMessageContainer::Messages CEventHistoryControllerComp::EventContainer::GetMessages() const
{
	QMutexLocker locker(&m_mutex);
	return m_messageContainer.GetMessages();
}


void CEventHistoryControllerComp::EventContainer::ClearMessages()
{
	QMutexLocker locker(&m_mutex);
	m_messageContainer.ClearMessages();
}


// reimplemented (iser::ISerializable)

bool CEventHistoryControllerComp::EventContainer::Serialize(iser::IArchive& archive)
{
	QMutexLocker locker(&m_mutex);
	return m_messageContainer.Serialize(archive);
}


// public methods of embedded class Reader

CEventHistoryControllerComp::Reader::Reader(CEventHistoryControllerComp* parentPtr)
	:m_parentPtr(parentPtr)
{
}

// private methods of embedded class Reader

void CEventHistoryControllerComp::Reader::ImportFromDirs(const QStringList& archiveDirList)
{
	for (QString archiveDir : archiveDirList){
		if (isInterruptionRequested()){
			break;
		}

		QDir currentDir(m_parentPtr->m_logFolderCompPtr->GetPath() + "/" + archiveDir);
		QStringList containerFileList = currentDir.entryList(
			QDir::Files | QDir::NoSymLinks,
			QDir::Time | QDir::Reversed);

		for (QString containerFileItem : containerFileList){
			if (isInterruptionRequested()){
				break;
			}

			EventContainerPtr containerPtr(new EventContainer);

			ifile::CCompactXmlFileReadArchive xmlArchive(currentDir.path() + "/" + containerFileItem, m_parentPtr->m_versionInfoCompPtr.GetPtr());
			if (m_parentPtr->SerializeContainer(containerPtr, xmlArchive)){
				QMetaObject::invokeMethod(
					m_parentPtr, "OnContainerReadComplete", Qt::QueuedConnection,
					Q_ARG(EventContainerPtr, containerPtr));
			}
			else{
				m_parentPtr->SendErrorMessage(
					0, tr("Unable to deserialize event container \"%1\". Event container skipped")
					.arg(containerFileItem));
			}
		}
	}
}


void CEventHistoryControllerComp::Reader::ImportFromFiles(const QStringList& fileList)
{
	for (QString fileItem : fileList){
		if (isInterruptionRequested()){
			break;
		}

		QDir tempDir = QDir::tempPath();
		QString uuid = QUuid::createUuid().toString();
		if (tempDir.mkpath("ImtCore/" + uuid)){
			tempDir.cd("ImtCore/" + uuid);

			if (m_parentPtr->m_compressorCompPtr->DecompressFolder(
						m_parentPtr->m_logFolderCompPtr->GetPath() + "/" + fileItem,
						tempDir.path())){
				QStringList containerFileList = tempDir.entryList(
							QDir::Files | QDir::NoSymLinks,
							QDir::Time | QDir::Reversed);

				for (QString containerFileItem : containerFileList){
					if (isInterruptionRequested()){
						break;
					}

					EventContainerPtr containerPtr(new EventContainer);

					ifile::CCompactXmlFileReadArchive xmlArchive(tempDir.path() + "/" + containerFileItem, m_parentPtr->m_versionInfoCompPtr.GetPtr());
					if (m_parentPtr->SerializeContainer(containerPtr, xmlArchive)){
						QMetaObject::invokeMethod(
									m_parentPtr, "OnContainerReadComplete", Qt::QueuedConnection,
									Q_ARG(EventContainerPtr, containerPtr));
					}
					else{
						m_parentPtr->SendErrorMessage(
									0, tr("Unable to deserialize event container \"%1\". Event container skipped")
									.arg(containerFileItem));
					}
				}
			}
			else{
				m_parentPtr->SendErrorMessage(
							0, tr("Unable to decompress event archive \"%1\". Event archive skipped")
							.arg(fileItem));
			}

			tempDir.removeRecursively();
		}
		else{
			m_parentPtr->SendErrorMessage(0, tr("Unable to create temporary directory"));
		}
	}
}


// reimplemented (QThread)

void CEventHistoryControllerComp::Reader::run()
{

	QDir repositoryDir(m_parentPtr->m_logFolderCompPtr->GetPath());

	if (m_parentPtr->m_compressorCompPtr.IsValid()){
		QStringList fileList = repositoryDir.entryList(
					QDir::Files | QDir::NoSymLinks,
					QDir::Time | QDir::Reversed);

		ImportFromFiles(fileList);
	}
	else{
		QStringList dirList = repositoryDir.entryList(
					QDir::AllDirs | QDir::NoSymLinks | QDir::NoDotAndDotDot,
					QDir::Time | QDir::Reversed);

		ImportFromDirs(dirList);
	}

	QMetaObject::invokeMethod(m_parentPtr, "OnReaderFinished", Qt::QueuedConnection);
}


// public methods of embedded class Writer

CEventHistoryControllerComp::Writer::Writer(CEventHistoryControllerComp* parentPtr)
	:m_parentPtr(parentPtr)
{
}

// private methods of embedded class Writer

// reimplemented (QThread)

void CEventHistoryControllerComp::Writer::run()
{
	while (true){
		QMutexLocker writingQueueLocker(&m_parentPtr->m_writingQueueMutex);

		if (m_parentPtr->m_writingQueue.isEmpty()){
			break;
		}

		EventContainerPtr containerPtr = m_parentPtr->m_writingQueue.first();
		writingQueueLocker.unlock();

		if (containerPtr.isNull()){
			m_parentPtr->SendErrorMessage(0, tr("Skipped event container with nullptr in the queue"));
			writingQueueLocker.relock();
			m_parentPtr->m_writingQueue.removeFirst();
			continue;
		}

		if (containerPtr->GetMessagesCount() == 0){
			m_parentPtr->SendErrorMessage(0, tr("Skipped empty event container")); 
			writingQueueLocker.relock();
			m_parentPtr->m_writingQueue.removeFirst();
			continue;
		}

		QDir tempDir = QDir::tempPath();
		QString uuid = QUuid::createUuid().toString();
		if (tempDir.mkpath("ImtCore/" + uuid)){
			tempDir.cd("ImtCore/" + uuid);

			QString containerFileName =
						containerPtr->GetBeginTime().toString(*m_parentPtr->m_containerNameFormatAttrPtr) +
						"." + *m_parentPtr->m_containerExtensionAttrPtr;
			QString containerPath = tempDir.path() + "/" + containerFileName;

			QDir repositoryDir(m_parentPtr->m_logFolderCompPtr->GetPath());
			QString archiveDir = containerPtr->GetBeginTime().toString(*m_parentPtr->m_archiveNameFormatAttrPtr);
			QString archiveFileName = archiveDir + "." + *m_parentPtr->m_archiveExtensionAttrPtr;

			if (m_parentPtr->m_compressorCompPtr.IsValid()){
				if (repositoryDir.exists(archiveFileName)){
					QString archiveFilePath = repositoryDir.path() + "/" + archiveFileName;
					if (!m_parentPtr->m_compressorCompPtr->DecompressFolder(archiveFilePath, tempDir.path())){
						m_parentPtr->SendErrorMessage(
							0, tr("Unable to decompress event archive \"%1\". Day archive lost")
							.arg(archiveFileName));
					}
				}
			}

			ifile::CCompactXmlFileWriteArchive xmlArchive(containerPath, m_parentPtr->m_versionInfoCompPtr.GetPtr());
			if (!m_parentPtr->SerializeContainer(containerPtr, xmlArchive)){
				m_parentPtr->SendErrorMessage(
							0, tr("Unable to serialize event container with begin time \"%1\". Event container skipped")
							.arg(containerPtr->GetBeginTime().toString("dd.MM.yyyy hh-mm-ss.zzz")));

				xmlArchive.Flush();
				tempDir.removeRecursively();
				continue;
			}
			xmlArchive.Flush();

			if (m_parentPtr->m_compressorCompPtr.IsValid()){
				QString archiveFilePath = repositoryDir.path() + "/" + archiveFileName;
				if (!m_parentPtr->m_compressorCompPtr->CompressFolder(tempDir.path(), archiveFilePath, true)){
					m_parentPtr->SendErrorMessage(
								0, tr("Unable to compress event archive \"%1\". Event container skipped. Day archive possibly lost")
								.arg(archiveFileName));

					tempDir.removeRecursively();
					continue;
				}
			}
			else{
				QString archiveFilePath = repositoryDir.path() + "/" + archiveDir + "/" + containerFileName;
				if (!istd::CSystem::FileCopy(containerPath, archiveFilePath)){
					m_parentPtr->SendErrorMessage(0, tr("Unable to copy event archive to log folder"));

					tempDir.removeRecursively();
					continue;
				}
			}

			tempDir.removeRecursively();
		}
		else{
			m_parentPtr->SendErrorMessage(0, tr("Cannot create temporary folder. Event container skipped"));			
		}

		writingQueueLocker.relock();
		m_parentPtr->m_writingQueue.removeFirst();
	}

	QMetaObject::invokeMethod(m_parentPtr, "OnWriterFinished", Qt::QueuedConnection);
}

} // namespace imtlog


