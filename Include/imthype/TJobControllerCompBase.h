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
	void EmitNewJobAdded(const QByteArray& jobId);

protected Q_SLOTS:
	virtual void OnNewJobAdded(const QByteArray& jobId) = 0;
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
	I_END_COMPONENT;

	TJobControllerCompBase();

	// reimplemented (imthype::TIJobController)
	virtual IJobController::RequestStatus BeginJob(const QByteArray& jobId, const JobParams& jobParams) override;
	virtual JobResultPtr GetJobResult(const QByteArray& jobId) const override;
	virtual IJobController::JobStatus GetJobStatus(const QByteArray& jobId) const override;
	virtual IJobController::RequestStatus CancelJob(const QByteArray& jobId) override;
	virtual IJobController::RequestStatus RemoveJob(const QByteArray& jobId) override;

protected:
	struct JobInfo
	{
		JobParams params;
		JobResultPtr resultPtr;
		typename IJobController::JobStatus status = IJobController::JS_FAILED;

		JobInfoExtension extension;

		ibase::IProgressManager* jobProgressManagerPtr = nullptr;
		std::unique_ptr<ibase::IProgressLogger> progressLoggerPtr;
	};
	typedef std::shared_ptr<JobInfo> JobInfoPtr;

protected:
	virtual void OnNewJobAdded(const QByteArray& jobId) = 0;
	virtual void OnTaskStatusChanged(const QByteArray& taskId, imthype::IJobQueueManager::ProcessingStatus taskStatus) = 0;
	virtual void OnTaskProgressChanged(const QByteArray& taskId, double progress) = 0;

	// reimplemented (icomp::CComponentBase)
	void OnComponentCreated() override;
	void OnComponentDestroyed() override;

protected:
	QMap<QByteArray, JobInfoPtr> m_jobs;

	mutable QRecursiveMutex m_mutex;

	I_ATTR(QByteArray, m_defaultJobTypeIdAttrPtr);
	I_REF(imtbase::IProgressSessionsManager, m_progressSessionManagerCompPtr);
	I_REF(imthype::IJobQueueManager, m_jobQueueManagerCompPtr);

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
	connect(this, &TJobControllerCompBase::EmitNewJobAdded, this, &TJobControllerCompBase::OnNewJobAdded, Qt::QueuedConnection);
}


// reimplemented (imtservergql::ICollectionImportController)

template<typename JobParams, typename JobResult, typename JobInfoExtension>
typename TIJobController<JobParams, JobResult>::RequestStatus TJobControllerCompBase<JobParams, JobResult, JobInfoExtension>::BeginJob(const QByteArray& jobId, const JobParams& jobParams)
{
	if (m_jobQueueManagerCompPtr.IsValid()){
		QMutexLocker locker(&m_mutex);

		if (!m_jobs.contains(jobId)){
			JobInfoPtr jobPtr(new JobInfo);
			jobPtr->params = jobParams;
			jobPtr->status = IJobController::JS_IN_PROGRESS;

			if (m_progressSessionManagerCompPtr.IsValid()){
				jobPtr->jobProgressManagerPtr = m_progressSessionManagerCompPtr->BeginProgressSession(jobId, "Job processing progress");
			}

			m_jobs[jobId] = jobPtr;

			EmitNewJobAdded(jobId);

			return IJobController::RS_SUCCESS;
		}
	}

	return IJobController::RS_FAILED;
}


template<typename JobParams, typename JobResult, typename JobInfoExtension>
std::shared_ptr<JobResult> TJobControllerCompBase<JobParams, JobResult, JobInfoExtension>::GetJobResult(const QByteArray& jobId) const
{
	QMutexLocker locker(&m_mutex);

	if (m_jobs.contains(jobId)){
		return m_jobs[jobId]->resultPtr;
	}

	return nullptr;
}


template<typename JobParams, typename JobResult, typename JobInfoExtension>
typename TIJobController<JobParams, JobResult>::JobStatus TJobControllerCompBase<JobParams, JobResult, JobInfoExtension>::GetJobStatus(const QByteArray& jobId) const
{
	QMutexLocker locker(&m_mutex);

	if (m_jobs.contains(jobId)){
		return m_jobs[jobId]->status;
	}

	return IJobController::JS_INVALID_JOB_ID;
}


template<typename JobParams, typename JobResult, typename JobInfoExtension>
typename TIJobController<JobParams, JobResult>::RequestStatus TJobControllerCompBase<JobParams, JobResult, JobInfoExtension>::CancelJob(const QByteArray& jobId)
{
	QMutexLocker locker(&m_mutex);

	if (m_jobs.contains(jobId)){
		if (m_progressSessionManagerCompPtr.IsValid()){
			m_progressSessionManagerCompPtr->CancelProgressSession(jobId, "Job canceled");
		}

		m_jobs.remove(jobId);

		return IJobController::RS_SUCCESS;
	}

	return IJobController::RS_INVALID_JOB_ID;
}


template<typename JobParams, typename JobResult, typename JobInfoExtension>
typename TIJobController<JobParams, JobResult>::RequestStatus TJobControllerCompBase<JobParams, JobResult, JobInfoExtension>::RemoveJob(const QByteArray& jobId)
{
	QMutexLocker locker(&m_mutex);

	if (m_jobs.contains(jobId)){
		if (m_progressSessionManagerCompPtr.IsValid()){
			m_progressSessionManagerCompPtr->EndProgressSession(jobId);
		}

		m_jobs.remove(jobId);

		return IJobController::RS_SUCCESS;
	}

	return IJobController::RS_INVALID_JOB_ID;
}


// protected methods

// reimplemented (icomp::CComponentBase)

template<typename JobParams, typename JobResult, typename JobInfoExtension>
void TJobControllerCompBase<JobParams, JobResult, JobInfoExtension>::OnComponentCreated()
{
	BaseClass::OnComponentCreated();

	if (m_jobQueueManagerCompPtr.IsValid()){
		m_jobQueueObserver.RegisterObject(m_jobQueueManagerCompPtr.GetPtr(), &TJobControllerCompBase::OnJobQueueChanged);
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

		OnTaskProgressChanged(info.elementId, info.progress);
	}

	if (changeset.GetChangeInfoMap().contains(imthype::IJobQueueManager::CN_JOB_STATUS_CHANGED)){
		QVariant value = changeset.GetChangeInfoMap().value(imthype::IJobQueueManager::CN_JOB_STATUS_CHANGED);
		imthype::IJobQueueManager::JobStatusInfo info = value.value<imthype::IJobQueueManager::JobStatusInfo>();

		OnTaskStatusChanged(info.elementId, info.status);
	}
}


} // namespace imtservergql
