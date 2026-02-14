// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ImtCore includes
#include <imtcom/IServerConnectionInterface.h>

#undef SetPort

#ifdef Q_OS_WIN
#undef SetPort
#endif


namespace imtcom
{


/**
	\class CServerConnectionInterfaceParam
	\brief Basic implementation of IServerConnectionInterface with protocol-port mapping.
	
	CServerConnectionInterfaceParam provides the core data storage and logic for
	server connection configuration. It maintains a map of protocols to port numbers
	and implements all methods defined by IServerConnectionInterface.
	
	This class serves as the base for CServerConnectionInterfaceParamComp and can
	be used directly in non-ACF contexts where component infrastructure is not needed.
	
	\section cserverconnectionparam_features Key Features
	
	<b>Protocol Registration:</b>
	- Dynamic protocol registration via RegisterProtocol()
	- Supports any protocol type from IServerConnectionInterface::ProtocolType
	- GetSupportedProtocols() returns only registered protocols
	
	<b>Port Mapping:</b>
	- Independent port configuration per protocol
	- Map-based storage (QMap<ProtocolType, int>)
	- Zero port value indicates unset/disabled protocol
	
	<b>URL Generation:</b>
	- Automatic scheme selection (http/https, ws/wss) based on CF_SECURE flag
	- Handles standard and custom ports
	- Proper URL encoding and formatting
	
	<b>Serialization:</b>
	- Implements ISerializable for configuration persistence
	- Serializes host, ports, and connection flags
	- Compatible with ACF serialization framework
	
	<b>Change Management:</b>
	- Implements IChangeable for observable modifications
	- Supports copy, clone, reset, and equality operations
	- Change notifications when properties modified
	
	\section cserverconnectionparam_usage Usage Example
	
	\code{.cpp}
	// Create instance
	CServerConnectionInterfaceParam connection;
	
	// Register protocols
	connection.RegisterProtocol(IServerConnectionInterface::PT_HTTP);
	connection.RegisterProtocol(IServerConnectionInterface::PT_WEBSOCKET);
	
	// Configure connection
	connection.SetHost("api.example.com");
	connection.SetPort(IServerConnectionInterface::PT_HTTP, 8080);
	connection.SetPort(IServerConnectionInterface::PT_WEBSOCKET, 8081);
	connection.SetConnectionFlags(IServerConnectionInterface::CF_SECURE);
	
	// Generate URLs
	QUrl httpUrl, wsUrl;
	connection.GetUrl(IServerConnectionInterface::PT_HTTP, httpUrl);
	// httpUrl = "https://api.example.com:8080"
	
	connection.GetUrl(IServerConnectionInterface::PT_WEBSOCKET, wsUrl);
	// wsUrl = "wss://api.example.com:8081"
	
	// Check supported protocols
	auto protocols = connection.GetSupportedProtocols();
	qDebug() << "Supports" << protocols.size() << "protocols";
	\endcode
	
	\section cserverconnectionparam_serialization Serialization Format
	
	The class serializes the following data:
	- Connection flags (int): CF_DEFAULT or CF_SECURE
	- Host name (QString): DNS name, IP address, or file path
	- Port map (map of ProtocolType → int): Protocol-specific ports
	
	\section cserverconnectionparam_thread_safety Thread Safety
	
	This class is not thread-safe. If accessed from multiple threads,
	external synchronization is required. Consider using CServerConnectionInterfaceParamComp
	which may provide additional thread safety through component framework.
	
	\sa IServerConnectionInterface, CServerConnectionInterfaceParamComp
*/
class CServerConnectionInterfaceParam : virtual public IServerConnectionInterface
{
public:
	/**
		\brief Default constructor.
		
		Initializes the connection parameter with:
		- Connection flags: CF_DEFAULT (insecure)
		- Host: empty string
		- Port map: empty (no protocols registered)
	*/
	CServerConnectionInterfaceParam();

	/**
		\brief Register a protocol as supported.
		
		Adds the specified protocol to the list of supported protocols.
		Once registered, the protocol can be configured with SetPort() and
		queried with GetPort() and GetUrl().
		
		Registering a protocol does not set a port - you must call SetPort()
		separately to configure the port number.
		
		\param protocol Protocol type to register.
		\sa GetSupportedProtocols(), SetPort()
		
		\par Example:
		\code{.cpp}
		CServerConnectionInterfaceParam connection;
		
		// Register protocols you want to support
		connection.RegisterProtocol(IServerConnectionInterface::PT_HTTP);
		connection.RegisterProtocol(IServerConnectionInterface::PT_GRPC);
		
		// Now configure ports
		connection.SetPort(IServerConnectionInterface::PT_HTTP, 8080);
		connection.SetPort(IServerConnectionInterface::PT_GRPC, 50051);
		\endcode
	*/
	void RegisterProtocol(ProtocolType protocol);

	// reimplemented (imtcom::IServerConnectionInterface)
	virtual int GetConnectionFlags() const override;
	virtual void SetConnectionFlags(int connectionFlags) override;
	virtual QString GetHost() const override;
	virtual void SetHost(const QString& host) override;
	virtual int GetPort(ProtocolType protocol) const override;
	virtual void SetPort(ProtocolType protocol, int port) override;
	virtual ProtocolTypes GetSupportedProtocols() const override;
	virtual bool GetUrl(ProtocolType protocol, QUrl& url) const override;

	// reimplemented (iser::ISerializable)
	virtual bool Serialize(iser::IArchive& archive) override;

	// reimplemented (istd::IChangeable)
	virtual int GetSupportedOperations() const override;
	virtual bool CopyFrom(const IChangeable& object, CompatibilityMode mode = CM_WITHOUT_REFS) override;
	virtual bool IsEqual(const IChangeable& object) const override;
	virtual istd::IChangeableUniquePtr CloneMe(CompatibilityMode mode) const override;
	virtual bool ResetData(CompatibilityMode mode = CM_WITHOUT_REFS) override;

protected:
	typedef QMap<ProtocolType, int> InterfaceMap;
	InterfaceMap m_interfaceMap;

	int m_connectionFlags = CF_DEFAULT;
	QString m_host;
};


} // namespace imtcom


