#include <imtrest/CHttpRootServletComp.h>


// ImtCore includes
#include <imtrest/IRequest.h>
#include <imtrest/ISender.h>
#include <imtrest/IResponse.h>
#include <imtrest/IProtocolEngine.h>


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

	const IProtocolEngine& engine = request.GetProtocolEngine();

	const IRequestServlet* handlerPtr = FindRequestHandler(commandId);
	if (handlerPtr != nullptr){
		return handlerPtr->ProcessRequest(request);
	}
	else if (commandId.isEmpty()){
		QByteArray body = QByteArray("<html><head><title>Error</title></head><body><p>Empty command-ID</p></body></html>");
		QByteArray reponseTypeId = QByteArray("text/html; charset=utf-8");

		ConstResponsePtr responsePtr(engine.CreateResponse(request, IProtocolEngine::SC_OPERATION_NOT_AVAILABLE, body, reponseTypeId));

		return responsePtr;
	}
	else{
		QByteArray commandIdSafe = commandId.replace("<", "<<");
		commandIdSafe = commandId.replace(">", ">>");
		commandIdSafe = commandId.replace("/", "//");
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
	QMutexLocker lockMap(&m_handlersMapMutex);

	if (m_handlersMap.contains(commandId)){
		return m_handlersMap[commandId];
	}

	QByteArrayList handlerIds = m_handlersMap.keys();

	for (const QByteArray& handlerId : handlerIds){
		if (handlerId.endsWith("*")){
			QByteArray cleanedId = handlerId;
			cleanedId.remove(cleanedId.length() - 1, 1);

			if (commandId.startsWith(cleanedId)){
				return m_handlersMap[handlerId];
			}
		}
	}

	return nullptr;
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

			if ((handlerPtr != nullptr) && !registeredCommandId.isEmpty()){
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


