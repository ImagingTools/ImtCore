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
	\class IServerConnectionInterface
	\brief Interface for describing server connection configuration with multi-protocol support.
	
	The IServerConnectionInterface defines the contract for configuring server connections
	across multiple communication protocols. A single server instance can support multiple
	protocols (HTTP, WebSocket, gRPC, File) on the same host, each on a separate port.
	
	This interface supports both secure (HTTPS/WSS) and insecure (HTTP/WS) connection modes,
	making it suitable for development, testing, and production deployments.
	
	\section iserverconnection_overview Key Concepts
	
	<b>Multi-Protocol Support:</b>
	- Single host can expose multiple protocols simultaneously
	- Each protocol configured with independent port numbers
	- Default ports provided by CServerConnectionInterfaceParamComp
	- Protocol availability queryable via GetSupportedProtocols()
	
	<b>Connection Modes:</b>
	- CF_DEFAULT: Insecure connections (HTTP, WS)
	- CF_SECURE: Secure connections with SSL/TLS (HTTPS, WSS)
	- Flag-based switching between secure and insecure modes
	
	<b>URL Generation:</b>
	- Automatic scheme selection (http/https, ws/wss) based on CF_SECURE flag
	- Protocol-specific URL construction via GetUrl()
	- Simplifies client configuration
	
	\section iserverconnection_implementations Implementations
	
	- \b CServerConnectionInterfaceParam: Basic implementation with port mapping
	- \b CServerConnectionInterfaceParamComp: ACF component with default ports
	  - HTTP: 9001
	  - WebSocket: 9000
	  - gRPC: 50101
	
	\section iserverconnection_usage Usage Example
	
	\code{.cpp}
	// Create connection configuration
	auto connection = icomp::CreateComponent<CServerConnectionInterfaceParamComp>();
	
	// Configure host and ports
	connection->SetHost("api.example.com");
	connection->SetPort(IServerConnectionInterface::PT_HTTP, 8080);
	connection->SetPort(IServerConnectionInterface::PT_WEBSOCKET, 8081);
	
	// Enable secure connections
	connection->SetConnectionFlags(IServerConnectionInterface::CF_SECURE);
	
	// Generate URLs
	QUrl httpUrl, wsUrl;
	connection->GetUrl(IServerConnectionInterface::PT_HTTP, httpUrl);
	// httpUrl = "https://api.example.com:8080"
	
	connection->GetUrl(IServerConnectionInterface::PT_WEBSOCKET, wsUrl);
	// wsUrl = "wss://api.example.com:8081"
	\endcode
	
	\section iserverconnection_integration Integration Points
	
	<b>Server Components (imtrest):</b>
	- CTcpServerComp (HTTP server)
	- CWebSocketServerComp (WebSocket server)
	- CUdpServerComp (UDP server)
	
	<b>Client Components:</b>
	- imtclientgql: GraphQL clients read WebSocket URLs
	- imtqml: QML applications observe configuration changes
	- imthttp: HTTP clients use connection parameters
	
	<b>Server Management:</b>
	- IServerDispatcher: Uses ProtocolType for server lifecycle control
	- CServerDispatcherComp: Manages multi-protocol server instances
	
	\sa CServerConnectionInterfaceParam, CServerConnectionInterfaceParamComp, IServerDispatcher
	\sa imtrest::IServer, imtrest::CTcpServerComp, imtrest::CWebSocketServerComp
*/
class IServerConnectionInterface : virtual public iser::ISerializable
{
public:
	/**
		\enum ProtocolType
		\brief Communication protocols supported by server connections.
		
		Defines the types of communication protocols that can be configured
		for a server connection. Each protocol operates on an independent port
		and may have protocol-specific behavior.
		
		\sa SetPort(), GetPort(), GetUrl()
	*/
	enum ProtocolType
	{
		PT_UNKNOWN,			///< Unknown or unspecified protocol
		PT_HTTP,			///< HTTP/HTTPS protocol for RESTful APIs (default port: 9001)
		PT_WEBSOCKET,		///< WebSocket/WSS protocol for real-time bidirectional communication (default port: 9000)
		PT_FILE,			///< File-based protocol for local file system access
		PT_GRPC				///< gRPC protocol for high-performance RPC (default port: 50101)
	};
	I_DECLARE_ENUM(ProtocolType, PT_UNKNOWN, PT_HTTP, PT_WEBSOCKET, PT_FILE, PT_GRPC);

	/**
		\enum ConnectionFlags
		\brief Flags controlling connection security and behavior.
		
		Bitflags that control aspects of the connection configuration,
		particularly whether secure (encrypted) connections are used.
		
		\sa SetConnectionFlags(), GetConnectionFlags()
	*/
	enum ConnectionFlags
	{
		CF_DEFAULT = 0x1,	///< Default insecure connection (HTTP, WS)
		CF_SECURE = 0x2		///< Secure connection with SSL/TLS encryption (HTTPS, WSS)
	};
	I_DECLARE_FLAGS(ConnectionFlags, CF_DEFAULT, CF_SECURE);

	/**
		\typedef ProtocolTypes
		\brief List of protocol types for querying supported protocols.
		\sa GetSupportedProtocols()
	*/
	typedef QList<ProtocolType> ProtocolTypes;

	/**
		\brief Get the connection flags determining secure/insecure mode.
		
		Returns the current connection flags, which control whether connections
		use secure (HTTPS/WSS) or insecure (HTTP/WS) protocols.
		
		\return Current connection flags (CF_DEFAULT or CF_SECURE).
		\sa SetConnectionFlags(), ConnectionFlags
	*/
	virtual int GetConnectionFlags() const = 0;

