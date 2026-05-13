// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtservergql/CCollectionImportControllerComp.h>


// Qt includes
#include <QtCore/QMutexLocker>
#include <QtCore/QFile>

// ACF includes
#include <istd/TDelPtr.h>
#include <iprm/TParamsPtr.h>
#include <iprm/IParamsManager.h>
#include <iprm/IIdParam.h>
#include <ifile/IFileNameParam.h>

// ImtCore includes
#include <imtbase/CSimpleReferenceCollection.h>
#include <imthype/CStandardJobOutput.h>


namespace imtservergql
{


// public methods

CCollectionImportControllerComp::CCollectionImportControllerComp()
	:m_jobQueueObserver(*this)
{
}


// reimplemented (imtbase::IProgressLoggerProvider)

bool CCollectionImportControllerComp::BeginCollectionImportSession(const SessionInfo& sessionInfo, QString& errorMessage)
{
	std::shared_ptr<WorkingSessionInfo> sessionPtr(new WorkingSessionInfo);
	*dynamic_cast<SessionInfo*>(sessionPtr.get()) = sessionInfo;

	sessionPtr->tempFileSessionId = m_fileManagerCompPtr->BeginSession(QString(), sessionPtr->sessionId);

	for (const FileInfo& fileInfo : sessionInfo.files){
		WorkingFileInfo file;

		*dynamic_cast<FileInfo*>(&file) = fileInfo;

		file.uploadProgressLoggerPtr.reset(new ProgressLogger);
		file.uploadProgressLoggerPtr->SetParent(*this);
		file.uploadProgressLoggerPtr->sessionId = sessionPtr->sessionId;
		file.uploadProgressLoggerPtr->fileId = fileInfo.id;
		file.tempFileId = m_fileManagerCompPtr->AddFileItem(sessionPtr->tempFileSessionId, fileInfo.name, fileInfo.id);
		file.path = m_fileManagerCompPtr->GetPath(sessionPtr->tempFileSessionId, file.tempFileId);

		if (fileInfo.additionalParams.IsValid()){
			file.additionalParams.MoveCastedPtr(fileInfo.additionalParams->CloneMe());
			if (!file.additionalParams.IsValid()){
				return false;
			}
		}

		// Modify base file info ICollectionImportController::SessionInfo::files
		sessionPtr->files[fileInfo.id] = file;
		SessionInfo* baseSessionPtr = dynamic_cast<SessionInfo*>(sessionPtr.get());
		Q_ASSERT(baseSessionPtr != nullptr);
		for (FileInfo& baseFile : baseSessionPtr->files){
			if (baseFile.id == file.id){
				baseFile.path = file.path;

				break;
			}
		}

		if (!PrepareProgressManager(*sessionPtr)){
			errorMessage = tr("Unable to initialize progress manager");

			return false;
		}

		if (!file.tempFileId.isEmpty()){
			Q_ASSERT(!file.path.isEmpty());

			QMutexLocker locker(&m_mutex);

			m_sessions[sessionPtr->sessionId] = sessionPtr;
		}
		else {
			errorMessage = tr("Unable to create temporary fileInfo for upload");

			return false;
		}
	}

	return true;
}


bool CCollectionImportControllerComp::CancelCollectionImportSession(const QByteArray& sessionId, QString& errorMessage)
{
	QMutexLocker locker(&m_mutex);

	if (m_sessions.contains(sessionId)){
		QByteArray jobId = m_sessions[sessionId]->jobId;

		if (!jobId.isEmpty()){
			m_jobQueueManagerCompPtr->CancelJob(jobId);
		}
		else{
			m_fileManagerCompPtr->FinishSession(sessionId);
			m_progressSessionManagerCompPtr->CancelProgressSession(sessionId, tr("Collection import cancelled"));
		}
	}
	else{
		errorMessage = tr("Collection import session not found");

		return false;
	}

	return true;
}


IFileUploadHandler::FilelUploadStatus CCollectionImportControllerComp::ReceiveFileChunk(
			const QByteArray& fileId,
			const istd::CIntRange& range,
			const QByteArray& data)
{
	QByteArray sessionId = FindSession(fileId);

	if (!m_fileManagerCompPtr.IsValid()){
		return FUS_FAILED_TO_OPEN;
	}

	QString filePath = m_fileManagerCompPtr->GetPath(sessionId, fileId);
	if (filePath.isEmpty()){
		return FUS_FAILED_TO_OPEN;
	}

	QFile file(filePath);
	if (!file.open(QFile::ReadWrite)){
		return FUS_FAILED_TO_OPEN;
	}

	if (!file.seek(range.GetMinValue())){
		return FUS_FAILED_TO_SEEK;
	}

	if (file.write(data) != data.size()){
		return FUS_FAILED_TO_WRITE;
	}

	file.close();

	FileInfo* fileInfoPtr = FindFileInfo(fileId);
	Q_ASSERT(fileInfoPtr != nullptr);

	if (range.GetMinValue() < 0 || range.GetMaxValue() > fileInfoPtr->size){
		return FUS_CHUNK_OUT_OF_RANGE;
	}

	FilelUploadStatus retVal = AddFileChunk(fileId, range);

	UpdateUploadProgress(fileId);

	return retVal;
}


// protected methods

// reimplemented (icomp::CComponentBase)

void CCollectionImportControllerComp::OnComponentCreated()
{
	BaseClass::OnComponentCreated();

	Q_ASSERT(
		m_fileManagerCompPtr.IsValid() &&
		m_progressSessionManagerCompPtr.IsValid() &&
		m_jobQueueManagerCompPtr.IsValid() &&
		m_paramsFillerCompPtr.IsValid() &&
		m_jobParamsFactPtr.IsValid());

	if (m_fileManagerCompPtr.IsValid()){
		m_fileManagerCompPtr->ResetAllSessions();
	}

	if (m_jobQueueManagerCompPtr.IsValid()){
		m_jobQueueObserver.RegisterObject(m_jobQueueManagerCompPtr.GetPtr(), &CCollectionImportControllerComp::OnJobQueueChanged);
	}
}


void CCollectionImportControllerComp::OnComponentDestroyed()
{
	m_jobQueueObserver.UnregisterAllObjects();

	BaseClass::OnComponentDestroyed();
}


// private methods

bool CCollectionImportControllerComp::PrepareProgressManager(WorkingSessionInfo& session) const
{
	session.mainProgressManagerPtr = m_progressSessionManagerCompPtr->BeginProgressSession(session.sessionId, tr("Import files to the collection"));
	session.uploadProgressManagerPtr = session.mainProgressManagerPtr->CreateSubtaskManager("uploading", tr("Uploading files to the server"), 0.2);
	session.fileProcessingProgressManagerPtr = session.mainProgressManagerPtr->CreateSubtaskManager("processing", tr("Processing files on the server"), 0.8);
	session.fileProcessingProgressLoggerPtr = session.fileProcessingProgressManagerPtr->StartProgressLogger();

	for (WorkingFileInfo& fileInfo: session.files){
		fileInfo.uploadProgressLoggerPtr->progressManagerPtr =
			session.uploadProgressManagerPtr->CreateSubtaskManager(fileInfo.id, fileInfo.name, 1./session.files.size());
		if (fileInfo.uploadProgressLoggerPtr->progressManagerPtr == nullptr){
			return false;
		}

		fileInfo.uploadProgressLoggerPtr->progressLoggerPtr =
			fileInfo.uploadProgressLoggerPtr->progressManagerPtr->StartProgressLogger(false, fileInfo.name);
		if (fileInfo.uploadProgressLoggerPtr->progressLoggerPtr == nullptr){
			return false;
		}
	}

	return true;
}


void CCollectionImportControllerComp::UploadProgressChanged(QByteArray sessionId, QByteArray /*fileId*/)
{
	QMutexLocker locker(&m_mutex);

	if (m_sessions.contains(sessionId)){
		bool uploadingFinished = true;

		for (const WorkingFileInfo& fileInfo : m_sessions[sessionId]->files){
			if (fileInfo.uploadProgressLoggerPtr->m_progress < 1.0){
				uploadingFinished = false;

				break;
			}
		}

		if (uploadingFinished){
			StartImportJob(*m_sessions[sessionId]);
		}
	}
}


bool CCollectionImportControllerComp::StartImportJob(WorkingSessionInfo& session)
{
	imtbase::CSimpleReferenceCollection refCollection;

	int index = FindCollectionIndex(session.collectionId);
	if (index < 0){
		return false;
	}

	QByteArray jobTypeId = m_jobTypeIdAttrPtr[index];
	if (jobTypeId.isEmpty()){
		return false;
	}

	if (m_jobTypeIdAttrPtr[index].isEmpty() || m_jobParamsFactPtr[index] == nullptr || m_paramsFillerCompPtr[index] != nullptr){

	}

	iprm::IParamsSetUniquePtr jobParamsPtr = m_jobParamsFactPtr.CreateInstance(index);
	if (!jobParamsPtr.IsValid()) {
		return false;
	}

	if (!m_paramsFillerCompPtr[index]->FillCollectionImportJobParams(session, *jobParamsPtr.GetPtr())){
		return false;
	}

	session.jobId = m_jobQueueManagerCompPtr->InsertNewJobIntoQueue(
		session.sessionId,
		jobTypeId,
		refCollection,
		jobParamsPtr.GetPtr());

	return !session.jobId.isEmpty();
}


void CCollectionImportControllerComp::OnJobQueueChanged(const istd::IChangeable::ChangeSet& changeset, const imthype::IJobQueueManager* /*modelPtr*/)
{
	if (changeset.GetChangeInfoMap().contains(imthype::IJobQueueManager::CN_JOB_PROGRESS_CHANGED)){
		QVariant value = changeset.GetChangeInfoMap().value(imthype::IJobQueueManager::CN_JOB_PROGRESS_CHANGED);
		imthype::IJobQueueManager::JobProgressInfo info = value.value<imthype::IJobQueueManager::JobProgressInfo>();

		QMutexLocker locker(&m_mutex);

		for (const SessionInfoPtr& sessionPtr : m_sessions){
			if (sessionPtr->jobId == info.elementId){
				sessionPtr->fileProcessingProgressLoggerPtr->OnProgress(info.progress);
			}
		}
	}

	if (changeset.GetChangeInfoMap().contains(imthype::IJobQueueManager::CN_JOB_STATUS_CHANGED)){
		if (changeset.GetChangeInfoMap().contains(imtbase::ICollectionInfo::CN_ELEMENTS_REMOVED)){
			return;
		}

		QVariant value = changeset.GetChangeInfoMap().value(imthype::IJobQueueManager::CN_JOB_STATUS_CHANGED);
		imthype::IJobQueueManager::JobStatusInfo info = value.value<imthype::IJobQueueManager::JobStatusInfo>();

		if (info.status == imthype::IJobQueueManager::PS_FINISHED || info.status == imthype::IJobQueueManager::PS_CANCELED){
			QMutexLocker locker(&m_mutex);

			if (!m_sessions.isEmpty()){
				for (SessionInfoPtr& sessionPtr : m_sessions){
					if (sessionPtr->jobId == info.elementId){
						QByteArray sessionId = sessionPtr->sessionId;

						imthype::CStandardJobOutput jobOutput;
						m_jobQueueManagerCompPtr->GetJobResult(sessionPtr->jobId, jobOutput);
						istd::IInformationProvider::InformationCategory category = jobOutput.GetInformationCategory();

						m_fileManagerCompPtr->FinishSession(sessionId);

						if (info.status == imthype::IJobQueueManager::PS_CANCELED){
							m_progressSessionManagerCompPtr->CancelProgressSession(sessionId, tr("Collection import cancelled"));
						}
						else{
							if (category == istd::IInformationProvider::IC_ERROR){
								m_progressSessionManagerCompPtr->CancelProgressSession(sessionId, tr("An error occurred during collection import"), true);
							}
							else{
								m_progressSessionManagerCompPtr->EndProgressSession(sessionId);
							}
						}

						m_sessions.remove(sessionId);

						break;
					}
				}
			}
		}
	}
}


int CCollectionImportControllerComp::FindCollectionIndex(const QByteArray& collectionId) const
{
	int count = qMin(m_collectionIdAttrPtr.GetCount(), m_jobTypeIdAttrPtr.GetCount());
	count = qMin(count, m_jobParamsFactPtr.GetCount());
	count = qMin(count, m_paramsFillerCompPtr.GetCount());

	for (int i = 0; i < count; i++){
		if (m_collectionIdAttrPtr[i] == collectionId){
			return i;
		}
	}

	return -1;
}


QByteArray CCollectionImportControllerComp::FindSession(const QByteArray& fileId) const
{
	QMutexLocker locker(&m_mutex);

	for (const QByteArray& sessionId : m_sessions.keys()){
		if (m_sessions[sessionId]->files.contains(fileId)){
			return sessionId;
		}
	}

	return QByteArray();
}


CCollectionImportControllerComp::WorkingFileInfo* CCollectionImportControllerComp::FindFileInfo(const QByteArray& fileId)
{
	QMutexLocker locker(&m_mutex);

	for (const SessionInfoPtr& sessionInfoPtr : m_sessions){
		if (sessionInfoPtr->files.contains(fileId)){
			return &sessionInfoPtr->files[fileId];
		}
	}

	return nullptr;
}


CCollectionImportControllerComp::FilelUploadStatus CCollectionImportControllerComp::AddFileChunk(const QByteArray& fileId, const istd::CIntRange& range)
{
	QMutexLocker locker(&m_mutex);

	WorkingFileInfo* fileInfoPtr = FindFileInfo(fileId);
	Q_ASSERT(fileInfoPtr != nullptr);

	// Find intersections with uploaded chunks
	QVector<int> intersectsWith;
	for (int i = 0; i < fileInfoPtr->uploadedChunks.size(); i++){
		if (fileInfoPtr->uploadedChunks[i].IsIntersectedBy(range)){
			intersectsWith.push_back(i);
		}
	}

	// Unite a chunk with intersecting chunks
	istd::CIntRange result = range;
	for (int index : intersectsWith){
		result.Unite(fileInfoPtr->uploadedChunks[index]);
	}

	for (qsizetype i = intersectsWith.size() - 1; i >= 0; i--){
		fileInfoPtr->uploadedChunks.remove(i);
	}

	fileInfoPtr->uploadedChunks.push_back(result);

	return FUS_OK_PARTIAL;
}


double CCollectionImportControllerComp::GetFileProgress(const QByteArray& fileId)
{
	QMutexLocker locker(&m_mutex);

	WorkingFileInfo* fileInfoPtr = FindFileInfo(fileId);
	Q_ASSERT(fileInfoPtr != nullptr);

	int size = fileInfoPtr->size;
	int uploaded = 0;

	Q_ASSERT(size != 0);

	for (const istd::CIntRange& chunk : fileInfoPtr->uploadedChunks){
		uploaded += chunk.GetLength();
	}

	return (double)uploaded / size;
}


void CCollectionImportControllerComp::UpdateUploadProgress(const QByteArray& fileId)
{
	QMutexLocker locker(&m_mutex);

	WorkingFileInfo* fileInfoPtr = FindFileInfo(fileId);
	Q_ASSERT(fileInfoPtr != nullptr);

	ibase::IProgressLogger* progressLoggerPtr = fileInfoPtr->uploadProgressLoggerPtr.get();
	if (progressLoggerPtr != nullptr){
		double progress = GetFileProgress(fileId);

		progressLoggerPtr->OnProgress(progress);
	}
}


// public methods of the embedded class ProgressLogger

void CCollectionImportControllerComp::ProgressLogger::SetParent(CCollectionImportControllerComp& parent)
{
	parentPtr = &parent;
}


// reimplamanted (ibase::IProgressLogger)

void CCollectionImportControllerComp::ProgressLogger::OnProgress(double currentProgress)
{
	Q_ASSERT(parentPtr != nullptr);
	Q_ASSERT(progressLoggerPtr != nullptr);

	progressLoggerPtr->OnProgress(currentProgress);
	m_progress = currentProgress;

	parentPtr->UploadProgressChanged(sessionId, fileId);
}


bool CCollectionImportControllerComp::ProgressLogger::IsCanceled() const
{
	Q_ASSERT(progressLoggerPtr != nullptr);

	return progressLoggerPtr->IsCanceled();
}


} // namespace imtservergql


