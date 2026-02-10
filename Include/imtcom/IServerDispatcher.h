// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
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


