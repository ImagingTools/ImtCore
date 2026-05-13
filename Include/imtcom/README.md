# imtcom Module

## Overview

The **imtcom** module provides comprehensive infrastructure for server connection configuration, multi-protocol dispatch, SSL/TLS security management, and network connection status monitoring within the ImtCore framework.

This module serves as the foundation for client-server communication, handling connection lifecycle, security, and health checking across multiple protocols (HTTP, WebSocket, gRPC, File).

## Key Features

- **Multi-Protocol Support**: HTTP, WebSocket, gRPC, and File protocols
- **Server Connection Configuration**: Host, port, and protocol management via IServerConnectionInterface
- **Multi-Protocol Dispatch**: Server lifecycle management with CServerDispatcherComp
- **SSL/TLS Security**: Comprehensive certificate and key management
- **Connection Monitoring**: Multiple health checking strategies (async, GraphQL-based, internet)
- **Secure/Insecure Modes**: CF_DEFAULT and CF_SECURE connection flags
- **ACF Component Architecture**: All classes implement ACF component patterns
- **Observable Status**: IChangeable integration for reactive status updates
- **Serialization Support**: ISerializable for configuration persistence

## Core Concepts

### Server Connection Interface (IServerConnectionInterface)

The central interface for describing server connection configuration. A server can support multiple protocols on the same host, each on a separate port.

**Key responsibilities:**
- Define host (DNS name, IP address, or file path)
- Configure ports for different protocols
- Set connection flags (secure/default)
- Generate protocol-specific URLs
- Serialize/deserialize configuration

**Protocol Types:**
- `PT_HTTP` - HTTP/HTTPS protocol (default port: 9001)
- `PT_WEBSOCKET` - WebSocket/WSS protocol (default port: 9000)
- `PT_GRPC` - gRPC protocol (default port: 50101)
- `PT_FILE` - File-based communication
- `PT_UNKNOWN` - Undefined protocol

**Connection Flags:**
- `CF_DEFAULT` - Standard insecure connection (HTTP, WS)
- `CF_SECURE` - Secure connection with SSL/TLS (HTTPS, WSS)

**Implementation Hierarchy:**
```
IServerConnectionInterface
├─ CServerConnectionInterfaceParam (basic implementation)
└─ CServerConnectionInterfaceParamComp (ACF component with defaults)
```

### Server Dispatcher (IServerDispatcher)

Manages the lifecycle of multi-protocol servers, supporting start, stop, and status queries.

**Key responsibilities:**
- Start/stop servers for specific protocols
- Query server status (unknown, stopped, starting, running, stopping)
- Report supported protocols
- Coordinate with imtrest::IServer implementations

**Implementation:**
- `CServerDispatcherComp` - Dispatches HTTP and WebSocket servers

### Connection Status Provider (IConnectionStatusProvider)

Provides observable connection status with three states: unknown, disconnected, connected.

**Key responsibilities:**
- Monitor connection health
- Notify observers of status changes via IChangeable
- Support various checking strategies

**Implementations:**

| Class | Strategy | Use Case |
|-------|----------|----------|
| `CAsyncConnectionCheckerComp` | Timer-based HTTP requests | Periodic health checks with retry logic |
| `CSimpleConnectionCheckerComp` | GraphQL client requests | Application-level connectivity |
| `CInternetConnectionCheckerComp` | External site checks (e.g., Google) | General internet availability |

### SSL/TLS Configuration

Two interfaces provide comprehensive SSL/TLS management:

**ISslConfigurationManager:**
- Creates `QSslConfiguration` objects from parameter sets
- Defines SSL protocols, key algorithms, encoding formats
- Manages peer verification modes

**ISslConfigurationApplier:**
- Applies certificates, private keys, CA chains to configurations
- Sets SSL protocols and peer verification modes
- Loads certificate/key files in PEM or DER formats

## Architecture

### Design Patterns

**Interface-Implementation Pattern:**
- Clean separation between contracts (I*) and implementations (C*)
- Enables dependency injection and testability
- Supports polymorphic usage

**Component Pattern (ACF):**
- Factory-based instantiation via `icomp::CreateComponent`
- Attribute-based configuration with I_ATTR macros
- Reference-based dependency injection with I_REF macros
- Interface registration with I_REGISTER_INTERFACE

**Observer Pattern:**
- `IConnectionStatusProvider` extends `IChangeable`
- Clients subscribe to connection status changes
- Event-driven reactive architecture

**Strategy Pattern:**
- Multiple connection checker implementations
- Pluggable SSL configuration strategies
- Protocol-specific server dispatching

