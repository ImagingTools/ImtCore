#include <imtrest/CWorkerManagerComp.h>


// ImtCore includes
#include <imtrest/IRequest.h>
#include <imtrest/IResponse.h>
#include <imtrest/ISender.h>
#include <imtrest/CMultiThreadServer.h>
#include <imtrest/CHttpRequest.h>


namespace imtrest
{


// CWorker immlemented
CWorker::CWorker(const imtrest::IRequestServlet* requestServletPtr, CWorkerThread* workerThread)
			:m_requestServletPtr(requestServletPtr),
			  m_workerThread(workerThread)
{

}


void CWorker::ProcessRequest(const IRequest* request)
{
	m_workerThread->SetStatus(CWorkerThread::ST_PROCESS);

	if (m_requestServletPtr != nullptr && request != nullptr){
		qptrdiff threadId = (qptrdiff)QThread::currentThreadId();
		qDebug() << QString("CWorker GetConnectionName - %1").arg(threadId);

		ConstResponsePtr ResponsePtr = m_requestServletPtr->ProcessRequest(*request);
		if (ResponsePtr.IsValid()){
			const ISender* sender = m_workerThread->GetSender(request->GetRequestId());
			if (sender != nullptr){
				sender->SendResponse(ResponsePtr);
			}
			qDebug() << "SendResponse" << "Request:" << request->GetCommandId() << "Body:" << request->GetBody();
		}
		else{
			qDebug() << "Request result invalid !!!" ;
		}
	}
	m_workerThread->SetStatus(CWorkerThread::ST_CLOSE);
	emit FinishProcess(request);
}




// CWorkerThread immlemented
CWorkerThread::CWorkerThread(const CWorkerManagerComp* workerManager)
	: m_status(ST_PROCESS),
	  m_workerPtr(nullptr)
{
	  m_workerManager = dynamic_cast<CWorkerManagerComp*>(const_cast<CWorkerManagerComp*>(workerManager));
}


CWorkerThread::Status CWorkerThread::GetStatus()
{
	QMutexLocker loc(&m_statusMutex);
	return m_status;
}


void CWorkerThread::SetStatus(Status status)
{
	QMutexLocker loc(&m_statusMutex);
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


void CWorkerThread::run()
{
	m_requestServletPtr = m_workerManager->CreateServlet();
	m_workerPtr.SetPtr(new CWorker(m_requestServletPtr, this));
	connect(this, &CWorkerThread::StartProcess, m_workerPtr.GetPtr(), &CWorker::ProcessRequest); //, Qt::QueuedConnection
	connect(m_workerPtr.GetPtr(), &CWorker::FinishProcess, this, &CWorkerThread::onFinishProcess, Qt::DirectConnection); //, Qt::QueuedConnection
	qptrdiff threadId = (qptrdiff)QThread::currentThreadId();
	qDebug() << QString("CWorker run GetConnectionName - %1").arg(threadId);
	m_workerPtr->ProcessRequest(m_request);
	exec();
}


void CWorkerThread::onStarted()
{
	emit StartProcess(m_request);
}


void CWorkerThread::onFinishProcess(const IRequest* request)
{
	emit FinishProcess(request);
}



CWorkerManagerComp::CWorkerManagerComp()
{
	connect(QCoreApplication::instance(), &QCoreApplication::aboutToQuit, this, &CWorkerManagerComp::AboutToQuit, Qt::DirectConnection);
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
			qDebug() << m_workerList.count()  << "StartProcess1" << "Request:" << request.GetRequestId() << requestPtr->GetCommandId() << "Body:" << request.GetBody();

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
		qDebug() << m_workerList.count() << "StartProcess2" << "Request:" << request.GetRequestId() << request.GetCommandId() << requestPtr->GetCommandId() << "Body:" << request.GetBody();
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
	 qDebug() << "FinishProcess" << "Request:" << request->GetRequestId();
	 delete request;
	 
	 if (m_requestList.isEmpty()){
		 return;
	 }

	 for (CWorkerThread* workerPtr: m_workerList){
		 if (workerPtr->GetStatus() == CWorkerThread::ST_CLOSE){
			 const IRequest* requestPtr = m_requestList.at(0);
			 m_requestList.removeAt(0);
			 workerPtr->SetStatus(CWorkerThread::ST_PROCESS);
			 emit workerPtr->StartProcess(requestPtr);
			 qDebug() << "StartProcess3" << "Request:" << requestPtr->GetCommandId() << "Body:" << requestPtr->GetBody();

			 return;
		 }
	 }
 }


 void CWorkerManagerComp::AboutToQuit()
 {
	 for (CWorkerThread* workerPtr: m_workerList){
		 workerPtr->quit();
		 workerPtr->wait(1000);
		 workerPtr->deleteLater();
	 }

	 qDeleteAll(m_requestList);
	 m_requestList.clear();
 }



} // namespace imtrest


