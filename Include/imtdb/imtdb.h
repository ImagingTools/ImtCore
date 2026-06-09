// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// Qt includes
#include <QtCore/QString>

// ImtCore includes
#include <imtdb/IDatabaseEngine.h>


/**
 * \namespace imtdb
 * \brief Database Abstraction Layer (ORM Framework) for ImtCore applications
 * 
 * The **imtdb** namespace provides a comprehensive database abstraction layer that enables
 * multi-database support, object-relational mapping, transaction management, and schema migrations.
 * 
 * \section imtdb_ns_overview Overview
 * 
 * This library offers:
 * - **Multi-database support**: PostgreSQL, SQLite, and file-based databases
 * - **Object-Relational Mapping (ORM)**: Automatic conversion between C++ objects and database records
 * - **SQL query generation**: Type-safe, parameterized query builders for CRUD operations
 * - **Transaction management**: ACID-compliant transactions with multi-collection coordination
 * - **Schema migrations**: Version-controlled database evolution with SQL scripts
 * - **Document management**: JSON-based serialization with rich metadata support
 * - **Revision control**: Complete audit trail and version history
 * - **Hierarchical data**: Tree structures with parent-child relationships
 * 
 * \section imtdb_ns_architecture Architecture
 * 
 * The library follows a layered architecture:
 * 
 * \verbatim
 * Application Layer (IObjectCollection)
 *          ↓
 * ORM Layer (ISqlDatabaseObjectDelegate)
 *          ↓
 * SQL Generation (Query Builders)
 *          ↓
 * Database Engine (IDatabaseEngine)
 *          ↓
 * Qt SQL Driver (QSqlDatabase)
 *          ↓
 * Database Server (PostgreSQL/SQLite)
 * \endverbatim
 * 
 * \section imtdb_ns_key_interfaces Key Interfaces
 * 
 * - imtdb::IDatabaseEngine - Low-level SQL execution and transaction management
 * - imtdb::IDatabaseConnector - Database connection lifecycle
 * - imtdb::ISqlDatabaseObjectCollection - Object collection management
 * - imtdb::ISqlDatabaseObjectDelegate - Object-to-SQL mapping strategy
 * - imtdb::IMigrationController - Schema versioning and migrations
 * - imtdb::ISqlJsonXPathExtractor - Database-agnostic JSON queries
 * 
 * \section imtdb_ns_patterns Design Patterns
 * 
 * - **Delegate Pattern**: Separates collection logic from database operations
 * - **Template Method**: Base classes define algorithms, subclasses implement specifics
 * - **Component Architecture**: ACF-based dependency injection
 * - **Observer Pattern**: Cascading metadata updates
 * - **Factory Pattern**: Polymorphic object creation from database records
 * 
 * \section imtdb_ns_usage Usage Example
 * 
 * \code{.cpp}
 * // Create and configure database engine
 * auto engine = acf::CreateComponent<imtdb::CDatabaseEngineComp>();
 * engine->SetDatabaseName("myapp.db");
 * engine->ConnectToDatabase();
 * 
 * // Create object collection
 * auto collection = acf::CreateComponent<imtdb::CSqlDatabaseObjectCollectionComp>();
 * auto delegate = acf::CreateComponent<imtdb::CSqlDatabaseDocumentDelegateComp>();
 * delegate->SetTableName("users");
 * collection->SetDelegate(delegate);
 * 
 * // Insert object
 * QString objectId;
 * collection->InsertNewObject(userObject, objectId);
 * 
 * // Query with filters
 * auto filters = acf::CreateComponent<iprm::CParamsSetComp>();
 * filters->SetValue("Name", "John%");
 * auto results = collection->CreateSubCollection(filters);
 * \endcode
 * 
 * \see \ref imtdb_architecture "Complete Architecture Documentation"
 * 
 * This package is system independent and provides platform-agnostic database abstractions.
 */
