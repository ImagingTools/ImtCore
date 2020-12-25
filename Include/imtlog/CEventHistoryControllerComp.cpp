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
	://m_reader(this),
	m_writer(this),
	m_readerState(TS_IDLE),
	m_writerState(TS_IDLE),
	m_controllerState(CS_INIT),
	m_messageId(0)
{
	qRegisterMetaType<MessagePtr>("MessagePtr");
	qRegisterMetaType<MessageHistoryContainerPtr>("MessageHistoryContainerPtr");

	connect(&m_containerCheckTimer, &QTimer::timeout, this, &CEventHistoryControllerComp::OnContainerCheckTimer);
}


// reimplemented (imtlog::ITimeRangeProvider)

CTimeRange CEventHistoryControllerComp::GetTimeRange() const
{
	return m_archiveTimeRange;
}


// reimplemented (imtlog::IMessageFilter)

IMessageHistoryConsumer::Messages CEventHistoryControllerComp::GetMessages(const IMessageFilter* filterPtr) const
{
	imtlog::IMessageHistoryConsumer::Messages retVal;

	CTimeRange timeRange = filterPtr->GetTimeRange();
	QDateTime begin = timeRange.GetBeginTime();
	QDateTime end = timeRange.GetEndTime();
	QDate beginDate = begin.date();
	QDate endDate = end.date();
	QStringList dateList;

	QMap<QDate, QString> dirMap = GetHistoryDirMap();
	QList<QDate> dirMapKeys = dirMap.keys();

	if (dirMap.isEmpty()){
		return retVal;
	}

	QDate date = beginDate;
	while (date <= endDate){
		if (dirMap.contains(date)){
			QMap<QDateTime, QString> fileMap = GetHistoryFileMap(dirMap[date]);
			QList<QDateTime> fileMapKeys = fileMap.keys();

			for (int i = 0; i < fileMapKeys.count(); i++){
				MessageHistoryContainerPtr container = LoadHistoryContainer(fileMap[fileMapKeys[i]]);
				if (timeRange.Intersect(container->GetTimeRange()).IsValid()){
					imtlog::CMessageHistoryContainer::Messages messages = container->GetMessages();

					for (int j = 0; j < messages.count(); j++){
						QDateTime timeStamp = messages[j]->messagePtr->GetInformationTimeStamp();
						if (timeRange.Contains(timeStamp)){
							if (filterPtr->IsMessageAccepted(messages[j]->messagePtr.GetPtr())){
								retVal.append(messages[j]);
							}
						}
					}
				}
			}
		}

		date = date.addDays(1);
	}

	//imtlog::IMessageHistoryConsumer::Messages::iterator it = retVal.begin();
	//while (it != retVal.end()){
	//	QDateTime timeStamp = it->messagePtr->GetInformationTimeStamp();
	//	if (timeStamp < timeRange.GetBeginTime() || timeStamp > timeRange.GetEndTime()){
	//		it = retVal.erase(it);
	//	}
	//	else{
	//		it++;
	//	}
	//}

	return retVal;
}


// reimplemented (ilog::IMessageConsumer)

bool CEventHistoryControllerComp::IsMessageSupported(
			int /*messageCategory*/,
			int /*messageId*/,
			const istd::IInformationProvider* /*messagePt*/) const
{
	return true;
}


