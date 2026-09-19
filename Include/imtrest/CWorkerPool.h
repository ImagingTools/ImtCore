// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// STL includes
#include <functional>

// Qt includes
#include <QtCore/QList>
#include <QtCore/QMutex>
#include <QtCore/QObject>

// ACF includes
#include <istd/IPolymorphic.h>
#include <istd/TInterfacePtr.h>

// ImtCore includes
#include <imtrest/CWorkerTaskSchedule.h>
#include <imtrest/IWorkerTaskQueue.h>


namespace imtrest
{

class CWorkerThread;


/**
	A pool of worker threads running posted tasks, with the ordering guarantees of
	\ref IWorkerTaskQueue.

	It knows nothing about what a task does. A task that needs something bound to the thread
	it runs on - a servlet, a connection, a cache - gets it from the worker context: one object
	per worker, created by \ref SetWorkerContextFactory on the pool's own thread and offered to
	the running task through \ref GetWorkerContext. Without a factory the pool still works;
	its tasks simply have no context.

	Dispatch runs on the thread of the QObject the pool was constructed with, whichever thread
	posted the task. That is what makes the "created on the pool's thread" promise above true
	rather than a hope about who the caller is, and it keeps every change to the schedule but
	an append on a single thread.

	\note The owning QObject's thread must be pumping its event loop for tasks to be
		  dispatched; this is already the case for completion, which reports back the same way.
*/
class CWorkerPool
{
public:
	typedef IWorkerTaskQueue::Task Task;
	typedef istd::TUniqueInterfacePtr<istd::IPolymorphic> ContextPtr;
	typedef std::function<ContextPtr()> WorkerContextFactory;

	/**
		\param dispatchOwner  QObject whose thread dispatches tasks and creates worker
		                      contexts. It must outlive the pool.
	*/
	explicit CWorkerPool(QObject& dispatchOwner);
	~CWorkerPool();

	void SetThreadsLimit(int threadsLimit);

	/**
		Set the factory producing the per-worker context, or leave it unset for a pool whose
		tasks need none. It is called on the owner's thread, once per worker thread created.
	*/
	void SetWorkerContextFactory(WorkerContextFactory workerContextFactory);

	/**
		Queue \a task. See \ref IWorkerTaskQueue::PostTask.
	*/
	bool PostTask(const QByteArray& orderingKey, Task task);

	/**
		Context of the worker running the calling task, or nullptr outside one.
	*/
	istd::IPolymorphic* GetWorkerContext() const;

	/**
		Refuse further tasks, drop the queued ones and join the worker threads. Called by the
		destructor, and on application shutdown by the owner.
	*/
	void Shutdown();

	/**
		Run \a call on the owner's thread, directly when already there.
	*/
	void PostToOwnThread(std::function<void()> call);

	/**
		Release \a orderingKey and dispatch the next queued task. Called by CWorkerThread when
		a task finishes; runs on the owner's thread.
	*/
	void OnTaskFinished(const QByteArray& orderingKey);

private:
	void ScheduleDispatch();

	/**
		Hand the first dispatchable task to an idle worker, starting a new one if none is idle
		and the thread limit allows it. Runs on the owner's thread. Does nothing when the queue
		holds nothing runnable or the pool is saturated - the task stays queued until a later
		OnTaskFinished picks it up.
	*/
	void DispatchNext();

	CWorkerThread* FindIdleWorker() const;
	bool CreateWorkerContext(ContextPtr& result) const;

private:
	QObject& m_dispatchOwner;

	mutable QMutex m_mutex;
	CWorkerTaskSchedule m_schedule;
	QList<CWorkerThread*> m_workerList;

	int m_threadsLimit;
	WorkerContextFactory m_workerContextFactory;
};


} // namespace imtrest
