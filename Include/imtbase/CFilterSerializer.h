// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// Qt includes
#include <QtCore/QString>
#include <QtCore/QJsonObject>

// ImtCore includes
#include <imtbase/IComplexCollectionFilter.h>
#include <imtbase/CComplexCollectionFilter.h>


namespace imtbase
{


/**
 * @brief Serializes and deserializes IComplexCollectionFilter to/from
 *        URL query-string and JSON formats.
 *
 * This allows filters to be persisted, shared as URL links, and transported
 * over HTTP/REST APIs.
 *
 * @section Filter URL format
 *
 * Text filter:
 * @code
 * text=john
 * @endcode
 *
 * Text filter with field IDs:
 * @code
 * text=john&textFields=Name,Email
 * @endcode
 *
 * Field filter (simple form):
 * @code
 * filter[Name][contains]=john&filter[Status][eq]=active
 * @endcode
 *
 * Sorting:
 * @code
 * sort=Name:asc,CreatedAt:desc
 * @endcode
 *
 * Pagination:
 * @code
 * page=2&limit=50
 * @endcode
 *
 * Full example:
 * @code
 * text=john&filter[Status][eq]=active&sort=Name:asc&page=2&limit=50
 * @endcode
 *
 * @section Filter JSON format
 *
 * @code{.json}
 * {
 *   "text": "john",
 *   "textFields": ["Name", "Email"],
 *   "sort": [{"field": "Name", "order": "asc"}],
 *   "filter": {
 *     "op": "and",
 *     "fields": [
 *       {"field": "Status", "op": "eq", "value": "active"}
 *     ]
 *   },
 *   "page": 2,
 *   "limit": 50
 * }
 * @endcode
 */
class CFilterSerializer
{
public:
    // -----------------------------------------------------------------------
    // URL query-string serialization
    // -----------------------------------------------------------------------

    /**
     * @brief Serialize a filter to a URL query string.
     *
     * @param filter   Source filter.
     * @param page     Current page (1-based).  Pass -1 to omit.
     * @param pageSize Page size.                Pass -1 to omit.
     * @return URL-encoded query string, e.g.
     *         "text=john&filter[Status][eq]=active&sort=Name:asc&page=2&limit=50"
     */
    static QString ToQueryString(
        const IComplexCollectionFilter& filter,
        int page = -1,
        int pageSize = -1);

    /**
     * @brief Deserialize a filter from a URL query string.
     *
     * Supported keys: text, textFields, filter[field][op], sort, page, limit.
     *
     * @param queryString Source query string (may start with '?').
     * @param filter      Output filter.
     * @return @c true on success, @c false if the string could not be parsed.
     */
    static bool FromQueryString(const QString& queryString, CComplexCollectionFilter& filter);

    // -----------------------------------------------------------------------
    // JSON serialization
    // -----------------------------------------------------------------------

    /**
     * @brief Serialize a filter to a JSON object.
     *
     * @param filter   Source filter.
     * @param page     Current page (1-based).  Pass -1 to omit.
     * @param pageSize Page size.                Pass -1 to omit.
     * @return JSON representation of the filter.
     */
    static QJsonObject ToJson(
        const IComplexCollectionFilter& filter,
        int page = -1,
        int pageSize = -1);

    /**
     * @brief Deserialize a filter from a JSON object.
     *
     * @param json   Source JSON.
     * @param filter Output filter.
     * @return @c true on success, @c false on parse error.
     */
    static bool FromJson(const QJsonObject& json, CComplexCollectionFilter& filter);

private:
    static QString SerializeFilterExpression(const IComplexCollectionFilter::FilterExpression& expr);
    static bool DeserializeFilterExpression(
        const QJsonObject& json,
        IComplexCollectionFilter::FilterExpression& expr);
    static QString FieldOperationToString(IComplexCollectionFilter::FieldOperation op);
    static IComplexCollectionFilter::FieldOperation StringToFieldOperation(const QString& str);
};


} // namespace imtbase


