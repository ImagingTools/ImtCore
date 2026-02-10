// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ACF includes
#include <icomp/CComponentBase.h>
#include <iprm/IParamsSet.h>

// ImtCore includes
#include <imtbase/IProgressSessionsManager.h>
#include <imtbase/TModelUpdateBinder.h>
#include <imthype/IJobQueueManager.h>
#include <imthype/TIJobController.h>
#include <imthype/CStandardJobOutput.h>
#include <imtbase/CSimpleReferenceCollection.h>


namespace imthype
{


template<typename JobParams, typename JobResult, typename JobInfoExtension>
class TJobControllerCompBase: public icomp::CComponentBase, virtual public TIJobController<JobParams, JobResult>
{
public:
	typedef CComponentBase BaseClass;

	using IJobController = TIJobController<JobParams, JobResult>;
	using JobResultPtr = std::shared_ptr<JobResult>;
	using JobParamsSetPtr = std::shared_ptr<iprm::IParamsSet>;

	I_BEGIN_BASE_COMPONENT(TJobControllerCompBase)
		I_REGISTER_INTERFACE(IJobController)
		I_ASSIGN(m_defaultJobTypeIdAttrPtr, "DefaultJobTypeId", "Default job type ID", true, "");
		I_ASSIGN(m_progressSessionManagerCompPtr, "ProgressSessionManager", "ProgressSessionManager", false, "ProgressSessionManager");
		I_ASSIGN(m_jobQueueManagerCompPtr, "JobQueueManager", "Job queue manager", false, "JobQueueManager");
		I_ASSIGN(m_inputCollectionCompPtr, "InputCollection", "Input collection", false, "InputCollection");
		I_ASSIGN(m_resultCollectionCompPtr, "ResultCollection", "Result collection", false, "ResultCollection");
		I_ASSIGN(m_jobParamsCompPtr , "JobParams", "Job params", false, "JobParams");
	I_END_COMPONENT;

	TJobControllerCompBase();

	// reimplemented (TIJobController)
	virtual typename IJobController::RequestStatus BeginJob(const QByteArray& jobId, const JobParams& jobParams) override;
	virtual JobResultPtr GetJobResult(const QByteArray& jobId) const override;
	virtual typename IJobController::JobStatus GetJobStatus(const QByteArray& jobId) const override;
	virtual typename IJobController::RequestStatus CancelJob(const QByteArray& jobId) override;
	virtual typename IJobController::RequestStatus RemoveJob(const QByteArray& jobId) override;

protected:
	struct JobInfo
	{
		QByteArray queueJobId;

		JobParams params;
		JobResultPtr resultPtr;
		typename IJobController::JobStatus status = IJobController::JS_FAILED;

		JobInfoExtension extension;

		ibase::IProgressManager* jobProgressManagerPtr = nullptr;
		std::unique_ptr<ibase::IProgressLogger> progressLoggerPtr;

		imtbase::CSimpleReferenceCollection inputRefCollection;

		CStandardJobOutput jobOutput;
	};
	typedef std::shared_ptr<JobInfo> JobInfoPtr;

protected:
	virtual bool PrepareJobParamsSet(const JobInfo& jobInfo, iprm::IParamsSet& jobParamsSet) const = 0;
	virtual bool PrepareInputObjects(JobInfo& jobInfo) = 0;
	virtual bool PrepareResult(JobInfo& jobInfo) = 0;

	// reimplemented (icomp::CComponentBase)
	void OnComponentCreated() override;
	void OnComponentDestroyed() override;

protected:
	I_REF(imtbase::IObjectCollection, m_inputCollectionCompPtr);
	I_REF(imtbase::IObjectCollection, m_resultCollectionCompPtr);

	QMap<QByteArray, JobInfoPtr> m_jobs;
	mutable QRecursiveMutex m_mutex;

private:
	void OnJobQueueChanged(
	const istd::IChangeable::ChangeSet& changeset, const IJobQueueManager* modelPtr);
	void OnTaskStatusChanged(const QByteArray& queueJobId, IJobQueueManager::ProcessingStatus taskStatus);
	void OnTaskProgressChanged(const QByteArray& queueJobId, double progress);
	QByteArray FindJob(const QByteArray& queueJobId) const;

private:
	I_ATTR(QByteArray, m_defaultJobTypeIdAttrPtr);
	I_REF(imtbase::IProgressSessionsManager, m_progressSessionManagerCompPtr);
	I_REF(IJobQueueManager, m_jobQueueManagerCompPtr);
	I_FACT(iprm::IParamsSet, m_jobParamsCompPtr);