### Class Relationships

```
Connection Configuration Hierarchy:
━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
IServerConnectionInterface
├─ Protocols: PT_HTTP, PT_WEBSOCKET, PT_GRPC, PT_FILE
├─ Flags: CF_DEFAULT, CF_SECURE
│
├─ CServerConnectionInterfaceParam
│  └─ Basic parameter storage with port mapping
│
└─ CServerConnectionInterfaceParamComp
   ├─ Default ports: HTTP:9001, WS:9000, gRPC:50101
   └─ SSL configuration integration


Server Dispatch Hierarchy:
━━━━━━━━━━━━━━━━━━━━━━━━━━
IServerDispatcher
└─ CServerDispatcherComp
   ├─ HTTP server management (imtrest::CTcpServerComp)
   └─ WebSocket server management (imtrest::CWebSocketServerComp)


Connection Monitoring Hierarchy:
━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
IConnectionStatusProvider (extends IChangeable)
├─ Status: CS_UNKNOWN, CS_DISCONNECTED, CS_CONNECTED
│
├─ CAsyncConnectionCheckerComp
│  └─ Timer-based HTTP health checks
│
├─ CSimpleConnectionCheckerComp
│  └─ GraphQL-based application checks
│
└─ CInternetConnectionCheckerComp
   └─ Internet availability detection


SSL/TLS Configuration Hierarchy:
━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
ISslConfigurationManager
├─ Protocols: TLS 1.0-1.3, DTLS 1.0-1.2
├─ Key Algorithms: RSA, DSA, EC, DH, OPAQUE
├─ Encoding: PEM, DER
├─ Peer Verification: NONE, QUERY, VERIFY, AUTO_VERIFY
└─ CSslConfigurationManagerComp

ISslConfigurationApplier
├─ Load certificates/keys from files
├─ Set protocols and verification modes
└─ CSslConfigurationAdapterComp
```

## Components Reference

### Core Components

| Component | Interface | Purpose |
|-----------|-----------|---------|
| `CServerConnectionInterfaceParamComp` | `IServerConnectionInterface` | Connection configuration with default ports |
| `CServerDispatcherComp` | `IServerDispatcher` | Multi-protocol server lifecycle management |
| `CSslConfigurationManagerComp` | `ISslConfigurationManager` | SSL configuration factory from parameters |
| `CSslConfigurationAdapterComp` | `ISslConfigurationApplier` | SSL settings application (cert/key/protocol) |
| `CAsyncConnectionCheckerComp` | `IConnectionStatusProvider` | Timer-based async connection health checks |
| `CSimpleConnectionCheckerComp` | `IConnectionStatusProvider` | GraphQL-based connection validation |
| `CInternetConnectionCheckerComp` | `IConnectionStatusProvider` | General internet connectivity detection |

### Utility Classes

| Class | Purpose |
|-------|---------|
| `CRequestSender` | Synchronous HTTP helper (GET/POST/PUT) with timeout support |
| `CStaticOptionList` | Base for non-editable SSL option lists |
| `CSslProtocolOptionList` | SSL protocol enumeration options |
| `CSslKeyAlgorithmOptionList` | Key algorithm options (RSA, DSA, EC, DH) |
| `CSslEncodingFormatOptionList` | Encoding format options (PEM, DER) |
| `CSslKeyTypeOptionList` | Key type options (private, public) |
| `CSslVerifyModeOptionList` | Peer verification mode options |

### Auxiliary Interfaces

| Interface | Purpose |
|-----------|---------|
| `IConnectionController` | Basic connection control (Connect/Disconnect) |
| `IFileTransfer` | File upload/download via URL |

## Usage Examples

### 1. Configuring a Server Connection

```cpp
#include <imtcom/CServerConnectionInterfaceParamComp.h>
#include <icomp/ComponentFactory.h>

// Create connection configuration component
auto connectionParam = icomp::CreateComponent<imtcom::CServerConnectionInterfaceParamComp>();

// Configure basic connection parameters
connectionParam->SetHost("api.example.com");
connectionParam->SetPort(imtcom::IServerConnectionInterface::PT_HTTP, 8080);
connectionParam->SetPort(imtcom::IServerConnectionInterface::PT_WEBSOCKET, 8081);

// Enable secure connection (HTTPS/WSS)
connectionParam->SetConnectionFlags(imtcom::IServerConnectionInterface::CF_SECURE);

// Generate URLs for different protocols
QUrl httpUrl, wsUrl;
if (connectionParam->GetUrl(imtcom::IServerConnectionInterface::PT_HTTP, httpUrl)) {
    qDebug() << "HTTP URL:" << httpUrl.toString();
    // Output: "https://api.example.com:8080"
}

if (connectionParam->GetUrl(imtcom::IServerConnectionInterface::PT_WEBSOCKET, wsUrl)) {
    qDebug() << "WebSocket URL:" << wsUrl.toString();
    // Output: "wss://api.example.com:8081"
}

// Check supported protocols
auto protocols = connectionParam->GetSupportedProtocols();
for (auto protocol : protocols) {
    qDebug() << "Supported protocol:" << protocol;
}
```

