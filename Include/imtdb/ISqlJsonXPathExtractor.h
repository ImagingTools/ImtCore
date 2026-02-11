// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// Qt includes
#include <QtCore/QMetaType>

// ACF includes
#include <istd/IPolymorphic.h>


namespace imtdb
{


/**
 * @interface ISqlJsonXPathExtractor
 * @brief Database-specific JSON field extraction abstraction
 * 
 * ISqlJsonXPathExtractor provides a database-agnostic interface for extracting values
 * from JSON/JSONB columns in SQL databases. Different databases use different syntax
 * for JSON operations (PostgreSQL uses operators, SQLite uses functions), and this
 * interface abstracts those differences.
 * 
 * @section isjx_purpose Purpose
 * 
 * Enables consistent JSON field access across database backends:
 * - PostgreSQL: Uses `->`, `->>`, `#>` operators for JSONB columns
 * - SQLite: Uses `json_extract()`, `json_type()` functions
 * 
 * @section isjx_implementations Implementations
 * 
 * - **CPostgresXPathExtractorComp**: PostgreSQL JSONB operators
 * - **CSqliteXPathExtractorComp**: SQLite JSON functions
 * 
 * @section isjx_usage Usage Pattern
 * 
 * The extractor is used internally by delegates to generate database-specific
 * JSON queries in WHERE clauses and SELECT statements:
 * 
 * @code{.cpp}
 * // PostgreSQL output: metadata->>'userName'
 * // SQLite output: json_extract(metadata, '$.userName')
 * QString extracted = extractor->ExtractXPath("metadata", "userName");
 * 
 * // Use in SQL query
 * QString whereClause = QString("WHERE %1 = 'john'").arg(extracted);
 * @endcode
 * 
 * @section isjx_path_syntax Path Syntax
 * 
 * The fieldId parameter uses dot notation for nested fields:
 * - Simple field: `"userName"`
 * - Nested field: `"address.city"`
 * - Array element: `"tags[0]"`
 * - Deep nesting: `"user.address.coordinates.latitude"`
 * 
 * @note Automatically handles database-specific escaping and type casting
 * @see CPostgresXPathExtractorComp for PostgreSQL implementation
 * @see CSqliteXPathExtractorComp for SQLite implementation
 */
class ISqlJsonXPathExtractor: virtual public istd::IPolymorphic
{
public:
	/**
	 * @brief Extracts a JSON field using database-specific syntax
	 * 
	 * Generates the appropriate SQL expression to extract a value from a JSON column
	 * based on the target database's syntax.
	 * 
	 * @param jsonKey The name of the JSON/JSONB column in the table (e.g., "metadata", "dataMetaInfo")
	 * @param fieldId The path to the field within the JSON (e.g., "userName", "address.city", "tags[0]")
	 * @param metaType Expected Qt type of the extracted value (for type casting)
	 * @param tableAlias Optional table alias prefix (e.g., "t1" for "t1.metadata")
	 * @return SQL expression string for extracting the JSON field
	 * 
	 * Examples:
	 * 
	 * **PostgreSQL:**
	 * @code{.cpp}
	 * // Extract string value
	 * QString expr = extractor->ExtractXPath("metadata", "userName");
	 * // Returns: metadata->>'userName'
	 * 
	 * // Extract nested value
	 * QString expr = extractor->ExtractXPath("metadata", "address.city");
	 * // Returns: metadata#>>'{address,city}'
	 * 
	 * // Extract with table alias
	 * QString expr = extractor->ExtractXPath("metadata", "age", QMetaType::Int, "users");
	 * // Returns: (users.metadata->>'age')::integer
	 * @endcode
	 * 
	 * **SQLite:**
	 * @code{.cpp}
	 * // Extract string value
	 * QString expr = extractor->ExtractXPath("metadata", "userName");
	 * // Returns: json_extract(metadata, '$.userName')
	 * 
	 * // Extract nested value
	 * QString expr = extractor->ExtractXPath("metadata", "address.city");
	 * // Returns: json_extract(metadata, '$.address.city')
	 * 
	 * // Extract with table alias
	 * QString expr = extractor->ExtractXPath("metadata", "age", QMetaType::Int, "users");
	 * // Returns: CAST(json_extract(users.metadata, '$.age') AS INTEGER)
	 * @endcode
	 * 
	 * @note The metaType parameter enables proper type casting (important for numeric comparisons)
	 * @note Array access syntax varies by database and is handled internally
	 * @note Nested fields are properly escaped to prevent SQL injection
	 * 
	 * @see CPostgresXPathExtractorComp, CSqliteXPathExtractorComp
	 */
	virtual QString ExtractXPath(const QString& jsonKey, const QString& fieldId, QMetaType::Type metaType = QMetaType::QString, const QString& tableAlias = QString()) const = 0;
};


} // namespace imtdb


