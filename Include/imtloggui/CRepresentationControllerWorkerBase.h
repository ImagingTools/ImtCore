#pragma once


// Acf includes
#include <istd/IChangeable.h>
#include <istd/TSmartPtr.h>

// Qt includes
#include <QtCore/QThread>
#include <QtCore/QMutex>
#include <QtCore/QMap>

namespace imtloggui
{


class CWorker: public QThread
{
	Q_OBJECT
public:
	typedef istd::TSmartPtr<istd::IChangeable> TaskPtr;

	CWorker();

	QByteArray AddJob(const TaskPtr& taskPtr);
	bool CancelTask(const QByteArray& taskId);
	void CancelAll();
	QByteArray GetCurrentTaskId();
	void Wait();

Q_SIGNALS:
	void TaskFinished(QByteArray taskId);
	void TaskCanceled(QByteArray taskId);

protected:
	virtual bool DoJob(TaskPtr& taskPtr) = 0;
	bool IsTaskInterruptionRequested();

private:
	virtual void run() override;

private Q_SLOTS:
	void OnFinished();

private:
	enum ThreadState
	{
		TS_IDLE = 0,
		TS_RUNNING,
		TS_PENDING
	};

	struct TaskListItem
	{
		QByteArray id;
		TaskPtr taskPtr;
	};

private:
	ThreadState m_state;
	QMutex m_stateMutex;

	QList<TaskListItem> m_taskList;
	QMutex m_taskListMutex;

	TaskListItem m_currentTask;
	QMutex m_currentTaskMutex;

	bool m_isTaskInterruptionRequested;
	QMutex m_taskInterruptionMutex;
};


class CRepresentationControllerWorkerBase: public QObject
{
	Q_OBJECT
public:
	CRepresentationControllerWorkerBase();

Q_SIGNALS:
	void EmitNewJobAdded();
	void EmitResultReady();
	
protected Q_SLOTS:
	virtual void OnNewJobAdded() = 0;
	virtual void OnResultReady() = 0;
};


} // namespace imtloggui


