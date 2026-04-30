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
 * @brief Serializes standalone CFilter requests to URL query strings and JSON.
 */
class CFilterSerializer
{
public:
    static QString ToQueryString(const CFilter& filter);
    static bool FromQueryString(const QString& queryString, CFilter& filter);

    static QJsonObject ToJson(const CFilter& filter);
    static bool FromJson(const QJsonObject& json, CFilter& filter);

private:
    static QJsonObject ToJson(const CFilter::RuleSet& rules);
    static bool FromJson(const QJsonObject& json, CFilter::RuleSet& rules);
    static QString JoinToString(CFilter::RuleSet::Join join);
    static CFilter::RuleSet::Join StringToJoin(const QString& value);
};


} // namespace imtbase