### 2. Starting and Managing Servers

```cpp
#include <imtcom/CServerDispatcherComp.h>
#include <icomp/ComponentFactory.h>

// Create server dispatcher
auto dispatcher = icomp::CreateComponent<imtcom::CServerDispatcherComp>();

// Configure dispatcher with connection parameters, server implementations, etc.
// (via component attributes and references)

// Start HTTP server
if (dispatcher->StartServer(imtcom::IServerConnectionInterface::PT_HTTP)) {
    qDebug() << "HTTP server started successfully";
    
    // Check server status
    auto status = dispatcher->GetServerStatus(imtcom::IServerConnectionInterface::PT_HTTP);
    if (status == imtrest::IServer::ServerStatus::SS_RUNNING) {
        qDebug() << "Server is running";
    }
} else {
    qCritical() << "Failed to start HTTP server";
}

// Start WebSocket server
if (dispatcher->StartServer(imtcom::IServerConnectionInterface::PT_WEBSOCKET)) {
    qDebug() << "WebSocket server started successfully";
}

// Get list of supported protocols
auto supportedProtocols = dispatcher->GetSupportedProtocols();
qDebug() << "Dispatcher supports" << supportedProtocols.size() << "protocols";

// Stop servers when done
dispatcher->StopServer(imtcom::IServerConnectionInterface::PT_HTTP);
dispatcher->StopServer(imtcom::IServerConnectionInterface::PT_WEBSOCKET);
```

### 3. Monitoring Connection Status

```cpp
#include <imtcom/CAsyncConnectionCheckerComp.h>
#include <icomp/ComponentFactory.h>

// Create async connection checker
auto checker = icomp::CreateComponent<imtcom::CAsyncConnectionCheckerComp>();

// Configure checker parameters (check interval, timeout, retry count, etc.)
// via component attributes

// Subscribe to connection status changes
QObject::connect(checker.get(), &istd::IChangeable::OnChanged, 
    [checker](const istd::IChangeable::ChangeSet& changeSet) {
        auto status = checker->GetConnectionStatus();
        
        switch (status) {
            case imtcom::IConnectionStatusProvider::CS_CONNECTED:
                qDebug() << "Connection established";
                // Enable UI features that require connectivity
                break;
                
            case imtcom::IConnectionStatusProvider::CS_DISCONNECTED:
                qWarning() << "Connection lost";
                // Show reconnection UI, disable network features
                break;
                
            case imtcom::IConnectionStatusProvider::CS_UNKNOWN:
                qDebug() << "Connection status unknown";
                // Initial state or after reset
                break;
        }
    });

// Start monitoring
// (checker typically starts automatically based on component configuration)
```

### 4. Configuring SSL/TLS Security

#### Using SSL Configuration Manager

```cpp
#include <imtcom/CSslConfigurationManagerComp.h>
#include <iprm/CParamsSetComp.h>
#include <icomp/ComponentFactory.h>

// Create SSL configuration manager
auto sslManager = icomp::CreateComponent<imtcom::CSslConfigurationManagerComp>();

// Create parameter set with SSL configuration
auto params = icomp::CreateComponent<iprm::CParamsSetComp>();

// Configure SSL protocol (TLS 1.3 or later recommended)
params->SetValue(
    imtcom::ISslConfigurationManager::ParamKeys::s_protocolParamKey,
    imtcom::ISslConfigurationManager::SP_TLS_V1_3_OR_LATER
);

// Configure peer verification mode
params->SetValue(
    imtcom::ISslConfigurationManager::ParamKeys::s_verifyModeParamKey,
    imtcom::ISslConfigurationManager::PVM_VERIFY
);

// Configure key algorithm
params->SetValue(
    imtcom::ISslConfigurationManager::ParamKeys::s_keyAlgorithmParamKey,
    imtcom::ISslConfigurationManager::KA_RSA
);

// Configure encoding format
params->SetValue(
    imtcom::ISslConfigurationManager::ParamKeys::s_encodingFormatParamKey,
    imtcom::ISslConfigurationManager::EF_PEM
);

// Create SSL configuration
QSslConfiguration sslConfig;
if (sslManager->CreateSslConfiguration(*params, sslConfig)) {
    qDebug() << "SSL configuration created successfully";
    
    // Apply to network request
    QNetworkRequest request(QUrl("https://secure.example.com/api"));
    request.setSslConfiguration(sslConfig);
    
    // Use request with QNetworkAccessManager
}
```