namespace imtdb
{


/**
 * \brief SQL NULL literal constant
 * 
 * This constant represents the SQL NULL value and should be used when constructing
 * SQL queries that need to explicitly set NULL values.
 */
static const QString NULL_DATA_LITERAL		= QStringLiteral("NULL");

/**
 * \brief SQL DEFAULT literal constant
 * 
 * This constant represents the SQL DEFAULT keyword and should be used when constructing
 * SQL queries that rely on column default values.
 */
static const QString DEFAULT_DATA_LITERAL	= QStringLiteral("DEFAULT");


/**
 * \brief Encodes a string for safe SQL usage
 * 
 * Escapes special characters in SQL strings to prevent SQL injection attacks and syntax errors.
 * Specifically:
 * - Single quotes (') are doubled ('')
 * - Semicolons (;) are replaced with backspace characters
 * 
 * \param sqlQuery The SQL string to encode
 * \return The encoded SQL string safe for use in queries
 * 
 * \warning This function provides basic escaping but should not be relied upon as the sole
 *          defense against SQL injection. Always use parameterized queries via IDatabaseEngine.
 * 
 * \note This is primarily used internally by query builders. Application code should use
 *       parameterized queries through the IDatabaseEngine interface instead of manual encoding.
 * 
 * Example:
 * \code{.cpp}
 * QString userInput = "O'Reilly";
 * QString encoded = imtdb::SqlEncode(userInput);
 * // encoded = "O''Reilly"
 * \endcode
 */
QString SqlEncode(const QString& sqlQuery);


/**
 * \brief Returns the resource path for an SQL script file based on the database driver.
 *
 * Selects between `:/SQL/SQLite/` and `:/SQL/Postgres/` prefixes depending on
 * whether the engine uses the `QSQLITE` driver.
 *
 * \param databaseEngine  The database engine to query for the driver ID.
 * \param fileName        The SQL script file name (e.g. "CreateUsersTable.sql").
 * \return Full resource path to the SQL script.
 */
QString GetSqlResourcePath(const IDatabaseEngine& databaseEngine, const QString& fileName);


/**
 * \brief Returns the current UTC date-time as an ISO 8601 string with milliseconds.
 *
 * Commonly used in SQL delegates to generate timestamps for CreatedAt/UpdatedAt columns.
 *
 * \return Current UTC timestamp in Qt::ISODateWithMs format (e.g. "2025-01-15T12:30:45.123Z").
 */
QString UtcNow();


/**
 * \brief Escapes single quotes in a string for safe use in SQL literals.
 *
 * Doubles single-quote characters to prevent SQL syntax errors. Unlike SqlEncode(),
 * this function does not touch semicolons, making it suitable for escaping user-provided
 * values that will be wrapped in single quotes in SQL statements.
 *
 * \param value The string to escape.
 * \return The escaped string with single quotes doubled.
 *
 * Example:
 * \code{.cpp}
 * QString name = "O'Reilly";
 * QString sql = QString("INSERT INTO t(name) VALUES('%1')").arg(imtdb::EscapeSql(name));
 * // sql = "INSERT INTO t(name) VALUES('O''Reilly')"
 * \endcode
 */
QString EscapeSql(const QString& value);


/**
 * \brief Safely converts a QVariant to QByteArray, handling QUuid without braces.
 *
 * In Qt 6.11+, database drivers may return UUID columns as QUuid values. When
 * QVariant::toByteArray() is called on such a value, the result is wrapped in
 * curly braces (e.g. "{xxxxxxxx-xxxx-xxxx-xxxx-xxxxxxxxxxxx}"), which breaks
 * existing data and comparison logic that expects braces-free UUID strings.
 *
 * This function detects QUuid-typed variants and converts them using
 * QUuid::WithoutBraces. For all other types, it falls through to
 * QVariant::toByteArray().
 *
 * \param value The QVariant value to convert.
 * \return The byte array representation without UUID braces.
 *
 * Example:
 * \code{.cpp}
 * QByteArray id = imtdb::VariantToByteArray(record.value("Id"));
 * \endcode
 */
QByteArray VariantToByteArray(const QVariant& value);


/**
 * \brief Returns "NULL" if the input string is empty, otherwise returns the string wrapped in single quotes.
 *
 * This is a utility function for constructing SQL queries where empty strings should be treated as NULL or DEFAULT values.
 * If the input string is not empty, it will be escaped for SQL and wrapped in single quotes.
 *
 * \param value The input string to evaluate.
 * \param useDefaultIfEmpty If true, returns "DEFAULT" when empty, otherwise "NULL".
 * \return "NULL" or "DEFAULT" if the input is empty, otherwise the escaped string wrapped in single quotes.
 *
 * Example:
 * \code{.cpp}
 * QString name = "";
 * QString sqlValue = imtdb::SqlValue(name); // sqlValue will be "NULL"
 * QString name2 = "John";
 * QString sqlValue2 = imtdb::SqlValue(name2); // sqlValue2 will be "'John'"
 */
QString SqlValue(const QString& value, bool useDefaultIfEmpty = false);


/**
 * \brief Returns "NULL" or "DEFAULT" if the input integer is 0, otherwise returns the number as a string.
 *
 * This is a utility function for constructing SQL queries where a zero value should be treated as a NULL or DEFAULT value.
 *
 * \param value The input integer to evaluate.
 * \param useDefaultIfEmpty If true, returns "DEFAULT" when 0, otherwise "NULL".
 * \return "NULL" or "DEFAULT" if the input is 0, otherwise the string representation of the integer.
 */
QString SqlValue(int value, bool useDefaultIfEmpty = false);


/**
 * \brief Returns "NULL" or "DEFAULT" if the input double is 0.0, otherwise returns the number as a string.
 *
 * This is a utility function for constructing SQL queries where a zero value should be treated as a NULL or DEFAULT value.
 *
 * \param value The input double to evaluate.
 * \param useDefaultIfEmpty If true, returns "DEFAULT" when 0.0, otherwise "NULL".
 * \return "NULL" or "DEFAULT" if the input is 0.0, otherwise the string representation of the double.
 */
QString SqlValue(double value, bool useDefaultIfEmpty = false);


} // namespace imtdb


