// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtrest/CServletRequestDispatcherComp.h>

// ImtCore includes
#include <imtcom/CRequestSender.h>


namespace imtrest
{


CServletRequestDispatcherComp::CServletRequestDispatcherComp()
	:m_requestDispatcher(*this)
{
	// Prime Qt network/SSL globals on the application thread before any worker runs
	// CreateGqlContext / DoSyncPost. Otherwise the first worker task owns that one-shot init
	// and Qt logs:
	//   QObject: Cannot create children for a parent that is in a different thread
	//   (Parent is QCoreApplication..., current thread is CWorkerThread...)
	imtcom::CRequestSender::InitializeNetworkBackend();
}


// reimplemented (imtrest::IRequestServlet)

bool CServletRequestDispatcherComp::IsCommandSupported(const QByteArray& /*commandId*/) const
{
	return true;
}


ConstResponsePtr CServletRequestDispatcherComp::ProcessRequest(const IRequest& request, const QByteArray& subCommandId) const
{
	m_requestDispatcher.ProcessRequest(request, subCommandId);

	// Asynchronous: the response is dispatched by the task, once a worker has run it.
	return ConstResponsePtr();
}


// reimplemented (icomp::CComponentBase)

void CServletRequestDispatcherComp::OnComponentCreated()
{
	BaseClass::OnComponentCreated();

	m_requestDispatcher.SetTaskQueue(m_taskQueueCompPtr.GetPtr());
	m_requestDispatcher.SetResponseDispatcher(m_requestManagerCompPtr.GetPtr());

	// The pool holds the servlets, one per worker, but has no idea what they are - see
	// IWorkerTaskQueue::SetWorkerContextFactory.
	m_taskQueueCompPtr->SetWorkerContextFactory(
				[this]() -> IWorkerTaskQueue::WorkerContextPtr {
					return m_requestHandlerCompPtr.CreateInstance();
				});
}


} // namespace imtrest
