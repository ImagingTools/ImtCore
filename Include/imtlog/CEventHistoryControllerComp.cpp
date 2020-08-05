#include <imtlog/CEventHistoryControllerComp.h>


// Qt includes
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
	m_idDayTimer(0),
	m_idIntervalTimer(0),
	m_readerState(TS_IDLE),
	m_writerState(TS_IDLE),
	m_isHistoryActive(true)
{
	qRegisterMetaType<MessagePtr>("MessagePtr");
	qRegisterMetaType<IntervalContainerPtr>("IntervalContainerPtr");
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
	if (m_isHistoryActive){
		QMetaObject::invokeMethod(this, "AddMessageSync",  Q_ARG(MessagePtr, messagePtr));
	}

	if (m_slaveMessageConsumerCompPtr.IsValid()){
		m_slaveMessageConsumerCompPtr->AddMessage(messagePtr);
	}
}


void CEventHistoryControllerComp::OnComponentCreated()
{
	BaseClass::OnComponentCreated();

	Q_ASSERT(m_repositoryPathCompPtr.IsValid());
	Q_ASSERT(m_repositoryPathCompPtr->GetPathType() == ifile::IFileNameParam::PT_DIRECTORY);

	if (!istd::CSystem::EnsurePathExists(m_repositoryPathCompPtr->GetPath())){
		m_isHistoryActive = false;
		SendErrorMessage(0, tr("Unable access event history repository folder. History disabled"));
		return;
	}

	m_containerPtr = IntervalContainerPtr(new IntervalContainer);
	m_containerPtr->beginTime = QDateTime::currentDateTime();

	StartDayTimer();
	StartIntervalTimer();

	m_startedAt = QDateTime::currentDateTime();
	StartReader();
}


void CEventHistoryControllerComp::OnComponentDestroyed()
{
	if (m_idDayTimer){
		killTimer(m_idDayTimer);
	}

	if (m_idIntervalTimer){
		killTimer(m_idIntervalTimer);
	}

	BaseClass::OnComponentDestroyed();
}


// protected methods

// reimplemented (QObject)

void CEventHistoryControllerComp::timerEvent(QTimerEvent *event)
{
	if (event->timerId() == m_idDayTimer){
		if (!IsCurrentDay(QDateTime::currentDateTime())){
			CloseInterval();
		}

		killTimer(m_idDayTimer);
		StartDayTimer();
	}

	if (event->timerId() == m_idIntervalTimer)
	{
		if(!IsCurrentInterval(QDateTime::currentDateTime())){
			CloseInterval();
		}
	}
}


// private methods

void CEventHistoryControllerComp::StartDayTimer()
{
	QDateTime currentTime = QDateTime::currentDateTime();
	QDateTime nextDay = QDateTime(currentTime.date().addDays(1), QTime(0, 0, 5, 0));

	qint64 dayTimeout = currentTime.msecsTo(nextDay);
	m_idDayTimer = startTimer(dayTimeout);
}


void CEventHistoryControllerComp::StartIntervalTimer()
{
	m_idIntervalTimer = startTimer(*m_flushingIntervalAttrPtr);
}


bool CEventHistoryControllerComp::IsCurrentInterval(const QDateTime& timestamp)
{
	if (m_containerPtr->beginTime.addSecs(*m_flushingIntervalAttrPtr) > timestamp){
		return true;
	}

	return false;
}


bool CEventHistoryControllerComp::IsCurrentDay(const QDateTime& timestamp)
{
	if (m_containerPtr->beginTime.date() == timestamp.date()){
		return true;
	}

	return false;
}


void CEventHistoryControllerComp::CloseInterval()
{
	if (m_containerPtr->messageContainer.GetMessagesCount() != 0){
		m_containerPtr->endTime = QDateTime::currentDateTime();
		m_containerQueue.append(m_containerPtr);

		m_containerPtr = IntervalContainerPtr(new IntervalContainer);
		m_containerPtr->beginTime = QDateTime::currentDateTime();
		StartWriter();

		return;
	}

	m_containerPtr->beginTime = QDateTime::currentDateTime();
}


bool CEventHistoryControllerComp::SerializeContainer(IntervalContainerPtr intervalContainerPtr, iser::IArchive& archive)
{
	int messageCount = intervalContainerPtr->messageContainer.GetMessagesCount();

	if (!archive.IsStoring()){
		messageCount = 0;

		intervalContainerPtr->messageContainer.ClearMessages();
		intervalContainerPtr->beginTime = QDateTime();
	}

	static iser::CArchiveTag beginTimeTag("BeginTime", "Interval begin time", iser::CArchiveTag::TT_LEAF);
	static iser::CArchiveTag endTimeTag("EndTime", "Interval end time", iser::CArchiveTag::TT_LEAF);

	bool retVal = true;

	retVal = retVal && archive.BeginTag(beginTimeTag);
	retVal = retVal && iser::CPrimitiveTypesSerializer::SerializeDateTime(archive, intervalContainerPtr->beginTime);
	retVal = retVal && archive.EndTag(beginTimeTag);

	retVal = retVal && archive.BeginTag(endTimeTag);
	retVal = retVal && iser::CPrimitiveTypesSerializer::SerializeDateTime(archive, intervalContainerPtr->endTime);
	retVal = retVal && archive.EndTag(endTimeTag);

	retVal = retVal && intervalContainerPtr->messageContainer.Serialize(archive);

	return retVal;
}


