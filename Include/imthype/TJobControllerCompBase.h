#pragma once


// ACF includes
#include <icomp/CComponentBase.h>
#include <iprm/IParamsSet.h>

// ImtCore includes
#include <imtbase/IProgressSessionsManager.h>
#include <imtbase/TModelUpdateBinder.h>
#include <imthype/IJobQueueManager.h>
#include <imthype/TIJobController.h>


namespace imthype
{


class CJobControllerBase: public QObject
{
	Q_OBJECT

Q_SIGNALS:
	void EmitBeginJob(const QByteArray& jobId);

protected Q_SLOTS:
	virtual void OnBeginJob(const QByteArray& jobId) {};
};


template<typename JobParams, typename JobResult, typename JobInfoExtension>
class TJobControllerCompBase: public CJobControllerBase, public icomp::CComponentBase, virtual public TIJobController<JobParams, JobResult>
{
public:
	typedef CComponentBase BaseClass;

	using IJobController = TIJobController<JobParams, JobResult>;
	using JobResultPtr = std::shared_ptr<JobResult>;
	using JobParamsSetPtr = std::shared_ptr<iprm::IParamsSet>;

	I_BEGIN_BASE_COMPONENT(TJobControllerCompBase)
		I_REGISTER_INTERFACE(IJobController)
		I_ASSIGN(m_defaultJobTypeIdAttrPtr, "DefaultJobTypeId", "Default job type ID", true, "");
		I_ASSIGN(m_progressSessionManagerCompPtr, "ProgressSessionManager", "ProgressSessionManager", true, "ProgressSessionManager");
		I_ASSIGN(m_jobQueueManagerCompPtr, "JobQueueManager", "Job queue manager", true, "JobQueueManager");
		I_ASSIGN(m_jobParamsFactPtr, "JobParamsFactory", "Job params", true, "JobParams");
	I_END_COMPONENT;

	TJobControllerCompBase();

	// reimplemented (imtservergql::ICollectionImportController)
	virtual QByteArray BeginJob(const JobParams& jobParams) override;
	virtual JobResultPtr GetJobResult(const QByteArray& jobId) const override;
	virtual IJobController::JobStatus GetJobStatus(const QByteArray& jobId) const override;
	virtual IJobController::RequestStatus CancelJob(const QByteArray& jobId) override;
	virtual IJobController::RequestStatus RemoveJob(const QByteArray& jobId) override;

protected:
	struct JobInfo
	{
		JobParams params;
		JobResult result;
		typename IJobController::JobStatus status = IJobController::JS_FAILED;

		QByteArray queueJobId;

		JobInfoExtension extension;

		ibase::IProgressManager* jobProgressManagerPtr = nullptr;
		std::unique_ptr<ibase::IProgressLogger> progressLoggerPtr;
	};
	typedef std::shared_ptr<JobInfo> JobInfoPtr;

protected:
	virtual void OnBeginJob(const QByteArray& jobId) override;
	virtual bool ConfigureJobParamsSet(const JobParams& params, iprm::IParamsSet& paramsSet) const = 0;

	// reimplemented (icomp::CComponentBase)
	void OnComponentCreated() override;
	void OnComponentDestroyed() override;

protected:
	QMap<QByteArray, JobInfoPtr> m_jobs;

	mutable QRecursiveMutex m_mutex;

	I_ATTR(QByteArray, m_defaultJobTypeIdAttrPtr);
	I_REF(imtbase::IProgressSessionsManager, m_progressSessionManagerCompPtr);
	I_REF(imthype::IJobQueueManager, m_jobQueueManagerCompPtr);
	I_FACT(iprm::IParamsSet, m_jobParamsFactPtr);

