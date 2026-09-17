// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// Qt includes
#include <QtCore/QList>
#include <QtCore/QMutex>
#include <QtCore/QSet>

// ACF includes
#include <ilog/TLoggerCompWrap.h>

// ImtCore includes
#include <imtrest/IRequestServlet.h>
#include <imtrest/IResponseDispatcher.h>
#include <imtrest/IWorkerTaskQueue.h>
#include <imtrest/CWorkerThread.h>


namespace imtrest
{


/**
	Owner of the server's worker-thread pool.

	Two entry points feed the same pool and the same queue:
	- IRequestServlet::ProcessRequest - the transport path. Each worker owns a servlet,
	  a response is sent, and the request is deleted when it finishes.
	- IWorkerTaskQueue::PostTask - a plain callable. No servlet, no response.

	A queued work item is handed to whichever worker is idle, so nothing may be assumed
	about the order in which two of them complete, with one exception: tasks sharing a
	non-empty ordering key are serialised against each other (see
	\ref IWorkerTaskQueue::PostTask).
*/
class CWorkerManagerComp:
			public QObject,
			public ilog::CLoggerComponentBase,
			public imtrest::IRequestServlet,
			public imtrest::IWorkerTaskQueue
{
	Q_OBJECT
public:
	typedef ilog::CLoggerComponentBase BaseClass;

	I_BEGIN_COMPONENT(CWorkerManagerComp);
		I_REGISTER_INTERFACE(IRequestServlet)
		I_REGISTER_INTERFACE(IWorkerTaskQueue)
		I_ASSIGN(m_requestHandlerCompPtr, "RequestHandler", "Request handler registered for the server", true, "RequestHandler");
		I_ASSIGN(m_requestManagerCompPtr, "RequestManager", "Request manager registered for the server", true, "RequestManager");
		I_ASSIGN(m_threadsLimitAttrPtr, "ThreadsLimit", "Limit of threads", true, 5);
	I_END_COMPONENT

	CWorkerManagerComp();

	IRequestServletPtr CreateServlet() const;
	bool SendResponse(const QByteArray& requestId, ConstResponsePtr& response);

	// reimplemented (imtrest::IRequestServlet)
	virtual bool IsCommandSupported(const QByteArray& commandId) const override;
	virtual ConstResponsePtr ProcessRequest(const IRequest& request, const QByteArray& subCommandId = QByteArray()) const override;

	// reimplemented (imtrest::IWorkerTaskQueue)
	virtual bool PostTask(const QByteArray& orderingKey, Task task) override;

	/**
		Called (via a queued lambda) by CWorkerThread when a request finishes: frees the
		request and dispatches the next queued work item. Runs on this component's thread.
	*/
	void OnFinish(const IRequest* request, const QByteArray& subCommandId);

	/**
		Called (via a queued lambda) by CWorkerThread when a posted task finishes:
		releases \a orderingKey so that the next task carrying it becomes dispatchable,
		and dispatches the next queued work item. Runs on this component's thread.
	*/
	void OnTaskFinish(const QByteArray& orderingKey);

protected Q_SLOTS:
	void AboutToQuit();

private:
	/**
		One entry of the shared queue: either a request (transport path) or a task
		(IWorkerTaskQueue path). Exactly one of the two arms is set.
	*/
	struct WorkItem
	{
		// Request arm. Owned by the queue until a worker takes the item; deleted in OnFinish.
		const IRequest* requestPtr = nullptr;
		QByteArray subCommandId;

		// Task arm.
		Task task;
		QByteArray orderingKey;

		bool IsTask() const
		{
			return requestPtr == nullptr;
		}
	};

	/**
		Index of the first queued item that may run now, or -1 if there is none.

		A request is always dispatchable. A keyed task is dispatchable only while no other
		task with the same key is in flight; scanning front to back is what keeps same-key
		tasks in submission order, and skipping a blocked task is what stops one busy key
		from stalling unrelated work, requests included.

		\pre m_workItemListMutex is held.
	*/
	int FindDispatchableIndex() const;

	/**
		Hand the first dispatchable queued item to an idle worker, starting a new one if
		none is idle and the thread limit allows it. Does nothing when the queue holds
		nothing runnable or the pool is saturated - the item stays queued until a later
		OnFinish / OnTaskFinish picks it up.

		\pre m_workItemListMutex is held.
	*/
	void DispatchNext() const;

	/**
		Mark \a item as in flight, so that no other task with the same ordering key is
		dispatched until it reports back. A no-op for requests and for unkeyed tasks.

		\pre m_workItemListMutex is held.
	*/
	void MarkInFlight(const WorkItem& item) const;

private:
	I_FACT(imtrest::IRequestServlet, m_requestHandlerCompPtr);
	I_REF(imtrest::IResponseDispatcher, m_requestManagerCompPtr);
	I_ATTR(int, m_threadsLimitAttrPtr);

	mutable QList<CWorkerThread*> m_workerList;
	mutable QList<WorkItem> m_workItemList;
	/** Ordering keys of the tasks currently in flight. */
	mutable QSet<QByteArray> m_busyKeys;
	mutable QMutex m_workItemListMutex;
	mutable bool m_isShuttingDown;
};


} // namespace imtrest