void CEventHistoryControllerComp::AddMessageSync(MessagePtr messagePtr)
{
	if ((!IsCurrentDay(messagePtr->GetInformationTimeStamp())) || (!IsCurrentInterval(messagePtr->GetInformationTimeStamp()))){
		CloseInterval();
	}

	m_containerPtr->messageContainer.AddMessage(messagePtr);
}


void CEventHistoryControllerComp::DisableHistory()
{
	if (m_idDayTimer){
		killTimer(m_idDayTimer);
		m_idDayTimer = 0;
	}

	if (m_idIntervalTimer){
		killTimer(m_idIntervalTimer);
		m_idIntervalTimer = 0;
	}

	m_isHistoryActive = false;
	m_containerPtr->messageContainer.ClearMessages();
	m_containerQueue.clear();
}


void CEventHistoryControllerComp::StartReader()
{
	m_reader.start();
}


void CEventHistoryControllerComp::OnReaderFinished()
{
}


void CEventHistoryControllerComp::OnReaderInterrupted()
{
}


void CEventHistoryControllerComp::OnContainerReadComplete(IntervalContainerPtr intervalContainerPtr)
{
	if (m_slaveMessageConsumerCompPtr.IsValid()){
		if (intervalContainerPtr->endTime < m_startedAt){
			for (ilog::IMessageConsumer::MessagePtr messagePtr : intervalContainerPtr->messageContainer.GetMessages()){
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


void CEventHistoryControllerComp::OnWriterInterrupted()
{
}


// public methods of embedded class Reader

CEventHistoryControllerComp::Reader::Reader(CEventHistoryControllerComp* parentPtr)
	:m_parentPtr(parentPtr),
	m_isInterrupted(false)
{
}

// private methods of embedded class Reader

void CEventHistoryControllerComp::Reader::DisableHistory()
{
	m_isInterrupted = true;
	QMetaObject::invokeMethod(m_parentPtr, "DisableHistory");
}


void CEventHistoryControllerComp::Reader::ImportFromDirs(const QStringList& archiveDirList)
{
	for (QString archiveDir : archiveDirList){
		if (isInterruptionRequested() || m_isInterrupted){
			break;
		}

		QDir currentDir(m_parentPtr->m_repositoryPathCompPtr->GetPath() + "/" + archiveDir);
		QStringList containerFileList = currentDir.entryList(
			QDir::Files | QDir::NoSymLinks,
			QDir::Time | QDir::Reversed);

		for (QString containerFileItem : containerFileList){
			if (isInterruptionRequested() || m_isInterrupted){
				break;
			}

			IntervalContainerPtr containerPtr(new IntervalContainer);

			ifile::CCompactXmlFileReadArchive xmlArchive(currentDir.path() + "/" + containerFileItem);
			if (m_parentPtr->SerializeContainer(containerPtr, xmlArchive)){
				QMetaObject::invokeMethod(
					m_parentPtr, "OnContainerReadComplete", Qt::QueuedConnection,
					Q_ARG(IntervalContainerPtr, containerPtr));
			}
			else{
				m_parentPtr->SendErrorMessage(
					0, tr("Unable to deserialize event container file [%1]. Event container skipped")
					.arg(containerFileItem));
			}
		}
	}
}


void CEventHistoryControllerComp::Reader::ImportFromFiles(const QStringList& fileList)
{
	for (QString fileItem : fileList){
		if (isInterruptionRequested() || m_isInterrupted){
			break;
		}

		QDir tempDir = QDir::tempPath();
		QString uuid = QUuid::createUuid().toString();
		if (tempDir.mkpath("ImtCore/" + uuid)){
			tempDir.cd("ImtCore/" + uuid);

			if (m_parentPtr->m_compressorCompPtr->DecompressFolder(
						m_parentPtr->m_repositoryPathCompPtr->GetPath() + "/" + fileItem,
						tempDir.path())){
				QStringList containerFileList = tempDir.entryList(
							QDir::Files | QDir::NoSymLinks,
							QDir::Time | QDir::Reversed);

				for (QString containerFileItem : containerFileList){
					if (isInterruptionRequested() || m_isInterrupted){
						break;
					}

					IntervalContainerPtr containerPtr(new IntervalContainer);

					ifile::CCompactXmlFileReadArchive xmlArchive(tempDir.path() + "/" + containerFileItem);
					if (m_parentPtr->SerializeContainer(containerPtr, xmlArchive)){
						QMetaObject::invokeMethod(
									m_parentPtr, "OnContainerReadComplete", Qt::QueuedConnection,
									Q_ARG(IntervalContainerPtr, containerPtr));
					}
					else{
						m_parentPtr->SendErrorMessage(
									0, tr("Unable to deserialize event container file [%1]. Event container skipped")
									.arg(containerFileItem));
					}
				}
			}
			else{
				m_parentPtr->SendErrorMessage(
							0, tr("Unable to decompress file [%1]. Event archive skipped")
							.arg(fileItem));
			}

			tempDir.removeRecursively();
		}
		else{
			DisableHistory();
			m_parentPtr->SendErrorMessage(0, tr("Unable to create temporary directory. History disabled"));
		}
	}
}


// reimplemented (QThread)

void CEventHistoryControllerComp::Reader::run()
{

	QDir repositoryDir(m_parentPtr->m_repositoryPathCompPtr->GetPath());

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

	if (isInterruptionRequested() || m_isInterrupted){
		QMetaObject::invokeMethod(m_parentPtr, "OnReaderInterrupted", Qt::QueuedConnection);
	}
	else{
		QMetaObject::invokeMethod(m_parentPtr, "OnReaderFinished", Qt::QueuedConnection);
	}
}


// public methods of embedded class Writer

CEventHistoryControllerComp::Writer::Writer(CEventHistoryControllerComp* parentPtr)
	:m_parentPtr(parentPtr),
	m_isInterrupted(false)
{
}

// private methods of embedded class Writer

void CEventHistoryControllerComp::Writer::DisableHistory()
{
	m_isInterrupted = true;
	QMetaObject::invokeMethod(m_parentPtr, "DisableHistory");
}


// reimplemented (QThread)

void CEventHistoryControllerComp::Writer::run()
{
	while (!isInterruptionRequested() && !m_isInterrupted){
		QMutexLocker locker(&m_parentPtr->m_queueMutex);

		if (m_parentPtr->m_containerQueue.isEmpty()){
			break;
		}

		IntervalContainerPtr containerPtr = m_parentPtr->m_containerQueue.first();
		locker.unlock();

		if (containerPtr.isNull()){
			m_parentPtr->SendErrorMessage(0, tr("Skipped event container with nullptr in the queue"));
			locker.relock();
			m_parentPtr->m_containerQueue.removeFirst();
			continue;
		}

		if (containerPtr->messageContainer.GetMessagesCount() == 0){
			m_parentPtr->SendErrorMessage(0, tr("Skipped empty event container")); 
			locker.relock();
			m_parentPtr->m_containerQueue.removeFirst();
			continue;
		}

		QDir tempDir = QDir::tempPath();
		QString uuid = QUuid::createUuid().toString();
		if (tempDir.mkpath("ImtCore/" + uuid)){
			tempDir.cd("ImtCore/" + uuid);

			QString containerFileName =
						containerPtr->beginTime.toString(*m_parentPtr->m_containerNameFormatAttrPtr) +
						"." + *m_parentPtr->m_containerExtensionAttrPtr;
			QString containerPath = tempDir.path() + "/" + containerFileName;

			QDir repositoryDir(m_parentPtr->m_repositoryPathCompPtr->GetPath());
			QString archiveDir = containerPtr->beginTime.toString(*m_parentPtr->m_archiveNameFormatAttrPtr);
			QString archiveFileName = archiveDir + "." + *m_parentPtr->m_archiveExtensionAttrPtr;

			if (m_parentPtr->m_compressorCompPtr.IsValid()){
				if (repositoryDir.exists(archiveFileName)){
					QString archiveFilePath = repositoryDir.path() + "/" + archiveFileName;
					m_parentPtr->m_compressorCompPtr->DecompressFolder(archiveFilePath, tempDir.path());
				}
			}

			ifile::CCompactXmlFileWriteArchive xmlArchive(containerPath);
			if (!m_parentPtr->SerializeContainer(containerPtr, xmlArchive)){
				m_parentPtr->SendErrorMessage(
							0, tr("Unable to serialize event event container at [%1]. Event container skipped")
							.arg(containerPtr->beginTime.toString("dd.MM.yyyy hh-mm-ss.zzz")));
				xmlArchive.Flush();
				tempDir.removeRecursively();
				continue;
			}
			xmlArchive.Flush();

			if (m_parentPtr->m_compressorCompPtr.IsValid()){
				QString archiveFilePath = repositoryDir.path() + "/" + archiveFileName;
				m_parentPtr->m_compressorCompPtr->CompressFolder(tempDir.path(), archiveFilePath, true);
			}
			else{
				QString archiveFilePath = repositoryDir.path() + "/" + archiveDir + "/" + containerFileName;
				if (!istd::CSystem::FileCopy(containerPath, archiveFilePath)){
					DisableHistory();
					m_parentPtr->SendErrorMessage(0, tr("Unable to store event history to repository. History disabled"));
				}
			}

			tempDir.removeRecursively();
		}
		else{
			DisableHistory();
			m_parentPtr->SendErrorMessage(0, tr("Unable to create temporary directory. History disabled"));			
		}

		locker.relock();
		m_parentPtr->m_containerQueue.removeFirst();
	}

	if (isInterruptionRequested() || m_isInterrupted){
		QMetaObject::invokeMethod(m_parentPtr, "OnWriterInterrupted", Qt::QueuedConnection);
	}
	else{
		QMetaObject::invokeMethod(m_parentPtr, "OnWriterFinished", Qt::QueuedConnection);
	}
}

} // namespace imtlog


