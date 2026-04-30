// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtbase/CFilterSerializer.h>


// Qt includes
#include <QtCore/QJsonArray>
#include <QtCore/QMap>
#include <QtCore/QStringList>
#include <QtCore/QUrl>
#include <QtCore/QUrlQuery>


namespace
{


const int s_invalidIndex = -1;
const QString s_rulePrefix = QStringLiteral("rule[");
const QString s_orderPrefix = QStringLiteral("order[");


QString NormalizeToken(const QString& value)
{
    return value.trimmed().toLower();
}


int ExtractIndexedKey(const QString& key, const QString& prefix, const QString& suffix)
{
    if (!key.startsWith(prefix)){
        return s_invalidIndex;
    }

    const int start = prefix.size();
    const int close = key.indexOf(QLatin1Char(']'), start);
    if (close <= start || key.mid(close + 1) != suffix){
        return s_invalidIndex;
    }

    bool ok = false;
    const int index = key.mid(start, close - start).toInt(&ok);
    return ok ? index : -1;
}


} // namespace


namespace imtbase
{


QString CFilterSerializer::ToQueryString(const CFilter& filter)
{
    QUrlQuery query;

    if (!filter.GetSearch().text.isEmpty()){
        query.addQueryItem(QStringLiteral("search"), filter.GetSearch().text);
    }

    if (!filter.GetSearch().scopes.isEmpty()){
        QStringList scopes;
        for (const QByteArray& scope : filter.GetSearch().scopes){
            scopes << QString::fromUtf8(scope);
        }
        query.addQueryItem(QStringLiteral("scope"), scopes.join(QLatin1Char(',')));
    }

    int ruleIndex = 0;
    for (const CFilter::Rule& rule : filter.GetRules().rules){
        if (!rule.IsValid()){
            continue;
        }
        const QString prefix = QStringLiteral("rule[%1].").arg(ruleIndex++);
        query.addQueryItem(prefix + QStringLiteral("path"), QString::fromUtf8(rule.path));
        query.addQueryItem(prefix + QStringLiteral("pred"), rule.predicate);
        query.addQueryItem(prefix + QStringLiteral("arg"), rule.argument.toString());
    }

    int orderIndex = 0;
    for (const CFilter::Order& order : filter.GetOrders()){
        if (!order.IsValid()){
            continue;
        }
        const QString prefix = QStringLiteral("order[%1].").arg(orderIndex++);
        query.addQueryItem(prefix + QStringLiteral("path"), QString::fromUtf8(order.path));
        query.addQueryItem(prefix + QStringLiteral("dir"), order.descending ? QStringLiteral("down") : QStringLiteral("up"));
    }

    if (filter.HasWindow()){
        query.addQueryItem(QStringLiteral("offset"), QString::number(filter.GetOffset()));
        query.addQueryItem(QStringLiteral("count"), QString::number(filter.GetLimit()));
    }

    return query.toString(QUrl::FullyEncoded);
}


bool CFilterSerializer::FromQueryString(const QString& queryString, CFilter& filter)
{
    QString normalized = queryString;
    if (normalized.startsWith(QLatin1Char('?'))){
        normalized = normalized.mid(1);
    }

    QUrlQuery query;
    query.setQuery(normalized);
    filter.Clear();

    QByteArrayList scopes;
    if (query.hasQueryItem(QStringLiteral("scope"))){
        const QString value = query.queryItemValue(QStringLiteral("scope"), QUrl::FullyDecoded);
        for (const QString& part : value.split(QLatin1Char(','), Qt::SkipEmptyParts)){
            scopes << part.trimmed().toUtf8();
        }
    }
    filter.SetSearch(query.queryItemValue(QStringLiteral("search"), QUrl::FullyDecoded), scopes);

    struct PendingRule
    {
        QByteArray path;
        QString predicate;
        QVariant argument;
    };

    struct PendingOrder
    {
        QByteArray path;
        bool descending = false;
    };

    QMap<int, PendingRule> rules;
    QMap<int, PendingOrder> orders;
    const QList<QPair<QString, QString>> items = query.queryItems(QUrl::FullyDecoded);
    for (const QPair<QString, QString>& item : items){
        const int pathRuleIndex = ExtractIndexedKey(item.first, s_rulePrefix, QStringLiteral(".path"));
        if (pathRuleIndex >= 0){
            rules[pathRuleIndex].path = item.second.toUtf8();
            continue;
        }

        const int predicateRuleIndex = ExtractIndexedKey(item.first, s_rulePrefix, QStringLiteral(".pred"));
        if (predicateRuleIndex >= 0){
            rules[predicateRuleIndex].predicate = item.second;
            continue;
        }

        const int argumentRuleIndex = ExtractIndexedKey(item.first, s_rulePrefix, QStringLiteral(".arg"));
        if (argumentRuleIndex >= 0){
            rules[argumentRuleIndex].argument = item.second;
            continue;
        }

        const int pathOrderIndex = ExtractIndexedKey(item.first, s_orderPrefix, QStringLiteral(".path"));
        if (pathOrderIndex >= 0){
            orders[pathOrderIndex].path = item.second.toUtf8();
            continue;
        }

        const int directionOrderIndex = ExtractIndexedKey(item.first, s_orderPrefix, QStringLiteral(".dir"));
        if (directionOrderIndex >= 0){
            orders[directionOrderIndex].descending = NormalizeToken(item.second) == QLatin1String("down");
            continue;
        }
    }

    CFilter::RuleSet ruleSet;
    for (auto it = rules.cbegin(); it != rules.cend(); ++it){
        const CFilter::Rule rule(it.value().path, it.value().predicate, it.value().argument);
        if (rule.IsValid()){
            ruleSet.rules << rule;
        }
    }
    filter.SetRules(ruleSet);

    QVector<CFilter::Order> orderList;
    for (auto it = orders.cbegin(); it != orders.cend(); ++it){
        const CFilter::Order order(it.value().path, it.value().descending);
        if (order.IsValid()){
            orderList << order;
        }
    }
    filter.SetOrders(orderList);

    if (query.hasQueryItem(QStringLiteral("offset")) || query.hasQueryItem(QStringLiteral("count"))){
        filter.SetWindow(
            query.queryItemValue(QStringLiteral("offset")).toInt(),
            query.queryItemValue(QStringLiteral("count")).toInt());
    }

    return true;
}


QJsonObject CFilterSerializer::ToJson(const CFilter& filter)
{
    QJsonObject json;

    if (filter.GetSearch().IsActive()){
        QJsonObject search;
        search[QStringLiteral("text")] = filter.GetSearch().text;
        QJsonArray scopes;
        for (const QByteArray& scope : filter.GetSearch().scopes){
            scopes << QString::fromUtf8(scope);
        }
        search[QStringLiteral("scope")] = scopes;
        json[QStringLiteral("search")] = search;
    }

    const QJsonObject rules = ToJson(filter.GetRules());
    if (!rules.isEmpty()){
        json[QStringLiteral("rules")] = rules;
    }

    QJsonArray orders;
    for (const CFilter::Order& order : filter.GetOrders()){
        if (!order.IsValid()){
            continue;
        }
        QJsonObject item;
        item[QStringLiteral("path")] = QString::fromUtf8(order.path);
        item[QStringLiteral("descending")] = order.descending;
        orders << item;
    }
    if (!orders.isEmpty()){
        json[QStringLiteral("orders")] = orders;
    }

    if (filter.HasWindow()){
        QJsonObject window;
        window[QStringLiteral("first")] = filter.GetOffset();
        window[QStringLiteral("count")] = filter.GetLimit();
        json[QStringLiteral("window")] = window;
    }

    return json;
}


bool CFilterSerializer::FromJson(const QJsonObject& json, CFilter& filter)
{
    filter.Clear();

    const QJsonObject search = json[QStringLiteral("search")].toObject();
    if (!search.isEmpty()){
        QByteArrayList scopes;
        for (const QJsonValue& value : search[QStringLiteral("scope")].toArray()){
            scopes << value.toString().toUtf8();
        }
        filter.SetSearch(search[QStringLiteral("text")].toString(), scopes);
    }

    if (json.contains(QStringLiteral("rules"))){
        CFilter::RuleSet rules;
        if (!FromJson(json[QStringLiteral("rules")].toObject(), rules)){
            return false;
        }
        filter.SetRules(rules);
    }

    QVector<CFilter::Order> orders;
    for (const QJsonValue& value : json[QStringLiteral("orders")].toArray()){
        const QJsonObject item = value.toObject();
        const CFilter::Order order(
            item[QStringLiteral("path")].toString().toUtf8(),
            item[QStringLiteral("descending")].toBool());
        if (order.IsValid()){
            orders << order;
        }
    }
    filter.SetOrders(orders);

    if (json.contains(QStringLiteral("window"))){
        const QJsonObject window = json[QStringLiteral("window")].toObject();
        filter.SetWindow(
            window[QStringLiteral("first")].toInt(-1),
            window[QStringLiteral("count")].toInt(-1));
    }

    return true;
}


QJsonObject CFilterSerializer::ToJson(const CFilter::RuleSet& rules)
{
    if (rules.IsEmpty()){
        return QJsonObject();
    }

    QJsonObject json;
    json[QStringLiteral("join")] = JoinToString(rules.join);

    QJsonArray ruleArray;
    for (const CFilter::Rule& rule : rules.rules){
        if (!rule.IsValid()){
            continue;
        }
        QJsonObject item;
        item[QStringLiteral("path")] = QString::fromUtf8(rule.path);
        item[QStringLiteral("pred")] = rule.predicate;
        item[QStringLiteral("arg")] = QJsonValue::fromVariant(rule.argument);
        ruleArray << item;
    }
    if (!ruleArray.isEmpty()){
        json[QStringLiteral("items")] = ruleArray;
    }

    QJsonArray childArray;
    for (const CFilter::RuleSet& child : rules.children){
        const QJsonObject childJson = ToJson(child);
        if (!childJson.isEmpty()){
            childArray << childJson;
        }
    }
    if (!childArray.isEmpty()){
        json[QStringLiteral("sets")] = childArray;
    }

    return json;
}


bool CFilterSerializer::FromJson(const QJsonObject& json, CFilter::RuleSet& rules)
{
    rules.join = StringToJoin(json[QStringLiteral("join")].toString());

    for (const QJsonValue& value : json[QStringLiteral("items")].toArray()){
        const QJsonObject item = value.toObject();
        const CFilter::Rule rule(
            item[QStringLiteral("path")].toString().toUtf8(),
            item[QStringLiteral("pred")].toString(),
            item[QStringLiteral("arg")].toVariant());
        if (rule.IsValid()){
            rules.rules << rule;
        }
    }

    for (const QJsonValue& value : json[QStringLiteral("sets")].toArray()){
        CFilter::RuleSet child;
        if (!FromJson(value.toObject(), child)){
            return false;
        }
        if (!child.IsEmpty()){
            rules.children << child;
        }
    }

    return true;
}


QString CFilterSerializer::JoinToString(CFilter::RuleSet::Join join)
{
    return join == CFilter::RuleSet::Any ? QStringLiteral("any") : QStringLiteral("all");
}


CFilter::RuleSet::Join CFilterSerializer::StringToJoin(const QString& value)
{
    return NormalizeToken(value) == QLatin1String("any")
        ? CFilter::RuleSet::Any
        : CFilter::RuleSet::All;
}


} // namespace imtbase