#### Using SSL Configuration Applier

```cpp
#include <imtcom/CSslConfigurationAdapterComp.h>
#include <icomp/ComponentFactory.h>

// Create SSL configuration applier
auto applier = icomp::CreateComponent<imtcom::CSslConfigurationAdapterComp>();

// Load local certificate from file
if (!applier->LoadLocalCertificateFromFile("/etc/ssl/certs/server.pem")) {
    qCritical() << "Failed to load server certificate";
}

// Load private key with passphrase
QByteArray passphrase = "secret_password";
if (!applier->LoadPrivateKeyFromFile(
        "/etc/ssl/private/server-key.pem",
        QSsl::Rsa,
        QSsl::Pem,
        passphrase)) {
    qCritical() << "Failed to load private key";
}

// Load CA certificate chain for verification
if (!applier->LoadCaCertificatesFromFile("/etc/ssl/certs/ca-bundle.pem")) {
    qCritical() << "Failed to load CA certificates";
}

// Set SSL protocol version
applier->SetSslProtocol(QSsl::TlsV1_2OrLater);

// Set peer verification mode (verify client certificates)
applier->SetPeerVerifyMode(QSslSocket::VerifyPeer);

qDebug() << "SSL configuration applied successfully";
```

### 5. Sending Synchronous HTTP Requests

```cpp
#include <imtcom/CRequestSender.h>

// Create request sender
imtcom::CRequestSender sender;

// Configure timeout (milliseconds)
sender.SetTimeout(5000);

// Send GET request
QByteArray responseData;
int statusCode = sender.SendRequest(
    QUrl("https://api.example.com/data"),
    QNetworkAccessManager::GetOperation,
    QByteArray(),  // no body for GET
    responseData
);

if (statusCode == 200) {
    qDebug() << "GET request successful:" << responseData;
} else {
    qWarning() << "GET request failed with status:" << statusCode;
}

// Send POST request with JSON body
QByteArray jsonBody = R"({"key": "value", "count": 42})";
int postStatus = sender.SendRequest(
    QUrl("https://api.example.com/submit"),
    QNetworkAccessManager::PostOperation,
    jsonBody,
    responseData
);

if (postStatus == 201) {
    qDebug() << "POST request successful";
}
```

### 6. Observing Server Connection Changes

```cpp
#include <imtcom/CServerConnectionInterfaceParamComp.h>
#include <imtbase/TModelUpdateBinder.h>

class MyConnectionObserver : public QObject {
public:
    MyConnectionObserver(imtcom::IServerConnectionInterface* connectionParam) 
        : m_connectionObserver(this, &MyConnectionObserver::OnConnectionChanged) {
        // Bind observer to connection parameter
        m_connectionObserver.Observe(connectionParam);
    }
    
private:
    void OnConnectionChanged(
            const istd::IChangeable::ChangeSet& changeSet,
            const imtcom::IServerConnectionInterface* connectionParam) {
        qDebug() << "Connection parameters changed";
        
        // React to changes
        if (connectionParam->GetConnectionFlags() & imtcom::IServerConnectionInterface::CF_SECURE) {
            qDebug() << "Secure connection enabled";
        }
        
        QUrl url;
        if (connectionParam->GetUrl(imtcom::IServerConnectionInterface::PT_HTTP, url)) {
            qDebug() << "New HTTP URL:" << url.toString();
        }
    }
    
    imtbase::TModelUpdateBinder<imtcom::IServerConnectionInterface, MyConnectionObserver> 
        m_connectionObserver;
};
```

## Cross-Module Integration

The imtcom module integrates with several other ImtCore modules:

### imtrest Module

**Integration points:**
- `CServerDispatcherComp` manages `imtrest::IServer` instances
- `imtrest::CTcpServerComp` implements HTTP server (references `IServerConnectionInterface`)
- `imtrest::CWebSocketServerComp` implements WebSocket server
- `imtrest::CUdpServerComp` uses connection interface for UDP configuration

