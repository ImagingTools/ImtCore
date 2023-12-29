#include <imtrest/CWorkerManagerComp.h>


// ImtCore includes
#include <imtrest/IRequest.h>
#include <imtrest/IResponse.h>
#include <imtrest/ISender.h>

// Qt includes
#include <QtCore/QDebug>


namespace imtrest
{


CWorker::CWorker(const imtrest::IRequestServlet* requestServletPtr, CWorkerThread* workerThread)
	:m_requestServletPtr(requestServletPtr),
	m_workerThread(workerThread)
{
}


void CWorker::ProcessRequest(const IRequest* request)
{
	m_workerThread->SetStatus(CWorkerThread::ST_PROCESS);

	if (m_requestServletPtr != nullptr && request != nullptr){
		QByteArray body = request->GetBody();
		if (body.size() > 100){
			body.resize(100);
			body += "...";
		}
		qDebug() << "Start process " << request->GetCommandId() << body;
		ConstResponsePtr responsePtr = m_requestServletPtr->ProcessRequest(*request);
		if (responsePtr.IsValid()){
			const ISender* sender = m_workerThread->GetSender(request->GetRequestId());
			if (sender != nullptr){
				sender->SendResponse(responsePtr);
			}
		}
		else{
			Q_ASSERT_X(false, __FILE__, "Request result invalid");
		}
	}

	m_workerThread->SetStatus(CWorkerThread::ST_CLOSE);

	Q_EMIT FinishProcess(request);
}


CWorkerThread::CWorkerThread(const CWorkerManagerComp* workerManager)
	:m_status(ST_PROCESS),
	m_workerPtr(nullptr)
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


void CWorkerThread::SetRequest(const IRequest* request)
{
	m_request = request;
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

	m_workerPtr->ProcessRequest(m_request);

	exec();
}


void CWorkerThread::OnStarted()
{
	Q_EMIT StartProcess(m_request);
}


void CWorkerThread::OnFinishProcess(const IRequest* request)
{
	Q_EMIT FinishProcess(request);
}


CWorkerManagerComp::CWorkerManagerComp()
{
	connect(
				QCoreApplication::instance(),
				&QCoreApplication::aboutToQuit,
				this,
				&CWorkerManagerComp::AboutToQuit,
				Qt::DirectConnection);
}


IRequestServlet* CWorkerManagerComp::CreateServlet()
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

ConstResponsePtr CWorkerManagerComp::ProcessRequest(const IRequest& request) const
{
	QMutexLocker loc(&m_requestListMutex);

	ConstResponsePtr retVal;

	m_requestList << &request;

	for (CWorkerThread* workerPtr: m_workerList){
		if (workerPtr->GetStatus() == CWorkerThread::ST_CLOSE){
			const IRequest* requestPtr = m_requestList.at(0);

			m_requestList.removeAt(0);

			workerPtr->SetStatus(CWorkerThread::ST_PROCESS);

			emit workerPtr->StartProcess(requestPtr);

			return retVal;
		}
	}

	if (m_workerList.count() < *m_threadsLimitAttrPtr){
		CWorkerThread* workerPtr = new CWorkerThread(this);
		connect(workerPtr, &CWorkerThread::FinishProcess, this, &CWorkerManagerComp::OnFinish); //, Qt::QueuedConnection
		m_workerList.append(workerPtr);

		const IRequest* requestPtr = m_requestList.at(0);
		m_requestList.removeAt(0);

		workerPtr->SetRequest(requestPtr);

		workerPtr->start();
	}

	return retVal;
}


QByteArray CWorkerManagerComp::GetSupportedCommandId() const
{
	return QByteArray();
}


void CWorkerManagerComp::OnFinish(const IRequest* request)
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
			emit workerPtr->StartProcess(requestPtr);

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


