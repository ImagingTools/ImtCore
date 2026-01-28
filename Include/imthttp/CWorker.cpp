#include <imthttp/CWorker.h>


// ImtCore includes
#include <imthttp/ISender.h>
#include <imthttp/CWorkerThread.h>
#include <imthttp/CWebSocketRequest.h>
#include <imtclientgql/CWebSocketClientComp.h>

// Qt includes
#include <QtCore/QDebug>
#include <QtCore/QJsonDocument>
#include <QtCore/QJsonObject>


namespace imthttp
{


CWorker::CWorker(imthttp::IRequestServletPtr&& requestServletPtr, CWorkerThread* workerThread)
	:m_requestServletPtr(std::move(requestServletPtr)),
	m_workerThread(workerThread)
{
}


void CWorker::ProcessRequest(const IRequest* request, const QByteArray& subCommandId)
{
	m_workerThread->SetStatus(CWorkerThread::ST_PROCESS);

	if (m_requestServletPtr.IsValid() && request != nullptr){
		QMutexLocker lock(&m_processMutex);

		QByteArray body = request->GetBody();
		if (body.size() > 100){
			body.resize(100);
			body += "...";
		}

		qDebug() << "Start process " << request->GetCommandId() << body;

		ConstResponsePtr responsePtr = m_requestServletPtr->ProcessRequest(*request, subCommandId);
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

	Q_EMIT FinishProcess(request, subCommandId);
}


} // namespace imthttp


