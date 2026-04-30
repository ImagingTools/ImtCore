// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// Qt includes
#include <QtCore/QByteArrayList>
#include <QtCore/QVariant>
#include <QtCore/QVector>
#include <QtCore/QString>


namespace imtbase
{


/**
 * @brief Standalone collection filter model.
 *
 * CFilter is intentionally independent from the existing collection-filter
 * interfaces.  It is a small data model that can be serialized,
 * sent over URLs/API calls, and translated to SQL by imtdb::CFilterQueryBuilder.
 */
class CFilter
{
public:
    enum FilterOperation
    {
        FO_EQUAL = 0,
        FO_NOT_EQUAL,
        FO_LESS,
        FO_GREATER,
        FO_NOT_LESS,
        FO_NOT_GREATER,
        FO_CONTAINS
    };

    enum LogicalOperation
    {
        LO_AND = 0,
        LO_OR
    };

    enum SortingOrder
    {
        SO_NO_ORDER = 0,
        SO_ASC,
        SO_DESC
    };

    struct FieldFilter
    {
        FieldFilter(
            const QByteArray& fieldId = QByteArray(),
            const QVariant& value = QVariant(),
            FilterOperation operation = FO_EQUAL);

        bool operator==(const FieldFilter& other) const;
        bool operator!=(const FieldFilter& other) const;

        QByteArray fieldId;
        QVariant value;
        FilterOperation operation = FO_EQUAL;
    };

    struct FilterExpression
    {
        FilterExpression(
            const QVector<FieldFilter>& fieldFilters = QVector<FieldFilter>(),
            const QVector<FilterExpression>& filterExpressions = QVector<FilterExpression>(),
            LogicalOperation logicalOperation = LO_AND);

        bool operator==(const FilterExpression& other) const;
        bool operator!=(const FilterExpression& other) const;

        QVector<FieldFilter> fieldFilters;
        QVector<FilterExpression> filterExpressions;
        LogicalOperation logicalOperation = LO_AND;
    };

    struct SortField
    {
        SortField(
            const QByteArray& fieldId = QByteArray(),
            SortingOrder sortingOrder = SO_NO_ORDER);

        bool operator==(const SortField& other) const;
        bool operator!=(const SortField& other) const;

        QByteArray fieldId;
        SortingOrder sortingOrder = SO_NO_ORDER;
    };

    QString GetTextFilter() const;
    void SetTextFilter(const QString& textFilter);

    QByteArrayList GetTextFieldIds() const;
    void SetTextFieldIds(const QByteArrayList& fieldIds);

    const FilterExpression& GetFilterExpression() const;
    void SetFilterExpression(const FilterExpression& expression);
    void AddFieldFilter(const FieldFilter& filter);
    void AddFilterExpression(const FilterExpression& expression);

    QVector<SortField> GetSortFields() const;
    void SetSortFields(const QVector<SortField>& sortFields);
    void AddSortField(const SortField& sortField);

    int GetPage() const;
    void SetPage(int page);

    int GetPageSize() const;
    void SetPageSize(int pageSize);

    int GetOffset() const;
    int GetLimit() const;
    bool HasPagination() const;

    void Clear();

private:
    QString m_textFilter;
    QByteArrayList m_textFieldIds;
    FilterExpression m_filterExpression;
    QVector<SortField> m_sortFields;
    int m_page = -1;
    int m_pageSize = -1;
};


} // namespace imtbase