	imtbase::TModelUpdateBinder<IJobQueueManager, TJobControllerCompBase> m_jobQueueObserver;
};


// public methods

template<typename JobParams, typename JobResult, typename JobInfoExtension>
TJobControllerCompBase<JobParams, JobResult, JobInfoExtension>::TJobControllerCompBase()
	:m_jobQueueObserver(*this)
{
}


// reimplemented (imtservergql::ICollectionImportController)

template<typename JobParams, typename JobResult, typename JobInfoExtension>
typename TIJobController<JobParams, JobResult>::RequestStatus TJobControllerCompBase<JobParams, JobResult, JobInfoExtension>::BeginJob(const QByteArray& jobId, const JobParams& jobParams)
{
	if (m_jobQueueManagerCompPtr.IsValid() && m_jobParamsCompPtr.IsValid()){
		QMutexLocker locker(&m_mutex);

		if (!m_jobs.contains(jobId)){
			JobInfoPtr jobPtr(new JobInfo);
			jobPtr->params = jobParams;
			jobPtr->status = IJobController::JS_IN_PROGRESS;

			istd::TUniqueInterfacePtr<iprm::IParamsSet> jobParamsPtr;
			jobParamsPtr = m_jobParamsCompPtr.CreateInstance();

			if (PrepareInputObjects(*jobPtr)){
				if (PrepareJobParamsSet(*jobPtr, *jobParamsPtr)){
					jobPtr->queueJobId = m_jobQueueManagerCompPtr->InsertNewJobIntoQueue(jobId, *m_defaultJobTypeIdAttrPtr, jobPtr->inputRefCollection, jobParamsPtr.GetPtr());
					if (!jobPtr->queueJobId.isEmpty()){
						jobPtr->status = IJobController::JS_IN_PROGRESS;

						if (m_progressSessionManagerCompPtr.IsValid()){
							jobPtr->jobProgressManagerPtr = m_progressSessionManagerCompPtr->BeginProgressSession(jobId, "Job processing progress");
							jobPtr->progressLoggerPtr = jobPtr->jobProgressManagerPtr->StartProgressLogger(true);
						}

						QMutexLocker locker(&m_mutex);

						m_jobs[jobId] = jobPtr;

						return IJobController::RS_SUCCESS;
					}
				}
			}
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
		//if (m_progressSessionManagerCompPtr.IsValid()){
		//	m_progressSessionManagerCompPtr->CancelProgressSession(jobId, "Job canceled");
		//}

		//m_jobs.remove(jobId);
		if (m_jobQueueManagerCompPtr.IsValid() && m_jobParamsCompPtr.IsValid()){
			if (m_jobQueueManagerCompPtr->CancelJob(m_jobs[jobId]->queueJobId)){
				return IJobController::RS_SUCCESS;
			}
		}

		return IJobController::RS_FAILED;
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
void TJobControllerCompBase<JobParams, JobResult, JobInfoExtension>::OnJobQueueChanged(const istd::IChangeable::ChangeSet& changeset, const IJobQueueManager* /* modelPtr */)
{
	if (changeset.GetChangeInfoMap().contains(IJobQueueManager::CN_JOB_PROGRESS_CHANGED)){
		QVariant value = changeset.GetChangeInfoMap().value(IJobQueueManager::CN_JOB_PROGRESS_CHANGED);
		IJobQueueManager::JobProgressInfo info = value.value<IJobQueueManager::JobProgressInfo>();

		OnTaskProgressChanged(info.elementId, info.progress);
	}

	if (changeset.GetChangeInfoMap().contains(IJobQueueManager::CN_JOB_STATUS_CHANGED)){
		QVariant value = changeset.GetChangeInfoMap().value(IJobQueueManager::CN_JOB_STATUS_CHANGED);
		IJobQueueManager::JobStatusInfo info = value.value<IJobQueueManager::JobStatusInfo>();

		OnTaskStatusChanged(info.elementId, info.status);
	}
}


template<typename JobParams, typename JobResult, typename JobInfoExtension>
inline void TJobControllerCompBase<JobParams, JobResult, JobInfoExtension>::OnTaskStatusChanged(const QByteArray& queueJobId, IJobQueueManager::ProcessingStatus taskStatus)
{
	QMutexLocker locker(&m_mutex);

	QByteArray jobId = FindJob(queueJobId);

	if (!jobId.isEmpty()){
		switch (taskStatus){
		case IJobQueueManager::PS_RUNNING:
			m_jobs[jobId]->status = IJobController::JS_IN_PROGRESS;
			break;
		case IJobQueueManager::PS_CANCELING:
			m_jobs[jobId]->status = IJobController::JS_CANCELLATION;
			break;
		case IJobQueueManager::PS_CANCELED:
			m_jobs[jobId]->status = IJobController::JS_CANCELLED;
			if (m_progressSessionManagerCompPtr.IsValid()){
				m_progressSessionManagerCompPtr->CancelProgressSession(jobId, "Task processing has been cancelled");
			}
			break;
		case IJobQueueManager::PS_REJECTED:
			m_jobs[jobId]->status = IJobController::JS_FAILED;
			if (m_progressSessionManagerCompPtr.IsValid()){
				m_progressSessionManagerCompPtr->CancelProgressSession(jobId, "Task processing failed");
			}
			break;
		case IJobQueueManager::PS_FINISHED:
			m_jobs[jobId]->status = IJobController::JS_COMPLETED;

			if (m_jobQueueManagerCompPtr->GetJobResult(queueJobId, m_jobs[jobId]->jobOutput)){
				PrepareResult(*m_jobs[jobId]);
			}

			if (m_progressSessionManagerCompPtr.IsValid()){
				m_progressSessionManagerCompPtr->EndProgressSession(jobId);
			}

			break;
		}
	}
}


template<typename JobParams, typename JobResult, typename JobInfoExtension>
inline void TJobControllerCompBase<JobParams, JobResult, JobInfoExtension>::OnTaskProgressChanged(const QByteArray& queueJobId, double progress)
{
	QMutexLocker locker(&m_mutex);

	QByteArray jobId = FindJob(queueJobId);

	if (!jobId.isEmpty() && m_jobs[jobId]->progressLoggerPtr != nullptr){
		m_jobs[jobId]->progressLoggerPtr->OnProgress(progress);
	}
}


template<typename JobParams, typename JobResult, typename JobInfoExtension>
inline QByteArray TJobControllerCompBase<JobParams, JobResult, JobInfoExtension>::FindJob(const QByteArray& queueJobId) const
{
	QMutexLocker locker(&m_mutex);
	for (const QByteArray& id : m_jobs.keys()){
		if (m_jobs[id]->queueJobId == queueJobId){
			return id;
		}
	}

	return QByteArray();
}


} // namespace imtservergql


