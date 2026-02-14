// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ACF includes
#include <icomp/CComponentBase.h>
#include <iprm/IEnableableParam.h>

// ImtCore includes
#include <imtcom/CServerConnectionInterfaceParam.h>


namespace imtcom
{


/**
	\class CServerConnectionInterfaceParamComp
	\brief ACF component implementation of IServerConnectionInterface with default port configuration.
	
	CServerConnectionInterfaceParamComp extends CServerConnectionInterfaceParam with ACF
	component infrastructure and provides sensible default port numbers for common protocols.
	This is the recommended class to use in ImtCore applications requiring server connection
	configuration.
	
	\section cserverconnectioncomp_features Key Features
	
	<b>Default Port Configuration:</b>
	- HTTP (PT_HTTP): Port 9001 (default)
	- WebSocket (PT_WEBSOCKET): Port 9000 (default)
	- gRPC (PT_GRPC): Port 50101 (default)
	- Default host: "localhost"
	- Defaults configurable via component attributes
	
	<b>ACF Component Integration:</b>
	- I_BEGIN_COMPONENT / I_END_COMPONENT macros
	- Interface registration: IServerConnectionInterface, ISerializable
	- Attribute-based configuration system
	- Reference support for external SSL configuration
	
	<b>SSL Configuration Integration:</b>
	- External SSL enablement via IEnableableParam reference
	- CF_SECURE flag automatically reflects SSL enabled state
	- Seamless integration with SSL configuration components
	
	<b>Component Lifecycle:</b>
	- OnComponentCreated() override for initialization
	- Automatic protocol registration on creation
	- Default value application
	
	\section cserverconnectioncomp_attributes Component Attributes
	
	| Attribute | Type | Description | Default |
	|-----------|------|-------------|---------|
	| DefaultHost | QString | Default server host name | "localhost" |
	| DefaultWebSocketPort | int | Default WebSocket port | 9000 |
	| DefaultHttpPort | int | Default HTTP port | 9001 |
	| DefaultgRPCPort | int | Default gRPC port | 50101 |
	
	\section cserverconnectioncomp_references Component References
	
	| Reference | Interface | Description |
	|-----------|-----------|-------------|
	| ExternalEnableSSL | iprm::IEnableableParam | External SSL configuration control |
	
	When ExternalEnableSSL is set and enabled, GetConnectionFlags() returns CF_SECURE.
	Otherwise, it returns the manually set connection flags.
	
	\section cserverconnectioncomp_usage Usage Example
	
	\subsection usage_basic Basic Usage with Defaults
	
	\code{.cpp}
	// Create component (uses default ports)
	auto connection = icomp::CreateComponent<CServerConnectionInterfaceParamComp>();
	
	// Default configuration is ready to use:
	// - Host: "localhost"
	// - HTTP port: 9001
	// - WebSocket port: 9000
	// - gRPC port: 50101
	
	// Generate URLs with defaults
	QUrl httpUrl;
	connection->GetUrl(IServerConnectionInterface::PT_HTTP, httpUrl);
	// httpUrl = "http://localhost:9001"
	
	// Or override defaults as needed
	connection->SetHost("api.example.com");
	connection->SetPort(IServerConnectionInterface::PT_HTTP, 443);
	connection->SetConnectionFlags(IServerConnectionInterface::CF_SECURE);
	
	connection->GetUrl(IServerConnectionInterface::PT_HTTP, httpUrl);
	// httpUrl = "https://api.example.com:443"
	\endcode
	
	\subsection usage_custom Custom Port Configuration
	
	\code{.cpp}
	// Create component
	auto connection = icomp::CreateComponent<CServerConnectionInterfaceParamComp>();
	
	// Override default ports via attributes before component creation
	// (in component configuration file or factory setup)
	// connection->SetAttributeValue("DefaultHttpPort", 8080);
	// connection->SetAttributeValue("DefaultWebSocketPort", 8081);
	
	// Or set directly after creation
	connection->SetPort(IServerConnectionInterface::PT_HTTP, 8080);
	connection->SetPort(IServerConnectionInterface::PT_WEBSOCKET, 8081);
	connection->SetPort(IServerConnectionInterface::PT_GRPC, 50051);
	\endcode
	
	\subsection usage_ssl SSL Configuration Integration
	
	\code{.cpp}
	// Create connection component
	auto connection = icomp::CreateComponent<CServerConnectionInterfaceParamComp>();
	
	// Create SSL enableable parameter
	auto sslParam = icomp::CreateComponent<iprm::CEnableableParamComp>();
	
	// Link SSL parameter to connection
	connection->SetReference("ExternalEnableSSL", sslParam);
	
	// Enable SSL
	sslParam->SetEnabled(true);
	
	// GetConnectionFlags() now returns CF_SECURE automatically
	int flags = connection->GetConnectionFlags();
	// flags == IServerConnectionInterface::CF_SECURE
	
	// URLs are automatically generated with secure schemes
	QUrl url;
	connection->GetUrl(IServerConnectionInterface::PT_HTTP, url);
	// url = "https://localhost:9001" (note: https, not http)
	\endcode
	
	\subsection usage_factory Factory-Based Creation
	
	\code{.cpp}
	// Component factory XML configuration example:
	// <Component type="CServerConnectionInterfaceParamComp" name="ServerConnection">
	//   <Attribute name="DefaultHost" value="api.example.com"/>
	//   <Attribute name="DefaultHttpPort" value="8080"/>
	//   <Attribute name="DefaultWebSocketPort" value="8081"/>
	//   <Reference name="ExternalEnableSSL" target="SslConfig"/>
	// </Component>
	
	// Load from configuration
	auto connection = componentFactory->CreateComponent("ServerConnection");
	
	// Configuration automatically applied
	QUrl url;
	connection->GetUrl(IServerConnectionInterface::PT_HTTP, url);
	// url = "http://api.example.com:8080" (or https if SSL enabled)
	\endcode
	
	\section cserverconnectioncomp_defaults Port Number Rationale
	
	The default port numbers are chosen to:
	- Avoid conflicts with standard services (< 1024)
	- Avoid conflicts with common development ports (3000, 8000, 8080)
	- Provide memorable, consistent values
	- Allow multiple ImtCore services on same host
	
	<b>Default Ports:</b>
	- 9000 (WebSocket): Even number for WebSocket convention
	- 9001 (HTTP): Odd number, adjacent to WebSocket for easy remembering
	- 50101 (gRPC): Standard gRPC port range (50000-59999)
	
	\section cserverconnectioncomp_comparison Comparison with Base Class
	
	| Feature | CServerConnectionInterfaceParam | CServerConnectionInterfaceParamComp |
	|---------|--------------------------------|-------------------------------------|
	| ACF Component | ❌ No | ✅ Yes |
	| Default Ports | ❌ Manual configuration required | ✅ HTTP: 9001, WS: 9000, gRPC: 50101 |
	| Default Host | ❌ Empty string | ✅ "localhost" |
	| Attribute Support | ❌ No | ✅ Yes |
	| SSL Integration | ❌ Manual flags only | ✅ IEnableableParam integration |
	| Factory Creation | ❌ No | ✅ Yes |
	| Recommended Use | Non-ACF contexts | ✅ All ImtCore applications |
	
	\section cserverconnectioncomp_best_practices Best Practices
	
	1. **Use as Default**: Always prefer this class over CServerConnectionInterfaceParam
	   in ACF-based applications for consistent defaults.
	
	2. **Override Selectively**: Only override ports when defaults conflict with
	   your deployment environment.
	
	3. **SSL Integration**: Use ExternalEnableSSL reference for SSL control rather
	   than manually setting CF_SECURE flag.
	
	4. **Factory Configuration**: Configure via component factory XML for
	   deployment-specific settings.
	
	5. **Default Host**: Change default host from "localhost" to actual server name
	   in production deployments.
	
	\sa CServerConnectionInterfaceParam, IServerConnectionInterface
	\sa iprm::IEnableableParam, icomp::CComponentBase
*/
class CServerConnectionInterfaceParamComp : public CServerConnectionInterfaceParam, public icomp::CComponentBase
{
public:
	typedef icomp::CComponentBase BaseClass;
	typedef imtcom::CServerConnectionInterfaceParam BaseClass2;

