// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// Qt includes
#include <QtCore/QTimer>
#include <QtNetwork/QNetworkAccessManager>

// ACF includes
#include <icomp/CComponentBase.h>

// ImtCore includes
#include <imtbase/IUrlParam.h>
#include <imtcom/IConnectionStatusProvider.h>


namespace imtcom
{


/**
	\class CAsyncConnectionCheckerComp
	\brief Timer-based asynchronous connection status checker using HTTP requests.
	
	CAsyncConnectionCheckerComp implements IConnectionStatusProvider using a timer-driven
	approach that periodically sends HTTP requests to verify network connectivity. It provides
	non-blocking health checks with configurable intervals, timeouts, and retry logic.
	
	This component is ideal for periodic server availability monitoring, network connectivity
	validation, and health checking scenarios where you need to detect when a server becomes
	unreachable or recovers.
	
	\section casyncchecker_features Key Features
	
	<b>Timer-Based Checking:</b>
	- Periodic health checks via QTimer
	- Configurable check interval (default: 60 seconds)
	- Non-blocking asynchronous requests
	- Automatic retry on failure
	
	<b>HTTP-Based Validation:</b>
	- Uses QNetworkAccessManager for HTTP requests
	- Supports both HTTP and HTTPS endpoints
	- Timeout detection
	- Connection error handling
	
	<b>Status Management:</b>
	- Three states: CS_UNKNOWN, CS_DISCONNECTED, CS_CONNECTED
	- Observable status changes via IChangeable
	- Initial state: CS_UNKNOWN
	- Transitions only when status actually changes
	
	<b>URL Configuration:</b>
	- Supports direct URL specification
	- Integration with IUrlParam for dynamic URL management
	- Server prefix support for URL construction
	
	\section casyncchecker_architecture Architecture
	
	\code
	        CAsyncConnectionCheckerComp
	                |
	    +-------------------------+
	    |                         |
	    v                         v
	QTimer                QNetworkAccessManager
	(periodic trigger)    (HTTP request execution)
	    |                         |
	    v                         v
	OnTimeout()              ReplyFinished()
	    |                         |
	    v                         v
	SendRequest()            Update m_status
	    |                         |
	    +------------+------------+
	                 |
	                 v
	        Notify IChangeable observers
	        (only if status changed)
	\endcode
	
	\section casyncchecker_component_attrs Component Attributes
	
	| Attribute | Type | Description | Default | Required |
	|-----------|------|-------------|---------|----------|
	| CheckInterval | int | Health check interval in seconds | 60 | No |
	| UrlParam | IUrlParam | URL parameter provider for target endpoint | "UrlParam" | No |
	| ServerPrefix | QByteArray | Server prefix for URL construction | "ServerPrefix" | Yes |
	
	\section casyncchecker_lifecycle Component Lifecycle
	
	1. **OnComponentCreated()**: Initializes QNetworkAccessManager and starts timer
	2. **Timer Triggers**: Calls OnTimeout() at configured intervals
	3. **SendRequest()**: Initiates HTTP GET request to target URL
	4. **ReplyFinished()**: Processes response and updates status
	5. **SetStatus()**: Updates status and notifies observers if changed
	6. **OnComponentDestroyed()**: Stops timer and cleans up network manager
	
	\section casyncchecker_status_logic Status Determination Logic
	
	<b>CS_CONNECTED:</b>
	- HTTP response received (any status code)
	- Network accessible
	- DNS resolution successful
	
	<b>CS_DISCONNECTED:</b>
	- Network timeout
	- Connection refused
	- DNS resolution failure
	- Network error (no internet, host unreachable, etc.)
	
	<b>CS_UNKNOWN:</b>
	- Initial state before first check
	- After component reset
	
	\section casyncchecker_usage Usage Examples
	
	\subsection usage_basic Basic Usage
	
	\code{.cpp}
	// Create checker component
	auto checker = icomp::CreateComponent<CAsyncConnectionCheckerComp>();
	
	// Configure check interval (30 seconds)
	checker->SetAttributeValue("CheckInterval", 30);
	
	// Create URL parameter with target server
	auto urlParam = icomp::CreateComponent<imtbase::CUrlParamComp>();
	urlParam->SetUrl(QUrl("https://api.example.com/health"));
	
	// Assign URL parameter to checker
	checker->SetReference("UrlParam", urlParam);
	
	// Subscribe to status changes
	QObject::connect(checker.get(), &IChangeable::OnChanged,
	    [checker](const IChangeable::ChangeSet&) {
	        auto status = checker->GetConnectionStatus();
	        if (status == IConnectionStatusProvider::CS_CONNECTED) {
	            qDebug() << "Server is reachable";
	        } else if (status == IConnectionStatusProvider::CS_DISCONNECTED) {
	            qWarning() << "Server is unreachable";
	        }
	    });
	
	// Component automatically starts checking after OnComponentCreated()
	\endcode
	
	\subsection usage_health_endpoint Health Endpoint Monitoring
	
	\code{.cpp}
	class ServerHealthMonitor : public QObject {
	    Q_OBJECT
	    
	public:
	    ServerHealthMonitor(const QUrl& healthEndpoint) {
	        m_checker = icomp::CreateComponent<CAsyncConnectionCheckerComp>();
	        
	        // Configure for frequent checks (every 10 seconds)
	        m_checker->SetAttributeValue("CheckInterval", 10);
	        
	        // Set health endpoint URL
	        auto urlParam = icomp::CreateComponent<imtbase::CUrlParamComp>();
	        urlParam->SetUrl(healthEndpoint);  // e.g., /api/health
	        m_checker->SetReference("UrlParam", urlParam);
	        
	        // Monitor status changes
	        connect(m_checker.get(), &IChangeable::OnChanged,
	                this, &ServerHealthMonitor::OnStatusChanged);
	        
	        // Initialize counters
	        m_consecutiveFailures = 0;
	        m_lastUptime = QDateTime::currentDateTime();
	    }
	    
	signals:
	    void serverDown();
	    void serverUp();
	    void alertThresholdExceeded(int failureCount);
	    
	private slots:
	    void OnStatusChanged() {
	        auto status = m_checker->GetConnectionStatus();
	        
	        if (status == IConnectionStatusProvider::CS_CONNECTED) {
	            if (m_consecutiveFailures > 0) {
	                // Server recovered
	                qInfo() << "Server recovered after"
	                        << m_consecutiveFailures << "failures";
	                emit serverUp();
	            }
	            m_consecutiveFailures = 0;
	            m_lastUptime = QDateTime::currentDateTime();
	            
	        } else if (status == IConnectionStatusProvider::CS_DISCONNECTED) {
	            m_consecutiveFailures++;
	            
	            if (m_consecutiveFailures == 1) {
	                // First failure
	                qWarning() << "Server health check failed";
	                emit serverDown();
	            } else if (m_consecutiveFailures == 3) {
	                // Multiple failures - send alert
	                qCritical() << "Server down for" << m_consecutiveFailures
	                           << "consecutive checks";
	                emit alertThresholdExceeded(m_consecutiveFailures);
	            }
	        }
	    }
	    
	private:
	    std::shared_ptr<CAsyncConnectionCheckerComp> m_checker;
	    int m_consecutiveFailures;
	    QDateTime m_lastUptime;
	};
	\endcode
	
	\subsection usage_multi_server Multi-Server Monitoring
	
	\code{.cpp}
	class MultiServerMonitor : public QObject {
	    Q_OBJECT
	    
	public:
	    void AddServer(const QString& name, const QUrl& url) {
	        auto checker = icomp::CreateComponent<CAsyncConnectionCheckerComp>();
	        checker->SetAttributeValue("CheckInterval", 30);
	        
	        auto urlParam = icomp::CreateComponent<imtbase::CUrlParamComp>();
	        urlParam->SetUrl(url);
	        checker->SetReference("UrlParam", urlParam);
	        
	        connect(checker.get(), &IChangeable::OnChanged,
	            [this, name, checker]() {
	                OnServerStatusChanged(name, checker->GetConnectionStatus());
	            });
	        
	        m_checkers[name] = checker;
	        m_status[name] = IConnectionStatusProvider::CS_UNKNOWN;
	    }
	    
	    QStringList GetOnlineServers() const {
	        QStringList online;
	        for (auto it = m_status.begin(); it != m_status.end(); ++it) {
	            if (it.value() == IConnectionStatusProvider::CS_CONNECTED) {
	                online << it.key();
	            }
	        }
	        return online;
	    }
	    
	    int GetHealthyServerCount() const {
	        return GetOnlineServers().size();
	    }
	    
	signals:
	    void serverStatusChanged(const QString& serverName,
	                            IConnectionStatusProvider::ConnectionStatus status);
	    
	private:
	    void OnServerStatusChanged(const QString& name,
	                              IConnectionStatusProvider::ConnectionStatus status) {
	        if (m_status[name] != status) {
	            m_status[name] = status;
	            
	            QString statusStr = (status == IConnectionStatusProvider::CS_CONNECTED)
	                              ? "ONLINE" : "OFFLINE";
	            qInfo() << "Server" << name << "is now" << statusStr;
	            
	            emit serverStatusChanged(name, status);
	        }
	    }
	    
	    QMap<QString, std::shared_ptr<CAsyncConnectionCheckerComp>> m_checkers;
	    QMap<QString, IConnectionStatusProvider::ConnectionStatus> m_status;
	};
	
	// Usage:
	// MultiServerMonitor monitor;
	// monitor.AddServer("Primary", QUrl("https://primary.example.com/health"));
	// monitor.AddServer("Backup", QUrl("https://backup.example.com/health"));
	// monitor.AddServer("Cache", QUrl("https://cache.example.com/health"));
	\endcode
	
	\section casyncchecker_best_practices Best Practices
	
	1. **Check Intervals**: Balance responsiveness and resource usage
	   - Critical services: 5-10 seconds
	   - Normal monitoring: 30-60 seconds
	   - Background checks: 120+ seconds
	
	2. **Timeout Handling**: Network timeouts are detected by QNetworkAccessManager
	   - Default Qt timeout: 30 seconds
	   - Configure shorter timeouts for faster failure detection
	
	3. **Resource Management**: Component automatically cleans up on destruction
	   - Timer stopped in OnComponentDestroyed()
	   - QNetworkAccessManager deleted
	   - No manual cleanup required
	
	4. **Status Notifications**: Only fires IChangeable::OnChanged when status changes
	   - Reduces unnecessary event processing
	   - Use for reactive UI updates
	
	5. **URL Configuration**: Use IUrlParam for dynamic URL changes
	   - Allows runtime reconfiguration
	   - Supports URL updates without recreating component
	
	6. **Health Endpoints**: Design dedicated health check endpoints
	   - Lightweight responses
	   - Quick processing
	   - Return 200 OK if healthy
	   - Don't require authentication if possible
	
	\section casyncchecker_troubleshooting Troubleshooting
	
	<b>Status Always CS_DISCONNECTED:</b>
	- Check URL is correct and reachable
	- Verify network connectivity
	- Check firewall settings
	- Ensure DNS resolution works
	- Review QNetworkAccessManager logs
	
	<b>Status Never Updates:</b>
	- Verify timer is started (check CheckInterval attribute)
	- Ensure OnComponentCreated() was called
	- Check UrlParam reference is set
	- Verify component is not destroyed prematurely
	
	<b>False Negatives (Shows Disconnected When Online):</b>
	- Increase timeout settings
	- Check for intermittent network issues
	- Verify server response time is reasonable
	- Review network request logs
	
	\sa IConnectionStatusProvider, CSimpleConnectionCheckerComp, CInternetConnectionCheckerComp
	\sa imtbase::IUrlParam, QNetworkAccessManager, QTimer
*/
class CAsyncConnectionCheckerComp:
			public QObject,
			public icomp::CComponentBase,
			virtual public IConnectionStatusProvider
{
	Q_OBJECT
public:
	typedef icomp::CComponentBase BaseClass;

	I_BEGIN_COMPONENT(CAsyncConnectionCheckerComp);
		I_REGISTER_INTERFACE(IConnectionStatusProvider);
		I_ASSIGN(m_prefixServer, "ServerPrefix", "Server prefix", true, "ServerPrefix");
		I_ASSIGN(m_urlParamCompPtr, "UrlParam", "The object holds connection's url.", false, "UrlParam");
		I_ASSIGN(m_checkIntervalAttrPtr, "CheckInterval", "Interval for backup timer (in sec)", false, 60);
	I_END_COMPONENT;

	CAsyncConnectionCheckerComp();

	void SendRequest();
	void SetStatus(ConnectionStatus status);

	// reimplemented (icomp::CComponentBase)
	virtual void OnComponentCreated() override;
	virtual void OnComponentDestroyed() override;

	// reimplemented (IConnectionStatusProvider)
	virtual ConnectionStatus GetConnectionStatus() const override;

public Q_SLOTS:
	void ReplyFinished();
	void OnTimeout();

private:
	ConnectionStatus m_status;
	QTimer m_timer;
	QNetworkAccessManager* m_managerPtr;

private:
	I_ATTR(QByteArray, m_urlAttrPtr);
	I_REF(imtbase::IUrlParam, m_urlParamCompPtr);
	I_ATTR(QByteArray, m_prefixServer);
	I_ATTR(int, m_checkIntervalAttrPtr);
};


} // namespace imtcom


