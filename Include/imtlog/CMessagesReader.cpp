#include <imtlog/CMessagesReader.h>


// Qt includes
#include <QtCore/QDir>
#include <QtCore/QUuid>

// ACF includes
#include <ifile/CCompactXmlFileReadArchive.h>
#include <istd/CSystem.h>
#include <ilog/CMessage.h>

// ImtCore includes
#include <imtlog/CEventContainer.h>


namespace imtlog
{


// public methods

CMessagesReader::CMessagesReader(
			const QString& dir,
			const QString& containerExtension,
			const QString& archiveExtension,
			const iser::IVersionInfo* versionInfoPtr,
			const imtfile::IFileCompression* compressorPtr,
			ilog::IMessageConsumer* logPtr)
	:m_dir(dir),
	m_containerExtension(containerExtension),
	m_archiveExtension(archiveExtension),
	m_versionInfoPtr(versionInfoPtr),
	m_compressorPtr(compressorPtr),
	m_isCanceled(false),
	m_logPtr(logPtr)
{
}


imtbase::CTimeRange CMessagesReader::ReadTimeRange() const
{
	Q_ASSERT(!m_dir.isEmpty());
	Q_ASSERT(!m_containerExtension.isEmpty());
	Q_ASSERT(!m_archiveExtension.isEmpty());
	Q_ASSERT(m_versionInfoPtr != nullptr);

	QDateTime begin;
	QDateTime end;

	QMap<QDate, QString> dirMap = GetDirMap(m_dir);

	if (!dirMap.isEmpty()){
		QMap<QDate, QString>::const_iterator itDate;

		bool finish = false;
		itDate = dirMap.cbegin();
		while (itDate != dirMap.cend() && !finish){
			QMap<QDateTime, QString> fileMap = GetFileMap(*itDate);
			QMap<QDateTime, QString>::const_iterator itTime = fileMap.cbegin();
			while (itTime != fileMap.cend() && !finish){
				EventContainerPtr containerPtr = ImportContainer(*itTime);
				if (containerPtr.IsValid()){
					ilog::IMessageContainer::Messages messages = containerPtr->GetMessages();
					if (!messages.empty()){
						begin = messages.back()->GetInformationTimeStamp();
						finish = true;
					}
				}

				itTime++;
			}

			itDate++;
		}

		finish = false;
		itDate = dirMap.cend();
		while (itDate != dirMap.cbegin() && !finish){
			itDate--;

			QMap<QDateTime, QString> fileMap = GetFileMap(*itDate);
			QMap<QDateTime, QString>::const_iterator itTime = fileMap.cend();
			while (itTime != fileMap.cbegin() && !finish){
				itTime--;

				EventContainerPtr containerPtr = ImportContainer(*itTime);
				if (containerPtr.IsValid()){
					ilog::IMessageContainer::Messages messages = containerPtr->GetMessages();
					if (!messages.empty()){
						end = messages.front()->GetInformationTimeStamp();
						finish = true;
					}
				}
			}
		}
	}

	Q_ASSERT(begin.isValid() == end.isValid());

	return imtbase::CTimeRange(begin, end);
}


CMessagesReader::EventContainerListPtr CMessagesReader::ReadContainers(const imtbase::CTimeRange& timeRange) const
{
	Q_ASSERT(!m_dir.isEmpty());
	Q_ASSERT(!m_containerExtension.isEmpty());
	Q_ASSERT(!m_archiveExtension.isEmpty());
	Q_ASSERT(m_versionInfoPtr != nullptr);

	EventContainerListPtr retVal(new EventContainerList());

	QDateTime begin = timeRange.GetBeginTime();
	QDateTime end = timeRange.GetEndTime();
	QDate beginDate = begin.date();
	QDate endDate = end.date();
	QStringList dateList;

	QMap<QDate, QString> dirMap = GetDirMap(m_dir);

	if (dirMap.isEmpty()){
		return retVal;
	}

	QDate date = beginDate;
	while (date <= endDate){
		if (dirMap.contains(date)){
			QMap<QDateTime, QString> fileMap = GetFileMap(dirMap[date]);
			QList<QDateTime> fileMapKeys = fileMap.keys();

			for (int i = 0; i < fileMapKeys.count(); i++){
				if (m_isCanceled){
					return EventContainerListPtr();
				}

				EventContainerPtr container = ImportContainer(fileMap[fileMapKeys[i]]);
				if (timeRange.Intersect(container->GetTimeRange()).IsClosed()){
					const ilog::IMessageContainer::Messages messages = container->GetMessages();

					for (ilog::IMessageContainer::Messages::const_iterator it = messages.begin(); it != messages.end(); it++) {
						QDateTime timeStamp = (*it)->GetInformationTimeStamp();
						if (timeRange.Contains(timeStamp)){
							retVal->append(container);
						}
					}
				}
			}
		}

		date = date.addDays(1);
	}

	return retVal;
}


void CMessagesReader::Cancel()
{
	m_isCanceled = true;
}


// private methods

void CMessagesReader::SendErrorMessage(const QString& message) const
{
	if (m_logPtr != nullptr){
		ilog::IMessageConsumer::MessagePtr messagePtr(new ilog::CMessage(istd::IInformationProvider::IC_ERROR, 0, message, "imtlog::CMessageReader"));
		m_logPtr->AddMessage(messagePtr);
	}
}


QMap<QDate, QString> CMessagesReader::GetDirMap(const QString& dirPath) const
{
	QMap<QDate, QString> map;

	QDir dir(dirPath);
	QFileInfoList list = dir.entryInfoList(QDir::Dirs | QDir::NoSymLinks | QDir::NoDotAndDotDot);

	for (int i = 0; i < list.count(); i++){
		QString dirName = list[i].fileName();
		QDate date = QDate::fromString(dirName, "dd.MM.yyyy");
		if (date.isValid()){
			map[date] = list[i].filePath();
		}
	}

	return map;
}


QMap<QDateTime, QString> CMessagesReader::GetFileMap(const QString& dirPath) const
{
	QMap<QDateTime, QString> map;

	QDir dir(dirPath);
	QFileInfoList list = dir.entryInfoList(QDir::Files | QDir::NoSymLinks);

	for (int i = 0; i < list.count(); i++){
		QString fileName = list[i].completeBaseName();
		QDateTime timestamp = QDateTime::fromString(fileName, "dd.MM.yyyy hh-mm-ss.zzz");
		if (timestamp.isValid()){
			map[timestamp] = list[i].filePath();
		}
	}

	return map;
}


CMessagesReader::EventContainerPtr CMessagesReader::ImportContainer(const QString& filePath) const
{
	EventContainerPtr containerPtr;

	QFileInfo info(filePath);
	if (!info.exists()){
		return containerPtr;
	}

	QString containerPath;
	QString uuid = QUuid::createUuid().toString();
	QString tempDir = QDir::tempPath() + "/ImtCore/" + uuid;

	if (info.suffix() == m_archiveExtension){
		if (m_compressorPtr == nullptr){
			SendErrorMessage(QObject::tr("No compressor component. Load history container failed"));
			return containerPtr;
		}

		if (istd::CSystem::EnsurePathExists(tempDir)){
			if (m_compressorPtr->DecompressFolder(filePath, tempDir)){
				containerPath = tempDir + "/" + info.completeBaseName() + "." + m_containerExtension;
			}
		}
		else{
			SendErrorMessage(QObject::tr("Cannot create temporary folder. Load history container failed"));
		}
	}

	if (info.suffix() == m_containerExtension) {
		containerPath = filePath;
	}

	ifile::CCompactXmlFileReadArchive xmlArchive(containerPath, m_versionInfoPtr);
	containerPtr.SetPtr(new CEventContainer);
	if (!containerPtr->Serialize(xmlArchive)){
		containerPtr.Reset();
		SendErrorMessage(QObject::tr("Unable to deserialize history container \"%1\". History container skipped").arg(containerPath));
	}

	if (QDir(tempDir).exists()){
		istd::CSystem::RemoveDirectory(tempDir);
	}

	return containerPtr;
}


} // namespace imtlog