**Usage pattern:**
```cpp
// Server components read configuration from IServerConnectionInterface
int httpPort = serverConnectionInterface->GetPort(IServerConnectionInterface::PT_HTTP);
QString host = serverConnectionInterface->GetHost();
```

### imtclientgql Module

**Integration points:**
- `CSimpleConnectionCheckerComp` uses GraphQL client for health checks
- `CGqlClientEngineComp` reads WebSocket URLs from `IServerConnectionInterface`
- `CWebSocketClientComp` uses connection configuration
- `CSubscriptionManagerComp` monitors connection status

**Usage pattern:**
```cpp
// GraphQL client gets WebSocket URL for subscriptions
QUrl wsUrl;
if (serverConnectionInterface->GetUrl(IServerConnectionInterface::PT_WEBSOCKET, wsUrl)) {
    webSocketClient->ConnectToServer(wsUrl);
}
```

### imtservergql Module

**Integration points:**
- `CSettingsControllerComp` exposes `IServerConnectionInterface` via GraphQL API
- `CConnectionStatusSubscriberControllerComp` publishes connection status changes
- Server settings queryable and mutable through GraphQL

**GraphQL schema example:**
```graphql
type ServerConnection {
    host: String!
    httpPort: Int!
    websocketPort: Int!
    isSecure: Boolean!
}

type Query {
    serverConnection: ServerConnection
}

type Mutation {
    updateServerConnection(input: ServerConnectionInput!): ServerConnection
}
```

### imtqml Module

**Integration points:**
- `CGqlQuickObjectComp` observes `IServerConnectionInterface` for changes
- QML applications receive reactive updates when connection parameters change
- Enables dynamic server switching in client UI

**QML usage pattern:**
```qml
// QML component reacts to connection changes
GqlQuickObject {
    id: gqlClient
    // Automatically updates when server connection changes
    onServerConnectionChanged: {
        console.log("Server URL updated:", gqlClient.serverUrl)
    }
}
```

### imtauthgui Module

**Integration points:**
- `CServerConnectionEditorComp` - GUI for editing connection parameters
- `CAdministrationObserverQmlComp` - monitors connection changes
- `CStandardLoginGuiComp` - uses connection status for login availability

**Use case:**
User-facing GUI components for configuring and testing server connections in administration interfaces.

### imthttp Module

**Integration points:**
- `CHttpFileTransfer` implements `IFileTransfer` interface
- Uses `IServerConnectionInterface` for file upload/download endpoints

**Usage pattern:**
```cpp
// File transfer uses connection interface for endpoint URLs
QUrl uploadUrl;
serverConnectionInterface->GetUrl(IServerConnectionInterface::PT_HTTP, uploadUrl);
uploadUrl.setPath("/upload");
fileTransfer->UploadFile(localPath, uploadUrl);
```

### imtservice Module

**Integration points:**
- `IConnectionCollection` manages multiple server connections
- Connection pooling and load balancing support
- Multi-server architecture

### imtbase Module

**Integration points:**
- `CSystemStatusComp` monitors overall system connection status
- `ISystemStatus` aggregates connection health information

## SSL/TLS Protocol Reference

### Supported SSL/TLS Protocols

| Protocol | Enum Value | Security | Recommendation |
|----------|------------|----------|----------------|
| **TLS 1.3** | `SP_TLS_V1_3` | ✅ Strong | ✅ Recommended |
| **TLS 1.3+** | `SP_TLS_V1_3_OR_LATER` | ✅ Strong | ✅ Recommended |
| **TLS 1.2** | `SP_TLS_V1_2` | ✅ Good | ✅ Acceptable |
| **TLS 1.2+** | `SP_TLS_V1_2_OR_LATER` | ✅ Good | ✅ Recommended |
| **TLS 1.1** | `SP_TLS_V1_1` | ⚠️ Weak | ⚠️ Deprecated |
| **TLS 1.1+** | `SP_TLS_V1_1_OR_LATER` | ⚠️ Weak | ⚠️ Deprecated |
| **TLS 1.0** | `SP_TLS_V1_0` | ⚠️ Weak | ⚠️ Deprecated |
| **TLS 1.0+** | `SP_TLS_V1_0_OR_LATER` | ⚠️ Weak | ⚠️ Deprecated |
| **DTLS 1.2** | `SP_DTLS_V1_2` | ✅ Good | ✅ For UDP |
| **DTLS 1.2+** | `SP_DTLS_V1_2_OR_LATER` | ✅ Good | ✅ For UDP |
| **DTLS 1.0** | `SP_DTLS_V1_0` | ⚠️ Weak | ⚠️ Deprecated |
| **DTLS 1.0+** | `SP_DTLS_V1_0_OR_LATER` | ⚠️ Weak | ⚠️ Deprecated |
| **Secure Protocols** | `SP_SECURE_PROTOCOLS` | ✅ Auto | ✅ Default |
| **Any Protocol** | `SP_ANY_PROTOCOL` | ⚠️ Variable | ⚠️ Client only |

