// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtrest/CWorkerManagerComp.h>

// Qt includes
#include <QtCore/QCoreApplication>

// ImtCore includes
#include <imtcom/CRequestSender.h>


namespace imtrest
{


CWorkerManagerComp::CWorkerManagerComp()
	:m_requestDispatcher(*this, *this),
	m_pool(*this)
{
	// Prime Qt network/SSL globals on the application thread before any worker runs
	// CreateGqlContext / DoSyncPost. Otherwise the first worker task owns that one-shot init
	// and Qt logs:
	//   QObject: Cannot create children for a parent that is in a different thread
	//   (Parent is QCoreApplication..., current thread is CWorkerThread...)
	imtcom::CRequestSender::InitializeNetworkBackend();

	connect(
				QCoreApplication::instance(),
				&QCoreApplication::aboutToQuit,
				this,
				&CWorkerManagerComp::AboutToQuit,
				Qt::DirectConnection);
}


// reimplemented (imtrest::IRequestServlet)

bool CWorkerManagerComp::IsCommandSupported(const QByteArray& /*commandId*/) const
{
	return true;
}


ConstResponsePtr CWorkerManagerComp::ProcessRequest(const IRequest& request, const QByteArray& subCommandId) const
{
	m_requestDispatcher.ProcessRequest(request, subCommandId);

	// The transport path is asynchronous: the response is dispatched by the task.
	return ConstResponsePtr();
}


// reimplemented (imtrest::IWorkerTaskQueue)

bool CWorkerManagerComp::PostTask(const QByteArray& orderingKey, Task task)
{
	return m_pool.PostTask(orderingKey, std::move(task));
}


istd::IPolymorphic* CWorkerManagerComp::GetWorkerContext() const
{
	return m_pool.GetWorkerContext();
}


// reimplemented (icomp::CComponentBase)

void CWorkerManagerComp::OnComponentCreated()
{
	BaseClass::OnComponentCreated();

	m_pool.SetThreadsLimit(*m_threadsLimitAttrPtr);

	// Only when a servlet factory is wired: a pool without one still runs posted tasks, it
	// just has no context to offer them - and requests, which are the only tasks needing one,
	// cannot arrive without a servlet anyway.
	if (m_requestHandlerCompPtr.IsValid()){
		m_pool.SetWorkerContextFactory(
					[this]() -> CWorkerPool::ContextPtr {
						return m_requestHandlerCompPtr.CreateInstance();
					});
	}

	if (m_requestManagerCompPtr.IsValid()){
		m_requestDispatcher.SetResponseDispatcher(m_requestManagerCompPtr.GetPtr());
	}
}


void CWorkerManagerComp::AboutToQuit()
{
	m_pool.Shutdown();
}


} // namespace imtrest
