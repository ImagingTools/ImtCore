// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtrest/CWorkerThread.h>


// Qt includes
#include <QtCore/QMetaObject>

// ImtCore includes
#include <imtrest/CWorkerPool.h>


namespace imtrest
{


namespace
{


/**
	Worker context of the calling thread. Per-thread rather than per-pool: a thread belongs to
	exactly one pool and runs exactly one task at a time.
*/
thread_local istd::IPolymorphic* g_currentContextPtr = nullptr;


} // anonymous namespace


CWorkerThread::CWorkerThread(CWorkerPool& pool)
	: m_status(ST_PROCESS),
	  m_poolPtr(&pool),
	  m_workerPtr(nullptr)
{
}


CWorkerThread::Status CWorkerThread::GetStatus()
{
	QMutexLocker lock(&m_statusMutex);

	return m_status;
}


void CWorkerThread::SetStatus(Status status)
{
	QMutexLocker lock(&m_statusMutex);

	m_status = status;
}


void CWorkerThread::SetContext(ContextPtr&& contextPtr)
{
	m_contextPtr = std::move(contextPtr);
}


void CWorkerThread::SetPendingTask(Task task, const QByteArray& orderingKey)
{
	m_pendingTask = std::move(task);
	m_pendingOrderingKey = orderingKey;
}


istd::IPolymorphic* CWorkerThread::GetCurrentContext()
{
	return g_currentContextPtr;
}


void CWorkerThread::SetCurrentContext(istd::IPolymorphic* contextPtr)
{
	g_currentContextPtr = contextPtr;
}


//reimplemented (QThread)

void CWorkerThread::run()
{
	// CWorker is a QObject bound to THIS worker thread. The task is posted so that it runs
	// only after exec() pumps the event loop: a task may block in a nested event loop, and
	// timers need a dispatcher.
	m_workerPtr.SetPtr(new CWorker(this, m_contextPtr.GetPtr()));
	m_workerPtr->moveToThread(this);

	if (m_pendingTask){
		PostTask(std::move(m_pendingTask), m_pendingOrderingKey);

		m_pendingTask = Task();
	}

	exec();
}


void CWorkerThread::PostTask(Task task, const QByteArray& orderingKey)
{
	CWorker* workerPtr = m_workerPtr.GetPtr();
	if (workerPtr == nullptr){
		return;
	}

	QMetaObject::invokeMethod(
				workerPtr,
				[workerPtr, task = std::move(task), orderingKey]() mutable {
					workerPtr->ProcessTask(std::move(task), orderingKey);
				},
				Qt::QueuedConnection);
}


void CWorkerThread::NotifyTaskFinished(const QByteArray& orderingKey)
{
	CWorkerPool* poolPtr = m_poolPtr;
	if (poolPtr == nullptr){
		return;
	}

	// Hop to the pool's own thread: the pool releases the ordering key and dispatches the
	// next queued task there, and every change to its schedule but an append happens there.
	poolPtr->PostToOwnThread(
				[poolPtr, orderingKey](){
					poolPtr->OnTaskFinished(orderingKey);
				});
}


} // namespace imtrest
