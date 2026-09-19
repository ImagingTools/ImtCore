// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// Qt includes
#include <QtCore/QThread>

// ACF includes
#include <istd/IPolymorphic.h>
#include <istd/TDelPtr.h>
#include <istd/TInterfacePtr.h>

// ImtCore includes
#include <imtrest/CWorker.h>
#include <imtrest/IWorkerTaskQueue.h>


namespace imtrest
{

class CWorkerPool;


/**
	One thread of \ref CWorkerPool. It runs an event loop and executes the tasks the pool hands
	it, one at a time, for as long as the pool lives.
*/
class CWorkerThread: public QThread
{
	Q_OBJECT
public:
	typedef IWorkerTaskQueue::Task Task;
	typedef istd::TUniqueInterfacePtr<istd::IPolymorphic> ContextPtr;

	explicit CWorkerThread(CWorkerPool& pool);

	enum Status
	{
		ST_PROCESS,
		ST_CLOSE
	};

	Status GetStatus();
	void SetStatus(Status status);

	/**
		Provide the context object this worker offers to the tasks it runs.

		The pool creates it on its own thread and never on the worker: a factory's
		OnComponentCreated() may construct QObjects parented to qApp (translators, timers),
		and doing that from a worker thread logs the one-shot affinity warning and leaves the
		objects with the wrong thread affinity.
	*/
	void SetContext(ContextPtr&& contextPtr);

	/**
		Set the task this thread runs as soon as it is up. Used only for the task a freshly
		created thread is started with; afterwards work arrives through \ref PostTask.
	*/
	void SetPendingTask(Task task, const QByteArray& orderingKey);

	/**
		Post \a task to this worker's CWorker on the worker thread. Uses a captured-argument
		lambda rather than a queued signal: a queued signal needs every argument type
		registered as a queued metatype, and without it Qt silently drops the call, so the
		task never runs and the worker idles forever in exec().
	*/
	void PostTask(Task task, const QByteArray& orderingKey);

	/**
		Notify the pool, on the pool's own thread, that a task finished, so that
		\a orderingKey is released and the next queued task is dispatched. Called by CWorker.
	*/
	void NotifyTaskFinished(const QByteArray& orderingKey);

	/**
		Context of the pool worker running the calling task, or nullptr when the caller is not
		a pool worker executing a task. \ref SetCurrentContext is called by CWorker around
		each task.
	*/
	static istd::IPolymorphic* GetCurrentContext();
	static void SetCurrentContext(istd::IPolymorphic* contextPtr);

	//reimplemented (QThread)
	virtual void run() override;

private:
	Status m_status;
	CWorkerPool* m_poolPtr;
	istd::TDelPtr<CWorker> m_workerPtr;
	ContextPtr m_contextPtr;
	Task m_pendingTask;
	QByteArray m_pendingOrderingKey;
	mutable QMutex m_statusMutex;
};


} // namespace imtrest
