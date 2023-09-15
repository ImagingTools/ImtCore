#include <imtrest/CTcpServerComp.h>


// ImtCore includes
#include <imtrest/IRequest.h>
#include <imtrest/IResponse.h>
#include <imtrest/ISender.h>
#include <imtrest/CMultiThreadServer.h>


namespace imtrest
{


// public methods

CTcpServerComp::CTcpServerComp(): m_server(new CMultiThreadServer(this))
{

}


CTcpServerComp::~CTcpServerComp()
{
	delete m_server.PopPtr();
}


imtrest::IRequestServlet* CTcpServerComp::GetRequestServlet()
{
	if (!m_requestHandlerCompPtr.IsValid()){
		return nullptr;
	}

    return m_requestHandlerCompPtr.GetPtr();
}


imtrest::IProtocolEngine* CTcpServerComp::GetProtocolEngine()
{
	if (!m_protocolEngineCompPtr.IsValid()){
		return nullptr;
	}

    return m_protocolEngineCompPtr.GetPtr();
}


int CTcpServerComp::GetThreadsLimit()
{
	return *m_threadsLimitAttrPtr;
}


// protected methods

// reimplemented (ibase::TRuntimeStatusHanderCompWrap)

void CTcpServerComp::OnSystemShutdown()
{
}


// reimplemented (icomp::CComponentBase)

void CTcpServerComp::OnComponentCreated()
{
	BaseClass::OnComponentCreated();
	if (m_startServerOnCreateAttrPtr.IsValid() && *m_startServerOnCreateAttrPtr){
		if (m_serverAddressAttrPtr.IsValid() && m_serverPortCompPtr.IsValid()){
			QString port = m_serverPortCompPtr->GetText();
			StartListening(QHostAddress((*m_serverAddressAttrPtr).toStdString().c_str()), port.toInt());
		}
		else if (m_serverAddressAttrPtr.IsValid() && m_serverPortAttrPtr.IsValid()){
			StartListening(QHostAddress((*m_serverAddressAttrPtr).toStdString().c_str()), *m_serverPortAttrPtr);
		}
		else{
			StartListening();
		}
	}
}


const IRequest* CTcpServerComp::GetRequest(const QByteArray& requestId) const
{
	const IRequest* retVal = m_server->GetRequest(requestId);

	return retVal;
}


const ISender* CTcpServerComp::GetSender(const QByteArray& requestId) const
{
	const ISender* retVal = m_server->GetSender(requestId);

	return retVal;
}


// private methods

bool CTcpServerComp::StartListening(const QHostAddress &address, quint16 port)
{
	if (m_server->listen(address, port)){
		SendInfoMessage(0, QString("Server successfully started on %1:%2").arg(address.toString()).arg(port));
		qDebug() << QString("Server successfully started on %1:%2").arg(address.toString()).arg(port);

		connect(m_server.GetPtr(), &CMultiThreadServer::NewThreadConnection, this, &CTcpServerComp::OnNewThreadConnection); //, Qt::QueuedConnection

		return true;
	}
	else{
		SendErrorMessage(0, QString("Server could not be started on %1:%2").arg(address.toString()).arg(port));
	}

	return false;
}


void CTcpServerComp::OnNewThreadConnection(const IRequest* request)
{
//    qDebug() << "OnNewThreadConnection" << "Request:" << request->GetCommandId() << "Body:" << request->GetBody();

    if (m_requestHandlerCompPtr.IsValid()){
        m_requestHandlerCompPtr->ProcessRequest(*request);
    }

}


} // namespace imtrest


