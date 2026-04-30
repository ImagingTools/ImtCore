// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtbase/CFilterSerializer.h>


// Qt includes
#include <QtCore/QJsonArray>
#include <QtCore/QUrl>
#include <QtCore/QUrlQuery>


namespace imtbase
{


// -------------------------------------------------------------------------
// URL query-string serialization
// -------------------------------------------------------------------------

QString CFilterSerializer::ToQueryString(
    const IComplexCollectionFilter& filter,
    int page,
    int pageSize)
{
    QUrlQuery query;

    // Text filter
    const QString textFilter = filter.GetTextFilter();
    if (!textFilter.isEmpty()){
        query.addQueryItem(QStringLiteral("text"), textFilter);
    }

    // Text filter field IDs
    QByteArrayList textFieldIds;
    for (const IComplexCollectionFilter::FieldInfo& info : filter.GetFields()){
        if (info.metaInfo.flags & IComplexCollectionFilter::SO_TEXT_FILTER){
            textFieldIds << info.id;
        }
    }
    if (!textFieldIds.isEmpty()){
        QStringList parts;
        for (const QByteArray& id : textFieldIds){
            parts << QString::fromUtf8(id);
        }
        query.addQueryItem(QStringLiteral("textFields"), parts.join(QLatin1Char(',')));
    }

    // Sort fields
    QStringList sortParts;
    for (const IComplexCollectionFilter::FieldInfo& info : filter.GetFields()){
        if (!(info.metaInfo.flags & IComplexCollectionFilter::SO_SORT)){
            continue;
        }
        if (info.metaInfo.sortingOrder == IComplexCollectionFilter::SO_NO_ORDER){
            continue;
        }
        const QString order = (info.metaInfo.sortingOrder == IComplexCollectionFilter::SO_ASC)
            ? QStringLiteral("asc")
            : QStringLiteral("desc");
        sortParts << QStringLiteral("%1:%2").arg(QString::fromUtf8(info.id), order);
    }
    if (!sortParts.isEmpty()){
        query.addQueryItem(QStringLiteral("sort"), sortParts.join(QLatin1Char(',')));
    }

    // Field filters (flat: filter[field][op]=value)
    const IComplexCollectionFilter::FilterExpression& expr = filter.GetFilterExpression();
    for (const IComplexCollectionFilter::FieldFilter& ff : expr.fieldFilters){
        const QString key = QStringLiteral("filter[%1][%2]")
            .arg(QString::fromUtf8(ff.fieldId), FieldOperationToString(ff.filterOperation));
        query.addQueryItem(key, ff.filterValue.toString());
    }

    // Pagination
    if (page > 0){
        query.addQueryItem(QStringLiteral("page"), QString::number(page));
    }
    if (pageSize > 0){
        query.addQueryItem(QStringLiteral("limit"), QString::number(pageSize));
    }

    return query.toString(QUrl::FullyEncoded);
}


bool CFilterSerializer::FromQueryString(const QString& queryString, CComplexCollectionFilter& filter)
{
    QString qs = queryString;
    if (qs.startsWith(QLatin1Char('?'))){
        qs = qs.mid(1);
    }

    QUrlQuery query;
    query.setQuery(qs);

    // Text filter
    if (query.hasQueryItem(QStringLiteral("text"))){
        filter.SetTextFilter(query.queryItemValue(QStringLiteral("text"), QUrl::FullyDecoded));
    }

    // Text filter fields
    if (query.hasQueryItem(QStringLiteral("textFields"))){
        const QString fieldsStr = query.queryItemValue(QStringLiteral("textFields"), QUrl::FullyDecoded);
        IComplexCollectionFilter::Fields fields = filter.GetFields();

        for (const QString& fieldStr : fieldsStr.split(QLatin1Char(','), Qt::SkipEmptyParts)){
            const QByteArray fieldId = fieldStr.trimmed().toUtf8();
            bool found = false;
            for (IComplexCollectionFilter::FieldInfo& info : fields){
                if (info.id == fieldId){
                    info.metaInfo.flags |= IComplexCollectionFilter::SO_TEXT_FILTER;
                    found = true;
                    break;
                }
            }
            if (!found){
                IComplexCollectionFilter::FieldInfo info(fieldId);
                info.metaInfo.flags = IComplexCollectionFilter::SO_TEXT_FILTER;
                fields << info;
            }
        }
        filter.SetFields(fields);
    }

    // Sort fields
    if (query.hasQueryItem(QStringLiteral("sort"))){
        const QString sortStr = query.queryItemValue(QStringLiteral("sort"), QUrl::FullyDecoded);
        IComplexCollectionFilter::Fields fields = filter.GetFields();

        for (const QString& part : sortStr.split(QLatin1Char(','), Qt::SkipEmptyParts)){
            const QStringList pair = part.split(QLatin1Char(':'));
            if (pair.size() != 2){
                continue;
            }
            const QByteArray fieldId = pair.at(0).trimmed().toUtf8();
            const IComplexCollectionFilter::SortingOrder order =
                (pair.at(1).trimmed().toLower() == QLatin1String("desc"))
                ? IComplexCollectionFilter::SO_DESC
                : IComplexCollectionFilter::SO_ASC;

            bool found = false;
            for (IComplexCollectionFilter::FieldInfo& info : fields){
                if (info.id == fieldId){
                    info.metaInfo.flags |= IComplexCollectionFilter::SO_SORT;
                    info.metaInfo.sortingOrder = order;
                    found = true;
                    break;
                }
            }
            if (!found){
                IComplexCollectionFilter::FieldInfo info(fieldId);
                info.metaInfo.flags = IComplexCollectionFilter::SO_SORT;
                info.metaInfo.sortingOrder = order;
                fields << info;
            }
        }
        filter.SetFields(fields);
    }

    // Field filters
    IComplexCollectionFilter::FilterExpression expr = filter.GetFilterExpression();
    const QList<QPair<QString, QString>> allItems = query.queryItems(QUrl::FullyDecoded);
    for (const QPair<QString, QString>& item : allItems){
        const QString& key = item.first;
        if (!key.startsWith(QStringLiteral("filter["))){
            continue;
        }
        // Parse filter[field][op]
        const int firstBracket = key.indexOf(QLatin1Char('['));
        const int secondBracket = key.indexOf(QLatin1Char(']'), firstBracket);
        const int thirdBracket = key.indexOf(QLatin1Char('['), secondBracket);
        const int fourthBracket = key.indexOf(QLatin1Char(']'), thirdBracket);
        if (firstBracket < 0 || secondBracket < 0 || thirdBracket < 0 || fourthBracket < 0){
            continue;
        }
        const QByteArray fieldId = key.mid(firstBracket + 1, secondBracket - firstBracket - 1).toUtf8();
        const QString opStr = key.mid(thirdBracket + 1, fourthBracket - thirdBracket - 1);
        const IComplexCollectionFilter::FieldOperation op = StringToFieldOperation(opStr);

        IComplexCollectionFilter::FieldFilter ff(fieldId, item.second, op);
        if (!expr.fieldFilters.contains(ff)){
            expr.fieldFilters << ff;
        }
    }
    filter.SetFilterExpression(expr);

    return true;
}


// -------------------------------------------------------------------------
// JSON serialization
// -------------------------------------------------------------------------

QJsonObject CFilterSerializer::ToJson(
    const IComplexCollectionFilter& filter,
    int page,
    int pageSize)
{
    QJsonObject obj;

    // Text filter
    const QString textFilter = filter.GetTextFilter();
    if (!textFilter.isEmpty()){
        obj[QStringLiteral("text")] = textFilter;
    }

    // Fields
    QJsonArray textFieldsArray;
    QJsonArray sortArray;

    for (const IComplexCollectionFilter::FieldInfo& info : filter.GetFields()){
        if (info.metaInfo.flags & IComplexCollectionFilter::SO_TEXT_FILTER){
            textFieldsArray << QString::fromUtf8(info.id);
        }
        if ((info.metaInfo.flags & IComplexCollectionFilter::SO_SORT) &&
            info.metaInfo.sortingOrder != IComplexCollectionFilter::SO_NO_ORDER){
            QJsonObject sortItem;
            sortItem[QStringLiteral("field")] = QString::fromUtf8(info.id);
            sortItem[QStringLiteral("order")] = (info.metaInfo.sortingOrder == IComplexCollectionFilter::SO_ASC)
                ? QStringLiteral("asc")
                : QStringLiteral("desc");
            sortArray << sortItem;
        }
    }

    if (!textFieldsArray.isEmpty()){
        obj[QStringLiteral("textFields")] = textFieldsArray;
    }
    if (!sortArray.isEmpty()){
        obj[QStringLiteral("sort")] = sortArray;
    }

    // Filter expression
    const QString exprStr = SerializeFilterExpression(filter.GetFilterExpression());
    if (!exprStr.isEmpty()){
        QJsonArray fieldFiltersArray;
        for (const IComplexCollectionFilter::FieldFilter& ff : filter.GetFilterExpression().fieldFilters){
            QJsonObject ffObj;
            ffObj[QStringLiteral("field")] = QString::fromUtf8(ff.fieldId);
            ffObj[QStringLiteral("op")] = FieldOperationToString(ff.filterOperation);
            ffObj[QStringLiteral("value")] = QJsonValue::fromVariant(ff.filterValue);
            fieldFiltersArray << ffObj;
        }
        if (!fieldFiltersArray.isEmpty()){
            QJsonObject filterExprObj;
            filterExprObj[QStringLiteral("op")] =
                (filter.GetFilterExpression().logicalOperation == IComplexCollectionFilter::LO_OR)
                ? QStringLiteral("or")
                : QStringLiteral("and");
            filterExprObj[QStringLiteral("fields")] = fieldFiltersArray;
            obj[QStringLiteral("filter")] = filterExprObj;
        }
    }

    // Pagination
    if (page > 0){
        obj[QStringLiteral("page")] = page;
    }
    if (pageSize > 0){
        obj[QStringLiteral("limit")] = pageSize;
    }

    return obj;
}


bool CFilterSerializer::FromJson(const QJsonObject& json, CComplexCollectionFilter& filter)
{
    // Text filter
    if (json.contains(QStringLiteral("text"))){
        filter.SetTextFilter(json[QStringLiteral("text")].toString());
    }

    IComplexCollectionFilter::Fields fields = filter.GetFields();

    // Text fields
    if (json.contains(QStringLiteral("textFields"))){
        const QJsonArray textFields = json[QStringLiteral("textFields")].toArray();
        for (const QJsonValue& val : textFields){
            const QByteArray fieldId = val.toString().toUtf8();
            bool found = false;
            for (IComplexCollectionFilter::FieldInfo& info : fields){
                if (info.id == fieldId){
                    info.metaInfo.flags |= IComplexCollectionFilter::SO_TEXT_FILTER;
                    found = true;
                    break;
                }
            }
            if (!found){
                IComplexCollectionFilter::FieldInfo info(fieldId);
                info.metaInfo.flags = IComplexCollectionFilter::SO_TEXT_FILTER;
                fields << info;
            }
        }
    }

    // Sort fields
    if (json.contains(QStringLiteral("sort"))){
        const QJsonArray sortArray = json[QStringLiteral("sort")].toArray();
        for (const QJsonValue& val : sortArray){
            const QJsonObject sortObj = val.toObject();
            const QByteArray fieldId = sortObj[QStringLiteral("field")].toString().toUtf8();
            const IComplexCollectionFilter::SortingOrder order =
                (sortObj[QStringLiteral("order")].toString().toLower() == QLatin1String("desc"))
                ? IComplexCollectionFilter::SO_DESC
                : IComplexCollectionFilter::SO_ASC;

            bool found = false;
            for (IComplexCollectionFilter::FieldInfo& info : fields){
                if (info.id == fieldId){
                    info.metaInfo.flags |= IComplexCollectionFilter::SO_SORT;
                    info.metaInfo.sortingOrder = order;
                    found = true;
                    break;
                }
            }
            if (!found){
                IComplexCollectionFilter::FieldInfo info(fieldId);
                info.metaInfo.flags = IComplexCollectionFilter::SO_SORT;
                info.metaInfo.sortingOrder = order;
                fields << info;
            }
        }
    }

    filter.SetFields(fields);

    // Filter expression
    if (json.contains(QStringLiteral("filter"))){
        IComplexCollectionFilter::FilterExpression expr;
        if (!DeserializeFilterExpression(json[QStringLiteral("filter")].toObject(), expr)){
            return false;
        }
        filter.SetFilterExpression(expr);
    }

    return true;
}


// -------------------------------------------------------------------------
// private helpers
// -------------------------------------------------------------------------

QString CFilterSerializer::SerializeFilterExpression(
    const IComplexCollectionFilter::FilterExpression& expr)
{
    if (expr.fieldFilters.isEmpty() && expr.filterExpressions.isEmpty()){
        return QString();
    }
    return QStringLiteral("<non-empty>");
}


bool CFilterSerializer::DeserializeFilterExpression(
    const QJsonObject& json,
    IComplexCollectionFilter::FilterExpression& expr)
{
    const QString opStr = json[QStringLiteral("op")].toString().toLower();
    expr.logicalOperation = (opStr == QLatin1String("or"))
        ? IComplexCollectionFilter::LO_OR
        : IComplexCollectionFilter::LO_AND;

    const QJsonArray fields = json[QStringLiteral("fields")].toArray();
    for (const QJsonValue& val : fields){
        const QJsonObject ffObj = val.toObject();
        const QByteArray fieldId = ffObj[QStringLiteral("field")].toString().toUtf8();
        const IComplexCollectionFilter::FieldOperation op =
            StringToFieldOperation(ffObj[QStringLiteral("op")].toString());
        const QVariant value = ffObj[QStringLiteral("value")].toVariant();

        expr.fieldFilters << IComplexCollectionFilter::FieldFilter(fieldId, value, op);
    }

    // Nested expressions
    if (json.contains(QStringLiteral("groups"))){
        const QJsonArray groups = json[QStringLiteral("groups")].toArray();
        for (const QJsonValue& val : groups){
            IComplexCollectionFilter::FilterExpression subExpr;
            if (!DeserializeFilterExpression(val.toObject(), subExpr)){
                return false;
            }
            expr.filterExpressions << subExpr;
        }
    }

    return true;
}


QString CFilterSerializer::FieldOperationToString(IComplexCollectionFilter::FieldOperation op)
{
    switch (op){
    case IComplexCollectionFilter::FO_EQUAL:        return QStringLiteral("eq");
    case IComplexCollectionFilter::FO_NOT_EQUAL:    return QStringLiteral("ne");
    case IComplexCollectionFilter::FO_LESS:         return QStringLiteral("lt");
    case IComplexCollectionFilter::FO_GREATER:      return QStringLiteral("gt");
    case IComplexCollectionFilter::FO_NOT_LESS:     return QStringLiteral("gte");
    case IComplexCollectionFilter::FO_NOT_GREATER:  return QStringLiteral("lte");
    case IComplexCollectionFilter::FO_CONTAINS:     return QStringLiteral("contains");
    default:                                         return QStringLiteral("eq");
    }
}


IComplexCollectionFilter::FieldOperation CFilterSerializer::StringToFieldOperation(const QString& str)
{
    if (str == QLatin1String("ne"))       return IComplexCollectionFilter::FO_NOT_EQUAL;
    if (str == QLatin1String("lt"))       return IComplexCollectionFilter::FO_LESS;
    if (str == QLatin1String("gt"))       return IComplexCollectionFilter::FO_GREATER;
    if (str == QLatin1String("gte"))      return IComplexCollectionFilter::FO_NOT_LESS;
    if (str == QLatin1String("lte"))      return IComplexCollectionFilter::FO_NOT_GREATER;
    if (str == QLatin1String("contains")) return IComplexCollectionFilter::FO_CONTAINS;
    return IComplexCollectionFilter::FO_EQUAL;
}


} // namespace imtbase


