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
	QMutexLocker loc(&m_requestListMutex);

	ConstResponsePtr retVal;

	m_requestList << &request;

	for (CWorkerThread* workerPtr: m_workerList){
		if (workerPtr->GetStatus() == CWorkerThread::ST_CLOSE){
			const IRequest* requestPtr = m_requestList.at(0);

			m_requestList.removeAt(0);

			workerPtr->SetStatus(CWorkerThread::ST_PROCESS);

			workerPtr->PostRequest(requestPtr, subCommandId);

			return retVal;
		}
	}

	if (m_workerList.count() < *m_threadsLimitAttrPtr){
		// Re-prime if constructor ran before QCoreApplication was fully up
		// (defensive); no-op when already initialized on main.
		imtcom::CRequestSender::InitializeNetworkBackend();

		CWorkerThread* workerPtr = new CWorkerThread(this, subCommandId);
		// The worker notifies completion by calling OnFinish via a queued lambda
		// (CWorkerThread::NotifyFinished) - no FinishProcess signal / raw-pointer
		// queued metatype involved.
		m_workerList.append(workerPtr);

		// Pop request from the queue:
		const IRequest* requestPtr = m_requestList.at(0);
		m_requestList.removeAt(0);

		// Create servlet on THIS (manager/app) thread — not inside CWorkerThread::run().
		IRequestServletPtr servletPtr = CreateServlet();
		if (!servletPtr.IsValid()){
			Q_ASSERT(false);
			m_workerList.removeOne(workerPtr);
			delete workerPtr;
			return retVal;
		}
		workerPtr->SetServlet(std::move(servletPtr));

		// Set popped request to the worker thread:
		workerPtr->SetRequestPtr(requestPtr);

		// Start processing of the request:
		workerPtr->start();
	}

	return retVal;
}


bool CWorkerManagerComp::IsCommandSupported(const QByteArray& /*commandId*/) const
{
	return true;
}


void CWorkerManagerComp::OnFinish(const IRequest* request, const QByteArray& subCommandId)
{
	QMutexLocker loc(&m_requestListMutex);

	delete request;

	if (m_requestList.isEmpty()){
		return;
	}

	for (CWorkerThread* workerPtr : m_workerList){
		if (workerPtr->GetStatus() == CWorkerThread::ST_CLOSE){
			const IRequest* requestPtr = m_requestList.at(0);
			m_requestList.removeAt(0);

			workerPtr->SetStatus(CWorkerThread::ST_PROCESS);
			workerPtr->PostRequest(requestPtr, subCommandId);

			return;
		}
	}
}


void CWorkerManagerComp::AboutToQuit()
{
	for (CWorkerThread* workerPtr : m_workerList){
		workerPtr->quit();
		workerPtr->wait(1000);
		workerPtr->deleteLater();
	}

	qDeleteAll(m_requestList);
	m_requestList.clear();
}


} // namespace imtrest


