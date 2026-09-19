// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// STL includes
#include <memory>

// Qt includes
#include <QtCore/QPointer>

// ImtCore includes
#include <imtrest/IRequest.h>
#include <imtrest/IRequestServlet.h>
#include <imtrest/IResponseDispatcher.h>
#include <imtrest/IWorkerTaskQueue.h>


namespace imtrest
{


/**
	The transport path expressed as worker-pool tasks: one special case of a task, not a second
	kind of work item.

	Each request becomes a task that looks up the servlet of the worker it landed on - the
	pool's worker context - runs the request through it and dispatches the response. The pool
	itself therefore knows nothing about requests, servlets or responses.

	Ownership of a request passes to this dispatcher and lasts until the task has run or has
	been dropped by a shutdown, whichever comes first. The request is destroyed on the owner's
	thread in both cases: a request's destructor may notify subscription bookkeeping, and that
	must not start happening on a worker thread.
*/
class CServletRequestDispatcher
{
public:
	/**
		\param taskQueue     Pool running the requests. Its worker context must be the servlet.
		\param requestOwner  QObject whose thread requests are destroyed on.
	*/
	CServletRequestDispatcher(IWorkerTaskQueue& taskQueue, QObject& requestOwner);

	void SetResponseDispatcher(IResponseDispatcher* responseDispatcherPtr);

	/**
		Queue \a request for processing on a pool worker, taking ownership of it.

		\return false if the pool refused the request, in which case it has been destroyed.
	*/
	bool ProcessRequest(const IRequest& request, const QByteArray& subCommandId);

private:
	typedef std::shared_ptr<const IRequest> OwnedRequestPtr;

	OwnedRequestPtr MakeOwnedRequest(const IRequest& request) const;

private:
	IWorkerTaskQueue& m_taskQueue;
	QPointer<QObject> m_requestOwnerPtr;
	IResponseDispatcher* m_responseDispatcherPtr;
};


} // namespace imtrest
