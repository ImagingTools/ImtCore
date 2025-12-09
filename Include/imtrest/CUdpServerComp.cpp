#include <imtrest/CUdpServerComp.h>


// Qt includes
#include <QtCore/QFileInfo>

// ACF includes
#include <iprm/TParamsPtr.h>
#include <iprm/IEnableableParam.h>

// ImtCore includes
#include <imtrest/IRequest.h>
#include <imtrest/IResponse.h>
#include <imtrest/ISender.h>
#include <imtrest/CUdpRequest.h>
#include <imtrest/CUdpSender.h>


namespace imtrest
{


// public methods

CUdpServerComp::CUdpServerComp()
{
	connect(QCoreApplication::instance(), &QCoreApplication::aboutToQuit, this, &CUdpServerComp::OnSystemShutdown, Qt::DirectConnection);
}


CUdpServerComp::~CUdpServerComp()
{
}


imtrest::IRequestServlet* CUdpServerComp::GetRequestServlet()
{
	if (!m_requestHandlerCompPtr.IsValid()){
		return nullptr;
	}

	return m_requestHandlerCompPtr.GetPtr();
}


// protected methods

void CUdpServerComp::OnModelChanged(int /*modelId*/, const istd::IChangeable::ChangeSet& /*changeSet*/)
{
	if (m_startServerOnCreateAttrPtr.IsValid() && *m_startServerOnCreateAttrPtr && m_isInitialized){
		EnsureServerStarted();
	}
}


// reimplemented (ibase::TRuntimeStatusHanderCompWrap)

void CUdpServerComp::OnSystemShutdown()
{
	BaseClass2::UnregisterAllModels();
}


// reimplemented (icomp::CComponentBase)

void CUdpServerComp::OnComponentCreated()
{
	BaseClass::OnComponentCreated();


	if (m_startServerOnCreateAttrPtr.IsValid() && *m_startServerOnCreateAttrPtr){
		EnsureServerStarted();
	}

	m_isInitialized = true;
}


const ISender* CUdpServerComp::GetSender(const QByteArray& requestId) const
{
	for (int i=0;i< m_requests.GetCount();i++){
		if (m_requests.GetAt(i)->GetRequestId() == requestId){
			CUdpSender *sender;
			sender = new CUdpSender(m_requests.GetAt(i));
			//m_requests.(i);
			return sender;
		}
	}

	return nullptr;
}


// reimplemented (imtrest::IServer)

bool CUdpServerComp::StartServer()
{
	return EnsureServerStarted();
}


bool CUdpServerComp::StopServer()
{
	m_udpSocket.close();

	return true;
}


IServer::ServerStatus CUdpServerComp::GetServerStatus() const
{
	return SS_LISTENING;
}


// private methods

bool CUdpServerComp::StartListening(const QHostAddress &address, quint16 port)
{
	if(m_udpSocket.bind(QHostAddress::LocalHost, port)){
		connect(
					&m_udpSocket,
					&QUdpSocket::readyRead,
					this,
					&CUdpServerComp::ReadPendingDatagrams);
		qDebug()<<"Udp server start for port"<<port;
		return true;
	}

	return false;
}


void CUdpServerComp::ReadPendingDatagrams()
{
	while (m_udpSocket.hasPendingDatagrams()) {
		QNetworkDatagram datagram = m_udpSocket.receiveDatagram();
		CUdpRequest req(
					*m_requestHandlerCompPtr.GetPtr(),
					*m_protocolEngineCompPtr.GetPtr(),
					&m_udpSocket,
					datagram.senderAddress(),
					datagram.senderPort());

		m_requests.PushBack(&req);

		ConstResponsePtr resp = m_requestHandlerCompPtr->ProcessRequest(req, datagram.data());
		//m_udpSocket.writeDatagram(datagram.makeReply(resp->GetData()));
	}
}


bool CUdpServerComp::EnsureServerStarted()
{
	if (m_serverConnnectionInterfaceCompPtr.IsValid()){
		int port = m_serverConnnectionInterfaceCompPtr->GetPort(imtcom::IServerConnectionInterface::PT_HTTP);

		return StartListening(QHostAddress::Any, port);
	}
	else{
		return StartListening();
	}

	return false;
}


} // namespace imtrest


