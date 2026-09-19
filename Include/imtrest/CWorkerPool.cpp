// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtrest/CWorkerPool.h>


// Qt includes
#include <QtCore/QMetaObject>
#include <QtCore/QThread>

// ImtCore includes
#include <imtrest/CWorkerThread.h>


namespace imtrest
{


CWorkerPool::CWorkerPool(QObject& dispatchOwner)
	:m_dispatchOwner(dispatchOwner),
	m_threadsLimit(5)
{
}


CWorkerPool::~CWorkerPool()
{
	Shutdown();
}


void CWorkerPool::SetThreadsLimit(int threadsLimit)
{
	QMutexLocker lock(&m_mutex);

	m_threadsLimit = qMax(1, threadsLimit);
}


void CWorkerPool::SetWorkerContextFactory(WorkerContextFactory workerContextFactory)
{
	QMutexLocker lock(&m_mutex);

	m_workerContextFactory = std::move(workerContextFactory);
}


bool CWorkerPool::PostTask(const QByteArray& orderingKey, Task task)
{
	{
		QMutexLocker lock(&m_mutex);

		if (!m_schedule.Append(orderingKey, std::move(task))){
			return false;
		}
	}

	ScheduleDispatch();

	return true;
}


istd::IPolymorphic* CWorkerPool::GetWorkerContext() const
{
	return CWorkerThread::GetCurrentContext();
}


void CWorkerPool::Shutdown()
{
	QList<CWorkerThread*> workerList;
	QList<CWorkerTaskSchedule::Item> droppedList;

	{
		QMutexLocker lock(&m_mutex);

		droppedList = m_schedule.Shutdown();

		workerList = m_workerList;
		m_workerList.clear();
	}

	// Destroyed outside the lock: a dropped task owns whatever it captured, and releasing
	// that may run arbitrary code - the request arm of imtrest, for one, frees a request here.
	droppedList.clear();

	// Joined without the lock held: a worker finishing in the meantime reports back through
	// the owner's event loop, and nothing here may hold the lock it needs.
	for (CWorkerThread* workerPtr: workerList){
		workerPtr->quit();
		workerPtr->wait(1000);
		workerPtr->deleteLater();
	}
}


void CWorkerPool::PostToOwnThread(std::function<void()> call)
{
	if (QThread::currentThread() == m_dispatchOwner.thread()){
		call();

		return;
	}

	QMetaObject::invokeMethod(&m_dispatchOwner, std::move(call), Qt::QueuedConnection);
}


void CWorkerPool::OnTaskFinished(const QByteArray& orderingKey)
{
	{
		QMutexLocker lock(&m_mutex);

		m_schedule.ReleaseKey(orderingKey);
	}

	DispatchNext();
}


// private methods

void CWorkerPool::ScheduleDispatch()
{
	PostToOwnThread([this](){
		DispatchNext();
	});
}


void CWorkerPool::DispatchNext()
{
	CWorkerThread* idleWorkerPtr = nullptr;

	{
		QMutexLocker lock(&m_mutex);

		if (!m_schedule.HasDispatchable()){
			return;
		}

		idleWorkerPtr = FindIdleWorker();
		if ((idleWorkerPtr == nullptr) && (m_workerList.count() >= m_threadsLimit)){
			// Pool saturated: the task stays queued until a worker reports back.
			return;
		}
	}

	// Only an append may happen between the check above and the take below, and it can only
	// add work, so what was dispatchable still is. Everything else about the schedule changes
	// on this thread alone.

	if (idleWorkerPtr != nullptr){
		CWorkerTaskSchedule::Item item;
		{
			QMutexLocker lock(&m_mutex);

			if (!m_schedule.TakeDispatchable(item)){
				return;
			}
		}

		idleWorkerPtr->SetStatus(CWorkerThread::ST_PROCESS);
		idleWorkerPtr->PostTask(std::move(item.task), item.orderingKey);

		return;
	}

	// Created before the task leaves the queue, so that a failure here leaves it queued
	// rather than dropping it, and on this thread rather than on the new worker - see
	// CWorkerThread::SetContext.
	ContextPtr contextPtr;
	if (!CreateWorkerContext(contextPtr)){
		return;
	}

	CWorkerTaskSchedule::Item item;
	CWorkerThread* workerPtr = nullptr;

	{
		QMutexLocker lock(&m_mutex);

		if (!m_schedule.TakeDispatchable(item)){
			return;
		}

		workerPtr = new CWorkerThread(*this);

		m_workerList.append(workerPtr);
	}

	workerPtr->SetContext(std::move(contextPtr));
	workerPtr->SetPendingTask(std::move(item.task), item.orderingKey);

	workerPtr->start();
}


CWorkerThread* CWorkerPool::FindIdleWorker() const
{
	for (CWorkerThread* workerPtr: m_workerList){
		if (workerPtr->GetStatus() == CWorkerThread::ST_CLOSE){
			return workerPtr;
		}
	}

	return nullptr;
}


bool CWorkerPool::CreateWorkerContext(ContextPtr& result) const
{
	WorkerContextFactory workerContextFactory;
	{
		QMutexLocker lock(&m_mutex);

		workerContextFactory = m_workerContextFactory;
	}

	if (!workerContextFactory){
		// A pool whose tasks need no context.
		return true;
	}

	// Called without the lock held: creating a context runs foreign code, which may post to
	// this pool.
	result = workerContextFactory();

	return result.IsValid();
}


} // namespace imtrest
