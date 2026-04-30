// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtbase/CFilter.h>


// Qt includes
#include <QtCore/QtGlobal>


namespace imtbase
{


CFilter::FieldFilter::FieldFilter(
    const QByteArray& fieldId,
    const QVariant& value,
    FilterOperation operation)
    : fieldId(fieldId)
    , value(value)
    , operation(operation)
{
}


bool CFilter::FieldFilter::operator==(const FieldFilter& other) const
{
    return fieldId == other.fieldId &&
        value == other.value &&
        operation == other.operation;
}


bool CFilter::FieldFilter::operator!=(const FieldFilter& other) const
{
    return !(*this == other);
}


CFilter::FilterExpression::FilterExpression(
    const QVector<FieldFilter>& fieldFilters,
    const QVector<FilterExpression>& filterExpressions,
    LogicalOperation logicalOperation)
    : fieldFilters(fieldFilters)
    , filterExpressions(filterExpressions)
    , logicalOperation(logicalOperation)
{
}


bool CFilter::FilterExpression::operator==(const FilterExpression& other) const
{
    return fieldFilters == other.fieldFilters &&
        filterExpressions == other.filterExpressions &&
        logicalOperation == other.logicalOperation;
}


bool CFilter::FilterExpression::operator!=(const FilterExpression& other) const
{
    return !(*this == other);
}


CFilter::SortField::SortField(const QByteArray& fieldId, SortingOrder sortingOrder)
    : fieldId(fieldId)
    , sortingOrder(sortingOrder)
{
}


bool CFilter::SortField::operator==(const SortField& other) const
{
    return fieldId == other.fieldId && sortingOrder == other.sortingOrder;
}


bool CFilter::SortField::operator!=(const SortField& other) const
{
    return !(*this == other);
}


QString CFilter::GetTextFilter() const
{
    return m_textFilter;
}


void CFilter::SetTextFilter(const QString& textFilter)
{
    m_textFilter = textFilter;
}


QByteArrayList CFilter::GetTextFieldIds() const
{
    return m_textFieldIds;
}


void CFilter::SetTextFieldIds(const QByteArrayList& fieldIds)
{
    m_textFieldIds = fieldIds;
}


const CFilter::FilterExpression& CFilter::GetFilterExpression() const
{
    return m_filterExpression;
}


void CFilter::SetFilterExpression(const FilterExpression& expression)
{
    m_filterExpression = expression;
}


void CFilter::AddFieldFilter(const FieldFilter& filter)
{
    m_filterExpression.fieldFilters << filter;
}


void CFilter::AddFilterExpression(const FilterExpression& expression)
{
    m_filterExpression.filterExpressions << expression;
}


QVector<CFilter::SortField> CFilter::GetSortFields() const
{
    return m_sortFields;
}


void CFilter::SetSortFields(const QVector<SortField>& sortFields)
{
    m_sortFields = sortFields;
}


void CFilter::AddSortField(const SortField& sortField)
{
    m_sortFields << sortField;
}


int CFilter::GetPage() const
{
    return m_page;
}


void CFilter::SetPage(int page)
{
    m_page = page > 0 ? page : -1;
}


int CFilter::GetPageSize() const
{
    return m_pageSize;
}


void CFilter::SetPageSize(int pageSize)
{
    m_pageSize = pageSize > 0 ? pageSize : -1;
}


int CFilter::GetOffset() const
{
    if (!HasPagination()){
        return -1;
    }
    return (m_page - 1) * m_pageSize;
}


int CFilter::GetLimit() const
{
    return HasPagination() ? m_pageSize : -1;
}


bool CFilter::HasPagination() const
{
    return m_page > 0 && m_pageSize > 0;
}


void CFilter::Clear()
{
    m_textFilter.clear();
    m_textFieldIds.clear();
    m_filterExpression = FilterExpression();
    m_sortFields.clear();
    m_page = -1;
    m_pageSize = -1;
}


} // namespace imtbase