**Recommendation:** Use `SP_TLS_V1_2_OR_LATER` or `SP_TLS_V1_3_OR_LATER` for production deployments.

### Peer Verification Modes

| Mode | Enum Value | Behavior | Use Case |
|------|------------|----------|----------|
| **None** | `PVM_NONE` | No certificate verification, encryption only | Development, internal networks |
| **Query** | `PVM_QUERY` | Request certificate, don't validate | Display cert info, server default |
| **Verify** | `PVM_VERIFY` | Require valid certificate | Production clients, secure connections |
| **Auto Verify** | `PVM_AUTO_VERIFY` | Auto-select: Query for servers, Verify for clients | Recommended default |

**Recommendation:** Use `PVM_VERIFY` for production client connections, `PVM_AUTO_VERIFY` for components that act as both client and server.

### Key Algorithms

| Algorithm | Enum Value | Purpose | Common Key Sizes |
|-----------|------------|---------|------------------|
| **RSA** | `KA_RSA` | Asymmetric encryption, signatures | 2048, 3072, 4096 bits |
| **DSA** | `KA_DSA` | Digital signatures | 2048, 3072 bits |
| **EC** | `KA_EC` | Elliptic curve cryptography | P-256, P-384, P-521 |
| **DH** | `KA_DH` | Key exchange | 2048, 3072 bits |
| **OPAQUE** | `KA_OPAQUE` | Black box keys (e.g., HSM) | Hardware-dependent |

**Recommendation:** Use RSA 2048+ or EC P-256+ for new deployments.

### Encoding Formats

| Format | Enum Value | Description | File Extensions |
|--------|------------|-------------|-----------------|
| **PEM** | `EF_PEM` | Base64 text format with headers | `.pem`, `.crt`, `.key` |
| **DER** | `EF_DER` | Binary format | `.der`, `.cer` |

**Recommendation:** Use PEM for text-based configuration, DER for binary storage.

## Best Practices

### Connection Configuration

1. **Use Component Defaults**: Always prefer `CServerConnectionInterfaceParamComp` over `CServerConnectionInterfaceParam` for consistent default ports.

2. **Secure by Default**: Set `CF_SECURE` flag for production deployments:
   ```cpp
   connectionParam->SetConnectionFlags(IServerConnectionInterface::CF_SECURE);
   ```

3. **URL Generation**: Use `GetUrl()` for protocol-specific URLs with proper scheme handling:
   ```cpp
   QUrl url;
   connectionParam->GetUrl(IServerConnectionInterface::PT_HTTP, url);
   // Automatically generates http:// or https:// based on CF_SECURE flag
   ```

4. **Validate Protocols**: Check supported protocols before configuration:
   ```cpp
   auto protocols = connectionParam->GetSupportedProtocols();
   if (protocols.contains(IServerConnectionInterface::PT_GRPC)) {
       // Configure gRPC port
   }
   ```

### Server Management

1. **Check Status Before Stopping**: Verify server is running before attempting to stop:
   ```cpp
   if (dispatcher->GetServerStatus(protocolType) == imtrest::IServer::SS_RUNNING) {
       dispatcher->StopServer(protocolType);
   }
   ```

2. **Handle Start Failures**: Always check `StartServer()` return value:
   ```cpp
   if (!dispatcher->StartServer(protocolType)) {
       qCritical() << "Failed to start server for protocol:" << protocolType;
       // Log error, notify user, attempt recovery
   }
   ```

3. **Graceful Shutdown**: Stop servers in reverse order of startup:
   ```cpp
   dispatcher->StopServer(IServerConnectionInterface::PT_WEBSOCKET);
   dispatcher->StopServer(IServerConnectionInterface::PT_HTTP);
   ```

### SSL/TLS Configuration

1. **Use Modern Protocols**: Prefer TLS 1.2 or later:
   ```cpp
   // Good
   sslConfig.setProtocol(QSsl::TlsV1_2OrLater);
   
   // Better
   sslConfig.setProtocol(QSsl::TlsV1_3OrLater);
   
   // Avoid
   sslConfig.setProtocol(QSsl::TlsV1_0);  // Deprecated
   ```

