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
	Direct access to the server's worker-thread pool for work that is not a request.

	The pool is otherwise only reachable through the transport/servlet path
	(IRequestServlet::ProcessRequest, which needs an IRequest, creates a servlet per
	worker and sends a response). This interface exists so that callers which merely
	want to run something on a pool worker do not have to fabricate an IRequest and a
	throw-away response just to get scheduled: it bypasses transport rather than
	faking a request.

	Typical use is a component that must keep its own thread responsive - for example
	an asynchronous measurement session that receives a batch of results on a
	non-blocking session thread but must apply them to a document, which pulls a long
	synchronous notify/publish stack in behind it.
*/
class IWorkerTaskQueue: virtual public istd::IPolymorphic
{
public:
	typedef std::function<void()> Task;

	/**
		Post \a task for execution on one of the pool's worker threads.

		Tasks sharing a non-empty \a orderingKey run in submission order and never
		concurrently with one another; tasks with different keys may run in parallel.
		An empty key means "no ordering constraint". The key matters whenever the
		posted tasks mutate shared state: the dispatcher hands work to whichever
		worker is idle, so without a key two batches posted in order could be applied
		out of order.

		Thread-safe, callable from any thread, and never blocks the posting thread.

		\param  orderingKey Serialisation key, or an empty array for none.
		\param  task        Callable to run. It must not block waiting on the thread
							that posted it - that thread is free to be a caller which
							is itself waiting for this task.
		\return false if the task could not be queued because the pool is shutting down.
	*/
	virtual bool PostTask(const QByteArray& orderingKey, Task task) = 0;
};


} // namespace imtrest
