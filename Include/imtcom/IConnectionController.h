// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ACF includes
#include <istd/IPolymorphic.h>


namespace imtcom
{


/**
	\class IConnectionController
	\brief Simple interface for connection lifecycle control.
	
	IConnectionController provides a minimal contract for components that need
	to establish and tear down network connections. This interface is intentionally
	lightweight and generic, suitable for various connection types (TCP, WebSocket,
	database connections, etc.).
	
	\section iconcontroller_features Key Concepts
	
	<b>Connection States:</b>
	- Not connected (initial state)
	- Connected (after successful Connect())
	- Disconnected (after Disconnect() or connection loss)
	
	<b>Lifecycle:</b>
	- Connect(): Establish connection
	- Disconnect(): Close connection gracefully
	- Idempotent operations (calling Connect() when already connected should succeed)
	
	\section iconcontroller_usage Usage Example
	
	\code{.cpp}
	// Assume component implements IConnectionController
	IConnectionController* controller = ...;
	
	// Establish connection
	if (controller->Connect()) {
	    qDebug() << "Connection established";
	    
	    // Perform operations...
	    
	    // Close connection when done
	    controller->Disconnect();
	} else {
	    qCritical() << "Failed to connect";
	}
	\endcode
	
	\section iconcontroller_implementations Typical Implementations
	
	Components implementing this interface typically include:
	- Network client components (HTTP, WebSocket, gRPC clients)
	- Database connection managers
	- Remote service connectors
	- Session managers
	
	\note This interface does not provide connection status queries. For observable
	      connection status, use IConnectionStatusProvider instead.
	
	\sa IConnectionStatusProvider, IServerConnectionInterface
*/
class IConnectionController: virtual public istd::IPolymorphic
{
public:
	/**
		\brief Establish a connection.
		
		Initiates the connection establishment process. Implementation-specific
		details may include DNS resolution, TCP handshake, authentication,
		protocol negotiation, etc.
		
		\return \c true if connection was established successfully; \c false on failure.
		\sa Disconnect()
		
		\par Example:
		\code{.cpp}
		if (!controller->Connect()) {
		    qCritical() << "Connection failed";
		    // Check logs for detailed error information
		}
		\endcode
		
		\note If already connected, implementations typically return \c true without
		      re-establishing the connection (idempotent behavior).
	*/
	virtual bool Connect() = 0;
	
	/**
		\brief Close the connection.
		
		Gracefully terminates the connection. Implementations should clean up
		resources, close sockets, and notify peers if applicable.
		
		\return \c true if disconnection was successful; \c false on error.
		\sa Connect()
		
		\par Example:
		\code{.cpp}
		controller->Disconnect();
		qDebug() << "Connection closed";
		\endcode
		
		\note If already disconnected, implementations typically return \c true
		      without error (idempotent behavior).
	*/
	virtual bool Disconnect() = 0;
};


} // namespace imtcom


