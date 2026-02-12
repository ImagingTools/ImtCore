// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ACF includes
#include <istd/IChangeable.h>


namespace imtcom
{


/**
	\class IConnectionStatusProvider
	\brief Interface for monitoring network connection status with observable state changes.
	
	The IConnectionStatusProvider defines a contract for components that monitor
	network connectivity and provide real-time status updates. It extends IChangeable
	to support reactive programming patterns, allowing clients to observe connection
	state transitions.
	
	This interface is central to connection health monitoring in ImtCore, supporting
	various monitoring strategies from simple endpoint checks to comprehensive
	internet availability detection.
	
	\section iconnectionstatus_overview Key Features
	
	<b>Observable Status:</b>
	- Extends istd::IChangeable for change notifications
	- Clients can react to connection state transitions
	- Supports event-driven architectures
	- Enables UI updates without polling
	
	<b>Three-State Model:</b>
	- CS_UNKNOWN: Initial state or monitoring disabled
	- CS_DISCONNECTED: Connection is down or unreachable
	- CS_CONNECTED: Connection is established and healthy
	
	<b>Multiple Strategies:</b>
	- Async timer-based checks (CAsyncConnectionCheckerComp)
	- GraphQL endpoint validation (CSimpleConnectionCheckerComp)
	- Internet connectivity detection (CInternetConnectionCheckerComp)
	
	\section iconnectionstatus_states Connection States
	
	\subsection state_unknown CS_UNKNOWN (0)
	
	<b>Meaning:</b> Connection status has not been determined yet.
	
	<b>Typical Scenarios:</b>
	- Initial state before first health check
	- Monitoring disabled or not started
	- Error in status determination logic
	- After Reset() call
	
	<b>Recommended Actions:</b>
	- Show loading/initializing indicator in UI
	- Wait for first status update
	- Do not enable connection-dependent features yet
	
	\subsection state_disconnected CS_DISCONNECTED (1)
	
	<b>Meaning:</b> Connection is unavailable, unreachable, or failed health check.
	
	<b>Typical Scenarios:</b>
	- Network interface down
	- Server not responding (timeout)
	- DNS resolution failure
	- HTTP error response (4xx, 5xx)
	- WebSocket connection closed
	- GraphQL query failed
	
	<b>Recommended Actions:</b>
	- Show "offline" or "disconnected" indicator
	- Disable network-dependent UI features
	- Queue operations for retry when reconnected
	- Implement exponential backoff for reconnection attempts
	- Log diagnostic information for troubleshooting
	
	\subsection state_connected CS_CONNECTED (2)
	
	<b>Meaning:</b> Connection is established and health check succeeded.
	
	<b>Typical Scenarios:</b>
	- HTTP endpoint returned 2xx status
	- WebSocket connection established
	- GraphQL query executed successfully
	- Periodic health check passed
	
	<b>Recommended Actions:</b>
	- Show "online" or "connected" indicator
	- Enable network-dependent features
	- Process queued operations
	- Reset retry backoff timer
	
	\section iconnectionstatus_implementations Implementations
	
	\subsection impl_async CAsyncConnectionCheckerComp
	
	<b>Strategy:</b> Timer-based asynchronous HTTP requests
	
	<b>Features:</b>
	- Configurable check interval (e.g., every 30 seconds)
	- Timeout support for slow connections
	- Retry logic with exponential backoff
	- Non-blocking checks via QTimer
	
	<b>Use Cases:</b>
	- Periodic health monitoring
	- Server availability detection
	- Network connectivity validation
	
	<b>Configuration:</b>
	- Check interval (milliseconds)
	- Request timeout (milliseconds)
	- Maximum retry count
	- Target URL for health checks
	
	\subsection impl_simple CSimpleConnectionCheckerComp
	
	<b>Strategy:</b> GraphQL client request validation
	
	<b>Features:</b>
	- Application-level connectivity check
	- Validates full stack (network + auth + GraphQL)
	- Uses actual application endpoints
	- Tests real communication paths
	
	<b>Use Cases:</b>
	- End-to-end application connectivity
	- Post-authentication validation
	- Service-specific health checks
	
	<b>Benefits:</b>
	- Tests actual application protocols
	- Detects authentication failures
	- Validates complete request pipeline
	
	\subsection impl_internet CInternetConnectionCheckerComp
	
	<b>Strategy:</b> External internet availability detection
	
	<b>Features:</b>
	- Checks connectivity to well-known sites (e.g., Google)
	- Differentiates between local network and internet issues
	- Configurable target URLs
	- Timeout and retry support
	
	<b>Use Cases:</b>
	- General internet availability detection
	- Network diagnostics
	- Differentiating local vs. remote issues
	
	<b>Configuration:</b>
	- Target URL (default: google.com)
	- Check interval
	- Timeout settings
	
	\section iconnectionstatus_usage Usage Examples
	
	\subsection usage_basic Basic Connection Monitoring
	
	\code{.cpp}
	// Create connection checker
	auto checker = icomp::CreateComponent<CAsyncConnectionCheckerComp>();
	
	// Configure via component attributes (check interval, timeout, etc.)
	// ...
	
	// Get current status
	auto status = checker->GetConnectionStatus();
	switch (status) {
	    case IConnectionStatusProvider::CS_CONNECTED:
	        qDebug() << "Online";
	        break;
	    case IConnectionStatusProvider::CS_DISCONNECTED:
	        qDebug() << "Offline";
	        break;
	    case IConnectionStatusProvider::CS_UNKNOWN:
	        qDebug() << "Status unknown";
	        break;
	}
	\endcode
	
	\subsection usage_observer Observing Status Changes
	
	\code{.cpp}
	// Subscribe to status changes
	QObject::connect(checker.get(), &istd::IChangeable::OnChanged,
	    [checker](const istd::IChangeable::ChangeSet& changeSet) {
	        auto status = checker->GetConnectionStatus();
	        
	        if (status == IConnectionStatusProvider::CS_CONNECTED) {
	            qDebug() << "Connection restored!";
	            // Enable network features, process queued operations
	            ProcessQueuedRequests();
	        } else if (status == IConnectionStatusProvider::CS_DISCONNECTED) {
	            qWarning() << "Connection lost!";
	            // Disable network features, show offline indicator
	            ShowOfflineIndicator();
	        }
	    });
	\endcode
	
	\subsection usage_retry Retry Logic with Exponential Backoff
	
	\code{.cpp}
	class ConnectionManager {
	public:
	    ConnectionManager(IConnectionStatusProvider* checker) 
	        : m_checker(checker), m_retryDelay(1000) {
	        connect(checker, &IChangeable::OnChanged, 
	                this, &ConnectionManager::OnStatusChanged);
	    }
	    
	private:
	    void OnStatusChanged() {
	        auto status = m_checker->GetConnectionStatus();
	        
	        if (status == IConnectionStatusProvider::CS_CONNECTED) {
	            m_retryDelay = 1000;  // Reset backoff
	            qDebug() << "Connected successfully";
	        } else if (status == IConnectionStatusProvider::CS_DISCONNECTED) {
	            // Schedule retry with exponential backoff
	            QTimer::singleShot(m_retryDelay, this, &ConnectionManager::AttemptReconnect);
	            m_retryDelay = qMin(m_retryDelay * 2, 60000);  // Max 60 seconds
	            qWarning() << "Disconnected, retry in" << m_retryDelay << "ms";
	        }
	    }
	    
	    void AttemptReconnect() {
	        // Trigger reconnection logic
	    }
	    
	    IConnectionStatusProvider* m_checker;
	    int m_retryDelay;
	};
	\endcode
	
	\subsection usage_ui UI Status Indicator
	
	\code{.cpp}
	// QML example with model update binder
	class ConnectionStatusModel : public QObject {
	    Q_OBJECT
	    Q_PROPERTY(QString statusText READ statusText NOTIFY statusChanged)
	    Q_PROPERTY(QColor statusColor READ statusColor NOTIFY statusChanged)
	    
	public:
	    ConnectionStatusModel(IConnectionStatusProvider* provider)
	        : m_observer(this, &ConnectionStatusModel::OnStatusChanged) {
	        m_observer.Observe(provider);
	    }
	    
	signals:
	    void statusChanged();
	    
	private:
	    void OnStatusChanged(const IChangeable::ChangeSet&,
	                        const IConnectionStatusProvider* provider) {
	        m_status = provider->GetConnectionStatus();
	        emit statusChanged();
	    }
	    
	    QString statusText() const {
	        switch (m_status) {
	            case CS_CONNECTED: return "Online";
	            case CS_DISCONNECTED: return "Offline";
	            default: return "Unknown";
	        }
	    }
	    
	    QColor statusColor() const {
	        switch (m_status) {
	            case CS_CONNECTED: return QColor(Qt::green);
	            case CS_DISCONNECTED: return QColor(Qt::red);
	            default: return QColor(Qt::gray);
	        }
	    }
	    
	    ConnectionStatus m_status = CS_UNKNOWN;
	    TModelUpdateBinder<IConnectionStatusProvider, ConnectionStatusModel> m_observer;
	};
	\endcode
	
	\section iconnectionstatus_best_practices Best Practices
	
	<b>Check Intervals:</b>
	- Balance responsiveness vs. resource usage
	- Typical range: 10-60 seconds
	- More frequent for critical connections (5-10 seconds)
	- Less frequent for background monitoring (60+ seconds)
	
	<b>Timeout Configuration:</b>
	- Set reasonable timeouts (5-10 seconds typical)
	- Avoid very short timeouts (< 1 second) - may cause false negatives
	- Avoid very long timeouts (> 30 seconds) - delays status updates
	
	<b>Retry Logic:</b>
	- Always implement exponential backoff
	- Set maximum retry delay (e.g., 60 seconds)
	- Reset backoff on successful connection
	- Consider jitter to avoid thundering herd
	
	<b>Observable Pattern:</b>
	- Always use IChangeable notifications, not polling
	- Unsubscribe from observers when no longer needed
	- Handle both CS_CONNECTED and CS_DISCONNECTED transitions
	- Account for CS_UNKNOWN initial state in UI
	
	<b>Error Handling:</b>
	- Log connection failures with diagnostic details
	- Differentiate between transient and persistent failures
	- Provide user feedback for prolonged disconnection
	- Consider implementing connection quality metrics (not just binary on/off)
	
	\sa CAsyncConnectionCheckerComp, CSimpleConnectionCheckerComp, CInternetConnectionCheckerComp
	\sa istd::IChangeable, imtbase::TModelUpdateBinder
*/
class IConnectionStatusProvider: virtual public istd::IChangeable
{
public:
	/**
		\enum ConnectionStatus
		\brief Connection status enumeration.
		
		Represents the current state of network connectivity as determined
		by the monitoring implementation.
		
		\sa GetConnectionStatus()
	*/
	enum ConnectionStatus
	{
		CS_UNKNOWN = 0,			///< Connection status unknown or not yet determined
		CS_DISCONNECTED,		///< Connection is unavailable or failed health check
		CS_CONNECTED			///< Connection is established and healthy
	};

	/**
		\brief Get the current connection status.
		
		Returns the current state of the connection as determined by the most
		recent health check or monitoring activity.
		
		This method is typically very lightweight and suitable for frequent calls,
		as it returns cached status rather than performing an actual connectivity test.
		
		\return Current connection status (CS_UNKNOWN, CS_DISCONNECTED, or CS_CONNECTED).
		\sa ConnectionStatus
		
		\par Example:
		\code{.cpp}
		auto status = statusProvider->GetConnectionStatus();
		if (status == IConnectionStatusProvider::CS_CONNECTED) {
		    // Safe to perform network operations
		    SendNetworkRequest();
		} else {
		    // Connection unavailable, queue operation or show error
		    QueueForLater();
		}
		\endcode
		
		\note Status changes trigger IChangeable::OnChanged notifications.
		      Subscribe to these signals rather than polling GetConnectionStatus()
		      for reactive updates.
	*/
	virtual ConnectionStatus GetConnectionStatus() const = 0;
};


} // namespace imtcom


