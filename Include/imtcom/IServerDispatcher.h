#pragma once


// ACF includes
#include <istd/IPolymorphic.h>

// ImtCore includes
#include <imtrest/IServer.h>
#include <imtcom/IServerConnectionInterface.h>


namespace imtcom
{


class IServerDispatcher : virtual public istd::IPolymorphic
{
public:
	virtual bool StartServer(IServerConnectionInterface::ProtocolType protocolType) = 0;
	virtual bool StopServer(IServerConnectionInterface::ProtocolType protocolType) = 0;
	virtual imtrest::IServer::ServerStatus GetServerStatus(IServerConnectionInterface::ProtocolType protocolType) const = 0;
	virtual IServerConnectionInterface::ProtocolTypes GetSupportedProtocols() const = 0;
};


} // namespace imtcom


