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

bool CCollectionImportControllerComp::BeginCollectionImportSession(const ICollectionImportController::SessionInfo& sessionInfo, QString& errorMessage)
{
	std::shared_ptr<SessionInfo> sessionPtr(new SessionInfo);
	sessionPtr->sessionId = sessionInfo.sessionId;
	sessionPtr->collectionId = sessionInfo.collectionId;

	sessionPtr->tempFileSessionId = m_fileManagerCompPtr->BeginSession(QString(), sessionPtr->sessionId);

	istd::TDelPtr<iprm::IParamsManager> paramsPtr;
	paramsPtr.SetCastedOrRemove(m_jobParamsFactPtr.CreateInstance());

	for (const ICollectionImportController::FileInfo& fileInfo : sessionInfo.files){
		FileInfo file;

		static_cast<ICollectionImportController::FileInfo>(file) = fileInfo;

		file.id = fileInfo.id;
		file.name = fileInfo.name;
		file.objectTypeId = fileInfo.objectTypeId;
		file.size = fileInfo.size;
		file.uploadProgressLoggerPtr.reset(new ProgressLogger);
		file.uploadProgressLoggerPtr->SetParent(*this);
		file.uploadProgressLoggerPtr->sessionId = sessionPtr->sessionId;
		file.uploadProgressLoggerPtr->fileId = fileInfo.id;
		file.tempFileId = m_fileManagerCompPtr->AddFileItem(sessionPtr->tempFileSessionId, fileInfo.name, fileInfo.id);

		sessionPtr->files[file.id] = file;

		if (!PrepareProgressManager(*sessionPtr)){
			errorMessage = tr("Unable to initialize progress manager");

			return false;
		}

		if (!file.tempFileId.isEmpty()){
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
	bool retVal = true;
	bool isSessionActive = false;
	
	{
		QMutexLocker locker(&m_mutex);

		isSessionActive = m_sessions.contains(sessionId);
	}

	if (isSessionActive){
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
		errorMessage = tr("Session not found");

		return true;
	}

	return retVal;
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

bool CCollectionImportControllerComp::PrepareProgressManager(SessionInfo& session)
{
	session.mainProgressManagerPtr = m_progressSessionManagerCompPtr->BeginProgressSession(session.sessionId, tr("Import files to the collection"));
	session.uploadProgressManagerPtr = session.mainProgressManagerPtr->CreateSubtaskManager("uploading", tr("Uploading files to the server"), 0.2);
	session.fileProcessingProgressManagerPtr = session.mainProgressManagerPtr->CreateSubtaskManager("processing", tr("Processing files on the server"), 0.8);
	session.fileProcessingProgressLoggerPtr = session.fileProcessingProgressManagerPtr->StartProgressLogger();

	for (FileInfo& fileInfo: session.files){
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


void CCollectionImportControllerComp::UploadProgressChanged(QByteArray sessionId, QByteArray fileId)
{
	QMutexLocker locker(&m_mutex);
	if (m_sessions.contains(sessionId)){
		bool uploadingFinished = true;

		for (const FileInfo& fileInfo : m_sessions[sessionId]->files){
			if (m_sessions[sessionId]->files[fileId].uploadProgressLoggerPtr->m_progress < 1.0){
				uploadingFinished = false;

				break;
			}
		}

		if (uploadingFinished){
			StartImportJob(*m_sessions[sessionId]);
		}
	}
}


bool CCollectionImportControllerComp::StartImportJob(SessionInfo& session)
{
	imtbase::CSimpleReferenceCollection refCollection;

	istd::TDelPtr<iprm::IParamsSet> jobParamsPtr;
	jobParamsPtr.SetCastedOrRemove(m_jobParamsFactPtr.CreateInstance());
	if (!jobParamsPtr.IsValid()){
		return false;
	}

	iprm::TEditableParamsPtr<iprm::IParamsManager> fileParamsListPtr(jobParamsPtr.GetPtr(), "FileParamsList");
	Q_ASSERT(fileParamsListPtr.IsValid());

	QByteArrayList ids = session.files.keys();
	for (const QByteArray& id: ids){
		int index = fileParamsListPtr->InsertParamsSet();
		if (index < 0){
			return false;
		}

		iprm::IParamsSet* fileParamsPtr = fileParamsListPtr->GetParamsSet(index);
		Q_ASSERT(fileParamsPtr != nullptr);

		iprm::TEditableParamsPtr<ifile::IFileNameParam> filePathPtr(fileParamsPtr, "FilePath");
		iprm::TEditableParamsPtr<iprm::IIdParam> objectTypeIdPtr(fileParamsPtr, "ObjectTypeId");
		iprm::TEditableParamsPtr<iprm::IIdParam> collectionIdPtr(fileParamsPtr, "CollectionId");
		iprm::TEditableParamsPtr<iprm::IIdParam> proposedIdPtr(fileParamsPtr, "ProposedId");
		Q_ASSERT(
			filePathPtr.IsValid() &&
			objectTypeIdPtr.IsValid() &&
			collectionIdPtr.IsValid() &&
			proposedIdPtr.IsValid());

		filePathPtr->SetPath(m_fileManagerCompPtr->GetPath(session.sessionId, id));
		objectTypeIdPtr->SetId(session.files[id].objectTypeId);
		collectionIdPtr->SetId(session.collectionId);
	}

	session.jobId = m_jobQueueManagerCompPtr->InsertNewJobIntoQueue(
		session.sessionId,
		"CharacterizationImport",
		refCollection,
		jobParamsPtr.GetPtr());

	return !session.jobId.isEmpty();
}


void CCollectionImportControllerComp::OnJobQueueChanged(const istd::IChangeable::ChangeSet& changeset, const imthype::IJobQueueManager* modelPtr)
{
	if (changeset.GetChangeInfoMap().contains(imthype::IJobQueueManager::CN_JOB_PROGRESS_CHANGED)){
		QVariant value = changeset.GetChangeInfoMap().value(imthype::IJobQueueManager::CN_JOB_PROGRESS_CHANGED);
		imthype::IJobQueueManager::JobProgressInfo info = value.value<imthype::IJobQueueManager::JobProgressInfo>();

		for (std::shared_ptr<SessionInfo>& session : m_sessions){
			if (session->jobId == info.elementId){
				session->fileProcessingProgressLoggerPtr->OnProgress(info.progress);
			}
		}
	}

	if (changeset.GetChangeInfoMap().contains(imthype::IJobQueueManager::CN_JOB_STATUS_CHANGED)){
		if (changeset.GetChangeInfoMap().contains(imtbase::ICollectionInfo::CN_ELEMENT_REMOVED)){
			return;
		}

		QVariant value = changeset.GetChangeInfoMap().value(imthype::IJobQueueManager::CN_JOB_STATUS_CHANGED);
		imthype::IJobQueueManager::JobStatusInfo info = value.value<imthype::IJobQueueManager::JobStatusInfo>();

		if (info.status == imthype::IJobQueueManager::PS_FINISHED || info.status == imthype::IJobQueueManager::PS_CANCELED){
			if (!m_sessions.isEmpty()){
				for (std::shared_ptr<SessionInfo>& session : m_sessions){
					if (session->jobId == info.elementId){
						QByteArray sessionId = session->sessionId;

						imthype::CStandardJobOutput jobOutput;
						m_jobQueueManagerCompPtr->GetJobResult(session->jobId, jobOutput);
						istd::IInformationProvider::InformationCategory category = jobOutput.GetInformationCategory();

						m_jobQueueManagerCompPtr->RemoveJob(session->jobId);
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

						QMutexLocker locker(&m_mutex);

						m_sessions.remove(sessionId);

						break;
					}
				}
			}
		}
	}
}


QByteArray CCollectionImportControllerComp::FindSession(const QByteArray& fileId) const
{
	QMutexLocker locker(&m_mutex);

	for (const QByteArray& sessionId : m_sessions.keys()){
		if (m_sessions[sessionId]->files.contains(fileId)){
			return sessionId;
		}
	}

	QByteArray();
}


CCollectionImportControllerComp::FileInfo* CCollectionImportControllerComp::FindFileInfo(const QByteArray& fileId)
{
	QMutexLocker locker(&m_mutex);

	for (const SessionInfoPtr& sessionInfoPtr : m_sessions){
		if (sessionInfoPtr->files.contains(fileId)){
			return &sessionInfoPtr->files[fileId];
		}
	}

	nullptr;
}


CCollectionImportControllerComp::FilelUploadStatus CCollectionImportControllerComp::AddFileChunk(const QByteArray& fileId, const istd::CIntRange& range)
{
	FileInfo* fileInfoPtr = FindFileInfo(fileId);
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

	for (int i = intersectsWith.size() - 1; i >= 0; i--){
		fileInfoPtr->uploadedChunks.remove(i);
	}

	fileInfoPtr->uploadedChunks.push_back(result);

	return FUS_OK_PARTIAL;
}


double CCollectionImportControllerComp::GetFileProgress(const QByteArray& fileId)
{
	FileInfo* fileInfoPtr = FindFileInfo(fileId);
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
	FileInfo* fileInfoPtr = FindFileInfo(fileId);
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


