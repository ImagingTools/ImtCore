#pragma once


// ACF includes
#include <icomp/CComponentBase.h>
#include <imod/TModelWrap.h>
#include <ifile/ITempFileManager.h>
#include <iprm/IParamsSet.h>
#include <imod/TSingleModelObserverBase.h>

// ImtCore includes
#include <imtbase/IProgressSessionsManager.h>
#include <imtservergql/ICollectionImportController.h>
#include <imtservergql/IFileUploadHandler.h>
#include <imthype/IJobQueueManager.h>
#include <imtbase/TModelUpdateBinder.h>


namespace imtservergql
{


class CCollectionImportControllerComp:
	public icomp::CComponentBase,
	virtual public ICollectionImportController,
	virtual public IFileUploadHandler
{
	Q_DECLARE_TR_FUNCTIONS(CCollectionImportControllerComp)
public:
	typedef CComponentBase BaseClass;

	I_BEGIN_COMPONENT(CCollectionImportControllerComp);
		I_REGISTER_INTERFACE(ICollectionImportController)
		I_REGISTER_INTERFACE(IFileUploadHandler)
		I_ASSIGN(m_fileManagerCompPtr, "TempFileManager", "Temporary file manager for uploaded files", true, "TempFileManager");
		I_ASSIGN(m_progressSessionManagerCompPtr, "ProgressSessionManager", "ProgressSessionManager", true, "ProgressSessionManager");
		I_ASSIGN(m_jobQueueManagerCompPtr, "JobQueueManager", "Job queue manager", true, "JobQueueManager");
		I_ASSIGN(m_jobParamsFactPtr, "JobParamsFactory", "Job params", true, "JobParams");
	I_END_COMPONENT;

	CCollectionImportControllerComp();

	// reimplemented (imtservergql::ICollectionImportController)
	virtual bool BeginCollectionImportSession(const SessionInfo& sessionInfo, QString& errorMessage) override;
	virtual bool CancelCollectionImportSession(const QByteArray& sessionId, QString& errorMessage) override;

	// reimplemented (imtservergql::IFileUploadHandler)
	virtual FilelUploadStatus ReceiveFileChunk(
		const QByteArray& fileId,
		const istd::CIntRange& range,
		const QByteArray& data) override;

protected:
	// reimplemented (icomp::CComponentBase)
	void OnComponentCreated() override;
	void OnComponentDestroyed() override;

private:
	class ProgressLogger: public ibase::IProgressLogger
	{
	public:
		void SetParent(CCollectionImportControllerComp& parent);

		// reimplamanted (ibase::IProgressLogger)
		virtual void OnProgress(double currentProgress) override;
		virtual bool IsCanceled() const override;

	public:
		QByteArray sessionId;
		QByteArray fileId;
		std::unique_ptr<ibase::IProgressManager> progressManagerPtr;
		std::unique_ptr<ibase::IProgressLogger> progressLoggerPtr;
		double m_progress = 0;

		CCollectionImportControllerComp* parentPtr = nullptr;
	};
	typedef std::shared_ptr<ProgressLogger> ProgressLoggerPtr;

	struct FileInfo: public ICollectionImportController::FileInfo
	{
		QByteArray tempFileId;
		ProgressLoggerPtr uploadProgressLoggerPtr;
		QVector<istd::CIntRange> uploadedChunks;
	};

	struct SessionInfo
	{
		QByteArray sessionId;
		QByteArray collectionId;
		QMap<QByteArray, FileInfo> files;
		QByteArray jobId;

		QByteArray tempFileSessionId;
		ibase::IProgressManager* mainProgressManagerPtr = nullptr;
		std::unique_ptr<ibase::IProgressManager> uploadProgressManagerPtr;
		std::unique_ptr<ibase::IProgressManager> fileProcessingProgressManagerPtr;
		std::unique_ptr<ibase::IProgressLogger> fileProcessingProgressLoggerPtr;
	};

	typedef std::shared_ptr<SessionInfo> SessionInfoPtr;

private:
	bool PrepareProgressManager(SessionInfo& session);
	void UploadProgressChanged(QByteArray sessionId, QByteArray fileId);
	bool StartImportJob(SessionInfo& session);
	void OnJobQueueChanged(const istd::IChangeable::ChangeSet& changeset, const imthype::IJobQueueManager* modelPtr);

	QByteArray FindSession(const QByteArray& fileId) const;
	FileInfo* FindFileInfo(const QByteArray& fileId);
	FilelUploadStatus AddFileChunk(const QByteArray& fileId, const istd::CIntRange& range);
	double GetFileProgress(const QByteArray& fileId);
	void UpdateUploadProgress(const QByteArray& fileId);

private:
	I_REF(ifile::ITempFileManager, m_fileManagerCompPtr);
	I_REF(imtbase::IProgressSessionsManager, m_progressSessionManagerCompPtr);
	I_REF(imthype::IJobQueueManager, m_jobQueueManagerCompPtr);
	I_FACT(iprm::IParamsSet, m_jobParamsFactPtr);

	QMap<QByteArray, SessionInfoPtr> m_sessions;

	mutable QRecursiveMutex m_mutex;

	imtbase::TModelUpdateBinder<imthype::IJobQueueManager, CCollectionImportControllerComp> m_jobQueueObserver;
};


} // namespace imtservergql


