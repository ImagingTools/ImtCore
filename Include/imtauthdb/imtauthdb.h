// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// Qt includes
#include <QtCore/QByteArrayList>
#include <QtCore/QStringList>

// ACF includes
#include <iprm/CParamsSet.h>
#include <iprm/TParamsPtr.h>

// ImtCore includes
#include <imtbase/COperationContext.h>
#include <imtbase/COperationDescription.h>
#include <imtbase/IObjectCollection.h>
#include <imtbase/IObjectCollectionIterator.h>
#include <imtcol/CDocumentIdFilter.h>
#include <imtdb/ISqlDatabaseObjectDelegate.h>
#include <imtdb/imtdb.h>

/**
	\file imtauthdb.h
	\brief Main header file for the imtauthdb module - Authentication Database Persistence Layer
	
	This file provides namespace-level documentation for the imtauthdb module.
	Include this file to access the complete imtauthdb module documentation in Doxygen.
*/

/**
	\namespace imtauthdb
	\brief Authentication Database Persistence Layer Module
	
	The \b imtauthdb module provides SQL database persistence for the authentication
	and authorization system, storing users, roles, groups, permissions, sessions,
	and user settings in PostgreSQL or SQLite databases.
	
	\section imtauthdb_overview Overview
	
	This module acts as the persistence layer for imtauth, implementing database
	delegates that handle:
	- User account storage and retrieval
	- Role and permission management
	- User group membership
	- Session tracking and management
	- User settings and preferences persistence
	- Superuser administration
	- User action logging
	
	\section imtauthdb_architecture Architecture
	
	\subsection imtauthdb_patterns Design Patterns
	
	<b>Delegate Pattern:</b>
	- Database delegates separate business logic from SQL queries
	- Each entity type (User, Role, Group) has dedicated delegate
	- Delegates implement ISqlDatabaseObjectDelegate interface
	- Clean separation between data model and persistence
	
	<b>Strategy Pattern:</b>
	- Multiple database implementations (SQLite, PostgreSQL)
	- CSqlite*DatabaseDelegateComp for SQLite-specific SQL
	- Standard delegates for cross-database compatibility
	- Database-agnostic application code
	
	<b>Repository Pattern:</b>
	- Delegates act as repositories for authentication entities
	- CRUD operations through SQL queries
	- Complex filtering and sorting support
	- Transaction management integration
	
	<b>Provider Pattern:</b>
	- CSuperuserProviderComp provides superuser information
	- Abstraction over superuser storage
	- Facilitates testing and mocking
	
	\subsection imtauthdb_core_components Core Components
	
	The module provides database delegates for authentication entities:
	
	<b>User Management Delegates:</b>
	\code
	CUserDatabaseDelegateComp (imtdb::CSqlDatabaseDocumentDelegateComp)
	├─ CreateNewObjectQuery() - SQL INSERT for new users
	├─ CreateUpdateObjectQuery() - SQL UPDATE for user changes
	├─ CreateDeleteObjectsQuery() - SQL DELETE for users
	├─ CreateObjectFilterQuery() - SQL WHERE clauses for filtering
	├─ CreateJoinTablesQuery() - SQL JOIN with user connections (Postgres)
	└─ GetCustomColumnsQuery() - LastConnection column (Postgres)
	    Handles both SQLite and PostgreSQL via IsSQLite() branching
	\endcode
	
	<b>Role Management Delegates:</b>
	\code
	CRoleDatabaseDelegateComp (imtdb::CSqlDatabaseDocumentDelegateComp)
	├─ Manages role definitions in database
	├─ Role-permission associations
	├─ Role hierarchy queries
	└─ Permission lookup and validation
	    Handles both SQLite and PostgreSQL via IsSQLite() branching
	\endcode
	
	<b>Group Management Delegates:</b>
	\code
	CUserGroupDatabaseDelegateComp (imtdb::CSqlDatabaseDocumentDelegateComp)
	├─ User group storage
	├─ Group membership management
	├─ Group hierarchy support
	└─ Multi-level group queries
	    Handles both SQLite and PostgreSQL via IsSQLite() branching
	\endcode
	
	<b>Session Management Delegates:</b>
	\code
	CUsersSessionsDatabaseDelegateComp (imtdb::CSqlDatabaseDocumentDelegateComp)
	├─ Active session tracking
	├─ Session expiration management
	├─ Login/logout event logging
	└─ Concurrent session handling
	\endcode
	
	<b>Settings and Preferences:</b>
	\code
	CUsersSettingsDatabaseDelegateComp (imtdb::CSqlDatabaseDocumentDelegateComp)
	├─ User-specific settings storage
	├─ Key-value preference pairs
	├─ Settings serialization
	└─ Default settings management
	\endcode
	
	<b>User Action Logging:</b>
	\code
	CUserActionDatabaseDelegateComp (imtdb::CSqlDatabaseDocumentDelegateComp)
	├─ User activity logging
	├─ Audit trail creation
	├─ Action history queries
	└─ Compliance and forensics support
	\endcode
	
	<b>Administrative Services:</b>
	\code
	CSuperuserProviderComp (imtauth::ISuperuserProvider)
	├─ SuperuserExists() - Check if superuser account exists
	├─ GetSuperuserId() - Retrieve superuser account ID
	└─ Initial system setup support
	\endcode
	
	<b>Utility Components:</b>
	\code
	CUsersSqlPathExtractorComp
	├─ Extracts SQL paths for user queries
	├─ Dynamic SQL generation support
	└─ Query optimization helpers
	\endcode
	
	\section imtauthdb_integration Integration Patterns
	
	\subsection imtauthdb_with_imtauth Integration with imtauth
	
	**Pattern: User Collection with Database Persistence:**
	\code{.cpp}
	// Create database-backed user collection
	auto userCollection = CSqlDatabaseObjectCollectionComp::CreateInstance();
	
	// Configure database delegate
	auto userDelegate = CUserDatabaseDelegateComp::CreateInstance();
	userDelegate->SetAttribute("UserGroupDatabaseSqlDelegate", 
	                          groupDelegate.get());
	userDelegate->SetAttribute("UserGroupCollection", 
	                          groupCollection.get());
	
	// Assign delegate to collection
	userCollection->SetAttribute("Delegate", userDelegate.get());
	
	// Configure database connection
	auto databaseEngine = GetDatabaseEngine(); // PostgreSQL or SQLite
	userCollection->SetAttribute("DatabaseEngine", databaseEngine.get());
	
	// Now collection CRUD operations persist to database
	auto newUser = CUserInfo::CreateInstance();
	newUser->SetName("John Doe");
	newUser->SetEmail("john@example.com");
	
	// Automatically persisted to database
	userCollection->AddObject(newUser.get());
	\endcode
	
	\subsection imtauthdb_database_choice Database Selection
	
	**PostgreSQL (Production):**
	\code{.cpp}
	// Use standard delegates for PostgreSQL
	auto userDelegate = CUserDatabaseDelegateComp::CreateInstance();
	auto roleDelegate = CRoleDatabaseDelegateComp::CreateInstance();
	auto groupDelegate = CUserGroupDatabaseDelegateComp::CreateInstance();
	
	// PostgreSQL database engine
	auto dbEngine = CPostgreSqlDatabaseEngineComp::CreateInstance();
	dbEngine->SetHost("localhost");
	dbEngine->SetPort(5432);
	dbEngine->SetDatabaseName("authdb");
	dbEngine->SetUserName("auth_user");
	dbEngine->SetPassword("secure_password");
	
	// Connect collections to PostgreSQL
	userCollection->SetAttribute("DatabaseEngine", dbEngine.get());
	\endcode
	
	**SQLite (Embedded/Development):**
	\code{.cpp}
	// Use unified delegates (handle both SQLite and PostgreSQL via IsSQLite())
	auto userDelegate = CUserDatabaseDelegateComp::CreateInstance();
	auto roleDelegate = CRoleDatabaseDelegateComp::CreateInstance();
	auto groupDelegate = CUserGroupDatabaseDelegateComp::CreateInstance();
	
	// SQLite database engine
	auto dbEngine = CSqliteDatabaseEngineComp::CreateInstance();
	QString dbPath = QStandardPaths::writableLocation(
	    QStandardPaths::AppDataLocation) + "/auth.db";
	dbEngine->SetDatabaseName(dbPath);
	
	// Connect collections to SQLite
	userCollection->SetAttribute("DatabaseEngine", dbEngine.get());
	\endcode
	
	\subsection imtauthdb_sessions Session Management
	
	**Pattern: Session Tracking:**
	\code{.cpp}
	class CSessionManagerComp : public ACF_COMPONENT(ISessionManager)
	{
	    I_REFERENCE(ISqlDatabaseObjectCollection, m_sessionCollection)
	    I_REFERENCE(IUsersSessionsDatabaseDelegate, m_sessionDelegate)
	    
	public:
	    QString CreateSession(const QString& userId, 
	                         const QString& ipAddress)
	    {
	        // Create new session record
	        auto session = CUserSessionInfo::CreateInstance();
	        session->SetUserId(userId);
	        session->SetIpAddress(ipAddress);
	        session->SetLoginTime(QDateTime::currentDateTime());
	        session->SetExpirationTime(
	            QDateTime::currentDateTime().addSecs(3600)); // 1 hour
	        
	        // Generate session token
	        QString sessionToken = GenerateSecureToken();
	        session->SetToken(sessionToken);
	        
	        // Persist to database
	        m_sessionCollection->AddObject(session.get());
	        
	        return sessionToken;
	    }
	    
	    bool ValidateSession(const QString& sessionToken)
	    {
	        // Query database for session
	        auto filter = CreateFilterParams("token", sessionToken);
	        auto sessions = m_sessionCollection->GetObjectsByFilter(filter.get());
	        
	        if (sessions.isEmpty()) {
	            return false; // Session not found
	        }
	        
	        auto session = sessions.first();
	        QDateTime expiration = session->GetExpirationTime();
	        
	        if (expiration < QDateTime::currentDateTime()) {
	            // Session expired - remove from database
	            m_sessionCollection->DeleteObject(session->GetId());
	            return false;
	        }
	        
	        // Update last activity time
	        session->SetLastActivityTime(QDateTime::currentDateTime());
	        m_sessionCollection->UpdateObject(session.get());
	        
	        return true;
	    }
	    
	    void InvalidateSession(const QString& sessionToken)
	    {
	        auto filter = CreateFilterParams("token", sessionToken);
	        auto sessionIds = m_sessionCollection->GetIdsByFilter(filter.get());
	        
	        for (const QByteArray& sessionId : sessionIds) {
	            m_sessionCollection->DeleteObject(sessionId);
	        }
	    }
	    
	    void CleanupExpiredSessions()
	    {
	        // Find all expired sessions
	        QDateTime now = QDateTime::currentDateTime();
	        auto filter = CreateFilterParams("expiration_time", "<=", now);
	        
	        auto expiredIds = m_sessionCollection->GetIdsByFilter(filter.get());
	        
	        // Delete expired sessions
	        for (const QByteArray& sessionId : expiredIds) {
	            m_sessionCollection->DeleteObject(sessionId);
	        }
	        
	        qDebug() << "Cleaned up" << expiredIds.size() << "expired sessions";
	    }
	};
	\endcode
	
	\subsection imtauthdb_settings User Settings Persistence
	
	**Pattern: User Preferences:**
	\code{.cpp}
	class CUserPreferencesComp : public ACF_COMPONENT(IUserPreferences)
	{
	    I_REFERENCE(ISqlDatabaseObjectCollection, m_settingsCollection)
	    I_REFERENCE(IUsersSettingsDatabaseDelegate, m_settingsDelegate)
	    
	public:
	    void SaveUserSetting(const QString& userId,
	                        const QString& key,
	                        const QVariant& value)
	    {
	        // Check if setting already exists
	        auto filter = CreateFilterParams();
	        filter->SetValue("user_id", userId);
	        filter->SetValue("setting_key", key);
	        
	        auto existingIds = m_settingsCollection->GetIdsByFilter(filter.get());
	        
	        if (!existingIds.isEmpty()) {
	            // Update existing setting
	            auto setting = m_settingsCollection->GetObject(existingIds.first());
	            setting->SetValue(value);
	            m_settingsCollection->UpdateObject(setting.get());
	        } else {
	            // Create new setting
	            auto setting = CUserSettingInfo::CreateInstance();
	            setting->SetUserId(userId);
	            setting->SetKey(key);
	            setting->SetValue(value);
	            m_settingsCollection->AddObject(setting.get());
	        }
	    }
	    
	    QVariant LoadUserSetting(const QString& userId,
	                            const QString& key,
	                            const QVariant& defaultValue = QVariant())
	    {
	        auto filter = CreateFilterParams();
	        filter->SetValue("user_id", userId);
	        filter->SetValue("setting_key", key);
	        
	        auto settings = m_settingsCollection->GetObjectsByFilter(filter.get());
	        
	        if (settings.isEmpty()) {
	            return defaultValue;
	        }
	        
	        return settings.first()->GetValue();
	    }
	    
	    QMap<QString, QVariant> LoadAllUserSettings(const QString& userId)
	    {
	        QMap<QString, QVariant> settingsMap;
	        
	        auto filter = CreateFilterParams("user_id", userId);
	        auto settings = m_settingsCollection->GetObjectsByFilter(filter.get());
	        
	        for (auto setting : settings) {
	            settingsMap[setting->GetKey()] = setting->GetValue();
	        }
	        
	        return settingsMap;
	    }
	};
	\endcode
	
	\subsection imtauthdb_superuser Superuser Management
	
	**Pattern: Initial Setup Check:**
	\code{.cpp}
	class CApplicationBootstrapComp : public ACF_COMPONENT(IApplicationBootstrap)
	{
	    I_REFERENCE(ISuperuserProvider, m_superuserProvider)
	    I_REFERENCE(ISuperuserController, m_superuserController)
	    
	public:
	    bool InitializeApplication()
	    {
	        // Check if superuser exists
	        QString errorMessage;
	        auto status = m_superuserProvider->SuperuserExists(errorMessage);
	        
	        if (status == imtauth::ISuperuserProvider::ES_NOT_EXISTS) {
	            // No superuser - show initial setup wizard
	            return ShowInitialSetupWizard();
	        }
	        else if (status == imtauth::ISuperuserProvider::ES_EXISTS) {
	            // Superuser exists - proceed with normal login
	            return ShowLoginScreen();
	        }
	        else {
	            // Error checking - show error message
	            qCritical() << "Error checking superuser:" << errorMessage;
	            return false;
	        }
	    }
	    
	    bool ShowInitialSetupWizard()
	    {
	        // Collect superuser credentials from user
	        QString username, password, email;
	        
	        if (!GetSuperuserCredentialsFromUI(username, password, email)) {
	            return false;
	        }
	        
	        // Create superuser account
	        QString errorMessage;
	        bool success = m_superuserController->CreateSuperuser(
	            username, password, email, errorMessage);
	        
	        if (!success) {
	            qCritical() << "Failed to create superuser:" << errorMessage;
	            return false;
	        }
	        
	        qDebug() << "Superuser created successfully";
	        return true;
	    }
	};
	\endcode
	
	\section imtauthdb_examples Complete Examples
	
	\subsection imtauthdb_example_complete Complete Authentication System
	
	\code{.cpp}
	// Complete database-backed authentication system setup
	class CAuthenticationSystemComp : public ACF_COMPONENT(IAuthenticationSystem)
	{
	    // Database engine
	    I_REFERENCE(IDatabaseEngine, m_databaseEngine)
	    
	    // Collections
	    I_REFERENCE(ISqlDatabaseObjectCollection, m_userCollection)
	    I_REFERENCE(ISqlDatabaseObjectCollection, m_roleCollection)
	    I_REFERENCE(ISqlDatabaseObjectCollection, m_groupCollection)
	    I_REFERENCE(ISqlDatabaseObjectCollection, m_sessionCollection)
	    I_REFERENCE(ISqlDatabaseObjectCollection, m_settingsCollection)
	    
	    // Delegates
	    I_REFERENCE(ISqlDatabaseObjectDelegate, m_userDelegate)
	    I_REFERENCE(ISqlDatabaseObjectDelegate, m_roleDelegate)
	    I_REFERENCE(ISqlDatabaseObjectDelegate, m_groupDelegate)
	    I_REFERENCE(ISqlDatabaseObjectDelegate, m_sessionDelegate)
	    I_REFERENCE(ISqlDatabaseObjectDelegate, m_settingsDelegate)
	    
	    // Providers
	    I_REFERENCE(ISuperuserProvider, m_superuserProvider)
	    
	public:
	    bool InitializeSystem()
	    {
	        // Initialize database connection
	        if (!InitializeDatabaseConnection()) {
	            return false;
	        }
	        
	        // Setup user collection
	        SetupUserCollection();
	        
	        // Setup role collection
	        SetupRoleCollection();
	        
	        // Setup group collection
	        SetupGroupCollection();
	        
	        // Setup session collection
	        SetupSessionCollection();
	        
	        // Setup settings collection
	        SetupSettingsCollection();
	        
	        // Check superuser
	        CheckSuperuserStatus();
	        
	        return true;
	    }
	    
	private:
	    bool InitializeDatabaseConnection()
	    {
	        // Connect to database
	        bool connected = m_databaseEngine->Connect();
	        
	        if (!connected) {
	            qCritical() << "Failed to connect to database";
	            return false;
	        }
	        
	        // Run migrations if needed
	        RunDatabaseMigrations();
	        
	        return true;
	    }
	    
	    void SetupUserCollection()
	    {
	        m_userCollection->SetAttribute("DatabaseEngine", m_databaseEngine);
	        m_userCollection->SetAttribute("Delegate", m_userDelegate);
	        m_userCollection->SetAttribute("TableName", "users");
	    }
	    
	    void SetupRoleCollection()
	    {
	        m_roleCollection->SetAttribute("DatabaseEngine", m_databaseEngine);
	        m_roleCollection->SetAttribute("Delegate", m_roleDelegate);
	        m_roleCollection->SetAttribute("TableName", "roles");
	    }
	    
	    void SetupGroupCollection()
	    {
	        m_groupCollection->SetAttribute("DatabaseEngine", m_databaseEngine);
	        m_groupCollection->SetAttribute("Delegate", m_groupDelegate);
	        m_groupCollection->SetAttribute("TableName", "user_groups");
	    }
	    
	    void SetupSessionCollection()
	    {
	        m_sessionCollection->SetAttribute("DatabaseEngine", m_databaseEngine);
	        m_sessionCollection->SetAttribute("Delegate", m_sessionDelegate);
	        m_sessionCollection->SetAttribute("TableName", "user_sessions");
	        
	        // Setup automatic session cleanup
	        SetupSessionCleanupTimer();
	    }
	    
	    void SetupSettingsCollection()
	    {
	        m_settingsCollection->SetAttribute("DatabaseEngine", m_databaseEngine);
	        m_settingsCollection->SetAttribute("Delegate", m_settingsDelegate);
	        m_settingsCollection->SetAttribute("TableName", "user_settings");
	    }
	    
	    void CheckSuperuserStatus()
	    {
	        QString errorMessage;
	        auto status = m_superuserProvider->SuperuserExists(errorMessage);
	        
	        if (status == imtauth::ISuperuserProvider::ES_NOT_EXISTS) {
	            qWarning() << "No superuser account exists - initial setup required";
	        }
	    }
	};
	\endcode
	
	\section imtauthdb_database_schema Database Schema
	
	\subsection imtauthdb_tables Database Tables
	
	**users Table:**
	- id (UUID/TEXT PRIMARY KEY)
	- name (TEXT)
	- email (TEXT UNIQUE)
	- password_hash (TEXT)
	- created_at (TIMESTAMP)
	- updated_at (TIMESTAMP)
	- is_active (BOOLEAN)
	- data (JSON) - Serialized user object
	
	**roles Table:**
	- id (UUID/TEXT PRIMARY KEY)
	- name (TEXT UNIQUE)
	- description (TEXT)
	- permissions (JSON) - Array of permission strings
	- data (JSON) - Serialized role object
	
	**user_groups Table:**
	- id (UUID/TEXT PRIMARY KEY)
	- name (TEXT)
	- description (TEXT)
	- parent_group_id (UUID/TEXT FOREIGN KEY)
	- data (JSON) - Serialized group object
	
	**user_group_members Table:**
	- user_id (UUID/TEXT FOREIGN KEY)
	- group_id (UUID/TEXT FOREIGN KEY)
	- joined_at (TIMESTAMP)
	- PRIMARY KEY (user_id, group_id)
	
	**user_sessions Table:**
	- id (UUID/TEXT PRIMARY KEY)
	- user_id (UUID/TEXT FOREIGN KEY)
	- token (TEXT UNIQUE)
	- ip_address (TEXT)
	- user_agent (TEXT)
	- login_time (TIMESTAMP)
	- last_activity_time (TIMESTAMP)
	- expiration_time (TIMESTAMP)
	
	**user_settings Table:**
	- id (UUID/TEXT PRIMARY KEY)
	- user_id (UUID/TEXT FOREIGN KEY)
	- setting_key (TEXT)
	- setting_value (TEXT)
	- UNIQUE (user_id, setting_key)
	
	**user_actions Table:**
	- id (SERIAL/INTEGER PRIMARY KEY)
	- user_id (UUID/TEXT FOREIGN KEY)
	- action_type (TEXT)
	- action_data (JSON)
	- timestamp (TIMESTAMP)
	- ip_address (TEXT)
	
	\section imtauthdb_best_practices Best Practices
	
	\subsection imtauthdb_security Security Considerations
	
	- Never store passwords in plain text - always use hashing (see imtauth)
	- Use prepared statements to prevent SQL injection (handled by delegates)
	- Implement proper index management for query performance
	- Regularly clean up expired sessions to prevent table bloat
	- Use database transactions for multi-step operations
	- Implement proper access control on database level
	
	\subsection imtauthdb_performance Performance Optimization
	
	- Create indexes on frequently queried columns (email, token)
	- Use database connection pooling for concurrent access
	- Batch operations when possible instead of individual queries
	- Implement caching layer for frequently accessed data
	- Regular vacuum/analyze on PostgreSQL databases
	- Monitor slow query logs and optimize
	
	\subsection imtauthdb_maintenance Database Maintenance
	
	- Regular backups of authentication database
	- Implement database migration strategy
	- Version control for database schema changes
	- Test migrations in staging environment
	- Monitor database size and plan for growth
	- Archive old user action logs periodically
	
	\section imtauthdb_related_modules Integration with Other Modules
	
	<b>With imtauth (Authentication):</b>
	- Provides persistence for all auth entities
	- User, role, group, session storage
	- Superuser management integration
	
	<b>With imtdb (Database Layer):</b>
	- Uses ISqlDatabaseObjectCollection for CRUD
	- Extends CSqlDatabaseDocumentDelegateComp
	- Database engine abstraction
	
	<b>With imtauthgql (GraphQL API):</b>
	- Backend storage for GraphQL resolvers
	- Data persistence for API operations
	- Transaction support for mutations
	
	<b>With imtauthgui (UI Layer):</b>
	- Data backend for authentication UI
	- User management interface support
	- Settings persistence for UI preferences
	
	\section imtauthdb_references References
	
	<b>Related Modules:</b>
	- imtauth - Core authentication and authorization
	- imtdb - SQL database infrastructure
	- imtauthgql - GraphQL API layer
	- imtauthgui - Authentication UI components
	
	<b>ACF Interfaces:</b>
	- imtdb::ISqlDatabaseObjectDelegate - SQL delegate interface
	- imtdb::ISqlDatabaseObjectCollection - Database collection
	- imtdb::IDatabaseEngine - Database connection
	
	<b>External Documentation:</b>
	- PostgreSQL Documentation: https://www.postgresql.org/docs/
	- SQLite Documentation: https://www.sqlite.org/docs.html
	- Qt SQL Module: https://doc.qt.io/qt-6/qtsql-index.html
*/

