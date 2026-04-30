// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtdb/CFilterQueryBuilder.h>


// Qt includes
#include <QtCore/QStringList>


namespace imtdb
{


CFilterQueryBuilder::CFilterQueryBuilder(DatabaseDialect dialect)
    : m_dialect(dialect)
{
}


CFilterQueryBuilder::QueryResult CFilterQueryBuilder::Build(const imtbase::CFilter& filter) const
{
    QueryResult result;

    QStringList whereParts;
    const QString textClause = BuildTextFilter(filter.GetTextFilter(), filter.GetTextFieldIds(), result.bindValues);
    if (!textClause.isEmpty()){
        whereParts << textClause;
    }

    const QString expressionClause = BuildFilterExpression(filter.GetFilterExpression(), result.bindValues);
    if (!expressionClause.isEmpty()){
        whereParts << expressionClause;
    }

    result.whereClause = whereParts.join(QStringLiteral(" AND "));
    result.orderByClause = BuildSortClause(filter.GetSortFields());
    result.limitClause = BuildLimitClause(filter);

    return result;
}


QString CFilterQueryBuilder::BuildTextFilter(
    const QString& text,
    const QByteArrayList& columnIds,
    QVariantList& bindValues) const
{
    if (text.isEmpty() || columnIds.isEmpty()){
        return QString();
    }

    QStringList parts;
    for (const QByteArray& columnId : columnIds){
        const QString placeholder = AddBindValue(
            QStringLiteral("%") + EscapeLikePattern(text) + QStringLiteral("%"),
            bindValues);
        parts << MakeLikeCondition(MakeFieldAccess(columnId), placeholder);
    }

    return QStringLiteral("(%1)").arg(parts.join(QStringLiteral(" OR ")));
}


QString CFilterQueryBuilder::BuildFilterExpression(
    const imtbase::CFilter::FilterExpression& expression,
    QVariantList& bindValues) const
{
    QStringList parts;

    for (const imtbase::CFilter::FieldFilter& fieldFilter : expression.fieldFilters){
        if (fieldFilter.fieldId.isEmpty()){
            continue;
        }

        const QString fieldAccess = MakeFieldAccess(fieldFilter.fieldId);
        QString part;
        switch (fieldFilter.operation){
        case imtbase::CFilter::FO_EQUAL:
            part = QStringLiteral("%1 = %2").arg(fieldAccess, AddBindValue(fieldFilter.value, bindValues));
            break;
        case imtbase::CFilter::FO_NOT_EQUAL:
            part = QStringLiteral("%1 != %2").arg(fieldAccess, AddBindValue(fieldFilter.value, bindValues));
            break;
        case imtbase::CFilter::FO_LESS:
            part = QStringLiteral("%1 < %2").arg(fieldAccess, AddBindValue(fieldFilter.value, bindValues));
            break;
        case imtbase::CFilter::FO_GREATER:
            part = QStringLiteral("%1 > %2").arg(fieldAccess, AddBindValue(fieldFilter.value, bindValues));
            break;
        case imtbase::CFilter::FO_NOT_LESS:
            part = QStringLiteral("%1 >= %2").arg(fieldAccess, AddBindValue(fieldFilter.value, bindValues));
            break;
        case imtbase::CFilter::FO_NOT_GREATER:
            part = QStringLiteral("%1 <= %2").arg(fieldAccess, AddBindValue(fieldFilter.value, bindValues));
            break;
        case imtbase::CFilter::FO_CONTAINS:
            part = MakeLikeCondition(
                fieldAccess,
                AddBindValue(
                    QStringLiteral("%") + EscapeLikePattern(fieldFilter.value.toString()) + QStringLiteral("%"),
                    bindValues));
            break;
        }

        if (!part.isEmpty()){
            parts << part;
        }
    }

    for (const imtbase::CFilter::FilterExpression& childExpression : expression.filterExpressions){
        const QString childPart = BuildFilterExpression(childExpression, bindValues);
        if (!childPart.isEmpty()){
            parts << childPart;
        }
    }

    if (parts.isEmpty()){
        return QString();
    }

    const QString op = expression.logicalOperation == imtbase::CFilter::LO_OR
        ? QStringLiteral(" OR ")
        : QStringLiteral(" AND ");
    return QStringLiteral("(%1)").arg(parts.join(op));
}


QString CFilterQueryBuilder::BuildSortClause(const QVector<imtbase::CFilter::SortField>& sortFields) const
{
    QStringList parts;
    for (const imtbase::CFilter::SortField& sortField : sortFields){
        if (sortField.fieldId.isEmpty() || sortField.sortingOrder == imtbase::CFilter::SO_NO_ORDER){
            continue;
        }
        const QString order = sortField.sortingOrder == imtbase::CFilter::SO_DESC
            ? QStringLiteral("DESC")
            : QStringLiteral("ASC");
        parts << QStringLiteral("%1 %2").arg(MakeFieldAccess(sortField.fieldId), order);
    }

    if (parts.isEmpty()){
        return QString();
    }
    return QStringLiteral("ORDER BY %1").arg(parts.join(QStringLiteral(", ")));
}


QString CFilterQueryBuilder::BuildLimitClause(const imtbase::CFilter& filter) const
{
    if (!filter.HasPagination()){
        return QString();
    }
    return QStringLiteral("LIMIT %1 OFFSET %2").arg(filter.GetLimit()).arg(filter.GetOffset());
}


void CFilterQueryBuilder::SetJsonColumnName(const QString& columnName)
{
    m_jsonColumnName = columnName;
}


void CFilterQueryBuilder::SetUseJsonExtract(bool useJsonExtract)
{
    m_useJsonExtract = useJsonExtract;
}


QString CFilterQueryBuilder::MakeLikeCondition(const QString& expression, const QString& placeholder) const
{
    if (m_dialect == DD_SQLITE){
        return QStringLiteral("LOWER(%1) LIKE LOWER(%2) ESCAPE '\\\\'").arg(expression, placeholder);
    }
    return QStringLiteral("%1 ILIKE %2 ESCAPE '\\\\'").arg(expression, placeholder);
}


QString CFilterQueryBuilder::MakeJsonFieldAccess(const QString& fieldId) const
{
    if (m_useJsonExtract){
        return QStringLiteral("json_extract(%1, '$.%2')").arg(QuoteIdentifier(m_jsonColumnName), EscapeJsonKey(fieldId));
    }
    return QStringLiteral("%1->>'%2'").arg(QuoteIdentifier(m_jsonColumnName), EscapeJsonKey(fieldId));
}


QString CFilterQueryBuilder::MakePlaceholder(int bindIndex) const
{
    if (m_dialect == DD_POSTGRESQL){
        return QStringLiteral("$%1").arg(bindIndex);
    }
    return QStringLiteral("?");
}


QString CFilterQueryBuilder::AddBindValue(const QVariant& value, QVariantList& bindValues) const
{
    bindValues << value;
    return MakePlaceholder(bindValues.size());
}


QString CFilterQueryBuilder::MakeFieldAccess(const QByteArray& fieldId) const
{
    const QString field = QString::fromUtf8(fieldId);
    if (!m_jsonColumnName.isEmpty()){
        return MakeJsonFieldAccess(field);
    }
    return QuoteIdentifier(field);
}


QString CFilterQueryBuilder::EscapeLikePattern(const QString& text) const
{
    QString escaped = text;
    escaped.replace(QLatin1Char('\\'), QStringLiteral("\\\\"));
    escaped.replace(QLatin1Char('%'), QStringLiteral("\\%"));
    escaped.replace(QLatin1Char('_'), QStringLiteral("\\_"));
    return escaped;
}


QString CFilterQueryBuilder::QuoteIdentifier(const QString& identifier)
{
    QString quoted = identifier;
    quoted.replace(QLatin1Char('"'), QStringLiteral("\"\""));
    return QStringLiteral("\"%1\"").arg(quoted);
}


QString CFilterQueryBuilder::EscapeJsonKey(const QString& key)
{
    QString escaped = key;
    escaped.replace(QLatin1Char('\''), QStringLiteral("''"));
    return escaped;
}


} // namespace imtdb


