// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtrest/CWorker.h>


// ImtCore includes
#include <imtrest/CWorkerThread.h>


namespace imtrest
{


namespace
{


/**
	Publishes \a contextPtr as the calling thread's worker context for as long as it lives, so
	that a task can reach it through IWorkerTaskQueue::GetWorkerContext without being handed
	anything.
*/
class CContextGuard
{
public:
	explicit CContextGuard(istd::IPolymorphic* contextPtr)
	{
		CWorkerThread::SetCurrentContext(contextPtr);
	}

	~CContextGuard()
	{
		CWorkerThread::SetCurrentContext(nullptr);
	}
};


} // anonymous namespace


CWorker::CWorker(CWorkerThread* workerThread, istd::IPolymorphic* contextPtr)
	:m_workerThread(workerThread),
	m_contextPtr(contextPtr)
{
}


void CWorker::ProcessTask(Task task, const QByteArray& orderingKey)
{
	m_workerThread->SetStatus(CWorkerThread::ST_PROCESS);

	if (task){
		QMutexLocker lock(&m_processMutex);

		CContextGuard contextGuard(m_contextPtr);

		task();
	}

	m_workerThread->SetStatus(CWorkerThread::ST_CLOSE);

	// Reported only once the status is back to ST_CLOSE: the pool dispatches the next task
	// from OnTaskFinished and hands it to an idle worker, so reporting earlier could make it
	// skip this one.
	m_workerThread->NotifyTaskFinished(orderingKey);
}


} // namespace imtrest
