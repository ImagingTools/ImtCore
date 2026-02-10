// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// Qt includes
#include <QtCore/QString>


/**
 * @namespace imtdb
 * @brief Database Abstraction Layer (ORM Framework) for ImtCore applications
 * 
 * The **imtdb** namespace provides a comprehensive database abstraction layer that enables
 * multi-database support, object-relational mapping, transaction management, and schema migrations.
 * 
 * @section imtdb_ns_overview Overview
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
 * @section imtdb_ns_architecture Architecture
 * 
 * The library follows a layered architecture:
 * 
 * @verbatim
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
 * @endverbatim
 * 
 * @section imtdb_ns_key_interfaces Key Interfaces
 * 
 * - imtdb::IDatabaseEngine - Low-level SQL execution and transaction management
 * - imtdb::IDatabaseConnector - Database connection lifecycle
 * - imtdb::ISqlDatabaseObjectCollection - Object collection management
 * - imtdb::ISqlDatabaseObjectDelegate - Object-to-SQL mapping strategy
 * - imtdb::IMigrationController - Schema versioning and migrations
 * - imtdb::ISqlJsonXPathExtractor - Database-agnostic JSON queries
 * 
 * @section imtdb_ns_patterns Design Patterns
 * 
 * - **Delegate Pattern**: Separates collection logic from database operations
 * - **Template Method**: Base classes define algorithms, subclasses implement specifics
 * - **Component Architecture**: ACF-based dependency injection
 * - **Observer Pattern**: Cascading metadata updates
 * - **Factory Pattern**: Polymorphic object creation from database records
 * 
 * @section imtdb_ns_usage Usage Example
 * 
 * @code{.cpp}
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
 * @endcode
 * 
 * @see @ref imtdb_architecture "Complete Architecture Documentation"
 * 
 * This package is system independent and provides platform-agnostic database abstractions.
 */
namespace imtdb
{


/**
 * @brief SQL NULL literal constant
 * 
 * This constant represents the SQL NULL value and should be used when constructing
 * SQL queries that need to explicitly set NULL values.
 */
static const QString NULL_DATA_LITERAL		= QStringLiteral("NULL");

/**
 * @brief SQL DEFAULT literal constant
 * 
 * This constant represents the SQL DEFAULT keyword and should be used when constructing
 * SQL queries that rely on column default values.
 */
static const QString DEFAULT_DATA_LITERAL	= QStringLiteral("DEFAULT");


/**
 * @brief Encodes a string for safe SQL usage
 * 
 * Escapes special characters in SQL strings to prevent SQL injection attacks and syntax errors.
 * Specifically:
 * - Single quotes (') are doubled ('')
 * - Semicolons (;) are replaced with backspace characters
 * 
 * @param sqlQuery The SQL string to encode
 * @return The encoded SQL string safe for use in queries
 * 
 * @warning This function provides basic escaping but should not be relied upon as the sole
 *          defense against SQL injection. Always use parameterized queries via IDatabaseEngine.
 * 
 * @note This is primarily used internally by query builders. Application code should use
 *       parameterized queries through the IDatabaseEngine interface instead of manual encoding.
 * 
 * Example:
 * @code{.cpp}
 * QString userInput = "O'Reilly";
 * QString encoded = imtdb::SqlEncode(userInput);
 * // encoded = "O''Reilly"
 * @endcode
 */
QString SqlEncode(const QString& sqlQuery);


} // namespace imtdb