2. **Verify Certificates**: Use `PVM_VERIFY` for secure client connections:
   ```cpp
   applier->SetPeerVerifyMode(QSslSocket::VerifyPeer);
   ```

3. **Protect Private Keys**: Never hardcode passphrases:
   ```cpp
   // Bad
   QByteArray passphrase = "hardcoded_password";
   
   // Good
   QByteArray passphrase = qgetenv("SSL_KEY_PASSPHRASE");
   if (passphrase.isEmpty()) {
       // Prompt user or load from secure storage
   }
   ```

4. **Load CA Certificates**: Always load CA certificates for proper chain validation:
   ```cpp
   if (!applier->LoadCaCertificatesFromFile("/etc/ssl/certs/ca-bundle.pem")) {
       qCritical() << "Failed to load CA certificates - verification may fail";
   }
   ```

5. **Validate File Paths**: Check certificate/key file existence before loading:
   ```cpp
   if (!QFile::exists(certPath)) {
       qCritical() << "Certificate file not found:" << certPath;
       return false;
   }
   applier->LoadLocalCertificateFromFile(certPath);
   ```

### Connection Monitoring

1. **Implement Retry Logic**: Use exponential backoff for reconnection attempts:
   ```cpp
   int retryDelay = 1000;  // Start with 1 second
   const int maxDelay = 60000;  // Max 60 seconds
   
   if (status == IConnectionStatusProvider::CS_DISCONNECTED) {
       QTimer::singleShot(retryDelay, this, &MyClass::AttemptReconnect);
       retryDelay = qMin(retryDelay * 2, maxDelay);
   }
   ```

2. **Balance Check Intervals**: Choose appropriate intervals for responsiveness vs. resource usage:
   ```cpp
   // Frequent checks for critical connections
   checker->SetCheckInterval(5000);  // 5 seconds
   
   // Infrequent checks for non-critical monitoring
   checker->SetCheckInterval(60000);  // 1 minute
   ```

3. **Subscribe to Changes**: Use IChangeable notifications for reactive updates:
   ```cpp
   connect(checker, &IChangeable::OnChanged, this, &MyClass::OnStatusChanged);
   ```

4. **Handle Unknown States**: Account for initial state before first check:
   ```cpp
   switch (checker->GetConnectionStatus()) {
       case CS_UNKNOWN:
           // Show loading indicator
           break;
       case CS_CONNECTED:
           // Enable features
           break;
       case CS_DISCONNECTED:
           // Show error message
           break;
   }
   ```

### Component Configuration

1. **Use Attributes for Configuration**: Leverage ACF attribute system:
   ```cpp
   // In component definition
   I_ASSIGN(m_checkIntervalAttrPtr, "CheckInterval", "Health check interval (ms)", false, 30000);
   ```

2. **Validate References**: Check component references before use:
   ```cpp
   if (!m_serverConnectionInterfaceCompPtr) {
       qCritical() << "Server connection interface not configured";
       return;
   }
   ```

3. **Handle Serialization**: Implement proper serialization for persistence:
   ```cpp
   bool Serialize(iser::IArchive& archive) override {
       if (!BaseClass::Serialize(archive)) return false;
       // Serialize custom data
       return true;
   }
   ```

## Testing and Diagnostics

The module provides several utilities for testing and diagnostics:

### Testing Utilities

**CRequestSender** - Synchronous HTTP testing:
```cpp
CRequestSender sender;
sender.SetTimeout(10000);  // 10 second timeout

// Test endpoint availability
QByteArray response;
int status = sender.SendRequest(testUrl, QNetworkAccessManager::GetOperation, {}, response);
QCOMPARE(status, 200);
```

**CInternetConnectionCheckerComp** - Internet availability:
```cpp
auto checker = icomp::CreateComponent<CInternetConnectionCheckerComp>();
// Configure to check google.com or custom URL
// Use to validate internet connectivity before application tests
```

**CSimpleConnectionCheckerComp** - Application-level testing:
```cpp
auto checker = icomp::CreateComponent<CSimpleConnectionCheckerComp>();
// Uses GraphQL client to validate full application stack
// Tests authentication, GraphQL endpoint, WebSocket subscriptions
```

### Diagnostic Logging

Enable detailed logging for connection diagnostics:
```cpp
// All connection components extend CLoggerComponentBase
// Logs include:
// - Connection attempts and results
// - SSL handshake details
// - Protocol negotiation
// - Error conditions with stack traces
```

## Dependencies

