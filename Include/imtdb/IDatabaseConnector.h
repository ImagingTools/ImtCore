// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ACF includes
#include <istd/IPolymorphic.h>

// ImtCore includes
#include <imtdb/IDatabaseLoginSettings.h>


namespace imtdb
{


/**
 * @interface IDatabaseConnector
 * @brief Manages database connection lifecycle for remote database servers
 * 
 * IDatabaseConnector handles the establishment and termination of connections to
 * database servers. It abstracts the connection process and supports various database
 * backends through the IDatabaseLoginSettings interface.
 * 
 * Typical usage pattern:
 * @code{.cpp}
 * auto connector = acf::CreateComponent<imtdb::CDatabaseConnectorComp>();
 * 
 * // Create login settings
 * imtdb::CDatabaseAccessSettings settings;
 * settings.SetHost("localhost");
 * settings.SetPort(5432);
 * settings.SetDatabaseName("myapp");
 * settings.SetUserName("dbuser");
 * settings.SetPassword("secure_password");
 * 
 * // Connect
 * if (connector->ConnectToDatabase(settings)) {
 *     // Connection successful, can now execute queries
 * }
 * 
 * // Later, disconnect
 * connector->DisconnectFromDatabase(settings.GetConnectionName());
 * @endcode
 * 
 * @note Connection names allow multiple simultaneous connections to different databases
 * @see IDatabaseLoginSettings for connection configuration
 * @see CDatabaseConnectorComp for the implementation
 */
class IDatabaseConnector: virtual public istd::IPolymorphic
{
public:
	/**
	 * @brief Establishes a connection to a database server
	 * 
	 * Creates and opens a database connection using the provided login credentials.
	 * The connection is registered with Qt's SQL system and can be used for
	 * query execution.
	 * 
	 * @param loginSettings Configuration containing host, port, database name, credentials
	 * @return true if connection established successfully, false on failure
	 * 
	 * @note Connection failures may be due to:
	 *       - Invalid credentials
	 *       - Network connectivity issues
	 *       - Database server not running
	 *       - Insufficient permissions
	 * 
	 * @note The connection name from loginSettings determines the connection identifier.
	 *       Use unique connection names for multiple simultaneous database connections.
	 * 
	 * @see DisconnectFromDatabase()
	 * @see IDatabaseLoginSettings
	 */
	virtual bool ConnectToDatabase(const IDatabaseLoginSettings& loginSettings) const = 0;
	
	/**
	 * @brief Closes an existing database connection
	 * 
	 * Terminates the connection identified by the given connection name and
	 * releases associated resources.
	 * 
	 * @param connectionName The name of the connection to close (from IDatabaseLoginSettings)
	 * @return true if disconnection successful, false if connection not found or error occurred
	 * 
	 * @note Always disconnect when done to free resources
	 * @note Disconnecting closes all open transactions (may cause rollback)
	 * @note Safe to call even if connection is already closed
	 * 
	 * @see ConnectToDatabase()
	 */
	virtual bool DisconnectFromDatabase(const QString& connectionName) const = 0;
};


} // namespace imtdb


