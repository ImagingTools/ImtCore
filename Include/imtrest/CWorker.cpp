// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtrest/CWorker.h>


// ImtCore includes
#include <imtrest/ISender.h>
#include <imtrest/CWorkerThread.h>
#include <imtrest/CWebSocketRequest.h>
#include <imtclientgql/CWebSocketClientComp.h>

// Qt includes
#include <QtCore/QDebug>
#include <QtCore/QJsonDocument>
#include <QtCore/QJsonObject>


namespace imtrest
{


CWorker::CWorker(imtrest::IRequestServletPtr&& requestServletPtr, CWorkerThread* workerThread)
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


} // namespace imtrest


