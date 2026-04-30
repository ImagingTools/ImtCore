// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// Qt includes
#include <QtCore/QByteArrayList>
#include <QtCore/QString>
#include <QtCore/QVariantList>

// ImtCore includes
#include <imtbase/ICollectionFilter.h>
#include <imtbase/IComplexCollectionFilter.h>


namespace imtdb
{


/**
 * @brief Centralized SQL query builder for collection filters.
 *
 * CFilterQueryBuilder translates an IComplexCollectionFilter (or legacy
 * ICollectionFilter) into parameterized SQL clauses — WHERE, ORDER BY, and
 * LIMIT/OFFSET — for either PostgreSQL or SQLite.
 *
 * It supports plain-column tables as well as JSON-document tables (via
 * SetJsonColumnName / SetUseJsonExtract).
 *
 * Usage example:
 * @code{.cpp}
 * CFilterQueryBuilder builder(CFilterQueryBuilder::DD_POSTGRESQL);
 * builder.SetJsonColumnName("Document");
 *
 * auto result = builder.Build(complexFilter);
 * QString sql = "SELECT * FROM \"Objects\"";
 * if (!result.whereClause.isEmpty())
 *     sql += " WHERE " + result.whereClause;
 * if (!result.orderByClause.isEmpty())
 *     sql += " " + result.orderByClause;
 * if (!result.limitClause.isEmpty())
 *     sql += " " + result.limitClause;
 * @endcode
 */
class CFilterQueryBuilder
{
public:
    /**
     * @brief Target database dialect.
     *
     * Determines the case-insensitive LIKE syntax used when building text
     * filter conditions.
     */
    enum DatabaseDialect
    {
        DD_POSTGRESQL, ///< Use ILIKE for case-insensitive matching
        DD_SQLITE      ///< Use LOWER(…) LIKE LOWER(…) for case-insensitive matching
    };

    /**
     * @brief SQL clause bundle produced by Build().
     */
    struct QueryResult
    {
        QString whereClause;    ///< Condition suitable for "WHERE …"  (no leading keyword)
        QString orderByClause;  ///< Clause suitable for "ORDER BY …" (includes keyword)
        QString limitClause;    ///< Clause suitable for "LIMIT … OFFSET …" (includes keyword)
        QVariantList bindValues;///< Reserved for future parameterised binding
    };

    /**
     * @param dialect Target database dialect. Defaults to DD_POSTGRESQL.
     */
    explicit CFilterQueryBuilder(DatabaseDialect dialect = DD_POSTGRESQL);

    // -----------------------------------------------------------------------
    // Build methods
    // -----------------------------------------------------------------------

    /**
     * @brief Build SQL clauses from an IComplexCollectionFilter.
     *
     * Produces a WHERE clause from the filter's text-filter and field-filter
     * expression, plus an ORDER BY clause from the field sorting meta-info.
     *
     * @param filter Source filter.
     * @return Populated QueryResult.
     */
    QueryResult Build(const imtbase::IComplexCollectionFilter& filter) const;

    /**
     * @brief Build SQL clauses from a legacy ICollectionFilter (adapter).
     *
     * Converts the legacy filter's text search and sort info into the
     * corresponding SQL clauses without requiring migration to the complex
     * interface.
     *
     * @param filter Legacy filter.
     * @return Populated QueryResult.
     */
    QueryResult Build(const imtbase::ICollectionFilter& filter) const;

    // -----------------------------------------------------------------------
    // Fine-grained building
    // -----------------------------------------------------------------------

    /**
     * @brief Build a text-search WHERE clause for specific columns.
     *
     * Generates "col1 ILIKE '%text%' OR col2 ILIKE '%text%' …" (PostgreSQL)
     * or the LOWER(…) LIKE LOWER(…) equivalent for SQLite.
     *
     * @param text      Search text.
     * @param columnIds List of quoted column identifiers.
     * @return WHERE condition string (empty when text or columnIds is empty).
     */
    QString BuildTextFilter(const QString& text, const QByteArrayList& columnIds) const;

    /**
     * @brief Recursively build a SQL condition from a FilterExpression tree.
     *
     * @param expr       Root expression node.
     * @param bindValues Accumulator for future parameterised values.
     * @return Parenthesised SQL condition string.
     */
    QString BuildFilterExpression(
        const imtbase::IComplexCollectionFilter::FilterExpression& expr,
        QVariantList& bindValues) const;

    /**
     * @brief Build an ORDER BY clause from IComplexCollectionFilter fields.
     *
     * Only fields whose meta-info has the SO_SORT flag set and whose
     * sortingOrder is not SO_NO_ORDER are included.
     *
     * @param fields Source field list.
     * @return "ORDER BY …" string (empty when no sortable fields exist).
     */
    QString BuildSortClause(const imtbase::IComplexCollectionFilter::Fields& fields) const;

    /**
     * @brief Build an ORDER BY clause from a legacy ICollectionFilter.
     *
     * @param filter Legacy filter.
     * @return "ORDER BY …" string.
     */
    QString BuildSortClause(const imtbase::ICollectionFilter& filter) const;

    // -----------------------------------------------------------------------
    // JSON-document configuration
    // -----------------------------------------------------------------------

    /**
     * @brief Set the JSON column name used for document-field access.
     *
     * When non-empty, text-filter conditions for fields not found in the
     * regular column list are emitted as JSON field expressions.
     *
     * @param columnName Column name, e.g. "Document" or "DataMetaInfo".
     */
    void SetJsonColumnName(const QString& columnName);

    /**
     * @brief Control JSON extraction syntax.
     *
     * @param useJsonExtract
     *   - @c true  — use SQLite `json_extract(col, '$.field')`
     *   - @c false — use PostgreSQL `->>` operator
     */
    void SetUseJsonExtract(bool useJsonExtract);

private:
    QString MakeLikeCondition(const QString& expression, const QString& pattern) const;
    QString MakeJsonFieldAccess(const QString& fieldId) const;
    QString EscapeLikePattern(const QString& text) const;

private:
    DatabaseDialect m_dialect;
    QString m_jsonColumnName;
    bool m_useJsonExtract = false;
};


} // namespace imtdb