namespace imtauthdb
{


/**
	Filter parameter (imtcol::IDocumentIdFilter) selecting users or groups holding any of the given roles.
*/
static const QByteArray s_roleFilterId = QByteArrayLiteral("RoleFilter");

/**
	Filter parameter (imtcol::IDocumentIdFilter) selecting roles or groups whose parents include any of the given ids.
*/
static const QByteArray s_parentFilterId = QByteArrayLiteral("ParentFilter");


inline QString JoinQuotedIds(const QByteArrayList& ids)
{
	QStringList quotedIds;
	for (const QByteArray& id : ids){
		quotedIds << QStringLiteral("'%1'").arg(imtdb::SqlEncode(QString::fromUtf8(id)));
	}

	return quotedIds.join(',');
}


/**
	SQL condition: the JSON array \c arrayKey of the document contains any of the ids.
*/
inline QString CreateArrayContainsAnyQuery(bool isSqlite, const QString& arrayKey, const QByteArrayList& ids)
{
	if (isSqlite){
		return QStringLiteral(R"(EXISTS (SELECT 1 FROM json_each(json_extract(root."Document", '$.%1')) AS item WHERE item.value IN (%2)))")
					.arg(arrayKey, JoinQuotedIds(ids));
	}

	return QStringLiteral(R"((root."Document"->'%1' ?| array[%2]))").arg(arrayKey, JoinQuotedIds(ids));
}


/**
	SQL condition: the per-product role lists of a user or a group contain any of the role ids.
*/
inline QString CreateProductRolesContainAnyQuery(bool isSqlite, const QByteArrayList& roleIds)
{
	if (isSqlite){
		return QStringLiteral(R"(EXISTS (SELECT 1 FROM json_each(json_extract(root."Document", '$.Products')) AS product, json_each(json_extract(product.value, '$.Roles')) AS role WHERE role.value IN (%1)))")
					.arg(JoinQuotedIds(roleIds));
	}

	return QStringLiteral(R"(EXISTS (SELECT 1 FROM jsonb_array_elements(coalesce(root."Document"->'Products', '[]'::jsonb)) AS product WHERE product->'Roles' ?| array[%1]))")
				.arg(JoinQuotedIds(roleIds));
}


/**
	Ids of the given document id filter parameter, empty if it is not set.
*/
inline QByteArrayList GetFilterIds(const iprm::IParamsSet& filterParams, const QByteArray& filterId)
{
	iprm::TParamsPtr<imtcol::IDocumentIdFilter> filterPtr(&filterParams, filterId);

	return filterPtr.IsValid() ? filterPtr->GetDocumentIds() : QByteArrayList();
}


/**
	Add a document id filter parameter with the given ids.
*/
inline void SetFilterIds(iprm::CParamsSet& filterParams, const QByteArray& filterId, const QByteArrayList& ids)
{
	istd::TUniqueInterfacePtr<imtcol::CDocumentIdFilter> filterPtr = new imtcol::CDocumentIdFilter();
	filterPtr->SetDocumentIds(ids);
	filterParams.SetEditableParameter(filterId, filterPtr.PopPtr(), true);
}


/**
	A reference dropped from a document, described the way the change generators of the document describe it.
*/
struct RemovedReference
{
	QByteArray operationTypeId;
	QByteArray key;
	QString keyName;
	QByteArray id;
};

typedef QList<RemovedReference> RemovedReferences;


/**
	Add to \c operationContext the owner and the tenant of \c sourceContextPtr and one reference change, so that
	an object updated because another object references it records that reference, not the operation of the source.
*/
inline void AddReferenceOperation(
			imtbase::COperationContext& operationContext,
			const imtbase::IOperationContext* sourceContextPtr,
			const QByteArray& operationTypeId,
			const QByteArray& key,
			const QString& keyName,
			const QByteArray& oldValue,
			const QByteArray& newValue)
{
	if (sourceContextPtr != nullptr){
		operationContext.SetOperationOwnerId(sourceContextPtr->GetOperationOwnerId());
		operationContext.SetTenantId(sourceContextPtr->GetTenantId());
	}

	imtbase::COperationDescription operationDescription;
	operationDescription.SetOperationTypeId(operationTypeId);
	operationDescription.SetKey(key);
	operationDescription.SetKeyName(keyName);
	operationDescription.SetOldValue(oldValue);
	operationDescription.SetNewValue(newValue);
	operationContext.GetChangesCollection()->InsertNewObject(QByteArrayLiteral("OperationInfo"), QString(), QString(), &operationDescription);
}


/**
	Create the queries storing the active objects selected by \c filterParams after \c removeFunction dropped
	their references to removed objects. \c removeFunction returns the dropped references; they become the
	revision description of the stored object, which is attributed to the owner of the removal.
	The objects in \c skippedIds are removed themselves and left as they are.
*/
template<class ObjectInterface, class RemoveFunction>
QByteArray CreateReferenceRemovalQuery(
			const imtbase::IObjectCollection& collection,
			const imtdb::ISqlDatabaseObjectDelegate& objectDelegate,
			const iprm::IParamsSet& filterParams,
			const imtbase::ICollectionInfo::Ids& skippedIds,
			RemoveFunction removeFunction,
			const imtbase::IOperationContext* operationContextPtr)
{
	QByteArray retVal;

	istd::TUniqueInterfacePtr<imtbase::IObjectCollectionIterator> iteratorPtr =
				collection.CreateObjectCollectionIterator(QByteArray(), 0, -1, &filterParams);
	if (!iteratorPtr.IsValid()){
		return retVal;
	}

	while (iteratorPtr->Next()){
		const QByteArray objectId = iteratorPtr->GetObjectId();
		if (skippedIds.contains(objectId)){
			continue;
		}

		imtbase::IObjectCollection::DataPtr dataPtr;
		if (!iteratorPtr->GetObjectData(dataPtr)){
			continue;
		}

		ObjectInterface* objectPtr = dataPtr.GetPtr<ObjectInterface>();
		if (objectPtr == nullptr){
			continue;
		}

		const RemovedReferences removedReferences = removeFunction(*objectPtr);
		if (removedReferences.isEmpty()){
			continue;
		}

		imtbase::COperationContext operationContext;
		for (const RemovedReference& removedReference : removedReferences){
			AddReferenceOperation(
						operationContext,
						operationContextPtr,
						removedReference.operationTypeId,
						removedReference.key,
						removedReference.keyName,
						removedReference.id,
						QByteArray());
		}

		retVal += objectDelegate.CreateUpdateObjectQuery(collection, objectId, *dataPtr, &operationContext, false);
	}

	return retVal;
}


} // namespace imtauthdb
