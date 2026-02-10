// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ACF includes
#include <iser/ISerializable.h>

#ifdef Q_OS_WIN
#undef SetPort
#endif


namespace imtcom
{


/**
	Interface for describing the connection configuration of a server.
	A server can provide support of several protocols on the same host.
	Each protocol will be supported on the separated port.
*/
class IServerConnectionInterface : virtual public iser::ISerializable
{
public:
	enum ProtocolType
	{
		PT_UNKNOWN,
		PT_HTTP,
		PT_WEBSOCKET,
		PT_FILE,
		PT_GRPC
	};
	I_DECLARE_ENUM(ProtocolType, PT_UNKNOWN, PT_HTTP, PT_WEBSOCKET, PT_FILE, PT_GRPC);

	enum ConnectionFlags
	{
		CF_DEFAULT = 0x1,
		CF_SECURE = 0x2
	};
	I_DECLARE_FLAGS(ConnectionFlags, CF_DEFAULT, CF_SECURE);

	typedef QList<ProtocolType> ProtocolTypes;

	/**
		Get connection flags.
	*/
	virtual int GetConnectionFlags() const = 0;

	/**
		Set connection flags.
	*/
	virtual void SetConnectionFlags(int connectionFlags) = 0;

	/**
		Get the host name of the server.
		It can be a DNS name, IP-address or a file path.
	*/
	virtual QString GetHost() const = 0;

	/**
		Set the host name of the server.
	*/
	virtual void SetHost(const QString& host) = 0;

	/**
		Get the port for the communication using the given protocol.
	*/
	virtual int GetPort(ProtocolType protocol) const = 0;

	/**
		Set the communication port for a given protocol.
	*/
	virtual void SetPort(ProtocolType protocol, int port) = 0;

	/**
		Get the list of protocol supported by the server implementation.
	*/
	virtual ProtocolTypes GetSupportedProtocols() const = 0;

	/**
		Get the URL object for the given protocol type and connection flags
	*/
	virtual bool GetUrl(ProtocolType protocol, QUrl& url) const = 0;
};


} // namespace imtcom


