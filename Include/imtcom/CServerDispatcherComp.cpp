// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtcom/CServerDispatcherComp.h>


namespace imtcom
{


// protected methods

// reimplemented (imtcom/CServerConnectionInterfaceParam)

bool CServerDispatcherComp::StartServer(IServerConnectionInterface::ProtocolType protocolType)
{
	if (protocolType == IServerConnectionInterface::ProtocolType::PT_HTTP){
		if (m_httpServerCompPtr.IsValid()){
			return m_httpServerCompPtr->StartServer();
		}
	}
	else if (protocolType == IServerConnectionInterface::ProtocolType::PT_WEBSOCKET){
		if (m_wsServerCompPtr.IsValid()){
			return m_wsServerCompPtr->StartServer();
		}
	}

	return false;
}


bool CServerDispatcherComp::StopServer(IServerConnectionInterface::ProtocolType protocolType)
{
	if (protocolType == IServerConnectionInterface::ProtocolType::PT_HTTP){
		if (m_httpServerCompPtr.IsValid()){
			return m_httpServerCompPtr->StopServer();
		}
	}
	else if (protocolType == IServerConnectionInterface::ProtocolType::PT_WEBSOCKET){
		if (m_wsServerCompPtr.IsValid()){
			return m_wsServerCompPtr->StopServer();
		}
	}

	return false;
}


imtrest::IServer::ServerStatus CServerDispatcherComp::GetServerStatus(IServerConnectionInterface::ProtocolType protocolType) const
{
	if (protocolType == IServerConnectionInterface::ProtocolType::PT_HTTP){
		if (m_httpServerCompPtr.IsValid()){
			return m_httpServerCompPtr->GetServerStatus();
		}
	}
	else if (protocolType == IServerConnectionInterface::ProtocolType::PT_WEBSOCKET){
		if (m_wsServerCompPtr.IsValid()){
			return m_wsServerCompPtr->GetServerStatus();
		}
	}

	return imtrest::IServer::SS_NOT_STARTED;
}


IServerConnectionInterface::ProtocolTypes CServerDispatcherComp::GetSupportedProtocols() const
{
	static IServerConnectionInterface::ProtocolTypes protocolTypes {IServerConnectionInterface::ProtocolType::PT_HTTP, IServerConnectionInterface::ProtocolType::PT_WEBSOCKET};

	return protocolTypes;
}


} // namespace imtcom


