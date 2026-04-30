// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtdb/CFilterQueryBuilder.h>

// ImtCore includes
#include <imtdb/imtdb.h>


namespace imtdb
{


// public methods

CFilterQueryBuilder::CFilterQueryBuilder(DatabaseDialect dialect)
    : m_dialect(dialect)
{
}


CFilterQueryBuilder::QueryResult CFilterQueryBuilder::Build(const imtbase::IComplexCollectionFilter& filter) const
{
    QueryResult result;

    // Text filter
    const QString textFilter = filter.GetTextFilter();
    if (!textFilter.isEmpty()){
        QByteArrayList columnIds;
        for (const imtbase::IComplexCollectionFilter::FieldInfo& info : filter.GetFields()){
            if (info.metaInfo.flags & imtbase::IComplexCollectionFilter::SO_TEXT_FILTER){
                columnIds << info.id;
            }
        }
        result.whereClause = BuildTextFilter(textFilter, columnIds);
    }

    // Field filter expression
    QVariantList bindValues;
    const QString exprClause = BuildFilterExpression(filter.GetFilterExpression(), bindValues);
    if (!exprClause.isEmpty()){
        if (!result.whereClause.isEmpty()){
            result.whereClause = QStringLiteral("(%1) AND (%2)").arg(result.whereClause, exprClause);
        }
        else{
            result.whereClause = exprClause;
        }
    }
    result.bindValues = bindValues;

    // ORDER BY
    result.orderByClause = BuildSortClause(filter.GetFields());

    return result;
}


CFilterQueryBuilder::QueryResult CFilterQueryBuilder::Build(const imtbase::ICollectionFilter& filter) const
{
    QueryResult result;

    // Text filter
    result.whereClause = BuildTextFilter(filter.GetTextFilter(), filter.GetFilteringInfoIds());

    // ORDER BY
    result.orderByClause = BuildSortClause(filter);

    return result;
}


QString CFilterQueryBuilder::BuildTextFilter(const QString& text, const QByteArrayList& columnIds) const
{
    if (text.isEmpty() || columnIds.isEmpty()){
        return QString();
    }

    const QString pattern = EscapeLikePattern(text);
    QStringList parts;

    for (const QByteArray& colId : columnIds){
        QString expression;
        if (!m_jsonColumnName.isEmpty()){
            expression = MakeJsonFieldAccess(QString::fromUtf8(colId));
        }
        else{
            expression = QStringLiteral(R"("%1")").arg(QString::fromUtf8(colId));
        }
        parts << MakeLikeCondition(expression, pattern);
    }

    return parts.join(QStringLiteral(" OR "));
}


QString CFilterQueryBuilder::BuildFilterExpression(
    const imtbase::IComplexCollectionFilter::FilterExpression& expr,
    QVariantList& bindValues) const
{
    Q_UNUSED(bindValues)

    QString logicStr;
    switch (expr.logicalOperation){
    case imtbase::IComplexCollectionFilter::LO_AND:
        logicStr = QStringLiteral("AND");
        break;
    case imtbase::IComplexCollectionFilter::LO_OR:
        logicStr = QStringLiteral("OR");
        break;
    default:
        return QString();
    }

    QStringList parts;

    for (const imtbase::IComplexCollectionFilter::FieldFilter& fieldFilter : expr.fieldFilters){
        const QString colExpr = QStringLiteral(R"("%1")").arg(QString::fromUtf8(fieldFilter.fieldId));

        switch (fieldFilter.filterOperation){
        case imtbase::IComplexCollectionFilter::FO_EQUAL:
            if (fieldFilter.filterValue.typeId() == QMetaType::Bool){
                parts << QStringLiteral("%1 = %2").arg(colExpr, fieldFilter.filterValue.toBool() ? QStringLiteral("true") : QStringLiteral("false"));
            }
            else{
                parts << QStringLiteral("%1 = '%2'").arg(colExpr, SqlEncode(fieldFilter.filterValue.toString()));
            }
            break;
        case imtbase::IComplexCollectionFilter::FO_NOT_EQUAL:
            parts << QStringLiteral("%1 != '%2'").arg(colExpr, SqlEncode(fieldFilter.filterValue.toString()));
            break;
        case imtbase::IComplexCollectionFilter::FO_LESS:
            parts << QStringLiteral("%1 < '%2'").arg(colExpr, SqlEncode(fieldFilter.filterValue.toString()));
            break;
        case imtbase::IComplexCollectionFilter::FO_GREATER:
            parts << QStringLiteral("%1 > '%2'").arg(colExpr, SqlEncode(fieldFilter.filterValue.toString()));
            break;
        case imtbase::IComplexCollectionFilter::FO_NOT_LESS:
            parts << QStringLiteral("%1 >= '%2'").arg(colExpr, SqlEncode(fieldFilter.filterValue.toString()));
            break;
        case imtbase::IComplexCollectionFilter::FO_NOT_GREATER:
            parts << QStringLiteral("%1 <= '%2'").arg(colExpr, SqlEncode(fieldFilter.filterValue.toString()));
            break;
        case imtbase::IComplexCollectionFilter::FO_CONTAINS:
            parts << MakeLikeCondition(colExpr, EscapeLikePattern(fieldFilter.filterValue.toString()));
            break;
        default:
            break;
        }
    }

    for (const imtbase::IComplexCollectionFilter::FilterExpression& subExpr : expr.filterExpressions){
        QVariantList subBindValues;
        const QString subPart = BuildFilterExpression(subExpr, subBindValues);
        if (!subPart.isEmpty()){
            parts << subPart;
        }
    }

    if (parts.isEmpty()){
        return QString();
    }

    return QStringLiteral("(%1)").arg(parts.join(QStringLiteral(" %1 ").arg(logicStr)));
}


QString CFilterQueryBuilder::BuildSortClause(const imtbase::IComplexCollectionFilter::Fields& fields) const
{
    QStringList orderParts;

    for (const imtbase::IComplexCollectionFilter::FieldInfo& info : fields){
        if (!(info.metaInfo.flags & imtbase::IComplexCollectionFilter::SO_SORT)){
            continue;
        }
        if (info.metaInfo.sortingOrder == imtbase::IComplexCollectionFilter::SO_NO_ORDER){
            continue;
        }
        if (info.id.isEmpty()){
            continue;
        }

        const QString order = (info.metaInfo.sortingOrder == imtbase::IComplexCollectionFilter::SO_ASC)
            ? QStringLiteral("ASC")
            : QStringLiteral("DESC");

        orderParts << QStringLiteral(R"("%1" %2)").arg(QString::fromUtf8(info.id), order);
    }

    if (orderParts.isEmpty()){
        return QString();
    }

    return QStringLiteral("ORDER BY %1").arg(orderParts.join(QStringLiteral(", ")));
}


QString CFilterQueryBuilder::BuildSortClause(const imtbase::ICollectionFilter& filter) const
{
    const QByteArrayList sortingInfoIds = filter.GetSortingInfoIds();
    if (sortingInfoIds.isEmpty()){
        return QString();
    }

    const QByteArray columnId = sortingInfoIds.first();
    if (columnId.isEmpty()){
        return QString();
    }

    QString sortOrder;
    switch (filter.GetSortingOrder()){
    case imtbase::ICollectionFilter::SO_ASC:
        sortOrder = QStringLiteral("ASC");
        break;
    case imtbase::ICollectionFilter::SO_DESC:
        sortOrder = QStringLiteral("DESC");
        break;
    default:
        break;
    }

    if (sortOrder.isEmpty()){
        return QString();
    }

    return QStringLiteral(R"(ORDER BY "%1" %2)").arg(QString::fromUtf8(columnId), sortOrder);
}


void CFilterQueryBuilder::SetJsonColumnName(const QString& columnName)
{
    m_jsonColumnName = columnName;
}


void CFilterQueryBuilder::SetUseJsonExtract(bool useJsonExtract)
{
    m_useJsonExtract = useJsonExtract;
}


// private methods

QString CFilterQueryBuilder::MakeLikeCondition(const QString& expression, const QString& pattern) const
{
    if (m_dialect == DD_SQLITE){
        return QStringLiteral("LOWER(%1) LIKE LOWER('%%%2%%')").arg(expression, pattern);
    }
    return QStringLiteral("%1 ILIKE '%%%2%%'").arg(expression, pattern);
}


QString CFilterQueryBuilder::MakeJsonFieldAccess(const QString& fieldId) const
{
    if (m_useJsonExtract){
        return QStringLiteral("json_extract(\"%1\", '$.%2')").arg(m_jsonColumnName, fieldId);
    }
    return QStringLiteral("\"%1\"->>'%2'").arg(m_jsonColumnName, fieldId);
}


QString CFilterQueryBuilder::EscapeLikePattern(const QString& text) const
{
    return SqlEncode(text);
}


} // namespace imtdb


