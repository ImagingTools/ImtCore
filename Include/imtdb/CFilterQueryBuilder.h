// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// Qt includes
#include <QtCore/QString>
#include <QtCore/QVariantList>

// ImtCore includes
#include <imtbase/CFilter.h>


namespace imtdb
{


/**
 * @brief SQL query builder for standalone imtbase::CFilter requests.
 */
class CFilterQueryBuilder
{
public:
    enum DatabaseDialect
    {
        DD_POSTGRESQL,
        DD_SQLITE
    };

    struct QueryResult
    {
        QString whereClause;
        QString orderByClause;
        QString limitClause;
        QVariantList bindValues;
    };

    explicit CFilterQueryBuilder(DatabaseDialect dialect = DD_POSTGRESQL);

    QueryResult Build(const imtbase::CFilter& filter) const;

    QString BuildSearchClause(
        const imtbase::CFilter::Search& search,
        QVariantList& bindValues) const;

    QString BuildRulesClause(
        const imtbase::CFilter::RuleSet& rules,
        QVariantList& bindValues) const;

    QString BuildOrderClause(const QVector<imtbase::CFilter::Order>& orders) const;
    QString BuildWindowClause(const imtbase::CFilter::Window& window) const;

    void SetJsonColumnName(const QString& columnName);
    void SetUseJsonExtract(bool useJsonExtract);

private:
    QString BuildRuleClause(const imtbase::CFilter::Rule& rule, QVariantList& bindValues) const;
    QString MakeLikeCondition(const QString& expression, const QString& placeholder) const;
    QString MakeJsonFieldAccess(const QString& path) const;
    QString MakePlaceholder(int bindIndex) const;
    QString AddBindValue(const QVariant& value, QVariantList& bindValues) const;
    QString AddLikeBindValue(const QString& value, QVariantList& bindValues) const;
    QString MakeFieldAccess(const QByteArray& path) const;
    QString EscapeLikePattern(const QString& text) const;
    static QString QuoteIdentifier(const QString& identifier);
    static QString EscapeJsonKey(const QString& key);

private:
    DatabaseDialect m_dialect;
    QString m_jsonColumnName;
    bool m_useJsonExtract = false;
};


} // namespace imtdb