	imtbase::TModelUpdateBinder<imthype::IJobQueueManager, TJobControllerCompBase> m_jobQueueObserver;

private:
	virtual void OnJobQueueChanged(
		const istd::IChangeable::ChangeSet& changeset, const imthype::IJobQueueManager* modelPtr);
};


// public methods

template<typename JobParams, typename JobResult, typename JobInfoExtension>
TJobControllerCompBase<JobParams, JobResult, JobInfoExtension>::TJobControllerCompBase()
	:m_jobQueueObserver(*this)
{
	connect(this, &TJobControllerCompBase::EmitBeginJob, this, &TJobControllerCompBase::OnBeginJob, Qt::QueuedConnection);
}


// reimplemented (imtservergql::ICollectionImportController)

template<typename JobParams, typename JobResult, typename JobInfoExtension>
QByteArray TJobControllerCompBase<JobParams, JobResult, JobInfoExtension>::BeginJob(const JobParams& jobParams)
{
	if (m_jobQueueManagerCompPtr.IsValid() && m_jobParamsFactPtr.IsValid()){
		QByteArray jobId = QUuid::createUuid().toByteArray(QUuid::WithoutBraces);

		JobInfoPtr jobPtr(new JobInfo);
		jobPtr->params = jobParams;
		jobPtr->status = IJobController::JS_IN_PROGRESS;

		if (m_progressSessionManagerCompPtr.IsValid()){
			jobPtr->jobProgressManagerPtr = m_progressSessionManagerCompPtr->BeginProgressSession(jobId, "Job processing progress");
		}

		QMutexLocker locker(&m_mutex);

		m_jobs[jobId] = jobPtr;

		EmitBeginJob(jobId);

		return jobId;
	}

	return QByteArray();
}


template<typename JobParams, typename JobResult, typename JobInfoExtension>
std::shared_ptr<JobResult> TJobControllerCompBase<JobParams, JobResult, JobInfoExtension>::GetJobResult(const QByteArray& jobId) const
{
	return std::shared_ptr<JobResult>();
}


template<typename JobParams, typename JobResult, typename JobInfoExtension>
typename TIJobController<JobParams, JobResult>::JobStatus TJobControllerCompBase<JobParams, JobResult, JobInfoExtension>::GetJobStatus(const QByteArray& jobId) const
{
	return IJobController::JS_FAILED;
}


template<typename JobParams, typename JobResult, typename JobInfoExtension>
typename TIJobController<JobParams, JobResult>::RequestStatus TJobControllerCompBase<JobParams, JobResult, JobInfoExtension>::CancelJob(const QByteArray& jobId)
{
	return IJobController::RS_FAILED;
}


template<typename JobParams, typename JobResult, typename JobInfoExtension>
typename TIJobController<JobParams, JobResult>::RequestStatus TJobControllerCompBase<JobParams, JobResult, JobInfoExtension>::RemoveJob(const QByteArray& jobId)
{
	return IJobController::RS_FAILED;
}


// protected methods

template<typename JobParams, typename JobResult, typename JobInfoExtension>
void TJobControllerCompBase<JobParams, JobResult, JobInfoExtension>::OnBeginJob(const QByteArray& jobId)
{
	JobParamsSetPtr jobParamsSetPtr(m_jobParamsFactPtr.CreateInstance());
	Q_ASSERT(jobParamsSetPtr != nullptr);

	QMutexLocker locker(&m_mutex);

	bool retVal = false;

	//if (m_jobs.contains(jobId)){
	//	if (ConfigureJobParamsSet(m_jobs[jobId]), *jobParamsSetPtr)){
	//		retVal = true;
	//	}
	//}

	//if (retVal && m_defaultJobTypeIdAttrPtr->isEmpty()){
	//	m_jobs[jobId]->queueJobId = m_jobQueueManagerCompPtr->InsertNewJobIntoQueue(
	//		jobId,
	//		"CharacterizationImport",
	//		refCollection,
	//		jobParamsPtr.GetPtr());
	//}
	//else{
	//	m_jobs[jobId]->status = JS_FAILED;
	//	if (m_progressSessionManagerCompPtr.IsValid()){
	//		m_progressSessionManagerCompPtr->CancelProgressSession(jobId, "Unable to start job", true);
	//	}
	//}
}


// reimplemented (icomp::CComponentBase)

template<typename JobParams, typename JobResult, typename JobInfoExtension>
void TJobControllerCompBase<JobParams, JobResult, JobInfoExtension>::OnComponentCreated()
{
	BaseClass::OnComponentCreated();

	if (m_jobQueueManagerCompPtr.IsValid()){
		//m_jobQueueObserver.RegisterObject(m_jobQueueManagerCompPtr.GetPtr, &TJobControllerCompBase::OnJobQueueChanged);
	}
}


template<typename JobParams, typename JobResult, typename JobInfoExtension>
void TJobControllerCompBase<JobParams, JobResult, JobInfoExtension>::OnComponentDestroyed()
{
	m_jobQueueObserver.UnregisterAllObjects();

	BaseClass::OnComponentDestroyed();
}


// private methods

template<typename JobParams, typename JobResult, typename JobInfoExtension>
void TJobControllerCompBase<JobParams, JobResult, JobInfoExtension>::OnJobQueueChanged(const istd::IChangeable::ChangeSet& changeset, const imthype::IJobQueueManager* modelPtr)
{
	if (changeset.GetChangeInfoMap().contains(imthype::IJobQueueManager::CN_JOB_PROGRESS_CHANGED)){
		QVariant value = changeset.GetChangeInfoMap().value(imthype::IJobQueueManager::CN_JOB_PROGRESS_CHANGED);
		imthype::IJobQueueManager::JobProgressInfo info = value.value<imthype::IJobQueueManager::JobProgressInfo>();

		QMutexLocker locker(&m_mutex);

		//for (const QByteArray& jobId : m_jobs.keys()){
		//	if (jobId == info.elementId){
		//		JobInfoPtr& jobInfoPtr = m_jobs[jobId];
		//		Q_ASSERT(jobInfoPtr != nullptr);

		//		jobInfoPtr->fileProcessingProgressLoggerPtr->OnProgress(info.progress);
		//	}
		//}
	}

	if (changeset.GetChangeInfoMap().contains(imthype::IJobQueueManager::CN_JOB_STATUS_CHANGED)){
		if (changeset.GetChangeInfoMap().contains(imtbase::ICollectionInfo::CN_ELEMENT_REMOVED)){
			return;
		}

		//QVariant value = changeset.GetChangeInfoMap().value(imthype::IJobQueueManager::CN_JOB_STATUS_CHANGED);
		//imthype::IJobQueueManager::JobStatusInfo info = value.value<imthype::IJobQueueManager::JobStatusInfo>();

		//if (info.status == imthype::IJobQueueManager::PS_FINISHED || info.status == imthype::IJobQueueManager::PS_CANCELED){
		//	QMutexLocker locker(&m_mutex);

		//	if (!m_sessions.isEmpty()){
		//		for (std::shared_ptr<JobInfo>& job : m_sessions){
		//			if (job->jobId == info.elementId){
		//				QByteArray sessionId = job->sessionId;

		//				imthype::CStandardJobOutput jobOutput;
		//				m_jobQueueManagerCompPtr->GetJobResult(job->jobId, jobOutput);
		//				istd::IInformationProvider::InformationCategory category = jobOutput.GetInformationCategory();

		//				m_jobQueueManagerCompPtr->RemoveJob(job->jobId);
		//				m_fileManagerCompPtr->FinishSession(sessionId);

		//				if (info.status == imthype::IJobQueueManager::PS_CANCELED){
		//					m_progressSessionManagerCompPtr->CancelProgressSession(sessionId, tr("Collection import cancelled"));
		//				}
		//				else{
		//					if (category == istd::IInformationProvider::IC_ERROR){
		//						m_progressSessionManagerCompPtr->CancelProgressSession(sessionId, tr("An error occurred during collection import"), true);
		//					}
		//					else{
		//						m_progressSessionManagerCompPtr->EndProgressSession(sessionId);
		//					}
		//				}

		//				QMutexLocker locker(&m_mutex);

		//				m_sessions.remove(sessionId);

		//				break;
		//			}
		//		}
		//	}
		//}
	}
}


} // namespace imtservergql
