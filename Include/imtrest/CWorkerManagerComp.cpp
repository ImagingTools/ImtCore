// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtrest/CWorkerManagerComp.h>

// Qt includes
#include <QtCore/QCoreApplication>
#include <QtCore/QDebug>

// ImtCore includes
#include <imtcom/CRequestSender.h>


namespace imtrest
{


CWorkerManagerComp::CWorkerManagerComp()
	: m_isShuttingDown(false)
{
	// Prime Qt network/SSL globals on the application thread before any
	// CWorkerThread runs CreateGqlContext / DoSyncPost. Otherwise the first
	// worker request owns that one-shot init and Qt logs:
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


IRequestServletPtr CWorkerManagerComp::CreateServlet() const
{
	if (m_requestHandlerCompPtr.IsValid()){
		return m_requestHandlerCompPtr.CreateInstance();
	}

	return nullptr;
}


bool CWorkerManagerComp::SendResponse(const QByteArray& requestId, ConstResponsePtr& response)
{
	if (!m_requestManagerCompPtr.IsValid()){
		return false;
	}

	return m_requestManagerCompPtr->SendResponse(requestId, response);
}


// reimplemented (IRequestHandler)

ConstResponsePtr CWorkerManagerComp::ProcessRequest(const IRequest& request, const QByteArray& subCommandId) const
{
	{
		QMutexLocker loc(&m_workItemListMutex);

		if (!m_isShuttingDown){
			WorkItem item;
			item.requestPtr = &request;
			item.subCommandId = subCommandId;

			m_workItemList.append(std::move(item));

			DispatchNext();

			return ConstResponsePtr();
		}
	}

	// Shutting down: the queue has already been drained and the workers torn down, so this
	// request would neither be dispatched nor freed by anyone else. Ownership passed to this
	// queue together with the reference, so release it here - the task arm reports the same
	// condition by returning false from PostTask, which a request cannot do.
	delete &request;

	return ConstResponsePtr();
}


bool CWorkerManagerComp::IsCommandSupported(const QByteArray& /*commandId*/) const
{
	return true;
}


// reimplemented (IWorkerTaskQueue)

bool CWorkerManagerComp::PostTask(const QByteArray& orderingKey, Task task)
{
	if (!task){
		return false;
	}

	QMutexLocker loc(&m_workItemListMutex);

	if (m_isShuttingDown){
		return false;
	}

	WorkItem item;
	item.task = std::move(task);
	item.orderingKey = orderingKey;

	m_workItemList.append(std::move(item));

	DispatchNext();

	return true;
}


void CWorkerManagerComp::OnFinish(const IRequest* request, const QByteArray& /*subCommandId*/)
{
	QMutexLocker loc(&m_workItemListMutex);

	delete request;

	// The finished request's subCommandId is deliberately not reused for the next item:
	// each queued item carries its own.
	DispatchNext();
}


void CWorkerManagerComp::OnTaskFinish(const QByteArray& orderingKey)
{
	QMutexLocker loc(&m_workItemListMutex);

	if (!orderingKey.isEmpty()){
		m_busyKeys.remove(orderingKey);
	}

	DispatchNext();
}


int CWorkerManagerComp::FindDispatchableIndex() const
{
	for (int index = 0; index < m_workItemList.count(); ++index){
		const WorkItem& item = m_workItemList.at(index);

		if (!item.IsTask() || item.orderingKey.isEmpty() || !m_busyKeys.contains(item.orderingKey)){
			return index;
		}
	}

	return -1;
}


void CWorkerManagerComp::MarkInFlight(const WorkItem& item) const
{
	if (item.IsTask() && !item.orderingKey.isEmpty()){
		m_busyKeys.insert(item.orderingKey);
	}
}


void CWorkerManagerComp::DispatchNext() const
{
	const int index = FindDispatchableIndex();
	if (index < 0){
		return;
	}

	for (CWorkerThread* workerPtr: m_workerList){
		if (workerPtr->GetStatus() == CWorkerThread::ST_CLOSE){
			WorkItem item = m_workItemList.takeAt(index);

			MarkInFlight(item);

			workerPtr->SetStatus(CWorkerThread::ST_PROCESS);

			if (item.IsTask()){
				workerPtr->PostTask(std::move(item.task), item.orderingKey);
			}
			else{
				workerPtr->PostRequest(item.requestPtr, item.subCommandId);
			}

			return;
		}
	}

	if (m_workerList.count() >= *m_threadsLimitAttrPtr){
		// Pool saturated: the item stays queued until a worker reports back.
		return;
	}

	// Re-prime if the constructor ran before QCoreApplication was fully up
	// (defensive); no-op when already initialized on main.
	imtcom::CRequestSender::InitializeNetworkBackend();

	// Create the servlet on THIS (manager) thread - not inside CWorkerThread::run().
	// A factory's OnComponentCreated() may construct QObjects parented to qApp
	// (translators, timers, etc.), and doing that from a worker logs the one-shot
	// affinity warning. Created before the item is dequeued, so that a failure here
	// leaves the item queued rather than dropping it.
	IRequestServletPtr servletPtr = CreateServlet();
	if (!servletPtr.IsValid()){
		Q_ASSERT(false);

		return;
	}

	WorkItem item = m_workItemList.takeAt(index);

	MarkInFlight(item);

	CWorkerThread* workerPtr = new CWorkerThread(this, item.subCommandId);
	// The worker notifies completion by calling OnFinish / OnTaskFinish via a queued
	// lambda (CWorkerThread::NotifyFinished) - no FinishProcess signal / raw-pointer
	// queued metatype involved.
	workerPtr->SetServlet(std::move(servletPtr));

	m_workerList.append(workerPtr);

	if (item.IsTask()){
		workerPtr->SetPendingTask(std::move(item.task), item.orderingKey);
	}
	else{
		workerPtr->SetRequestPtr(item.requestPtr);
	}

	// Start processing of the work item:
	workerPtr->start();
}


void CWorkerManagerComp::AboutToQuit()
{
	QList<CWorkerThread*> workerList;

	{
		QMutexLocker loc(&m_workItemListMutex);

		m_isShuttingDown = true;

		// Queued tasks are simply dropped; only the request arm owns memory here.
		for (const WorkItem& item: m_workItemList){
			delete item.requestPtr;
		}

		m_workItemList.clear();
		m_busyKeys.clear();

		workerList = m_workerList;

		// Cleared under the lock: these threads are being torn down, so no later dispatch may
		// reach them.
		m_workerList.clear();
	}

	// Joined without the queue mutex held: a worker finishing in the meantime hops to
	// this thread through a queued call, and nothing here may hold the lock it needs.
	for (CWorkerThread* workerPtr: workerList){
		workerPtr->quit();
		workerPtr->wait(1000);
		workerPtr->deleteLater();
	}
}


} // namespace imtrest
