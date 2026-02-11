// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// Qt includes
#include <QtSql/QtSql>

// ACF includes
#include <istd/IPolymorphic.h>


namespace imtdb
{


/**
 * @interface IDatabaseEngine
 * @brief Core database engine interface for low-level SQL execution and transaction management
 * 
 * IDatabaseEngine provides the fundamental database operations layer, handling:
 * - SQL query execution with parameterized binding
 * - Transaction lifecycle management (begin, commit, rollback)
 * - Query execution from files (SQL scripts)
 * - Forward-only result set optimization
 * 
 * This interface is database-agnostic and wraps Qt's QSqlDatabase functionality,
 * providing a consistent API across different database backends (PostgreSQL, SQLite, etc.).
 * 
 * @section idbe_transactions Transaction Management
 * 
 * The engine supports ACID-compliant transactions:
 * @code{.cpp}
 * if (engine->BeginTransaction()) {
 *     // Execute multiple operations
 *     engine->ExecSqlQuery("INSERT INTO users...");
 *     engine->ExecSqlQuery("UPDATE orders...");
 *     
 *     if (success) {
 *         engine->FinishTransaction();  // Commit
 *     } else {
 *         engine->CancelTransaction();  // Rollback
 *     }
 * }
 * @endcode
 * 
 * @section idbe_parameterized Parameterized Queries
 * 
 * The engine supports bound parameters to prevent SQL injection:
 * @code{.cpp}
 * QVariantMap bindings;
 * bindings[":name"] = userName;
 * bindings[":age"] = userAge;
 * engine->ExecSqlQuery("SELECT * FROM users WHERE name = :name AND age > :age", bindings);
 * @endcode
 * 
 * @note Implementations typically use Qt's QSqlDatabase under the hood.
 * @see CDatabaseEngineComp for the primary implementation
 * @see CSqlDatabaseObjectCollectionComp for usage in object collections
 */
class IDatabaseEngine: virtual public istd::IPolymorphic
{
public:
	/**
	 * @brief Begins a new database transaction
	 * 
	 * Starts a transaction on the underlying database connection. All subsequent SQL
	 * operations will be part of this transaction until either FinishTransaction()
	 * (commit) or CancelTransaction() (rollback) is called.
	 * 
	 * @return true if transaction started successfully, false otherwise
	 * 
	 * @note Transactions ensure ACID properties for database operations
	 * @note Nested transactions are not supported - begin/finish must be balanced
	 * @warning Always pair with FinishTransaction() or CancelTransaction()
	 * 
	 * @see FinishTransaction(), CancelTransaction()
	 */
	virtual bool BeginTransaction() const = 0;
	
	/**
	 * @brief Commits the current transaction
	 * 
	 * Commits all changes made during the current transaction to the database,
	 * making them permanent and visible to other connections.
	 * 
	 * @return true if transaction committed successfully, false if commit failed
	 * 
	 * @note If commit fails, changes are automatically rolled back
	 * @warning Must be called after BeginTransaction()
	 * 
	 * @see BeginTransaction(), CancelTransaction()
	 */
	virtual bool FinishTransaction() const = 0;
	
	/**
	 * @brief Rolls back the current transaction
	 * 
	 * Discards all changes made during the current transaction, reverting the
	 * database to its state before BeginTransaction() was called.
	 * 
	 * @return true if rollback succeeded, false otherwise
	 * 
	 * @note Use this when an error occurs during transaction to maintain data integrity
	 * @warning Must be called after BeginTransaction()
	 * 
	 * @see BeginTransaction(), FinishTransaction()
	 */
	virtual bool CancelTransaction() const = 0;
	
	/**
	 * @brief Executes a SQL query without parameters
	 * 
	 * Executes a raw SQL query string. The query can be SELECT, INSERT, UPDATE, DELETE,
	 * or any other valid SQL statement.
	 * 
	 * @param queryString The SQL query to execute (UTF-8 encoded)
	 * @param sqlError Optional pointer to receive error information if execution fails
	 * @param isForwardOnly If true, creates a forward-only result set (more efficient for large results)
	 * @return QSqlQuery object containing the results (check isActive() to verify success)
	 * 
	 * @warning For queries with user input, use the parameterized version to prevent SQL injection
	 * 
	 * @code{.cpp}
	 * QSqlError error;
	 * QSqlQuery result = engine->ExecSqlQuery("SELECT * FROM users", &error);
	 * if (result.isActive()) {
	 *     while (result.next()) {
	 *         QString name = result.value("name").toString();
	 *     }
	 * }
	 * @endcode
	 * 
	 * @see ExecSqlQuery(const QByteArray&, const QVariantMap&, QSqlError*, bool)
	 */
	virtual QSqlQuery ExecSqlQuery(const QByteArray& queryString, QSqlError* sqlError = nullptr, bool isForwardOnly = false) const = 0;
	
