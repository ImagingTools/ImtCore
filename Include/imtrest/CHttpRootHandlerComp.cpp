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

bool CHttpRootHandlerComp::ProcessRequest(const IRequest& request) const
{
	const IProtocolEngine& engine = request.GetProtocolEngine();

	QByteArray commandId = request.GetCommandId();
	if (commandId.isEmpty()){
		QByteArray body = QByteArray("<html><head><title>Error</title></head><body><p>The requested command could not be executed</p></body></html>");
		QByteArray reponseTypeId = QByteArray("text/html; charset=utf-8");

		istd::TDelPtr<IResponse> responsePtr(engine.CreateResponse(request, IProtocolEngine::SC_OPERATION_NOT_AVAILABLE, body, reponseTypeId));
		if (responsePtr.IsValid()){
			engine.GetResponder().SendResponse(*responsePtr);

			return false;
		}
	}

	const IRequestHandler* handlerPtr = FindRequestHandler(commandId);
	if (handlerPtr != nullptr){
		return handlerPtr->ProcessRequest(request);
	}
	else{
		SendErrorMessage(0, QString("No request handler found for: '%1'").arg(qPrintable(commandId)));
	}

	return false;
}


// protected methods

IRequestHandler* CHttpRootHandlerComp::FindRequestHandler(const QByteArray& /*commandId*/) const
{
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


} // namespace imtrest


