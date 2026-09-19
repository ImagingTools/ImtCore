// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// STL includes
#include <functional>

// Qt includes
#include <QtCore/QByteArray>

// ACF includes
#include <istd/IPolymorphic.h>


namespace imtrest
{


/**
	A pool of worker threads running posted tasks.

	This is the general entry point to the pool. Work that belongs to a transport - an
	IRequest, a servlet, a response - is one special case of a task, built on this interface
	rather than beside it.
*/
class IWorkerTaskQueue: virtual public istd::IPolymorphic
{
public:
	typedef std::function<void()> Task;

	/**
		Post \a task for execution on one of the pool's worker threads.

		Tasks sharing a non-empty \a orderingKey run in submission order and never
		concurrently with one another; tasks with different keys may run in parallel. An empty
		key means "no ordering constraint". The key matters whenever the posted tasks mutate
		shared state: the dispatcher hands work to whichever worker is idle, so without a key
		two batches posted in order could be applied out of order. Note that a lock around the
		shared state does not replace a key - it grants exclusion, while the key is what
		decides which of two waiting tasks goes first.

		A task whose key is in flight is skipped rather than blocking the queue behind it, so
		one busy key never holds up unrelated work.

		Accepting a task is not a promise to run it: tasks still queued when the pool shuts
		down are dropped, and a caller that must know the work happened has to observe that
		from the task itself.

		The task runs on a thread with a running event loop, so it may own QObjects and post
		queued calls; by the same token, a task that never returns also stalls that worker.
		It must not throw - an exception would unwind into the worker's event loop.

		Thread-safe, callable from any thread, and never blocks the posting thread.

		\param  orderingKey Serialisation key, or an empty array for none.
		\param  task        Callable to run. It must not block waiting on the thread that
		                    posted it - that thread is free to be a caller which is itself
		                    waiting for this task.
		\return false if the task could not be queued because the pool is shutting down.
	*/
	virtual bool PostTask(const QByteArray& orderingKey, Task task) = 0;

	/**
		The context object of the worker thread running the calling task.

		A pool has one context per worker, created once when the worker starts. It is how a
		task reaches something that has to be bound to the thread it runs on rather than
		shared - a servlet, a connection, a cache - without the pool knowing what that is.

		\return The context, or nullptr when the caller is not a pool worker running a task,
		        or when the pool was given no context factory.
	*/
	virtual istd::IPolymorphic* GetWorkerContext() const = 0;
};


} // namespace imtrest
