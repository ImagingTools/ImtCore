#include <imtrest/CTcpServerComp.h>


// ImtCore includes
#include <imtrest/IRequest.h>
#include <imtrest/IResponse.h>
#include <imtrest/ISender.h>
#include <imtrest/CMultiThreadServer.h>


namespace imtrest
{


// public methods

CTcpServerComp::CTcpServerComp()
	:m_server(new CMultiThreadServer(this))
{
}


CTcpServerComp::~CTcpServerComp()
{
	m_server.Reset();
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
		if (m_serverPortCompPtr.IsValid()){
			int port = m_serverPortCompPtr->GetUrl().port();

			StartListening(QHostAddress::LocalHost, port);
		}
		else{
			StartListening();
		}
	}
}


const IRequest* CTcpServerComp::GetRequest(const QByteArray& requestId) const
{
	const IRequest* requestPtr = m_server->GetRequest(requestId);

	return requestPtr;
}


const ISender* CTcpServerComp::GetSender(const QByteArray& requestId) const
{
	const ISender* senderPtr = m_server->GetSender(requestId);

	return senderPtr;
}


// private methods

bool CTcpServerComp::StartListening(const QHostAddress &address, quint16 port)
{
	if (m_server->listen(address, port)){
		SendInfoMessage(0, QString("Server successfully started on %1:%2").arg(address.toString()).arg(port));

		qDebug() << QString("Server successfully started on %1:%2").arg(address.toString()).arg(port);

		connect(m_server.GetPtr(), &CMultiThreadServer::NewThreadConnection, this, &CTcpServerComp::OnNewThreadConnection);

		return true;
	}
	else{
		QString errorMessage = m_server->errorString();

		SendErrorMessage(0, QString("Server could not be started on %1:%2. Error: %3").arg(address.toString()).arg(port).arg(errorMessage));
	}

	return false;
}


void CTcpServerComp::OnNewThreadConnection(const IRequest* request)
{
	if (m_requestHandlerCompPtr.IsValid()){
		m_requestHandlerCompPtr->ProcessRequest(*request);
	}
}


} // namespace imtrest


