#include <imtcol/CCollectionImportControllerComp.h>


// Qt includes
#include <QtCore/QMutexLocker>

// ACF includes
#include <istd/TDelPtr.h>

// ImtCore includes
#include <imtbase/CSimpleReferenceCollection.h>


namespace imtcol
{


// public methods

CCollectionImportControllerComp::CCollectionImportControllerComp()
{
	m_progressLoggerProvider.SetParent(*this);
}


// reimplemented (imtbase::IProgressLoggerProvider)

bool CCollectionImportControllerComp::BeginCollectionImportTransaction(const ICollectionImportController::TransactionInfo& transactionInfo, QString& errorMessage)
{
	std::shared_ptr<TransactionInfo> transactionPtr(new TransactionInfo);
	transactionPtr->transactionId = transactionInfo.transactionId;
	transactionPtr->collectionId = transactionInfo.collectionId;

	transactionPtr->tempFileSessionId = m_tempFileManagerCompPtr->BeginSession(QString(), transactionPtr->transactionId);

	istd::TDelPtr<iprm::IParamsManager> paramsPtr;
	paramsPtr.SetCastedOrRemove(m_jobParamsFactPtr.CreateInstance());

	for (const ICollectionImportController::FileInfo& fileInfo : transactionInfo.files){
		FileInfo file;

		static_cast<ICollectionImportController::FileInfo>(file) = fileInfo;

		file.id = fileInfo.id;
		file.name = fileInfo.name;
		file.objectTypeId = fileInfo.objectTypeId;
		file.size = fileInfo.size;
		file.uploadProgressLoggerPtr.reset(new ProgressLogger);
		file.uploadProgressLoggerPtr->SetParent(*this);
		file.uploadProgressLoggerPtr->transactionId = transactionPtr->transactionId;
		file.uploadProgressLoggerPtr->fileId = fileInfo.id;
		file.tempFileId = m_tempFileManagerCompPtr->AddFileItem(transactionPtr->tempFileSessionId, fileInfo.name, fileInfo.id);

		transactionPtr->files[file.id] = file;

		if (!PrepareProgressManager(*transactionPtr)){
			errorMessage = tr("Unable to initialize progress manager");

			return false;
		}

		if (!file.tempFileId.isEmpty()){
			QMutexLocker locker(&m_mutex);

			m_transactions[transactionPtr->transactionId] = transactionPtr;
		}
		else {
			errorMessage = tr("Unable to create temporary fileInfo for upload");

			return false;
		}
	}

	//imtbase::CSimpleReferenceCollection refCollection;

	//if (m_jobQueueManagerCompPtr->InsertNewJobIntoQueue(transactionInfo.transactionId, "CharacterizationImport", refCollection, ).isEmpty()){

	//}

	return true;
}


bool CCollectionImportControllerComp::CancelCollectionImportTransaction(const QByteArray& transactionId, QString& errorMessage)
{
	bool isTransactionActive = false;
	
	{
		QMutexLocker locker(&m_mutex);

		isTransactionActive = m_transactions.contains(transactionId);
	}

	if (isTransactionActive){
		m_jobQueueManagerCompPtr->RemoveJob(transactionId);
		m_progressSessionManagerCompPtr->EndProgressSession(transactionId);
		m_tempFileManagerCompPtr->FinishSession(transactionId);

		QMutexLocker locker(&m_mutex);

		m_transactions.remove(transactionId);
	}
	else{
		errorMessage = tr("Transaction not found");

		return true;
	}

	return true;
}


// protected methods

// reimplemented (icomp::CComponentBase)

void CCollectionImportControllerComp::OnComponentCreated()
{
	BaseClass::OnComponentCreated();

	Q_ASSERT(
		m_tempFileManagerCompPtr.IsValid() &&
		m_progressSessionManagerCompPtr.IsValid() &&
		m_jobQueueManagerCompPtr.IsValid() &&
		m_jobParamsFactPtr.IsValid());

	if (m_tempFileManagerCompPtr.IsValid()){
		m_tempFileManagerCompPtr->ResetAllSessions();
	}
}


// private methods

bool CCollectionImportControllerComp::PrepareProgressManager(TransactionInfo& transaction)
{
	transaction.mainProgressManagerPtr = m_progressSessionManagerCompPtr->BeginProgressSession(transaction.transactionId, tr("Import files to the collection"));
	transaction.uploadProgressManagerPtr = transaction.mainProgressManagerPtr->CreateSubtaskManager("uploading", tr("Uploading files to the server"), 0.2);
	transaction.fileProcessingProgressManagerPtr = transaction.mainProgressManagerPtr->CreateSubtaskManager("processing", tr("Processing files on the server"), 0.4);
	transaction.insertionProgressManagerPtr = transaction.mainProgressManagerPtr->CreateSubtaskManager("insertion", tr("Inserting objects to the collection"), 0.4);

	for (FileInfo& fileInfo: transaction.files){
		fileInfo.uploadProgressLoggerPtr->progressManagerPtr =
			transaction.uploadProgressManagerPtr->CreateSubtaskManager(fileInfo.id, fileInfo.name, 1. /transaction.files.size());
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


void CCollectionImportControllerComp::UploadProgressChanged(QByteArray transactionId, QByteArray fileId)
{
	QMutexLocker locker(&m_mutex);
	if (m_transactions.contains(transactionId)){
		bool uploadingFinished = true;

		for (const FileInfo& fileInfo : m_transactions[transactionId]->files){
			if (m_transactions[transactionId]->files[fileId].uploadProgressLoggerPtr->m_progress < 1.0){
				uploadingFinished = false;

				break;
			}
		}

		if (uploadingFinished){
			StartImportJob(*m_transactions[transactionId]);
		}
	}
}


bool CCollectionImportControllerComp::StartImportJob(TransactionInfo& transaction)
{
	return false;
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

	parentPtr->UploadProgressChanged(transactionId, fileId);
}


bool CCollectionImportControllerComp::ProgressLogger::IsCanceled() const
{
	Q_ASSERT(progressLoggerPtr != nullptr);

	return progressLoggerPtr->IsCanceled();
}


// public methods of the embedded class UploadProgressLoggerProvider

void CCollectionImportControllerComp::UploadProgressLoggerProvider::SetParent(CCollectionImportControllerComp& parent)
{
	m_parentPtr = &parent;
}


// reimplamanted (imtbase::IProgressLoggerProvider)

QByteArrayList CCollectionImportControllerComp::UploadProgressLoggerProvider::GetProgressLoggerIds() const
{
	QByteArrayList retVal;

	QMutexLocker locker(&m_parentPtr->m_mutex);
	for (const QByteArray transactionId : m_parentPtr->m_transactions.keys()){
		for (const FileInfo& fileInfo : m_parentPtr->m_transactions[transactionId]->files){
			retVal.append(fileInfo.id);
		}
	}

	return QByteArrayList();
}


ibase::IProgressLogger* CCollectionImportControllerComp::UploadProgressLoggerProvider::GetProgressLogger(const QByteArray& progressLoggerId) const
{
	QMutexLocker locker(&m_parentPtr->m_mutex);
	for (const QByteArray transactionId : m_parentPtr->m_transactions.keys()){
		if (m_parentPtr->m_transactions[transactionId]->files.contains(progressLoggerId)){
			return m_parentPtr->m_transactions[transactionId]->files[progressLoggerId].uploadProgressLoggerPtr->progressLoggerPtr.get();
		}
	}

	return nullptr;
}


} // namespace imtcol


