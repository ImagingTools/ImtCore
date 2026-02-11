// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtrest/CWorkerThread.h>


// Qt includes
#include <QtCore/QDebug>

// ImtCore includes
#include <imtrest/CWorkerManagerComp.h>


namespace imtrest
{


CWorkerThread::CWorkerThread(const CWorkerManagerComp* workerManager, const QByteArray& subCommandId)
	: m_status(ST_PROCESS),
	  m_workerPtr(nullptr),
	  m_requestPtr(nullptr),
	  m_subCommandId(subCommandId)
{
	m_workerManager = const_cast<CWorkerManagerComp*>(workerManager);
}


CWorkerThread::Status CWorkerThread::GetStatus()
{
	QMutexLocker lock(&m_statusMutex);

	return m_status;
}


void CWorkerThread::SetStatus(Status status)
{
	QMutexLocker lock(&m_statusMutex);

	m_status = status;
}


void CWorkerThread::SetRequestPtr(const IRequest* requestPtr)
{
	m_requestPtr = requestPtr;
}


const ISender* CWorkerThread::GetSender(const QByteArray& requestId)
{
	if (m_workerManager == nullptr){
		return nullptr;
	}

	return m_workerManager->GetSender(requestId);
}


//reimplemented (QThread)

void CWorkerThread::run()
{
	imtrest::IRequestServletPtr requestServletPtr = m_workerManager->CreateServlet();
	if (!requestServletPtr.IsValid()){
		Q_ASSERT(false);

		return;
	}

	m_workerPtr.SetPtr(new CWorker(std::move(requestServletPtr), this));

	connect(this, &CWorkerThread::StartProcess, m_workerPtr.GetPtr(), &CWorker::ProcessRequest); //, Qt::QueuedConnection
	connect(m_workerPtr.GetPtr(), &CWorker::FinishProcess, this, &CWorkerThread::OnFinishProcess, Qt::DirectConnection); //, Qt::QueuedConnection

	m_workerPtr->ProcessRequest(m_requestPtr, m_subCommandId);

	exec();
}


void CWorkerThread::OnStarted()
{
	Q_EMIT StartProcess(m_requestPtr, m_subCommandId);
}


void CWorkerThread::OnFinishProcess(const IRequest* request, const QByteArray& subCommandId)
{
	Q_EMIT FinishProcess(request,subCommandId);
}


} // namespace imtrest


