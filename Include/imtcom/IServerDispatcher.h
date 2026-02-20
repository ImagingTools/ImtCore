// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ACF includes
#include <istd/IPolymorphic.h>

// ImtCore includes
#include <imtrest/IServer.h>
#include <imtcom/IServerConnectionInterface.h>


namespace imtcom
{


/**
	\class IServerDispatcher
	\brief Interface for managing the lifecycle of multi-protocol servers.
	
	The IServerDispatcher provides control over server instances that can support
	multiple communication protocols (HTTP, WebSocket, gRPC). It enables starting,
	stopping, and querying the status of protocol-specific server implementations.
	
	This interface acts as a facade for managing underlying server components
	(typically from the imtrest module), providing a unified API for multi-protocol
	server orchestration.
	
	\section iserverdispatcher_overview Key Features
	
	<b>Multi-Protocol Management:</b>
	- Independent start/stop control for each protocol
	- Per-protocol status queries
	- Graceful startup and shutdown
	- Support for simultaneous protocols on same host
	
	<b>Server Lifecycle:</b>
	- StartServer(): Initialize and start listening on configured port
	- StopServer(): Gracefully shutdown and release resources
	- GetServerStatus(): Query current operational state
	
	<b>Protocol Discovery:</b>
	- GetSupportedProtocols(): Query which protocols this dispatcher can manage
	- Enables dynamic protocol capability detection
	
	\section iserverdispatcher_states Server Status States
	
	Servers progress through the following states (from imtrest::IServer::ServerStatus):
	- \b SS_UNKNOWN: Initial or error state
	- \b SS_STOPPED: Server is not running
	- \b SS_STARTING: Server is initializing
	- \b SS_RUNNING: Server is active and accepting connections
	- \b SS_STOPPING: Server is shutting down
	
	\section iserverdispatcher_implementation Implementation
	
	- \b CServerDispatcherComp: Primary implementation supporting HTTP and WebSocket protocols
	  - Manages imtrest::CTcpServerComp (HTTP)
	  - Manages imtrest::CWebSocketServerComp (WebSocket)
	  - Uses IServerConnectionInterface for port/host configuration
	
	\section iserverdispatcher_usage Usage Example
	
	\code{.cpp}
	// Create dispatcher component
	auto dispatcher = icomp::CreateComponent<CServerDispatcherComp>();
	
	// Configure dispatcher with connection parameters and server components
	// (via ACF component attributes and references)
	
	// Start HTTP server
	if (dispatcher->StartServer(IServerConnectionInterface::PT_HTTP)) {
	    qDebug() << "HTTP server started";
	    
	    // Verify it's running
	    auto status = dispatcher->GetServerStatus(IServerConnectionInterface::PT_HTTP);
	    if (status == imtrest::IServer::SS_RUNNING) {
	        qDebug() << "Server is accepting connections";
	    }
	} else {
	    qCritical() << "Failed to start HTTP server";
	}
	
	// Start WebSocket server simultaneously
	if (dispatcher->StartServer(IServerConnectionInterface::PT_WEBSOCKET)) {
	    qDebug() << "WebSocket server started";
	}
	
	// Query supported protocols
	auto protocols = dispatcher->GetSupportedProtocols();
	qDebug() << "Dispatcher supports" << protocols.size() << "protocols";
	
	// Later: graceful shutdown
	dispatcher->StopServer(IServerConnectionInterface::PT_WEBSOCKET);
	dispatcher->StopServer(IServerConnectionInterface::PT_HTTP);
	\endcode
	
	\section iserverdispatcher_integration Integration Points
	
	<b>Server Components (imtrest):</b>
	- Dispatches to protocol-specific IServer implementations
	- HTTP: imtrest::CTcpServerComp
	- WebSocket: imtrest::CWebSocketServerComp
	- Each server reads configuration from IServerConnectionInterface
	
	<b>Configuration:</b>
	- IServerConnectionInterface: Provides host, port, security flags
	- Ports determined by GetPort(ProtocolType)
	- Host determined by GetHost()
	- Security mode from GetConnectionFlags()
	
	<b>Client Modules:</b>
	- imtservergql: Uses dispatcher for GraphQL server management
	- imtserverapp: Integrates with application lifecycle
	- imtbase: System status monitoring
	
	\sa CServerDispatcherComp, IServerConnectionInterface
	\sa imtrest::IServer, imtrest::CTcpServerComp, imtrest::CWebSocketServerComp
*/
class IServerDispatcher : virtual public istd::IPolymorphic
{
public:
	/**
		\brief Start a server for the specified protocol.
		
		Initializes and starts the server implementation for the given protocol.
		The server will begin listening on the port configured in the associated
		IServerConnectionInterface.
		
		If the server is already running, this method may return \c true without
		restarting it (implementation-dependent).
		
		\param protocolType The protocol type to start (PT_HTTP, PT_WEBSOCKET, etc.).
		\return \c true if the server was started successfully or was already running;
		        \c false if startup failed (e.g., port already in use, configuration invalid).
		\sa StopServer(), GetServerStatus(), IServerConnectionInterface::GetPort()
		
		\par Example:
		\code{.cpp}
		if (!dispatcher->StartServer(IServerConnectionInterface::PT_HTTP)) {
		    qCritical() << "Failed to start HTTP server";
		    // Check logs for detailed error (port conflict, permission denied, etc.)
		    return false;
		}
		\endcode
		
		\note Ensure the IServerConnectionInterface is properly configured with
		      host and port before calling this method.
	*/
	virtual bool StartServer(IServerConnectionInterface::ProtocolType protocolType) = 0;
	
