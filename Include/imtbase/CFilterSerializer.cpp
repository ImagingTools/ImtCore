// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtbase/CFilterSerializer.h>


// Qt includes
#include <QtCore/QStringList>
#include <QtCore/QJsonArray>
#include <QtCore/QUrl>
#include <QtCore/QUrlQuery>


namespace
{


// Normalizes operation and sorting-order tokens for case-insensitive deserialization.
QString NormalizeToken(const QString& value)
{
    return value.trimmed().toLower();
}


} // namespace


namespace imtbase
{


QString CFilterSerializer::ToQueryString(const CFilter& filter)
{
    QUrlQuery query;

    if (!filter.GetTextFilter().isEmpty()){
        query.addQueryItem(QStringLiteral("text"), filter.GetTextFilter());
    }

    if (!filter.GetTextFieldIds().isEmpty()){
        QStringList fieldIds;
        for (const QByteArray& fieldId : filter.GetTextFieldIds()){
            fieldIds << QString::fromUtf8(fieldId);
        }
        query.addQueryItem(QStringLiteral("textFields"), fieldIds.join(QLatin1Char(',')));
    }

    QStringList sortParts;
    for (const CFilter::SortField& sortField : filter.GetSortFields()){
        if (sortField.fieldId.isEmpty() || sortField.sortingOrder == CFilter::SO_NO_ORDER){
            continue;
        }
        sortParts << QStringLiteral("%1:%2")
            .arg(QString::fromUtf8(sortField.fieldId), SortingOrderToString(sortField.sortingOrder));
    }
    if (!sortParts.isEmpty()){
        query.addQueryItem(QStringLiteral("sort"), sortParts.join(QLatin1Char(',')));
    }

    for (const CFilter::FieldFilter& fieldFilter : filter.GetFilterExpression().fieldFilters){
        if (fieldFilter.fieldId.isEmpty()){
            continue;
        }
        query.addQueryItem(
            QStringLiteral("filter[%1][%2]").arg(
                QString::fromUtf8(fieldFilter.fieldId),
                FieldOperationToString(fieldFilter.operation)),
            fieldFilter.value.toString());
    }

    if (filter.HasPagination()){
        query.addQueryItem(QStringLiteral("page"), QString::number(filter.GetPage()));
        query.addQueryItem(QStringLiteral("limit"), QString::number(filter.GetPageSize()));
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

    if (query.hasQueryItem(QStringLiteral("text"))){
        filter.SetTextFilter(query.queryItemValue(QStringLiteral("text"), QUrl::FullyDecoded));
    }

    if (query.hasQueryItem(QStringLiteral("textFields"))){
        QByteArrayList fieldIds;
        const QString value = query.queryItemValue(QStringLiteral("textFields"), QUrl::FullyDecoded);
        for (const QString& part : value.split(QLatin1Char(','), Qt::SkipEmptyParts)){
            fieldIds << part.trimmed().toUtf8();
        }
        filter.SetTextFieldIds(fieldIds);
    }

    if (query.hasQueryItem(QStringLiteral("sort"))){
        QVector<CFilter::SortField> sortFields;
        const QString value = query.queryItemValue(QStringLiteral("sort"), QUrl::FullyDecoded);
        for (const QString& part : value.split(QLatin1Char(','), Qt::SkipEmptyParts)){
            const QStringList pair = part.split(QLatin1Char(':'));
            if (pair.size() != 2){
                continue;
            }
            sortFields << CFilter::SortField(
                pair.at(0).trimmed().toUtf8(),
                StringToSortingOrder(pair.at(1).trimmed()));
        }
        filter.SetSortFields(sortFields);
    }

    CFilter::FilterExpression expression;
    const QList<QPair<QString, QString>> items = query.queryItems(QUrl::FullyDecoded);
    for (const QPair<QString, QString>& item : items){
        const QString& key = item.first;
        if (!key.startsWith(QStringLiteral("filter["))){
            continue;
        }

        const int firstClose = key.indexOf(QLatin1Char(']'));
        const int secondOpen = key.indexOf(QLatin1Char('['), firstClose);
        const int secondClose = key.indexOf(QLatin1Char(']'), secondOpen);
        if (firstClose <= 7 || secondOpen < 0 || secondClose < 0){
            continue;
        }

        const QByteArray fieldId = key.mid(7, firstClose - 7).toUtf8();
        const QString operation = key.mid(secondOpen + 1, secondClose - secondOpen - 1);
        expression.fieldFilters << CFilter::FieldFilter(
            fieldId,
            item.second,
            StringToFieldOperation(operation));
    }
    filter.SetFilterExpression(expression);

    if (query.hasQueryItem(QStringLiteral("page"))){
        filter.SetPage(query.queryItemValue(QStringLiteral("page")).toInt());
    }
    if (query.hasQueryItem(QStringLiteral("limit"))){
        filter.SetPageSize(query.queryItemValue(QStringLiteral("limit")).toInt());
    }

    return true;
}


QJsonObject CFilterSerializer::ToJson(const CFilter& filter)
{
    QJsonObject json;

    if (!filter.GetTextFilter().isEmpty()){
        json[QStringLiteral("text")] = filter.GetTextFilter();
    }

    QJsonArray textFields;
    for (const QByteArray& fieldId : filter.GetTextFieldIds()){
        textFields << QString::fromUtf8(fieldId);
    }
    if (!textFields.isEmpty()){
        json[QStringLiteral("textFields")] = textFields;
    }

    QJsonArray sort;
    for (const CFilter::SortField& sortField : filter.GetSortFields()){
        if (sortField.fieldId.isEmpty() || sortField.sortingOrder == CFilter::SO_NO_ORDER){
            continue;
        }
        QJsonObject sortItem;
        sortItem[QStringLiteral("field")] = QString::fromUtf8(sortField.fieldId);
        sortItem[QStringLiteral("order")] = SortingOrderToString(sortField.sortingOrder);
        sort << sortItem;
    }
    if (!sort.isEmpty()){
        json[QStringLiteral("sort")] = sort;
    }

    const QJsonObject expressionJson = ToJson(filter.GetFilterExpression());
    if (!expressionJson.isEmpty()){
        json[QStringLiteral("filter")] = expressionJson;
    }

    if (filter.HasPagination()){
        json[QStringLiteral("page")] = filter.GetPage();
        json[QStringLiteral("limit")] = filter.GetPageSize();
    }

    return json;
}


bool CFilterSerializer::FromJson(const QJsonObject& json, CFilter& filter)
{
    filter.Clear();

    if (json.contains(QStringLiteral("text"))){
        filter.SetTextFilter(json[QStringLiteral("text")].toString());
    }

    QByteArrayList textFieldIds;
    for (const QJsonValue& value : json[QStringLiteral("textFields")].toArray()){
        textFieldIds << value.toString().toUtf8();
    }
    filter.SetTextFieldIds(textFieldIds);

    QVector<CFilter::SortField> sortFields;
    for (const QJsonValue& value : json[QStringLiteral("sort")].toArray()){
        const QJsonObject sortItem = value.toObject();
        sortFields << CFilter::SortField(
            sortItem[QStringLiteral("field")].toString().toUtf8(),
            StringToSortingOrder(sortItem[QStringLiteral("order")].toString()));
    }
    filter.SetSortFields(sortFields);

    if (json.contains(QStringLiteral("filter"))){
        CFilter::FilterExpression expression;
        if (!FromJson(json[QStringLiteral("filter")].toObject(), expression)){
            return false;
        }
        filter.SetFilterExpression(expression);
    }

    if (json.contains(QStringLiteral("page"))){
        filter.SetPage(json[QStringLiteral("page")].toInt());
    }
    if (json.contains(QStringLiteral("limit"))){
        filter.SetPageSize(json[QStringLiteral("limit")].toInt());
    }

    return true;
}


QJsonObject CFilterSerializer::ToJson(const CFilter::FilterExpression& expression)
{
    if (expression.fieldFilters.isEmpty() && expression.filterExpressions.isEmpty()){
        return QJsonObject();
    }

    QJsonObject json;
    json[QStringLiteral("op")] = expression.logicalOperation == CFilter::LO_OR
        ? QStringLiteral("or")
        : QStringLiteral("and");

    QJsonArray fields;
    for (const CFilter::FieldFilter& fieldFilter : expression.fieldFilters){
        QJsonObject item;
        item[QStringLiteral("field")] = QString::fromUtf8(fieldFilter.fieldId);
        item[QStringLiteral("op")] = FieldOperationToString(fieldFilter.operation);
        item[QStringLiteral("value")] = QJsonValue::fromVariant(fieldFilter.value);
        fields << item;
    }
    if (!fields.isEmpty()){
        json[QStringLiteral("fields")] = fields;
    }

    QJsonArray groups;
    for (const CFilter::FilterExpression& group : expression.filterExpressions){
        const QJsonObject groupJson = ToJson(group);
        if (!groupJson.isEmpty()){
            groups << groupJson;
        }
    }
    if (!groups.isEmpty()){
        json[QStringLiteral("groups")] = groups;
    }

    return json;
}


bool CFilterSerializer::FromJson(const QJsonObject& json, CFilter::FilterExpression& expression)
{
    expression.logicalOperation = json[QStringLiteral("op")].toString().toLower() == QLatin1String("or")
        ? CFilter::LO_OR
        : CFilter::LO_AND;

    for (const QJsonValue& value : json[QStringLiteral("fields")].toArray()){
        const QJsonObject item = value.toObject();
        expression.fieldFilters << CFilter::FieldFilter(
            item[QStringLiteral("field")].toString().toUtf8(),
            item[QStringLiteral("value")].toVariant(),
            StringToFieldOperation(item[QStringLiteral("op")].toString()));
    }

    for (const QJsonValue& value : json[QStringLiteral("groups")].toArray()){
        CFilter::FilterExpression childExpression;
        if (!FromJson(value.toObject(), childExpression)){
            return false;
        }
        expression.filterExpressions << childExpression;
    }

    return true;
}


QString CFilterSerializer::FieldOperationToString(CFilter::FilterOperation operation)
{
    switch (operation){
    case CFilter::FO_NOT_EQUAL:   return QStringLiteral("ne");
    case CFilter::FO_LESS:        return QStringLiteral("lt");
    case CFilter::FO_GREATER:     return QStringLiteral("gt");
    case CFilter::FO_NOT_LESS:    return QStringLiteral("gte");
    case CFilter::FO_NOT_GREATER: return QStringLiteral("lte");
    case CFilter::FO_CONTAINS:    return QStringLiteral("contains");
    case CFilter::FO_EQUAL:
    default:                      return QStringLiteral("eq");
    }
}


CFilter::FilterOperation CFilterSerializer::StringToFieldOperation(const QString& value)
{
    const QString normalized = NormalizeToken(value);
    if (normalized == QLatin1String("ne"))       return CFilter::FO_NOT_EQUAL;
    if (normalized == QLatin1String("lt"))       return CFilter::FO_LESS;
    if (normalized == QLatin1String("gt"))       return CFilter::FO_GREATER;
    if (normalized == QLatin1String("gte"))      return CFilter::FO_NOT_LESS;
    if (normalized == QLatin1String("lte"))      return CFilter::FO_NOT_GREATER;
    if (normalized == QLatin1String("contains")) return CFilter::FO_CONTAINS;
    return CFilter::FO_EQUAL;
}


QString CFilterSerializer::SortingOrderToString(CFilter::SortingOrder order)
{
    switch (order){
    case CFilter::SO_DESC: return QStringLiteral("desc");
    case CFilter::SO_ASC:  return QStringLiteral("asc");
    default:               return QString();
    }
}


CFilter::SortingOrder CFilterSerializer::StringToSortingOrder(const QString& value)
{
    const QString normalized = NormalizeToken(value);
    if (normalized == QLatin1String("desc")){
        return CFilter::SO_DESC;
    }
    if (normalized == QLatin1String("asc")){
        return CFilter::SO_ASC;
    }
    return CFilter::SO_NO_ORDER;
}


} // namespace imtbase


