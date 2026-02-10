// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ACF includes
#include <istd/IChangeable.h>


#ifdef Q_OS_WIN
	#undef SetPort
	#undef GetUserName
#endif


namespace imtdb
{


/**
 * @interface IDatabaseLoginSettings
 * @brief Configuration interface for database connection credentials and parameters
 * 
 * IDatabaseLoginSettings encapsulates all information required to establish a database
 * connection, including host, port, database name, credentials, and connection options.
 * 
 * @section idls_overview Connection Types
 * 
 * The interface supports multiple database types:
 * 
 * **Remote Server Databases (PostgreSQL, MySQL):**
 * - Host/Port: Server address and port number
 * - DatabaseName: Specific database on the server
 * - UserName/Password: Authentication credentials
 * - ConnectionFlags: SSL/TLS options
 * 
 * **File-Based Databases (SQLite):**
 * - DatabasePath: File system path to database file
 * - DatabaseName: May be used as connection identifier
 * - UserName/Password: Typically unused
 * 
 * @section idls_security Security Options
 * 
 * ConnectionOptionFlags control security features:
 * - **COF_SSL**: Enable SSL/TLS encryption for network connections
 * - **COF_NO_SECURITY**: Disable security (development/testing only)
 * 
 * @section idls_usage Usage Example
 * 
 * @code{.cpp}
 * // PostgreSQL connection
 * auto settings = acf::CreateComponent<CDatabaseAccessSettingsComp>();
 * settings->SetHost("db.example.com");
 * settings->SetPort(5432);
 * settings->SetDatabaseName("production_db");
 * settings->SetUserName("app_user");
 * settings->SetPassword("secure_password");
 * settings->SetConnectionFlags(IDatabaseLoginSettings::COF_SSL);
 * 
 * // SQLite connection
 * auto sqliteSettings = acf::CreateComponent<CDatabaseAccessSettingsComp>();
 * sqliteSettings->SetDatabasePath("/var/data/app.sqlite");
 * sqliteSettings->SetDatabaseName("app_db");
 * @endcode
 * 
 * @warning Store passwords securely - never hardcode or commit to source control
 * @note Settings implement IChangeable to notify observers of configuration changes
 * @see CDatabaseAccessSettings for implementation
 * @see IDatabaseConnector for usage in connection establishment
 */
class IDatabaseLoginSettings: virtual public istd::IChangeable
{
public:
	/**
	 * @enum ConnectionOptionFlags
	 * @brief Flags controlling database connection security and behavior
	 */
	enum ConnectionOptionFlags
	{
        /**
         * @brief Disable connection security (insecure mode)
         * 
         * When enabled, connection will not use encryption or authentication.
         * **Only use for development/testing on trusted networks.**
         */
        COF_NO_SECURITY = 1,

        /**
		 * @brief Enable SSL/TLS encrypted connection
		 * 
		 * When enabled, connection uses SSL/TLS encryption to protect data in transit.
		 * **Recommended for production environments and remote connections.**
		 */
        COF_SSL = 2
	};

    I_DECLARE_FLAGS(ConnectionOptionFlags, COF_NO_SECURITY, COF_SSL);

	/**
	 * @brief Gets the database server hostname or IP address
	 * @return Host address (e.g., "localhost", "db.example.com", "192.168.1.100")
	 * @see SetHost()
	 */
	virtual QString GetHost() const = 0;
	
	/**
	 * @brief Sets the database server hostname or IP address
	 * @param host Server address (e.g., "localhost", "db.example.com")
	 * @see GetHost()
	 */
	virtual void SetHost(const QString& host) = 0;
	
	/**
	 * @brief Gets the database server port number
	 * @return Port number (e.g., 5432 for PostgreSQL, 3306 for MySQL)
	 * @see SetPort()
	 */
	virtual int GetPort() const = 0;
	
	/**
	 * @brief Sets the database server port number
	 * @param port Server port (e.g., 5432 for PostgreSQL, 3306 for MySQL)
	 * @see GetPort()
	 */
	virtual void SetPort(int port) = 0;
	
	/**
	 * @brief Gets the name of the database to connect to
	 * @return Database name (e.g., "myapp", "production_db")
	 * @note For SQLite, this may serve as a connection identifier
	 * @see SetDatabaseName()
	 */
	virtual QString GetDatabaseName() const = 0;
	
	/**
	 * @brief Sets the name of the database to connect to
	 * @param databaseName Database name on the server
	 * @see GetDatabaseName()
	 */
	virtual void SetDatabaseName(const QString& databaseName) = 0;
	
	/**
	 * @brief Gets the file system path for file-based databases
	 * @return Absolute or relative path to database file (used for SQLite)
	 * @note Empty for server-based databases (PostgreSQL, MySQL)
	 * @see SetDatabasePath()
	 */
	virtual QString GetDatabasePath() const = 0;
	
	/**
	 * @brief Sets the file system path for file-based databases
	 * @param databasePath Path to database file (e.g., "/var/data/app.sqlite")
	 * @note Only applicable for SQLite and similar file-based databases
	 * @see GetDatabasePath()
	 */
	virtual void SetDatabasePath(const QString& databasePath) = 0;
	
	/**
	 * @brief Gets the database authentication username
	 * @return Username for database login
	 * @see SetUserName()
	 */
	virtual QString GetUserName() const = 0;
	
	/**
	 * @brief Sets the database authentication username
	 * @param userName Username for database authentication
	 * @see GetUserName()
	 */
	virtual void SetUserName(const QString& userName) = 0;
	
	/**
	 * @brief Gets the database authentication password
	 * @return Password for database login
	 * @warning Passwords should be encrypted/secured in production
	 * @see SetPassword()
	 */
	virtual QString GetPassword() const = 0;
	
	/**
	 * @brief Sets the database authentication password
	 * @param password Password for database authentication
	 * @warning Never hardcode passwords - load from secure configuration
	 * @see GetPassword()
	 */
	virtual void SetPassword(const QString& password) = 0;
	
	/**
	 * @brief Gets the connection option flags
	 * @return Bitwise OR of ConnectionOptionFlags values
	 * @see SetConnectionFlags(), ConnectionOptionFlags
	 */
	virtual int GetConnectionFlags() const = 0;
	
	/**
	 * @brief Sets the connection option flags
	 * @param connectionFlags Bitwise OR of ConnectionOptionFlags values
	 * 
	 * Example:
	 * @code{.cpp}
	 * // Enable SSL
	 * settings->SetConnectionFlags(IDatabaseLoginSettings::COF_SSL);
	 * 
	 * // Multiple flags
	 * settings->SetConnectionFlags(
	 *     IDatabaseLoginSettings::COF_SSL | 
	 *     IDatabaseLoginSettings::COF_NO_SECURITY
	 * );
	 * @endcode
	 * 
	 * @see GetConnectionFlags(), ConnectionOptionFlags
	 */
	virtual void SetConnectionFlags(int connectionFlags) = 0;
};


} // namespace imtdb


