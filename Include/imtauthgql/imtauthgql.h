// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ImtCore includes
#include <imtauth/CTenantFilterParam.h>
#include <imtauth/ITenantRelationshipInfo.h>
#include <imtauth/ITenantRelationshipProposalInfo.h>
#include <imtauth/ITenantConnectionRequestInfo.h>
#include <imtauth/ITenantConnectionInfo.h>
#include <imtauth/ICrossOrgGrant.h>
#include <imtauth/IContract.h>
#include <imtauth/ICrossTenantMessage.h>
#include <imtauth/IOrderRequest.h>
#include <imtgql/CGqlRequest.h>
#include <GeneratedFiles/imtauthsdl/SDL/1.0/CPP/Tenants.h>


/**
	\file imtauthgql.h
	\brief Main header file for the imtauthgql module - Authentication GraphQL API Layer
	
	This file provides namespace-level documentation for the imtauthgql module.
	Include this file to access the complete imtauthgql module documentation in Doxygen.
*/

/**
	\namespace imtauthgql
	\brief Authentication GraphQL API Layer Module
	
	The \b imtauthgql module provides GraphQL API endpoints for authentication and
	authorization services, enabling remote user management, role administration,
	permission checking, and session handling through GraphQL queries and mutations.
	
	\section imtauthgql_overview Overview
	
	This module acts as the GraphQL API layer for imtauth, providing:
	- User registration, login, and management
	- Role and permission administration
	- Group membership operations
	- Session and token management
	- Personal access token generation
	- Permission checking and validation
	- Login status subscriptions (real-time)
	- User settings synchronization
	
	\section imtauthgql_architecture Architecture
	
	\subsection imtauthgql_patterns Design Patterns
	
	<b>Adapter Pattern:</b>
	- Adapts imtauth domain model to GraphQL schema
	- Controllers translate GraphQL requests to business logic
	- Response payloads adapt business results to GraphQL format
	- Clean separation between API and domain layers
	
	<b>Controller Pattern:</b>
	- Collection controllers for bulk operations (list, filter)
	- Object controllers for single-object operations (get, update)
	- Action controllers for specific operations (login, register)
	- Clear responsibility separation
	
	<b>Provider Pattern:</b>
	- Info providers supply data for GraphQL resolvers
	- Client request managers handle remote operations
	- Token-based permission providers for authorization
	- Abstraction over data sources
	
	<b>Observer Pattern:</b>
	- Login status subscriptions for real-time updates
	- Subscriber controllers broadcast authentication events
	- WebSocket-based real-time communication
	
	\subsection imtauthgql_core_components Core Components
	
	The module provides GraphQL controllers and providers:
	
	<b>User Management Controllers:</b>
	\code
	CRemoteUserControllerComp (imtauth::ISuperuserProvider)
	├─ OnRegisterUser() - Create new user account
	├─ OnChangePassword() - Update user password
	├─ OnCheckEmail() - Validate email availability
	├─ OnSendEmailCode() - Email verification code
	├─ OnCheckEmailCode() - Verify email code
	└─ Remote user operations via GraphQL
	    │
	    ├─ CUserCollectionControllerComp - List/filter users
	    └─ CUserSettingsControllerComp - User preferences
	\endcode
	
	<b>Role Management Controllers:</b>
	\code
	CRoleRemoteCollectionControllerComp
	├─ List roles with filtering
	├─ Create and update roles
	├─ Assign permissions to roles
	└─ Role hierarchy management
	    │
	    └─ CClientRequestRoleManagerComp - Remote role operations
	\endcode
	
	<b>Permission Management:</b>
	\code
	CRemotePermissionCheckerComp (iauth::IPermissionChecker)
	├─ CheckPermission() - Remote permission validation
	├─ HasPermission() - Boolean permission check
	└─ Integration with authorization system
	    │
	    └─ CTokenBasedPermissionsProviderComp (iauth::IRightsProvider)
	        ├─ Token-based permission checking
	        ├─ JWT or session token validation
	        └─ Stateless authorization
	\endcode
	
	<b>Session and Token Management:</b>
	\code
	CPersonalAccessTokenControllerComp
	├─ GenerateToken() - Create personal access token
	├─ RevokeToken() - Invalidate token
	├─ ListTokens() - Get user's active tokens
	└─ Token expiration management
	    │
	    └─ CLoginStatusSubscriberControllerComp
	        ├─ OnLogin subscription - Real-time login events
	        ├─ OnLogout subscription - Real-time logout events
	        └─ WebSocket-based event broadcasting
	\endcode
	
	<b>Group Management:</b>
	\code
	CClientRequestGroupManagerComp
	├─ CreateGroup() - New group creation
	├─ AddUserToGroup() - Group membership
	├─ RemoveUserFromGroup() - Remove member
	└─ Group hierarchy operations
	\endcode
	
	<b>Data Providers:</b>
	\code
	CClientRequestUserInfoProviderComp (imtauth::IUserInfoProvider)
	├─ GetUserInfo() - Retrieve user details
	├─ GetUsersByIds() - Bulk user lookup
	└─ User data for GraphQL resolvers
	    │
	    ├─ CClientRequestRoleInfoProviderComp - Role information
	    └─ CClientRequestGroupInfoProviderComp - Group information
	\endcode
	
	<b>User Representation:</b>
	\code
	CUserRepresentationController
	├─ GetUserRepresentation() - User display info
	├─ Avatar and profile data
	└─ Public user information
	\endcode
	
	<b>User Actions:</b>
	\code
	CUserActionCollectionControllerComp
	├─ LogAction() - Record user action
	├─ GetActionHistory() - Query action log
	└─ Audit trail for compliance
	\endcode
	
	\section imtauthgql_graphql_schema GraphQL Schema Overview
	
	\subsection imtauthgql_queries Queries
	
	**User Queries:**
	\code{.graphql}
	type Query {
	    # Get current user
	    me: User
	    
	    # Get user by ID
	    user(id: ID!): User
	    
	    # List users with filtering
	    users(
	        filter: UserFilter
	        limit: Int
	        offset: Int
	    ): UserConnection
	    
	    # Check email availability
	    checkEmail(email: String!): CheckEmailPayload
	}
	\endcode
	
	**Role Queries:**
	\code{.graphql}
	type Query {
	    # Get role by ID
	    role(id: ID!): Role
	    
	    # List all roles
	    roles: [Role!]!
	    
	    # Check if user has permission
	    hasPermission(permission: String!): Boolean!
	}
	\endcode
	
	**Token Queries:**
	\code{.graphql}
	type Query {
	    # List user's personal access tokens
	    personalAccessTokens: [PersonalAccessToken!]!
	}
	\endcode
	
	\subsection imtauthgql_mutations Mutations
	
	**User Mutations:**
	\code{.graphql}
	type Mutation {
	    # Register new user
	    registerUser(
	        email: String!
	        password: String!
	        name: String!
	    ): RegisterUserPayload!
	    
	    # Change password
	    changePassword(
	        oldPassword: String!
	        newPassword: String!
	    ): ChangePasswordPayload!
	    
	    # Send email verification code
	    sendEmailCode(email: String!): SendEmailCodePayload!
	    
	    # Verify email code
	    checkEmailCode(
	        email: String!
	        code: String!
	    ): CheckEmailCodePayload!
	}
	\endcode
	
	**Token Mutations:**
	\code{.graphql}
	type Mutation {
	    # Generate personal access token
	    generatePersonalAccessToken(
	        name: String!
	        expiresIn: Int
	    ): GenerateTokenPayload!
	    
	    # Revoke token
	    revokePersonalAccessToken(
	        tokenId: ID!
	    ): RevokeTokenPayload!
	}
	\endcode
	
	\subsection imtauthgql_subscriptions Subscriptions
	
	**Real-time Authentication Events:**
	\code{.graphql}
	type Subscription {
	    # Subscribe to login events
	    onLogin: LoginEvent!
	    
	    # Subscribe to logout events
	    onLogout: LogoutEvent!
	    
	    # Subscribe to user status changes
	    onUserStatusChanged(userId: ID!): UserStatusEvent!
	}
	\endcode
	
	\section imtauthgql_usage Usage Examples
	
	\subsection imtauthgql_client_setup Client Setup
	
	**Setting up GraphQL Client:**
	\code{.cpp}
	// Create GraphQL client request manager
	auto clientRequestManager = CClientRequestManagerComp::CreateInstance();
	
	// Configure server connection
	auto serverConnection = CServerConnectionInterfaceParamComp::CreateInstance();
	serverConnection->SetHost("api.example.com");
	serverConnection->SetHttpPort(443);
	serverConnection->SetConnectionFlags(
	    imtcom::IServerConnectionInterface::CF_SECURE);
	
	clientRequestManager->SetAttribute("ServerConnection", serverConnection.get());
	
	// Setup authentication token
	QString authToken = GetAuthToken();
	clientRequestManager->SetAuthenticationToken(authToken);
	
	// Create user controller
	auto userController = CRemoteUserControllerComp::CreateInstance();
	userController->SetAttribute("ClientRequestManager", 
	                            clientRequestManager.get());
	\endcode
	
	\subsection imtauthgql_user_registration User Registration
	
	**Register New User:**
	\code{.cpp}
	// Prepare registration request
	sdl::V1_0::imtauth::CRegisterUserGqlRequest request;
	request.SetEmail("newuser@example.com");
	request.SetPassword("SecurePassword123!");
	request.SetName("John Doe");
	
	// Execute mutation
	QString errorMessage;
	auto payload = userController->OnRegisterUser(
	    request,
	    gqlRequest,
	    errorMessage);
	
	if (errorMessage.isEmpty()) {
	    qDebug() << "User registered successfully:" << payload.GetUserId();
	} else {
	    qCritical() << "Registration failed:" << errorMessage;
	}
	\endcode
	
	\subsection imtauthgql_password_change Password Change
	
	**Change User Password:**
	\code{.cpp}
	// Prepare change password request
	sdl::V1_0::imtauth::CChangePasswordGqlRequest request;
	request.SetOldPassword("OldPassword123");
	request.SetNewPassword("NewSecurePassword456!");
	
	// Execute mutation
	QString errorMessage;
	auto payload = userController->OnChangePassword(
	    request,
	    gqlRequest,
	    errorMessage);
	
	if (errorMessage.isEmpty()) {
	    qDebug() << "Password changed successfully";
	} else {
	    qCritical() << "Password change failed:" << errorMessage;
	}
	\endcode
	
	\subsection imtauthgql_permission_check Permission Checking
	
	**Check User Permission:**
	\code{.cpp}
	// Create remote permission checker
	auto permissionChecker = CRemotePermissionCheckerComp::CreateInstance();
	permissionChecker->SetAttribute("ClientRequestManager", 
	                               clientRequestManager.get());
	
	// Check permission
	QString userId = GetCurrentUserId();
	QString permission = "users.manage";
	
	bool hasPermission = permissionChecker->CheckPermission(
	    userId,
	    permission);
	
	if (hasPermission) {
	    // User has permission - show admin UI
	    ShowUserManagementInterface();
	} else {
	    // Permission denied
	    ShowAccessDeniedMessage();
	}
	\endcode
	
	\subsection imtauthgql_personal_tokens Personal Access Tokens
	
	**Generate Personal Access Token:**
	\code{.cpp}
	// Create token controller
	auto tokenController = CPersonalAccessTokenControllerComp::CreateInstance();
	tokenController->SetAttribute("ClientRequestManager", 
	                             clientRequestManager.get());
	
	// Generate token
	sdl::V1_0::imtauth::CGenerateTokenGqlRequest request;
	request.SetName("API Access Token");
	request.SetExpiresIn(2592000); // 30 days
	
	QString errorMessage;
	auto payload = tokenController->OnGenerateToken(
	    request,
	    gqlRequest,
	    errorMessage);
	
	if (errorMessage.isEmpty()) {
	    QString token = payload.GetToken();
	    qDebug() << "Generated token:" << token;
	    
	    // Store token securely for API access
	    StoreToken(token);
	} else {
	    qCritical() << "Token generation failed:" << errorMessage;
	}
	\endcode
	
	\subsection imtauthgql_login_subscription Login Status Subscription
	
	**Subscribe to Login Events:**
	\code{.cpp}
	// Create subscription controller
	auto loginSubscriber = CLoginStatusSubscriberControllerComp::CreateInstance();
	loginSubscriber->SetAttribute("ClientRequestManager", 
	                             clientRequestManager.get());
	
	// Connect to subscription signals
	connect(loginSubscriber, &CLoginStatusSubscriberControllerComp::OnLogin,
	        this, &MyApp::HandleUserLogin);
	connect(loginSubscriber, &CLoginStatusSubscriberControllerComp::OnLogout,
	        this, &MyApp::HandleUserLogout);
	
	// Start subscription
	loginSubscriber->SubscribeToLoginEvents();
	
	// Handler implementation
	void MyApp::HandleUserLogin(const LoginEvent& event)
	{
	    qDebug() << "User logged in:" << event.GetUserId()
	             << "from" << event.GetIpAddress();
	    
	    // Update UI to show online status
	    UpdateUserStatus(event.GetUserId(), "online");
	}
	\endcode
	
	\section imtauthgql_integration Integration Patterns
	
	\subsection imtauthgql_with_ui Integration with UI
	
	**Pattern: Login Form with GraphQL:**
	\code{.cpp}
	class CLoginFormComp : public ACF_COMPONENT(ILoginForm)
	{
	    I_REFERENCE(CRemoteUserControllerComp, m_userController)
	    
	    QPushButton* m_loginButton;
	    QLineEdit* m_emailEdit;
	    QLineEdit* m_passwordEdit;
	    
	public:
	    void OnLoginButtonClicked()
	    {
	        QString email = m_emailEdit->text();
	        QString password = m_passwordEdit->text();
	        
	        // Validate input
	        if (!ValidateEmail(email)) {
	            ShowError("Invalid email address");
	            return;
	        }
	        
	        // Prepare GraphQL request
	        sdl::V1_0::imtauth::CLoginGqlRequest request;
	        request.SetEmail(email);
	        request.SetPassword(password);
	        
	        // Show loading indicator
	        SetUIEnabled(false);
	        ShowLoadingSpinner();
	        
	        // Execute login mutation asynchronously
	        QString errorMessage;
	        auto future = QtConcurrent::run([this, request]() {
	            return m_userController->OnLogin(request, 
	                                             imtgql::CGqlRequest(), 
	                                             errorMessage);
	        });
	        
	        // Handle result
	        auto watcher = new QFutureWatcher<LoginPayload>(this);
	        connect(watcher, &QFutureWatcher::finished, [this, watcher]() {
	            auto payload = watcher->result();
	            
	            HideLoadingSpinner();
	            SetUIEnabled(true);
	            
	            if (payload.IsSuccess()) {
	                // Store auth token
	                QString token = payload.GetAuthToken();
	                StoreAuthToken(token);
	                
	                // Navigate to main app
	                emit LoginSuccessful();
	            } else {
	                ShowError(payload.GetErrorMessage());
	            }
	            
	            watcher->deleteLater();
	        });
	        
	        watcher->setFuture(future);
	    }
	};
	\endcode
	
	\subsection imtauthgql_token_auth Token-Based Authentication
	
	**Pattern: API Client with Token Auth:**
	\code{.cpp}
	class CApiClientComp : public ACF_COMPONENT(IApiClient)
	{
	    I_REFERENCE(CClientRequestManagerComp, m_requestManager)
	    I_REFERENCE(CTokenBasedPermissionsProviderComp, m_permissionsProvider)
	    
	    QString m_authToken;
	    
	public:
	    void SetAuthToken(const QString& token)
	    {
	        m_authToken = token;
	        m_requestManager->SetAuthenticationToken(token);
	        
	        // Load user permissions from token
	        LoadPermissionsFromToken(token);
	    }
	    
	    void LoadPermissionsFromToken(const QString& token)
	    {
	        // Token-based permissions provider validates JWT
	        m_permissionsProvider->SetToken(token);
	        
	        // Permissions are now available for checking
	        auto permissions = m_permissionsProvider->GetPermissions();
	        qDebug() << "Loaded permissions:" << permissions;
	    }
	    
	    bool CanAccessResource(const QString& resource)
	    {
	        // Check permission using token-based provider
	        return m_permissionsProvider->HasPermission(resource);
	    }
	};
	\endcode
	
	\section imtauthgql_best_practices Best Practices
	
	\subsection imtauthgql_security Security Considerations
	
	- Always use HTTPS/WSS for GraphQL communication
	- Implement rate limiting on authentication mutations
	- Validate all input data before processing
	- Use secure token generation (cryptographically random)
	- Implement token expiration and refresh mechanisms
	- Log all authentication attempts for security auditing
	- Sanitize error messages (don't leak sensitive information)
	
	\subsection imtauthgql_error_handling Error Handling
	
	- Return meaningful error messages in GraphQL responses
	- Use consistent error code conventions
	- Distinguish between client errors (400s) and server errors (500s)
	- Implement retry logic for transient failures
	- Log detailed error information server-side
	- Handle network timeouts gracefully
	
	\subsection imtauthgql_performance Performance Optimization
	
	- Batch user queries to reduce network round-trips
	- Implement DataLoader pattern for N+1 query prevention
	- Cache frequently accessed user/role data
	- Use subscriptions judiciously (resource intensive)
	- Implement pagination for large result sets
	- Monitor GraphQL query complexity
	
	\section imtauthgql_related_modules Integration with Other Modules
	
	<b>With imtauth (Core Authentication):</b>
	- GraphQL API layer over authentication domain
	- Remote access to authentication services
	- Token-based authorization
	
	<b>With imtauthdb (Database Layer):</b>
	- Data persistence for GraphQL mutations
	- Query resolution from database
	- Transaction support
	
	<b>With imtclientgql (GraphQL Client):</b>
	- Client request management
	- GraphQL query execution
	- WebSocket subscription handling
	
	<b>With imtgql (GraphQL Infrastructure):</b>
	- GraphQL request/response handling
	- Schema definition and validation
	- Resolver infrastructure
	
	\section imtauthgql_references References
	
	<b>Related Modules:</b>
	- imtauth - Core authentication domain
	- imtauthdb - Database persistence
	- imtclientgql - GraphQL client infrastructure
	- imtgql - GraphQL core services
	
	<b>GraphQL Specifications:</b>
	- GraphQL Specification: https://spec.graphql.org/
	- GraphQL Over WebSocket: https://github.com/apollographql/subscriptions-transport-ws
	
	<b>External Documentation:</b>
	- Qt Network Module: https://doc.qt.io/qt-6/qtnetwork-index.html
	- JWT (JSON Web Tokens): https://jwt.io/
*/