	/**
		\brief Stop the server for the specified protocol.
		
		Gracefully shuts down the server implementation for the given protocol.
		The server will stop accepting new connections and may wait for active
		connections to complete (implementation-dependent).
		
		If the server is already stopped, this method typically returns \c true
		without error.
		
		\param protocolType The protocol type to stop (PT_HTTP, PT_WEBSOCKET, etc.).
		\return \c true if the server was stopped successfully or was already stopped;
		        \c false if shutdown failed.
		\sa StartServer(), GetServerStatus()
		
		\par Example:
		\code{.cpp}
		// Graceful shutdown: stop in reverse order of startup
		dispatcher->StopServer(IServerConnectionInterface::PT_WEBSOCKET);
		dispatcher->StopServer(IServerConnectionInterface::PT_HTTP);
		\endcode
		
		\note For production applications, consider implementing a graceful shutdown
		      timeout to avoid hanging indefinitely on stuck connections.
	*/
	virtual bool StopServer(IServerConnectionInterface::ProtocolType protocolType) = 0;
	
	/**
		\brief Query the operational status of a server.
		
		Returns the current status of the server implementation for the specified
		protocol. The status indicates whether the server is stopped, starting,
		running, or stopping.
		
		\param protocolType The protocol type to query (PT_HTTP, PT_WEBSOCKET, etc.).
		\return Current server status (SS_UNKNOWN, SS_STOPPED, SS_STARTING, SS_RUNNING, SS_STOPPING).
		\sa StartServer(), StopServer(), imtrest::IServer::ServerStatus
		
		\par Example:
		\code{.cpp}
		auto status = dispatcher->GetServerStatus(IServerConnectionInterface::PT_HTTP);
		switch (status) {
		    case imtrest::IServer::SS_RUNNING:
		        qDebug() << "Server is running";
		        break;
		    case imtrest::IServer::SS_STOPPED:
		        qDebug() << "Server is stopped";
		        break;
		    case imtrest::IServer::SS_STARTING:
		        qDebug() << "Server is starting...";
		        // Wait and check again
		        break;
		    case imtrest::IServer::SS_STOPPING:
		        qDebug() << "Server is stopping...";
		        break;
		    default:
		        qWarning() << "Unknown server status";
		        break;
		}
		\endcode
		
		\note Status queries are typically lightweight and suitable for frequent polling
		      or health checks.
	*/
	virtual imtrest::IServer::ServerStatus GetServerStatus(IServerConnectionInterface::ProtocolType protocolType) const = 0;
	
	/**
		\brief Get the list of protocols supported by this dispatcher.
		
		Returns all protocol types that this dispatcher implementation can manage.
		Only protocols in this list can be passed to StartServer(), StopServer(),
		and GetServerStatus().
		
		The list is typically static and defined by the dispatcher implementation.
		For example, CServerDispatcherComp supports PT_HTTP and PT_WEBSOCKET.
		
		\return List of supported protocol types.
		\sa IServerConnectionInterface::ProtocolTypes
		
		\par Example:
		\code{.cpp}
		auto protocols = dispatcher->GetSupportedProtocols();
		for (auto protocol : protocols) {
		    qDebug() << "Supported protocol:" << protocol;
		    
		    // Start all supported servers
		    if (!dispatcher->StartServer(protocol)) {
		        qWarning() << "Failed to start server for protocol:" << protocol;
		    }
		}
		\endcode
		
		\note The returned list represents dispatcher capabilities, not currently
		      configured protocols. Use IServerConnectionInterface::GetSupportedProtocols()
		      to query configured protocols.
	*/
	virtual IServerConnectionInterface::ProtocolTypes GetSupportedProtocols() const = 0;
};


} // namespace imtcom


