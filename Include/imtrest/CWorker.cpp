// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtrest/CWorker.h>


// ImtCore includes
#include <imtrest/ITransport.h>
#include <imtrest/CConnectionTimeoutBehavior.h>
#include <imtrest/CWorkerThread.h>
#include <imtrest/CWebSocketRequest.h>
#include <imtrest/IProtocolEngine.h>
#include <imtclientgql/CWebSocketClientComp.h>

// Qt includes
#include <QtCore/QDebug>
#include <QtCore/QJsonDocument>
#include <QtCore/QJsonObject>

// STL includes
#include <exception>


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

	if (request != nullptr){
		ConstResponsePtr responsePtr;

		{
			QMutexLocker lock(&m_processMutex);

			QByteArray body = request->GetBody();
			if (body.size() > 100){
				body.resize(100);
				body += "...";
			}

			qDebug() << "Start process " << request->GetCommandId() << body;

			responsePtr = ExecuteServlet(m_requestServletPtr.GetPtr(), *request, subCommandId);
		}

		if (responsePtr.IsValid()){
			m_workerThread->SendResponse(request->GetRequestId(), responsePtr);
		}
	}

	// The worker is free again whatever the servlet did
	m_workerThread->SetStatus(CWorkerThread::ST_CLOSE);

	m_workerThread->NotifyFinished(request, subCommandId);
}


// public static methods

ConstResponsePtr CWorker::ExecuteServlet(const IRequestServlet* servletPtr, const IRequest& request, const QByteArray& subCommandId)
{
	if (servletPtr == nullptr){
		qWarning() << "There is no servlet for the request" << request.GetCommandId();

		return CreateInternalErrorResponse(request);
	}

	try{
		ConstResponsePtr responsePtr = servletPtr->ProcessRequest(request, subCommandId);
		if (responsePtr.IsValid()){
			return responsePtr;
		}

		qWarning() << "The servlet returned no response for the request" << request.GetCommandId();
	}
	catch (const std::exception& error){
		qWarning() << "The servlet failed on the request" << request.GetCommandId() << error.what();
	}
	catch (...){
		qWarning() << "The servlet failed on the request" << request.GetCommandId();
	}

	return CreateInternalErrorResponse(request);
}


ConstResponsePtr CWorker::CreateInternalErrorResponse(const IRequest& request)
{
	const IProtocolEngine& engine = request.GetProtocolEngine();

	// Messages of a channel (TCP, WebSocket) can have no response or many, only a request of HTTP always has one
	if (!CConnectionTimeoutBehavior::CreateForProtocol(engine.GetProtocolTypeId()).IsRequestResponseCycle()){
		return {};
	}

	return ConstResponsePtr(engine.CreateResponse(request, IProtocolEngine::SC_INTERNAL_SERVER_ERROR, QByteArray(), QByteArray()).PopInterfacePtr());
}


} // namespace imtrest


