// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once

/**
	\file imtcom.h
	\brief Main header file for the imtcom module - Server Communication and Connection Management
	
	This file provides namespace-level documentation for the imtcom module.
	Include this file to access the complete imtcom module documentation in Doxygen.
*/

/**
	\namespace imtcom
	\brief Server Communication and Connection Management Module
	
	The \b imtcom module provides comprehensive infrastructure for server connection
	configuration, multi-protocol dispatch, SSL/TLS security management, and network
	connection status monitoring.
	
	\section imtcom_overview Overview
	
	This module serves as the foundation for client-server communication in ImtCore,
	handling:
	- Server connection configuration (host, ports, protocols)
	- Multi-protocol server dispatch (HTTP, WebSocket, gRPC, File)
	- SSL/TLS configuration and certificate management
	- Connection status monitoring and health checking
	- Secure and insecure connection modes
	
	\section imtcom_architecture Architecture
	
	\subsection imtcom_patterns Design Patterns
	
	<b>Interface-Implementation Pattern:</b>
	- Clean separation between interface contracts (I*) and implementations (C*)
	- Enables dependency injection and testability
	- Supports multiple implementation strategies
	
	<b>Component Pattern (ACF):</b>
	- All major classes are ACF components
	- Factory-based instantiation with I_BEGIN_COMPONENT macros
	- Attribute-based configuration support
	- Reference-based dependency injection
	
	<b>Observer Pattern:</b>
	- IConnectionStatusProvider extends IChangeable for change notifications
	- Clients can observe connection status changes reactively
	- Event-driven architecture support
	
	<b>Strategy Pattern:</b>
	- Multiple connection checker implementations (async, simple, internet)
	- Pluggable SSL configuration strategies
	- Protocol-specific server implementations
	
	\subsection imtcom_core_interfaces Core Interfaces
	
	The module is organized around several key interface hierarchies:
	
	<b>Connection Configuration Hierarchy:</b>
	\code
	IServerConnectionInterface (iser::ISerializable, istd::IChangeable)
	├─ Defines host, port, protocol configuration
	├─ Supports CF_DEFAULT and CF_SECURE connection flags
	└─ Protocols: PT_HTTP, PT_WEBSOCKET, PT_FILE, PT_GRPC
	    │
	    ├─ CServerConnectionInterfaceParam (concrete data holder)
	    └─ CServerConnectionInterfaceParamComp (ACF component with defaults)
	\endcode
	
	<b>Server Dispatch Hierarchy:</b>
	\code
	IServerDispatcher (istd::IPolymorphic)
	├─ Manages server lifecycle: StartServer(), StopServer()
	├─ Provides server status queries
	└─ Multi-protocol support
	    │
	    └─ CServerDispatcherComp (HTTP and WebSocket dispatcher)
	\endcode
	
	<b>Connection Monitoring Hierarchy:</b>
	\code
	IConnectionStatusProvider (istd::IChangeable)
	├─ CS_UNKNOWN, CS_DISCONNECTED, CS_CONNECTED states
	└─ Observable connection status
	    │
	    ├─ CAsyncConnectionCheckerComp (timer-based HTTP checks)
	    ├─ CSimpleConnectionCheckerComp (GraphQL-based checks)
	    └─ CInternetConnectionCheckerComp (internet availability)
	\endcode
	
	<b>SSL/TLS Configuration Hierarchy:</b>
	\code
	ISslConfigurationManager (istd::IPolymorphic)
	├─ Creates QSslConfiguration from parameter sets
	├─ Defines SSL protocols, key algorithms, encoding formats
	└─ Peer verification modes
	    │
	    └─ CSslConfigurationManagerComp
	
	ISslConfigurationApplier (istd::IPolymorphic)
	├─ Applies certificates, private keys, CA chains
	├─ Sets protocols and peer verification modes
	└─ File-based certificate/key loading
	    │
	    └─ CSslConfigurationAdapterComp
	\endcode
	
	\subsection imtcom_components Key Components
	
	| Component | Interface | Purpose |
	|-----------|-----------|---------|
	| CServerConnectionInterfaceParamComp | IServerConnectionInterface | Connection configuration with default ports (HTTP:9001, WS:9000, gRPC:50101) |
	| CServerDispatcherComp | IServerDispatcher | Multi-protocol server lifecycle management |
	| CSslConfigurationManagerComp | ISslConfigurationManager | SSL configuration factory from parameters |
	| CSslConfigurationAdapterComp | ISslConfigurationApplier | SSL settings application (certificates, keys, protocols) |
	| CAsyncConnectionCheckerComp | IConnectionStatusProvider | Timer-based async connection health checks |
	| CSimpleConnectionCheckerComp | IConnectionStatusProvider | GraphQL-based connection validation |
	| CInternetConnectionCheckerComp | IConnectionStatusProvider | General internet connectivity detection |
	| CRequestSender | N/A | Synchronous HTTP request helper (GET/POST/PUT) |
	
	\section imtcom_usage Usage Examples
	
	\subsection imtcom_example_config Configuring a Server Connection
	
	\code{.cpp}
	// Create connection configuration component
	auto connectionParam = icomp::CreateComponent<CServerConnectionInterfaceParamComp>();
	
	// Configure connection parameters
	connectionParam->SetHost("example.com");
	connectionParam->SetPort(IServerConnectionInterface::PT_HTTP, 8080);
	connectionParam->SetPort(IServerConnectionInterface::PT_WEBSOCKET, 8081);
	connectionParam->SetConnectionFlags(IServerConnectionInterface::CF_SECURE);
	
	// Get URL for a protocol
	QUrl httpUrl;
	if (connectionParam->GetUrl(IServerConnectionInterface::PT_HTTP, httpUrl)) {
	    qDebug() << "HTTP URL:" << httpUrl.toString();
	    // Output: "https://example.com:8080"
	}
	\endcode
	
	\subsection imtcom_example_dispatcher Using the Server Dispatcher
	
	\code{.cpp}
	// Create server dispatcher component
	auto dispatcher = icomp::CreateComponent<CServerDispatcherComp>();
	
	// Start HTTP server
	if (dispatcher->StartServer(IServerConnectionInterface::PT_HTTP)) {
	    qDebug() << "HTTP server started successfully";
	}
	
	// Check server status
	auto status = dispatcher->GetServerStatus(IServerConnectionInterface::PT_HTTP);
	if (status == imtrest::IServer::ServerStatus::SS_RUNNING) {
	    qDebug() << "Server is running";
	}
	
	// Stop server when done
	dispatcher->StopServer(IServerConnectionInterface::PT_HTTP);
	\endcode
	
	\subsection imtcom_example_status Monitoring Connection Status
	
	\code{.cpp}
	// Create connection checker
	auto checker = icomp::CreateComponent<CAsyncConnectionCheckerComp>();
	
	// Configure the checker (set server URL, check interval, etc.)
	// ... component attribute configuration ...
	
	// Observe connection status changes
	QObject::connect(checker.get(), &IChangeable::OnChanged, [](const IChangeable::ChangeSet& cs) {
	    auto provider = dynamic_cast<IConnectionStatusProvider*>(sender());
	    auto status = provider->GetConnectionStatus();
	    
	    switch (status) {
	        case IConnectionStatusProvider::CS_CONNECTED:
	            qDebug() << "Connection established";
	            break;
	        case IConnectionStatusProvider::CS_DISCONNECTED:
	            qDebug() << "Connection lost";
	            break;
	        case IConnectionStatusProvider::CS_UNKNOWN:
	            qDebug() << "Connection status unknown";
	            break;
	    }
	});
	\endcode
	
	\subsection imtcom_example_ssl Configuring SSL/TLS
	
	\code{.cpp}
	// Create SSL configuration manager
	auto sslManager = icomp::CreateComponent<CSslConfigurationManagerComp>();
	
	// Create parameter set with SSL configuration
	auto params = icomp::CreateComponent<iprm::CParamsSetComp>();
	params->SetValue(ISslConfigurationManager::ParamKeys::s_protocolParamKey, 
	                 ISslConfigurationManager::SP_TLS_V1_3_OR_LATER);
	params->SetValue(ISslConfigurationManager::ParamKeys::s_verifyModeParamKey,
	                 ISslConfigurationManager::PVM_VERIFY);
	
	// Create SSL configuration
	QSslConfiguration sslConfig;
	if (sslManager->CreateSslConfiguration(*params, sslConfig)) {
	    // Apply to network request
	    QNetworkRequest request;
	    request.setSslConfiguration(sslConfig);
	}
	
	// Alternative: Apply settings directly
	auto applier = icomp::CreateComponent<CSslConfigurationAdapterComp>();
	applier->LoadLocalCertificateFromFile("/path/to/cert.pem");
	applier->LoadPrivateKeyFromFile("/path/to/key.pem", QSsl::Rsa);
	applier->LoadCaCertificatesFromFile("/path/to/ca.pem");
	applier->SetSslProtocol(QSsl::TlsV1_3OrLater);
	applier->SetPeerVerifyMode(QSslSocket::VerifyPeer);
	\endcode
	
	\section imtcom_integration Cross-Module Integration
	
	The imtcom module integrates with several other ImtCore modules:
	
	<b>imtrest Module:</b>
	- CServerDispatcherComp manages IServer instances (CTcpServerComp, CWebSocketServerComp)
	- Server components reference IServerConnectionInterface for configuration
	- Protocol-specific servers read ports via GetPort(ProtocolType)
	
	<b>imtclientgql Module:</b>
	- CSimpleConnectionCheckerComp uses GraphQL client for health checks
	- Connection status affects client connection lifecycle
	- IServerConnectionInterface provides WebSocket URLs for GraphQL subscriptions
	
	<b>imtservergql Module:</b>
	- CSettingsControllerComp exposes IServerConnectionInterface via GraphQL API
	- Server settings can be queried and modified through GraphQL mutations
	- WebSocket URLs are generated for GraphQL subscription endpoints
	
	<b>imtqml Module:</b>
	- CGqlQuickObjectComp observes IServerConnectionInterface for configuration changes
	- QML applications receive reactive updates when connection parameters change
	- Enables dynamic server switching in client applications
	
	<b>imtauthgui Module:</b>
	- GUI components for server connection editing (CServerConnectionEditorComp)
	- Administration observers monitor connection changes
	- Login GUI components use connection status for availability feedback
	
	<b>imthttp Module:</b>
	- CHttpFileTransfer implements IFileTransfer interface
	- Uses IServerConnectionInterface for file upload/download endpoints
	
	<b>imtservice Module:</b>
	- IConnectionCollection manages multiple server connections
	- Supports multi-server architectures with connection pooling
	
	\section imtcom_protocols Supported Protocols
	
	The module supports four primary communication protocols via the ProtocolType enumeration:
	
	| Protocol | Enum Value | Typical Port | Use Case |
	|----------|------------|--------------|----------|
	| <b>HTTP</b> | PT_HTTP | 9001 (default) | REST API, traditional web services |
	| <b>WebSocket</b> | PT_WEBSOCKET | 9000 (default) | Real-time bidirectional communication, GraphQL subscriptions |
	| <b>gRPC</b> | PT_GRPC | 50101 (default) | High-performance RPC, microservices |
	| <b>File</b> | PT_FILE | N/A | File-based communication, local protocols |
	
	Each protocol can be independently configured with custom ports and security settings.
	The CF_SECURE flag determines whether HTTPS/WSS is used instead of HTTP/WS.
	
	\section imtcom_ssl SSL/TLS Security
	
	The module provides comprehensive SSL/TLS support with:
	
	<b>Protocols:</b>
	- TLS 1.0 through TLS 1.3 (TLS 1.2+ recommended)
	- DTLS 1.0 through DTLS 1.2
	- Deprecation warnings for older protocols
	
	<b>Key Algorithms:</b>
	- RSA (Rivest–Shamir–Adleman)
	- DSA (Digital Signature Algorithm)
	- EC (Elliptic Curve)
	- DH (Diffie-Hellman)
	- OPAQUE (black box keys)
	
	<b>Encoding Formats:</b>
	- PEM (Privacy Enhanced Mail) - text-based, default
	- DER (Distinguished Encoding Rules) - binary
	
	<b>Peer Verification Modes:</b>
	- PVM_NONE: No verification, encryption only
	- PVM_QUERY: Request certificate, don't validate (server default)
	- PVM_VERIFY: Require valid certificate (client default)
	- PVM_AUTO_VERIFY: Automatic selection based on client/server role
	
	\section imtcom_testing Testing Utilities
	
	The module includes several testing and diagnostic components:
	
	- <b>CRequestSender</b>: Synchronous HTTP helper for testing endpoints
	- <b>CInternetConnectionCheckerComp</b>: Validates internet availability
	- <b>CAsyncConnectionCheckerComp</b>: Periodic health checks with retry logic
	- <b>CSimpleConnectionCheckerComp</b>: Application-level connectivity validation
	
	These components support configurable timeouts, retry counts, and check intervals,
	making them suitable for both development diagnostics and production monitoring.
	
	\section imtcom_dependencies Module Dependencies
	
	<b>ACF Framework Dependencies:</b>
	- \c icomp (component framework)
	- \c iser (serialization)
	- \c istd (standard interfaces: IPolymorphic, IChangeable)
	- \c iprm (parameter management)
	- \c ilog (logging framework)
	
	<b>ImtCore Module Dependencies:</b>
	- \c imtrest (server implementations)
	- \c imtbase (URL parameters, system status)
	- \c imtclientgql (GraphQL client for health checks)
	- \c imthttp (file transfer implementations)
	
	<b>Qt Framework Dependencies:</b>
	- QtCore (QUrl, QString, QByteArray, QMap)
	- QtNetwork (QSslConfiguration, QSslSocket, QNetworkRequest)
	
	\section imtcom_best_practices Best Practices
	
	<b>Connection Configuration:</b>
	- Always use CServerConnectionInterfaceParamComp for consistent defaults
	- Set CF_SECURE flag for production deployments
	- Use GetUrl() to generate protocol-specific URLs with proper scheme handling
	
	<b>Server Management:</b>
	- Check GetServerStatus() before calling StopServer()
	- Handle StartServer() failures gracefully with appropriate error reporting
	- Use GetSupportedProtocols() to validate protocol availability
	
	<b>SSL/TLS Configuration:</b>
	- Prefer TLS 1.2 or later (SP_TLS_V1_2_OR_LATER, SP_TLS_V1_3_OR_LATER)
	- Use PVM_VERIFY for client connections requiring secure communication
	- Load CA certificates for proper certificate chain validation
	- Protect private key passphrases, never hardcode in source
	
	<b>Connection Monitoring:</b>
	- Implement retry logic with exponential backoff
	- Use appropriate check intervals to balance responsiveness and resource usage
	- Subscribe to IChangeable notifications for reactive status updates
	
	\section imtcom_see_also See Also
	
	Related ImtCore modules:
	- \ref imtrest "imtrest - REST Server Framework"
	- \ref imtclientgql "imtclientgql - GraphQL Client"
	- \ref imtservergql "imtservergql - GraphQL Server"
	- \ref imthttp "imthttp - HTTP Client"
	- \ref imtbase "imtbase - Base Utilities"
	
	\author ImtCore Development Team
	\version 1.0
	\date 2024-2026
*/

namespace imtcom
{
	// Namespace documented above
	// This file contains no executable code, only documentation
}
