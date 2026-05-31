// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imthype/CJobControllerCompBase.h>


// ACF includes
#include <icomp/CComponentBase.h>
#include <iprm/IParamsSet.h>

// ImtCore includes
#include <imtbase/IProgressSessionsManager.h>
#include <imtbase/TModelUpdateBinder.h>
#include <imthype/IJobQueueManager.h>
#include <imthype/CStandardJobOutput.h>
#include <imtbase/CSimpleReferenceCollection.h>


namespace imthype
{


// public methods

CJobControllerCompBase::CJobControllerCompBase()
	:m_jobQueueObserver(*this)
{
}


// reimplemented (imtservergql::ICollectionImportController)

IJobController::RequestStatus CJobControllerCompBase::BeginJob(const QByteArray& jobId, const iprm::IParamsSet* jobParams)
{
	if (m_jobQueueManagerCompPtr.IsValid() && m_jobParamsFactPtr.IsValid()){
		QMutexLocker locker(&m_mutex);

		if (!m_jobs.contains(jobId)){
			iprm::IParamsSetUniquePtr jobParamsPtr = m_jobParamsFactPtr.CreateInstance();
			Q_ASSERT(jobParamsPtr.IsValid());

			if (jobParams != nullptr && !jobParamsPtr->CopyFrom(*jobParams)){
				return RS_FAILED;
			}

			JobInfoPtr jobPtr(new JobInfo);
			jobPtr->paramsPtr = std::move(jobParamsPtr);
			jobPtr->status = IJobController::JS_IN_PROGRESS;

			if (PrepareInputObjects(*jobPtr)){
				jobPtr->queueJobId = m_jobQueueManagerCompPtr->InsertNewJobIntoQueue(jobId, *m_defaultJobTypeIdAttrPtr, jobPtr->inputRefCollection, jobPtr->paramsPtr.GetPtr());
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

	return IJobController::RS_FAILED;
}


istd::IChangeableUniquePtr CJobControllerCompBase::GetJobResult(const QByteArray& jobId) const
{
	QMutexLocker locker(&m_mutex);

	istd::IChangeableUniquePtr retVal;

	if (m_jobs.contains(jobId)){
		if (m_jobs[jobId]->resultPtr.IsValid()){
			retVal.MoveCastedPtr(m_jobs[jobId]->resultPtr->CloneMe());
		}
	}

	return retVal;
}


IJobController::JobStatus CJobControllerCompBase::GetJobStatus(const QByteArray& jobId) const
{
	QMutexLocker locker(&m_mutex);

	if (m_jobs.contains(jobId)){
		return m_jobs[jobId]->status;
	}

	return IJobController::JS_INVALID_JOB_ID;
}


IJobController::RequestStatus CJobControllerCompBase::CancelJob(const QByteArray& jobId)
{
	QMutexLocker locker(&m_mutex);

	if (m_jobs.contains(jobId)){
		//if (m_progressSessionManagerCompPtr.IsValid()){
		//	m_progressSessionManagerCompPtr->CancelProgressSession(jobId, "Job canceled");
		//}

		//m_jobs.remove(jobId);
		if (m_jobQueueManagerCompPtr.IsValid()){
			if (m_jobQueueManagerCompPtr->CancelJob(m_jobs[jobId]->queueJobId)){
				return IJobController::RS_SUCCESS;
			}
		}

		return IJobController::RS_FAILED;
	}

	return IJobController::RS_INVALID_JOB_ID;
}


IJobController::RequestStatus CJobControllerCompBase::RemoveJob(const QByteArray& jobId)
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

void CJobControllerCompBase::OnComponentCreated()
{
	BaseClass::OnComponentCreated();

	if (m_jobQueueManagerCompPtr.IsValid()){
		m_jobQueueObserver.RegisterObject(m_jobQueueManagerCompPtr.GetPtr(), &CJobControllerCompBase::OnJobQueueChanged);
	}
}


void CJobControllerCompBase::OnComponentDestroyed()
{
	m_jobQueueObserver.UnregisterAllObjects();

	BaseClass::OnComponentDestroyed();
}


// private methods

void CJobControllerCompBase::OnJobQueueChanged(const istd::IChangeable::ChangeSet& changeset, const IJobQueueManager* /* modelPtr */)
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


inline void CJobControllerCompBase::OnTaskStatusChanged(const QByteArray& queueJobId, IJobQueueManager::ProcessingStatus taskStatus)
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


inline void CJobControllerCompBase::OnTaskProgressChanged(const QByteArray& queueJobId, double progress)
{
	QMutexLocker locker(&m_mutex);

	QByteArray jobId = FindJob(queueJobId);

	if (!jobId.isEmpty() && m_jobs[jobId]->progressLoggerPtr != nullptr){
		m_jobs[jobId]->progressLoggerPtr->OnProgress(progress);
	}
}


inline QByteArray CJobControllerCompBase::FindJob(const QByteArray& queueJobId) const
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


