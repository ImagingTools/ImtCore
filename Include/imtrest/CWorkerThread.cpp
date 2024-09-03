#include <imtrest/CWorkerThread.h>

// ImtCore includes
#include <imtrest/CWorkerManagerComp.h>

// Qt includes
#include <QtCore/QDebug>



namespace imtrest
{


CWorkerThread::CWorkerThread(const CWorkerManagerComp* workerManager, const QByteArray& subCommandId)
	:m_status(ST_PROCESS),
	m_workerPtr(nullptr),
	m_subCommandId(subCommandId)
{
	m_workerManager = dynamic_cast<CWorkerManagerComp*>(const_cast<CWorkerManagerComp*>(workerManager));
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
	m_requestServletPtr = m_workerManager->CreateServlet();
	if (m_requestServletPtr == nullptr){
		Q_ASSERT(false);

		return;
	}

	m_workerPtr.SetPtr(new CWorker(m_requestServletPtr, this));

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


