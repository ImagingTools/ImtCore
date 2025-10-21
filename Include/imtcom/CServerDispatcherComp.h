#pragma once


// ACF includes
#include <icomp/CComponentBase.h>

// ImtCore includes
#include <imtrest/IServer.h>
#include <imtcom/IServerDispatcher.h>


namespace imtcom
{


class CServerDispatcherComp: public icomp::CComponentBase, virtual public imtcom::IServerDispatcher
{
public:
	typedef icomp::CComponentBase BaseClass;

	I_BEGIN_COMPONENT(CServerDispatcherComp);
		I_REGISTER_INTERFACE(imtcom::IServerDispatcher);
		I_ASSIGN(m_httpServerCompPtr, "HttpServer", "HTTP server", false, "HttpServer");
		I_ASSIGN(m_wsServerCompPtr, "WebSocketServer", "Web socket server", false, "WebSocketServer");
	I_END_COMPONENT;

protected:
	// reimplemented (imtcom/CServerConnectionInterfaceParam)
	virtual bool StartServer(IServerConnectionInterface::ProtocolType protocolType) override;
	virtual bool StopServer(IServerConnectionInterface::ProtocolType protocolType) override;
	virtual imtrest::IServer::ServerStatus GetServerStatus(IServerConnectionInterface::ProtocolType protocolType) const override;
	virtual IServerConnectionInterface::ProtocolTypes GetSupportedProtocols() const override;

private:
	I_REF(imtrest::IServer, m_httpServerCompPtr);
	I_REF(imtrest::IServer, m_wsServerCompPtr);
};


} // namespace imtcom


