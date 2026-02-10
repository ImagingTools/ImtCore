// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
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
#include <imtservergql/ICollectionImportJobParamsFiller.h>
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
		I_ASSIGN_MULTI_0(m_collectionIdAttrPtr, "CollectionId", "Collection ID associated with import job type ID", false);
		I_ASSIGN_MULTI_0(m_jobTypeIdAttrPtr, "ImportJobTypeId", "Import job type ID associated with collection ID", false);
		I_ASSIGN_MULTI_0(m_paramsFillerCompPtr, "CollectionImportJobParamsFiller", "Collection import job params filler associated with collection ID", false);
		I_ASSIGN_MULTI_0(m_jobParamsFactPtr, "JobParamsFactory", "Job params factory associated with collection ID", false);
		I_ASSIGN(m_fileManagerCompPtr, "TempFileManager", "Temporary file manager for uploaded files", true, "TempFileManager");
		I_ASSIGN(m_progressSessionManagerCompPtr, "ProgressSessionManager", "ProgressSessionManager", true, "ProgressSessionManager");
		I_ASSIGN(m_jobQueueManagerCompPtr, "JobQueueManager", "Job queue manager", true, "JobQueueManager");
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

	struct WorkingFileInfo: public FileInfo
	{
		QByteArray tempFileId;
		ProgressLoggerPtr uploadProgressLoggerPtr;
		QList<istd::CIntRange> uploadedChunks;
	};

	struct WorkingSessionInfo: public SessionInfo
	{
		QMap<QByteArray, WorkingFileInfo> files;
		QByteArray jobId;

		QByteArray tempFileSessionId;
		ibase::IProgressManager* mainProgressManagerPtr = nullptr;
		std::unique_ptr<ibase::IProgressManager> uploadProgressManagerPtr;
		std::unique_ptr<ibase::IProgressManager> fileProcessingProgressManagerPtr;
		std::unique_ptr<ibase::IProgressLogger> fileProcessingProgressLoggerPtr;
	};

	typedef std::shared_ptr<WorkingSessionInfo> SessionInfoPtr;

private:
	bool PrepareProgressManager(WorkingSessionInfo& session) const;
	void UploadProgressChanged(QByteArray sessionId, QByteArray fileId);
	bool StartImportJob(WorkingSessionInfo& session);
	void OnJobQueueChanged(const istd::IChangeable::ChangeSet& changeset, const imthype::IJobQueueManager* modelPtr);
	int FindCollectionIndex(const QByteArray& collectionId) const;

	QByteArray FindSession(const QByteArray& fileId) const;
	WorkingFileInfo* FindFileInfo(const QByteArray& fileId);
	FilelUploadStatus AddFileChunk(const QByteArray& fileId, const istd::CIntRange& range);
	double GetFileProgress(const QByteArray& fileId);
	void UpdateUploadProgress(const QByteArray& fileId);

private:
	I_MULTIATTR(QByteArray, m_collectionIdAttrPtr);
	I_MULTIATTR(QByteArray, m_jobTypeIdAttrPtr);
	I_MULTIREF(imtservergql::ICollectionImportJobParamsFiller, m_paramsFillerCompPtr);
	I_MULTIFACT(iprm::IParamsSet, m_jobParamsFactPtr);
	I_REF(ifile::ITempFileManager, m_fileManagerCompPtr);
	I_REF(imtbase::IProgressSessionsManager, m_progressSessionManagerCompPtr);
	I_REF(imthype::IJobQueueManager, m_jobQueueManagerCompPtr);

	QMap<QByteArray, SessionInfoPtr> m_sessions;

	mutable QRecursiveMutex m_mutex;

	imtbase::TModelUpdateBinder<imthype::IJobQueueManager, CCollectionImportControllerComp> m_jobQueueObserver;
};


} // namespace imtservergql