namespace imtauthgql
{


/**
	Create an optional tenant filter param from a tenant id.
	Returns nullptr when no tenant id is available.
	The returned pointer is owned by the caller (ParamsSet takes ownership via SetEditableParameter).
*/
inline imtauth::CTenantFilterParam* CreateTenantFilterParam(const QByteArray& tenantId)
{
	if (tenantId.isEmpty()){
		return nullptr;
	}

	imtauth::CTenantFilterParam* tenantFilterPtr = new imtauth::CTenantFilterParam();
	tenantFilterPtr->SetTenantId(tenantId);

	return tenantFilterPtr;
}


/**
	Create an optional tenant filter param from the GQL request context.
	Returns nullptr when no tenant context is available.
	The returned pointer is owned by the caller (ParamsSet takes ownership via SetEditableParameter).
*/
inline imtauth::CTenantFilterParam* CreateTenantFilterParam(const imtgql::CGqlRequest& gqlRequest)
{
	const imtgql::IGqlContext* gqlContextPtr = gqlRequest.GetRequestContext();
	if (gqlContextPtr == nullptr){
		return nullptr;
	}

	return CreateTenantFilterParam(gqlContextPtr->GetTenantId());
}


// --- SDL ↔ C++ Enum Converters ---

// Relationship Role

inline imtauth::ITenantRelationshipInfo::TenantRelationshipRole FromSdlRelationshipRole(sdl::V1_0::imtauth::TenantRelationshipRole role)
{
	switch (role){
	case sdl::V1_0::imtauth::TenantRelationshipRole::Parent:
		return imtauth::ITenantRelationshipInfo::TRR_PARENT;
	case sdl::V1_0::imtauth::TenantRelationshipRole::Child:
		return imtauth::ITenantRelationshipInfo::TRR_CHILD;
	case sdl::V1_0::imtauth::TenantRelationshipRole::Supplier:
		return imtauth::ITenantRelationshipInfo::TRR_SUPPLIER;
	case sdl::V1_0::imtauth::TenantRelationshipRole::Customer:
		return imtauth::ITenantRelationshipInfo::TRR_CUSTOMER;
	case sdl::V1_0::imtauth::TenantRelationshipRole::Affiliate:
		return imtauth::ITenantRelationshipInfo::TRR_AFFILIATE;
	default:
		return imtauth::ITenantRelationshipInfo::TRR_PARTNER;
	}
}


inline sdl::V1_0::imtauth::TenantRelationshipRole ToSdlRelationshipRole(imtauth::ITenantRelationshipInfo::TenantRelationshipRole role)
{
	switch (role){
	case imtauth::ITenantRelationshipInfo::TRR_PARENT:
		return sdl::V1_0::imtauth::TenantRelationshipRole::Parent;
	case imtauth::ITenantRelationshipInfo::TRR_CHILD:
		return sdl::V1_0::imtauth::TenantRelationshipRole::Child;
	case imtauth::ITenantRelationshipInfo::TRR_SUPPLIER:
		return sdl::V1_0::imtauth::TenantRelationshipRole::Supplier;
	case imtauth::ITenantRelationshipInfo::TRR_CUSTOMER:
		return sdl::V1_0::imtauth::TenantRelationshipRole::Customer;
	case imtauth::ITenantRelationshipInfo::TRR_AFFILIATE:
		return sdl::V1_0::imtauth::TenantRelationshipRole::Affiliate;
	default:
		return sdl::V1_0::imtauth::TenantRelationshipRole::Partner;
	}
}


// Relationship Status

inline sdl::V1_0::imtauth::RelationshipStatus ToSdlRelationshipStatus(imtauth::ITenantRelationshipInfo::TenantRelationshipStatus status)
{
	switch (status){
	case imtauth::ITenantRelationshipInfo::TRS_ARCHIVED:
		return sdl::V1_0::imtauth::RelationshipStatus::Archived;
	case imtauth::ITenantRelationshipInfo::TRS_PENDING_APPROVED:
		return sdl::V1_0::imtauth::RelationshipStatus::PendingApproval;
	default:
		return sdl::V1_0::imtauth::RelationshipStatus::Active;
	}
}


// Connection Request Status

inline sdl::V1_0::imtauth::ConnectionRequestStatus ToSdlConnectionRequestStatus(imtauth::ITenantConnectionRequestInfo::ConnectionRequestStatus status)
{
	switch (status){
	case imtauth::ITenantConnectionRequestInfo::CRS_APPROVED:
		return sdl::V1_0::imtauth::ConnectionRequestStatus::Approved;
	case imtauth::ITenantConnectionRequestInfo::CRS_REJECTED:
		return sdl::V1_0::imtauth::ConnectionRequestStatus::Rejected;
	case imtauth::ITenantConnectionRequestInfo::CRS_CANCELED:
		return sdl::V1_0::imtauth::ConnectionRequestStatus::Canceled;
	default:
		return sdl::V1_0::imtauth::ConnectionRequestStatus::Pending;
	}
}


// Connection Status

inline sdl::V1_0::imtauth::ConnectionStatus ToSdlConnectionStatus(imtauth::ITenantConnectionInfo::ConnectionStatus status)
{
	switch (status){
	case imtauth::ITenantConnectionInfo::CS_REMOVED:
		return sdl::V1_0::imtauth::ConnectionStatus::Removed;
	case imtauth::ITenantConnectionInfo::CS_SUSPENDED:
		return sdl::V1_0::imtauth::ConnectionStatus::Suspended;
	default:
		return sdl::V1_0::imtauth::ConnectionStatus::Active;
	}
}


// Relationship Proposal Type

inline sdl::V1_0::imtauth::RelationshipProposalType ToSdlProposalType(imtauth::ITenantRelationshipProposalInfo::RelationshipProposalType type)
{
	switch (type){
	case imtauth::ITenantRelationshipProposalInfo::RPT_UPDATE:
		return sdl::V1_0::imtauth::RelationshipProposalType::Update;
	case imtauth::ITenantRelationshipProposalInfo::RPT_DELETE:
		return sdl::V1_0::imtauth::RelationshipProposalType::Delete;
	default:
		return sdl::V1_0::imtauth::RelationshipProposalType::Create;
	}
}


inline imtauth::ITenantRelationshipProposalInfo::RelationshipProposalType FromSdlProposalType(sdl::V1_0::imtauth::RelationshipProposalType type)
{
	switch (type){
	case sdl::V1_0::imtauth::RelationshipProposalType::Update:
		return imtauth::ITenantRelationshipProposalInfo::RPT_UPDATE;
	case sdl::V1_0::imtauth::RelationshipProposalType::Delete:
		return imtauth::ITenantRelationshipProposalInfo::RPT_DELETE;
	default:
		return imtauth::ITenantRelationshipProposalInfo::RPT_CREATE;
	}
}


// Relationship Proposal Status

inline sdl::V1_0::imtauth::RelationshipProposalStatus ToSdlProposalStatus(imtauth::ITenantRelationshipProposalInfo::RelationshipProposalStatus status)
{
	switch (status){
	case imtauth::ITenantRelationshipProposalInfo::RPS_APPROVED_BY_INITIATOR:
		return sdl::V1_0::imtauth::RelationshipProposalStatus::ApprovedByInitiator;
	case imtauth::ITenantRelationshipProposalInfo::RPS_APPROVED_BY_COUNTERPARTY:
		return sdl::V1_0::imtauth::RelationshipProposalStatus::ApprovedByCounterparty;
	case imtauth::ITenantRelationshipProposalInfo::RPS_REJECTED:
		return sdl::V1_0::imtauth::RelationshipProposalStatus::Rejected;
	case imtauth::ITenantRelationshipProposalInfo::RPS_CANCELED:
		return sdl::V1_0::imtauth::RelationshipProposalStatus::Canceled;
	case imtauth::ITenantRelationshipProposalInfo::RPS_EXPIRED:
		return sdl::V1_0::imtauth::RelationshipProposalStatus::Expired;
	case imtauth::ITenantRelationshipProposalInfo::RPS_APPLIED:
		return sdl::V1_0::imtauth::RelationshipProposalStatus::Applied;
	default:
		return sdl::V1_0::imtauth::RelationshipProposalStatus::Pending;
	}
}


// Cross-Org Access Level

inline imtauth::CrossOrgAccessLevel FromSdlAccessLevel(sdl::V1_0::imtauth::CrossOrgAccessLevel level)
{
	switch (level){
	case sdl::V1_0::imtauth::CrossOrgAccessLevel::Read:
		return imtauth::COAL_READ;
	case sdl::V1_0::imtauth::CrossOrgAccessLevel::Write:
		return imtauth::COAL_WRITE;
	case sdl::V1_0::imtauth::CrossOrgAccessLevel::Admin:
		return imtauth::COAL_ADMIN;
	default:
		return imtauth::COAL_NONE;
	}
}


inline sdl::V1_0::imtauth::CrossOrgAccessLevel ToSdlAccessLevel(imtauth::CrossOrgAccessLevel level)
{
	switch (level){
	case imtauth::COAL_READ:
		return sdl::V1_0::imtauth::CrossOrgAccessLevel::Read;
	case imtauth::COAL_WRITE:
		return sdl::V1_0::imtauth::CrossOrgAccessLevel::Write;
	case imtauth::COAL_ADMIN:
		return sdl::V1_0::imtauth::CrossOrgAccessLevel::Admin;
	default:
		return sdl::V1_0::imtauth::CrossOrgAccessLevel::None;
	}
}


// Contract Status

inline imtauth::ContractStatus FromSdlContractStatus(sdl::V1_0::imtauth::ContractStatus status)
{
	switch (status){
	case sdl::V1_0::imtauth::ContractStatus::Active:
		return imtauth::CTS_ACTIVE;
	case sdl::V1_0::imtauth::ContractStatus::Expired:
		return imtauth::CTS_EXPIRED;
	case sdl::V1_0::imtauth::ContractStatus::Terminated:
		return imtauth::CTS_TERMINATED;
	case sdl::V1_0::imtauth::ContractStatus::Renewed:
		return imtauth::CTS_RENEWED;
	default:
		return imtauth::CTS_DRAFT;
	}
}


inline sdl::V1_0::imtauth::ContractStatus ToSdlContractStatus(imtauth::ContractStatus status)
{
	switch (status){
	case imtauth::CTS_ACTIVE:
		return sdl::V1_0::imtauth::ContractStatus::Active;
	case imtauth::CTS_EXPIRED:
		return sdl::V1_0::imtauth::ContractStatus::Expired;
	case imtauth::CTS_TERMINATED:
		return sdl::V1_0::imtauth::ContractStatus::Terminated;
	case imtauth::CTS_RENEWED:
		return sdl::V1_0::imtauth::ContractStatus::Renewed;
	default:
		return sdl::V1_0::imtauth::ContractStatus::Draft;
	}
}


// Cross-Tenant Message Type

inline sdl::V1_0::imtauth::CrossTenantMessageType ToSdlMessageType(imtauth::CrossTenantMessageType type)
{
	switch (type){
	case imtauth::CTMT_ORDER_REQUEST:
		return sdl::V1_0::imtauth::CrossTenantMessageType::OrderRequest;
	case imtauth::CTMT_ORDER_CONFIRMATION:
		return sdl::V1_0::imtauth::CrossTenantMessageType::OrderConfirmation;
	case imtauth::CTMT_ORDER_REJECTION:
		return sdl::V1_0::imtauth::CrossTenantMessageType::OrderRejection;
	case imtauth::CTMT_ORDER_STATUS_UPDATE:
		return sdl::V1_0::imtauth::CrossTenantMessageType::OrderStatusUpdate;
	case imtauth::CTMT_ORDER_CANCELLATION:
		return sdl::V1_0::imtauth::CrossTenantMessageType::OrderCancellation;
	case imtauth::CTMT_DOCUMENT_SHARE:
		return sdl::V1_0::imtauth::CrossTenantMessageType::DocumentShare;
	default:
		return sdl::V1_0::imtauth::CrossTenantMessageType::Custom;
	}
}


inline imtauth::CrossTenantMessageType FromSdlMessageType(sdl::V1_0::imtauth::CrossTenantMessageType type)
{
	switch (type){
	case sdl::V1_0::imtauth::CrossTenantMessageType::OrderRequest:
		return imtauth::CTMT_ORDER_REQUEST;
	case sdl::V1_0::imtauth::CrossTenantMessageType::OrderConfirmation:
		return imtauth::CTMT_ORDER_CONFIRMATION;
	case sdl::V1_0::imtauth::CrossTenantMessageType::OrderRejection:
		return imtauth::CTMT_ORDER_REJECTION;
	case sdl::V1_0::imtauth::CrossTenantMessageType::OrderStatusUpdate:
		return imtauth::CTMT_ORDER_STATUS_UPDATE;
	case sdl::V1_0::imtauth::CrossTenantMessageType::OrderCancellation:
		return imtauth::CTMT_ORDER_CANCELLATION;
	case sdl::V1_0::imtauth::CrossTenantMessageType::DocumentShare:
		return imtauth::CTMT_DOCUMENT_SHARE;
	default:
		return imtauth::CTMT_CUSTOM;
	}
}


// Cross-Tenant Message Status

inline sdl::V1_0::imtauth::CrossTenantMessageStatus ToSdlMessageStatus(imtauth::CrossTenantMessageStatus status)
{
	switch (status){
	case imtauth::CTMS_VALIDATED:
		return sdl::V1_0::imtauth::CrossTenantMessageStatus::Validated;
	case imtauth::CTMS_DELIVERED:
		return sdl::V1_0::imtauth::CrossTenantMessageStatus::Delivered;
	case imtauth::CTMS_ACKNOWLEDGED:
		return sdl::V1_0::imtauth::CrossTenantMessageStatus::Acknowledged;
	case imtauth::CTMS_PROCESSED:
		return sdl::V1_0::imtauth::CrossTenantMessageStatus::Processed;
	case imtauth::CTMS_FAILED:
		return sdl::V1_0::imtauth::CrossTenantMessageStatus::Failed;
	case imtauth::CTMS_EXPIRED:
		return sdl::V1_0::imtauth::CrossTenantMessageStatus::Expired;
	default:
		return sdl::V1_0::imtauth::CrossTenantMessageStatus::Created;
	}
}


inline imtauth::CrossTenantMessageStatus FromSdlMessageStatus(sdl::V1_0::imtauth::CrossTenantMessageStatus status)
{
	switch (status){
	case sdl::V1_0::imtauth::CrossTenantMessageStatus::Validated:
		return imtauth::CTMS_VALIDATED;
	case sdl::V1_0::imtauth::CrossTenantMessageStatus::Delivered:
		return imtauth::CTMS_DELIVERED;
	case sdl::V1_0::imtauth::CrossTenantMessageStatus::Acknowledged:
		return imtauth::CTMS_ACKNOWLEDGED;
	case sdl::V1_0::imtauth::CrossTenantMessageStatus::Processed:
		return imtauth::CTMS_PROCESSED;
	case sdl::V1_0::imtauth::CrossTenantMessageStatus::Failed:
		return imtauth::CTMS_FAILED;
	case sdl::V1_0::imtauth::CrossTenantMessageStatus::Expired:
		return imtauth::CTMS_EXPIRED;
	default:
		return imtauth::CTMS_CREATED;
	}
}


// Order Request Status

inline sdl::V1_0::imtauth::OrderRequestStatus ToSdlOrderStatus(imtauth::OrderRequestStatus status)
{
	switch (status){
	case imtauth::ORS_CONFIRMED:
		return sdl::V1_0::imtauth::OrderRequestStatus::Confirmed;
	case imtauth::ORS_REJECTED:
		return sdl::V1_0::imtauth::OrderRequestStatus::Rejected;
	case imtauth::ORS_IN_PROGRESS:
		return sdl::V1_0::imtauth::OrderRequestStatus::InProgress;
	case imtauth::ORS_COMPLETED:
		return sdl::V1_0::imtauth::OrderRequestStatus::Completed;
	case imtauth::ORS_CANCELLED:
		return sdl::V1_0::imtauth::OrderRequestStatus::Cancelled;
	default:
		return sdl::V1_0::imtauth::OrderRequestStatus::Received;
	}
}


inline imtauth::OrderRequestStatus FromSdlOrderStatus(sdl::V1_0::imtauth::OrderRequestStatus status)
{
	switch (status){
	case sdl::V1_0::imtauth::OrderRequestStatus::Confirmed:
		return imtauth::ORS_CONFIRMED;
	case sdl::V1_0::imtauth::OrderRequestStatus::Rejected:
		return imtauth::ORS_REJECTED;
	case sdl::V1_0::imtauth::OrderRequestStatus::InProgress:
		return imtauth::ORS_IN_PROGRESS;
	case sdl::V1_0::imtauth::OrderRequestStatus::Completed:
		return imtauth::ORS_COMPLETED;
	case sdl::V1_0::imtauth::OrderRequestStatus::Cancelled:
		return imtauth::ORS_CANCELLED;
	default:
		return imtauth::ORS_RECEIVED;
	}
}


} // namespace imtauthgql
