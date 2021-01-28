#include <imtloggui/CRepresentationControllerWorkerBase.h>


// Qt includes
#include <QtCore/QUuid>
#include <QtCore/QDebug>

// Acf includes
#include <istd/CChangeNotifier.h>


namespace imtloggui
{


// public methods

CWorker::CWorker()
	:m_state(TS_IDLE)
{
	connect(this, &QThread::finished, this, &CWorker::OnFinished, Qt::QueuedConnection);
}


QByteArray CWorker::AddJob(const TaskPtr& taskPtr)
{

	TaskListItem task;
	task.id = QUuid::createUuid().toByteArray();
	task.taskPtr = taskPtr;

	QMutexLocker lockerTaskList(&m_taskListMutex);
	m_taskList.push_back(task);
	lockerTaskList.unlock();

	QMutexLocker lockerState(&m_stateMutex);
	if (m_state == TS_IDLE){
		m_state = TS_RUNNING;
		start();
	}
	else{
		m_state = TS_PENDING;
	}
	lockerState.unlock();

	return task.id;
}


bool CWorker::CancelTask(const QByteArray& taskId)
{
	QMutexLocker lockerCurrentTask(&m_currentTaskMutex);
	QMutexLocker lockerTaskList(&m_taskListMutex);

	if (m_currentTask.id == taskId){
		QMutexLocker lockerTaskInterruption(&m_taskInterruptionMutex);
		m_isTaskInterruptionRequested = true;
		return true;
	}

	for (int i = 0; i < m_taskList.count(); i++){
		if (m_taskList[i].id == taskId){
			m_taskList.removeAt(i);
			Q_EMIT TaskCanceled(taskId);
			return true;
		}
	}

	return false;
}


void CWorker::CancelAll()
{
	QMutexLocker lockerTaskInterruption(&m_taskInterruptionMutex);
	QMutexLocker lockerTaskList(&m_taskListMutex);
	m_isTaskInterruptionRequested = true;
	m_taskList.clear();
}


QByteArray CWorker::GetCurrentTaskId()
{
	QMutexLocker locker(&m_taskInterruptionMutex);
	return m_currentTask.id;
}


void CWorker::Wait()
{
	wait();
}


// protected methods

bool CWorker::IsTaskInterruptionRequested()
{
	QMutexLocker locker(&m_taskInterruptionMutex);
	return m_isTaskInterruptionRequested;
}


// private methods

void CWorker::run()
{
	bool isFinished = false;
	while (!isFinished && !isInterruptionRequested()){
		QMutexLocker lockerTaskList(&m_taskListMutex);
		QMutexLocker lockerCurrentTask(&m_currentTaskMutex);
		if (!m_taskList.isEmpty()){
			qDebug() << m_taskList.count();
			m_currentTask = m_taskList.front();
			m_taskList.pop_front();
			lockerCurrentTask.unlock();
			lockerTaskList.unlock();

			QMutexLocker lockerTaskInterruption(&m_taskInterruptionMutex);
			m_isTaskInterruptionRequested = false;
			lockerTaskInterruption.unlock();

			if (DoJob(m_currentTask.taskPtr)){
				istd::CChangeNotifier notifier(m_currentTask.taskPtr.GetPtr());
				Q_EMIT TaskFinished(m_currentTask.id);
			}
			else{
				Q_EMIT TaskCanceled(m_currentTask.id);
			}

			lockerCurrentTask.relock();
			m_currentTask.id.clear();
			m_currentTask.taskPtr.SetPtr(nullptr);
		}
		else{
			isFinished = true;
		}
	};

	QMutexLocker lockerTaskList(&m_taskListMutex);
	QMutexLocker lockerCurrentTask(&m_currentTaskMutex);
	QMutexLocker lockerTaskInterruption(&m_taskInterruptionMutex);

	m_taskList.clear();
	m_currentTask.id.clear();
	m_currentTask.taskPtr.SetPtr(nullptr);
	m_isTaskInterruptionRequested = false;
}

// private slots:

void CWorker::OnFinished()
{
	QMutexLocker locker(&m_stateMutex);
	if (m_state == TS_PENDING){
		m_state = TS_RUNNING;
		start();
	}
	else{
		m_state = TS_IDLE;
	}
}


} // namespace imtloggui


