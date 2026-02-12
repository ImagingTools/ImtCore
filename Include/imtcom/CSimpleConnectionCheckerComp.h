// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ImtCore includes
#include <imtcom/IConnectionStatusProvider.h>
#include <imtclientgql/TClientRequestManagerCompWrap.h>


namespace imtcom
{


/**
	\class CSimpleConnectionCheckerComp
	\brief GraphQL-based connection status checker for application-level connectivity validation.
	
	CSimpleConnectionCheckerComp implements IConnectionStatusProvider using the GraphQL
	client infrastructure to validate full-stack connectivity. Unlike CAsyncConnectionCheckerComp
	which only tests HTTP-level connectivity, this component validates the entire application
	protocol stack including network, authentication, and GraphQL query execution.
	
	This component extends imtclientgql::CClientRequestManagerCompBase, leveraging the
	GraphQL client framework to perform real application-level health checks. It's ideal
	for validating that not just the network is working, but the entire service pipeline
	is operational and responsive.
	
	\section csimplechecker_features Key Features
	
	<b>Application-Level Validation:</b>
	- Tests complete request pipeline (network + auth + GraphQL)
	- Validates actual application protocols, not just HTTP
	- Detects authentication failures
	- Confirms GraphQL endpoint accessibility
	
	<b>GraphQL Integration:</b>
	- Extends CClientRequestManagerCompBase
	- Uses GraphQL client for requests
	- Supports configurable GraphQL commands
	- Leverages existing GraphQL infrastructure
	
	<b>Status Management:</b>
	- Three states: CS_UNKNOWN, CS_DISCONNECTED, CS_CONNECTED
	- Observable status via IChangeable
	- Updated based on GraphQL request success/failure
	
	<b>Command Configuration:</b>
	- Configurable GraphQL command ID
	- Flexible query selection
	- Typically uses lightweight introspection or health queries
	
	\section csimplechecker_architecture Architecture
	
	\code
	  CSimpleConnectionCheckerComp
	           |
	           | extends
	           v
	  CClientRequestManagerCompBase
	           |
	           | uses
	           v
	   GraphQL Client Infrastructure
	           |
	  +--------+--------+
	  |                 |
	  v                 v
	Network Layer    Auth Layer
	  |                 |
	  +--------+--------+
	           |
	           v
	   GraphQL Endpoint
	           |
	           v
	   Update m_status based on
	   success/failure
	\endcode
	
	\section csimplechecker_component_attrs Component Attributes
	
	| Attribute | Type | Description | Required |
	|-----------|------|-------------|----------|
	| Command-ID | QByteArray | GraphQL command identifier for health check query | Yes |
	
	The Command-ID typically refers to a lightweight GraphQL query such as:
	- Introspection query (__schema)
	- Health check query (healthCheck, ping, status)
	- Simple scalar query (serverTime, version)
	
	\section csimplechecker_vs_async Comparison with CAsyncConnectionCheckerComp
	
	| Aspect | CAsyncConnectionCheckerComp | CSimpleConnectionCheckerComp |
	|--------|----------------------------|------------------------------|
	| **Check Method** | HTTP GET request | GraphQL query execution |
	| **What It Tests** | HTTP connectivity only | Full application stack |
	| **Use Case** | Basic network availability | Application health validation |
	| **Dependencies** | QNetworkAccessManager | GraphQL client infrastructure |
	| **Authentication** | Not tested | Validated as part of request |
	| **Protocol** | Generic HTTP | Application-specific GraphQL |
	| **Failure Detection** | Network/HTTP errors | Auth + GraphQL + network errors |
	| **Overhead** | Low | Moderate (full request pipeline) |
	
	\section csimplechecker_status_logic Status Determination Logic
	
	<b>CS_CONNECTED:</b>
	- GraphQL query executed successfully
	- Authentication succeeded (if required)
	- Network connection established
	- Valid GraphQL response received
	
	<b>CS_DISCONNECTED:</b>
	- Network unreachable
	- Authentication failed
	- GraphQL query failed
	- Timeout or connection error
	- Invalid response format
	
	<b>CS_UNKNOWN:</b>
	- Initial state before first request
	- After component reset
	
	\section csimplechecker_usage Usage Examples
	
	\subsection usage_basic Basic Usage
	
	\code{.cpp}
	// Create checker component
	auto checker = icomp::CreateComponent<CSimpleConnectionCheckerComp>();
	
	// Configure GraphQL command for health check
	// (Command-ID should reference a registered GraphQL query)
	checker->SetAttributeValue("Command-ID", "HealthCheckQuery");
	
	// Configure GraphQL client infrastructure
	// (inherited from CClientRequestManagerCompBase)
	// - Server connection interface
	// - Authentication provider
	// - GraphQL client engine
	// ... (via component factory or manual configuration)
	
	// Subscribe to status changes
	QObject::connect(checker.get(), &IChangeable::OnChanged,
	    [checker](const IChangeable::ChangeSet&) {
	        auto status = checker->GetConnectionStatus();
	        if (status == IConnectionStatusProvider::CS_CONNECTED) {
	            qDebug() << "Application service is healthy";
	        } else if (status == IConnectionStatusProvider::CS_DISCONNECTED) {
	            qWarning() << "Application service is unavailable";
	        }
	    });
	
	// Trigger health check by executing GraphQL request
	// (typically triggered by application logic or timer)
	\endcode
	
	\subsection usage_post_login Post-Login Validation
	
	\code{.cpp}
	class LoginValidator : public QObject {
	    Q_OBJECT
	    
	public:
	    LoginValidator() {
	        m_checker = icomp::CreateComponent<CSimpleConnectionCheckerComp>();
	        m_checker->SetAttributeValue("Command-ID", "UserProfileQuery");
	        
	        // Configure GraphQL client with authenticated session
	        // ...
	        
	        connect(m_checker.get(), &IChangeable::OnChanged,
	                this, &LoginValidator::OnValidationComplete);
	    }
	    
	    void ValidateSession() {
	        qDebug() << "Validating authenticated session...";
	        // Trigger GraphQL request via CClientRequestManagerCompBase
	        // The status will update based on success/failure
	    }
	    
	signals:
	    void sessionValid();
	    void sessionInvalid();
	    
	private slots:
	    void OnValidationComplete() {
	        auto status = m_checker->GetConnectionStatus();
	        
	        if (status == IConnectionStatusProvider::CS_CONNECTED) {
	            qInfo() << "Session validated successfully";
	            emit sessionValid();
	        } else {
	            qWarning() << "Session validation failed - re-login required";
	            emit sessionInvalid();
	        }
	    }
	    
	private:
	    std::shared_ptr<CSimpleConnectionCheckerComp> m_checker;
	};
	\endcode
	
	\subsection usage_service_health Service Health Dashboard
	
	\code{.cpp}
	class ServiceHealthDashboard : public QObject {
	    Q_OBJECT
	    
	public:
	    ServiceHealthDashboard() {
	        // Create checkers for different service aspects
	        m_authChecker = CreateChecker("AuthHealthQuery");
	        m_dataChecker = CreateChecker("DataHealthQuery");
	        m_apiChecker = CreateChecker("ApiHealthQuery");
	        
	        // Update dashboard when any status changes
	        connect(m_authChecker.get(), &IChangeable::OnChanged,
	                this, &ServiceHealthDashboard::UpdateDashboard);
	        connect(m_dataChecker.get(), &IChangeable::OnChanged,
	                this, &ServiceHealthDashboard::UpdateDashboard);
	        connect(m_apiChecker.get(), &IChangeable::OnChanged,
	                this, &ServiceHealthDashboard::UpdateDashboard);
	    }
	    
	    struct HealthStatus {
	        bool authService = false;
	        bool dataService = false;
	        bool apiService = false;
	        
	        bool AllHealthy() const {
	            return authService && dataService && apiService;
	        }
	        
	        int HealthyCount() const {
	            return (authService ? 1 : 0) +
	                   (dataService ? 1 : 0) +
	                   (apiService ? 1 : 0);
	        }
	    };
	    
	    HealthStatus GetHealthStatus() const {
	        HealthStatus status;
	        status.authService = IsConnected(m_authChecker);
	        status.dataService = IsConnected(m_dataChecker);
	        status.apiService = IsConnected(m_apiChecker);
	        return status;
	    }
	    
	signals:
	    void healthStatusChanged(const HealthStatus& status);
	    
	private:
	    std::shared_ptr<CSimpleConnectionCheckerComp> CreateChecker(
	            const QByteArray& commandId) {
	        auto checker = icomp::CreateComponent<CSimpleConnectionCheckerComp>();
	        checker->SetAttributeValue("Command-ID", commandId);
	        // Configure GraphQL client infrastructure...
	        return checker;
	    }
	    
	    static bool IsConnected(const std::shared_ptr<CSimpleConnectionCheckerComp>& checker) {
	        return checker->GetConnectionStatus() == IConnectionStatusProvider::CS_CONNECTED;
	    }
	    
	    void UpdateDashboard() {
	        auto status = GetHealthStatus();
	        
	        qInfo() << "Service Health:"
	                << "Auth:" << (status.authService ? "OK" : "FAIL")
	                << "Data:" << (status.dataService ? "OK" : "FAIL")
	                << "API:" << (status.apiService ? "OK" : "FAIL");
	        
	        emit healthStatusChanged(status);
	    }
	    
	    std::shared_ptr<CSimpleConnectionCheckerComp> m_authChecker;
	    std::shared_ptr<CSimpleConnectionCheckerComp> m_dataChecker;
	    std::shared_ptr<CSimpleConnectionCheckerComp> m_apiChecker;
	};
	\endcode
	
	\section csimplechecker_best_practices Best Practices
	
	1. **Lightweight Queries**: Use minimal GraphQL queries for health checks
	   - Introspection: __schema { queryType { name } }
	   - Simple scalar: { serverTime }
	   - Health endpoint: { health { status } }
	   - Avoid heavy queries with joins or complex logic
	
	2. **Authentication Awareness**: Component validates auth as part of request
	   - Ensure auth tokens are valid before health checks
	   - Handle authentication failures appropriately
	   - Refresh tokens if health check fails due to auth
	
	3. **Complementary Use**: Combine with CAsyncConnectionCheckerComp
	   - CAsync for basic network availability
	   - CSimple for application-level validation
	   - Fast network checks + thorough service validation
	
	4. **Error Handling**: Distinguish between network and application errors
	   - Network errors: likely transient, retry with backoff
	   - Auth errors: require user intervention
	   - GraphQL errors: may indicate service degradation
	
	5. **Check Frequency**: Less frequent than network-only checks
	   - CAsync: 10-30 seconds (network check)
	   - CSimple: 60-300 seconds (full validation)
	   - More overhead due to full request pipeline
	
	6. **Command Registration**: Ensure Command-ID references valid query
	   - Register health check queries in GraphQL schema
	   - Use dedicated health check resolvers
	   - Keep health queries read-only (no mutations)
	
	\section csimplechecker_graphql_examples GraphQL Health Check Queries
	
	\subsection query_introspection Introspection Query
	\code{.graphql}
	# Minimal introspection - verifies GraphQL endpoint
	query HealthCheck {
	    __schema {
	        queryType {
	            name
	        }
	    }
	}
	\endcode
	
	\subsection query_dedicated Dedicated Health Endpoint
	\code{.graphql}
	# Custom health check query
	query HealthCheck {
	    health {
	        status          # "OK" | "DEGRADED" | "ERROR"
	        version
	        uptime
	        timestamp
	    }
	}
	\endcode
	
	\subsection query_lightweight Lightweight Data Query
	\code{.graphql}
	# Simple query to verify data access
	query HealthCheck {
	    serverInfo {
	        version
	        timestamp
	    }
	}
	\endcode
	
	\section csimplechecker_troubleshooting Troubleshooting
	
	<b>Always CS_DISCONNECTED:</b>
	- Verify Command-ID references a valid GraphQL query
	- Check authentication credentials
	- Ensure GraphQL client is configured correctly
	- Review GraphQL endpoint URL
	- Check network connectivity
	
	<b>Inconsistent with Network Status:</b>
	- May indicate authentication issues
	- GraphQL endpoint may be down while HTTP is up
	- Check GraphQL-specific errors in logs
	
	<b>Authentication Failures:</b>
	- Validate auth tokens are not expired
	- Ensure auth provider is configured
	- Check auth headers are sent correctly
	- Review auth error responses
	
	\sa IConnectionStatusProvider, CAsyncConnectionCheckerComp, CInternetConnectionCheckerComp
	\sa imtclientgql::CClientRequestManagerCompBase
*/
class CSimpleConnectionCheckerComp:
			public imtclientgql::CClientRequestManagerCompBase,
			virtual public IConnectionStatusProvider
{
public:
	typedef imtclientgql::CClientRequestManagerCompBase BaseClass;

	I_BEGIN_COMPONENT(CSimpleConnectionCheckerComp);
		I_REGISTER_INTERFACE(IConnectionStatusProvider);
		I_ASSIGN(m_commandIdAttrPtr, "Command-ID", "Request command-ID", true, "");
	I_END_COMPONENT;

	CSimpleConnectionCheckerComp();
	// reimplemented (IConnectionStatusProvider)
	virtual ConnectionStatus GetConnectionStatus() const override;

private:
	I_ATTR(QByteArray, m_commandIdAttrPtr);

	ConnectionStatus m_status;
};


} // namespace imtcom


