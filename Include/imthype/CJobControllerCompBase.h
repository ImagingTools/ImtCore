// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ACF includes
#include <icomp/CComponentBase.h>
#include <iprm/IParamsSet.h>

// ImtCore includes
#include <imtbase/IProgressSessionsManager.h>
#include <imtbase/TModelUpdateBinder.h>
#include <imthype/IJobQueueManager.h>
#include <imthype/IJobController.h>
#include <imthype/CStandardJobOutput.h>
#include <imtbase/CSimpleReferenceCollection.h>


namespace imthype
{


class CJobControllerCompBase: public icomp::CComponentBase, virtual public IJobController
{
public:
	typedef CComponentBase BaseClass;

	I_BEGIN_BASE_COMPONENT(CJobControllerCompBase)
		I_REGISTER_INTERFACE(IJobController)
		I_ASSIGN(m_defaultJobTypeIdAttrPtr, "DefaultJobTypeId", "Default job type ID", true, "");
		I_ASSIGN(m_progressSessionManagerCompPtr, "ProgressSessionManager", "ProgressSessionManager", false, "ProgressSessionManager");
		I_ASSIGN(m_jobQueueManagerCompPtr, "JobQueueManager", "Job queue manager", false, "JobQueueManager");
		I_ASSIGN(m_inputCollectionCompPtr, "InputCollection", "Input collection", false, "InputCollection");
		I_ASSIGN(m_resultCollectionCompPtr, "ResultCollection", "Result collection", false, "ResultCollection");
		I_ASSIGN(m_jobParamsFactPtr , "JobParams", "Job params factory", false, "JobParams");
	I_END_COMPONENT;

	CJobControllerCompBase();

	// reimplemented (IJobController)
	virtual typename IJobController::RequestStatus BeginJob(const QByteArray& jobId, const iprm::IParamsSet* jobParams) override;
	virtual istd::IChangeableUniquePtr GetJobResult(const QByteArray& jobId) const override;
	virtual typename IJobController::JobStatus GetJobStatus(const QByteArray& jobId) const override;
	virtual typename IJobController::RequestStatus CancelJob(const QByteArray& jobId) override;
	virtual typename IJobController::RequestStatus RemoveJob(const QByteArray& jobId) override;

protected:
	struct JobInfo
	{
		QByteArray queueJobId;

		iprm::IParamsSetUniquePtr paramsPtr;
		istd::IChangeableUniquePtr resultPtr;
		typename IJobController::JobStatus status = IJobController::JS_FAILED;

		ibase::IProgressManager* jobProgressManagerPtr = nullptr;
		std::unique_ptr<ibase::IProgressLogger> progressLoggerPtr;

		imtbase::CSimpleReferenceCollection inputRefCollection;

		CStandardJobOutput jobOutput;
	};
	typedef std::shared_ptr<JobInfo> JobInfoPtr;

protected:
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
	I_FACT(iprm::IParamsSet, m_jobParamsFactPtr);

	imtbase::TModelUpdateBinder<IJobQueueManager, CJobControllerCompBase> m_jobQueueObserver;
};


} // namespace imtservergql


