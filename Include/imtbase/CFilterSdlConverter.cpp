// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtbase/CFilterSdlConverter.h>


// Qt includes
#include <QtCore/QJsonArray>

// ImtCore includes
#include <imtbase/CFilterSerializer.h>


namespace
{


QString NormalizePredicate(const QString& predicate)
{
    return predicate.trimmed().toLower();
}


QString PredicateToOperator(const QString& predicate)
{
    const QString normalized = NormalizePredicate(predicate);
    if (normalized == QLatin1String("is")){
        return QStringLiteral("=");
    }
    if (normalized == QLatin1String("like")){
        return QStringLiteral("contains");
    }
    if (normalized == QLatin1String("after")){
        return QStringLiteral(">");
    }
    if (normalized == QLatin1String("before")){
        return QStringLiteral("<");
    }
    return predicate;
}


QString JoinToCombinator(imtbase::CFilter::RuleSet::Join join)
{
    return join == imtbase::CFilter::RuleSet::Any ? QStringLiteral("or") : QStringLiteral("and");
}


QJsonObject ToQmlQuery(const imtbase::CFilter::RuleSet& rules)
{
    QJsonObject json;
    json[QStringLiteral("combinator")] = JoinToCombinator(rules.join);

    QJsonArray items;
    for (const imtbase::CFilter::Rule& rule : rules.rules){
        if (!rule.IsValid()){
            continue;
        }
        QJsonObject item;
        item[QStringLiteral("field")] = QString::fromUtf8(rule.path);
        item[QStringLiteral("operator")] = PredicateToOperator(rule.predicate);
        item[QStringLiteral("value")] = QJsonValue::fromVariant(rule.argument);
        items << item;
    }
    for (const imtbase::CFilter::RuleSet& child : rules.children){
        if (!child.IsEmpty()){
            items << ToQmlQuery(child);
        }
    }

    json[QStringLiteral("rules")] = items;
    return json;
}


} // namespace


namespace imtbase
{


QJsonObject CFilterSdlConverter::ToSdlJson(const CFilter& filter)
{
    QJsonObject json;

    if (filter.GetSearch().IsActive()){
        QJsonObject search;
        search[QStringLiteral("text")] = filter.GetSearch().text;
        QJsonArray scopes;
        for (const QByteArray& scope : filter.GetSearch().scopes){
            scopes << QString::fromUtf8(scope);
        }
        search[QStringLiteral("scopes")] = scopes;
        json[QStringLiteral("search")] = search;
    }

    if (!filter.GetRules().IsEmpty()){
        json[QStringLiteral("query")] = ToQmlQuery(filter.GetRules());
    }

    QJsonArray sort;
    for (const CFilter::Order& order : filter.GetOrders()){
        if (!order.IsValid()){
            continue;
        }
        QJsonObject item;
        item[QStringLiteral("field")] = QString::fromUtf8(order.path);
        item[QStringLiteral("direction")] = order.descending ? QStringLiteral("desc") : QStringLiteral("asc");
        sort << item;
    }
    if (!sort.isEmpty()){
        json[QStringLiteral("sort")] = sort;
    }

    if (filter.HasWindow()){
        QJsonObject window;
        window[QStringLiteral("first")] = filter.GetOffset();
        window[QStringLiteral("count")] = filter.GetLimit();
        json[QStringLiteral("window")] = window;
    }

    return json;
}


bool CFilterSdlConverter::FromSdlJson(const QJsonObject& filterModel, CFilter& filter)
{
    return CFilterSerializer::FromJson(filterModel, filter);
}


bool CFilterSdlConverter::FromQmlModel(const QVariant& filterModel, CFilter& filter)
{
    if (!filterModel.canConvert<QVariantMap>()){
        return false;
    }

    return FromSdlJson(QJsonObject::fromVariantMap(filterModel.toMap()), filter);
}


} // namespace imtbase

