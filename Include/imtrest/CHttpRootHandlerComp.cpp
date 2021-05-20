#include <imtrest/CHttpRootHandlerComp.h>


// ImtCore includes
#include <imtrest/IRequest.h>
#include <imtrest/IResponder.h>
#include <imtrest/IResponse.h>
#include <imtrest/IProtocolEngine.h>


namespace imtrest
{


// public methods

// reimplemented (IRequestHandler)

IRequestHandler::ConstResponsePtr CHttpRootHandlerComp::ProcessRequest(const IRequest& request) const
{
	QByteArray commandId = request.GetCommandId();
	if (commandId.startsWith('/')){
		commandId = commandId.remove(0, 1);
	}

	if (commandId.endsWith('/')){
		commandId = commandId.remove(commandId.length() - 1, 1);
	}

	const IProtocolEngine& engine = request.GetProtocolEngine();

	const IRequestHandler* handlerPtr = FindRequestHandler(commandId);
	if (handlerPtr != nullptr) {
		return handlerPtr->ProcessRequest(request);
	}
	else if (commandId.isEmpty()){
		QByteArray body = QByteArray("<html><head><title>Error</title></head><body><p>Empty command-ID</p></body></html>");
		QByteArray reponseTypeId = QByteArray("text/html; charset=utf-8");

		ConstResponsePtr responsePtr(engine.CreateResponse(request, IProtocolEngine::SC_OPERATION_NOT_AVAILABLE, body, reponseTypeId));

		return responsePtr;
	}
	else{
		QByteArray body = QString("<html><head><title>Error</title></head><body><p>The requested command could not be executed. No servlet was found for the given command: '%1'</p></body></html>").arg(qPrintable(commandId)).toUtf8();
		QByteArray reponseTypeId = QByteArray("text/html; charset=utf-8");

		ConstResponsePtr responsePtr(engine.CreateResponse(request, IProtocolEngine::SC_OPERATION_NOT_AVAILABLE, body, reponseTypeId));
		if (responsePtr.IsValid()) {
			engine.GetResponder().SendResponse(*responsePtr);
		}

		SendErrorMessage(0, QString("No request handler found for: '%1'").arg(qPrintable(commandId)));

		return responsePtr;
	}

	return ConstResponsePtr();
}


// protected methods

IRequestHandler* CHttpRootHandlerComp::FindRequestHandler(const QByteArray& commandId) const
{
	/// contains an IRequestHandler pointer in which the commandID is exactly the same as the request (highest priority) 
	/// \warning This pointer MUST be returned if is not null!
	IRequestHandler* exactsCommandIdHandler = nullptr;

	/// contains an IRequestHandler pointer in which the commandID is exactly the same as the request (highest priority) 
	/// \warning This pointer chould be returned ONLY exactsCommandIdHandler is null!
	IRequestHandler* startsCommandIdHandler = nullptr;

	for (int i = 0; i < m_requestHandlersCompPtr.GetCount(); ++i) {
		IRequestHandler* handlerPtr = m_requestHandlersCompPtr[i];
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

void CHttpRootHandlerComp::OnComponentCreated()
{
	BaseClass::OnComponentCreated();

	{
		QMutexLocker lockHandlerMap(&m_handlersMapMutex);
		m_handlersMap.clear();

		int handlersCount = qMin(m_commandIdsAttrPtr.GetCount(), m_requestHandlersCompPtr.GetCount());
		
		for (int i = 0; i < handlersCount; ++i){
			QString registeredCommandId = m_commandIdsAttrPtr[i];
			IRequestHandler* handlerPtr = m_requestHandlersCompPtr[i];

			if ((handlerPtr != nullptr) && !registeredCommandId.isEmpty()){
				m_handlersMap[registeredCommandId] = handlerPtr;
			}
		}
	}
}


QByteArray CHttpRootHandlerComp::GetSupportedCommandId() const
{
	return "/";
}


} // namespace imtrest


