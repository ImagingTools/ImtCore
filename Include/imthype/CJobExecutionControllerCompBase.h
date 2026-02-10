// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// Qt includes
#include <QtCore/QTimer>
#include <QtCore/QThreadPool>
#include <QtCore/QRunnable>
#include <QtCore/QMap>

// ACF includes
#include <ibase/CCumulatedProgressManagerBase.h>
#include <ilog/TLoggerCompWrap.h>
#include <iprm/IParamsSet.h>

// Acula includes
#include <imtbase/TModelUpdateBinder.h>
#include <imthype/IJobProcessor.h>
#include <imthype/IJobQueueManager.h>
#include <imthype/CStandardJobOutput.h>


namespace imthype
{


/**
	Base implementation of a controller for the job execution.
	This impementation works using polling of the underlaying job manager.
	If a job in the job queue becomes the status \c PS_WAITING_FOR_PROCESSING,
	the controller takes the job item and starts the job execution in a separate thread.
*/
class CJobExecutionControllerCompBase: public QObject, public ilog::CLoggerComponentBase
{
	Q_OBJECT
public:
	typedef ilog::CLoggerComponentBase BaseClass;

	I_BEGIN_BASE_COMPONENT(CJobExecutionControllerCompBase);
		I_ASSIGN(m_jobQueueManagerCompPtr, "JobQueueManager", "Manager for the job queue", true, "JobQueueManager");
	I_END_COMPONENT;

	CJobExecutionControllerCompBase();

protected:
	virtual void ShutdownAllTasks();

	class JobProgressManager : public ibase::CCumulatedProgressManagerBase
	{
	public:
		JobProgressManager(CJobExecutionControllerCompBase& parent, const QByteArray& jobId);

	protected:
		virtual void OnProgressChanged(double cumulatedValue) override;

	private:
		CJobExecutionControllerCompBase& m_parent;
		QByteArray m_jobId;
	};

	typedef std::shared_ptr<JobProgressManager> JobProgressManagerPtr;

	class Task: public QRunnable
	{
	public:
		Task(
					CJobExecutionControllerCompBase& parent,
					const QByteArray& jobId,
					const IJobProcessor& taskProcessor,
					const imtbase::IReferenceCollection& input,
					iprm::IParamsSetSharedPtr paramsPtr,
					JobProgressManager* progressPtr);

		// reimplemented (QRunnable)
		virtual void run();

	private:
		const IJobProcessor& m_taskProcessor;
		CJobExecutionControllerCompBase& m_parent;
		QByteArray m_jobId;
		CStandardJobOutput m_jobOutput;
		imtbase::IReferenceCollectionSharedPtr m_inputPtr;
		iprm::IParamsSetSharedPtr m_paramsPtr;
		JobProgressManager* m_progressPtr;
	};

protected:
	// reimplemented (icomp::CComponentBase)
	virtual void OnComponentCreated() override;
	virtual void OnComponentDestroyed() override;

protected Q_SLOTS:
	/**
		Transport the inserted job objects to the processing queue.
	*/
	void OnJobPolling();
	void OnJobFinished(const QByteArray& jobId, const imthype::CStandardJobOutput& results);
	void OnJobProgressChanged(const QByteArray& jobId, double progress);

Q_SIGNALS:
	void EmitJobFinished(const QByteArray& jobId, const imthype::CStandardJobOutput& results);
	void EmitJobProgressChanged(const QByteArray& jobId, double progress);

protected:
	void OnJobQueueChanged(const istd::IChangeable::ChangeSet& changeset, const imthype::IJobQueueManager* modelPtr);

	/**
		Get job processor instance for a given worker type.
	*/
	virtual const IJobProcessor* GetTaskProcessor(const QByteArray& workerTypeId) const = 0;

private:
	/**
		Reference to the list of job tasks for the corresponding file type.
	*/
	I_MULTIREF(IJobProcessor, m_jobTasksCompPtr);

	/**
		Manager for the job queue.
	*/
	I_REF(IJobQueueManager, m_jobQueueManagerCompPtr);

	/**
		Time for update the status of the job queue.
	*/
	QTimer m_updateJobQueueTimer;

	/**
		Flag used for stopping all running job tasks.
	*/
	bool m_stopAllTasks;

	/**
		Thread pool used for processing the job queue.
	*/
	QThreadPool m_threadPool;

	QMap<QByteArray, JobProgressManagerPtr> m_jobProgressList;

	imtbase::TModelUpdateBinder<imthype::IJobQueueManager, CJobExecutionControllerCompBase> m_jobQueueObserver;
};


} // namespace imthype


