// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtrest/CWorkerManagerComp.h>

// Qt includes
#include <QtCore/QDebug>


namespace imtrest
{


CWorkerManagerComp::CWorkerManagerComp()
{
	connect(
				QCoreApplication::instance(),
				&QCoreApplication::aboutToQuit,
				this,
				&CWorkerManagerComp::AboutToQuit,
				Qt::DirectConnection);
}


IRequestServletPtr CWorkerManagerComp::CreateServlet()
{
	if (m_requestHandlerCompPtr.IsValid()){
		return m_requestHandlerCompPtr.CreateInstance();
	}

	return nullptr;
}


const ISender* CWorkerManagerComp::GetSender(const QByteArray& requestId)
{
	if (!m_requestManagerCompPtr.IsValid()){
		return nullptr;
	}

	return m_requestManagerCompPtr->GetSender(requestId);
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

			Q_EMIT workerPtr->StartProcess(requestPtr, subCommandId);

			return retVal;
		}
	}

	if (m_workerList.count() < *m_threadsLimitAttrPtr){
		CWorkerThread* workerPtr = new CWorkerThread(this, subCommandId);
		connect(workerPtr, &CWorkerThread::FinishProcess, this, &CWorkerManagerComp::OnFinish); //, Qt::QueuedConnection
		m_workerList.append(workerPtr);

		// Pop request from the queue:
		const IRequest* requestPtr = m_requestList.at(0);
		m_requestList.removeAt(0);

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
			Q_EMIT workerPtr->StartProcess(requestPtr, subCommandId);

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


