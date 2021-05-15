#pragma once


// ImtCore includes
#include <imtrest/CServerCompBase.h>


namespace imtrest
{


template <class Server, class Socket>
class TSocketServerComp: public CServerCompBase
{
public:
	typedef CServerCompBase BaseClass;

	I_BEGIN_COMPONENT(TSocketServerComp);
	I_END_COMPONENT;

protected:
	// reimplemented (CServerCompBase)
	virtual bool StartListening(const QHostAddress& address = QHostAddress::Any, quint16 port = 0) override;
	virtual SocketList CreateConnectionSockets(QObject* serverPtr) const override;
	virtual void RegisterSocket(QObject* socketPtr) override;
};


// reimplemented (CServerCompBase)

template <class Server, class Socket>
bool TSocketServerComp<Server, Socket>::StartListening(const QHostAddress& address, quint16 port)
{
	if (!m_protocolEngineCompPtr.IsValid()){
		return false;
	}

	istd::TDelPtr<Server> tcpServerPtr(new Server(this));
	if (tcpServerPtr->listen(address, port)){
		connect(tcpServerPtr.GetPtr(), &Server::newConnection, this, &CServerCompBase::HandleNewConnections, Qt::UniqueConnection);

		m_servers.push_back(tcpServerPtr.PopPtr());

		return true;
	}
	else{
		SendErrorMessage(0, QString("Server could not be started on %1:%2").arg(address.toString()).arg(port));
	}

	return false;
}


template <class Server, class Socket>
CServerCompBase::SocketList TSocketServerComp<Server, Socket>::CreateConnectionSockets(QObject* serverPtr) const
{
	SocketList retVal;

	Server* serverImplPtr = qobject_cast<Server*>(serverPtr);
	Q_ASSERT(serverImplPtr != nullptr);

	while (Socket* socketPtr = serverImplPtr->nextPendingConnection()){
		retVal.push_back(socketPtr);
	}

	return retVal;
}


template <class Server, class Socket>
void TSocketServerComp<Server, Socket>::RegisterSocket(QObject* socketPtr)
{
	Socket* socketImplPtr = dynamic_cast<Socket*>(socketPtr);
	Q_ASSERT(socketImplPtr != nullptr);

	connect(socketImplPtr, &Socket::disconnected, this, &CServerCompBase::OnSocketDisconnected);
}


} // namespace imtrest