	/**
		\brief Set the connection flags to control secure/insecure mode.
		
		Configures whether the connection should use secure (encrypted) or
		insecure protocols. This flag affects URL generation via GetUrl().
		
		\param connectionFlags Connection flags to set (CF_DEFAULT or CF_SECURE).
		\sa GetConnectionFlags(), ConnectionFlags, GetUrl()
		
		\par Example:
		\code{.cpp}
		// Enable secure connections
		connection->SetConnectionFlags(IServerConnectionInterface::CF_SECURE);
		
		// Generate URL - will use https:// scheme
		QUrl url;
		connection->GetUrl(IServerConnectionInterface::PT_HTTP, url);
		// url = "https://example.com:9001"
		\endcode
	*/
	virtual void SetConnectionFlags(int connectionFlags) = 0;

	/**
		\brief Get the server host name or address.
		
		Returns the host name, which can be:
		- DNS name (e.g., "api.example.com")
		- IPv4 address (e.g., "192.168.1.100")
		- IPv6 address (e.g., "[2001:db8::1]")
		- File path for PT_FILE protocol (e.g., "/var/run/app.sock")
		
		\return Server host name or address.
		\sa SetHost()
	*/
	virtual QString GetHost() const = 0;

	/**
		\brief Set the server host name or address.
		
		Configures the server host, which can be a DNS name, IP address,
		or file path depending on the protocol being used.
		
		\param host Server host name, IP address, or file path.
		\sa GetHost()
		
		\par Example:
		\code{.cpp}
		connection->SetHost("api.example.com");
		connection->SetHost("192.168.1.100");
		connection->SetHost("[::1]");  // IPv6 localhost
		connection->SetHost("/var/run/app.sock");  // Unix socket for PT_FILE
		\endcode
	*/
	virtual void SetHost(const QString& host) = 0;

	/**
		\brief Get the port number for a specific protocol.
		
		Returns the configured port number for the specified communication protocol.
		Each protocol has an independent port configuration on the same host.
		
		\param protocol The protocol type (PT_HTTP, PT_WEBSOCKET, etc.).
		\return Port number for the specified protocol, or 0 if not configured.
		\sa SetPort(), ProtocolType
		
		\par Example:
		\code{.cpp}
		int httpPort = connection->GetPort(IServerConnectionInterface::PT_HTTP);
		int wsPort = connection->GetPort(IServerConnectionInterface::PT_WEBSOCKET);
		\endcode
	*/
	virtual int GetPort(ProtocolType protocol) const = 0;

	/**
		\brief Set the port number for a specific protocol.
		
		Configures the port number on which the specified protocol will listen.
		Different protocols can use different ports on the same host, allowing
		a single server to expose multiple communication methods.
		
		\param protocol The protocol type (PT_HTTP, PT_WEBSOCKET, etc.).
		\param port Port number (typically 1-65535, 0 to clear configuration).
		\sa GetPort(), ProtocolType
		
		\par Example:
		\code{.cpp}
		// Configure multiple protocols on same host
		connection->SetHost("example.com");
		connection->SetPort(IServerConnectionInterface::PT_HTTP, 8080);
		connection->SetPort(IServerConnectionInterface::PT_WEBSOCKET, 8081);
		connection->SetPort(IServerConnectionInterface::PT_GRPC, 50051);
		\endcode
	*/
	virtual void SetPort(ProtocolType protocol, int port) = 0;

	/**
		\brief Get the list of protocols supported by this connection.
		
		Returns all protocol types that have been configured (registered) for
		this connection. Only protocols in this list can be used with GetPort()
		and GetUrl().
		
		\return List of supported protocol types.
		\sa RegisterProtocol() (in CServerConnectionInterfaceParam)
		
		\par Example:
		\code{.cpp}
		auto protocols = connection->GetSupportedProtocols();
		for (auto protocol : protocols) {
		    qDebug() << "Supported:" << protocol 
		             << "on port" << connection->GetPort(protocol);
		}
		\endcode
	*/
	virtual ProtocolTypes GetSupportedProtocols() const = 0;

	/**
		\brief Generate a complete URL for the specified protocol.
		
		Constructs a fully-qualified URL for the given protocol, incorporating:
		- Appropriate scheme (http/https, ws/wss) based on CF_SECURE flag
		- Host name from GetHost()
		- Port number from GetPort(protocol)
		- Proper URL encoding and format
		
		This is the recommended way to obtain connection URLs for clients,
		as it handles all protocol-specific URL construction logic.
		
		\param protocol The protocol type for URL generation.
		\param[out] url Output URL object to populate.
		\return \c true if URL was successfully generated; \c false if protocol
		        is not supported or configuration is incomplete.
		\sa GetHost(), GetPort(), GetConnectionFlags()
		
		\par Example:
		\code{.cpp}
		connection->SetHost("api.example.com");
		connection->SetPort(IServerConnectionInterface::PT_HTTP, 443);
		connection->SetConnectionFlags(IServerConnectionInterface::CF_SECURE);
		
		QUrl url;
		if (connection->GetUrl(IServerConnectionInterface::PT_HTTP, url)) {
		    qDebug() << url.toString();
		    // Output: "https://api.example.com:443"
		}
		
		// WebSocket URL
		connection->SetPort(IServerConnectionInterface::PT_WEBSOCKET, 443);
		if (connection->GetUrl(IServerConnectionInterface::PT_WEBSOCKET, url)) {
		    qDebug() << url.toString();
		    // Output: "wss://api.example.com:443"
		}
		\endcode
	*/
	virtual bool GetUrl(ProtocolType protocol, QUrl& url) const = 0;
};


} // namespace imtcom


