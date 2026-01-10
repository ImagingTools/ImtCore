#include <imthype/CJobQueueManagerCompBase.h>


// Qt includes
#include <QtCore/QFileInfo>
#include <QtCore/QFile>
#include <QtCore/QUuid>

// ACF includes
#include <istd/TDelPtr.h>
#include <istd/CSystem.h>
#include <iser/CPrimitiveTypesSerializer.h>
#include <ifile/CCompactXmlFileReadArchive.h>
#include <ifile/CCompactXmlFileWriteArchive.h>
#include <ifile/CFileListProviderComp.h>

// ImtCore includes
#include <imtbase/imtbase.h>

namespace imthype
{


// public methods

CJobQueueManagerCompBase::CJobQueueManagerCompBase()
	:m_directoryBlocked(false),
	m_mutex(QReadWriteLock::Recursive)
{
}


// reimplemented (IJobQueueManager)

QByteArray CJobQueueManagerCompBase::GetTaskTypeId(const QByteArray& jobId) const
{
	QReadLocker lock(&m_mutex);

	int index = FindItemById(jobId);
	if (index >= 0){
		return m_jobItems[index].typeId;
	}

	return QByteArray();
}


QByteArray CJobQueueManagerCompBase::InsertNewJobIntoQueue(
			const QByteArray& contextId,
			const QByteArray& typeId,
			const imtbase::IReferenceCollection& input,
			const iprm::IParamsSet* jobProcessingParamsPtr,
			const IJobSchedulerParams* /*schedulerParamsPtr*/,
			ilog::IMessageConsumer* /*logPtr*/)
{
	DirectoryBlocker blockDirectory(*this);

	JobItem jobItem;

	jobItem.contextId = contextId;
	jobItem.uuid = QUuid::createUuid().toByteArray();
	jobItem.typeId = typeId;
	jobItem.processingStatus = PS_WAITING_FOR_ACCEPTING;
	
	if (!jobItem.input.CopyFrom(input)){
		return QByteArray();
	}

	if (jobProcessingParamsPtr != nullptr){
		jobItem.paramsPtr.FromUnique(CreateJobParameters(contextId, typeId, jobProcessingParamsPtr));
		if (!jobItem.paramsPtr.IsValid()){
			SendErrorMessage(0, "Job parameters could not be created");

			return QByteArray();
		}
	}

	istd::IChangeable::ChangeSet changeSet = istd::IChangeable::GetAnyChange();
	imtbase::ICollectionInfo::NotifierInfo info;
	info.elementId = jobItem.uuid;
	changeSet.SetChangeInfo(imtbase::ICollectionInfo::CN_ELEMENT_INSERTED, QVariant::fromValue(info));

	istd::CChangeNotifier changeNotifier(this, &changeSet);

	QWriteLocker lock(&m_mutex);

	m_jobItems.push_back(jobItem);

	SaveJobItem(jobItem);

	lock.unlock();

	return jobItem.uuid;
}


bool CJobQueueManagerCompBase::CancelJob(const QByteArray & jobId)
{
	DirectoryBlocker blockDirectory(*this);

	QWriteLocker lock(&m_mutex);

	int index = FindItemById(jobId);
	if (index >= 0){
		JobItem& item = m_jobItems[index];

		istd::IChangeable::ChangeSet changeSet = istd::IChangeable::GetAnyChange();
		std::unique_ptr<istd::CChangeNotifier> notifierPtr;

		if (item.processingStatus < PS_RUNNING){
			JobStatusInfo info;
			info.elementId = jobId;
			info.status = PS_CANCELED;

			changeSet.SetChangeInfo(IJobQueueManager::CN_JOB_STATUS_CHANGED, QVariant::fromValue(info));

			notifierPtr.reset(new istd::CChangeNotifier(this, &changeSet));

			item.processingStatus = PS_CANCELED;
		}

		if (item.processingStatus == PS_RUNNING){
			JobStatusInfo info;
			info.elementId = jobId;
			info.status = PS_CANCELING;

			changeSet.SetChangeInfo(IJobQueueManager::CN_JOB_STATUS_CHANGED, QVariant::fromValue(info));

			notifierPtr.reset(new istd::CChangeNotifier(this, &changeSet));

			item.processingStatus = PS_CANCELING;
		}

		SaveJobItem(item);

		lock.unlock();

		return true;
	}

	return false;
}


bool CJobQueueManagerCompBase::ResumeJob(const QByteArray & jobId)
{
	DirectoryBlocker blockDirectory(*this);

	QWriteLocker lock(&m_mutex);

	int index = FindItemById(jobId);
	if (index >= 0){
		JobItem& item = m_jobItems[index];

		if ((item.processingStatus == PS_CANCELED) || (item.processingStatus == PS_FINISHED)){
			istd::IChangeable::ChangeSet changeSet = istd::IChangeable::GetAnyChange();
			JobStatusInfo info;
			info.elementId = jobId;
			info.status = PS_WAITING_FOR_PROCESSING;

			changeSet.SetChangeInfo(IJobQueueManager::CN_JOB_STATUS_CHANGED, QVariant::fromValue(info));

			istd::CChangeNotifier changeNotifier(this, &changeSet);

			item.processingStatus = PS_WAITING_FOR_PROCESSING;

			SaveJobItem(item);

			lock.unlock();

			return true;
		}
	}

	lock.unlock();

	return false;
}


bool CJobQueueManagerCompBase::RemoveJob(const QByteArray& jobId)
{
	DirectoryBlocker blockDirectory(*this);

	QWriteLocker lock(&m_mutex);

	int index = FindItemById(jobId);
	if (index >= 0){
		// Job is running:
		const JobItem& item = m_jobItems[index];
		if (m_jobItems[index].processingStatus > PS_NONE && m_jobItems[index].processingStatus < PS_CANCELED){
			if (!CancelJob(jobId)){
				lock.unlock();
			
				return false;
			}
		}

		istd::IChangeable::ChangeSet changeSet = istd::IChangeable::GetAnyChange();
		imtbase::ICollectionInfo::ElementsRemoveInfo info;
		info.elementIds << jobId;

		changeSet.SetChangeInfo(imtbase::ICollectionInfo::CN_ELEMENTS_REMOVED, QVariant::fromValue(info));

		istd::CChangeNotifier changeNotifier(this, &changeSet);

		QFile::remove(GetJobItemPath(item.uuid));

		m_jobItems.removeAt(index);

		lock.unlock();

		return true;
	}

	lock.unlock();

	return false;
}


bool CJobQueueManagerCompBase::GetJobConfiguration(
			const QByteArray& jobId,
			ParamsPtr& processingParamsPtr,
			imtbase::IReferenceCollection& input) const
{
	QReadLocker lock(&m_mutex);

	int index = FindItemById(jobId);
	if (index >= 0){
		const JobItem& jobItem = m_jobItems[index];
		
		if (input.CopyFrom(jobItem.input)){
			if (jobItem.paramsPtr.IsValid()){
				processingParamsPtr.FromUnique(CreateJobParameters(jobItem.contextId, jobItem.typeId, nullptr));
				if (processingParamsPtr.IsValid()){
					return processingParamsPtr->CopyFrom(*jobItem.paramsPtr);
				}
			}
		}
	}

	return false;
}


CJobQueueManagerCompBase::ProcessingStatus CJobQueueManagerCompBase::GetProcessingStatus(const QByteArray& jobId) const
{
	QReadLocker lock(&m_mutex);

	int index = FindItemById(jobId);
	if (index >= 0){
		return m_jobItems[index].processingStatus;
	}

	return PS_NONE;
}


bool CJobQueueManagerCompBase::SetProcessingStatus(const QByteArray & jobId, ProcessingStatus status)
{
	DirectoryBlocker blockDirectory(*this);

	int index = FindItemById(jobId);
	if (index >= 0){
		JobItem& item = m_jobItems[index];

		istd::IChangeable::ChangeSet changeSet = istd::IChangeable::GetAnyChange();
		JobStatusInfo info;
		info.elementId = jobId;
		info.status = status;

		changeSet.SetChangeInfo(IJobQueueManager::CN_JOB_STATUS_CHANGED, QVariant::fromValue(info));

		istd::CChangeNotifier changeNotifier(this, &changeSet);

		item.processingStatus = status;

		SaveJobItem(item);

		return true;
	}

	return false;
}


double CJobQueueManagerCompBase::GetProgress(const QByteArray & jobId) const
{
	QReadLocker lock(&m_mutex);

	int index = FindItemById(jobId);
	if (index >= 0){
		return m_jobItems[index].progress;
	}

	return 0.0;
}


bool CJobQueueManagerCompBase::SetProgress(const QByteArray& jobId, double progress)
{
	int index = FindItemById(jobId);
	if (index >= 0){
		JobItem& item = m_jobItems[index];

		if (!qFuzzyCompare(item.progress, progress)){
			istd::IChangeable::ChangeSet changeSet = istd::IChangeable::GetAnyChange();
			JobProgressInfo info;
			info.elementId = jobId;
			info.progress = progress;

			changeSet.SetChangeInfo(IJobQueueManager::CN_JOB_PROGRESS_CHANGED, QVariant::fromValue(info));

			istd::CChangeNotifier changeNotifier(this, &changeSet);

			item.progress = progress;

			SaveJobItem(item);

			return true;
		}
	}

	return false;
}


bool CJobQueueManagerCompBase::GetJobResult(const QByteArray& jobId, IJobOutput& result) const
{
	QReadLocker lock(&m_mutex);

	int index = FindItemById(jobId);
	if (index >= 0){
		return result.CopyFrom(m_jobItems[index].results);
	}

	return false;
}


bool CJobQueueManagerCompBase::SetJobResult(const QByteArray& jobId, const IJobOutput& result)
{
	int index = FindItemById(jobId);
	if (index >= 0){
		JobItem& item = m_jobItems[index];

		if (!result.IsEqual(item.results)){
			istd::IChangeable::ChangeSet changeSet = istd::IChangeable::GetAnyChange();
			JobResultInfo info;
			info.elementId = jobId;

			changeSet.SetChangeInfo(IJobQueueManager::CN_JOB_RESULT_CHANGED, QVariant::fromValue(info));

			istd::CChangeNotifier changeNotifier(this, &changeSet);

			item.results.CopyFrom(result);

			SaveJobItem(item);

			return true;
		}
	}

	return false;
}


// reimplemented (imtbase::ICollectionInfo)

int CJobQueueManagerCompBase::GetElementsCount(const iprm::IParamsSet* /*selectionParamPtr*/, ilog::IMessageConsumer* /*logPtr*/) const
{
	QReadLocker lock(&m_mutex);

	return imtbase::narrow_cast<int>(m_jobItems.size());
}



imtbase::ICollectionInfo::Ids CJobQueueManagerCompBase::GetElementIds(
			int offset,
			int count,
			const iprm::IParamsSet* /*selectionParamsPtr*/,
			ilog::IMessageConsumer* /*logPtr*/) const
{
	QReadLocker lock(&m_mutex);

	Ids retVal;

	Q_ASSERT(offset >= 0);

	qsizetype elementsCount = count >= 0 ? qMin(count, m_jobItems.size()) : m_jobItems.size();

	for (int i = offset; i < elementsCount; ++i){
		retVal.push_back(m_jobItems[i].uuid);
	}

	return retVal;
}

bool CJobQueueManagerCompBase::GetSubsetInfo(
			imtbase::ICollectionInfo& /*subsetInfo*/,
			int /*offset*/,
			int /*count*/,
			const iprm::IParamsSet* /*selectionParamsPtr*/,
			ilog::IMessageConsumer* /*logPtr*/) const
{
	return false;
}


QVariant CJobQueueManagerCompBase::GetElementInfo(const QByteArray& elementId, int infoType, ilog::IMessageConsumer* /*logPtr*/) const
{
	QReadLocker lock(&m_mutex);

	int index = FindItemById(elementId);
	if (index >= 0){
		switch (infoType){
		case EIT_NAME:
			return m_jobItems[index].name;
		default:
			I_IF_DEBUG(qWarning() << __FILE__ << __LINE__ << "Unexpected info type: " << infoType;)
			break;
		}
	}

	return QVariant();
}


idoc::MetaInfoPtr CJobQueueManagerCompBase::GetElementMetaInfo(const Id& /*elementId*/, ilog::IMessageConsumer* /*logPtr*/) const
{
	return idoc::MetaInfoPtr();
}


bool CJobQueueManagerCompBase::SetElementName(const Id& /*elementId*/, const QString& /*name*/, ilog::IMessageConsumer* /*logPtr*/)
{
	return false;
}


bool CJobQueueManagerCompBase::SetElementDescription(const Id& /*elementId*/, const QString& /*description*/, ilog::IMessageConsumer* /*logPtr*/)
{
	return false;
}


bool CJobQueueManagerCompBase::SetElementEnabled(const Id& /*elementId*/, bool /*isEnabled*/, ilog::IMessageConsumer* /*logPtr*/)
{
	return false;
}


// protected methods

bool CJobQueueManagerCompBase::SerializeJobItem(JobItem& item, iser::IArchive& archive) const
{
	bool retVal = true;

	static iser::CArchiveTag contextIdTag("ContextId", "ID of the job context", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(contextIdTag);
	retVal = retVal && archive.Process(item.contextId);
	retVal = retVal && archive.EndTag(contextIdTag);

	static iser::CArchiveTag uuidTag("Uuid", "UUID of the job", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(uuidTag);
	retVal = retVal && archive.Process(item.uuid);
	retVal = retVal && archive.EndTag(uuidTag);

	static iser::CArchiveTag typeIdTag("TypeId", "Processing type-ID", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(typeIdTag);
	retVal = retVal && archive.Process(item.typeId);
	retVal = retVal && archive.EndTag(typeIdTag);

	static iser::CArchiveTag nameTag("Name", "Name of the job", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(nameTag);
	retVal = retVal && archive.Process(item.name);
	retVal = retVal && archive.EndTag(nameTag);

	static iser::CArchiveTag inputTag("Input", "Job input", iser::CArchiveTag::TT_GROUP);
	retVal = retVal && archive.BeginTag(inputTag);
	retVal = retVal && item.input.Serialize(archive);
	retVal = retVal && archive.EndTag(inputTag);

	static iser::CArchiveTag statusTag("Status", "Processing status", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(statusTag);
	retVal = retVal && I_SERIALIZE_ENUM(ProcessingStatus, archive, item.processingStatus);
	retVal = retVal && archive.EndTag(statusTag);

	static iser::CArchiveTag progressTag("Progress", "Processing progress", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(progressTag);
	retVal = retVal && archive.Process(item.progress);
	retVal = retVal && archive.EndTag(progressTag);

	if (retVal && !archive.IsStoring()){
		item.paramsPtr.FromUnique(CreateJobParameters(item.contextId, item.typeId, nullptr));
	}

	if (item.paramsPtr.IsValid()){
		static iser::CArchiveTag paramsTag("Configuration", "Processing parameters", iser::CArchiveTag::TT_GROUP);
		retVal = retVal && archive.BeginTag(paramsTag);
		retVal = retVal && item.paramsPtr->Serialize(archive);
		retVal = retVal && archive.EndTag(paramsTag);
	}

	static iser::CArchiveTag resultsTag("Results", "Job results", iser::CArchiveTag::TT_GROUP);
	retVal = retVal && archive.BeginTag(resultsTag);
	retVal = retVal && item.results.Serialize(archive);
	retVal = retVal && archive.EndTag(resultsTag);

	return retVal;
}


QString CJobQueueManagerCompBase::SaveJobItem(const JobItem& jobItem) const
{
	if (!m_dataFolderCompPtr.IsValid()){
		return QString();
	}

	QString rootFolder = m_dataFolderCompPtr->GetPath();
	istd::CSystem::EnsurePathExists(rootFolder);

	QString itemFilePath = GetJobItemPath(jobItem.uuid);

	ifile::CCompactXmlFileWriteArchive archive(itemFilePath, m_versionInfoCompPtr.GetPtr());

	if (!SerializeJobItem(const_cast<JobItem&>(jobItem), archive)){
		SendErrorMessage(0, QString("Job item '%1' could not be saved into '%2'").arg(jobItem.name).arg(itemFilePath));

		return QString();
	}

	return itemFilePath;
}


QString CJobQueueManagerCompBase::GetJobItemPath(const QByteArray& jobId) const
{
	if (!m_dataFolderCompPtr.IsValid()){
		return QString();
	}

	QString rootFolder = m_dataFolderCompPtr->GetPath();

	QString itemFilePath = rootFolder + "/" + jobId + ".xml";

	return itemFilePath;
}


int CJobQueueManagerCompBase::FindItemById(const QByteArray& itemId) const
{
	for (int i = 0; i < m_jobItems.count(); ++i){
		if (m_jobItems[i].uuid == itemId){
			return i;
		}
	}

	return -1;
}


// reimplemented (icomp::CComponentBase)

void CJobQueueManagerCompBase::OnComponentCreated()
{
	BaseClass::OnComponentCreated();

	DirectoryBlocker blockDirectory(*this);

	m_dataFolderCompPtr.EnsureInitialized();
	if (!m_dataFolderCompPtr.IsValid()){
		SendCriticalMessage(0, "Wrong component configuration. Job data folder component was not set");
	}
	else{
		QString dataFolderPath = m_dataFolderCompPtr->GetPath();
		if (dataFolderPath.isEmpty()){
			SendErrorMessage(0, "Job data folder was not set");
			
			return;
		}

		if (!QFile::exists(dataFolderPath)){
			if (!istd::CSystem::EnsurePathExists(dataFolderPath)){
				SendErrorMessage(0, QString("Job data folder '%1' could not be created").arg(dataFolderPath));
			
				return;
			}
		}

		istd::CChangeNotifier changeNotifier(this);

		QWriteLocker lock(&m_mutex);

		ReadJobItems(m_jobItems);

		if (*m_pollFileSystemAttrPtr){
			connect(&m_syncTimer, &QTimer::timeout, this, &CJobQueueManagerCompBase::OnSync);

			m_syncTimer.start(std::chrono::seconds(1));
		}

		lock.unlock();
	}
}


void CJobQueueManagerCompBase::OnComponentDestroyed()
{
	BaseClass::OnComponentDestroyed();
}


// private methods

void CJobQueueManagerCompBase::ReadJobItems(JobItems& items) const
{
	items.clear();

	if (!m_dataFolderCompPtr.IsValid()){
		return;
	}

	QString rootFolder = m_dataFolderCompPtr->GetPath();
	QDir itemsDir(rootFolder);

	QFileInfoList itemFiles;
	ifile::CFileListProviderComp::CreateFileList(itemsDir, 0, 0, QStringList() << "*.xml", QDir::Name, itemFiles);

	for (int fileIndex = 0; fileIndex < itemFiles.count(); ++fileIndex){
		QString itemFilePath = itemFiles[fileIndex].absoluteFilePath();

		ifile::CCompactXmlFileReadArchive archive(itemFilePath);
		JobItem jobItem;

		if (!SerializeJobItem(jobItem, archive)){
			SendErrorMessage(0, QString("Job item '%1' could not be loaded from'%2'").arg(jobItem.name).arg(itemFilePath));

			QFile::remove(itemFilePath);

			continue;
		}

		items.push_back(jobItem);
	}
}


// private slots

void CJobQueueManagerCompBase::OnSync()
{
	if (!m_directoryBlocked){
		JobItems items;
		ReadJobItems(items);

		istd::CChangeNotifier changeNotifier(this);

		QWriteLocker lock(&m_mutex);

		m_jobItems = items;

		lock.unlock();
	}
}


} // namespace imthype