void CEventHistoryControllerComp::AddMessage(const MessagePtr& messagePtr)
{
	BaseClass::AddMessage(messagePtr);

	if (m_controllerState == CS_OK){
		MessageHistoryContainerPtr containerPtr = GetContainerForMessage(messagePtr);
		if (!containerPtr.IsValid()){
			SendWarningMessage(0, QString("Event container for message at \"%1\" not found").arg(messagePtr->GetInformationTimeStamp().toString()));
		}
		else{
			//m_messageCache.AddMessage(messagePtr, m_messageId);

			IMessageHistoryConsumer::MessagePtr msgPtr(new IMessageHistoryConsumer::Message);
			msgPtr->id = m_messageId;
			msgPtr->messagePtr = messagePtr;

			containerPtr->AddMessage(msgPtr);
			if (m_messageHistoryConsumerCompPtr.GetCount() > 0){
				QMetaObject::invokeMethod(
					this, "OnAddMessage", Qt::AutoConnection,
					Q_ARG(const MessagePtr&, messagePtr),
					Q_ARG(quint64, m_messageId));
			}

			m_messageId++;

			QDateTime timestamp = messagePtr->GetInformationTimeStamp();

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
}


// reimplemented (ibase::TRuntimeStatusHanderCompWrap)

void CEventHistoryControllerComp::OnSystemShutdown()
{
	m_controllerState = CS_SHUTDOWN;

	m_containerCheckTimer.stop();

	{
		QMutexLocker workingQueueLocker(&m_workingQueueMutex);
		QMutexLocker writingQueueLocker(&m_writingQueueMutex);

		while (!m_workingQueue.isEmpty()){
			MessageHistoryContainerPtr containerPtr = m_workingQueue.dequeue();
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


// reimplemented (icomp::CComponentBase)

void CEventHistoryControllerComp::OnComponentCreated()
{
	BaseClass::OnComponentCreated();

	m_logFolderCompPtr.EnsureInitialized();
	m_slaveMessageConsumerCompPtr.EnsureInitialized();
	m_compressorCompPtr.EnsureInitialized();
	m_versionInfoCompPtr.EnsureInitialized();

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

	{
		istd::CChangeNotifier notifier(this);
		Init();
	}

	m_systemStartTime = QDateTime::currentDateTime();
	m_containerCheckTimer.start(1000);

	m_controllerState = CS_OK;
}


// private slots

void CEventHistoryControllerComp::OnContainerCheckTimer()
{
	QMutexLocker workingQueueLocker(&m_workingQueueMutex);
	QMutexLocker writingQueueLocker(&m_writingQueueMutex);

	if (!m_workingQueue.isEmpty()){
		if (m_workingQueue.head()->GetEndTime().addSecs(*m_containerWriteDelayAttrPtr) < QDateTime::currentDateTime()){
			MessageHistoryContainerPtr containerPtr = m_workingQueue.dequeue();
			if (containerPtr->GetMessagesCount() > 0){
				m_writingQueue.enqueue(containerPtr);
				StartWriter();
			}
		}
	}

	workingQueueLocker.unlock();
	writingQueueLocker.unlock();
}


void CEventHistoryControllerComp::OnAddMessage(const MessagePtr& messagePtr, quint64 id)
{
	imtlog::IMessageHistoryConsumer::MessagePtr msgPtr;
	msgPtr.SetPtr(new imtlog::IMessageHistoryConsumer::Message);
	msgPtr->id = id;
	msgPtr->messagePtr = messagePtr;

	for (int i = 0; i < m_messageHistoryConsumerCompPtr.GetCount(); i++){
		m_messageHistoryConsumerCompPtr[i]->AddMessage(msgPtr);
	}
}


// private methods


CEventHistoryControllerComp::MessageHistoryContainerPtr CEventHistoryControllerComp::GetContainerForMessage(const MessagePtr& messagePtr)
{
	QDateTime time = messagePtr->GetInformationTimeStamp();

	QMutexLocker workingQueueLocker(&m_workingQueueMutex);

	for (MessageHistoryContainerPtr containerPtr : m_workingQueue){
		if (time >= containerPtr->GetBeginTime() && time <= containerPtr->GetEndTime()){
			return containerPtr;
		}
	}

	if (!m_workingQueue.isEmpty()){
		Q_ASSERT(messagePtr->GetInformationTimeStamp() > m_workingQueue.last()->GetEndTime());
	}

	// Prepare new container

	CTimeRange timeRange = CalculateContainerTimeRange(messagePtr->GetInformationTimeStamp());

	MessageHistoryContainerPtr containerPtr(new CMessageHistoryContainer);
	containerPtr->SetBeginTime(timeRange.GetBeginTime());
	containerPtr->SetEndTime(timeRange.GetEndTime());
	m_workingQueue.enqueue(containerPtr);

	return containerPtr;
}


CTimeRange CEventHistoryControllerComp::CalculateContainerTimeRange(const QDateTime& timestamp)
{
	QDateTime beginTime = timestamp;
	QDateTime endTime = beginTime.addMSecs(*m_containerTimeDurationAttrPtr * 1000 - 1);

	if (endTime.date() != beginTime.date()){
		endTime = QDateTime(beginTime.date(), QTime(23, 59, 59, 999));
	}

	return CTimeRange(beginTime, endTime);
}


QList<CEventHistoryControllerComp::MessageHistoryContainerPtr> CEventHistoryControllerComp::ImportContainersFromFile(const QString& file) const
{
	QList<CEventHistoryControllerComp::MessageHistoryContainerPtr> retVal;

	QDir tempDir = QDir::tempPath();
	QString uuid = QUuid::createUuid().toString();
	if (tempDir.mkpath("ImtCore/" + uuid)){
		tempDir.cd("ImtCore/" + uuid);

		if (m_compressorCompPtr->DecompressFolder(file,	tempDir.path())){
			QStringList containerFileList = tempDir.entryList(QDir::Files | QDir::NoSymLinks);
			QMap<qint64, QString> containerFileMap;

			for (QString containerFile : containerFileList){
				QFileInfo info(containerFile);
				QDateTime timeStamp = QDateTime::fromString(info.completeBaseName(), "dd.MM.yyyy hh-mm-ss.zzz");
				if (timeStamp.isValid()){
					containerFileMap[timeStamp.toMSecsSinceEpoch()] = containerFile;
				}
			}

			for (QString containerFileItem : containerFileMap){
				MessageHistoryContainerPtr containerPtr(new CMessageHistoryContainer);

				ifile::CCompactXmlFileReadArchive xmlArchive(tempDir.path() + "/" + containerFileItem, m_versionInfoCompPtr.GetPtr());
				if (containerPtr->Serialize(xmlArchive)){
					retVal.append(containerPtr);
				}
				else{
					SendErrorMessage(0, tr("Unable to deserialize event container \"%1\". Event container skipped").arg(containerFileItem));
				}
			}
		}
		else{
			SendErrorMessage(0, tr("Unable to decompress event archive \"%1\". Event archive skipped").arg(file));
		}

		tempDir.removeRecursively();
	}
	else{
		SendErrorMessage(0, tr("Unable to create temporary directory"));
	}

	return retVal;
}


void CEventHistoryControllerComp::Init()
{
	QMap<QDate, QString> dirMap = GetHistoryDirMap();

	if (dirMap.isEmpty()){
		return;
	}

	QList<QDate> dirMapKeys = dirMap.keys();

	// Find first container

	MessageHistoryContainerPtr firstContainer;

	for (int i = 0; i < dirMapKeys.count() && !firstContainer.IsValid(); i++){
		QMap<QDateTime, QString> fileMap = GetHistoryFileMap(dirMap[dirMapKeys[i]]);
		QList<QDateTime> fileMapKeys = fileMap.keys();

		for (int j = 0; j < fileMapKeys.count(); j++){
			firstContainer = LoadHistoryContainer(fileMap[fileMapKeys[j]]);

			if (firstContainer.IsValid() && firstContainer->GetMessagesCount() > 0){
				break;
			}

			firstContainer.Reset();
		}
	}

	if (!firstContainer.IsValid()){
		return;
	}

	// Find last container

	MessageHistoryContainerPtr lastContainer;

	for (int i = dirMapKeys.count() - 1; i >= 0 && !lastContainer.IsValid(); i--){
		QMap<QDateTime, QString> fileMap = GetHistoryFileMap(dirMap[dirMapKeys[i]]);
		QList<QDateTime> fileMapKeys = fileMap.keys();

		for (int j = fileMapKeys.count() - 1; j >= 0; j--){
			lastContainer = LoadHistoryContainer(fileMap[fileMapKeys[j]]);

			if (lastContainer.IsValid() && lastContainer->GetMessagesCount() > 0){
				break;
			}

			lastContainer.Reset();
		}
	}

	Q_ASSERT(lastContainer.IsValid());

	// Retreive last MessageId and history time range

	imtlog::CMessageHistoryContainer::Messages firstMessages = firstContainer->GetMessages();
	imtlog::CMessageHistoryContainer::Messages lastMessages = lastContainer->GetMessages();

	m_messageId = lastMessages.last()->id + 1;
	m_archiveTimeRange = CTimeRange(
				firstMessages.first()->messagePtr->GetInformationTimeStamp(),
				lastMessages.last()->messagePtr->GetInformationTimeStamp());

	qDebug() << "";
	qDebug() << "=== Initialize Event History Controller ===";
	qDebug() << "New message ID: " << m_messageId;
	qDebug() << "First message DT: " << m_archiveTimeRange.GetBeginTime();
	qDebug() << "Last message DT: " << m_archiveTimeRange.GetEndTime();
	qDebug() << "===========================================";
	qDebug() << "";
}


QMap<QDate, QString> CEventHistoryControllerComp::GetHistoryDirMap() const
{
	QMap<QDate, QString> map;

	if (!m_logFolderCompPtr.IsValid() || m_logFolderCompPtr->GetPath().isEmpty()){
		return map;
	}

	QDir repositoryDir(m_logFolderCompPtr->GetPath());
	QFileInfoList dateList = repositoryDir.entryInfoList(QDir::Dirs | QDir::NoSymLinks | QDir::NoDotAndDotDot);

	for (int i = 0; i < dateList.count(); i++){
		QString dateStr = dateList[i].fileName();
		QDate date = QDate::fromString(dateStr, "dd.MM.yyyy");
		if (date.isValid()){
			map[date] = dateList[i].filePath();
		}
	}

	return map;
}


QMap<QDateTime, QString> CEventHistoryControllerComp::GetHistoryFileMap(const QString& dir) const
{
	QMap<QDateTime, QString> map;

	if (dir.isEmpty()){
		return map;
	}

	QDir repositoryDir(dir);
	QFileInfoList dateList = repositoryDir.entryInfoList(QDir::Files | QDir::NoSymLinks);

	for (int i = 0; i < dateList.count(); i++){
		QString timestampStr = dateList[i].completeBaseName();
		QDateTime timestamp = QDateTime::fromString(timestampStr, "dd.MM.yyyy hh-mm-ss.zzz");
		if (timestamp.isValid()){
			map[timestamp] = dateList[i].filePath();
		}
	}

	return map;
}


CEventHistoryControllerComp::MessageHistoryContainerPtr CEventHistoryControllerComp::LoadHistoryContainer(const QString& filePath) const
{
	QFileInfo fileInfo(filePath);
	if (!fileInfo.exists()){
		return MessageHistoryContainerPtr();
	}

	QString containerPath;

	QString uuid = QUuid::createUuid().toString();
	QString tempDir = QDir::tempPath() + "/ImtCore/" + uuid;

	if (fileInfo.suffix() == *m_archiveExtensionAttrPtr){
		if (!m_compressorCompPtr.IsValid()){
			SendErrorMessage(0, tr("No compressor component. Load history container failed"));
			return MessageHistoryContainerPtr();
		}

		if (istd::CSystem::EnsurePathExists(tempDir)){
			if (m_compressorCompPtr->DecompressFolder(filePath, tempDir)){
				containerPath = tempDir + "/"  + fileInfo.completeBaseName() + "." + *m_containerExtensionAttrPtr;
			}
		}
		else{
			SendErrorMessage(0, tr("Cannot create temporary folder. Load history container failed"));
		}
	}

	if (fileInfo.suffix() == *m_containerExtensionAttrPtr) {
		containerPath = filePath;
	}

	ifile::CCompactXmlFileReadArchive xmlArchive(containerPath, m_versionInfoCompPtr.GetPtr());
	MessageHistoryContainerPtr containerPtr = MessageHistoryContainerPtr(new CMessageHistoryContainer);
	if (!containerPtr->Serialize(xmlArchive)){
		containerPtr.Reset();
		SendErrorMessage(0, tr("Unable to deserialize history container \"%1\". History container skipped").arg(containerPath));
	}

	if (QDir(tempDir).exists()){
		istd::CSystem::RemoveDirectory(tempDir);
	}

	return containerPtr;
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


// public methods of embedded class Writer

CEventHistoryControllerComp::Writer::Writer(CEventHistoryControllerComp* parentPtr)
	:m_parentPtr(parentPtr)
{
}

// private methods of embedded class Writer

// reimplemented (QThread)

void CEventHistoryControllerComp::Writer::run()
{
	if (!m_parentPtr->m_logFolderCompPtr.IsValid() || m_parentPtr->m_logFolderCompPtr->GetPath().isEmpty()){
		QMutexLocker writingQueueLocker(&m_parentPtr->m_writingQueueMutex);
		m_parentPtr->m_writingQueue.clear();

		return;
	}

	while (!m_parentPtr->m_writingQueue.isEmpty()){
		if (isInterruptionRequested()){
			break;
		}

		MessageHistoryContainerPtr containerPtr;
		{
			QMutexLocker writingQueueLocker(&m_parentPtr->m_writingQueueMutex);
			containerPtr = m_parentPtr->m_writingQueue.dequeue();
		}

		Q_ASSERT(containerPtr.IsValid());

		if (containerPtr->GetMessagesCount() == 0){
			continue;
		}

		QString uuid = QUuid::createUuid().toString();
		QString tempDir = QDir::tempPath() + "/ImtCore/" + uuid;
		if (istd::CSystem::EnsurePathExists(tempDir)){
			do{
				QString containerFileBaseName = containerPtr->GetBeginTime().toString("dd.MM.yyyy hh-mm-ss.zzz");
				QString containerFileName = containerFileBaseName + "." + *m_parentPtr->m_containerExtensionAttrPtr;
				QString containerPath = tempDir + "/" + containerFileName;
				ifile::CCompactXmlFileWriteArchive xmlArchive(containerPath, m_parentPtr->m_versionInfoCompPtr.GetPtr());

				if (!containerPtr->Serialize(xmlArchive)){
					m_parentPtr->SendErrorMessage(
								0, tr("Unable to serialize message history container \"%1\". Event container skipped")
								.arg(containerPath));

					break;
				}

				xmlArchive.Flush();

				QString archiveFileName = containerFileBaseName + "." + *m_parentPtr->m_archiveExtensionAttrPtr;
				QString archivePath = tempDir + "/" + archiveFileName;

				if (m_parentPtr->m_compressorCompPtr.IsValid()){
					if (!m_parentPtr->m_compressorCompPtr->CompressFile(containerPath, archivePath)){
						m_parentPtr->SendErrorMessage(
									0, tr("Unable to compress message history container \"%1\". Event container skipped")
									.arg(archivePath));

						break;
					}
				}

				QString outPath =
							m_parentPtr->m_logFolderCompPtr->GetPath() + "/" +
							containerPtr->GetBeginTime().date().toString("dd.MM.yyyy") + "/" +
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


// public methods of embedded class MessageCache

//CEventHistoryControllerComp::MessageCache::MessageCache(CEventHistoryControllerComp* parentPtr, int cacheDaySize)
//{
//	m_parentPtr = parentPtr;
//	m_cacheDayCount = cacheDaySize;
//}


// private methods of embedded class MessageCache

//void CEventHistoryControllerComp::MessageCache::Init()
//{
//	QDir repositoryDir(m_parentPtr->m_logFolderCompPtr->GetPath());
//	QDate date = QDate::currentDate();
//	//QString fileName = date.toString(*(m_parentPtr->m_archiveNameFormatAttrPtr)) + '.' + *m_parentPtr->m_archiveExtensionAttrPtr;
//
//	m_2dayCache[QDate::fromJulianDay(0)] = imtlog::IMessageHistoryConsumer::Messages();
//	m_2dayCache[date] = imtlog::IMessageHistoryConsumer::Messages();
//
//	//if (m_parentPtr->m_compressorCompPtr.IsValid()){
//	//	QFileInfoList fileInfoList = repositoryDir.entryInfoList(
//	//		QDir::Files | QDir::NoSymLinks,
//	//		QDir::Time | QDir::Reversed);
//
//	//	for (QFileInfo fileInfo : fileInfoList){
//	//		if (fileInfo.fileName() == fileName){
//	//			QList<MessageHistoryContainerPtr> containers = m_parentPtr->ImportContainersFromFile(fileInfo.absoluteFilePath());
//	//			for (MessageHistoryContainerPtr container : containers){
//	//				m_2dayCache.last().append(container->GetMessages());
//	//			}
//	//		}
//	//	}
//	//}
//}
//
//
//void CEventHistoryControllerComp::MessageCache::AddMessage(const MessagePtr& messagePtr, uint64_t id)
//{
//	QDate date = messagePtr->GetInformationTimeStamp().date();
//
//	Q_ASSERT(m_2dayCache.size() == 2);
//
//	if (m_2dayCache.lastKey() != date){
//		m_2dayCache.remove(m_2dayCache.lastKey());
//		m_2dayCache[date] = imtlog::IMessageHistoryConsumer::Messages();
//	}
//
//	Q_ASSERT(m_2dayCache.size() == 2);
//
//	imtlog::IMessageHistoryConsumer::Message message;
//	message.id = id;
//	message.messagePtr = messagePtr;
//
//	m_2dayCache.last().append(message);
//}
//
//
//imtlog::IMessageHistoryConsumer::Messages CEventHistoryControllerComp::MessageCache::GetMessages(const imtlog::CTimeRange& timeRange) const
//{
//	if (m_parentPtr == nullptr){
//		return imtlog::IMessageHistoryConsumer::Messages();
//	}
//
//	imtlog::IMessageHistoryConsumer::Messages retVal;
//
//	QDateTime begin = timeRange.GetBeginTime();
//	QDateTime end = timeRange.GetEndTime();
//	QDate beginDate = begin.date();
//	QDate endDate = end.date();
//	QStringList dateList;
//
//	QDate date = beginDate;
//	while (date <= endDate){
//		if (m_2dayCache.lastKey() == date){
//			imtlog::IMessageHistoryConsumer::Messages messages = m_2dayCache.last();
//			for (int i = 0; i < messages.count(); i++){
//				QDateTime timeStamp = messages[i].messagePtr->GetInformationTimeStamp();
//				if (begin <= timeStamp && timeStamp <= end){
//					retVal.append(messages[i]);
//				}
//			}
//
//			date = date.addDays(1);
//			continue;
//		}
//
//		if (m_2dayCache.firstKey() == date){
//			imtlog::IMessageHistoryConsumer::Messages messages = m_2dayCache.last();
//			for (int i = 0; i < messages.count(); i++){
//				QDateTime timeStamp = messages[i].messagePtr->GetInformationTimeStamp();
//				if (begin <= timeStamp && timeStamp <= end){
//					retVal.append(messages[i]);
//				}
//			}
//
//			date = date.addDays(1);
//			continue;
//		}
//
//		bool isFound = false;
//		for (const QDate& cachedDate : m_cache.keys()){
//			if (cachedDate == date){
//				retVal.append(m_cache[cachedDate]);
//
//				isFound = true;
//				break;
//			}
//		}
//
//		if (!isFound){
//			QDir repositoryDir(m_parentPtr->m_logFolderCompPtr->GetPath());
//			QString fileName;// = date.toString(*(m_parentPtr->m_archiveNameFormatAttrPtr)) + '.' + *m_parentPtr->m_archiveExtensionAttrPtr;
//
//			if (m_parentPtr->m_compressorCompPtr.IsValid()){
//				QFileInfoList fileInfoList = repositoryDir.entryInfoList(
//					QDir::Files | QDir::NoSymLinks,
//					QDir::Time | QDir::Reversed);
//
//				for (QFileInfo fileInfo : fileInfoList){
//					if (fileInfo.fileName() == fileName){
//						QList<MessageHistoryContainerPtr> containers = m_parentPtr->ImportContainersFromFile(fileInfo.absoluteFilePath());
//
//						imtlog::IMessageHistoryConsumer::Messages messages;
//						for (MessageHistoryContainerPtr container : containers){
//							imtlog::IMessageHistoryConsumer::Messages msgs = container->GetMessages();
//
//							Q_ASSERT(msgs.count() > 0);
//
//							messages.append(msgs);
//						}
//
//						Q_ASSERT(m_cache.size() == m_cacheQueue.size());
//
//						while (m_cache.size() >= m_cacheDayCount){
//							m_cache.remove(m_cacheQueue.dequeue());
//						}
//
//						m_cache[date] = messages;
//						m_cacheQueue.enqueue(date);
//
//						for (int i = 0; i < messages.count(); i++){
//							QDateTime timeStamp = messages[i].messagePtr->GetInformationTimeStamp();
//							if (begin <= timeStamp && timeStamp <= end){
//								retVal.append(messages[i]);
//							}
//						}
//
//						break;
//					}
//				}
//			}
//		}
//
//		date = date.addDays(1);
//	}
//
//	imtlog::IMessageHistoryConsumer::Messages::iterator it = retVal.begin();
//	while (it != retVal.end()){
//		QDateTime timeStamp = it->messagePtr->GetInformationTimeStamp();
//		if (timeStamp < timeRange.GetBeginTime() || timeStamp > timeRange.GetEndTime()){
//			it = retVal.erase(it);
//		}
//		else{
//			it++;
//		}
//	}
//
//	return retVal;
//}


} // namespace imtlog