### ACF Framework Dependencies

- **icomp**: Component framework for factory instantiation and lifecycle
- **iser**: Serialization framework for configuration persistence
- **istd**: Standard interfaces (IPolymorphic, IChangeable, ICloneable)
- **iprm**: Parameter management for configuration
- **ilog**: Logging framework for diagnostics

### ImtCore Module Dependencies

- **imtrest**: Server implementations (CTcpServerComp, CWebSocketServerComp, CUdpServerComp)
- **imtbase**: Base utilities (URL parameters, system status, model update binders)
- **imtclientgql**: GraphQL client for health checks
- **imthttp**: HTTP client and file transfer

### Qt Framework Dependencies

- **QtCore**: Core types (QString, QUrl, QByteArray, QMap, QList)
- **QtNetwork**: Network classes (QSslConfiguration, QSslSocket, QNetworkRequest, QNetworkAccessManager)

## Module Structure

```
Include/imtcom/
├── imtcom.h                                    # Module header with namespace documentation
├── README.md                                   # This file
│
├── Interfaces/
│   ├── IServerConnectionInterface.h            # Server connection configuration interface
│   ├── IServerDispatcher.h                     # Server lifecycle management interface
│   ├── IConnectionStatusProvider.h             # Connection status monitoring interface
│   ├── IConnectionController.h                 # Connection control interface
│   ├── IFileTransfer.h                         # File transfer interface
│   ├── ISslConfigurationManager.h              # SSL configuration factory interface
│   └── ISslConfigurationApplier.h              # SSL settings application interface
│
├── Implementations/
│   ├── Connection Configuration:
│   │   ├── CServerConnectionInterfaceParam.h/.cpp
│   │   └── CServerConnectionInterfaceParamComp.h/.cpp
│   │
│   ├── Server Management:
│   │   └── CServerDispatcherComp.h/.cpp
│   │
│   ├── Connection Monitoring:
│   │   ├── CAsyncConnectionCheckerComp.h/.cpp
│   │   ├── CSimpleConnectionCheckerComp.h/.cpp
│   │   └── CInternetConnectionCheckerComp.h/.cpp
│   │
│   ├── SSL/TLS Configuration:
│   │   ├── CSslConfigurationManagerComp.h/.cpp
│   │   └── CSslConfigurationAdapterComp.h/.cpp
│   │
│   └── Utilities:
│       ├── CRequestSender.h/.cpp
│       ├── CStaticOptionList.h/.cpp
│       ├── CSslProtocolOptionList.h/.cpp
│       ├── CSslKeyAlgorithmOptionList.h/.cpp
│       ├── CSslEncodingFormatOptionList.h/.cpp
│       ├── CSslKeyTypeOptionList.h/.cpp
│       └── CSslVerifyModeOptionList.h/.cpp
│
└── Build/
    ├── CMake/                                  # CMake build configuration
    └── QMake/                                  # QMake build configuration
```

## See Also

### Related ImtCore Modules

- **imtrest** - REST server framework with HTTP and WebSocket implementations
- **imtclientgql** - GraphQL client for server communication
- **imtservergql** - GraphQL server with connection configuration API
- **imthttp** - HTTP client and file transfer implementations
- **imtbase** - Base utilities and system status monitoring
- **imtqml** - QML integration for reactive UI updates
- **imtauthgui** - GUI components for connection editing

### External References

- [Qt QSslConfiguration Documentation](https://doc.qt.io/qt-6/qsslconfiguration.html)
- [Qt QSslSocket Documentation](https://doc.qt.io/qt-6/qsslsocket.html)
- [Qt QNetworkAccessManager Documentation](https://doc.qt.io/qt-6/qnetworkaccessmanager.html)
- [TLS Best Practices (IETF RFC 7525)](https://tools.ietf.org/html/rfc7525)
- [WebSocket Protocol (IETF RFC 6455)](https://tools.ietf.org/html/rfc6455)
- [gRPC Documentation](https://grpc.io/docs/)

## Version History

- **1.0** (2024-2026): Initial documentation
  - Core interfaces and implementations
  - Multi-protocol support
  - SSL/TLS configuration
  - Connection monitoring

## Contributing

When extending this module:

1. Follow the Interface-Implementation pattern
2. Extend existing interfaces rather than creating parallel hierarchies
3. Use ACF component patterns for all new classes
4. Document all public APIs with Doxygen comments
5. Add usage examples to README
6. Update cross-module integration section when adding dependencies
7. Provide unit tests for new components

## Authors

ImtCore Development Team

## License

SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
