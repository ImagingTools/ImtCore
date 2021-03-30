#include <imtrest/CHttpRootHandlerComp.h>


// ImtCore includes
#include <imtrest/IRequest.h>
#include <imtrest/IResponder.h>
#include <imtrest/IResponse.h>
#include <imtrest/IProtocolEngine.h>


namespace imtrest
{


// public methods

// reimplemented (IProtocolEngine)

const imtrest::IResponse* CHttpRootHandlerComp::ProcessRequest(const IRequest& request) const
{
	const IProtocolEngine& engine = request.GetProtocolEngine();

	QByteArray commandId = request.GetCommandId();
	if (commandId.startsWith('/'))
	{
		commandId = commandId.remove(0, 1);
	}

	if (commandId.isEmpty()){
		QByteArray body = QByteArray("<html><head><title>Error</title></head><body><p>The requested command could not be executed</p></body></html>");
		QByteArray reponseTypeId = QByteArray("text/html; charset=utf-8");

		istd::TDelPtr<IResponse> responsePtr(engine.CreateResponse(request, IProtocolEngine::SC_OPERATION_NOT_AVAILABLE, body, reponseTypeId));
        if (responsePtr.IsValid()){

            return nullptr;
		}
	}
	const IRequestHandler* handlerPtr = FindRequestHandler(commandId);
	if (handlerPtr != nullptr){
		return handlerPtr->ProcessRequest(request);
	}
	else{
        QByteArray body = QByteArray("<html><head><title>Error</title></head><body><p>The requested command could not be executed</p></body></html>");
        QByteArray reponseTypeId = QByteArray("text/html; charset=utf-8");

        istd::TDelPtr<IResponse> responsePtr(engine.CreateResponse(request, IProtocolEngine::SC_OPERATION_NOT_AVAILABLE, body, reponseTypeId));
        if (responsePtr.IsValid()){
            engine.GetResponder().SendResponse(*responsePtr);
        }
		SendErrorMessage(0, QString("No request handler found for: '%1'").arg(qPrintable(commandId)));
	}

	return false;
}


// protected methods


IRequestHandler* CHttpRootHandlerComp::FindRequestHandler(const QByteArray& commandId) const
{
//    int handlersCount = qMin(m_commandIdsAttrPtr.GetCount(), m_requestHandlersCompPtr.GetCount());
    for (int i = 0; i < m_requestHandlersCompPtr.GetCount(); ++i)
    {
        IRequestHandler* handlerPtr = m_requestHandlersCompPtr[i];
        if (handlerPtr && handlerPtr->GetSupportedCommandId() == commandId)
        {
            return handlerPtr;
        }
    }
//    for(auto handlerPtr: m_requestHandlersCompPtr)
//    {
//        if (handlerPtr && handlerPtr->GetSupportedCommandId() == commandId)
//        {
//            return handlerPtr;
//        }
//    }
	return nullptr;
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


