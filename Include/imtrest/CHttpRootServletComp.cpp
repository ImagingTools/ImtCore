#include <imtrest/CHttpRootServletComp.h>


// ImtCore includes
#include <imtrest/IRequest.h>
#include <imtrest/ISender.h>
#include <imtrest/IResponse.h>
#include <imtrest/IProtocolEngine.h>
#include <imtrest/CHttpResponse.h>
#include <imtrest/CHttpRequest.h>

// Qt i ncludes
#include <QtCore/QDataStream>

#include "zlib.h"


namespace imtrest
{


// public methods

// reimplemented (IRequestHandler)


IRequestServlet::ConstResponsePtr CHttpRootServletComp::ProcessRequest(const IRequest& request) const
{
	QByteArray commandId = request.GetCommandId();
	if (commandId.startsWith('/')){
		commandId = commandId.remove(0, 1);
	}

	if (commandId.endsWith('/')){
		commandId = commandId.remove(commandId.length() - 1, 1);
	}

	QByteArray bodyRequest = request.GetBody();

	const IProtocolEngine& engine = request.GetProtocolEngine();

	const IRequestServlet* handlerPtr = FindRequestHandler(commandId);
	if (handlerPtr != nullptr){
		ConstResponsePtr responsePtr = handlerPtr->ProcessRequest(request);
		const CHttpResponse* httpResponseConstPtr = dynamic_cast<const CHttpResponse*>(responsePtr.GetPtr());
		CHttpResponse* httpResponsePtr = dynamic_cast<CHttpResponse*>(const_cast<CHttpResponse*>(httpResponseConstPtr));

		if (m_autoCompressionAttrPtr->GetValue() == true && httpResponsePtr != nullptr){
			EncodingType encodingType = ET_NONE;
			const CHttpRequest* httprequestPtr = dynamic_cast<const CHttpRequest*>(&request);
			if (httprequestPtr != nullptr){
				QByteArrayList headers = httprequestPtr->GetHeaders();
				QByteArray encodingHeader = httprequestPtr->GetHeaderValue("Accept-Encoding");
				if (encodingHeader.contains("deflate")){
					encodingType = ET_DEFLATE;
				}
				else{
					if (encodingHeader.contains("gzip")){
						encodingType = ET_GZIP;
					}
				}

			}
			if (encodingType == ET_DEFLATE){
				QByteArray data = responsePtr->GetData();
				QByteArray qData = qCompress(data, 8);
				qData.remove(0, 4);
				httpResponsePtr->SetData(qData);
				IResponse::Headers headers = responsePtr->GetHeaders();
				headers.insert("Content-Encoding", "deflate");
				httpResponsePtr->SetHeaders(headers);
			}
			if (encodingType == ET_GZIP){
				QByteArray data = responsePtr->GetData();
				QByteArray qData = qCompress(data, 8);
				qData.remove(0, 6);
				qData.remove(qData.count() - 4, 4);
				int crc = (int)crc32(0, (unsigned char *)qData.data(), qData.count());
				const char gzipheader[] = {
						 0x1f, static_cast<char>(0x8b)      // gzip magic number
						 , 8             // compress method "defalte"
						 , 1             // text data
						 , 0, 0, 0, 0    // timestamp is not set
						 , 2             // maximum compression flag
						 , static_cast<char>(255)           // unknown OS
					 };
				QByteArray outData;
				QDataStream datastream(&outData, QIODevice::WriteOnly);
				datastream.device()->write(gzipheader,10);
				datastream.device()->write(qData);
				datastream << crc;
				datastream << qData.count();
				httpResponsePtr->SetData(outData);
				IResponse::Headers headers = responsePtr->GetHeaders();
				headers.insert("Content-Encoding", "gzip");
				httpResponsePtr->SetHeaders(headers);
			}

		}
		return responsePtr;
	}
	else if (commandId.isEmpty()){
		QByteArray body = QByteArray("<html><head><title>Error</title></head><body><p>Empty command-ID</p></body></html>");
		QByteArray reponseTypeId = QByteArray("text/html; charset=utf-8");

		ConstResponsePtr responsePtr(engine.CreateResponse(request, IProtocolEngine::SC_OPERATION_NOT_AVAILABLE, body, reponseTypeId));

		return responsePtr;
	}
	else{
		QByteArray commandIdSafe = commandId.replace("<","<<");
		commandIdSafe = commandId.replace(">",">>");
		commandIdSafe = commandId.replace("/","//");
		QByteArray body = QString("<html><head><title>Error</title></head><body><p>The requested command could not be executed. No servlet was found for the given command: '%1'</p></body></html>").arg(qPrintable(commandIdSafe)).toUtf8();
		QByteArray reponseTypeId = QByteArray("text/html; charset=utf-8");

		ConstResponsePtr responsePtr(engine.CreateResponse(request, IProtocolEngine::SC_OPERATION_NOT_AVAILABLE, body, reponseTypeId));
		if (responsePtr.IsValid()){
			engine.GetSender().SendResponse(*responsePtr);
		}

		SendErrorMessage(0, QString("No request handler found for: '%1'").arg(qPrintable(commandId)));

		return responsePtr;
	}

	return ConstResponsePtr();
}


// protected methods

IRequestServlet* CHttpRootServletComp::FindRequestHandler(const QByteArray& commandId) const
{
	/// contains an IRequestHandler pointer in which the commandID is exactly the same as the request (highest priority)
	/// \warning This pointer MUST be returned if is not null!
	IRequestServlet* exactsCommandIdHandler = nullptr;

	/// contains an IRequestHandler pointer in which the commandID is exactly the same as the request (highest priority)
	/// \warning This pointer chould be returned ONLY exactsCommandIdHandler is null!
	IRequestServlet* startsCommandIdHandler = nullptr;

	for (int i = 0; i < m_requestHandlersCompPtr.GetCount(); ++i){
		IRequestServlet* handlerPtr = m_requestHandlersCompPtr[i];
		if (i > m_commandIdsAttrPtr.GetCount() - 1){
			break;
		}
		if (handlerPtr != nullptr){
			//			QByteArray handlersPtrSupportedCommandId = handlerPtr->GetSupportedCommandId();
			QByteArray handlersPtrSupportedCommandId = m_commandIdsAttrPtr[i];
			if (handlersPtrSupportedCommandId == commandId)
			{
				exactsCommandIdHandler = handlerPtr;
				startsCommandIdHandler = nullptr;
				break;
			}

			if
					(handlersPtrSupportedCommandId.endsWith("*") &&
					 commandId.startsWith(handlersPtrSupportedCommandId.remove(handlersPtrSupportedCommandId.length() - 1, 1))
					 )
			{
				startsCommandIdHandler = handlerPtr;
			}

		}
	}
	return exactsCommandIdHandler == nullptr ? startsCommandIdHandler : exactsCommandIdHandler;
}


// reimplemented (icomp::CComponentBase)

void CHttpRootServletComp::OnComponentCreated()
{
	BaseClass::OnComponentCreated();

	{
		QMutexLocker lockHandlerMap(&m_handlersMapMutex);
		m_handlersMap.clear();

		int handlersCount = qMin(m_commandIdsAttrPtr.GetCount(), m_requestHandlersCompPtr.GetCount());
		
		for (int i = 0; i < handlersCount; ++i){
			QByteArray registeredCommandId = m_commandIdsAttrPtr[i];
			IRequestServlet* handlerPtr = m_requestHandlersCompPtr[i];

			if ((handlerPtr != nullptr)){
				m_handlersMap[registeredCommandId] = handlerPtr;
			}
		}
	}
}


QByteArray CHttpRootServletComp::GetSupportedCommandId() const
{
	return "/";
}


} // namespace imtrest


