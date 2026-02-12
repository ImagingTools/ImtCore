// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ACF includes
#include <icomp/CComponentBase.h>

// ImtCore includes
#include <imtrest/IServer.h>
#include <imtcom/IServerDispatcher.h>


namespace imtcom
{


/**
	\class CServerDispatcherComp
	\brief ACF component implementing multi-protocol server dispatch for HTTP and WebSocket.
	
	CServerDispatcherComp is the primary implementation of IServerDispatcher, providing
	lifecycle management for HTTP and WebSocket server instances. It acts as a facade
	for controlling multiple server implementations from the imtrest module through a
	unified protocol-based API.
	
	This component enables applications to start, stop, and monitor multiple communication
	protocols simultaneously on the same host, with independent control over each protocol.
	
	\section cserverdispatcher_features Key Features
	
	<b>Protocol Support:</b>
	- PT_HTTP: HTTP/HTTPS server via imtrest::CTcpServerComp
	- PT_WEBSOCKET: WebSocket/WSS server via imtrest::CWebSocketServerComp
	- Extensible design for additional protocols (gRPC, UDP, etc.)
	
	<b>Server Lifecycle Management:</b>
	- Independent start/stop control per protocol
	- Status monitoring with ServerStatus enum
	- Graceful startup and shutdown
	- Error handling and logging
	
	<b>ACF Component Integration:</b>
	- References to server components (HttpServer, WebSocketServer)
	- Component-based dependency injection
	- Factory-compatible configuration
	
	<b>Server Status Tracking:</b>
	- Delegates to underlying IServer implementations
	- Real-time status queries
	- States: SS_UNKNOWN, SS_STOPPED, SS_STARTING, SS_RUNNING, SS_STOPPING
	
	\section cserverdispatcher_architecture Architecture
	
	\code
	                  CServerDispatcherComp
	                          |
	                  IServerDispatcher
	                          |
	           +--------------+---------------+
	           |                              |
	    StartServer(PT_HTTP)        StartServer(PT_WEBSOCKET)
	           |                              |
	           v                              v
	   m_httpServerCompPtr           m_wsServerCompPtr
	  (imtrest::IServer)            (imtrest::IServer)
	           |                              |
	           v                              v
	  CTcpServerComp                CWebSocketServerComp
	 (HTTP/HTTPS Server)           (WebSocket/WSS Server)
	           |                              |
	           v                              v
	  Reads IServerConnectionInterface for host/port/security
	\endcode
	
	\section cserverdispatcher_component_refs Component References
	
	| Reference | Type | Description | Default |
	|-----------|------|-------------|---------|
	| HttpServer | imtrest::IServer | HTTP/HTTPS server implementation | "HttpServer" |
	| WebSocketServer | imtrest::IServer | WebSocket/WSS server implementation | "WebSocketServer" |
	
	Both server components must be properly configured with IServerConnectionInterface
	references to provide host, port, and security settings.
	
	\section cserverdispatcher_protocol_mapping Protocol-to-Server Mapping
	
	The dispatcher routes protocol operations to specific server implementations:
	
	| Protocol Type | Server Reference | Typical Implementation | Port (Default) |
	|---------------|------------------|------------------------|----------------|
	| PT_HTTP | m_httpServerCompPtr | imtrest::CTcpServerComp | 9001 |
	| PT_WEBSOCKET | m_wsServerCompPtr | imtrest::CWebSocketServerComp | 9000 |
	| PT_GRPC | (not supported) | — | 50101 |
	| PT_FILE | (not supported) | — | N/A |
	
	\section cserverdispatcher_usage Usage Examples
	
	\subsection usage_basic Basic Server Management
	
	\code{.cpp}
	// Create dispatcher component
	auto dispatcher = icomp::CreateComponent<CServerDispatcherComp>();
	
	// Create and configure HTTP server
	auto httpServer = icomp::CreateComponent<imtrest::CTcpServerComp>();
	// ... configure httpServer with connection parameters ...
	
	// Create and configure WebSocket server
	auto wsServer = icomp::CreateComponent<imtrest::CWebSocketServerComp>();
	// ... configure wsServer with connection parameters ...
	
	// Assign servers to dispatcher
	dispatcher->SetReference("HttpServer", httpServer);
	dispatcher->SetReference("WebSocketServer", wsServer);
	
	// Start servers
	if (dispatcher->StartServer(IServerConnectionInterface::PT_HTTP)) {
	    qDebug() << "HTTP server started";
	}
	
	if (dispatcher->StartServer(IServerConnectionInterface::PT_WEBSOCKET)) {
	    qDebug() << "WebSocket server started";
	}
	
	// Query status
	auto httpStatus = dispatcher->GetServerStatus(IServerConnectionInterface::PT_HTTP);
	if (httpStatus == imtrest::IServer::SS_RUNNING) {
	    qDebug() << "HTTP server is running";
	}
	
	// Later: stop servers
	dispatcher->StopServer(IServerConnectionInterface::PT_WEBSOCKET);
	dispatcher->StopServer(IServerConnectionInterface::PT_HTTP);
	\endcode
	
	\subsection usage_factory Factory-Based Configuration
	
	\code{.cpp}
	// Component factory XML configuration:
	// <Component type="CServerDispatcherComp" name="ServerDispatcher">
	//   <Reference name="HttpServer" target="HttpServerImpl"/>
	//   <Reference name="WebSocketServer" target="WebSocketServerImpl"/>
	// </Component>
	//
	// <Component type="CTcpServerComp" name="HttpServerImpl">
	//   <Reference name="ServerConnectionInterface" target="ConnectionConfig"/>
	//   <!-- ... other HTTP server configuration ... -->
	// </Component>
	//
	// <Component type="CWebSocketServerComp" name="WebSocketServerImpl">
	//   <Reference name="ServerConnectionInterface" target="ConnectionConfig"/>
	//   <!-- ... other WebSocket server configuration ... -->
	// </Component>
	//
	// <Component type="CServerConnectionInterfaceParamComp" name="ConnectionConfig">
	//   <Attribute name="DefaultHost" value="0.0.0.0"/>
	//   <Attribute name="DefaultHttpPort" value="8080"/>
	//   <Attribute name="DefaultWebSocketPort" value="8081"/>
	// </Component>
	
	// Load dispatcher from factory
	auto dispatcher = componentFactory->CreateComponent("ServerDispatcher");
	
	// Start all supported protocols
	auto protocols = dispatcher->GetSupportedProtocols();
	for (auto protocol : protocols) {
	    if (dispatcher->StartServer(protocol)) {
	        qDebug() << "Started server for protocol:" << protocol;
	    }
	}
	\endcode
	
	\subsection usage_application_lifecycle Application Lifecycle Integration
	
	\code{.cpp}
	class ApplicationServer : public QObject {
	public:
	    ApplicationServer() {
	        m_dispatcher = icomp::CreateComponent<CServerDispatcherComp>();
	        // ... configure dispatcher ...
	    }
	    
	    bool Start() {
	        // Start servers in order
	        if (!StartProtocol(IServerConnectionInterface::PT_HTTP)) {
	            qCritical() << "Failed to start HTTP server";
	            return false;
	        }
	        
	        if (!StartProtocol(IServerConnectionInterface::PT_WEBSOCKET)) {
	            qWarning() << "Failed to start WebSocket server";
	            // Continue anyway - WebSocket is optional
	        }
	        
	        qInfo() << "Application servers started";
	        return true;
	    }
	    
	    void Stop() {
	        // Stop in reverse order (graceful shutdown)
	        qInfo() << "Stopping application servers...";
	        
	        StopProtocol(IServerConnectionInterface::PT_WEBSOCKET);
	        StopProtocol(IServerConnectionInterface::PT_HTTP);
	        
	        qInfo() << "Application servers stopped";
	    }
	    
	private:
	    bool StartProtocol(IServerConnectionInterface::ProtocolType protocol) {
	        if (!m_dispatcher->StartServer(protocol)) {
	            return false;
	        }
	        
	        // Wait for server to reach running state
	        int retries = 10;
	        while (retries-- > 0) {
	            auto status = m_dispatcher->GetServerStatus(protocol);
	            if (status == imtrest::IServer::SS_RUNNING) {
	                return true;
	            }
	            QThread::msleep(100);
	        }
	        
	        qCritical() << "Server failed to reach running state:" << protocol;
	        return false;
	    }
	    
	    void StopProtocol(IServerConnectionInterface::ProtocolType protocol) {
	        auto status = m_dispatcher->GetServerStatus(protocol);
	        if (status != imtrest::IServer::SS_RUNNING) {
	            return;  // Already stopped
	        }
	        
	        m_dispatcher->StopServer(protocol);
	        
	        // Wait for graceful shutdown
	        int retries = 20;
	        while (retries-- > 0) {
	            status = m_dispatcher->GetServerStatus(protocol);
	            if (status == imtrest::IServer::SS_STOPPED) {
	                break;
	            }
	            QThread::msleep(100);
	        }
	    }
	    
	    std::shared_ptr<CServerDispatcherComp> m_dispatcher;
	};
	\endcode
	
	\subsection usage_health_monitoring Health Monitoring
	
	\code{.cpp}
	class ServerHealthMonitor : public QObject {
	    Q_OBJECT
	    
	public:
	    ServerHealthMonitor(IServerDispatcher* dispatcher) 
	        : m_dispatcher(dispatcher) {
	        // Monitor server health every 30 seconds
	        m_timer.setInterval(30000);
	        connect(&m_timer, &QTimer::timeout, this, &ServerHealthMonitor::CheckHealth);
	        m_timer.start();
	    }
	    
	signals:
	    void serverUnhealthy(IServerConnectionInterface::ProtocolType protocol);
	    void serverRecovered(IServerConnectionInterface::ProtocolType protocol);
	    
	private slots:
	    void CheckHealth() {
	        auto protocols = m_dispatcher->GetSupportedProtocols();
	        
	        for (auto protocol : protocols) {
	            auto status = m_dispatcher->GetServerStatus(protocol);
	            bool healthy = (status == imtrest::IServer::SS_RUNNING);
	            
	            if (!healthy && m_lastHealthy.value(protocol, false)) {
	                // Transitioned from healthy to unhealthy
	                qWarning() << "Server became unhealthy:" << protocol;
	                emit serverUnhealthy(protocol);
	                
	                // Attempt restart
	                m_dispatcher->StopServer(protocol);
	                if (m_dispatcher->StartServer(protocol)) {
	                    qInfo() << "Server restart initiated:" << protocol;
	                }
	            } else if (healthy && !m_lastHealthy.value(protocol, false)) {
	                // Transitioned from unhealthy to healthy
	                qInfo() << "Server recovered:" << protocol;
	                emit serverRecovered(protocol);
	            }
	            
	            m_lastHealthy[protocol] = healthy;
	        }
	    }
	    
	private:
	    IServerDispatcher* m_dispatcher;
	    QTimer m_timer;
	    QMap<IServerConnectionInterface::ProtocolType, bool> m_lastHealthy;
	};
	\endcode
	
	\section cserverdispatcher_error_handling Error Handling
	
	The dispatcher handles several error scenarios:
	
	<b>Startup Failures:</b>
	- Port already in use → StartServer() returns false
	- Invalid configuration → StartServer() returns false
	- Missing server reference → StartServer() returns false
	- Permission denied → StartServer() returns false (logged)
	
	<b>Status Queries:</b>
	- Invalid protocol → Returns SS_UNKNOWN
	- Null server reference → Returns SS_UNKNOWN
	
	<b>Shutdown:</b>
	- StopServer() on already-stopped server → Returns true (no-op)
	- StopServer() with null reference → Returns false
	
	\section cserverdispatcher_best_practices Best Practices
	
	1. **Initialization Order**: Configure all server components and their connection
	   parameters before calling StartServer().
	
	2. **Graceful Shutdown**: Stop servers in reverse order of startup to ensure
	   dependent connections are closed first (WebSocket before HTTP).
	
	3. **Status Checking**: Always check GetServerStatus() before attempting to
	   stop a server to avoid unnecessary operations.
	
	4. **Error Logging**: Check StartServer() return value and log detailed errors
	   for troubleshooting deployment issues.
	
	5. **Health Monitoring**: Implement periodic status checks to detect and recover
	   from unexpected server failures.
	
	6. **Reference Validation**: Ensure HttpServer and WebSocketServer references
	   are set before using the dispatcher.
	
	7. **Wait for Startup**: After StartServer(), wait for SS_RUNNING status before
	   considering the server operational (servers may start asynchronously).
	
	\section cserverdispatcher_limitations Current Limitations
	
	- Only supports HTTP and WebSocket protocols
	- gRPC and File protocols are not implemented
	- No built-in health monitoring (implement externally)
	- No automatic restart on failure
	- No load balancing or server pooling
	
	\section cserverdispatcher_integration Integration Points
	
	<b>Server Implementations (imtrest):</b>
	- imtrest::CTcpServerComp: HTTP/HTTPS server
	- imtrest::CWebSocketServerComp: WebSocket/WSS server
	- Both implement imtrest::IServer interface
	
	<b>Configuration (imtcom):</b>
	- IServerConnectionInterface: Host, port, security configuration
	- Typically CServerConnectionInterfaceParamComp
	
	<b>Applications:</b>
	- imtserverapp: Application server lifecycle
	- imtservergql: GraphQL server management
	- imtbase: System status reporting
	
	\sa IServerDispatcher, IServerConnectionInterface
	\sa imtrest::IServer, imtrest::CTcpServerComp, imtrest::CWebSocketServerComp
*/
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


