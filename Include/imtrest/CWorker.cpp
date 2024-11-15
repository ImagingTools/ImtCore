#include <imtrest/CWorker.h>


// ImtCore includes
#include <imtrest/ISender.h>
#include <imtrest/CWorkerThread.h>
#include <imtrest/CWebSocketRequest.h>

// Qt includes
#include <QtCore/QDebug>
#include <QtCore/QJsonDocument>
#include <QtCore/QJsonObject>


namespace imtrest
{


CWorker::CWorker(const imtrest::IRequestServlet* requestServletPtr, CWorkerThread* workerThread)
	:m_requestServletPtr(requestServletPtr),
	m_workerThread(workerThread)
{
}


void CWorker::ProcessRequest(const IRequest* request, const QByteArray& subCommandId)
{
	m_workerThread->SetStatus(CWorkerThread::ST_PROCESS);

	if (m_requestServletPtr != nullptr && request != nullptr){
		QMutexLocker lock(&m_processMutex);

		QByteArray body = request->GetBody();
		if (body.size() > 100){
			body.resize(100);
			body += "...";
		}

		qDebug() << "Start process " << request->GetCommandId() << body;

		ConstResponsePtr responsePtr = m_requestServletPtr->ProcessRequest(*request, subCommandId);
		if (responsePtr.IsValid()){
			const CWebSocketRequest* websocketRequestPrt = dynamic_cast<const CWebSocketRequest*>(request);
			if (websocketRequestPrt != nullptr){
				CWebSocketRequest::MethodType methodType = websocketRequestPrt->GetMethodType();
				QByteArray data = websocketRequestPrt->GetBody();
				QJsonDocument document = QJsonDocument::fromJson(data);
				QJsonObject object = document.object();
				QByteArray body = object.value("payload").toObject().value("data").toString().toUtf8();

				// QString responseData = responseDataModelPtr->ToJson();
				if (!body.isEmpty() && object.value("type").toString() != "data"){
					data = QString(R"({"type": "query_data","id": "%1","payload": %2})")
					.arg(object.value("id").toString()).arg(body).toUtf8();
				}
				// responsePtr->
			}
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


} // namespace imtrest