	/**
	 * @brief Executes a parameterized SQL query with bound values
	 * 
	 * Executes a SQL query with placeholder parameters that are safely bound to prevent
	 * SQL injection. Placeholders use the syntax `:paramName` and are matched with keys
	 * in the bindValues map.
	 * 
	 * @param queryString The SQL query with placeholders (e.g., "SELECT * FROM users WHERE id = :userId")
	 * @param bindValues Map of placeholder names to values (e.g., {":userId", 42})
	 * @param sqlError Optional pointer to receive error information if execution fails
	 * @param isForwardOnly If true, creates a forward-only result set (more efficient)
	 * @return QSqlQuery object containing the results
	 * 
	 * @note This is the preferred method for queries with dynamic values
	 * @note Parameter binding prevents SQL injection attacks
	 * 
	 * @code{.cpp}
	 * QVariantMap bindings;
	 * bindings[":name"] = userName;
	 * bindings[":minAge"] = 18;
	 * QSqlQuery result = engine->ExecSqlQuery(
	 *     "SELECT * FROM users WHERE name LIKE :name AND age >= :minAge",
	 *     bindings
	 * );
	 * @endcode
	 * 
	 * @see ExecSqlQuery(const QByteArray&, QSqlError*, bool)
	 */
	virtual QSqlQuery ExecSqlQuery(const QByteArray& queryString, const QVariantMap& bindValues, QSqlError* sqlError = nullptr, bool isForwardOnly = false) const = 0;
	
	/**
	 * @brief Executes SQL query from a file without parameters
	 * 
	 * Reads SQL content from a file and executes it. Useful for schema creation,
	 * migration scripts, and complex queries stored externally.
	 * 
	 * @param filePath Path to the SQL file (absolute or relative)
	 * @param sqlError Optional pointer to receive error information
	 * @param isForwardOnly If true, creates a forward-only result set
	 * @return QSqlQuery object containing the results
	 * 
	 * @note The file is read using UTF-8 encoding
	 * @note Comments and multi-line statements are supported
	 * 
	 * @code{.cpp}
	 * QSqlQuery result = engine->ExecSqlQueryFromFile("./migrations/v001_create_users.sql");
	 * @endcode
	 * 
	 * @see ExecSqlQueryFromFile(const QString&, const QVariantMap&, QSqlError*, bool)
	 */
	virtual QSqlQuery ExecSqlQueryFromFile(const QString& filePath, QSqlError* sqlError = nullptr, bool isForwardOnly = false) const = 0;
	
	/**
	 * @brief Executes parameterized SQL query from a file
	 * 
	 * Reads SQL content from a file and executes it with parameter binding.
	 * Combines the benefits of external SQL files with safe parameterization.
	 * 
	 * @param filePath Path to the SQL file containing placeholders
	 * @param bindValues Map of placeholder names to values
	 * @param sqlError Optional pointer to receive error information
	 * @param isForwardOnly If true, creates a forward-only result set
	 * @return QSqlQuery object containing the results
	 * 
	 * @note Useful for templated SQL scripts with runtime parameters
	 * 
	 * @code{.cpp}
	 * QVariantMap params;
	 * params[":schemaName"] = "public";
	 * QSqlQuery result = engine->ExecSqlQueryFromFile("./queries/list_tables.sql", params);
	 * @endcode
	 * 
	 * @see ExecSqlQueryFromFile(const QString&, QSqlError*, bool)
	 */
	virtual QSqlQuery ExecSqlQueryFromFile(const QString& filePath, const QVariantMap& bindValues, QSqlError* sqlError = nullptr, bool isForwardOnly = false) const = 0;
};


} // namespace imtdb


