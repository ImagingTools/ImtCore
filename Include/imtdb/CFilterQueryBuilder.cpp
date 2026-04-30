// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtdb/CFilterQueryBuilder.h>


// Qt includes
#include <QtCore/QDebug>
#include <QtCore/QStringList>


namespace
{


const QString s_likeEscapeSql = QStringLiteral("ESCAPE '\\\\'");


QString NormalizePredicate(const QString& predicate)
{
    return predicate.trimmed().toLower();
}


} // namespace


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
    const QString searchClause = BuildSearchClause(filter.GetSearch(), result.bindValues);
    if (!searchClause.isEmpty()){
        whereParts << searchClause;
    }

    const QString rulesClause = BuildRulesClause(filter.GetRules(), result.bindValues);
    if (!rulesClause.isEmpty()){
        whereParts << rulesClause;
    }

    result.whereClause = whereParts.join(QStringLiteral(" AND "));
    result.orderByClause = BuildOrderClause(filter.GetOrders());
    result.limitClause = BuildWindowClause(filter.GetWindow());

    return result;
}


QString CFilterQueryBuilder::BuildSearchClause(
    const imtbase::CFilter::Search& search,
    QVariantList& bindValues) const
{
    if (!search.IsActive()){
        return QString();
    }

    QStringList parts;
    for (const QByteArray& scope : search.scopes){
        parts << MakeLikeCondition(MakeFieldAccess(scope), AddLikeBindValue(search.text, bindValues));
    }

    return parts.isEmpty() ? QString() : QStringLiteral("(%1)").arg(parts.join(QStringLiteral(" OR ")));
}


QString CFilterQueryBuilder::BuildRulesClause(
    const imtbase::CFilter::RuleSet& rules,
    QVariantList& bindValues) const
{
    QStringList parts;

    for (const imtbase::CFilter::Rule& rule : rules.rules){
        const QString ruleClause = BuildRuleClause(rule, bindValues);
        if (!ruleClause.isEmpty()){
            parts << ruleClause;
        }
    }

    for (const imtbase::CFilter::RuleSet& child : rules.children){
        const QString childClause = BuildRulesClause(child, bindValues);
        if (!childClause.isEmpty()){
            parts << childClause;
        }
    }

    if (parts.isEmpty()){
        return QString();
    }

    const QString join = rules.join == imtbase::CFilter::RuleSet::Any
        ? QStringLiteral(" OR ")
        : QStringLiteral(" AND ");
    return QStringLiteral("(%1)").arg(parts.join(join));
}


QString CFilterQueryBuilder::BuildOrderClause(const QVector<imtbase::CFilter::Order>& orders) const
{
    QStringList parts;
    for (const imtbase::CFilter::Order& order : orders){
        if (!order.IsValid()){
            continue;
        }
        parts << QStringLiteral("%1 %2").arg(
            MakeFieldAccess(order.path),
            order.descending ? QStringLiteral("DESC") : QStringLiteral("ASC"));
    }

    return parts.isEmpty() ? QString() : QStringLiteral("ORDER BY %1").arg(parts.join(QStringLiteral(", ")));
}


QString CFilterQueryBuilder::BuildWindowClause(const imtbase::CFilter::Window& window) const
{
    if (!window.IsActive()){
        return QString();
    }
    // CFilter::Window stores offset in first and limit in count; SQL requires LIMIT count OFFSET first ordering.
    return QStringLiteral("LIMIT %1 OFFSET %2").arg(window.count).arg(window.first);
}


void CFilterQueryBuilder::SetJsonColumnName(const QString& columnName)
{
    m_jsonColumnName = columnName;
}


void CFilterQueryBuilder::SetUseJsonExtract(bool useJsonExtract)
{
    m_useJsonExtract = useJsonExtract;
}


QString CFilterQueryBuilder::BuildRuleClause(const imtbase::CFilter::Rule& rule, QVariantList& bindValues) const
{
    if (!rule.IsValid()){
        return QString();
    }

    const QString fieldAccess = MakeFieldAccess(rule.path);
    const QString predicate = NormalizePredicate(rule.predicate);
    if (predicate == QLatin1String("is")){
        return QStringLiteral("%1 = %2").arg(fieldAccess, AddBindValue(rule.argument, bindValues));
    }
    if (predicate == QLatin1String("not")){
        return QStringLiteral("%1 != %2").arg(fieldAccess, AddBindValue(rule.argument, bindValues));
    }
    if (predicate == QLatin1String("before")){
        return QStringLiteral("%1 < %2").arg(fieldAccess, AddBindValue(rule.argument, bindValues));
    }
    if (predicate == QLatin1String("after")){
        return QStringLiteral("%1 > %2").arg(fieldAccess, AddBindValue(rule.argument, bindValues));
    }
    if (predicate == QLatin1String("from")){
        return QStringLiteral("%1 >= %2").arg(fieldAccess, AddBindValue(rule.argument, bindValues));
    }
    if (predicate == QLatin1String("until")){
        return QStringLiteral("%1 <= %2").arg(fieldAccess, AddBindValue(rule.argument, bindValues));
    }
    if (predicate == QLatin1String("like")){
        return MakeLikeCondition(fieldAccess, AddLikeBindValue(rule.argument.toString(), bindValues));
    }

    qWarning() << "Unexpected filter predicate" << rule.predicate << "for field" << QString::fromUtf8(rule.path);
    return QString();
}


QString CFilterQueryBuilder::MakeLikeCondition(const QString& expression, const QString& placeholder) const
{
    if (m_dialect == DD_SQLITE){
        return QStringLiteral("LOWER(%1) LIKE LOWER(%2) %3").arg(expression, placeholder, s_likeEscapeSql);
    }
    return QStringLiteral("%1 ILIKE %2 %3").arg(expression, placeholder, s_likeEscapeSql);
}


QString CFilterQueryBuilder::MakeJsonFieldAccess(const QString& path) const
{
    if (m_useJsonExtract){
        return QStringLiteral("json_extract(%1, '$.%2')").arg(QuoteIdentifier(m_jsonColumnName), EscapeJsonKey(path));
    }
    return QStringLiteral("%1->>'%2'").arg(QuoteIdentifier(m_jsonColumnName), EscapeJsonKey(path));
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


QString CFilterQueryBuilder::AddLikeBindValue(const QString& value, QVariantList& bindValues) const
{
    return AddBindValue(QStringLiteral("%") + EscapeLikePattern(value) + QStringLiteral("%"), bindValues);
}


QString CFilterQueryBuilder::MakeFieldAccess(const QByteArray& path) const
{
    const QString field = QString::fromUtf8(path);
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