	I_BEGIN_COMPONENT(CServerConnectionInterfaceParamComp);
		I_REGISTER_INTERFACE(IServerConnectionInterface);
		I_REGISTER_INTERFACE(iser::ISerializable);
		I_ASSIGN(m_defaultHostAttrPtr, "DefaultHost", "Default host name", false, "localhost");
		I_ASSIGN(m_defaulWebSocketPortAttrPtr, "DefaultWebSocketPort", "Default value for the WebSocket port", false, 9000);
		I_ASSIGN(m_defaulHttpSocketPortAttrPtr, "DefaultHttpPort", "Default value for the HTTP port", false, 9001)
		I_ASSIGN(m_defaulGrpcSocketPortAttrPtr, "DefaultgRPCPort", "Default value for the gRPC port", false, 50101);
		I_ASSIGN(m_sslEnabledCompPtr, "ExternalEnableSSL", "External SSL configuration", false, "ExternalEnableSSL");
		I_END_COMPONENT;

protected:
	// reimplemented (imtcom/CServerConnectionInterfaceParam)
	virtual int GetConnectionFlags() const override;

	// reimplemented (icomp::CComponentBase)
	virtual void OnComponentCreated() override;

private:
	I_ATTR(int, m_defaulWebSocketPortAttrPtr);
	I_ATTR(int, m_defaulHttpSocketPortAttrPtr);
	I_ATTR(int, m_defaulGrpcSocketPortAttrPtr);
	I_ATTR(QString, m_defaultHostAttrPtr);
	I_REF(iprm::IEnableableParam, m_sslEnabledCompPtr);
};


} // namespace imtcom


