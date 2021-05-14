#include <imtrest/CServerCompBase.h>


// ImtCore includes
#include <imtrest/IResponder.h>


namespace imtrest
{


// public methods

// reimplemented (IRequestHandler)

IRequestHandler::ConstResponsePtr CServerCompBase::ProcessRequest(const IRequest& request) const
{
	ConstResponsePtr retVal;

	if (m_requestHandlerCompPtr.IsValid()){
		retVal = m_requestHandlerCompPtr->ProcessRequest(request);
		if (retVal.IsValid()){
			request.GetProtocolEngine().GetResponder().SendResponse(*retVal);
		}
	}

	return retVal;
}


QByteArray CServerCompBase::GetSupportedCommandId() const
{
	return QByteArray();
}


// protected methods

// reimplemented (icomp::CComponentBase)

void CServerCompBase::OnComponentCreated()
{
	BaseClass::OnComponentCreated();

	if (m_protocolEngineCompPtr.IsValid() && m_requestHandlerCompPtr.IsValid()){
		if (m_startServerOnCreateAttrPtr.IsValid() && *m_startServerOnCreateAttrPtr){
			if (m_serverAddressAttrPtr.IsValid() && m_serverPortAttrPtr.IsValid()){
				StartListening(QHostAddress((*m_serverAddressAttrPtr).toStdString().c_str()), *m_serverPortAttrPtr);
			}
			else{
				StartListening();
			}
		}
	}
}


// private slots

void CServerCompBase::HandleNewConnections()
{
	QList<QObject*> sockets = CreateConnectionSockets(sender());

	for (QObject* socketPtr : sockets){
		IRequest* newRequestPtr = m_protocolEngineCompPtr->CreateRequest(socketPtr, *this);
		if (newRequestPtr != nullptr){
			RegisterSocket(socketPtr);

			m_requests.PushBack(newRequestPtr);
		}
	}
}


void CServerCompBase::OnSocketDisconnected()
{
	QObject* socketObjectPtr = sender();
	Q_ASSERT(socketObjectPtr != nullptr);

	for (int i = 0; i < m_requests.GetCount(); ++i){
		IRequest* requestPtr = m_requests.GetAt(i);
		Q_ASSERT(requestPtr != nullptr);

		if (&requestPtr->GetSocketObject() == socketObjectPtr){
			m_requests.RemoveAt(i);

			qDebug("Request deleted");

			break;
		}
	}

	socketObjectPtr->deleteLater();
}


} // namespace imtrest


