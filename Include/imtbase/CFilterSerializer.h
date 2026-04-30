// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// Qt includes
#include <QtCore/QJsonObject>
#include <QtCore/QString>

// ImtCore includes
#include <imtbase/CFilter.h>


namespace imtbase
{


/**
 * @brief Serializes standalone CFilter objects to URL query strings and JSON.
 */
class CFilterSerializer
{
public:
    static QString ToQueryString(const CFilter& filter);
    static bool FromQueryString(const QString& queryString, CFilter& filter);

    static QJsonObject ToJson(const CFilter& filter);
    static bool FromJson(const QJsonObject& json, CFilter& filter);

private:
    static QJsonObject ToJson(const CFilter::FilterExpression& expression);
    static bool FromJson(const QJsonObject& json, CFilter::FilterExpression& expression);
    static QString FieldOperationToString(CFilter::FilterOperation operation);
    static CFilter::FilterOperation StringToFieldOperation(const QString& value);
    static QString SortingOrderToString(CFilter::SortingOrder order);
    static CFilter::SortingOrder StringToSortingOrder(const QString& value);
};


} // namespace imtbase


