// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once

/**
	\file imtauthgui.h
	\brief Main header file for the imtauthgui module - Authentication UI Components
	
	This file provides namespace-level documentation for the imtauthgui module.
	Include this file to access the complete imtauthgui module documentation in Doxygen.
*/

/**
	\namespace imtauthgui
	\brief Authentication UI Components Module
	
	The \b imtauthgui module provides Qt-based user interface components for
	authentication and authorization, including login screens, user management
	interfaces, role editors, and account settings dialogs.
	
	\section imtauthgui_overview Overview
	
	This module provides ready-to-use UI components for authentication:
	- Login and registration screens
	- User account information editors
	- Contact and address editors
	- Role and permission management UI
	- Server connection configuration dialogs
	- Account menu and profile widgets
	- User administration interfaces
	- QML-based observers for administration
	
	\section imtauthgui_architecture Architecture
	
	\subsection imtauthgui_patterns Design Patterns
	
	<b>Model-View-Controller (MVC):</b>
	- Controllers handle user interactions
	- Models provide data binding to auth domain
	- Views render UI elements (Qt Widgets/Designer)
	- Clean separation of concerns
	
	<b>Observer Pattern:</b>
	- QML observers for reactive UI updates
	- Administration observers monitor auth events
	- Change notification from domain models
	- Real-time UI synchronization
	
	<b>Delegate Pattern:</b>
	- View delegates for custom item rendering
	- Account info view delegates
	- Contact info view delegates
	- Consistent data presentation
	
	<b>Component Pattern (ACF):</b>
	- All UI components are ACF components
	- Dependency injection for services
	- Designer-based UI composition
	- Reusable widget library
	
	\subsection imtauthgui_core_components Core Components
	
	The module provides authentication UI components:
	
	<b>Login Components:</b>
	\code
	CStandardLoginGuiComp (iqtgui::TDesignerGuiCompBase)
	├─ User/password login form
	├─ Superuser password entry
	├─ Connection status display
	├─ Wait indicator for async operations
	└─ Login log message display
	    │
	    └─ CRemoteStandardLoginGuiComp
	        ├─ Remote authentication via GraphQL
	        ├─ Server connection configuration
	        └─ Network error handling
	\endcode
	
	<b>Account Management:</b>
	\code
	CAccountInfoEditorComp (iqtgui::TDesignerGuiCompBase)
	├─ Edit user account details
	├─ Name, email, password fields
	├─ Account status and permissions
	└─ Data validation and persistence
	    │
	    ├─ CAccountInfoViewDelegateComp - View delegate for accounts
	    └─ CStandardAccountMenuComp - Account menu widget
	\endcode
	
	<b>Contact Information:</b>
	\code
	CContactInfoEditorComp (iqtgui::TDesignerGuiCompBase)
	├─ Edit contact details
	├─ Phone, email, social media
	├─ Multiple contact methods
	└─ Validation and formatting
	    │
	    └─ CContactInfoViewDelegateComp - View delegate for contacts
	\endcode
	
	<b>Address Management:</b>
	\code
	CAddressEditorComp (iqtgui::TDesignerGuiCompBase)
	├─ Street, city, postal code fields
	├─ Country and region selection
	├─ Address validation
	└─ Google Maps integration
	\endcode
	
	<b>Server Configuration:</b>
	\code
	CServerConnectionEditorComp (iqtgui::TDesignerGuiCompBase)
	├─ Edit server connection settings
	├─ Host, port, protocol configuration
	├─ SSL/TLS settings
	└─ Connection testing
	\endcode
	
	<b>Administration Interfaces:</b>
	\code
	CAdministrationObserverQmlComp
	├─ QML-based administration observer
	├─ Real-time admin event monitoring
	├─ User action notifications
	└─ System status display
	\endcode
	
	\section imtauthgui_usage Usage Examples
	
	\subsection imtauthgui_login_screen Login Screen Setup
	
	**Standard Login Screen:**
	\code{.cpp}
	// Create login GUI
	auto loginGui = CStandardLoginGuiComp::CreateInstance();
	
	// Configure login service
	auto loginService = CLoginServiceComp::CreateInstance();
	loginGui->SetAttribute("Login", loginService.get());
	
	// Configure superuser provider
	auto superuserProvider = CSuperuserProviderComp::CreateInstance();
	loginGui->SetAttribute("SuperuserProvider", superuserProvider.get());
	
	// Configure connection status provider
	auto connectionStatus = CConnectionStatusProviderComp::CreateInstance();
	loginGui->SetAttribute("ConnectionStatusProvider", connectionStatus.get());
	
	// Show login window
	loginGui->Show();
	
	// Connect to login success signal
	connect(loginGui, &CStandardLoginGuiComp::LoginSuccessful,
	        this, &MyApp::OnLoginSuccess);
	\endcode
	
	\subsection imtauthgui_remote_login Remote Login
	
	**Remote Login with GraphQL:**
	\code{.cpp}
	// Create remote login GUI
	auto loginGui = CRemoteStandardLoginGuiComp::CreateInstance();
	
	// Configure server connection
	auto serverConnection = CServerConnectionInterfaceParamComp::CreateInstance();
	serverConnection->SetHost("auth.example.com");
	serverConnection->SetHttpPort(443);
	serverConnection->SetConnectionFlags(
	    imtcom::IServerConnectionInterface::CF_SECURE);
	
	loginGui->SetAttribute("ServerConnection", serverConnection.get());
	
	// Configure remote login service
	auto remoteLogin = CRemoteLoginServiceComp::CreateInstance();
	loginGui->SetAttribute("Login", remoteLogin.get());
	
	// Show login window
	loginGui->Show();
	\endcode
	
	\subsection imtauthgui_account_editor Account Editor Dialog
	
	**Edit User Account:**
	\code{.cpp}
	// Create account editor
	auto accountEditor = CAccountInfoEditorComp::CreateInstance();
	
	// Load user account
	auto userCollection = GetUserCollection();
	auto userId = GetCurrentUserId();
	auto userInfo = userCollection->GetObject(userId);
	
	// Set account for editing
	accountEditor->SetAccountInfo(userInfo.get());
	
	// Show editor dialog
	if (accountEditor->ShowDialog() == QDialog::Accepted) {
	    // User clicked OK - save changes
	    userCollection->UpdateObject(userInfo.get());
	    qDebug() << "Account updated successfully";
	}
	\endcode
	
	\subsection imtauthgui_contact_editor Contact Editor
	
	**Edit Contact Information:**
	\code{.cpp}
	// Create contact editor
	auto contactEditor = CContactInfoEditorComp::CreateInstance();
	
	// Load contact info
	auto contactInfo = userInfo->GetContactInfo();
	contactEditor->SetContactInfo(contactInfo.get());
	
	// Configure supported contact methods
	contactEditor->EnableEmailEditing(true);
	contactEditor->EnablePhoneEditing(true);
	contactEditor->EnableSocialMediaEditing(true);
	
	// Show editor
	if (contactEditor->ShowDialog() == QDialog::Accepted) {
	    // Save contact changes
	    userInfo->SetContactInfo(contactInfo.get());
	    SaveUserInfo(userInfo.get());
	}
	\endcode
	
	\subsection imtauthgui_address_editor Address Editor
	
	**Edit User Address:**
	\code{.cpp}
	// Create address editor
	auto addressEditor = CAddressEditorComp::CreateInstance();
	
	// Load address
	auto address = userInfo->GetAddress();
	addressEditor->SetAddress(address.get());
	
	// Configure address validation
	addressEditor->EnablePostalCodeValidation(true);
	addressEditor->EnableCountrySelection(true);
	
	// Show editor
	if (addressEditor->ShowDialog() == QDialog::Accepted) {
	    // Save address changes
	    userInfo->SetAddress(address.get());
	    SaveUserInfo(userInfo.get());
	}
	\endcode
	
	\subsection imtauthgui_server_config Server Configuration Dialog
	
	**Configure Server Connection:**
	\code{.cpp}
	// Create server connection editor
	auto serverEditor = CServerConnectionEditorComp::CreateInstance();
	
	// Load current connection settings
	auto serverConnection = LoadServerConnectionSettings();
	serverEditor->SetServerConnection(serverConnection.get());
	
	// Show configuration dialog
	if (serverEditor->ShowDialog() == QDialog::Accepted) {
	    // Test connection
	    if (serverEditor->TestConnection()) {
	        // Save new settings
	        SaveServerConnectionSettings(serverConnection.get());
	        qDebug() << "Server connection configured successfully";
	    } else {
	        QMessageBox::warning(nullptr, "Connection Failed",
	            "Failed to connect to server with provided settings");
	    }
	}
	\endcode
	
	\subsection imtauthgui_account_menu Account Menu Widget
	
	**User Account Menu:**
	\code{.cpp}
	// Create account menu widget
	auto accountMenu = CStandardAccountMenuComp::CreateInstance();
	
	// Configure current user
	auto currentUser = GetCurrentUserInfo();
	accountMenu->SetUserInfo(currentUser.get());
	
	// Add to toolbar or menu bar
	ui->toolBar->addWidget(accountMenu->GetWidget());
	
	// Connect to menu actions
	connect(accountMenu, &CStandardAccountMenuComp::EditProfileRequested,
	        this, &MyApp::OnEditProfile);
	connect(accountMenu, &CStandardAccountMenuComp::LogoutRequested,
	        this, &MyApp::OnLogout);
	connect(accountMenu, &CStandardAccountMenuComp::SettingsRequested,
	        this, &MyApp::OnShowSettings);
	\endcode
	
	\section imtauthgui_integration Integration Patterns
	
	\subsection imtauthgui_mvc_pattern MVC Integration
	
	**Pattern: User Management UI:**
	\code{.cpp}
	class CUserManagementWidgetComp : public ACF_COMPONENT(IUserManagementWidget)
	{
	    // Model
	    I_REFERENCE(IObjectCollection, m_userCollection)
	    
	    // View components
	    QTableView* m_usersTable;
	    QPushButton* m_addButton;
	    QPushButton* m_editButton;
	    QPushButton* m_deleteButton;
	    
	    // Editors
	    IAccountInfoEditorComp* m_accountEditor;
	    
	public:
	    void InitializeUI()
	    {
	        // Setup table view
	        m_usersTable = new QTableView();
	        
	        // Create model from collection
	        auto model = new QStandardItemModel();
	        LoadUsersIntoModel(model);
	        m_usersTable->setModel(model);
	        
	        // Setup buttons
	        m_addButton = new QPushButton("Add User");
	        m_editButton = new QPushButton("Edit User");
	        m_deleteButton = new QPushButton("Delete User");
	        
	        // Connect signals
	        connect(m_addButton, &QPushButton::clicked,
	                this, &CUserManagementWidgetComp::OnAddUser);
	        connect(m_editButton, &QPushButton::clicked,
	                this, &CUserManagementWidgetComp::OnEditUser);
	        connect(m_deleteButton, &QPushButton::clicked,
	                this, &CUserManagementWidgetComp::OnDeleteUser);
	        
	        // Create account editor
	        m_accountEditor = CAccountInfoEditorComp::CreateInstance();
	    }
	    
	    void OnAddUser()
	    {
	        // Create new user
	        auto newUser = CUserInfo::CreateInstance();
	        
	        // Show editor for new user
	        m_accountEditor->SetAccountInfo(newUser.get());
	        
	        if (m_accountEditor->ShowDialog() == QDialog::Accepted) {
	            // Add to collection
	            m_userCollection->AddObject(newUser.get());
	            
	            // Refresh view
	            RefreshUsersTable();
	        }
	    }
	    
	    void OnEditUser()
	    {
	        // Get selected user
	        QModelIndex index = m_usersTable->currentIndex();
	        if (!index.isValid()) return;
	        
	        QByteArray userId = GetUserIdFromIndex(index);
	        auto userInfo = m_userCollection->GetObject(userId);
	        
	        // Show editor
	        m_accountEditor->SetAccountInfo(userInfo.get());
	        
	        if (m_accountEditor->ShowDialog() == QDialog::Accepted) {
	            // Update collection
	            m_userCollection->UpdateObject(userInfo.get());
	            
	            // Refresh view
	            RefreshUsersTable();
	        }
	    }
	    
	    void OnDeleteUser()
	    {
	        // Confirm deletion
	        auto reply = QMessageBox::question(nullptr, "Confirm Delete",
	            "Are you sure you want to delete this user?",
	            QMessageBox::Yes | QMessageBox::No);
	        
	        if (reply == QMessageBox::Yes) {
	            QModelIndex index = m_usersTable->currentIndex();
	            QByteArray userId = GetUserIdFromIndex(index);
	            
	            // Delete from collection
	            m_userCollection->DeleteObject(userId);
	            
	            // Refresh view
	            RefreshUsersTable();
	        }
	    }
	};
	\endcode
	
	\subsection imtauthgui_qml_integration QML Integration
	
	**Pattern: QML Administration Observer:**
	\code{.cpp}
	// Create QML administration observer
	auto adminObserver = CAdministrationObserverQmlComp::CreateInstance();
	
	// Configure user collection for monitoring
	adminObserver->SetAttribute("UserCollection", userCollection.get());
	
	// Register with QML engine
	QQmlApplicationEngine engine;
	engine.rootContext()->setContextProperty("adminObserver", 
	                                        adminObserver->GetQmlObject());
	
	// Load QML file
	engine.load(QUrl("qrc:/qml/AdministrationPanel.qml"));
	\endcode
	
	**QML Usage:**
	\code{.qml}
	// AdministrationPanel.qml
	import QtQuick 2.15
	import QtQuick.Controls 2.15
	
	Item {
	    ListView {
	        model: adminObserver.userListModel
	        delegate: ItemDelegate {
	            text: model.userName
	            onClicked: adminObserver.selectUser(model.userId)
	        }
	    }
	    
	    Connections {
	        target: adminObserver
	        
	        function onUserAdded(userId, userName) {
	            console.log("User added:", userName)
	        }
	        
	        function onUserModified(userId, userName) {
	            console.log("User modified:", userName)
	        }
	        
	        function onUserDeleted(userId) {
	            console.log("User deleted:", userId)
	        }
	    }
	}
	\endcode
	
	\section imtauthgui_best_practices Best Practices
	
	\subsection imtauthgui_validation Input Validation
	
	- Validate email addresses before submission
	- Check password strength requirements
	- Validate phone number formats
	- Sanitize all user input
	- Provide real-time validation feedback
	- Use Qt validators where appropriate
	
	\subsection imtauthgui_security Security in UI
	
	- Mask password input fields
	- Clear sensitive data from UI on logout
	- Implement session timeout with warning
	- Disable auto-complete for passwords
	- Show security indicators (HTTPS lock)
	- Log all authentication attempts
	
	\subsection imtauthgui_ux User Experience
	
	- Provide clear error messages
	- Show loading indicators for async operations
	- Implement keyboard shortcuts
	- Support tab navigation between fields
	- Remember last used username (not password)
	- Provide "Forgot Password?" functionality
	
	\section imtauthgui_related_modules Integration with Other Modules
	
	<b>With imtauth (Core Authentication):</b>
	- UI for authentication domain objects
	- User, role, permission management
	- Login and session handling
	
	<b>With imtauthgql (GraphQL API):</b>
	- Remote authentication UI
	- GraphQL-based data operations
	- Real-time subscriptions
	
	<b>With imtcom (Server Communication):</b>
	- Server connection configuration UI
	- Connection status display
	- Network error handling
	
	<b>With imtgui (Core GUI):</b>
	- Base widget components
	- Designer-based UI composition
	- Common UI patterns
	
	\section imtauthgui_references References
	
	<b>Related Modules:</b>
	- imtauth - Core authentication domain
	- imtauthgql - GraphQL API layer
	- imtgui - Core GUI components
	- imtcom - Server communication
	
	<b>ACF Interfaces:</b>
	- iqtgui::TDesignerGuiCompBase - Designer UI base
	- iauth::ILogin - Login service interface
	
	<b>External Documentation:</b>
	- Qt Widgets: https://doc.qt.io/qt-6/qtwidgets-index.html
	- Qt Designer: https://doc.qt.io/qt-6/qtdesigner-manual.html
	- Qt QML: https://doc.qt.io/qt-6/qtqml-index.html
*/

namespace imtauthgui
{
	// Namespace for authentication UI components
}
