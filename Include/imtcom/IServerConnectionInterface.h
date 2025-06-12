#pragma once


// ACF includes
#include <iser/ISerializable.h>


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
	enum ProtocalType
	{
		PT_HTTP,
		PT_WEBSOCKET,
		PT_FILE
	};

	enum ConnectionFlags
	{
		CF_NONE = 0,
		CF_SECURE = 1
	};

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
	virtual int GetPort(ProtocalType protocol) const = 0;

	/**
		Set the communication port for a given protocol.
	*/
	virtual void SetPort(ProtocalType protocol, int port) = 0;

	/**
		Get the list of protocol supported by the server implementation.
	*/
	virtual QList<ProtocalType> GetSupportedProtocols() const = 0;

	/**
		Get the URL object for the given protocol type and connection flags
	*/
	virtual QUrl GetUrl(ProtocolType protocol, ConnectionFlags flags) const = 0;
};


} // namespace imtcom


