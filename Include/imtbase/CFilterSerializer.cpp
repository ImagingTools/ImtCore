// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtbase/CFilterSerializer.h>


// Qt includes
#include <QtCore/QDebug>
#include <QtCore/QJsonArray>
#include <QtCore/QJsonDocument>
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
    return ok ? index : s_invalidIndex;
}


bool TryNormalizeOperator(const QString& predicate, QString& op)
{
    const QString normalized = NormalizeToken(predicate);
    if (normalized == QLatin1String("is") || normalized == QLatin1String("eq") || normalized == QLatin1String("=")){
        op = QStringLiteral("=");
        return true;
    }
    if (normalized == QLatin1String("like") || normalized == QLatin1String("contains")){
        op = QStringLiteral("contains");
        return true;
    }
    if (normalized == QLatin1String("after") || normalized == QLatin1String("gt") || normalized == QLatin1String(">")){
        op = QStringLiteral(">");
        return true;
    }
    if (normalized == QLatin1String("before") || normalized == QLatin1String("lt") || normalized == QLatin1String("<")){
        op = QStringLiteral("<");
        return true;
    }
    if (normalized == QLatin1String("nin") || normalized == QLatin1String("not_in")){
        op = QStringLiteral("not_in");
        return true;
    }
    if (normalized == QLatin1String("in") ||
        normalized == QLatin1String("between") ||
        normalized == QLatin1String("not_between")){
        op = normalized;
        return true;
    }
    return false;
}


QString JoinToCombinator(imtbase::CFilter::RuleSet::Join join)
{
    return join == imtbase::CFilter::RuleSet::Any ? QStringLiteral("or") : QStringLiteral("and");
}


bool ContainsOnlyAllowedKeys(const QJsonObject& json, const QStringList& keys)
{
    for (auto it = json.constBegin(); it != json.constEnd(); ++it){
        if (!keys.contains(it.key())){
            return false;
        }
    }
    return true;
}


QJsonValue NormalizeValue(const QString& op, const QJsonValue& value)
{
    if ((op == QLatin1String("in") || op == QLatin1String("not_in")) && !value.isArray()){
        QJsonArray array;
        array << value;
        return array;
    }
    return value;
}


bool ValidateValueByOperator(const QString& op, const QJsonValue& value)
{
    if (op == QLatin1String("in") || op == QLatin1String("not_in")){
        return value.isArray() && !value.toArray().isEmpty();
    }
    if (op == QLatin1String("between") || op == QLatin1String("not_between")){
        return value.isArray() && value.toArray().size() == 2;
    }
    return !value.isArray();
}


imtbase::CFilter::RuleSet::Join CombinatorToJoin(const QString& combinator)
{
    return NormalizeToken(combinator) == QLatin1String("or")
        ? imtbase::CFilter::RuleSet::Any
        : imtbase::CFilter::RuleSet::All;
}


bool IsValidCombinator(const QString& combinator)
{
    const QString normalized = NormalizeToken(combinator);
    return normalized == QLatin1String("and") || normalized == QLatin1String("or");
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
        QString op;
        if (!TryNormalizeOperator(rule.predicate, op)){
            qWarning() << "Skipping filter rule with unsupported operator" << rule.predicate
                << "for field" << QString::fromUtf8(rule.path);
            continue;
        }
        QJsonObject item;
        item[QStringLiteral("field")] = QString::fromUtf8(rule.path);
        item[QStringLiteral("operator")] = op;
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


bool FromQmlQuery(const QJsonObject& json, imtbase::CFilter::RuleSet& rules)
{
    if (!json.contains(QStringLiteral("combinator")) ||
        !json.contains(QStringLiteral("rules")) ||
        !ContainsOnlyAllowedKeys(json, { QStringLiteral("combinator"), QStringLiteral("rules") })){
        return false;
    }

    const QString combinator = json[QStringLiteral("combinator")].toString();
    if (!IsValidCombinator(combinator)){
        return false;
    }
    rules.join = CombinatorToJoin(combinator);

    for (const QJsonValue& value : json[QStringLiteral("rules")].toArray()){
        const QJsonObject item = value.toObject();
        if (item.contains(QStringLiteral("rules")) || item.contains(QStringLiteral("combinator"))){
            if (!item.contains(QStringLiteral("rules")) ||
                !item.contains(QStringLiteral("combinator")) ||
                !ContainsOnlyAllowedKeys(item, { QStringLiteral("combinator"), QStringLiteral("rules") })){
                return false;
            }
            imtbase::CFilter::RuleSet child;
            if (!FromQmlQuery(item, child)){
                return false;
            }
            if (!child.IsEmpty()){
                rules.children << child;
            }
            continue;
        }

        if (!item.contains(QStringLiteral("field")) ||
            !item.contains(QStringLiteral("operator")) ||
            !item.contains(QStringLiteral("value")) ||
            !ContainsOnlyAllowedKeys(item, { QStringLiteral("field"), QStringLiteral("operator"), QStringLiteral("value") })){
            return false;
        }

        QString predicate;
        if (!TryNormalizeOperator(item[QStringLiteral("operator")].toString(), predicate)){
            return false;
        }
        const QByteArray field = item[QStringLiteral("field")].toString().toUtf8();
        if (field.isEmpty()){
            return false;
        }
        const QJsonValue argument = NormalizeValue(predicate, item[QStringLiteral("value")]);
        if (!ValidateValueByOperator(predicate, argument)){
            return false;
        }

        const imtbase::CFilter::Rule rule(
            field,
            predicate,
            argument.toVariant());
        if (rule.IsValid()){
            rules.rules << rule;
        }
    }

    return true;
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

    if (!filter.GetRules().IsEmpty()){
        query.addQueryItem(
            QStringLiteral("query"),
            QString::fromUtf8(QJsonDocument(ToQmlQuery(filter.GetRules())).toJson(QJsonDocument::Compact)));
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
        query.addQueryItem(
            QStringLiteral("sort"),
            QString::fromUtf8(QJsonDocument(sort).toJson(QJsonDocument::Compact)));
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
            QString predicate;
            if (!TryNormalizeOperator(item.second, predicate)){
                return false;
            }
            rules[predicateRuleIndex].predicate = predicate;
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

    if (query.hasQueryItem(QStringLiteral("query"))){
        const QByteArray rulesJson = query.queryItemValue(QStringLiteral("query"), QUrl::FullyDecoded).toUtf8();
        const QJsonDocument rulesDocument = QJsonDocument::fromJson(rulesJson);
        if (!rulesDocument.isObject()){
            return false;
        }

        CFilter::RuleSet ruleSet;
        if (!FromQmlQuery(rulesDocument.object(), ruleSet)){
            return false;
        }
        filter.SetRules(ruleSet);
    } else if (query.hasQueryItem(QStringLiteral("rules"))){
        const QByteArray rulesJson = query.queryItemValue(QStringLiteral("rules"), QUrl::FullyDecoded).toUtf8();
        const QJsonDocument rulesDocument = QJsonDocument::fromJson(rulesJson);
        if (!rulesDocument.isObject()){
            return false;
        }

        CFilter::RuleSet ruleSet;
        if (!FromJson(rulesDocument.object(), ruleSet)){
            return false;
        }
        filter.SetRules(ruleSet);
    } else {
        CFilter::RuleSet ruleSet;
        for (auto it = rules.cbegin(); it != rules.cend(); ++it){
            const CFilter::Rule rule(it.value().path, it.value().predicate, it.value().argument);
            if (rule.IsValid()){
                ruleSet.rules << rule;
            }
        }
        filter.SetRules(ruleSet);
    }

    QVector<CFilter::Order> orderList;
    if (query.hasQueryItem(QStringLiteral("sort"))){
        const QByteArray sortJson = query.queryItemValue(QStringLiteral("sort"), QUrl::FullyDecoded).toUtf8();
        const QJsonDocument sortDocument = QJsonDocument::fromJson(sortJson);
        if (!sortDocument.isArray()){
            return false;
        }

        for (const QJsonValue& value : sortDocument.array()){
            const QJsonObject item = value.toObject();
            const CFilter::Order order(
                item[QStringLiteral("field")].toString().toUtf8(),
                NormalizeToken(item[QStringLiteral("direction")].toString()) == QLatin1String("desc"));
            if (order.IsValid()){
                orderList << order;
            }
        }
        filter.SetOrders(orderList);
    } else {
        for (auto it = orders.cbegin(); it != orders.cend(); ++it){
            const CFilter::Order order(it.value().path, it.value().descending);
            if (order.IsValid()){
                orderList << order;
            }
        }
        filter.SetOrders(orderList);
    }

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
        search[QStringLiteral("scopes")] = scopes;
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


QJsonObject CFilterSerializer::ToQmlJson(const CFilter& filter)
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


bool CFilterSerializer::FromJson(const QJsonObject& json, CFilter& filter)
{
    filter.Clear();

    const QJsonObject search = json[QStringLiteral("search")].toObject();
    if (!search.isEmpty()){
        QByteArrayList scopes;
        const QJsonArray scopeArray = search.contains(QStringLiteral("scopes"))
            ? search[QStringLiteral("scopes")].toArray()
            : search[QStringLiteral("scope")].toArray();
        for (const QJsonValue& value : scopeArray){
            scopes << value.toString().toUtf8();
        }
        filter.SetSearch(search[QStringLiteral("text")].toString(), scopes);
    }

    if (json.contains(QStringLiteral("query"))){
        CFilter::RuleSet rules;
        if (!FromQmlQuery(json[QStringLiteral("query")].toObject(), rules)){
            return false;
        }
        filter.SetRules(rules);
    } else if (json.contains(QStringLiteral("rules"))){
        CFilter::RuleSet rules;
        if (!FromJson(json[QStringLiteral("rules")].toObject(), rules)){
            return false;
        }
        filter.SetRules(rules);
    }

    QVector<CFilter::Order> orders;
    const QJsonArray orderArray = json.contains(QStringLiteral("sort"))
        ? json[QStringLiteral("sort")].toArray()
        : json[QStringLiteral("orders")].toArray();
    for (const QJsonValue& value : orderArray){
        const QJsonObject item = value.toObject();
        const CFilter::Order order(
            item.contains(QStringLiteral("field"))
                ? item[QStringLiteral("field")].toString().toUtf8()
                : item[QStringLiteral("path")].toString().toUtf8(),
            item.contains(QStringLiteral("direction"))
                ? NormalizeToken(item[QStringLiteral("direction")].toString()) == QLatin1String("desc")
                : item[QStringLiteral("descending")].toBool());
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
        QString predicate;
        if (!TryNormalizeOperator(item[QStringLiteral("pred")].toString(), predicate)){
            return false;
        }

        const CFilter::Rule rule(
            item[QStringLiteral("path")].toString().toUtf8(),
            predicate,
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
